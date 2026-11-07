// SPDX-License-Identifier: GPL-2.0
#include <linux/kernel.h>
#include <linux/hrtimer.h>
#include <linux/ktime.h>
#include <linux/pm.h>
#include <linux/suspend.h>
#include <linux/spinlock.h>
#include <linux/string.h>

#include "ged_hybrid_adaptive.h"

struct ged_ha_trace_record {
	unsigned int mode;
	unsigned int state;
	unsigned int reason;
	unsigned long long generation;
	unsigned long long expiry_ns;
};

struct ged_hybrid_adaptive_runtime {
	struct hrtimer expiry_timer;
	struct ged_ha_core core;
	struct ged_hybrid_adaptive_sample last_sample;
	bool initialized;
	bool pm_registered;
	bool timer_initialized;
	bool last_sample_valid;
};

static struct ged_hybrid_adaptive_runtime ged_ha_runtime;
static DEFINE_SPINLOCK(ged_ha_lock);
static struct notifier_block ged_ha_pm_notifier;

static const char *ged_ha_mode_name(unsigned int mode)
{
	switch (mode) {
	case GED_HA_MODE_OBSERVE:
		return "observe";
	case GED_HA_MODE_AUTO:
		return "auto";
	default:
		return "off";
	}
}

static const char *ged_ha_state_name(unsigned int state)
{
	return state == GED_HA_STATE_RENDER ? "render" : "baseline";
}

static const char *ged_ha_reason_name(unsigned int reason)
{
	switch (reason) {
	case GED_HA_REASON_GPU_DEADLINE:
		return "gpu_deadline";
	case GED_HA_REASON_LIGHT_EXIT:
		return "light_exit";
	case GED_HA_REASON_EXPIRY:
		return "expiry";
	case GED_HA_REASON_STREAM_CHANGE:
		return "stream_change";
	case GED_HA_REASON_AMBIGUOUS:
		return "ambiguous";
	case GED_HA_REASON_UNKNOWN_TARGET:
		return "unknown_target";
	case GED_HA_REASON_BLANK:
		return "blank";
	case GED_HA_REASON_UNBLANK:
		return "unblank";
	case GED_HA_REASON_DISPLAY_UNKNOWN:
		return "display_unknown";
	case GED_HA_REASON_SUSPEND:
		return "suspend";
	case GED_HA_REASON_RESUME:
		return "resume";
	case GED_HA_REASON_MODE_OFF:
		return "mode_off";
	case GED_HA_REASON_MODE_OBSERVE:
		return "mode_observe";
	case GED_HA_REASON_MODE_AUTO:
		return "mode_auto";
	case GED_HA_REASON_TEARDOWN:
		return "teardown";
	default:
		return "none";
	}
}

static void ged_ha_copy_trace_locked(struct ged_ha_trace_record *record)
{
	record->mode = ged_ha_runtime.core.mode;
	record->state = ged_ha_runtime.core.state;
	record->reason = ged_ha_runtime.core.reason;
	record->generation = ged_ha_runtime.core.generation;
	record->expiry_ns = ged_ha_runtime.core.expiry_ns;
}

static void ged_ha_emit_trace(const struct ged_ha_trace_record *record)
{
	trace_printk("ged_hybrid_adaptive mode=%s state=%s reason=%s "
		"generation=%llu expiry_ns=%llu\n",
		ged_ha_mode_name(record->mode),
		ged_ha_state_name(record->state),
		ged_ha_reason_name(record->reason),
		record->generation, record->expiry_ns);
}

static void ged_ha_update_timer_locked(void)
{
	struct ged_ha_core *core = &ged_ha_runtime.core;

	if (core->state != GED_HA_STATE_RENDER || !core->expiry_ns) {
		hrtimer_try_to_cancel(&ged_ha_runtime.expiry_timer);
		return;
	}

	hrtimer_start(&ged_ha_runtime.expiry_timer,
		ns_to_ktime(core->expiry_ns), HRTIMER_MODE_ABS);
}

static enum hrtimer_restart ged_ha_expiry_timer(struct hrtimer *timer)
{
	struct ged_ha_trace_record record;
	struct ged_ha_result result;
	unsigned long flags;
	bool emit = false;
	enum hrtimer_restart restart = HRTIMER_NORESTART;
	unsigned long long now_ns;

	spin_lock_irqsave(&ged_ha_lock, flags);
	if (ged_ha_runtime.initialized &&
		!ged_ha_runtime.core.shutting_down) {
		now_ns = ktime_get_ns();
		ged_ha_core_expire(&ged_ha_runtime.core, now_ns,
			ged_ha_runtime.core.generation, &result);
		if (result.changed) {
			ged_ha_copy_trace_locked(&record);
			emit = true;
		} else if (ged_ha_runtime.core.state == GED_HA_STATE_RENDER &&
			ged_ha_runtime.core.expiry_ns > now_ns) {
			hrtimer_set_expires(timer,
				ns_to_ktime(ged_ha_runtime.core.expiry_ns));
			restart = HRTIMER_RESTART;
		}
	}
	spin_unlock_irqrestore(&ged_ha_lock, flags);

	if (emit)
		ged_ha_emit_trace(&record);
	return restart;
}

static int ged_ha_pm_event(struct notifier_block *nb,
		unsigned long event, void *unused)
{
	(void)nb;
	(void)unused;
	switch (event) {
	case PM_SUSPEND_PREPARE:
		ged_hybrid_adaptive_pm_event(true);
		break;
	case PM_POST_SUSPEND:
		ged_hybrid_adaptive_pm_event(false);
		break;
	default:
		break;
	}
	return NOTIFY_OK;
}

int ged_hybrid_adaptive_init(void)
{
	int ret;
	unsigned long flags;

	if (ged_ha_runtime.timer_initialized)
		return -EBUSY;

	hrtimer_init(&ged_ha_runtime.expiry_timer, CLOCK_MONOTONIC,
		HRTIMER_MODE_ABS);
	ged_ha_runtime.expiry_timer.function = ged_ha_expiry_timer;
	spin_lock_irqsave(&ged_ha_lock, flags);
	ged_ha_core_init(&ged_ha_runtime.core);
	memset(&ged_ha_runtime.last_sample, 0,
		sizeof(ged_ha_runtime.last_sample));
	ged_ha_runtime.last_sample_valid = false;
	ged_ha_pm_notifier.notifier_call = ged_ha_pm_event;
	ged_ha_runtime.initialized = true;
	ged_ha_runtime.pm_registered = false;
	ged_ha_runtime.timer_initialized = true;
	spin_unlock_irqrestore(&ged_ha_lock, flags);

	ret = register_pm_notifier(&ged_ha_pm_notifier);
	if (ret) {
		spin_lock_irqsave(&ged_ha_lock, flags);
		ged_ha_runtime.core.shutting_down = 1;
		ged_ha_runtime.initialized = false;
		spin_unlock_irqrestore(&ged_ha_lock, flags);
		return ret;
	}
	ged_ha_runtime.pm_registered = true;
	return 0;
}

u64 ged_hybrid_adaptive_generation(void)
{
	unsigned long flags;
	u64 generation;

	spin_lock_irqsave(&ged_ha_lock, flags);
	generation = ged_ha_runtime.initialized &&
		!ged_ha_runtime.core.shutting_down &&
		ged_ha_runtime.core.mode != GED_HA_MODE_OFF &&
		ged_ha_runtime.core.display_known &&
		ged_ha_runtime.core.display_on &&
		!ged_ha_runtime.core.suspended ?
		ged_ha_runtime.core.generation : 0;
	spin_unlock_irqrestore(&ged_ha_lock, flags);
	return generation;
}

int ged_hybrid_adaptive_set_mode(unsigned int mode)
{
	struct ged_ha_trace_record record;
	struct ged_ha_result result;
	unsigned long flags;
	u64 old_generation;
	bool emit = false;
	int ret = 0;

	spin_lock_irqsave(&ged_ha_lock, flags);
	if (!ged_ha_runtime.initialized ||
		ged_ha_runtime.core.shutting_down) {
		ret = -ESHUTDOWN;
	} else if (mode > GED_HA_MODE_AUTO) {
		ret = -EINVAL;
	} else {
		old_generation = ged_ha_runtime.core.generation;
		ged_ha_core_set_mode(&ged_ha_runtime.core, mode, &result);
		if (old_generation != ged_ha_runtime.core.generation) {
			ged_ha_runtime.last_sample_valid = false;
			memset(&ged_ha_runtime.last_sample, 0,
				sizeof(ged_ha_runtime.last_sample));
		}
		if (result.changed) {
			ged_ha_update_timer_locked();
			ged_ha_copy_trace_locked(&record);
			emit = true;
		}
	}
	spin_unlock_irqrestore(&ged_ha_lock, flags);

	if (emit)
		ged_ha_emit_trace(&record);
	return ret;
}

void ged_hybrid_adaptive_get_status(
	struct ged_hybrid_adaptive_status *status)
{
	unsigned long flags;
	u64 now_ns = ktime_get_ns();

	if (!status)
		return;
	spin_lock_irqsave(&ged_ha_lock, flags);
	status->generation = ged_ha_runtime.core.generation;
	status->last_observation_ns =
		ged_ha_runtime.core.last_observation_ns;
	status->last_evidence_ns = ged_ha_runtime.core.last_evidence_ns;
	status->expiry_ns = ged_ha_runtime.core.expiry_ns;
	status->mode = ged_ha_runtime.core.mode;
	status->state = ged_ha_runtime.core.state;
	status->reason = ged_ha_runtime.core.reason;
	status->sample_valid = ged_ha_runtime.last_sample_valid &&
		ged_ha_runtime.core.generation ==
		ged_ha_runtime.last_sample.generation &&
		now_ns >= ged_ha_runtime.core.last_observation_ns &&
		now_ns - ged_ha_runtime.core.last_observation_ns <=
			GED_HA_LEASE_NS;
	status->sample_generation =
		ged_ha_runtime.last_sample.generation;
	status->sample_timestamp_ns =
		ged_ha_runtime.last_sample.timestamp_ns;
	status->sample_queue_timestamp_ns =
		ged_ha_runtime.last_sample.queue_timestamp_ns;
	status->sample_fence_timestamp_ns =
		ged_ha_runtime.last_sample.fence_timestamp_ns;
	status->sample_window_id = ged_ha_runtime.last_sample.window_id;
	status->sample_cpu_remained_ns =
		ged_ha_runtime.last_sample.cpu_remained_ns;
	status->sample_gpu_remained_ns =
		ged_ha_runtime.last_sample.gpu_remained_ns;
	status->sample_pid = ged_ha_runtime.last_sample.pid;
	status->sample_frame_id = ged_ha_runtime.last_sample.frame_id;
	status->sample_target_fps = ged_ha_runtime.last_sample.target_fps;
	status->sample_gpu_loading = ged_ha_runtime.last_sample.gpu_loading;
	status->sample_target_fps_valid =
		ged_ha_runtime.last_sample.target_fps_valid;
	status->display_known = ged_ha_runtime.core.display_known;
	status->display_on = ged_ha_runtime.core.display_on;
	status->suspended = ged_ha_runtime.core.suspended;
	status->shutting_down = ged_ha_runtime.core.shutting_down;
	spin_unlock_irqrestore(&ged_ha_lock, flags);
}

void ged_hybrid_adaptive_display_event(bool known, bool on)
{
	struct ged_ha_trace_record record;
	struct ged_ha_result result;
	unsigned int reason;
	unsigned long flags;
	u64 old_generation;
	bool emit = false;

	reason = !known ? GED_HA_REASON_DISPLAY_UNKNOWN :
		(on ? GED_HA_REASON_UNBLANK : GED_HA_REASON_BLANK);
	spin_lock_irqsave(&ged_ha_lock, flags);
	if (ged_ha_runtime.initialized &&
		!ged_ha_runtime.core.shutting_down) {
		old_generation = ged_ha_runtime.core.generation;
		ged_ha_core_set_display(&ged_ha_runtime.core, known, on,
			reason, &result);
		if (old_generation != ged_ha_runtime.core.generation) {
			ged_ha_runtime.last_sample_valid = false;
			memset(&ged_ha_runtime.last_sample, 0,
				sizeof(ged_ha_runtime.last_sample));
		}
		ged_ha_update_timer_locked();
		ged_ha_copy_trace_locked(&record);
		emit = result.changed;
	}
	spin_unlock_irqrestore(&ged_ha_lock, flags);

	if (emit)
		ged_ha_emit_trace(&record);
}

void ged_hybrid_adaptive_pm_event(bool suspended)
{
	struct ged_ha_trace_record record;
	struct ged_ha_result result;
	unsigned long flags;
	u64 old_generation;
	bool emit = false;

	spin_lock_irqsave(&ged_ha_lock, flags);
	if (ged_ha_runtime.initialized &&
		!ged_ha_runtime.core.shutting_down) {
		old_generation = ged_ha_runtime.core.generation;
		ged_ha_core_set_suspended(&ged_ha_runtime.core, suspended,
			&result);
		if (old_generation != ged_ha_runtime.core.generation) {
			ged_ha_runtime.last_sample_valid = false;
			memset(&ged_ha_runtime.last_sample, 0,
				sizeof(ged_ha_runtime.last_sample));
		}
		ged_ha_update_timer_locked();
		ged_ha_copy_trace_locked(&record);
		emit = result.changed;
	}
	spin_unlock_irqrestore(&ged_ha_lock, flags);

	if (emit)
		ged_ha_emit_trace(&record);
}

void ged_hybrid_adaptive_sample(
	const struct ged_hybrid_adaptive_sample *sample)
{
	struct ged_ha_trace_record record;
	struct ged_ha_result result;
	struct ged_ha_sample core_sample;
	unsigned long flags;
	bool emit = false;

	if (!sample)
		return;

	core_sample.generation = sample->generation;
	core_sample.timestamp_ns = sample->timestamp_ns;
	core_sample.queue_timestamp_ns = sample->queue_timestamp_ns;
	core_sample.fence_timestamp_ns = sample->fence_timestamp_ns;
	core_sample.window_id = sample->window_id;
	core_sample.cpu_remained_ns = sample->cpu_remained_ns;
	core_sample.gpu_remained_ns = sample->gpu_remained_ns;
	core_sample.pid = sample->pid;
	core_sample.frame_id = sample->frame_id;
	core_sample.target_fps = sample->target_fps;
	core_sample.gpu_loading = sample->gpu_loading;
	core_sample.target_fps_valid = sample->target_fps_valid;
	core_sample.fence_valid = sample->fence_valid;
	core_sample.gpu_loading_valid = sample->gpu_loading_valid;
	core_sample.is_surfaceflinger = sample->is_surfaceflinger;
	core_sample.latest_frame = sample->latest_frame;

	spin_lock_irqsave(&ged_ha_lock, flags);
	if (ged_ha_runtime.initialized &&
		!ged_ha_runtime.core.shutting_down) {
		ged_ha_core_sample(&ged_ha_runtime.core, &core_sample, &result);
		if (result.observed) {
			ged_ha_runtime.last_sample = *sample;
			ged_ha_runtime.last_sample_valid = true;
		}
		ged_ha_update_timer_locked();
		if (result.changed) {
			ged_ha_copy_trace_locked(&record);
			emit = true;
		}
	}
	spin_unlock_irqrestore(&ged_ha_lock, flags);

	if (emit)
		ged_ha_emit_trace(&record);
}

void ged_hybrid_adaptive_quiesce(void)
{
	struct ged_ha_trace_record record;
	struct ged_ha_result result;
	unsigned long flags;
	bool emit = false;

	spin_lock_irqsave(&ged_ha_lock, flags);
	if (ged_ha_runtime.initialized &&
		!ged_ha_runtime.core.shutting_down) {
		ged_ha_core_shutdown(&ged_ha_runtime.core, &result);
		ged_ha_runtime.last_sample_valid = false;
		memset(&ged_ha_runtime.last_sample, 0,
			sizeof(ged_ha_runtime.last_sample));
		ged_ha_update_timer_locked();
		ged_ha_copy_trace_locked(&record);
		emit = result.changed;
	}
	spin_unlock_irqrestore(&ged_ha_lock, flags);

	if (emit)
		ged_ha_emit_trace(&record);
}

void ged_hybrid_adaptive_exit(void)
{
	unsigned long flags;

	if (!ged_ha_runtime.timer_initialized)
		return;
	ged_hybrid_adaptive_quiesce();
	if (ged_ha_runtime.pm_registered) {
		unregister_pm_notifier(&ged_ha_pm_notifier);
		ged_ha_runtime.pm_registered = false;
	}
	hrtimer_cancel(&ged_ha_runtime.expiry_timer);
	spin_lock_irqsave(&ged_ha_lock, flags);
	ged_ha_runtime.initialized = false;
	ged_ha_runtime.timer_initialized = false;
	spin_unlock_irqrestore(&ged_ha_lock, flags);
}

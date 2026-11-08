/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __GED_HYBRID_ADAPTIVE_CORE_H__
#define __GED_HYBRID_ADAPTIVE_CORE_H__

/* Keep this transition core independent of kernel APIs for replay testing. */
#define GED_HA_EVAL_INTERVAL_NS	250000000ULL
#define GED_HA_LEASE_NS		1000000000ULL
#define GED_HA_MAX_TARGET_FPS	240
#define GED_HA_MAX_REMAINED_NS	500000000LL

enum ged_ha_mode {
	GED_HA_MODE_OFF,
	GED_HA_MODE_OBSERVE,
	GED_HA_MODE_AUTO,
};

enum ged_ha_state {
	GED_HA_STATE_BASELINE,
	GED_HA_STATE_RENDER,
};

enum ged_ha_reason {
	GED_HA_REASON_NONE,
	GED_HA_REASON_GPU_DEADLINE,
	GED_HA_REASON_LIGHT_EXIT,
	GED_HA_REASON_EXPIRY,
	GED_HA_REASON_STREAM_CHANGE,
	GED_HA_REASON_AMBIGUOUS,
	GED_HA_REASON_UNKNOWN_TARGET,
	GED_HA_REASON_BLANK,
	GED_HA_REASON_UNBLANK,
	GED_HA_REASON_DISPLAY_UNKNOWN,
	GED_HA_REASON_SUSPEND,
	GED_HA_REASON_RESUME,
	GED_HA_REASON_MODE_OFF,
	GED_HA_REASON_MODE_OBSERVE,
	GED_HA_REASON_MODE_AUTO,
	GED_HA_REASON_TEARDOWN,
};

struct ged_ha_core {
	unsigned int mode;
	unsigned int state;
	unsigned int reason;
	unsigned int heavy_windows;
	unsigned int light_windows;
	unsigned long long generation;
	unsigned long long last_eval_ns;
	unsigned long long last_queue_ns;
	unsigned long long last_observation_ns;
	unsigned long long last_evidence_ns;
	unsigned long long expiry_ns;
	unsigned long long stream_window_id;
	unsigned int stream_pid;
	unsigned int stream_target_fps;
	int stream_valid;
	int stream_target_valid;
	int display_known;
	int display_on;
	int suspended;
	int shutting_down;
};

struct ged_ha_sample {
	unsigned long long generation;
	unsigned long long timestamp_ns;
	unsigned long long queue_timestamp_ns;
	unsigned long long fence_timestamp_ns;
	unsigned long long window_id;
	unsigned int pid;
	unsigned int frame_id;
	unsigned int target_fps;
	unsigned int gpu_loading;
	long long cpu_remained_ns;
	long long gpu_remained_ns;
	int target_fps_valid;
	int fence_valid;
	int gpu_loading_valid;
	int is_surfaceflinger;
	int latest_frame;
};

struct ged_ha_result {
	int changed;
	int observed;
	int accepted;
	unsigned int reason;
	unsigned long long expiry_ns;
};

static inline void ged_ha_result_reset(struct ged_ha_result *result)
{
	if (result) {
		result->changed = 0;
		result->observed = 0;
		result->accepted = 0;
		result->reason = GED_HA_REASON_NONE;
		result->expiry_ns = 0;
	}
}

static inline void ged_ha_core_clear(struct ged_ha_core *core,
		unsigned int reason, struct ged_ha_result *result)
{
	unsigned int old_state = core->state;
	unsigned long long old_expiry = core->expiry_ns;

	core->state = GED_HA_STATE_BASELINE;
	core->heavy_windows = 0;
	core->light_windows = 0;
	core->last_eval_ns = 0;
	core->last_queue_ns = 0;
	core->last_observation_ns = 0;
	core->last_evidence_ns = 0;
	core->expiry_ns = 0;
	core->stream_window_id = 0;
	core->stream_pid = 0;
	core->stream_target_fps = 0;
	core->stream_valid = 0;
	core->stream_target_valid = 0;
	core->generation++;
	if (!core->generation)
		core->generation = 1;
	core->reason = reason;

	if (result) {
		result->changed = (old_state != core->state || old_expiry != 0 ||
			reason != GED_HA_REASON_NONE);
		result->reason = reason;
		result->expiry_ns = 0;
	}
}

static inline void ged_ha_core_init(struct ged_ha_core *core)
{
	core->mode = GED_HA_MODE_OFF;
	core->state = GED_HA_STATE_BASELINE;
	core->reason = GED_HA_REASON_NONE;
	core->heavy_windows = 0;
	core->light_windows = 0;
	core->generation = 1;
	core->last_eval_ns = 0;
	core->last_queue_ns = 0;
	core->last_observation_ns = 0;
	core->last_evidence_ns = 0;
	core->expiry_ns = 0;
	core->stream_window_id = 0;
	core->stream_pid = 0;
	core->stream_target_fps = 0;
	core->stream_valid = 0;
	core->stream_target_valid = 0;
	core->display_known = 0;
	core->display_on = 0;
	core->suspended = 0;
	core->shutting_down = 0;
}

static inline int ged_ha_core_set_mode(struct ged_ha_core *core,
		unsigned int mode, struct ged_ha_result *result)
{
	unsigned int reason;

	/* AUTO stays observer-only until a later change wires an actuator. */
	ged_ha_result_reset(result);
	if (mode > GED_HA_MODE_AUTO || core->shutting_down)
		return 0;
	if (core->mode == mode)
		return 0;

	core->mode = mode;
	reason = mode == GED_HA_MODE_OFF ? GED_HA_REASON_MODE_OFF :
		(mode == GED_HA_MODE_AUTO ? GED_HA_REASON_MODE_AUTO :
		GED_HA_REASON_MODE_OBSERVE);
	ged_ha_core_clear(core, reason, result);
	return 1;
}

static inline void ged_ha_core_set_display(struct ged_ha_core *core,
		int known, int on, unsigned int reason,
		struct ged_ha_result *result)
{
	ged_ha_result_reset(result);
	if (core->shutting_down)
		return;
	core->display_known = !!known;
	core->display_on = !!known && !!on;
	ged_ha_core_clear(core, reason, result);
}

static inline void ged_ha_core_set_suspended(struct ged_ha_core *core,
		int suspended, struct ged_ha_result *result)
{
	ged_ha_result_reset(result);
	if (core->shutting_down)
		return;
	core->suspended = !!suspended;
	core->display_known = 0;
	core->display_on = 0;
	ged_ha_core_clear(core, suspended ? GED_HA_REASON_SUSPEND :
		GED_HA_REASON_RESUME, result);
}

static inline void ged_ha_core_shutdown(struct ged_ha_core *core,
		struct ged_ha_result *result)
{
	ged_ha_result_reset(result);
	core->shutting_down = 1;
	core->mode = GED_HA_MODE_OFF;
	core->display_known = 0;
	core->display_on = 0;
	core->suspended = 1;
	ged_ha_core_clear(core, GED_HA_REASON_TEARDOWN, result);
}

static inline int ged_ha_core_expire(struct ged_ha_core *core,
		unsigned long long now_ns, unsigned long long generation,
		struct ged_ha_result *result)
{
	ged_ha_result_reset(result);
	if (generation != core->generation ||
		core->state != GED_HA_STATE_RENDER || !core->expiry_ns ||
		now_ns < core->expiry_ns)
		return 0;

	core->state = GED_HA_STATE_BASELINE;
	core->heavy_windows = 0;
	core->light_windows = 0;
	core->expiry_ns = 0;
	core->reason = GED_HA_REASON_EXPIRY;
	if (result) {
		result->changed = 1;
		result->reason = GED_HA_REASON_EXPIRY;
	}
	return 1;
}

static inline int ged_ha_core_sample(struct ged_ha_core *core,
		const struct ged_ha_sample *sample,
		struct ged_ha_result *result)
{
	int stream_changed;
	int gpu_deadline;
	int light;
	int target_valid;

	ged_ha_result_reset(result);
	if (!sample || core->shutting_down ||
		core->mode == GED_HA_MODE_OFF || !core->display_known ||
		!core->display_on || core->suspended ||
		sample->generation != core->generation ||
		sample->is_surfaceflinger || !sample->fence_valid ||
		!sample->gpu_loading_valid ||
		sample->gpu_loading > 100 || !sample->queue_timestamp_ns ||
		!sample->latest_frame ||
		sample->timestamp_ns < sample->queue_timestamp_ns ||
		sample->timestamp_ns - sample->queue_timestamp_ns >=
			GED_HA_LEASE_NS ||
		sample->fence_timestamp_ns < sample->queue_timestamp_ns ||
		sample->timestamp_ns < sample->fence_timestamp_ns ||
		sample->timestamp_ns - sample->fence_timestamp_ns >=
			GED_HA_LEASE_NS ||
		sample->cpu_remained_ns < -GED_HA_MAX_REMAINED_NS ||
		sample->gpu_remained_ns < -GED_HA_MAX_REMAINED_NS)
		return 0;

	if (sample->queue_timestamp_ns <= core->last_queue_ns ||
		(sample->timestamp_ns < core->last_eval_ns))
		return 0;

	if (core->state == GED_HA_STATE_RENDER && core->expiry_ns &&
		sample->timestamp_ns >= core->expiry_ns)
		ged_ha_core_expire(core, sample->timestamp_ns,
			core->generation, result);
	else if (core->last_eval_ns &&
		sample->timestamp_ns - core->last_eval_ns > GED_HA_LEASE_NS) {
		core->heavy_windows = 0;
		core->light_windows = 0;
		core->last_evidence_ns = 0;
	}

	stream_changed = core->stream_valid &&
		(core->stream_pid != sample->pid ||
		core->stream_window_id != sample->window_id ||
		core->stream_target_valid != !!sample->target_fps_valid ||
		(sample->target_fps_valid &&
		core->stream_target_fps != sample->target_fps));
	if (stream_changed) {
		unsigned int old_state = core->state;
		unsigned long long old_expiry = core->expiry_ns;

		core->state = GED_HA_STATE_BASELINE;
		core->heavy_windows = 0;
		core->light_windows = 0;
		core->last_eval_ns = 0;
		core->last_evidence_ns = 0;
		core->expiry_ns = 0;
		core->reason = GED_HA_REASON_STREAM_CHANGE;
		if (result) {
			result->changed |= old_state != core->state || old_expiry != 0;
			result->reason = GED_HA_REASON_STREAM_CHANGE;
		}
	}

	core->stream_valid = 1;
	core->stream_pid = sample->pid;
	core->stream_window_id = sample->window_id;
	core->stream_target_valid = !!sample->target_fps_valid;
	core->stream_target_fps = sample->target_fps_valid ?
		sample->target_fps : 0;
	core->last_queue_ns = sample->queue_timestamp_ns;

	if (core->last_eval_ns &&
		sample->timestamp_ns - core->last_eval_ns <
			GED_HA_EVAL_INTERVAL_NS)
		return 0;

	core->last_eval_ns = sample->timestamp_ns;
	core->last_observation_ns = sample->timestamp_ns;
	if (result)
		result->observed = 1;

	target_valid = sample->target_fps_valid && sample->target_fps &&
		sample->target_fps <= GED_HA_MAX_TARGET_FPS;
	if (!target_valid) {
		unsigned int old_state = core->state;
		unsigned long long old_expiry = core->expiry_ns;

		core->state = GED_HA_STATE_BASELINE;
		core->heavy_windows = 0;
		core->light_windows = 0;
		core->last_evidence_ns = 0;
		core->expiry_ns = 0;
		core->reason = GED_HA_REASON_UNKNOWN_TARGET;
		if (result) {
			result->changed |= old_state != core->state || old_expiry != 0;
			result->reason = GED_HA_REASON_UNKNOWN_TARGET;
		}
		return 0;
	}

	/* A CPU-only miss remains baseline without independent CPU pressure. */
	if (sample->gpu_loading && sample->gpu_remained_ns < 0 &&
		sample->cpu_remained_ns >= 0)
		gpu_deadline = 1;
	else
		gpu_deadline = 0;
	light = sample->gpu_remained_ns >= 0 &&
		sample->cpu_remained_ns >= 0;

	if (!gpu_deadline && !light) {
		unsigned int old_state = core->state;
		unsigned long long old_expiry = core->expiry_ns;

		core->state = GED_HA_STATE_BASELINE;
		core->heavy_windows = 0;
		core->light_windows = 0;
		core->last_evidence_ns = 0;
		core->expiry_ns = 0;
		core->reason = GED_HA_REASON_AMBIGUOUS;
		if (result) {
			result->changed |= old_state != core->state || old_expiry != 0;
			result->reason = GED_HA_REASON_AMBIGUOUS;
		}
		return 0;
	}

	core->last_evidence_ns = sample->fence_timestamp_ns;
	if (result)
		result->accepted = 1;
	if (gpu_deadline) {
		core->heavy_windows++;
		core->light_windows = 0;
		if (core->state == GED_HA_STATE_BASELINE &&
			core->heavy_windows >= 2) {
			core->state = GED_HA_STATE_RENDER;
			core->reason = GED_HA_REASON_GPU_DEADLINE;
			if (result) {
				result->changed = 1;
				result->reason = GED_HA_REASON_GPU_DEADLINE;
			}
		}
	} else {
		core->light_windows++;
		core->heavy_windows = 0;
		if (core->state == GED_HA_STATE_RENDER &&
			core->light_windows >= 4) {
			core->state = GED_HA_STATE_BASELINE;
			core->light_windows = 0;
			core->expiry_ns = 0;
			core->reason = GED_HA_REASON_LIGHT_EXIT;
			if (result) {
				result->changed = 1;
				result->reason = GED_HA_REASON_LIGHT_EXIT;
			}
			return 1;
		}
	}

	if (core->state == GED_HA_STATE_RENDER)
		core->expiry_ns = sample->fence_timestamp_ns + GED_HA_LEASE_NS;
	if (result)
		result->expiry_ns = core->expiry_ns;
	return 1;
}

#endif /* __GED_HYBRID_ADAPTIVE_CORE_H__ */

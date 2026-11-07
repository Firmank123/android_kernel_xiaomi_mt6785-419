/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __GED_HYBRID_ADAPTIVE_H__
#define __GED_HYBRID_ADAPTIVE_H__

#include <linux/types.h>
#include <linux/errno.h>
#include <linux/string.h>

#include "ged_hybrid_adaptive_core.h"

struct ged_hybrid_adaptive_sample {
	u64 generation;
	u64 timestamp_ns;
	u64 queue_timestamp_ns;
	u64 fence_timestamp_ns;
	u64 window_id;
	s64 cpu_remained_ns;
	s64 gpu_remained_ns;
	u32 pid;
	u32 frame_id;
	u32 target_fps;
	u32 gpu_loading;
	bool target_fps_valid;
	bool fence_valid;
	bool gpu_loading_valid;
	bool is_surfaceflinger;
	bool latest_frame;
};

struct ged_hybrid_adaptive_status {
	u64 generation;
	u64 last_observation_ns;
	u64 last_evidence_ns;
	u64 expiry_ns;
	u64 sample_generation;
	u64 sample_timestamp_ns;
	u64 sample_queue_timestamp_ns;
	u64 sample_fence_timestamp_ns;
	u64 sample_window_id;
	s64 sample_cpu_remained_ns;
	s64 sample_gpu_remained_ns;
	u32 sample_pid;
	u32 sample_frame_id;
	u32 sample_target_fps;
	u32 sample_gpu_loading;
	unsigned int mode;
	unsigned int state;
	unsigned int reason;
	bool sample_valid;
	bool sample_target_fps_valid;
	bool display_known;
	bool display_on;
	bool suspended;
	bool shutting_down;
};

#ifdef CONFIG_MACH_MT6785
int ged_hybrid_adaptive_init(void);
void ged_hybrid_adaptive_quiesce(void);
void ged_hybrid_adaptive_exit(void);
u64 ged_hybrid_adaptive_generation(void);
int ged_hybrid_adaptive_set_mode(unsigned int mode);
void ged_hybrid_adaptive_get_status(
	struct ged_hybrid_adaptive_status *status);
void ged_hybrid_adaptive_display_event(bool known, bool on);
void ged_hybrid_adaptive_pm_event(bool suspended);
void ged_hybrid_adaptive_sample(
	const struct ged_hybrid_adaptive_sample *sample);
#else
static inline int ged_hybrid_adaptive_init(void)
{
	return 0;
}

static inline void ged_hybrid_adaptive_quiesce(void)
{
}

static inline void ged_hybrid_adaptive_exit(void)
{
}

static inline u64 ged_hybrid_adaptive_generation(void)
{
	return 0;
}

static inline int ged_hybrid_adaptive_set_mode(unsigned int mode)
{
	return -EINVAL;
}

static inline void ged_hybrid_adaptive_get_status(
	struct ged_hybrid_adaptive_status *status)
{
	memset(status, 0, sizeof(*status));
}

static inline void ged_hybrid_adaptive_display_event(bool known, bool on)
{
}

static inline void ged_hybrid_adaptive_pm_event(bool suspended)
{
}

static inline void ged_hybrid_adaptive_sample(
	const struct ged_hybrid_adaptive_sample *sample)
{
}
#endif

#endif /* __GED_HYBRID_ADAPTIVE_H__ */

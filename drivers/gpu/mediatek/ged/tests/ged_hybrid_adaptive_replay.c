// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <stdlib.h>

/* Host replay for the transition core; not part of the kernel build. */
#include "ged_hybrid_adaptive_core.h"

static int failures;

#define CHECK(cond) do { \
	if (!(cond)) { \
		fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); \
		failures++; \
	} \
} while (0)

static void active_core(struct ged_ha_core *core)
{
	struct ged_ha_result result;

	ged_ha_core_init(core);
	ged_ha_core_set_mode(core, GED_HA_MODE_OBSERVE, &result);
	ged_ha_core_set_display(core, 1, 1, GED_HA_REASON_UNBLANK, &result);
}

static struct ged_ha_sample gpu_deadline_sample(const struct ged_ha_core *core,
		unsigned long long now, unsigned long long queue_time,
		int pid, unsigned long long window)
{
	struct ged_ha_sample sample = { 0 };

	sample.generation = core->generation;
	sample.timestamp_ns = now;
	sample.queue_timestamp_ns = queue_time;
	sample.fence_timestamp_ns = now;
	sample.pid = pid;
	sample.window_id = window;
	sample.frame_id = (int)(queue_time / GED_HA_EVAL_INTERVAL_NS);
	sample.target_fps = 60;
	sample.target_fps_valid = 1;
	sample.fence_valid = 1;
	sample.gpu_loading_valid = 1;
	sample.latest_frame = 1;
	sample.gpu_loading = 80;
	sample.cpu_remained_ns = 1000000;
	sample.gpu_remained_ns = -1000000;
	return sample;
}

static void enter_render(struct ged_ha_core *core)
{
	struct ged_ha_result result;
	struct ged_ha_sample sample;

	active_core(core);
	sample = gpu_deadline_sample(core, 1000000000ULL, 990000000ULL,
			1, 101ULL);
	ged_ha_core_sample(core, &sample, &result);
	CHECK(core->state == GED_HA_STATE_BASELINE);
	sample = gpu_deadline_sample(core, 1250000000ULL, 1240000000ULL,
			1, 101ULL);
	ged_ha_core_sample(core, &sample, &result);
	CHECK(core->state == GED_HA_STATE_RENDER);
}

static void test_short_burst(void)
{
	struct ged_ha_core core;
	struct ged_ha_result result;
	struct ged_ha_sample sample;

	active_core(&core);
	sample = gpu_deadline_sample(&core, 1000000000ULL, 990000000ULL,
			1, 101ULL);
	ged_ha_core_sample(&core, &sample, &result);
	sample = gpu_deadline_sample(&core, 1100000000ULL, 1090000000ULL,
			1, 101ULL);
	ged_ha_core_sample(&core, &sample, &result);
	CHECK(core.state == GED_HA_STATE_BASELINE);
	CHECK(core.heavy_windows == 1);
}

static void test_old_candidate_window_expires(void)
{
	struct ged_ha_core core;
	struct ged_ha_result result;
	struct ged_ha_sample sample;

	active_core(&core);
	sample = gpu_deadline_sample(&core, 1000000000ULL, 990000000ULL,
			1, 101ULL);
	ged_ha_core_sample(&core, &sample, &result);
	sample = gpu_deadline_sample(&core, 2250000000ULL, 2240000000ULL,
			1, 101ULL);
	ged_ha_core_sample(&core, &sample, &result);
	CHECK(core.state == GED_HA_STATE_BASELINE);
	CHECK(core.heavy_windows == 1);
}

static void test_gpu_deadline_and_light_exit(void)
{
	struct ged_ha_core core;
	struct ged_ha_result result;
	struct ged_ha_sample sample;
	unsigned long long now = 1500000000ULL;
	int i;

	enter_render(&core);
	for (i = 0; i < 4; i++) {
		sample = gpu_deadline_sample(&core, now, now - 10000000ULL,
				1, 101ULL);
		sample.gpu_remained_ns = 2000000;
		ged_ha_core_sample(&core, &sample, &result);
		now += GED_HA_EVAL_INTERVAL_NS;
	}
	CHECK(core.state == GED_HA_STATE_BASELINE);
	CHECK(result.reason == GED_HA_REASON_LIGHT_EXIT);
}

static void test_invalid_and_corrupt_samples(void)
{
	int i;

	for (i = 0; i < 9; i++) {
		struct ged_ha_core core;
		struct ged_ha_result result;
		struct ged_ha_sample sample;

		active_core(&core);
		sample = gpu_deadline_sample(&core, 1000000000ULL,
				990000000ULL, 1, 101ULL);
		switch (i) {
		case 0:
			sample.fence_valid = 0;
			break;
		case 1:
			sample.is_surfaceflinger = 1;
			break;
		case 2:
			sample.gpu_loading_valid = 0;
			break;
		case 3:
			sample.target_fps_valid = 0;
			break;
		case 4:
			sample.gpu_loading = 101;
			break;
		case 5:
			sample.gpu_remained_ns = -600000000LL;
			break;
		case 6:
			sample.queue_timestamp_ns = 0;
		sample.fence_timestamp_ns = 0;
			break;
		case 7:
			sample.queue_timestamp_ns = 0;
		sample.fence_timestamp_ns = 0;
			sample.timestamp_ns = 2000000000ULL;
			break;
		case 8:
			sample.latest_frame = 0;
			break;
		}
		ged_ha_core_sample(&core, &sample, &result);
		CHECK(core.state == GED_HA_STATE_BASELINE);
		CHECK(core.heavy_windows == 0);
	}
}

static void test_ambiguous_cpu_or_gpu_miss(void)
{
	struct ged_ha_core core;
	struct ged_ha_result result;
	struct ged_ha_sample sample;

	active_core(&core);
	sample = gpu_deadline_sample(&core, 1000000000ULL, 990000000ULL,
			1, 101ULL);
	sample.gpu_remained_ns = 1000000;
	sample.cpu_remained_ns = -1000000;
	ged_ha_core_sample(&core, &sample, &result);
	CHECK(core.state == GED_HA_STATE_BASELINE);
	CHECK(core.heavy_windows == 0);

	sample = gpu_deadline_sample(&core, 1250000000ULL, 1240000000ULL,
			1, 101ULL);
	sample.cpu_remained_ns = -1000000;
	ged_ha_core_sample(&core, &sample, &result);
	CHECK(core.state == GED_HA_STATE_BASELINE);
	CHECK(core.heavy_windows == 0);
}

static void test_unknown_fps_is_observed_but_not_classified(void)
{
	struct ged_ha_core core;
	struct ged_ha_result result;
	struct ged_ha_sample sample;

	active_core(&core);
	sample = gpu_deadline_sample(&core, 1000000000ULL, 990000000ULL,
			1, 101ULL);
	sample.target_fps_valid = 0;
	ged_ha_core_sample(&core, &sample, &result);
	CHECK(result.observed == 1);
	CHECK(result.accepted == 0);
	CHECK(core.state == GED_HA_STATE_BASELINE);
	CHECK(core.heavy_windows == 0);
}

static void test_stream_change_and_old_frame(void)
{
	struct ged_ha_core core;
	struct ged_ha_result result;
	struct ged_ha_sample sample;

	enter_render(&core);
	sample = gpu_deadline_sample(&core, 1500000000ULL, 1490000000ULL,
			2, 202ULL);
	ged_ha_core_sample(&core, &sample, &result);
	CHECK(core.state == GED_HA_STATE_BASELINE);
	CHECK(core.heavy_windows == 1);

	sample = gpu_deadline_sample(&core, 1750000000ULL, 1240000000ULL,
			1, 101ULL);
	ged_ha_core_sample(&core, &sample, &result);
	CHECK(core.state == GED_HA_STATE_BASELINE);
	CHECK(core.heavy_windows == 1);
}

static void test_expiry_blank_disable_suspend(void)
{
	struct ged_ha_core core;
	struct ged_ha_result result;
	struct ged_ha_sample sample;
	unsigned long long old_generation;

	enter_render(&core);
	old_generation = core.generation;
	CHECK(core.expiry_ns == 2250000000ULL);
	CHECK(!ged_ha_core_expire(&core, core.expiry_ns - 1, core.generation,
			&result));
	CHECK(ged_ha_core_expire(&core, core.expiry_ns, core.generation,
			&result));
	CHECK(core.state == GED_HA_STATE_BASELINE);
	CHECK(result.reason == GED_HA_REASON_EXPIRY);

	enter_render(&core);
	old_generation = core.generation;
	ged_ha_core_set_display(&core, 1, 0, GED_HA_REASON_BLANK, &result);
	CHECK(core.state == GED_HA_STATE_BASELINE);
	sample = gpu_deadline_sample(&core, 2000000000ULL, 1990000000ULL,
			1, 101ULL);
	sample.generation = old_generation;
	ged_ha_core_sample(&core, &sample, &result);
	CHECK(core.state == GED_HA_STATE_BASELINE);

	ged_ha_core_set_display(&core, 1, 1, GED_HA_REASON_UNBLANK, &result);
	old_generation = core.generation;
	ged_ha_core_set_mode(&core, GED_HA_MODE_OFF, &result);
	sample = gpu_deadline_sample(&core, 2250000000ULL, 2240000000ULL,
			1, 101ULL);
	sample.generation = old_generation;
	ged_ha_core_sample(&core, &sample, &result);
	CHECK(core.state == GED_HA_STATE_BASELINE);
	CHECK(core.mode == GED_HA_MODE_OFF);

	ged_ha_core_set_mode(&core, GED_HA_MODE_OBSERVE, &result);
	ged_ha_core_set_suspended(&core, 1, &result);
	CHECK(core.state == GED_HA_STATE_BASELINE);
	sample = gpu_deadline_sample(&core, 2500000000ULL, 2490000000ULL,
			1, 101ULL);
	ged_ha_core_sample(&core, &sample, &result);
	CHECK(core.state == GED_HA_STATE_BASELINE);
	ged_ha_core_set_suspended(&core, 0, &result);
	CHECK(!core.display_known);
	CHECK(!core.display_on);
	sample = gpu_deadline_sample(&core, 2750000000ULL, 2740000000ULL,
			1, 101ULL);
	ged_ha_core_sample(&core, &sample, &result);
	CHECK(core.state == GED_HA_STATE_BASELINE);
	CHECK(result.observed == 0);

	ged_ha_core_set_mode(&core, GED_HA_MODE_AUTO, &result);
	ged_ha_core_set_display(&core, 1, 1, GED_HA_REASON_UNBLANK, &result);
	sample = gpu_deadline_sample(&core, 3000000000ULL, 2990000000ULL,
			1, 101ULL);
	ged_ha_core_sample(&core, &sample, &result);
	sample = gpu_deadline_sample(&core, 3250000000ULL, 3240000000ULL,
			1, 101ULL);
	ged_ha_core_sample(&core, &sample, &result);
	CHECK(core.mode == GED_HA_MODE_AUTO);
	CHECK(core.state == GED_HA_STATE_RENDER);
}

static void test_unknown_display_at_boot(void)
{
	struct ged_ha_core core;
	struct ged_ha_result result;
	struct ged_ha_sample sample;

	ged_ha_core_init(&core);
	ged_ha_core_set_mode(&core, GED_HA_MODE_OBSERVE, &result);
	sample = gpu_deadline_sample(&core, 1000000000ULL, 990000000ULL,
			1, 101ULL);
	ged_ha_core_sample(&core, &sample, &result);
	CHECK(core.state == GED_HA_STATE_BASELINE);
	CHECK(result.observed == 0);
}

int main(void)
{
	test_short_burst();
	test_old_candidate_window_expires();
	test_gpu_deadline_and_light_exit();
	test_invalid_and_corrupt_samples();
	test_ambiguous_cpu_or_gpu_miss();
	test_unknown_fps_is_observed_but_not_classified();
	test_stream_change_and_old_frame();
	test_expiry_blank_disable_suspend();
	test_unknown_display_at_boot();
	if (failures) {
		fprintf(stderr, "%d replay assertion(s) failed\n", failures);
		return EXIT_FAILURE;
	}
	puts("GED adaptive replay passed");
	return EXIT_SUCCESS;
}

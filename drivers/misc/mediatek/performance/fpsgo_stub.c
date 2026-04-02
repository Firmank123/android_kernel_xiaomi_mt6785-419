// SPDX-License-Identifier: GPL-2.0
/*
 * Mediatek FPSGO/FBT Stub Driver
 * Provides dummy sysfs nodes to silence libperfmgr warnings.
 *
 * Copyright (C) 2026 Gemini CLI
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>
#include <linux/init.h>

/* Stubs for /sys/module/fbt_cpu/parameters/ */
static int stub_int;
module_param_named(floor_bound, stub_int, int, 0644);
module_param_named(variance, stub_int, int, 0644);
module_param_named(bhr_opp, stub_int, int, 0644);
module_param_named(bhr, stub_int, int, 0644);

/* Stubs for /sys/kernel/fpsgo/ */
static struct kobject *fpsgo_kobj;
static struct kobject *common_kobj;
static struct kobject *fbt_kobj;

static ssize_t stub_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
    return sprintf(buf, "0\n");
}

static ssize_t stub_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
    return count;
}

static struct kobj_attribute gpu_block_boost_attr = __ATTR(gpu_block_boost, 0644, stub_show, stub_store);
static struct kobj_attribute boost_ta_attr = __ATTR(boost_ta, 0644, stub_show, stub_store);

static int __init fpsgo_stub_init(void)
{
    /* Create /sys/kernel/fpsgo/ */
    fpsgo_kobj = kobject_create_and_add("fpsgo", kernel_kobj);
    if (!fpsgo_kobj)
        return -ENOMEM;

    /* Create /sys/kernel/fpsgo/common/gpu_block_boost */
    common_kobj = kobject_create_and_add("common", fpsgo_kobj);
    if (common_kobj) {
        if (sysfs_create_file(common_kobj, &gpu_block_boost_attr.attr))
            pr_err("fpsgo_stub: failed to create gpu_block_boost\n");
    }

    /* Create /sys/kernel/fpsgo/fbt/boost_ta */
    fbt_kobj = kobject_create_and_add("fbt", fpsgo_kobj);
    if (fbt_kobj) {
        if (sysfs_create_file(fbt_kobj, &boost_ta_attr.attr))
            pr_err("fpsgo_stub: failed to create boost_ta\n");
    }

    pr_info("fpsgo_stub: dummy nodes initialized for libperfmgr compatibility\n");
    return 0;
}

/* Initialize early enough for HALs */
fs_initcall(fpsgo_stub_init);

MODULE_DESCRIPTION("Mediatek FPSGO/FBT Stub Driver for libperfmgr compatibility");
MODULE_LICENSE("GPL");

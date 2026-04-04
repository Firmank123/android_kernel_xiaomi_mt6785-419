// SPDX-License-Identifier: GPL-2.0
/*
 * Mediatek libperfmgr Stub Driver
 * Provides dummy sysfs/procfs nodes to silence libperfmgr warnings.
 *
 * Copyright (C) 2026
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/uaccess.h>
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

/* Procfs stubs for /proc/perfmgr/boost_ctrl/eas_ctrl/ */
static struct proc_dir_entry *perfmgr_dir;
static struct proc_dir_entry *boost_ctrl_dir;
static struct proc_dir_entry *eas_ctrl_dir;

static int stub_proc_show(struct seq_file *m, void *v)
{
	seq_puts(m, "0\n");
	return 0;
}

static int stub_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, stub_proc_show, NULL);
}

static ssize_t stub_proc_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos)
{
	return count;
}

static const struct file_operations stub_proc_fops = {
	.owner = THIS_MODULE,
	.open = stub_proc_open,
	.read = seq_read,
	.write = stub_proc_write,
	.llseek = seq_lseek,
	.release = single_release,
};

static const char *eas_ctrl_nodes[] = {
	"perfserv_fg_boost",
	"perfserv_ta_boost",
	"perfserv_bg_boost",
	"m_sched_migrate_cost_n",
	"perfserv_uclamp_min",
	"perfserv_bg_uclamp_min",
	"perfserv_fg_uclamp_min",
	"perfserv_ta_uclamp_min",
	NULL
};

static void create_eas_ctrl_stubs(void)
{
	const char **node;

	perfmgr_dir = proc_mkdir("perfmgr", NULL);
	if (!perfmgr_dir)
		return;

	boost_ctrl_dir = proc_mkdir("boost_ctrl", perfmgr_dir);
	if (!boost_ctrl_dir)
		return;

	eas_ctrl_dir = proc_mkdir("eas_ctrl", boost_ctrl_dir);
	if (!eas_ctrl_dir)
		return;

	for (node = eas_ctrl_nodes; *node; node++)
		proc_create(*node, 0644, eas_ctrl_dir, &stub_proc_fops);
}

static int __init libperfmgr_stub_init(void)
{
	/* Create /sys/kernel/fpsgo/ */
	fpsgo_kobj = kobject_create_and_add("fpsgo", kernel_kobj);
	if (!fpsgo_kobj)
		return -ENOMEM;

	/* Create /sys/kernel/fpsgo/common/gpu_block_boost */
	common_kobj = kobject_create_and_add("common", fpsgo_kobj);
	if (common_kobj) {
		if (sysfs_create_file(common_kobj, &gpu_block_boost_attr.attr))
			pr_err("libperfmgr_stub: failed to create gpu_block_boost\n");
	}

	/* Create /sys/kernel/fpsgo/fbt/boost_ta */
	fbt_kobj = kobject_create_and_add("fbt", fpsgo_kobj);
	if (fbt_kobj) {
		if (sysfs_create_file(fbt_kobj, &boost_ta_attr.attr))
			pr_err("libperfmgr_stub: failed to create boost_ta\n");
	}

	/* Create /proc/perfmgr/boost_ctrl/eas_ctrl/ nodes */
	create_eas_ctrl_stubs();

	pr_info("libperfmgr_stub: dummy nodes initialized for libperfmgr compatibility\n");
	return 0;
}

/* Initialize early enough for HALs */
fs_initcall(libperfmgr_stub_init);

MODULE_DESCRIPTION("Mediatek libperfmgr Stub Driver for libperfmgr compatibility");
MODULE_LICENSE("GPL");

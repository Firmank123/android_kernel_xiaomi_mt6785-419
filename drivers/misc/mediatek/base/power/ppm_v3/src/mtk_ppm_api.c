// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2020 MediaTek Inc.
 */

#include "mtk_ppm_internal.h"

/* APIs */
void mt_ppm_set_dvfs_table(unsigned int cpu,
	struct cpufreq_frequency_table *tbl,
	unsigned int num, enum dvfs_table_type type)
{
	struct ppm_data *p = &ppm_main_info;
	int i, j;

	FUNC_ENTER(FUNC_LV_API);

	for_each_ppm_clusters(i) {
		if (p->cluster_info[i].cpu_id != cpu)
			continue;

		/* return if table is existed */
		if (p->cluster_info[i].dvfs_tbl)
			return;

		ppm_lock(&(p->lock));

		p->dvfs_tbl_type = type;
		p->cluster_info[i].dvfs_tbl = tbl;
		p->cluster_info[i].dvfs_opp_num = num;
		/* dump dvfs table */
		ppm_info("DVFS table type = %d\n", type);
		ppm_info("DVFS table of cluster %d:\n",
			p->cluster_info[i].cluster_id);
		for (j = 0; j < num; j++) {
			ppm_info("%d: %d KHz\n",
				j, p->cluster_info[i].dvfs_tbl[j].frequency);
		}

		ppm_unlock(&(p->lock));

		FUNC_EXIT(FUNC_LV_API);
		return;
	}

	if (i == p->cluster_num)
		ppm_err("@%s: cpu_id(%d) not found!\n", __func__, cpu);

	FUNC_EXIT(FUNC_LV_API);
}

int mt_ppm_register_client(enum ppm_client client,
	void (*limit)(struct ppm_client_req req))
{
	int ret = 0;

	FUNC_ENTER(FUNC_LV_API);

	if (client < 0 || client >= NR_PPM_CLIENTS) {
		ppm_err("invalid client value: %d\n", client);
		return -EINVAL;
	}

	ppm_lock(&ppm_main_info.lock);

	/* init client */
	switch (client) {
	case PPM_CLIENT_DVFS:
		ppm_main_info.client_info[client].name = "DVFS";
		/* Replace DVFS callback fun with qos*/
		ret = ppm_init_qos_request();
		break;
	case PPM_CLIENT_HOTPLUG:
		ppm_main_info.client_info[client].name = "HOTPLUG";
		break;
	default:
		ppm_main_info.client_info[client].name = "UNKNOWN";
		break;
	}

	if (!ret) {
		ppm_main_info.client_info[client].client = client;
		ppm_main_info.client_info[client].limit_cb = limit;
	} else {
		ppm_main_info.client_info[client].name = NULL;
	}

	ppm_unlock(&ppm_main_info.lock);

	FUNC_EXIT(FUNC_LV_API);

	return ret;
}

void mt_ppm_unregister_client(enum ppm_client client)
{
	FUNC_ENTER(FUNC_LV_API);

	if (client < 0 || client >= NR_PPM_CLIENTS) {
		ppm_err("invalid client value: %d\n", client);
		return;
	}

	ppm_lock(&ppm_main_info.lock);

	if (client == PPM_CLIENT_DVFS)
		ppm_deinit_qos_request();

	ppm_main_info.client_info[client].limit_cb = NULL;
	ppm_main_info.client_info[client].name = NULL;

	ppm_unlock(&ppm_main_info.lock);

	FUNC_EXIT(FUNC_LV_API);
}


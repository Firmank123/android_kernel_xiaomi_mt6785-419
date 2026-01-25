/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (c) 2019 MediaTek Inc.
*/

#ifndef _MT_GPUFREQ_CORE_H_
#define _MT_GPUFREQ_CORE_H_

/**************************************************
 * GPU DVFS OPP Table Setting
 **************************************************/
#define SEG_GPU_DVFS_FREQ0		(950000)	/* KHz - OC Max Safe */
#define SEG_GPU_DVFS_FREQ1		(940000)	/* KHz - OC */
#define SEG_GPU_DVFS_FREQ2		(930000)	/* KHz - OC */
#define SEG_GPU_DVFS_FREQ3		(920000)	/* KHz - OC */
#define SEG_GPU_DVFS_FREQ4		(910000)	/* KHz - OC */
#define SEG_GPU_DVFS_FREQ5		(900000)	/* KHz - Stock max */
#define SEG_GPU_DVFS_FREQ6		(897000)	/* KHz */
#define SEG_GPU_DVFS_FREQ7		(892000)	/* KHz */
#define SEG_GPU_DVFS_FREQ8		(888000)	/* KHz */
#define SEG_GPU_DVFS_FREQ9		(884000)	/* KHz */
#define SEG_GPU_DVFS_FREQ10		(880000)	/* KHz */
#define SEG_GPU_DVFS_FREQ11		(875000)	/* KHz */
#define SEG_GPU_DVFS_FREQ12		(871000)	/* KHz */
#define SEG_GPU_DVFS_FREQ13		(867000)	/* KHz */
#define SEG_GPU_DVFS_FREQ14		(862000)	/* KHz */
#define SEG_GPU_DVFS_FREQ15		(858000)	/* KHz */
#define SEG_GPU_DVFS_FREQ16		(854000)	/* KHz */
#define SEG_GPU_DVFS_FREQ17		(850000)	/* KHz */
#define SEG_GPU_DVFS_FREQ18		(835000)	/* KHz */
#define SEG_GPU_DVFS_FREQ19		(821000)	/* KHz */
#define SEG_GPU_DVFS_FREQ20		(806000)	/* KHz */
#define SEG_GPU_DVFS_FREQ21		(792000)	/* KHz */
#define SEG_GPU_DVFS_FREQ22		(778000)	/* KHz */
#define SEG_GPU_DVFS_FREQ23		(763000)	/* KHz */
#define SEG_GPU_DVFS_FREQ24		(749000)	/* KHz */
#define SEG_GPU_DVFS_FREQ25		(735000)	/* KHz */
#define SEG_GPU_DVFS_FREQ26		(720000)	/* KHz */
#define SEG_GPU_DVFS_FREQ27		(706000)	/* KHz */
#define SEG_GPU_DVFS_FREQ28		(691000)	/* KHz */
#define SEG_GPU_DVFS_FREQ29		(677000)	/* KHz */
#define SEG_GPU_DVFS_FREQ30		(663000)	/* KHz */
#define SEG_GPU_DVFS_FREQ31		(648000)	/* KHz */
#define SEG_GPU_DVFS_FREQ32		(634000)	/* KHz */
#define SEG_GPU_DVFS_FREQ33		(620000)	/* KHz */
#define SEG_GPU_DVFS_FREQ34		(595000)	/* KHz */
#define SEG_GPU_DVFS_FREQ35		(570000)	/* KHz */
#define SEG_GPU_DVFS_FREQ36		(545000)	/* KHz */
#define SEG_GPU_DVFS_FREQ37		(520000)	/* KHz */
#define SEG_GPU_DVFS_FREQ38		(495000)	/* KHz */
#define SEG_GPU_DVFS_FREQ39		(470000)	/* KHz */
#define SEG_GPU_DVFS_FREQ40		(445000)	/* KHz */
#define SEG_GPU_DVFS_FREQ41		(420000)	/* KHz */
#define SEG_GPU_DVFS_FREQ42		(395000)	/* KHz */
#define SEG_GPU_DVFS_FREQ43		(370000)	/* KHz */
#define SEG_GPU_DVFS_FREQ44		(345000)	/* KHz */
#define SEG_GPU_DVFS_FREQ45		(320000)	/* KHz */
#define SEG_GPU_DVFS_FREQ46		(295000)	/* KHz */
#define SEG_GPU_DVFS_FREQ47		(270000)	/* KHz */
#define SEG_GPU_DVFS_FREQ48		(320000)	/* KHz - Unused */
#define SEG_GPU_DVFS_FREQ49		(295000)	/* KHz - Unused */
#define SEG_GPU_DVFS_FREQ50		(270000)	/* KHz - Unused */

#define SEG_GPU_DVFS_VOLT0		(93125)		/* mV x 100 - 950MHz OC Safe */
#define SEG_GPU_DVFS_VOLT1		(91875)		/* mV x 100 - 940MHz OC */
#define SEG_GPU_DVFS_VOLT2		(90625)		/* mV x 100 - 930MHz OC */
#define SEG_GPU_DVFS_VOLT3		(90000)		/* mV x 100 - 920MHz OC */
#define SEG_GPU_DVFS_VOLT4		(89375)		/* mV x 100 - 910MHz OC */
#define SEG_GPU_DVFS_VOLT5		(88750)		/* mV x 100 - 900MHz sustained */
#define SEG_GPU_DVFS_VOLT6		(88125)		/* mV x 100 - 897MHz */
#define SEG_GPU_DVFS_VOLT7		(87500)		/* mV x 100 - 892MHz */
#define SEG_GPU_DVFS_VOLT8		(86875)		/* mV x 100 - 888MHz */
#define SEG_GPU_DVFS_VOLT9		(86250)		/* mV x 100 - 884MHz */
#define SEG_GPU_DVFS_VOLT10		(85625)		/* mV x 100 */
#define SEG_GPU_DVFS_VOLT11		(85000)		/* mV x 100 */
#define SEG_GPU_DVFS_VOLT12		(84375)		/* mV x 100 */
#define SEG_GPU_DVFS_VOLT13		(83750)		/* mV x 100 */
#define SEG_GPU_DVFS_VOLT14		(83125)		/* mV x 100 */
#define SEG_GPU_DVFS_VOLT15		(82500)		/* mV x 100 */
#define SEG_GPU_DVFS_VOLT16		(81875)		/* mV x 100 */
#define SEG_GPU_DVFS_VOLT17		(81250)		/* mV x 100 */
#define SEG_GPU_DVFS_VOLT18		(81250)		/* mV x 100 */
#define SEG_GPU_DVFS_VOLT19		(80625)		/* mV x 100 */
#define SEG_GPU_DVFS_VOLT20		(80000)		/* mV x 100 */
#define SEG_GPU_DVFS_VOLT21		(79375)		/* mV x 100 */
#define SEG_GPU_DVFS_VOLT22		(78750)		/* mV x 100 */
#define SEG_GPU_DVFS_VOLT23		(78125)		/* mV x 100 */
#define SEG_GPU_DVFS_VOLT24		(77500)		/* mV x 100 */
#define SEG_GPU_DVFS_VOLT25		(76875)		/* mV x 100 */
#define SEG_GPU_DVFS_VOLT26		(76250)		/* mV x 100 */
#define SEG_GPU_DVFS_VOLT27		(75625)		/* mV x 100 */
#define SEG_GPU_DVFS_VOLT28		(75000)		/* mV x 100 */
#define SEG_GPU_DVFS_VOLT29		(74375)		/* mV x 100 */
#define SEG_GPU_DVFS_VOLT30		(73750)		/* mV x 100 */
#define SEG_GPU_DVFS_VOLT31		(73125)		/* mV x 100 */
#define SEG_GPU_DVFS_VOLT32		(72500)		/* mV x 100 */
#define SEG_GPU_DVFS_VOLT33		(71875)		/* mV x 100 */
#define SEG_GPU_DVFS_VOLT34		(71250)		/* mV x 100 */
#define SEG_GPU_DVFS_VOLT35		(70625)		/* mV x 100 */
#define SEG_GPU_DVFS_VOLT36		(70000)		/* mV x 100 */
#define SEG_GPU_DVFS_VOLT37		(69375)		/* mV x 100 */
#define SEG_GPU_DVFS_VOLT38		(68750)		/* mV x 100 */
#define SEG_GPU_DVFS_VOLT39		(68125)		/* mV x 100 */
#define SEG_GPU_DVFS_VOLT40		(67500)		/* mV x 100 */
#define SEG_GPU_DVFS_VOLT41		(66875)		/* mV x 100 */
#define SEG_GPU_DVFS_VOLT42		(66250)		/* mV x 100 */
#define SEG_GPU_DVFS_VOLT43		(65625)		/* mV x 100 */
#define SEG_GPU_DVFS_VOLT44		(65000)		/* mV x 100 */
#define SEG_GPU_DVFS_VOLT45		(64375)		/* mV x 100 */
#define SEG_GPU_DVFS_VOLT46		(63750)		/* mV x 100 */
#define SEG_GPU_DVFS_VOLT47		(61250)		/* mV x 100 */
#define SEG_GPU_DVFS_VOLT48		(64375)		/* mV x 100 - Unused */
#define SEG_GPU_DVFS_VOLT49		(63750)		/* mV x 100 - Unused */
#define SEG_GPU_DVFS_VOLT50		(61250)		/* mV x 100 - Unused */

/* VSRAM for Helio G95 - 950MHz Max Stable */
#define SEG_GPU_DVFS_VSRAM0		(103125)	/* mV x 100 - 950MHz OC Safe */
#define SEG_GPU_DVFS_VSRAM1		(101875)	/* mV x 100 - 940MHz OC */
#define SEG_GPU_DVFS_VSRAM2		(100625)	/* mV x 100 - 930MHz OC */
#define SEG_GPU_DVFS_VSRAM3		(100000)	/* mV x 100 - 920MHz OC */
#define SEG_GPU_DVFS_VSRAM4		(99375)		/* mV x 100 - 910MHz OC */
#define SEG_GPU_DVFS_VSRAM5		(98750)		/* mV x 100 - 900MHz sustained */
#define SEG_GPU_DVFS_VSRAM6		(98125)		/* mV x 100 - 897MHz */
#define SEG_GPU_DVFS_VSRAM7		(97500)		/* mV x 100 - 892MHz */
#define SEG_GPU_DVFS_VSRAM8		(96875)		/* mV x 100 - 888MHz */
#define SEG_GPU_DVFS_VSRAM9		(96250)		/* mV x 100 - 884MHz */
#define SEG_GPU_DVFS_VSRAM10	(95625)		/* mV x 100 */
#define SEG_GPU_DVFS_VSRAM11	(95000)		/* mV x 100 */
#define SEG_GPU_DVFS_VSRAM12	(94375)		/* mV x 100 */
#define SEG_GPU_DVFS_VSRAM13	(93750)		/* mV x 100 */
#define SEG_GPU_DVFS_VSRAM14	(93125)		/* mV x 100 */
#define SEG_GPU_DVFS_VSRAM15	(92500)		/* mV x 100 */
#define SEG_GPU_DVFS_VSRAM16	(91875)		/* mV x 100 */
#define SEG_GPU_DVFS_VSRAM17	(91250)		/* mV x 100 */
#define SEG_GPU_DVFS_VSRAM18	(91250)		/* mV x 100 */
#define SEG_GPU_DVFS_VSRAM19	(90625)		/* mV x 100 */
#define SEG_GPU_DVFS_VSRAM20	(90000)		/* mV x 100 */
#define SEG_GPU_DVFS_VSRAM21	(89375)		/* mV x 100 */
#define SEG_GPU_DVFS_VSRAM22	(88750)		/* mV x 100 */
#define SEG_GPU_DVFS_VSRAM23	(88125)		/* mV x 100 */
#define SEG_GPU_DVFS_VSRAM24	(87500)		/* mV x 100 */
#define SEG_GPU_DVFS_VSRAM25	(86875)		/* mV x 100 */
#define SEG_GPU_DVFS_VSRAM26	(86250)		/* mV x 100 */
#define SEG_GPU_DVFS_VSRAM27	(85625)		/* mV x 100 */
#define SEG_GPU_DVFS_VSRAM28	(85000)		/* mV x 100 */
#define SEG_GPU_DVFS_VSRAM29	(86250)		/* mV x 100 */
#define SEG_GPU_DVFS_VSRAM30	(86250)		/* mV x 100 */
#define SEG_GPU_DVFS_VSRAM31	(86250)		/* mV x 100 */

/**************************************************
 * DVFS Setting
 **************************************************/
#define NUM_OF_OPP_IDX				(48)	/* 43 stock + 5 OC */
#define BOOTUP_OPP_IDX				(47)	/* Boot at 270MHz */
#define FIXED_VSRAM_VOLT			(85000)
#define FIXED_VSRAM_VOLT_THSRESHOLD	(75000)	/* Safe VSRAM management */

/**************************************************
 * PMIC Setting
 **************************************************/
#define VGPU_OPP_MAX_VOLT		(SEG_GPU_DVFS_VOLT0)
#define VGPU_OPP_MIN_VOLT		(SEG_GPU_DVFS_VOLT47)
#define VSRAM_GPU_OPP_MAX_VOLT	(SEG_GPU_DVFS_VSRAM0)
#define VSRAM_GPU_OPP_MIN_VOLT	(SEG_GPU_DVFS_VSRAM31)
#define VGPU_MAX_VOLT			(93125)	/* mV x 100 - 950MHz safe */
#define VGPU_MIN_VOLT			(57500)	/* mV x 100 */
#define VSRAM_GPU_MAX_VOLT		(103125)/* mV x 100 - 950MHz safe */
#define VSRAM_GPU_MIN_VOLT		(85000)	/* mV x 100 */
#define PMIC_STEP				(625)	/* mV x 100 */
#define BUCK_DIFF_MAX			(27500)	/* mV x 100 - Safe switching */
#define BUCK_DIFF_MIN			(00000)	/* mV x 100 */

/**************************************************
 * Clock Setting
 **************************************************/
#define MFGPLL_CK_DEFAULT_FREQ	(620000)	/* KHz */
#define UNIVPLL_D3_DEFAULT_FREQ	(416000)	/* KHz */
#define MAINPLL_D3_DEFAULT_FREQ	(218400)	/* KHz */
#define CLK26M_DEFAULT_FREQ		(26000)		/* KHz */
#define POSDIV_2_MAX_FREQ		(1900000)	/* KHz */
#define POSDIV_2_MIN_FREQ		(750000)	/* KHz */
#define POSDIV_4_MAX_FREQ		(950000)	/* KHz */
#define POSDIV_4_MIN_FREQ		(375000)	/* KHz */
#define POSDIV_8_MAX_FREQ		(475000)	/* KHz */
#define POSDIV_8_MIN_FREQ		(187500)	/* KHz */
#define POSDIV_16_MAX_FREQ		(237500)	/* KHz */
#define POSDIV_16_MIN_FREQ		(125000)	/* KHz */
#define POSDIV_SHIFT			(24)		/* bit */
#define DDS_SHIFT				(14)		/* bit */
#define TO_MHZ_HEAD				(100)
#define TO_MHZ_TAIL				(10)
#define ROUNDING_VALUE			(5)
#define MFGPLL_FIN				(26)		/* MHz */
#define MFGPLL_FH_PLL			FH_PLL4
#define MFGPLL_CON0				(g_apmixed_base + 0x250)
#define MFGPLL_CON1				(g_apmixed_base + 0x254)
#define MFGPLL_CON2				(g_apmixed_base + 0x258)
#define MFGPLL_PWR_CON0			(g_apmixed_base + 0x25C)

/**************************************************
 * Reference Power Setting
 **************************************************/
#define GPU_ACT_REF_POWER			(1485)		/* mW - Sustained load */
#define GPU_ACT_REF_FREQ			(950000)	/* KHz - OC Max */
#define GPU_ACT_REF_VOLT			(93125)		/* mV x 100 */
#define GPU_DVFS_PTPOD_DISABLE_VOLT	(83125)		/* mV x 100 - Prevent throttle */
#define GPU_DVFS_PTPOD_DISABLE_VSRAM_VOLT (92500)	/* Safe stability */

/**************************************************
 * Log Setting
 **************************************************/
#define GPUFERQ_TAG		"[GPU/DVFS]"
#define gpufreq_pr_err(fmt, args...)	\
	pr_err(GPUFERQ_TAG"[ERROR]"fmt, ##args)
#define gpufreq_pr_warn(fmt, args...)	\
	pr_warn(GPUFERQ_TAG"[WARNING]"fmt, ##args)
#define gpufreq_pr_info(fmt, args...)	\
	pr_info(GPUFERQ_TAG"[INFO]"fmt, ##args)
#define gpufreq_pr_debug(fmt, args...)	\
	pr_debug(GPUFERQ_TAG"[DEBUG]"fmt, ##args)

/**************************************************
 * Battery Over Current Protect
 **************************************************/
#ifdef MT_GPUFREQ_BATT_OC_PROTECT
#define MT_GPUFREQ_BATT_OC_LIMIT_FREQ		(485000)	/* KHz */
#endif

/**************************************************
 * Battery Percentage Protect
 **************************************************/
#ifdef MT_GPUFREQ_BATT_PERCENT_PROTECT
#define MT_GPUFREQ_BATT_PERCENT_LIMIT_FREQ	(485000)	/* KHz */
#endif

/**************************************************
 * Low Battery Volume Protect
 **************************************************/
#ifdef MT_GPUFREQ_LOW_BATT_VOLT_PROTECT
#define MT_GPUFREQ_LOW_BATT_VOLT_LIMIT_FREQ	(485000)	/* KHz */
#endif

/**************************************************
 * Register Manipulations
 **************************************************/
#define READ_REGISTER_UINT32(reg)	\
	(*(unsigned int * const)(reg))
#define WRITE_REGISTER_UINT32(reg, val)	\
	((*(unsigned int * const)(reg)) = (val))
#define INREG32(x)	\
	READ_REGISTER_UINT32((unsigned int *)((void *)(x)))
#define OUTREG32(x, y)	\
	WRITE_REGISTER_UINT32((unsigned int *)((void *)(x)), (unsigned int)(y))
#define SETREG32(x, y)	\
	OUTREG32(x, INREG32(x)|(y))
#define CLRREG32(x, y)	\
	OUTREG32(x, INREG32(x)&~(y))
#define MASKREG32(x, y, z)	\
	OUTREG32(x, (INREG32(x)&~(y))|(z))
#define DRV_Reg32(addr)				INREG32(addr)
#define DRV_WriteReg32(addr, data)	OUTREG32(addr, data)
#define DRV_SetReg32(addr, data)	SETREG32(addr, data)
#define DRV_ClrReg32(addr, data)	CLRREG32(addr, data)

/**************************************************
 * Proc Node Definition
 **************************************************/
#ifdef CONFIG_PROC_FS
#define PROC_FOPS_RW(name)	\
	static int mt_ ## name ## _proc_open(	\
			struct inode *inode,	\
			struct file *file)	\
	{	\
		return single_open(	\
				file,	\
				mt_ ## name ## _proc_show,	\
				PDE_DATA(inode));	\
	}	\
	static const struct file_operations mt_ ## name ## _proc_fops =	\
	{	\
		.owner = THIS_MODULE,	\
		.open = mt_ ## name ## _proc_open,	\
		.read = seq_read,	\
		.llseek = seq_lseek,	\
		.release = single_release,	\
		.write = mt_ ## name ## _proc_write,	\
	}
#define PROC_FOPS_RO(name)	\
	static int mt_ ## name ## _proc_open(	\
			struct inode *inode,	\
			struct file *file)	\
	{	\
		return single_open(	\
				file,	\
				mt_ ## name ## _proc_show,	\
				PDE_DATA(inode));	\
	}	\
	static const struct file_operations mt_ ## name ## _proc_fops =	\
	{	\
		.owner = THIS_MODULE,	\
		.open = mt_ ## name ## _proc_open,	\
		.read = seq_read,	\
		.llseek = seq_lseek,	\
		.release = single_release,	\
	}
#define PROC_ENTRY(name) \
	{__stringify(name), &mt_ ## name ## _proc_fops}
#endif

/**************************************************
 * Operation Definition
 **************************************************/
#define VOLT_NORMALIZATION(volt)	\
	((volt % 625) ? (volt - (volt % 625) + 625) : volt)
#ifndef MIN
#define MIN(x, y)	(((x) < (y)) ? (x) : (y))
#endif
#define GPUOP(khz, volt, vsram)	\
	{	\
		.gpufreq_khz = khz,	\
		.gpufreq_volt = volt,	\
		.gpufreq_vsram = vsram,	\
	}

/**************************************************
 * Enumerations
 **************************************************/
enum g_segment_id_enum {
	MT6785U_SEGMENT = 1,
	MT6785T_SEGMENT,
	MT6785_SEGMENT,
	MT6783_SEGMENT,
};
enum g_posdiv_power_enum  {
	POSDIV_POWER_1 = 0,
	POSDIV_POWER_2,
	POSDIV_POWER_4,
	POSDIV_POWER_8,
	POSDIV_POWER_16,
};
enum g_clock_source_enum  {
	CLOCK_MAIN = 0,
	CLOCK_SUB,
};
enum g_limited_idx_enum {
	IDX_THERMAL_PROTECT_LIMITED = 0,
	IDX_LOW_BATT_LIMITED,
	IDX_BATT_PERCENT_LIMITED,
	IDX_BATT_OC_LIMITED,
	IDX_PBM_LIMITED,
	NUMBER_OF_LIMITED_IDX,
};

/**************************************************
 * Structures
 **************************************************/
struct g_opp_table_info {
	unsigned int gpufreq_khz;
	unsigned int gpufreq_volt;
	unsigned int gpufreq_vsram;
};
struct g_clk_info {
	/* main clock for mfg setting */
	struct clk *clk_mux;
	/* substitution clock for mfg transient mux setting */
	struct clk *clk_main_parent;
	/* substitution clock for mfg transient parent setting */
	struct clk *clk_sub_parent;
	/* clock gate, which has only two state with ON or OFF */
	struct clk *subsys_mfg_cg;
	struct clk *mtcmos_mfg_async;
	/* mtcmos_mfg dependent on mtcmos_mfg_async */
	struct clk *mtcmos_mfg;
	/* mtcmos_mfg_core0 dependent on mtcmos_mfg */
	struct clk *mtcmos_mfg_core0;
	/* mtcmos_mfg_core1 dependent on mtcmos_mfg_core0 */
	struct clk *mtcmos_mfg_core1;
	/* mtcmos_mfg_core2 dependent on mtcmos_mfg_core0 */
	struct clk *mtcmos_mfg_core2;
	/* mtcmos_mfg_core3 dependent on mtcmos_mfg_core0 */
	struct clk *mtcmos_mfg_core3;
};
struct g_pmic_info {
	struct regulator *reg_vgpu;
	struct regulator *reg_vsram_gpu;
};

/**************************************************
 * External functions declaration
 **************************************************/
extern bool mtk_get_gpu_loading(unsigned int *pLoading);
extern unsigned int mt_get_ckgen_freq(unsigned int idx);
#ifdef CONFIG_THERMAL
extern int mtk_gpufreq_register(
		struct mt_gpufreq_power_table_info *freqs, int num);
#endif

#endif /* _MT_GPUFREQ_CORE_H_ */

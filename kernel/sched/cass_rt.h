// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2024 Sultan Alsawaf <sultan@kerneltoast.com>.
 */

#ifdef CONFIG_SCHED_CASS
/*
 * Keep RT wakeup placement on the vanilla scheduler path.
 * CASS is intentionally limited to CFS.
 */
#endif /* CONFIG_SCHED_CASS */

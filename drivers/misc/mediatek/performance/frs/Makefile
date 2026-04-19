#
# Copyright (C) 2015 MediaTek Inc.
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License version 2 as
# published by the Free Software Foundation.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#

ifeq ($(CONFIG_MACH_MT6785),y)
ccflags-y += \
    -I$(srctree)/drivers/misc/mediatek/performance/fpsgo_v3/ \
    -I$(srctree)/drivers/misc/mediatek/performance/fpsgo_v3/mt-plat/ \
    -I$(srctree)/drivers/misc/mediatek/performance/fpsgo_v3/fstb/
else
ccflags-y += \
    -I$(srctree)/drivers/misc/mediatek/performance/fpsgo_v3_topology/ \
    -I$(srctree)/drivers/misc/mediatek/performance/fpsgo_v3_topology/mt-plat/ \
    -I$(srctree)/drivers/misc/mediatek/performance/fpsgo_v3_topology/fstb/
endif

obj-$(CONFIG_MTK_FRS) += frs.o

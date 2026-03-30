# SPDX-License-Identifier: GPL-2.0
hostprogs-y	:= modpost mk_elfconfig
always		:= $(hostprogs-y) empty.o

modpost-objs	:= modpost.o file2alias.o sumversion.o

devicetable-offsets-file := devicetable-offsets.h

# Force disable LTO for header generation
CFLAGS_devicetable-offsets.o := $(DISABLE_LTO)
CFLAGS_REMOVE_devicetable-offsets.o := $(CC_FLAGS_LTO)

$(obj)/$(devicetable-offsets-file): $(obj)/devicetable-offsets.s FORCE
	$(call filechk,offsets,__DEVICETABLE_OFFSETS_H__)

targets += $(devicetable-offsets-file) devicetable-offsets.s

# hadirkan kembali aturan standar untuk devicetable-offsets.s
$(obj)/devicetable-offsets.s: $(obj)/devicetable-offsets.c FORCE
	$(call if_changed_dep,cc_s_c)

# Kbuild-isms: ensure clean works
clean-files += $(devicetable-offsets-file)

obj-$(CONFIG_MODVERSIONS) += empty.o

# For the kernel, we don't want to link against any libraries.
# That is why we use -nostdlib.
#
# But for modpost, we need to link against the host's libraries.
# That is why we don't use -nostdlib.
HOST_EXTRACFLAGS += -I$(srctree)/scripts/mod

# include/linux/export.h
HOSTCFLAGS_modpost.o += -I$(objtree)/include

# we need to be able to find elf.h
ifeq ($(shell echo "" | $(HOSTCC) -E -include elf.h - >/dev/null 2>&1 && echo "yes"),yes)
  HOST_EXTRACFLAGS += -DHAVE_ELF_H
endif

# Elfconfig detection
quiet_cmd_elfconfig = MKELF   $@
      cmd_elfconfig = $(obj)/mk_elfconfig < $< > $@

$(obj)/elfconfig.h: $(obj)/empty.o FORCE
	$(call if_changed,elfconfig)

targets += elfconfig.h

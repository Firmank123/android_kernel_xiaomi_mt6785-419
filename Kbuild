# SPDX-License-Identifier: GPL-2.0
# Generate bound values for flags in struct page
# 1) Generate bounds.s
# 2) Generate bounds.h

bounds-file := include/generated/bounds.h

always  += $(bounds-file)
targets += kernel/bounds.s

# Force disable LTO for header generation
CFLAGS_bounds.o := $(DISABLE_LTO)

# We use internal kbuild rules to avoid the "is up to date" message from make
kernel/bounds.s: kernel/bounds.c FORCE
	$(call if_changed_dep,cc_s_c)

$(obj)/$(bounds-file): kernel/bounds.s FORCE
	$(call filechk,offsets,__LINUX_BOUNDS_H__)

#####
# 2) Generate timeconst.h

timeconst-file := include/generated/timeconst.h

always  += $(timeconst-file)

quiet_cmd_gentimeconst = GEN     $@
      cmd_gentimeconst = $(CONFIG_SHELL) $(srctree)/kernel/time/timeconst.bc $(HZ) > $@

$(obj)/$(timeconst-file): kernel/time/timeconst.bc FORCE
	$(call filechk,gentimeconst)

#####
# 3) Generate asm-offsets.h
#

offsets-file := include/generated/asm-offsets.h

always  += $(offsets-file)
targets += arch/$(SRCARCH)/kernel/asm-offsets.s

# Force disable LTO for header generation
CFLAGS_asm-offsets.o := $(DISABLE_LTO)

# We use internal kbuild rules to avoid the "is up to date" message from make
arch/$(SRCARCH)/kernel/asm-offsets.s: arch/$(SRCARCH)/kernel/asm-offsets.c \
                                      $(obj)/$(timeconst-file) $(obj)/$(bounds-file) FORCE
	$(call if_changed_dep,cc_s_c)

$(obj)/$(offsets-file): arch/$(SRCARCH)/kernel/asm-offsets.s FORCE
	$(call filechk,offsets,__ASM_OFFSETS_H__)

#####
# 4) Check for missing system calls
#

always += missing-syscalls

quiet_cmd_syscalls = CALL    $<
      cmd_syscalls = $(CONFIG_SHELL) $< $(CC) $(c_flags) $(missing_syscalls_flags)

missing-syscalls: scripts/checksyscalls.sh $(offsets-file) FORCE
	$(call if_changed,syscalls)

#####
# 5) Generate constants for python scripts
#

always += scripts/gdb/linux/constants.py

scripts/gdb/linux/constants.py: $(offsets-file) FORCE
	$(Q)$(MAKE) $(build)=scripts/gdb/linux $@

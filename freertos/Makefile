SHELL = bash
ifeq ($(V),1)
  Q =
else
  Q = @
endif

srctree   := $(shell pwd)

-include .config
include ./scripts/build/Build.include

# Init all relevant variables used in build files
INCS      :=
DFLAGS    :=
CFLAGS    :=
LDFLAGS   :=
LSP       :=
prepare   :=

# Toolchain
CC          = xt-xcc
AS          = xt-xcc
AR          = xt-ar
LD          = xt-ld
OBJC        = xt-objcopy
OBJS        = xt-strip
#XT_CORE     = $(patsubst %-params,%,$(notdir $(shell xt-xcc --show-config=core)))
CONFIGDIR   = $(shell xt-xcc --show-config=config)

# For platform-specific commands
include $(CONFIGDIR)/misc/hostenv.mk

# Target-specific options
ifeq ($(CONFIG_SIM_PLATFORM),y)
DFLAGS      += -DXT_SIMULATOR
else
DFLAGS      += -DXT_BOARD
endif

# Select LSP
LSP         = $(srctree)/arch/$(CONFIG_ARCH_PLATFORM)/lsp/$(CONFIG_CORE_ID)/$(CONFIG_LSP_DIR)

# Build options. Override CFLAGS from the make command line if desired.
# NOTE: Default LDFLAGS are set up to discard unused sections so that
# the final exe size is minimized.
INCS       += -Iprojects/$(CONFIG_PROJECT_DIR)/$(CONFIG_CORE_ID)/src -Iprojects/$(CONFIG_PROJECT_DIR)/$(CONFIG_CORE_ID)/include
INCS       += -Iinclude -Iinclude/hal -Iinclude/hal/sound -Iinclude/osal
INCS       += -Idrivers/hal/source
INCS       += -Icomponents/thirdparty/freertos
INCS       += -Ikernel/$(CONFIG_KERNEL_VERSION_DIR)/include -Ikernel/$(CONFIG_KERNEL_VERSION_DIR)/include/private
INCS       += -Ikernel/$(CONFIG_KERNEL_VERSION_DIR)/FreeRTOS/portable/XCC/Xtensa
INCS       += -Iarch/include -Iarch/$(CONFIG_ARCH_PLATFORM)/include/
DFLAGS     += -DSTANDALONE=1 -DXTUTIL_NO_OVERRIDE
ifeq ($(CONFIG_PROJECT_SIM_SMALL),y)
DFLAGS     += -DSMALL_TEST
endif
ifeq ($(CONFIG_FPGA_PLATFORM),y)
DFLAGS     += -DXT_CLOCK_FREQ=24000000
endif
ifeq ($(CONFIG_XCC_O3),y)
CFLAGS     += -$(CONFIG_XCC_OPT) -g -Wall
else
CFLAGS     += -$(CONFIG_XCC_OPT) -Os -g -Wall
endif
CFLAGS     += -ffunction-sections -fdata-sections -mlongcalls $(DFLAGS) $(INCS)
CFLAGS     += -I$(srctree)/include/generated -include autoconf.h -include map_func.h
LDFLAGS    += -Wl,--gc-sections
#LDFLAGS    += -lm -lxtutil,-Map=system.map
LDFLAGS    += -lm -lxtutil -Wl,-Map=$(CONFIG_CORE_ID).map

MAKEFLAGS += --no-print-directory

#export xt-xcc params for XtDevTools use
export CC AS AR LD
export CFLAGS

.PHONY : projects kernel FORCE menuconfig $(FORCE)

target_out = $(srctree)/out/$(CONFIG_PROJECT_DIR)
target_name = $(CONFIG_CORE_ID)
target_xcc_name = $(CONFIG_CORE_ID)_xcc

all: $(target_name).elf

sub_dir    := projects kernel drivers components arch

prepare += include/generated/autoconf.h include/reent.h
prepare += include/generated/version.h
ifeq ($(CONFIG_COMPONENTS_XTENSA_XTOS),y)
prepare += xtensa_xtos
endif
ifeq ($(CONFIG_COMPONENTS_XTENSA_LIBSIM),y)
prepare += xtensa_libsim
endif
ifeq ($(CONFIG_COMPONENTS_XTENSA_LIBGLOSS),y)
prepare += xtensa_libgloss
endif

$(addprefix $(OUTPUT),$(addsuffix /obj-in.o,$(sub_dir))): $(prepare) FORCE
	$(Q)$(MAKE) -f scripts/build/Makefile.build dir=$(notdir $(patsubst %/,%,$(dir $@)))

xtensa_xtos: FORCE
	$(Q)$(MAKE) -C components/thirdparty/xtensa/xtos -f Makefile.src \
		MAKEFILE_SRC=Makefile.src \
		INSTLIBDIR=$(LSP)
	$(Q)$(MAKE) -C components/thirdparty/xtensa/xtos -f Makefile.src install \
		MAKEFILE_SRC=Makefile.src \
		INSTLIBDIR=$(LSP)

xtensa_libsim: FORCE
	$(Q)$(MAKE) -C components/thirdparty/xtensa/sim -f Makefile.src \
		MAKEFILE_SRC=Makefile.src \
		INSTLIBDIR=$(LSP)
	$(Q)$(MAKE) -C components/thirdparty/xtensa/sim -f Makefile.src install \
		MAKEFILE_SRC=Makefile.src \
		INSTLIBDIR=$(LSP)

xtensa_libgloss: FORCE
	$(Q)$(MAKE) -C components/thirdparty/xtensa/libgloss -f Makefile.src \
		MAKEFILE_SRC=Makefile.src \
		INSTLIBDIR=$(LSP)
	$(Q)$(MAKE) -C components/thirdparty/xtensa/libgloss -f Makefile.src install \
		MAKEFILE_SRC=Makefile.src \
		INSTLIBDIR=$(LSP)

clean:
	$(Q)echo CLEAN objects
	$(Q)rm -rf $(target_out)
	$(Q)find -L ./ -path './XtDevTools*' -prune -o -name '*.o' -exec rm {} +
	$(Q)find -L ./ -path './XtDevTools*' -prune -o -name '.*.cmd' -exec rm {} +
	$(Q)find -L ./ -path './XtDevTools*' -prune -o -name '.*.d' -exec rm {} +
	$(Q)find -L ./ -path './XtDevTools*' -prune -o -name '*.tmp' -exec rm {} +
	$(Q)find -L ./ \( -path './XtDevTools*' -o -path './out' \) -prune -o -name '*.elf' -exec rm {} +
	$(Q)find -L ./ \( -path './XtDevTools*' -o -path './out' \) -prune -o -name '*.bin' -exec rm {} +
	$(Q)find -L ./ \( -path './XtDevTools*' -o -path './out' \) -prune -o -name '*.map' -exec rm {} +
	$(Q)find -path './include/reent.h' -delete
	$(Q)find -path './include/generated/*' -delete
	$(Q)find -path './include/config/*' -delete

# create elf
$(target_name).elf : $(addsuffix /obj-in.o,$(addprefix $(OUTPUT),$(sub_dir))) FORCE
	$(CC) $(CFLAGS) $(INCS) -o $@ $(addsuffix /obj-in.o,$(addprefix $(OUTPUT),$(sub_dir))) -mlsp=$(LSP) $(LDFLAGS) -DSTANDALONE=1

# Support for pack bin
pack : FORCE
	$(Q)echo -e "============= start pack for dsp bin ============="
	$(Q)if [ -f "$(srctree)/$(target_name).elf" ]; then \
		$(OBJS) $(srctree)/$(target_name).elf -o $(srctree)/$(target_xcc_name).bin; \
		./pack/dsp_pack -S $(target_xcc_name).bin -D $(target_name).bin; \
		mkdir -p $(target_out);\
		mv $(target_xcc_name).bin $(target_out);\
		mv $(target_name).bin $(target_out);\
		mv $(target_name).elf $(target_out);\
		mv $(CONFIG_CORE_ID).map $(target_out);\
		echo -e '\033[0;31;1m';\
		echo -e "$(target_out)/$(target_name).bin";\
		echo -e '\033[0m';\
		echo -e "\n#### make completed successfully ####\n"; \
	else \
		echo -e "ERROR: $(srctree)/$(target_name).elf does not exist\n"; \
	fi

# Support for menuconfig
menuconfig : FORCE
	@if [ -z "$(CONFIG_PROJECT_DIR)" ]; then \
		echo "$(CONFIG_PROJECT_DIR) not specified."; \
		echo "You can run 'make CONFIG_PROJECT_DIR=xxx menuconfig' to use the defconfig of a specific project."; \
		scripts/mconf Kconfig; \
	elif [ ! -e "projects/$(CONFIG_PROJECT_DIR)/$(CONFIG_CORE_ID)/defconfig" ]; then \
		echo "WARNING: project '$(CONFIG_PROJECT_DIR)/$(CONFIG_CORE_ID)/': defconfig NOT exists."; \
		scripts/mconf Kconfig; \
	else \
		cp projects/$(CONFIG_PROJECT_DIR)/$(CONFIG_CORE_ID)/defconfig .config; \
		scripts/mconf Kconfig; \
		cp .config projects/$(CONFIG_PROJECT_DIR)/$(CONFIG_CORE_ID)/defconfig; \
	fi

#create head file
include/reent.h:
	-$(CP) $(subst /,$(S),$(CONFIGDIR)/xtensa-elf/include/sys/reent.h) $(subst /,$(S),./include/reent.h)

include/generated/autoconf.h:.config
	$(Q)mkdir -p include/generated
	$(Q)mkdir -p include/config
	$(Q)scripts/conf --silentoldconfig Kconfig

include/generated/version.h:FORCE
	$(Q)$(call filechk,version)

$(notdir $(wildcard $(subst $\",, arch/$(CONFIG_ARCH_PLATFORM)/configs/*_defconfig))): FORCE
	$(Q)scripts/conf --defconfig=arch/$(CONFIG_ARCH_PLATFORM)/configs/$@ Kconfig

define filechk
	$(Q)set -e;				\
	echo '  CHK     $@';		\
	mkdir -p $(dir $@);			\
	$(filechk_$(1)) > $@.tmp;		\
	if [ -r $@ ] && cmp -s $@ $@.tmp; then	\
		rm -f $@.tmp;			\
	else					\
		echo '  UPD     $@';	\
		mv -f $@.tmp $@;		\
	fi
endef

define filechk_version
(echo "/*";\
	echo " * THIS IS CREATE WITH VERSION CHK";\
	echo " * DO NOT CHANGE IT MANUAL";\
	echo " */";\
	echo ;\
	echo "#ifndef _SUB_VER_";\
	echo "#define _SUB_VER_";\
	echo ;\
	echo "#define SUB_VER \"`scripts/setlocalversion`\"";\
	echo ;\
	echo "#endif /* _SUB_VER_ */";)
endef


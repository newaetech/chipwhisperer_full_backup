#
# Copyright (c) 2016, Intel Corporation
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice,
# this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright notice,
# this list of conditions and the following disclaimer in the documentation
# and/or other materials provided with the distribution.
# 3. Neither the name of the Intel Corporation nor the names of its
# contributors may be used to endorse or promote products derived from this
# software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE INTEL CORPORATION OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
#

# This file defines the 'rom' target, to build the first stage bootloader
# (a.k.a. the ROM).

# Add common bootstrap objects to the list of ROM objects
include $(BL_BASE_DIR)/bootstrap/boot.mk
ROM_OBJS += $(BOOT_OBJS)
# Add soc-specific bootstrap objects to the list of ROM objects
include $(BL_BASE_DIR)/bootstrap/soc/boot_soc.mk
ROM_OBJS += $(BOOT_SOC_OBJS)

### Variables
# The directory where the ROM binary will be put
ROM_BUILD_DIR = $(BUILD_DIR)/$(BUILD)/$(SOC)/rom
OBJ_DIRS += $(ROM_BUILD_DIR)

# The preliminary ROM binary (the final one will be created by the makeRomImage
# python script, starting from this one)
STARTUP_BIN = $(BOOT_SOC_DIR)/$(BUILD)/$(OBJ)/rom_startup.bin

ROM_LINKER_FILE ?= $(BOOT_SOC_DIR)/rom.ld

# TODO: create a soc-specific mk for this or a centralized config.mk. This will
# be responsible for 1) documenting all possible build options, 2) doing the
# "contract checking" (i.e. if quark_d2000 and ENABLE_RESTORE_CONTEXT, fail),
# 3) expanding the CFLAGS that result from the build-time options.
ifeq ($(SOC),quark_se)
  ENABLE_RESTORE_CONTEXT ?= 1
  ifeq ($(ENABLE_RESTORE_CONTEXT),0)
  CFLAGS += -DENABLE_RESTORE_CONTEXT=0
  ROM_SUFFIX_NO_RESTORE_CONTEXT = _no_restore_context
  else
  CFLAGS += -DENABLE_RESTORE_CONTEXT=1
  endif
endif

# Always include fw-manager (FM) makefile to ensure that
# clean/realclean/distclean works properly, i.e., also delete FM objects and
# build directories (since FM object dirs are added to OBJ_DIRS and
# GENERATED_DIRS).
include $(BL_BASE_DIR)/fw-manager/fw-manager.mk

# Compile FM code (i.e., add FM objects as rom dependencies) only if
# ENABLE_FIRMWARE_MANAGER=1.
ifneq ($(ENABLE_FIRMWARE_MANAGER),none)
  ### Enable FM mode
  ROM_OBJS += $(FM_OBJS)
  CFLAGS += -DENABLE_FIRMWARE_MANAGER=1
  # Rom file name will have a '_fm' suffix
  ifeq ($(ENABLE_FIRMWARE_MANAGER),uart)
  CFLAGS += -DENABLE_FIRMWARE_MANAGER_UART=1
  ROM_SUFFIX_FM = _fm
  endif
  ifeq ($(ENABLE_FIRMWARE_MANAGER),2nd-stage)
  CFLAGS += -DENABLE_FIRMWARE_MANAGER_2ND_STAGE=1
  CFLAGS += -DBL_HAS_2ND_STAGE=1
  ROM_SUFFIX_FM = _fm_2nd_stage
  endif
endif

# Define ROM file name
# (Suffix is built on multiple lines to respect 80 chars limit)
ROM_SUFFIX := $(ROM_SUFFIX_FM)
ROM_SUFFIX := $(ROM_SUFFIX)$(ROM_SUFFIX_NO_RESTORE_CONTEXT)
ROM = $(ROM_BUILD_DIR)/$(SOC)_rom$(ROM_SUFFIX).bin

.PHONY: rom

rom: $(ROM)

### Make 8kB ROM image
$(ROM): $(STARTUP_BIN)
	$(call mkdir, $(ROM_BUILD_DIR))
	python $(BOOT_SOC_DIR)/makeRomImage.py $(STARTUP_BIN) $(ROM)

### Link STARTUP.elf and get raw binary
$(STARTUP_BIN): $(ROM_OBJS) qmsi
	$(LD) $(LDFLAGS) -Xlinker -T$(ROM_LINKER_FILE) \
		-Xlinker -A$(OUTPUT_ARCH) \
		-Xlinker --oformat$(OUTPUT_FORMAT) \
		-Xlinker -Map=$(STARTUP_BIN).map \
		-o $(STARTUP_BIN).elf $(ROM_OBJS) $(LDLIBS)
	$(SIZE) $(STARTUP_BIN).elf
	$(OBJCOPY) --gap-fill 0xFF -O binary $(STARTUP_BIN).elf $@


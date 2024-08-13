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

# This file is supposed to be included by other Makefiles (typically at the
# beginning).
# It set common variables, validates build parameters, and define common
# clean-up targets.

### Helper functions

# list_to_str: a function for turning makefile list variables into
# comma-separated list with elements in single quotation marks.
# Example:
# LIST = a b
# $(call list_to_str, $(LIST)) --> 'a', 'b'
null  :=
space := $(null) $(null)
separator := ', '
list_to_str = $(subst $(space),$(separator),$(strip $(1)))

### Environment checks
ifeq ($(BL_BASE_DIR),)
$(error BL_BASE_DIR is not defined)
endif

### Parameter Check
DEFAULT_SOC = quark_d2000
SOC ?= $(DEFAULT_SOC)
$(info SOC = $(SOC))
SUPPORTED_SOCS = quark_se \
                 quark_d2000
ifeq ($(filter $(SOC),$(SUPPORTED_SOCS)),)
$(error SOC=$(SOC) is not supported.)
endif
QMSI_BUILD_OPTIONS += SOC=$(SOC)

TARGET ?= x86
ifneq ($(TARGET),x86)
$(error 'x86' is the only TARGET supported by the bootloader.)
endif
QMSI_BUILD_OPTIONS += TARGET=$(TARGET)

BUILD ?= release
$(info BUILD = $(BUILD))
SUPPORTED_BUILDS = debug \
                   release
ifeq ($(filter $(BUILD),$(SUPPORTED_BUILDS)),)
$(error Supported BUILD values are: $(SUPPORTED_BUILDS).)
endif
QMSI_BUILD_OPTIONS += BUILD=$(BUILD)

CSTD ?= c99
$(info CSTD = $(CSTD))
SUPPORTED_CSTD = c99 \
		 c90
ifeq ($(filter $(CSTD),$(SUPPORTED_CSTD)),)
$(error Supported CSTD values are 'c99' and 'c90'.)
endif
QMSI_BUILD_OPTIONS += CSTD=$(CSTD)

ENABLE_FIRMWARE_MANAGER ?= none
$(info ENABLE_FIRMWARE_MANAGER = $(ENABLE_FIRMWARE_MANAGER))
SUPPORTED_FM_MODE_quark_se = none \
			     uart \
			     2nd-stage
SUPPORTED_FM_MODE_quark_d2000 = none \
				uart
SUPPORTED_FM_MODE = $(SUPPORTED_FM_MODE_$(SOC))
SUPPORTED_FM_MODE_STR = $(call list_to_str, $(SUPPORTED_FM_MODE))
ifeq ($(filter $(ENABLE_FIRMWARE_MANAGER),$(SUPPORTED_FM_MODE)),)
  $(error Supported ENABLE_FIRMWARE_MANAGER values are: \
        '$(SUPPORTED_FM_MODE_STR)'; given value is '$(ENABLE_FIRMWARE_MANAGER)')
endif
ifeq ($(ENABLE_FIRMWARE_MANAGER),uart)
ifeq ($(BUILD),debug)
$(error "Cannot combine (first-stage) Firmware Management over UART with \
	 debug build due to footprint constraints.")
endif
endif

# TODO: move to a soc-specific mk
ifeq ($(SOC),quark_se)
    # Option to enable context sleep
    ENABLE_RESTORE_CONTEXT ?= 1
    SUPPORTED_ENABLE_RESTORE_CONTEXT = 0 \
		                       1
    $(info ENABLE_RESTORE_CONTEXT = $(ENABLE_RESTORE_CONTEXT))
    ifeq ($(filter $(ENABLE_RESTORE_CONTEXT),\
	    $(SUPPORTED_ENABLE_RESTORE_CONTEXT)),)
        $(error Supported ENABLE_RESTORE_CONTEXT values are '0' and '1'.)
    endif
    QMSI_BUILD_OPTIONS += ENABLE_RESTORE_CONTEXT=$(ENABLE_RESTORE_CONTEXT)
endif

# Special parameters for custom-boards (not mentioned in the help)
BOARD_HAS_RTC_XTAL ?= 1
SUPPORTED_BOARD_HAS_RTC_XTAL = 0 \
			       1
ifeq ($(filter $(BOARD_HAS_RTC_XTAL),$(SUPPORTED_BOARD_HAS_RTC_XTAL)),)
$(error Supported BOARD_HAS_RTC_XTAL values are '0' and '1'.)
endif

BOARD_HAS_HYB_XTAL ?= 1
SUPPORTED_BOARD_HAS_HYB_XTAL = 0 \
			       1
ifeq ($(filter $(BOARD_HAS_HYB_XTAL),$(SUPPORTED_BOARD_HAS_HYB_XTAL)),)
$(error Supported BOARD_HAS_HYB_XTAL values are '0' and '1'.)
endif

# Build verbosity level
V ?= 0
SUPPORTED_VERBOSITY = 0 \
		      1
ifeq ($(filter $(V),$(SUPPORTED_VERBOSITY)),)
$(error Supported V values are '0' and '1')
endif

### Tools
PREFIX ?= i586-intel-elfiamcu
TOOLCHAIN_DIR=$(IAMCU_TOOLCHAIN_DIR)

ifeq ($(TOOLCHAIN_DIR),)
$(info Toolchain path is not defined. Please run:)
$(info export IAMCU_TOOLCHAIN_DIR=<TOOLCHAIN_PATH>)
$(error IAMCU_TOOLCHAIN_DIR is not defined)
endif

### OS specific
ifeq ($(OS),Windows_NT)
# Windows variants
export PATH := $(TOOLCHAIN_DIR);$(PATH)
OSNAME := $(shell wmic os get name)
# 'more' has to be used for a small file
CAT := more
END_CMD := &
ifneq (,$(findstring Microsoft Windows Server, $(OSNAME)))
# Windows Server
mkdir = @mkdir -p $(1) || exit 0
copy = @cp $(1) $(2) || exit 0
else
# Any other version of Windows
mkdir = @md $(subst /,\,$(1)) > nul 2>&1 || exit 0
copy = @copy $(subst /,\,$(1)) $(subst /,\,$(2)) > nul 2>&1 || exit 0
endif
else
# Unix variants
export PATH := $(TOOLCHAIN_DIR):$(PATH)
mkdir = @mkdir -p $(1)
copy = @cp $(1) $(2)
CAT := cat
END_CMD := ;
endif

SIZE_0 = @echo "Size $@" && $(PREFIX)-size
SIZE_1 = $(PREFIX)-size
SIZE = $(SIZE_$(V))

OBJCOPY_0 = @echo "Objcopy $@" && $(PREFIX)-objcopy
OBJCOPY_1 = $(PREFIX)-objcopy
OBJCOPY = $(OBJCOPY_$(V))

AR_0 = @echo "AR $@" && $(PREFIX)-ar
AR_1 = $(PREFIX)-ar
AR = $(AR_$(V))

CC_0 = @echo "CC $@" && $(PREFIX)-gcc
CC_1 = $(PREFIX)-gcc
CC = $(CC_$(V))

LD_0 = @echo "LD $@" && $(PREFIX)-gcc
LD_1 = $(PREFIX)-gcc
LD = $(LD_$(V))

LN_0 = @ln
LN_1 = ln
LN = $(LN_$(V))

### Variables
BIN = bin
OBJ = obj
BUILD_DIR = $(BL_BASE_DIR)/build

### QMSI Section
LIBNAME=qmsi
LIBQMSI_DIR = $(QMSI_SRC_DIR)/build/$(BUILD)/$(SOC)/$(TARGET)/lib$(LIBNAME)
LIBQMSI_LIB_DIR = $(LIBQMSI_DIR)/lib
LIBQMSI_INCLUDE_DIR = $(LIBQMSI_DIR)/include

CFLAGS += -I$(LIBQMSI_DIR)/include
# FIXME: the following cflag is needed in order to have accesses to the header
# files in qmsi/driver/interrupt (e.g, mvic.h) which are needed to build the
# bootstrap code.
# We MUST remove this dependency on the QMSI source code.
CFLAGS += -I$(QMSI_SRC_DIR)/drivers/

LDLIBS += -l$(LIBNAME)
LDFLAGS += -L$(LIBQMSI_LIB_DIR)

### Flags
CFLAGS += -Wall -Wextra -Werror
CFLAGS += -fmessage-length=0
CFLAGS += -fno-asynchronous-unwind-tables
CFLAGS += -DHAS_RTC_XTAL=$(BOARD_HAS_RTC_XTAL)
CFLAGS += -DHAS_HYB_XTAL=$(BOARD_HAS_HYB_XTAL)
CFLAGS += -ffunction-sections -fdata-sections
CFLAGS += -DQM_LAKEMONT
CFLAGS += -march=lakemont -mtune=lakemont -miamcu -msoft-float
LDFLAGS += -nostdlib
LDLIBS += -lc -lnosys -lsoftfp -lgcc
LDFLAGS += -Xlinker --gc-sections

ifeq ($(BUILD), debug)
CFLAGS += -O0 -g -DDEBUG
else # release
CFLAGS += -Os -fomit-frame-pointer
endif

ifeq ($(CSTD), c99)
CFLAGS += -std=c99
else # c90
CFLAGS += -std=c90
endif

.PHONY: all clean realclean

### Define 'all' here so it will be the default make rule
all:

### Clean up
### 1) Remove the specified BUILD/SOC/TARGET directory.
clean::
	$(RM) -r $(OBJ_DIRS) $(BUILD_DIR)/$(BUILD)/$(SOC)/$(TARGET)

realclean::
	$(RM) -r $(GENERATED_DIRS) $(BUILD_DIR)

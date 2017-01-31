#
# Copyright (c) 2016, Intel Corporation
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice,
#    this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
# 3. Neither the name of the Intel Corporation nor the names of its
#    contributors may be used to endorse or promote products derived from this
#    software without specific prior written permission.
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

### Tools
ifeq ($(TARGET), sensor)
PREFIX ?= arc-elf32
TOOLCHAIN_DIR=$(ARCMCU_TOOLCHAIN_DIR)
else
PREFIX ?= i586-intel-elfiamcu
TOOLCHAIN_DIR=$(IAMCU_TOOLCHAIN_DIR)
endif

ifeq ($(TOOLCHAIN_DIR),)
$(info Toolchain path is not defined. Please run:)
ifeq ($(TARGET), sensor)
$(info export ARCMCU_TOOLCHAIN_DIR=<TOOLCHAIN_PATH>)
$(error ARCMCU_TOOLCHAIN_DIR is not defined)
else
$(info export IAMCU_TOOLCHAIN_DIR=<TOOLCHAIN_PATH>)
$(error IAMCU_TOOLCHAIN_DIR is not defined)
endif
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

### Build verbosity level
V ?= 0
ifeq ($(V), 0)
else ifeq ($(V), 1)
else
$(error Supported V values are '0' and '1')
endif

SIZE_0 = @echo "Size $@" && $(PREFIX)-size
SIZE_1 = $(PREFIX)-size
SIZE = $(SIZE_$(V))

OBJCOPY_0 = @echo "Objcopy $@" && $(PREFIX)-objcopy
OBJCOPY_1 = $(PREFIX)-objcopy
OBJCOPY = $(OBJCOPY_$(V))

# gcc-ar seems to be broken with arc toolchain, but gcc-arc is only needed if
# BUILD is set to lto.
ifeq ($(TARGET) , sensor)
AR_0 = @echo "AR $@" && $(PREFIX)-ar
AR_1 = $(PREFIX)-ar
# lto currently doesn't work with arc target.
ifeq ($(BUILD), lto)
$(error Sensor TARGET doesn't support lto BUILD option)
endif
else
AR_0 = @echo "AR $@" && $(PREFIX)-gcc-ar
AR_1 = $(PREFIX)-gcc-ar
endif
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

### Environment checks
ifeq ($(BASE_DIR),)
$(error BASE_DIR is not defined)
endif

### Variables
ITA_NO_ASSERT ?= 0
BUILD ?= release
CFLAGS += -ffunction-sections -fdata-sections
LDFLAGS += -Xlinker --gc-sections
ifeq ($(BUILD), debug)
CFLAGS += -O0 -g -DDEBUG
ifeq ($(ITA_NO_ASSERT), 1)
CFLAGS += -DITA_NO_ASSERT
$(info Asserts will be switched off in debug mode due to code size limitation)
endif
else ifeq ($(BUILD), release)
CFLAGS += -Os -fomit-frame-pointer
else ifeq ($(BUILD), lto)
CFLAGS += -Os -fomit-frame-pointer -flto
LDFLAGS += -flto
else
# In addition of debug and release, BUILD also support lto, but it's a hidden
# option for now.
$(error Supported BUILD values are 'release' and 'debug')
endif
$(info BUILD = $(BUILD))

CSTD ?= c99
ifeq ($(CSTD), c99)
CFLAGS += -std=c99
else ifeq ($(CSTD), c90)
CFLAGS += -std=c90
else
$(error Supported C standards are 'c99' and 'c90')
endif
$(info CSTD = $(CSTD))

SOC_WATCH_ENABLE ?= 0
ifeq ($(SOC_WATCH_ENABLE), 1)
CFLAGS += -DSOC_WATCH_ENABLE=1
else ifneq ($(SOC_WATCH_ENABLE), 0)
$(error Supported SOC_WATCH_ENABLE values are '1' and '0')
endif

BIN = bin
OBJ = obj

LIBNAME=qmsi
BUILD_DIR = $(BASE_DIR)/build
LIBQMSI_DIR = $(BUILD_DIR)/$(BUILD)/$(SOC)/$(TARGET)/lib$(LIBNAME)
LIBQMSI_LIB_DIR = $(LIBQMSI_DIR)/lib
LIBQMSI_INCLUDE_DIR = $(LIBQMSI_DIR)/include

ifeq ($(OS),Windows_NT)
VERSION_FILE := $(subst /,\,$(BASE_DIR)/VERSION)
else
VERSION_FILE := $(BASE_DIR)/VERSION
endif

VERSIONS := $(shell $(CAT) $(VERSION_FILE))

QM_VER_API := $(subst QMSI_API=,,$(filter QMSI_API=%,$(VERSIONS)))
QM_VER_API_MAJOR := $(word 1,$(subst ., ,$(QM_VER_API)))
QM_VER_API_MINOR := $(word 2,$(subst ., ,$(QM_VER_API)))
QM_VER_API_PATCH := $(word 3,$(subst ., ,$(QM_VER_API)))

$(info VERSION = '$(QM_VER_API)')

ifeq ($(BUILD), debug)
LIBQMSI_FILENAME = lib$(LIBNAME)_$(SOC)_$(QM_VER_API)d.a
LDLIBS_FILENAME = $(LIBNAME)_$(SOC)_$(QM_VER_API)d
else ifneq (, $(filter $(BUILD), release lto))
LIBQMSI_FILENAME = lib$(LIBNAME)_$(SOC)_$(QM_VER_API).a
LDLIBS_FILENAME = $(LIBNAME)_$(SOC)_$(QM_VER_API)
endif

HAS_RTC_XTAL := 1
HAS_HYB_XTAL := 1

### Flags
CFLAGS += -Wall -Wextra -Werror
CFLAGS += -fmessage-length=0
CFLAGS += -I$(BASE_DIR)/include
CFLAGS += -fno-asynchronous-unwind-tables
CFLAGS += -DHAS_RTC_XTAL=$(HAS_RTC_XTAL) -DHAS_HYB_XTAL=$(HAS_HYB_XTAL)
CFLAGS += -DQM_VER_API_MAJOR=$(QM_VER_API_MAJOR) \
	  -DQM_VER_API_MINOR=$(QM_VER_API_MINOR) \
	  -DQM_VER_API_PATCH=$(QM_VER_API_PATCH)
LDFLAGS += -nostdlib

STDOUT_UART_INIT ?= enable
ifeq ($(STDOUT_UART_INIT), disable)
CFLAGS += -DSTDOUT_UART_INIT_DISABLE
endif

ifeq ($(TARGET), sensor)
CFLAGS += -DQM_SENSOR
CFLAGS += -ffreestanding
CFLAGS += -mARCv2EM -mav2em -mno-sdata
LDFLAGS += -nostartfiles
LDLIBS += -lc -lgcc
else
LDLIBS += -lc -lnosys -lsoftfp -lgcc
endif

ifeq ($(TARGET), x86)
CFLAGS += -DQM_LAKEMONT
endif

### If interrupt handling is done externally, like in Zephyr.
ifeq ($(ENABLE_EXTERNAL_ISR_HANDLING), 1)
CFLAGS += -DENABLE_EXTERNAL_ISR_HANDLING
endif

.PHONY: all clean realclean

all: $(APP)

### Clean up
### 1) Remove the specified BUILD/SOC/TARGET directory.
clean::
	$(RM) -r $(OBJ_DIRS) $(BUILD_DIR)/$(BUILD)/$(SOC)/$(TARGET)

realclean::
	$(RM) -r $(GENERATED_DIRS) $(BUILD_DIR)

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

### Make all examples makefile

DEFAULT_SOC = quark_d2000
DEFAULT_TARGET = x86

SOC ?= $(DEFAULT_SOC)
TARGET ?= $(DEFAULT_TARGET)
BUILD ?= release

SUBDIRS := $(filter %/, $(wildcard ./*/))
SUBDIRS := $(filter-out ./include/, ${SUBDIRS})

ifeq ($(SOC), quark_se)
SUBDIRS := $(subst ./adc/,,${SUBDIRS})
SUBDIRS := $(subst ./adc_continuous/,,${SUBDIRS})
SUBDIRS := $(subst ./magneto/,,${SUBDIRS})
SUBDIRS := $(subst ./quark_d2000/,,${SUBDIRS})
ifeq ($(TARGET), x86)
#### Excluding the non supported modules
SUBDIRS := $(subst ./sensor/,,${SUBDIRS})
SUBDIRS := $(subst ./gyro/,,${SUBDIRS})
SUBDIRS := $(subst ./accel/,,${SUBDIRS})
else ifeq ($(TARGET), sensor)
#### Excluding the non supported modules
SUBDIRS := $(subst ./pic_timer/,,${SUBDIRS})
SUBDIRS := $(subst ./crypto/,,${SUBDIRS})
SUBDIRS := $(subst ./quark_se/,,${SUBDIRS})
SUBDIRS := $(subst ./power_profiler/,,${SUBDIRS})
SUBDIRS := $(subst ./irq_lock/,,${SUBDIRS})
endif
else ifeq ($(SOC), quark_d2000)
#### Excluding the non supported modules
SUBDIRS := $(subst ./aon_gpio/,,${SUBDIRS})
SUBDIRS := $(subst ./quark_se/,,${SUBDIRS})
SUBDIRS := $(subst ./sensor/,,${SUBDIRS})
SUBDIRS := $(subst ./mailbox/,,${SUBDIRS})
SUBDIRS := $(subst ./gyro/,,${SUBDIRS})
endif

all clean realclean:: $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) SOC=$(SOC) TARGET=$(TARGET) BUILD=$(BUILD) -C $@ $(MAKECMDGOALS)

.PHONY: $(SUBDIRS)

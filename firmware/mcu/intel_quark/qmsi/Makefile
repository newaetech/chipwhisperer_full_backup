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

DEFAULT_SOC = quark_d2000
DEFAULT_TARGET = x86
SUPPORTED_SOCS = quark_se \
                 quark_d2000

SUPPORTED_BUILDS = debug \
                   release \
		   lto

SOC ?= $(DEFAULT_SOC)
TARGET ?= $(DEFAULT_TARGET)

### Define supported targets for each SoCs
SUPPORTED_TARGETS_quark_d2000 = x86
SUPPORTED_TARGETS_quark_se = x86 \
                             sensor

ifeq ($(filter $(SOC),$(SUPPORTED_SOCS)),)
$(error SOC=$(SOC) is not supported. Run 'make help' for help)
endif

### $(SOC) is expanded here to match the correct variable
SUPPORTED_TARGETS = $(SUPPORTED_TARGETS_$(SOC))

ifeq ($(filter $(TARGET),$(SUPPORTED_TARGETS)),)
$(error TARGET=$(TARGET) is not supported for $(SOC). Run 'make help' for help)
endif

RULES = libqmsi

BASE_DIR = .

include $(BASE_DIR)/base.mk
include $(BASE_DIR)/soc/$(SOC)/$(SOC).mk
include $(BASE_DIR)/drivers/libqmsi.mk

.PHONY: help targets distclean

targets:
	$(info List of supported values for $(SOC): $(SUPPORTED_TARGETS))

help:
	$(info )
	$(info List of build targets. By default all targets are built.)
	$(info libqmsi  - Build the libqmsi package)
	$(info targets  - List the targets available for SOC)
	$(info )
	$(info List of clean targets.)
	$(info clean     - Clean specified TARGET for the given SOC)
	$(info distclean - Clean all generated files)
	$(info )
	$(info By default SOC=$(DEFAULT_SOC).)
	$(info By default TARGET=$(DEFAULT_TARGET).)
	$(info )
	$(info Verbosity of Make is controlled by V=0 / 1)
	$(info By default V=0.)
	$(info )
	$(info stdout UART initialisation STDOUT_UART_INIT=enable / disable)
	$(info By default STDOUT_UART_INIT=enable.)
	$(info )
	$(info List of supported values for SOC: $(SUPPORTED_SOCS))
	$(info List of supported values for CSTD: c90 and c99)

all: $(RULES)

distclean:
	$(MAKE) -C doc/ clean
	$(foreach soc, $(SUPPORTED_SOCS),\
	$(foreach target, $(SUPPORTED_TARGETS_$(soc)),\
	$(foreach build, $(SUPPORTED_BUILDS),\
		$(MAKE) SOC=$(soc) TARGET=$(target) BUILD=$(build)\
			realclean $(END_CMD))))
	$(foreach soc, $(SUPPORTED_SOCS),\
	$(foreach target, $(SUPPORTED_TARGETS_$(soc)),\
	$(foreach build, $(SUPPORTED_BUILDS),\
		$(MAKE) SOC=$(soc) TARGET=$(target) BUILD=$(build) -C examples\
			realclean $(END_CMD))))

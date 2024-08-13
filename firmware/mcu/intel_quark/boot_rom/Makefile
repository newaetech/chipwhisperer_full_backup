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

BL_BASE_DIR = .
# The user is expected to define the QMSI_SRC_DIR environment variable and make
# it point to the QMSI source directory.
# If not, QMSI source directory is supposed to be a sibling of the bootloader
# source directory.
# TODO: allow for the optional use of a pre-compiled QMSI
QMSI_SRC_DIR ?= $(BL_BASE_DIR)/../qmsi


BOARD_HAS_RTC_XTAL=0
BOARD_HAS_HYB_XTAL=0

include $(BL_BASE_DIR)/base.mk
include $(BL_BASE_DIR)/rom.mk

.PHONY: all help distclean

# Build rom by default (target defined in rom.mk)
all: rom

help:
	$(info )
	$(info List of build targets.)
	$(info rom      - Build the ROM firmware (first stage booloader))
	$(info )
	$(info List of clean targets.)
	$(info clean     - Clean all generated files for the given SOC)
	$(info distclean - Clean all generated files for all SOCs)
	$(info )
	$(info By default SOC=$(DEFAULT_SOC).)
	$(info )
	$(info Verbosity of Make is controlled by V=0 / 1)
	$(info By default V=0.)
	$(info )
	$(info List of supported values for SOC: $(SUPPORTED_SOCS))
	$(info List of supported values for CSTD: c90 and c99)
	$(info )
	$(info To enable FW management (FM), use the ENABLE_FIRMWARE_MANAGER)
	$(info build option.)
	$(info ENABLE_FIRMWARE_MANAGER=uart enables FM over UART on the ROM.)
	$(info ENABLE_FIRMWARE_MANAGER=2nd-stage delegates FM to the 2nd-stage)
	$(info bootloader.)
	$(info By default, ENABLE_FIRMWARE_MANAGER=none)
	$(info )
	$(info To disable context saving on sleep for Quark SE, compile the ROM)
	$(info with ENABLE_RESTORE_CONTEXT=0)
	$(info By default, ENABLE_RESTORE_CONTEXT=1)
	$(info )
	$(info QMSI source code is needed for compilation. By default QMSI)
	$(info code is expected to be located in '../qmsi'.)
	$(info The QMSI_SRC_DIR variable can be defined to change it.)
	@true #suppress the "Nothing to be done for 'help'" error.

qmsi:
	make -C $(QMSI_SRC_DIR) libqmsi $(QMSI_BUILD_OPTIONS)

# distclean calls 'make realclean' for every soc and build type; realclean is
# defined in base.mk
distclean:
	$(foreach soc, $(SUPPORTED_SOCS),\
	$(foreach build, $(SUPPORTED_BUILDS),\
		$(MAKE) SOC=$(soc) BUILD=$(build) realclean \
		$(END_CMD)))

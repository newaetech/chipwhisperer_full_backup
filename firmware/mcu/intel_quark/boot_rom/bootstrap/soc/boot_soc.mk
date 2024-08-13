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

# Makefile building soc-specific bootstrap code.
#
# This file is expected to be included by other makefiles, which can trigger
# the compilation of the bootstrap code by setting BOOT_SOC_OBJS as a dependency
# for their rule(s).

# The soc-dependent bootstrap directory
BOOT_SOC_DIR = $(BOOT_DIR)/soc/$(SOC)
# The soc-dependent bootstrap code
BOOT_SOC_SOURCES = $(wildcard $(BOOT_SOC_DIR)/*.c)
# The soc-dependent bootstrap assembly code
BOOT_SOC_SOURCES_ASM = $(wildcard $(BOOT_SOC_DIR)/*.s)
# The object dir of the soc-specific bootstrap code
BOOT_SOC_OBJ_DIR = $(BOOT_SOC_DIR)/$(BUILD)/$(OBJ)
# The objects of the soc-specific bootstrap code
BOOT_SOC_OBJS = $(addprefix $(BOOT_SOC_OBJ_DIR)/, \
			    $(notdir $(BOOT_SOC_SOURCES:.c=.o)))
BOOT_SOC_OBJS += $(addprefix $(BOOT_SOC_OBJ_DIR)/, \
			    $(notdir $(BOOT_SOC_SOURCES_ASM:.s=.s.o)))

# Update variables for 'clean' and 'realclean'
OBJ_DIRS += $(BOOT_SOC_DIR)/$(BUILD)
GENERATED_DIRS += $(BOOT_SOC_DIR)/$(BUILD)

CFLAGS += -I$(BOOT_SOC_DIR)/include

### Build C files
$(BOOT_SOC_OBJ_DIR)/%.o: $(BOOT_SOC_DIR)/%.c qmsi
	$(call mkdir, $(BOOT_SOC_OBJ_DIR))
	$(CC) $(CFLAGS) -c -o $@ $<

### Build assembly files
$(BOOT_SOC_OBJ_DIR)/%.s.o: $(BOOT_SOC_DIR)/%.s
	$(call mkdir, $(BOOT_SOC_DIR)/$(BUILD)/$(OBJ))
	$(CC) $(CFLAGS) -xassembler-with-cpp -c -o $@ $<


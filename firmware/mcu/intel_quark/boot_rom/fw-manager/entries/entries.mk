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

FM_ENTRY_DIR = $(FM_DIR)/entries
OBJ_DIRS += $(FM_ENTRY_DIR)/$(BUILD)/$(SOC)
GENERATED_DIRS += $(FM_ENTRY_DIR)/$(BUILD)
FM_ENTRY_OBJ_DIR = $(FM_ENTRY_DIR)/$(BUILD)/$(SOC)/$(OBJ)

FM_ENTRY_SRCS = fm_entry_uart.c
# Additional fm_entries valid only for Quark SE
ifeq ($(SOC),quark_se)
FM_ENTRY_SRCS += fm_entry_usb.c fm_entry_2nd_stage.c
endif

FM_ENTRY_OBJS = $(addprefix $(FM_ENTRY_OBJ_DIR)/,\
		$(notdir $(FM_ENTRY_SRCS:.c=.o)))
FM_OBJS += $(FM_ENTRY_OBJS)

### Flags
CFLAGS += -I$(FM_ENTRY_DIR)
CFLAGS += -I$(QMSI_SRC_DIR)/usb/include

### Build C files
$(FM_ENTRY_OBJ_DIR)/%.o: $(FM_ENTRY_DIR)/%.c qmsi
	$(call mkdir, $(FM_ENTRY_OBJ_DIR))
	$(CC) $(CFLAGS) -c -o $@ $<

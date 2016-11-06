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

FM_DIR = $(BL_BASE_DIR)/fw-manager
OBJ_DIRS += $(FM_DIR)/$(BUILD)/$(SOC)
FM_SOURCES = $(wildcard $(FM_DIR)/*.c)
FM_OBJ_DIR = $(FM_DIR)/$(BUILD)/$(SOC)/$(OBJ)
FM_OBJS = $(addprefix $(FM_OBJ_DIR)/,$(notdir $(FM_SOURCES:.c=.o)))
GENERATED_DIRS += $(FM_DIR)/$(BUILD)

### Flags
CFLAGS += -I$(FM_DIR)

### Build C files
$(FM_OBJ_DIR)/%.o: $(FM_DIR)/%.c qmsi
	$(call mkdir, $(FM_OBJ_DIR))
	$(CC) $(CFLAGS) -c -o $@ $<

include $(FM_DIR)/entries/entries.mk
include $(FM_DIR)/dfu/dfu.mk
include $(FM_DIR)/qfm/qfm.mk
include $(FM_DIR)/qfu/qfu.mk

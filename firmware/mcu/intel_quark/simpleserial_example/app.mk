#
# Copyright (c) 2015, Intel Corporation
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

# Application template

### Environment checks
ifeq ($(APP_NAME),)
$(error APP_NAME is not defined)
endif
$(info APP_NAME = $(APP_NAME))

ifeq ($(SOC), quark_se)
else ifeq ($(SOC), quark_d2000)
else
$(error Supported SOC values are 'quark_se' and 'quark_d2000')
endif
$(info SOC = $(SOC))

ifeq ($(APP_DIR),)
$(error APP_DIR is not defined)
endif
$(info APP_DIR = $(APP_DIR))

### Variables
APP = $(APP_DIR)/$(BUILD)/$(SOC)/$(BIN)/$(APP_NAME).bin
OBJ_DIRS += $(APP_DIR)/$(BUILD)
SOURCES = $(wildcard $(APP_DIR)/*.c)
OBJECTS += $(addprefix $(APP_DIR)/$(BUILD)/$(SOC)/$(OBJ)/,$(notdir $(SOURCES:.c=.o)))
CFLAGS += -DPRINTF_ENABLE -DPUTS_ENABLE
CFLAGS += -Wa,-adhlns=$(<:%.c=$(APP_DIR)/$(BUILD)/$(SOC)/$(OBJ)/%.lst)

### Make includes
include $(BASE_DIR)/base.mk
include $(BASE_DIR)/sys/sys.mk
include $(BASE_DIR)/drivers/libqmsi.mk
include $(BASE_DIR)/soc/$(SOC)/$(SOC).mk
include $(BASE_DIR)/board/drivers.mk

### Build C files in APP_DIR
$(APP_DIR)/$(BUILD)/$(SOC)/$(OBJ)/%.o: $(APP_DIR)/%.c libqmsi
	$(call mkdir, $(APP_DIR)/$(BUILD)/$(SOC)/$(OBJ))
	$(CC) $(CFLAGS) -c -o $@ $<

### Link object files into APP ELF
$(APP): $(LINKER_FILE) $(OBJECTS) libqmsi
	$(call mkdir, $(APP_DIR)/$(BUILD)/$(SOC)/$(BIN))
	$(LD) $(LDFLAGS) -Xlinker -T$(LINKER_FILE) \
		-Xlinker -A$(OUTPUT_ARCH) \
		-Xlinker --oformat$(OUTPUT_FORMAT) \
		-Xlinker -Map=$(APP_DIR)/$(BUILD)/$(SOC)/$(OBJ)/$(APP_NAME).map \
		-o $(APP_DIR)/$(BUILD)/$(SOC)/$(OBJ)/$(APP_NAME).elf $(OBJECTS) $(LDLIBS)
	$(SIZE) $(APP_DIR)/$(BUILD)/$(SOC)/$(OBJ)/$(APP_NAME).elf
	$(OBJCOPY) -O binary $(APP_DIR)/$(BUILD)/$(SOC)/$(OBJ)/$(APP_NAME).elf $@

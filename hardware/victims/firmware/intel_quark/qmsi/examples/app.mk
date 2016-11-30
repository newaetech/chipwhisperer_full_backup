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

# Application template

### Environment checks
ifeq ($(APP_NAME),)
$(error APP_NAME is not defined)
endif
$(info APP_NAME = $(APP_NAME))

ifeq ($(SOC), quark_se)
ifeq ($(TARGET), x86)
else ifeq ($(TARGET), sensor)
else
$(error Supported TARGET values for $(SOC) are 'x86' and 'sensor')
endif
else ifeq ($(SOC), quark_d2000)
ifeq ($(TARGET), x86)
else
$(error Supported TARGET value for $(SOC) is 'x86')
endif
else
$(error Supported SOC values are 'quark_se' and 'quark_d2000')
endif
$(info SOC = $(SOC))
$(info TARGET = $(TARGET))

ifeq ($(APP_DIR),)
$(error APP_DIR is not defined)
endif
$(info APP_DIR = $(APP_DIR))

### Variables
BIN_DIR = $(APP_DIR)/$(BUILD)/$(SOC)/$(TARGET)/$(BIN)
OBJ_DIR = $(APP_DIR)/$(BUILD)/$(SOC)/$(TARGET)/$(OBJ)
APP = $(BIN_DIR)/$(APP_NAME).bin
QFU = $(APP).dfu
OBJ_DIRS += $(BIN_DIR) $(OBJ_DIR)
GENERATED_DIRS += $(APP_DIR)/$(BUILD)
SOURCES = $(wildcard $(APP_DIR)/*.c)
OBJECTS += $(addprefix $(OBJ_DIR)/,$(SOURCES:.c=.o))
CFLAGS += -DPRINTF_ENABLE -DPUTS_ENABLE
CFLAGS += -Wno-unused-parameter

QM_BOOTLOADER_DIR ?=

QFU_GEN_0 = python $(QM_BOOTLOADER_DIR)/tools/sysupdate/qm_make_dfu.py
QFU_GEN_1 = $(QFU_GEN_0) -v
QFU_GEN = $(QFU_GEN_$(V))

ifneq ($(SERIAL_PORT), )
DFU_TOOL = dfu-util-qda
DFU_ARG = -p $(SERIAL_PORT)
else
ifneq ($(USB_DEVICE), )
DFU_TOOL = dfu-util
DFU_ARG += -d $(USB_DEVICE)
endif
ifneq ($(USB_SERIAL), )
DFU_TOOL = dfu-util
DFU_ARG += -S $(USB_SERIAL)
endif
endif

DM_MANAGE_ERASE_0 = python $(QM_BOOTLOADER_DIR)/tools/sysupdate/qm_manage.py erase
DM_MANAGE_ERASE_1 = $(DM_MANAGE_ERASE_0) -v
DM_MANAGE_ERASE = $(DM_MANAGE_ERASE_$(V))

ifeq ($(TARGET), sensor)
QFU_PARTITION = 2
else
QFU_PARTITION = 1
endif

### Intel(R) Quark(TM) SE development platform
### routing UART_1 to a dual FTDI JTAG/UART chip.
### This is the default stdio option for Quark SE.
ifeq ($(SOC), quark_se)
CFLAGS += -DSTDOUT_UART_1 -DUART1_FTDI
endif

### Make includes
include $(BASE_DIR)/base.mk
include $(BASE_DIR)/sys/sys.mk
include $(BASE_DIR)/drivers/libqmsi.mk
include $(BASE_DIR)/soc/$(SOC)/$(SOC).mk
include $(BASE_DIR)/soc/$(SOC)/drivers/drivers.mk
include $(BASE_DIR)/board/drivers.mk

ifeq ($(SOC), quark_se)
ifeq ($(TARGET), x86)
include $(BASE_DIR)/usb/usb_stack.mk
endif
endif


### Build C files in APP_DIR
$(OBJ_DIR)/%.o: $(APP_DIR)/%.c libqmsi
	$(call mkdir, $(OBJ_DIR))
	$(CC) $(CFLAGS) -c -o $@ $<

### Link object files into APP ELF
$(APP): $(LINKER_FILE) $(OBJECTS) libqmsi
	$(call mkdir, $(BIN_DIR))
	$(LD) $(LDFLAGS) -Xlinker -T$(LINKER_FILE) \
		-Xlinker -A$(OUTPUT_ARCH) \
		-Xlinker --oformat$(OUTPUT_FORMAT) \
		-Xlinker -Map=$(OBJ_DIR)/$(APP_NAME).map \
		-o $(OBJ_DIR)/$(APP_NAME).elf $(OBJECTS) \
		-Xlinker --start-group $(LDLIBS) -Xlinker --end-group
	$(SIZE) $(OBJ_DIR)/$(APP_NAME).elf
	$(OBJCOPY) -O binary $(OBJ_DIR)/$(APP_NAME).elf $@

$(QFU): $(APP)
ifeq ($(QM_BOOTLOADER_DIR), )
	$(error QM_BOOTLOADER_DIR needs to point to the root of qm-bootloader)
endif
	$(QFU_GEN) $(APP) -p $(QFU_PARTITION)

qfu: $(QFU)

flash: $(QFU)
ifeq ($(DFU_TOOL), )
	$(error Target flash requires SERIAL_PORT, USB_DEVICE or USB_SERIAL to be set)
endif
	$(DFU_TOOL) -D $(QFU) $(DFU_ARG) -R -a $(QFU_PARTITION)

erase:
ifeq ($(DFU_TOOL), )
	$(error Target erase requires SERIAL_PORT, USB_DEVICE or USB_SERIAL to be set)
endif
	$(DM_MANAGE_ERASE) $(DFU_ARG)

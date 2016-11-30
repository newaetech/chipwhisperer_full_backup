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

### Variables
SOC := quark_d2000
SOC_DIR = $(BASE_DIR)/soc/$(SOC)
SOC_SOURCES := $(wildcard $(SOC_DIR)/drivers/*.c)
OBJ_DIRS += $(SOC_DIR)/$(BUILD)/$(SOC)/$(TARGET)/$(OBJ)
OBJECTS += $(addprefix $(SOC_DIR)/$(BUILD)/$(SOC)/$(TARGET)/$(OBJ)/,$(notdir $(SOC_SOURCES:.c=.o)))
GENERATED_DIRS += $(SOC_DIR)/$(BUILD)

CFLAGS += -I$(SOC_DIR)/include

DRV_SOURCES =
DRV_SOURCES += adc/qm_adc.c
DRV_SOURCES += aon_counters/qm_aon_counters.c
DRV_SOURCES += comparator/qm_comparator.c
DRV_SOURCES += dma/qm_dma.c
DRV_SOURCES += flash/qm_flash.c
DRV_SOURCES += fpr/qm_fpr.c
DRV_SOURCES += gpio/qm_gpio.c
DRV_SOURCES += i2c/qm_i2c.c
DRV_SOURCES += qm_identification.c
DRV_SOURCES += qm_init.c
DRV_SOURCES += interrupt/qm_interrupt.c
DRV_SOURCES += mpr/qm_mpr.c
DRV_SOURCES += timer/qm_pic_timer.c
DRV_SOURCES += pinmux/qm_pinmux.c
DRV_SOURCES += pwm/qm_pwm.c
DRV_SOURCES += rtc/qm_rtc.c
DRV_SOURCES += spi/qm_spi.c
DRV_SOURCES += uart/qm_uart.c
DRV_SOURCES += qm_version.c
DRV_SOURCES += wdt/qm_wdt.c
DRV_SOURCES += soc_watch.c

$(SOC_DIR)/$(BUILD)/$(SOC)/$(TARGET)/$(OBJ)/%.o: $(SOC_DIR)/drivers/%.c
	$(call mkdir, $(SOC_DIR)/$(BUILD)/$(SOC)/$(TARGET)/$(OBJ))
	$(CC) $(CFLAGS) -c -o $@ $<

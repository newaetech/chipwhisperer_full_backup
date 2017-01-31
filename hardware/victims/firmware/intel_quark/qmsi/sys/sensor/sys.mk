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
SYS_SENSOR_SOURCES = $(wildcard $(SYS_DIR)/sensor/*.c)
OBJECTS += $(addprefix $(APP_DIR)/$(BUILD)/$(SOC)/$(TARGET)/$(OBJ)/,$(notdir $(SYS_SENSOR_SOURCES:.c=.o)))

### Build C files
$(APP_DIR)/$(BUILD)/$(SOC)/$(TARGET)/$(OBJ)/%.o: $(SYS_DIR)/sensor/%.c
	$(call mkdir, $(APP_DIR)/$(BUILD)/$(SOC)/$(TARGET)/$(OBJ))
	$(CC) $(CFLAGS) -c -o $@ $<

ASM_SENSOR_SOURCES = $(wildcard $(SYS_DIR)/sensor/*.s)
OBJECTS += $(addprefix $(APP_DIR)/$(BUILD)/$(SOC)/$(TARGET)/$(OBJ)/,$(notdir $(ASM_SENSOR_SOURCES:.s=.s.o)))

### Build assembly files
$(APP_DIR)/$(BUILD)/$(SOC)/$(TARGET)/$(OBJ)/%.s.o: $(SYS_DIR)/sensor/%.s
	$(call mkdir, $(APP_DIR)/$(BUILD)/$(SOC)/$(TARGET)/$(OBJ))
	$(CC) $(CFLAGS) -xassembler-with-cpp -c -o $@ $<

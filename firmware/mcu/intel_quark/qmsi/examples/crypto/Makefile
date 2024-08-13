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

### Crypto example makefile

### Variables
APP_NAME = crypto
SOC ?= quark_d2000
TARGET ?= x86
BASE_DIR := ../..
APP_DIR := .

ifeq ($(TARGET), sensor)
$(error Supported TARGET value is 'x86')
endif

### Make includes
include $(BASE_DIR)/base.mk

### Tinycript sources are not C90 compilant
ifneq ($(CSTD),c99)
$(error Supported CSTD value is 'c99')
endif

ifeq ($(TINYCRYPT_SRC_DIR),)
$(error TINYCRYPT_SRC_DIR is not defined)
endif
$(info TINYCRYPT_SRC_DIR = $(TINYCRYPT_SRC_DIR))

### Variables
CRYPT_LIB_SRC_DIR = $(TINYCRYPT_SRC_DIR)/lib/source
CRYPT_LIB_INC_DIR = $(TINYCRYPT_SRC_DIR)/lib/include
LMT_SOURCES = $(wildcard $(CRYPT_LIB_SRC_DIR)/*.c)
OBJECTS += $(addprefix $(APP_DIR)/$(BUILD)/$(SOC)/$(TARGET)/$(OBJ)/,$(notdir $(LMT_SOURCES:.c=.o)))

### Flags
CFLAGS += -I$(CRYPT_LIB_INC_DIR)

### Build C files
$(APP_DIR)/$(BUILD)/$(SOC)/$(TARGET)/$(OBJ)/%.o: $(CRYPT_LIB_SRC_DIR)/%.c
	$(call mkdir, $(APP_DIR)/$(BUILD)/$(SOC)/$(TARGET)/$(OBJ))
	$(CC) $(CFLAGS) -c -o $@ $<

include $(BASE_DIR)/examples/app.mk

#!/usr/bin/env python
# @file This file merges the reset vector and the ROM code into an 8 KB image

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

import os
import sys
import struct

# Command line parameters
romCodeFilename = sys.argv[1]
outputFilename = sys.argv[2]

imageFile = open(outputFilename, 'wb+')
romCodeFile = open(romCodeFilename, 'rb').read()
imageLen = 0x2000  # ROM size for Quark SE SOC is 8 KB (0x2000)
dataAreaLen = 0x200  # We reserve 512 bytes for the OTP word and data storage
offset = 0

# Pad data area with 0xFF
for offset in range(dataAreaLen):
    imageFile.write(b'\xFF')
    offset += 1

# Write ROM code
imageFile.write(romCodeFile)
romCodeLen = len(romCodeFile)
offset += romCodeLen

imageFile.close()

print('. . . . . . . . . . . . . . . . . . . . . . . . . . . .')
print('Image size = ' + hex(imageLen))
print('Data area size = ' + hex(dataAreaLen))
print('ROM code size = ' + hex(romCodeLen))
print('. . . . . . . . . . . . . . . . . . . . . . . . . . . .')

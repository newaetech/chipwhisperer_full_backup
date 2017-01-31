#!/usr/bin/python -tt
# -*- coding: utf-8 -*-
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

"""
Quark Microcontroller Firmware Management Python Library.

Provides:

+--------------------+--------------------------------------------------------+
| :class:`QFUHeader` | A class to manage the specific QM image header.        |
+--------------------+--------------------------------------------------------+
| :class:`QFUImage`  | A class to create the specific QM firmware image.      |
+--------------------+--------------------------------------------------------+

Example usage::

    import qmfmlib

    header = qmfmlib.QFUHeader()
    header.id_vendor = 0x0143
    header.print_info()

    image = qmfmlib.QFUImage()
    image.make(header, file_handler)
    # write image.content to a file
"""

from qmfmlib.qfu import QFUHeader, QFUImage, QFUException
from qmfmlib.dfu import DFUImage, DFUException
from qmfmlib.qfm import QFMRequest, QFMResponse, QFMSysInfo, QFMException

__version__ = "1.1"

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

""" Quark Microcontroller Firmware Update DFU Module

This module provides classes to add and DFU compliance to firmware images for
Quark Microcontrollers.
"""

from __future__ import print_function, division, absolute_import
import struct
import binascii

_ENDIAN = "<"   # Defines the endian for struct packing. ('<'=little, '>'=big)


class DFUException(Exception):
    """ DFU Exception """

    def __init__(self, message):
        super(DFUException, self).__init__(message)


class DFUImage(object):
    """ Creates a DFU compatible file from a binary file. """
    suffix_crc = 0
    dfu_spec = 0x0100    # DFU 1.0
    length = 16
    release = 0xFFFF
    id_product = 0xFFFF
    id_vendor = 0xFFFF

    _suffix_struct = struct.Struct("%sHHHH3cBI" % _ENDIAN)

    def __init__(self):
        pass

    @property
    def _pack_suffix_tuple(self):
        """ Tuple containing the suffix information in a defined order. """
        return (
            self.release,
            self.id_product,
            self.id_vendor,
            self.dfu_spec,
            'U', 'F', 'D',
            self.length,
            self.suffix_crc,
        )

    @property
    def packed_dfu_suffix(self):
        """ Binary representation DFU suffix. """
        ret = self._suffix_struct.pack(*self._pack_suffix_tuple)
        return ret

    def crc(self, data, return_type=str):
        """ Calculate CRC.

        CRC32 as binary string or as defined with `format` of image file
        ignoring the last 4 bytes. These 4 bytes are space-holder for the
        resulting CRC32.

        Args:
            data (string): Binary content. (Must have 4 bytes CRC32 appended)
            return_type (type): The return type of CRC value. (int or str)

        Returns:
            Formatted CRC value.

        Raises:
            QFUException: In case of an invalid return_type or missing content.
        """
        if len(data) < 4:
            raise DFUException("no content available")

        crc = binascii.crc32(data[:-4])
        # NOTE: DFU-Util expects ~CRC
        crc = ~crc & 0xffffffff

        if return_type == int:
            return crc

        if return_type == str:
            return struct.Struct("%sI" % _ENDIAN).pack(crc)

        raise DFUException("unknown return type: %s" % return_type)

    def add_suffix(self, data, id_product=0xFFFF, id_vendor=0xFFFF):
        """ Adds DFU Suffix and the binary data.

        Args:
            data (string): Content of the binary file as a binary string.
            id_product (int): USB / DFU Product ID.
            id_vendor (int): USB / DFU Vendor ID.

        Returns:
            Binary data with attached DFU suffix.
        """

        self.id_vendor = id_vendor
        self.id_product = id_product

        # Set DFU suffix.
        data += self.packed_dfu_suffix

        # Compute suffix CRC.
        self.suffix_crc = self.crc(data, str)
        return data[:-4] + self.suffix_crc

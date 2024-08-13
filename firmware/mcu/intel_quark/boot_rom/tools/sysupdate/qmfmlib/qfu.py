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

"""Quark Microcontroller Firmware Update Module

This module provides classes to create and manipulate firmware images for Quark
Microcontrollers."""

from __future__ import print_function, division, absolute_import
import os
import re
import struct

_ENDIAN = "<"   # Defines the endian for struct packing. ('<'=little, '>'=big)


class QFUException(Exception):
    """QFU Exception."""

    def __init__(self, message):
        super(QFUException, self).__init__(message)


class QFUDefineParser(object):
    """A simple parser for C header files to extract #define of integers

    Note:
        We only parse simple #define macros like::

            #define DFU_ID_VENDOR (0x1200)"""

    defines = {}

    VENDOR_ID = "DFU_VENDOR_ID"
    PRODUCT_ID = "DFU_PRODUCT_ID"
    VERSION = "DFU_VERSION"
    BLOCK_SIZE = "DFU_BLOCK_SIZE"

    # Compiled regular expressions for `#define A int` or `#define A (int)`
    _re_int_line = re.compile(
        r"^\s*\#define\s+(\S+)\s+\(?(\d+)\)?")
    # Compiled regular expressions for `#define A hex` or `#define A (hex)`
    _re_hex_line = re.compile(
        r"^\s*\#define\s+(\S+)\s+\(?0x([0-9,a-f,A-F]+)\)?")

    def _check_line(self, line):
        """Search for valid defines in a line."""
        match = self._re_hex_line.match(line)
        if match:
            grp = match.groups()
            self.defines[grp[0]] = int(grp[1], 16)
            return
        match = self._re_int_line.match(line)
        if match:
            grp = match.groups()
            self.defines[grp[0]] = int(grp[1])
            return

    def __init__(self, open_file):
        """Opens and parses a C header like file for integer defines."""
        for line in open_file.readlines():
            self._check_line(line)


class QFUImage(object):
    """Creates a QFU compatible file from a binary file."""

    def __init__(self):
        pass

    def make(self, header, infile):
        """Assembles the QFU Header and the binary data.

        Args:
            header (QFUHeader): Header containing all relevant information to
                                create the image.
            infile (file): Open file object containing the binary data.

        Returns:
            The newly constructed binary data."""

        # Read input file size.
        infile.seek(0, os.SEEK_END)
        size = infile.tell()

        # Compute block count (add 1 for header with padding)
        header.num_blocks = ((size-1) // header.block_size) + 2

        # Set QFU header and DFU suffix.
        content = header.packed_qfu_header
        infile.seek(0, os.SEEK_SET)
        content += infile.read()
        return content


class QFUHeader(object):
    """The class holding QFU Header and DFU Suffix information

    Attributes:
        id_vendor (int): The DFU/USB vendor id.
        id_product (int): The DFU/USB product id.
        id_product_dfu (int): The DFU specific product id.
        partition_id (int): Target partition number.
        version (int): Firmware version of this image.
        block_size (int): The DFU block size.
        num_blocks (int): The number of blocks in this image."""

    id_vendor = 0
    id_product = 0
    id_product_dfu = 0
    partition_id = 0
    version = 0
    block_size = 0
    num_blocks = 0
    cipher_suite = 0

    # Different structure formats. _ENDIAN defines little or big-endian.
    # H stands for uint16, I for uint32 and c for a single character.
    _header_struct = struct.Struct("%sHHHHIHHH" % _ENDIAN)

    def __init__(self):
        pass

    def print_info(self, prefix=""):
        """Prints verbose QFU Header and information."""
        inset = " " * len(prefix)
        print("%sQFU-Header content:" % prefix)
        print("%s    Partition:   %d" % (inset, self.partition_id))
        print("%s    Vendor ID:   0x%04x" % (inset, self.id_vendor))
        print("%s    Product ID:  0x%04x" % (inset, self.id_product))
        print("%s    Version:     %d" % (inset, self.version))
        print("%s    Block Size:  %d" % (inset, self.block_size))
        print("%s    Blocks:      %d" % (inset, self.num_blocks))

    def set_from_file(self, open_file):
        """Read configuration file (C-header format) and update header
        information.

        Args:
            open_file (file): An open file with read permission. The file
                              needs to contain C-header style defines."""

        conf = QFUDefineParser(open_file)

        # Map configuration to class variables.
        if QFUDefineParser.VENDOR_ID in conf.defines:
            self.id_vendor = conf.defines[QFUDefineParser.VENDOR_ID]
        if QFUDefineParser.PRODUCT_ID in conf.defines:
            self.id_product = conf.defines[QFUDefineParser.PRODUCT_ID]
        if QFUDefineParser.VERSION in conf.defines:
            self.version = conf.defines[QFUDefineParser.VERSION]
        if QFUDefineParser.BLOCK_SIZE in conf.defines:
            self.block_size = conf.defines[QFUDefineParser.BLOCK_SIZE]

    def set_from_data(self, data):
        """Update header information from binary data string.

        Args:
            data (string): A string containing header packed information."""

        if data[:4] != 'QFUH':
            raise QFUException("QFUH prefix missing")
        data = data[4:]
        try:
            unpacked_data = self._header_struct.unpack(data)
            (
                self.id_vendor,
                self.id_product,
                self.id_product_dfu,
                self.partition_id,
                self.version,
                self.block_size,
                self.num_blocks,
                self.cipher_suite
            ) = unpacked_data

        except struct.error:
            raise QFUException("QFU Header length not valid")

    @property
    def _pack_header_tuple(self):
        """Tuple containing the header information in a defined order."""

        return (
            self.id_vendor,
            self.id_product,
            self.id_product_dfu,
            self.partition_id,
            self.version,
            self.block_size,
            self.num_blocks,
            self.cipher_suite
        )

    @property
    def packed_qfu_header(self):
        """Binary representation of QFU header."""

        ret = "QFUH" + self._header_struct.pack(*self._pack_header_tuple)
        for _ in range(self.block_size-len(ret)):
            ret += '.'
        return ret

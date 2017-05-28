#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2013-2016, NewAE Technology Inc
# All rights reserved.
#
# Find this and more at newae.com - this file is part of the chipwhisperer
# project, http://www.assembla.com/spaces/chipwhisperer
#
#    This file is part of chipwhisperer.
#
#    chipwhisperer is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    chipwhisperer is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU Lesser General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with chipwhisperer.  If not, see <http://www.gnu.org/licenses/>.
#=================================================
import logging

from _base import SPIFlashTemplate
from chipwhisperer.common.utils.parameter import setupSetParam
from lowlevel.aardvark_py import *

class SPIFlash_Aardvark(SPIFlashTemplate):
    _name = 'Total Phase Aardvark'

    def __init__(self):
        SPIFlashTemplate.__init__(self)
        self._baud = 2000
        self._handle = None
        self._mode = 0
        self.params.addChildren([
            {'name':'Baud (kHz)', 'type':'int', 'key':'baud', 'limits':(125, 8000), 'get':self.baud, 'set':self.setBaud, 'default':500},
            {'name':'SPI Mode', 'type':'list', 'key':'mode', 'values':[0, 3], 'get':self.mode, 'set':self.setMode, 'default':0},
        ])

    def close(self):
        pass

    @setupSetParam("baud")
    def setBaud(self, baud):
        self._baud = baud
        if self._handle:
            self._baud = aa_spi_bitrate(self._handle, baud)
        else:
            logging.error('Baud rate not set, need to connect first')

    def baud(self):
        return self._baud

    def mode(self):
        return self._mode

    @setupSetParam("mode")
    def setMode(self, mode):
        aa_spi_configure(self._handle, mode >> 1, mode & 1, AA_SPI_BITORDER_MSB)
        self._mode = mode

    def close(self):
        if self._handle:
            aa_close(self._handle)
        self._handle = None

    def con(self, scope = None):
        self._handle = None
        handle = aa_open(0)
        if (handle <= 0):
            raise IOError("aa_open() failed with %d" % handle)
        self._handle = handle
        aa_configure(handle, AA_CONFIG_SPI_I2C)
        self.setMode(0)
        self.setBaud(2000)
        self.params.refreshAllParameters()
        self.connectStatus.setValue(True)

    def hw_spi_write(self, data_to_send):
        data_in = array_u08(len(data_to_send))
        data_out = array('B', data_to_send)
        (count, data_in) = aa_spi_write(self._handle, data_out, data_in)

        if count != len(data_in):
            raise IOError("Failed to execute command.")

        return list(data_in)
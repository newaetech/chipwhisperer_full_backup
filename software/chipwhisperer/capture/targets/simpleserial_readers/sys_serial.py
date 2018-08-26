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

from ._base import SimpleSerialTemplate
import serial

class SimpleSerial_serial(SimpleSerialTemplate):
    _name = "System Serial Port"

    def __init__(self, portname=None):
        SimpleSerialTemplate.__init__(self)
        self.ser = serial.Serial(baudrate=38400)

        if portname:
            self.set_port(portname)

    def debugInfo(self, lastTx=None, lastRx=None, logInfo=None):
        pass

    def baud(self):
        return self.ser.baudrate

    def set_baud(self, baud):
        if baud == self.ser.baudrate:
            return
        self.ser.baudrate = baud
        if self.ser.is_open:
            self.ser.close()
            self.ser.open()

    def set_port(self, portname):
        self.ser.port = portnames

    def hardware_write(self, string):
        return self.ser.write(bytearray(string.encode('utf-8')))

    def hardware_read(self, num, timeout=100):
        return self.ser.read(num)

    def hardware_inWaiting(self):
        return self.ser.in_waiting

    def close(self):
        if self.ser is not None:
            self.ser.close()
            self.ser = None

    def con(self, scope = None):
        if not self.ser.is_open:
            # Open serial port if not already
            self.ser.port     = self.portname
            self.ser.timeout  = 2     # 2 second timeout
            self.ser.open()

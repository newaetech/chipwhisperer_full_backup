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
from collections import OrderedDict
from chipwhisperer.common.utils.parameter import setupSetParam
from chipwhisperer.common.utils import serialport


class SimpleSerial_serial(SimpleSerialTemplate):
    _name = "System Serial Port"

    def __init__(self):
        SimpleSerialTemplate.__init__(self)
        self.ser = serial.Serial(baudrate=38400)
        baud_values = OrderedDict(list(zip([str(X) for X in serial.Serial.BAUDRATES], serial.Serial.BAUDRATES)))
        self.params.addChildren([
            {'name':'Baud', 'key':'baud', 'type':'list', 'values':baud_values, 'value':self.ser.baudrate},
            {'name':'Port', 'key':'port', 'type':'list', 'values':['Hit Refresh'], 'value':'Hit Refresh'},
            {'name':'Refresh', 'type':'action', 'action':self.updateSerial}
        ])

    def updateSerial(self, _=None):
        serialnames = serialport.scan()
        self.findParam('port').setLimits(serialnames)
        if len(serialnames) > 0:
            self.findParam('port').setValue(serialnames[0])

    def selectionChanged(self):
        self.updateSerial()

    def debugInfo(self, lastTx=None, lastRx=None, logInfo=None):
        pass

    def baud(self):
        return self.ser.baudrate

    @setupSetParam("baud")
    def setBaud(self, baud):
        if baud == self.ser.baudrate:
            return
        self.ser.baudrate = baud
        if self.ser.is_open:
            self.ser.close()
            self.ser.open()

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
            self.ser.port     = self.findParam('port').getValue()
            self.ser.baudrate = self.findParam('baud').getValue()
            self.ser.timeout  = 2     # 2 second timeout
            self.ser.open()

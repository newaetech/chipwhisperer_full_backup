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
from chipwhisperer.common.utils import serialport


class SimpleSerial_serial(SimpleSerialTemplate):
    _name = "System Serial Port"

    def __init__(self):
        SimpleSerialTemplate.__init__(self)
        self.ser = None
        self.params.addChildren([
            {'name':'Baud', 'key':'baud', 'type':'list', 'values':{'38400':38400, '19200':19200}, 'value':38400},
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

    def write(self, string):
        return self.ser.write(string)

    def read(self, num=0, timeout=100):
        return self.ser.read(num)

    def flush(self):
        self.ser.flushInput()

    def flushInput(self):
        self.ser.flushInput()

    def close(self):
        if self.ser is not None:
            self.ser.close()
            self.ser = None

    def con(self, scope = None):
        if self.ser == None:
            # Open serial port if not already
            self.ser = serial.Serial()
            self.ser.port     = self.findParam('port').getValue()
            self.ser.baudrate = self.findParam('baud').getValue()
            self.ser.timeout  = 2     # 2 second timeout
            self.ser.open()

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

from _base import SimpleSerialTemplate
from chipwhisperer.hardware.naeusb.serial import USART as CWL_USART


class SimpleSerial_ChipWhispererLite(SimpleSerialTemplate):
    _name = 'ChipWhisperer-Lite'

    def __init__(self, parentParam=None):
        SimpleSerialTemplate.__init__(self, parentParam)
        self.cwlite_usart = None
        self.params.addChildren([
            {'name':'baud', 'type':'int', 'key':'baud', 'limits':(500, 2000000), 'get':self.baud, 'set':self.setBaud, 'default':38400}
        ])

    def setBaud(self, baud):
        if self.cwlite_usart:
            self.cwlite_usart.init(baud)
        else:
            print "Baud rate not set, need to connect first"

    def baud(self):
        return 38400

    def write(self, string):
        self.cwlite_usart.write(string)

    def inWaiting(self):
        return self.cwlite_usart.inWaiting()

    def read(self, num=0, timeout=250):
        data = bytearray(self.cwlite_usart.read(num, timeout=timeout))
        result = data.decode('latin-1')
        return result

    def flush(self):
        waiting = self.inWaiting()
        while waiting > 0:
            self.cwlite_usart.read(waiting)
            waiting = self.inWaiting()

    def flushInput(self):
        self.flush()

    def close(self):
        pass

    def con(self, scope = None):
        if scope is None or not hasattr(scope, "qtadc"): Warning("You need a scope with OpenADC connected to use this Target")

        scope.connectStatus.connect(self.dis)
        self.cwlite_usart = CWL_USART(scope.qtadc.ser)
        self.cwlite_usart.init(baud=self.findParam('baud').getValue())
        self.params.refreshAllParameters()
        self.connectStatus.setValue(True)


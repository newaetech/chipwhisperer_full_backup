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

from _base import SimpleSerialTemplate
from chipwhisperer.hardware.naeusb.serial import USART as CWL_USART
from chipwhisperer.common.utils.parameter import setupSetParam


class SimpleSerial_ChipWhispererLite(SimpleSerialTemplate):
    _name = 'NewAE USB (CWLite/CW1200)'

    def __init__(self):
        SimpleSerialTemplate.__init__(self)
        self._baud = 38400
        self.cwlite_usart = None
        self.params.addChildren([
            {'name':'baud', 'type':'int', 'key':'baud', 'limits':(500, 2000000), 'get':self.baud, 'set':self.setBaud, 'default':38400}
        ])

    @setupSetParam("baud")
    def setBaud(self, baud):
        self._baud = baud
        if self.cwlite_usart:
            self.cwlite_usart.init(baud)
        else:
            logging.error('Baud rate not set, need to connect first')

    def baud(self):
        return self._baud

    def write(self, string):
        self.cwlite_usart.write(string)

    def inWaiting(self):
        bwait =  self.cwlite_usart.inWaiting()
        if bwait == 127:
            logging.warning('SAM3U Serial buffers OVERRUN - data loss has occurred.')
        return bwait

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


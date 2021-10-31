#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2013-2021, NewAE Technology Inc
# All rights reserved.
#
# Find this and more at newae.com - this file is part of the chipwhisperer
# project, http://www.assembla.com/spaces/chipwhisperer
#
#    This file is part of chipwhisperer.
#
#    Licensed under the Apache License, Version 2.0 (the "License");
#    you may not use this file except in compliance with the License.
#    You may obtain a copy of the License at
#
#       http://www.apache.org/licenses/LICENSE-2.0
#
#    Unless required by applicable law or agreed to in writing, software
#    distributed under the License is distributed on an "AS IS" BASIS,
#    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#    See the License for the specific language governing permissions and
#    limitations under the License.
#=================================================
import logging

from ._base import SimpleSerialTemplate
from ....capture.scopes.OpenADC import OpenADC

class SimpleSerial_ChipWhispererLite(SimpleSerialTemplate):
    _name = 'NewAE USB (CWLite/CW1200)'

    def __init__(self):
        SimpleSerialTemplate.__init__(self)
        self._baud = 38400
        self.cwlite_usart = None

    def close(self):
        pass

    def setBaud(self, baud):
        self._baud = baud
        if self.cwlite_usart:
            self.cwlite_usart.init(baud)
        else:
            logging.error('Baud rate not set, need to connect first')

    def baud(self):
        return self._baud

    def con(self, scope = None):
        if scope is None or not hasattr(scope, "qtadc"):
            Warning("You need a scope with OpenADC connected to use this Target")

        if isinstance(scope, OpenADC):
            ser = scope.scopetype.ser
            
            # if we don't do this, we get multiple serial objects kicking around
            # making it hard to save +  restore the baud
            self.cwlite_usart = scope.scopetype.serialstm32f._cwserial
        else:
            ser = scope._cwusb
            self.cwlite_usart = scope.usart
        self.cwlite_usart.init(baud=self._baud)


    def hardware_inWaiting(self):
        bwait = self.cwlite_usart.inWaiting()
        if bwait == 127:
            logging.warning('SAM3U Serial buffers OVERRUN - data loss has occurred.')
        return bwait

    def hardware_inWaitingTX(self):
        bwait = self.cwlite_usart.in_waiting_tx()
        return bwait

    def hardware_write(self, string):
        self.cwlite_usart.write(string)

    def hardware_read(self, num, timeout=250):
        return self.cwlite_usart.read(num, timeout)
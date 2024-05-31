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
# from ....capture.scopes.OpenADC import OpenADC
# from ....capture.scopes.cwnano import CWNano
# from typing import Union

class SimpleSerial_ChipWhispererLite(SimpleSerialTemplate):
    _name = 'NewAE USB (CWLite/CW1200)'

    def __init__(self):
        SimpleSerialTemplate.__init__(self)
        self._baud = 38400
        self._parity = "none"
        self._stopbits = 1
        self.cwlite_usart = None
        self._buf_size = None

    def close(self):
        pass

    def setBaud(self, baud):
        self._baud = baud
        if self.cwlite_usart:
            self.cwlite_usart.init(self._baud, self._stopbits, self._parity)
        else:
            logging.error('Baud rate not set, need to connect first')

    def baud(self):
        return self._baud

    def setParity(self, parity):
        old = self._parity
        try:
            self._parity = parity
            if self.cwlite_usart:
                self.cwlite_usart.init(self._baud, self._stopbits, self._parity)
            else:
                logging.error('Baud rate not set, need to connect first')
        except ValueError as e:
            self._parity = old
            raise e

    def parity(self):
        return self._parity

    def setStopBits(self, stopbits):
        old = self._stopbits
        try:
            self._stopbits = stopbits
            if self.cwlite_usart:
                self.cwlite_usart.init(self._baud, self._stopbits, self._parity)
            else:
                logging.error('Baud rate not set, need to connect first')
        except ValueError as e:
            self._stopbits = old
            raise e

    def stopBits(self):
        return self._stopbits
        

    def con(self, scope): # remove typing here to avoid pulling in OpenADC/CWNano
        if not scope is None:
            self.cwlite_usart = scope._get_usart()
            self.cwlite_usart.init(baud=self._baud, stopbits=self._stopbits, parity=self._parity)
            self._buf_size = 128
            if self.cwlite_usart._usb.check_feature("SERIAL_200_BUFFER"):
                self._buf_size = 200


    def hardware_inWaiting(self):
        bwait = self.cwlite_usart.inWaiting()
        if bwait >= (self._buf_size - 1):
            logging.warning('SAM3U Serial buffers OVERRUN - data loss has occurred.')
        return bwait

    def hardware_inWaitingTX(self):
        bwait = self.cwlite_usart.in_waiting_tx()
        return bwait

    def hardware_write(self, string):
        self.cwlite_usart.write(string)

    def hardware_read(self, num, timeout=250):
        return self.cwlite_usart.read(num, timeout)

    @property
    def xonxoff(self):
        # TODO: check version to make sure fw has this
        return self.cwlite_usart.xonxoff
    
    @xonxoff.setter
    def xonxoff(self, enable):
        self.cwlite_usart.xonxoff = enable

    @property
    def currently_xoff(self):
        return self.cwlite_usart.currently_xoff
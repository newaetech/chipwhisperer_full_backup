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

from _base import ProtocolTemplate


class ProtocolSASEBOWCardOS(ProtocolTemplate):

    def loadEncryptionKey(self, key):
        if len(key) != 16:
            raise ValueError("Encryption key != 16 bytes??")
        status = self.hw.sendAPDU(0x80, 0x12, 0x00, 0x00, key)
        self.key = key
        #print status

    def loadInput(self, inputtext):
        self.input = inputtext

    def go(self):
        if len(self.input) != 16:
            raise ValueError("Plaintext != 16 bytes??")
        status = self.hw.sendAPDU(0x80, 0x04, 0x04, 0x00, self.input)
        #print status

    def readOutput(self):
        (resp, pay) = self.hw.sendAPDU(0x80, 0xC0, 0x00, 0x00, rxdatalen=16)
        #print resp
        return pay

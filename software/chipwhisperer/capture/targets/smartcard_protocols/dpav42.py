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
from chipwhisperer.capture.targets.smartcard_readers import chipwhisperer_ser


class ProtocolDPAv42(ProtocolTemplate):
    _name = "DPAContestv4"

    def loadEncryptionKey(self, key):
        if len(key) != 16:
            raise ValueError("Encryption key != 16 bytes??")
        self.key = key

    def loadInput(self, inputtext):
        if len(inputtext) != 16:
            raise ValueError("Plaintext != 16 bytes??")
        self.input = inputtext

    def go(self):
        if not isinstance(self.hw, chipwhisperer_ser.ReaderChipWhispererSER):
            raise IOError("ERROR: DPAContestv4 only works with ChipWhisperer-SER Reader")

        data = []
        data.extend(self.key)
        data.extend(self.input)

        self.hw.ser.flush()
        self.hw.ser.write([0x80, 0xC0, 0x00, 0x00, 32])
        self.hw.waitEcho([0x80, 0xC0, 0x00, 0x00, 32])
        self.hw.ser.write(data)
        self.hw.waitEcho(data)

        #Ack
        ack = self.hw.ser.read(1)
        #print "%02x"%ord(ack[0])

        #Random Block
        rblock = self.hw.ser.read(16)
        rblock = [ord(t) for t in rblock]

        #Ack
        ack = self.hw.ser.read(1)

        #Key + Message sent back
        self.hw.waitEcho(data)

        #Ack - shuffle done
        ack = self.hw.ser.read(1)

        #Receive shuffle blocks back
        suffle0 = self.hw.ser.read(16)
        suffle0 = [ord(t) for t in suffle0]

        suffle1 = self.hw.ser.read(16)
        suffle1 = [ord(t) for t in suffle1]

        #Ack - encryption starting now
        ack = self.hw.ser.read(1)

        #Ack - encryption done now
        ack = self.hw.ser.read(1)

        result = self.hw.ser.read(16)
        result = [ord(t) for t in result]

        self.textout = result

    def readOutput(self):
        return self.textout

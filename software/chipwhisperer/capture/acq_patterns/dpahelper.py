#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2013-2014, NewAE Technology Inc
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

import random
from chipwhisperer.common.utils import util
from ._base import AcqKeyTextPattern_Base
from chipwhisperer.common.utils.parameter import setupSetParam


class AcqKeyTextPattern_DPA(AcqKeyTextPattern_Base):
    _name = "DPA Helper"

    def __init__(self, target=None):
        AcqKeyTextPattern_Base.__init__(self)
        self.initmask = 'FF 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00'
        self.initkey = '2B 7E 15 16 28 AE D2 A6 AB F7 15 88 09 CF 4F 3C'
        self._key = util.hexStrToByteArray(self.initkey)
        self._mask = util.hexStrToByteArray(self.initmask)
        self.setTarget(target)

    def getInitialKey(self):
        return " ".join(["%02X"%b for b in self._key])

    @setupSetParam("Fixed Encryption Key")
    def setInitialKey(self, initialKey, binaryKey=False):
        if initialKey:
            if binaryKey:
                keyStr = ''
                for s in initialKey:
                    keyStr += '%02x ' % s
                self._key = bytearray(initialKey)
            else:
                keyStr = initialKey
                self._key = util.hexStrToByteArray(initialKey)

            self.initkey = keyStr

    def getInitialMask(self):
        return " ".join(["%02X" % b for b in self._mask])

    @setupSetParam("Fixed Plaintext")
    def setInitialMask(self, initialMask, binaryMask=False):
        if initialMask:
            if binaryMask:
                maskStr = ''
                for s in initialMask:
                    maskStr += '%02x ' % s
                self._mask = bytearray(initialMask)
            else:
                maskStr = initialMask
                self._mask = util.hexStrToByteArray(initialMask)

            self.initmask = maskStr

    def initPair(self, maxtraces):
        pass

    def new_pair(self):
        randDPA = random.randint(0, 1)
        if randDPA == 0:
            DPAByte = 0
        else:
            DPAByte = 0xFF
        self._textin = bytearray(self.textLen())
        for i in range(0, self.textLen()):
            randbyte = random.randint(0, 255)
            self._textin[i] = (randbyte & ~self._mask[i]) | (DPAByte & self._mask[i])

        # Check pair works with target
        self.validateKey()
        self.validateText()

        return self._key, self._textin

    newPair = new_pair

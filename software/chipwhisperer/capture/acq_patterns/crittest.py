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
from _base import AcqKeyTextPattern_Base

class AcqKeyTextPattern_CRITTest(AcqKeyTextPattern_Base):
    _name = "CRI T-Test"

    def __init__(self, parentParam, target=None):
        AcqKeyTextPattern_Base.__init__(self, parentParam, target)
        self._fixedPlain = False
        self._fixedKey = True

    def _initPattern(self):
        pass

    def initPair(self):
        if self.keyLen() == 16:
            self._key = util.hexStrToByteArray("01 23 45 67 89 ab cd ef 12 34 56 78 9a bc de f0")
        elif self.keyLen() == 24:
            self._key = util.hexStrToByteArray("01 23 45 67 89 ab cd ef 12 34 56 78 9a bc de f0 23 45 67 89 ab cd ef 01")
        elif self.keyLen() == 32:
            self._key = util.hexStrToByteArray("01 23 45 67 89 ab cd ef 12 34 56 78 9a bc de f0 23 45 67 89 ab cd ef 01 34 56 78 9a bc de f0 12")
        else:
            raise ValueError("Invalid key length: %d bytes" % self.keyLen())

        self._textin1 = util.hexStrToByteArray("00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00")

        if self.keyLen() == 16:
            self._textin2 = util.hexStrToByteArray("da 39 a3 ee 5e 6b 4b 0d 32 55 bf ef 95 60 18 90")
        elif self.keyLen() == 24:
            self._textin2 = util.hexStrToByteArray("da 39 a3 ee 5e 6b 4b 0d 32 55 bf ef 95 60 18 88")
        elif self.keyLen() == 32:
            self._textin2 = util.hexStrToByteArray("da 39 a3 ee 5e 6b 4b 0d 32 55 bf ef 95 60 18 95")
        else:
            raise ValueError("Invalid key length: %d bytes" % self.keyLen())

        self.group1 = True

    def newPair(self):
        if self.group1:
            self.group1 = False
            self._textin = self._textin1

            try:
                from Crypto.Cipher import AES
                cipher = AES.new(str(self._key), AES.MODE_ECB)
                self._textin1 = bytearray(cipher.encrypt(str(self._textin1)))
            except ImportError:
                print "No AES Module, Using rand() instead!"
                self._textin1 = bytearray(16)
                for i in range(0, 16):
                    self._textin1[i] = random.randint(0, 255)
        else:
            self.group1 = True
            self._textin = self._textin2

        # Check key works with target
        self.validateKey()

        return (self._key, self._textin)

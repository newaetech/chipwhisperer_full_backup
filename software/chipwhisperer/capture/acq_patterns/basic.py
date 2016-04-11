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
from chipwhisperer.common.utils import Util
from chipwhisperer.capture.acq_patterns._base import AcqKeyTextPattern_Base


def getClass():
    """"Returns the Main Class in this Module"""
    return AcqKeyTextPattern_Basic


class AcqKeyTextPattern_Basic(AcqKeyTextPattern_Base):
    name = "Basic"

    def setupParams(self):
        self._fixedPlain = False
        self._fixedKey = True

        basicParams = [
                      {'name':'Key', 'type':'list', 'values':['Random', 'Fixed'], 'value':'Fixed', 'set':self.setKeyType},
                      {'name':'Plaintext', 'type':'list', 'values':['Random', 'Fixed'], 'value':'Random', 'set':self.setPlainType},
                      {'name':'Fixed Encryption Key', 'key':'initkey', 'type':'str', 'set':self.setInitialKey},
                      {'name':'Fixed Plaintext Key', 'key':'inittext', 'type':'str', 'set':self.setInitialText},
                  ]
        return basicParams

    def setKeyType(self, t):
        if t == 'Fixed':
            self._fixedKey = True
        elif t == 'Random':
            self._fixedKey = False
        else:
            raise ValueError("Invalid value for Key Type: %s" % t)

    def setPlainType(self, t):
        if t == 'Fixed':
            self._fixedPlain = True
        elif t == 'Random':
            self._fixedPlain = False
        else:
            raise ValueError("Invalid value for Text Type: %s" % t)

    def _initPattern(self):
        self.setInitialKey('2b 7e 15 16 28 ae d2 a6 ab f7 15 88 09 cf 4f 3c')
        self.setInitialText('00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F')

    def setInitialKey(self, initialKey, binaryKey=False):
        if initialKey:
            if binaryKey:
                keyStr = ''
                for s in initialKey:
                    keyStr += '%02x ' % s
                self._key = bytearray(initialKey)
            else:
                keyStr = initialKey
                self._key = Util.hexStrToByteArray(initialKey)

            self.initkey = keyStr

    def setInitialText(self, initialText, binaryText=False):
        if initialText:
            if binaryText:
                textStr = ''
                for s in initialText:
                    textStr += '%02x ' % s
                self._textin = bytearray(initialText)
            else:
                textStr = initialText
                self._textin = Util.hexStrToByteArray(initialText)

            self.inittext = textStr

    def initPair(self):
        self._initPattern()

    def newPair(self):
        if self._fixedKey is False:
            self._key = bytearray(self.keyLen())
            for i in range(0, self.keyLen()):
                self._key[i] = random.randint(0, 255)

        if self._fixedPlain is False:
            self._textin = bytearray(16)
            for i in range(0, 16):
                self._textin[i] = random.randint(0, 255)

        # Check key works with target
        self.validateKey()

        return (self._key, self._textin)
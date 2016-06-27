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
from chipwhisperer.common.utils.parameter import setupSetParam


class AcqKeyTextPattern_Basic(AcqKeyTextPattern_Base):
    _name = "Basic"

    def __init__(self, target=None):
        AcqKeyTextPattern_Base.__init__(self)
        self._fixedKey = True
        self._fixedPlain = False
        self.inittext = '00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F'
        self.initkey = '2B 7E 15 16 28 AE D2 A6 AB F7 15 88 09 CF 4F 3C'
        self._key = util.hexStrToByteArray(self.initkey)
        self._textin = bytearray(self.inittext)
        self.types = {'Random': False, 'Fixed': True}

        self.getParams().addChildren([
            {'name':'Key', 'type':'list', 'values':self.types , 'get':self.getKeyType, 'set':self.setKeyType, 'action':lambda p:self.findParam("initkey").show(p.getValue())},
            # {'name':'Size', 'type':'int'},
            {'name':'Fixed Encryption Key', 'key':'initkey', 'type':'str', 'get':self.getInitialKey, 'set':self.setInitialKey, 'visible':self.getKeyType()},
            {'name':'Plaintext', 'type':'list', 'values':self.types , 'get':self.getPlainType, 'set':self.setPlainType, 'action':lambda p:self.findParam("inittext").show(p.getValue())},
            {'name':'Fixed Plaintext Key', 'key':'inittext', 'type':'str', 'get':self.getInitialText, 'set':self.setInitialText, 'visible':self.getPlainType()},
        ])
        self.setTarget(target)

    def getKeyType(self):
        return self._fixedKey

    @setupSetParam("Key")
    def setKeyType(self, t):
        self._fixedKey = t

    def getPlainType(self):
        return self._fixedPlain

    @setupSetParam("Plaintext")
    def setPlainType(self, t):
        self._fixedPlain = t

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

    def getInitialText(self):
        return " ".join(["%02X" % b for b in self._textin])

    @setupSetParam("Fixed Plaintext Key")
    def setInitialText(self, initialText, binaryText=False):
        if initialText:
            if binaryText:
                textStr = ''
                for s in initialText:
                    textStr += '%02x ' % s
                self._textin = bytearray(initialText)
            else:
                textStr = initialText
                self._textin = util.hexStrToByteArray(initialText)

            self.inittext = textStr

    def initPair(self):
        pass

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

    def __str__(self):
        key = "Key=" + self.findParam("Key").getValueKey()
        if self._fixedKey:
            key = key + ":" + self.findParam("initkey").getValue()
        plaintext = "Plaintext=" + self.findParam("Plaintext").getValueKey()
        if self._fixedPlain:
            plaintext = plaintext + ":" + self.findParam("inittext").getValue()

        return self.getName() + " (%s, %s)" % (key, plaintext)
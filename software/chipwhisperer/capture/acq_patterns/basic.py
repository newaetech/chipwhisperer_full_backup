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
from ...common.utils import util
from ._base import AcqKeyTextPattern_Base
from ...common.utils.util import camel_case_deprecated
bytearray = util.CWByteArray # type: ignore

class AcqKeyTextPattern_Basic(AcqKeyTextPattern_Base):
    """Class for getting basic keys and plaintexts.

    Basic usage::

        import chipwhisperer as cw
        ktp = cw.ktp.Basic()
        key, text = ktp.next()
    """
    _name = "Basic"

    def __init__(self):
        AcqKeyTextPattern_Base.__init__(self)
        self._fixedKey = True
        self._fixedPlain = False
        self.inittext = '00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F'
        self.initkey = '2B 7E 15 16 28 AE D2 A6 AB F7 15 88 09 CF 4F 3C'
        self._key = util.hexStrToByteArray(self.initkey)
        self._textin = util.hexStrToByteArray(self.inittext)
        self.types = {'Random': False, 'Fixed': True}

    @property
    def fixed_key(self):
        """Generate fixed key (True) or not (False).

        :Getter: Return True if using fixed key or False if not.

        :Setter: Set whether to use fixed key (True) or not (False).

        .. versionadded:: 5.1
            Added fixed_key property
        """
        return self._fixedKey

    @fixed_key.setter
    def fixed_key(self, enabled):
        self._fixedKey = enabled

    @property
    def fixed_text(self):
        """Generate fixed plaintext (True) or not (False).

        :Getter: Return True if using fixed plaintext or False if not.

        :Setter: Set whether to use fixed plaintext (True) or not (False).

        .. versionadded:: 5.1
            Added fixed_text property
        """
        return self._fixedPlain

    @fixed_text.setter
    def fixed_text(self, enabled):
        self._fixedPlain = enabled

    def get_key_type(self):
        return self._fixedKey

    getKeyType = camel_case_deprecated(get_key_type)

    def set_key_type(self, t):
        self._fixedKey = t

    setKeyType = camel_case_deprecated(set_key_type)


    def getPlainType(self):
        return self._fixedPlain

    def setPlainType(self, t):
        self._fixedPlain = t

    def getInitialKey(self):
        return " ".join(["%02X"%b for b in self._key])

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

    def initPair(self, maxtraces):
        pass

    def new_pair(self):
        if self._fixedKey is False:
            self._key = bytearray(self.keyLen())
            for i in range(0, self.keyLen()):
                self._key[i] = random.randint(0, 255)

        if self._fixedPlain is False:
            self._textin = bytearray(self.textLen())
            for i in range(0, self.textLen()):
                self._textin[i] = random.randint(0, 255)

        # Check pair works with target
        self.validateKey()
        self.validateText()

        return self._key, self._textin

    newPair = new_pair

    def next(self):
        """Returns the next key text pair

        Updates last key and text

        Returns:
            (key (bytearray), text (bytearray))

        .. versionadded:: 5.1
            Added next
        """

        return self.next_key(), self.next_text()

    def next_text(self):
        """ Returns the next plaintext

        Does not update key. If text is fixed, returns the same plaintext as
        last time

        Returns:
            text (bytearray)

        .. versionadded:: 5.1
            Added next_text
        """
        if self._fixedPlain is False:
            self._textin = bytearray(self.textLen())
            for i in range(0, self.textLen()):
                self._textin[i] = random.randint(0, 255)

        self.validateText()
        return bytearray(self._textin)

    def next_key(self):
        """ Returns the next key

        Does not update text. If key is fixed, returns the same key as last
        time

        Returns:
            key (bytearray)

        .. versionadded:: 5.1
            Added next_key
        """
        if self._fixedKey is False:
            self._key = bytearray(self.keyLen())
            for i in range(0, self.keyLen()):
                self._key[i] = random.randint(0, 255)
        self.validateKey()
        return bytearray(self._key)

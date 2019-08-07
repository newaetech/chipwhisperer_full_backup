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
import logging
import random
from chipwhisperer.common.utils import util
from chipwhisperer.analyzer.utils.aes_funcs import key_schedule_rounds
from chipwhisperer.common.utils.aes_cipher import AESCipher
from ._base import AcqKeyTextPattern_Base

class AcqKeyTextPattern_TVLATTest(AcqKeyTextPattern_Base):
    """Class for getting key and text for TVLA T-Tests.

    Basic usage::

        import chipwhisperer as cw
        ktp = cw.ktp.TVLATTest()
        ktp.init(num_traces) # init with the number of traces you plan to
                             # capture
        key, text = ktp.next()

    """
    _name = "TVLA Rand vs Fixed"
    _description = "Welsh T-Test with random/fixed plaintext."

    def __init__(self, target=None):
        AcqKeyTextPattern_Base.__init__(self)
        self._interleavedPlaintext = []
        self._key = []


        self.setTarget(target)

    def _initPattern(self):
        pass

    def init(self, maxtraces):
        """Initialize key text pattern for a specific number of traces.

        Args:
            maxtraces (int): Number of traces to initialize for.

        Raises:
            ValueError: Invalid key length
        """
        length = self.keyLen()
        if length <= 32:
            self._key = util.hexStrToByteArray("01 23 45 67 89 ab cd ef 12 34 56 78 9a bc de f0 23 45 67 89 ab cd ef 01 34 56 78 9a bc de f0 12")[:length]
        else:
            raise ValueError("Invalid key length: %d bytes" % length)

        self._textin1 = util.hexStrToByteArray("00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00")

        if length == 16:
            self._interleavedPlaintext = util.hexStrToByteArray("da 39 a3 ee 5e 6b 4b 0d 32 55 bf ef 95 60 18 90")
        elif length == 24:
            self._interleavedPlaintext = util.hexStrToByteArray("da 39 a3 ee 5e 6b 4b 0d 32 55 bf ef 95 60 18 88")
        elif length == 32:
            self._interleavedPlaintext = util.hexStrToByteArray("da 39 a3 ee 5e 6b 4b 0d 32 55 bf ef 95 60 18 95")
        else:
            raise ValueError("Invalid key length: %d bytes" % length)

        self.num_group1 = int(maxtraces/2)
        self.num_group2 = int(maxtraces - self.num_group1)

    def new_pair(self):
        rand = random.random()
        num_tot = self.num_group1 + self.num_group2
        if num_tot == 0:
            group1 = (rand < 0.5)
        else:
            cutoff = float(self.num_group1) / num_tot
            group1 = (rand < cutoff)

        if group1:
            self._textin = self._textin1

            exp_key = list(self._key)
            rounds = 0
            keylen = self.keyLen()

            if keylen == 16:
                rounds = 10
            elif keylen == 24:
                rounds = 12
            elif keylen == 32:
                rounds = 14

            #expand key
            for i in range(1, rounds+1):
                exp_key.extend(key_schedule_rounds(list(self._key), 0, i))

            cipher = AESCipher(exp_key)
            self._textin1 = bytearray(cipher.cipher_block(list(self._textin1)))

            if self.num_group1 > 0:
                self.num_group1 -= 1

        else:
            self._textin = self._interleavedPlaintext
            if self.num_group2 > 0:
                self.num_group2 -= 1

        # Check key works with target
        self.validateKey()

        return self._key, self._textin

    def next(self):
        """Returns the next key text pair

        Updates last key and text

        Returns:
            (key (bytearray), text (bytearray))

        .. versionadded:: 5.1
            Added next
        """
        return self.new_pair()

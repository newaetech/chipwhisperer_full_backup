#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2013-2014, NewAE Technology Inc
# All rights reserved.
#
# Authors: Colin O'Flynn
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

from chipwhisperer.analyzer.attacks.models.aes.funcs import sbox, inv_sbox

from base import ModelsBase
from chipwhisperer.analyzer.attacks.models.aes.key_schedule import keyScheduleRounds
from chipwhisperer.common.utils.pluginmanager import Plugin


class AES128_8bit(ModelsBase, Plugin):
    _name = 'AES 128'

    LEAK_HW_SBOXOUT_FIRSTROUND = 1
    LEAK_HD_LASTROUND_STATE = 2
    LEAK_HD_SBOX_IN_OUT = 3
    LEAK_HD_SBOX_IN_SUCCESSIVE = 4
    LEAK_HD_SBOX_OUT_SUCCESSIVE = 5
    LEAK_HW_INVSBOXOUT_FIRSTROUND = 6

    hwModels_toStr = [None, 'LEAK_HW_SBOXOUT_FIRSTROUND', 'LEAK_HW_INVSBOXOUT_FIRSTROUND', 'LEAK_HD_LASTROUND_STATE',
                'LEAK_HD_SBOX_IN_OUT', 'LEAK_HD_SBOX_IN_SUCCESSIVE', 'LEAK_HD_SBOX_OUT_SUCCESSIVE']

    hwModels = {'HW: AES SBox Output, First Round (Enc)':LEAK_HW_SBOXOUT_FIRSTROUND,
                'HW: AES Inv SBox Output, First Round (Dec)':LEAK_HW_INVSBOXOUT_FIRSTROUND,
                'HD: AES Last-Round State':LEAK_HD_LASTROUND_STATE,
                'HD: AES SBox Input to Output':LEAK_HD_SBOX_IN_OUT,
                'HD: AES SBox Input i to i+1':LEAK_HD_SBOX_IN_SUCCESSIVE,
                'HD: AES SBox Output i to i+1':LEAK_HD_SBOX_OUT_SUCCESSIVE}

    SHIFT = []

    INVSHIFT = [0, 5, 10, 15, 4, 9, 14, 3, 8, 13, 2, 7, 12, 1, 6, 11]

    def __init__(self, model=LEAK_HW_SBOXOUT_FIRSTROUND):
        ModelsBase.__init__(self, 16, 256, model=model)

    def processKnownKey(self, inpkey):
        if self.model == self.LEAK_HD_LASTROUND_STATE:
            return keyScheduleRounds(inpkey, 0, 10)
        return inpkey

    def leakage(self, pt, ct, guess, bnum, state):

        if self.model == self.LEAK_HW_SBOXOUT_FIRSTROUND:
            # Classic HW of S-Box output
            return self.HW[sbox(pt[bnum] ^ guess)]

        elif self.model == self.LEAK_HW_INVSBOXOUT_FIRSTROUND:
            # HW Leakage of inverse S-Box (AES Decryption)
            return self.HW[inv_sbox(pt[bnum] ^ guess)]

        elif self.model == self.LEAK_HD_LASTROUND_STATE:
            # HD Leakage of AES State between 9th and 10th Round
            # Used to break SASEBO-GII / SAKURA-G
            st10 = ct[self.INVSHIFT[bnum]]
            st9 =  inv_sbox(ct[bnum] ^ guess)
            return self.HW[st9 ^ st10]

        elif self.model == self.LEAK_HD_SBOX_IN_OUT:
            # Leakage from HD of S-Box input to output
            st1 = pt[bnum] ^ guess
            st2 = sbox(st1)
            return self.HW[st1 ^ st2]

        elif self.model == self.LEAK_HD_SBOX_IN_SUCCESSIVE:
            pass

        elif self.model == self.LEAK_HD_SBOX_OUT_SUCCESSIVE:
            pass

        else:
            raise ValueError("Invalid model: %s" % str(self.model))

    # TODO: Use this
    def xtime(self, a):
        """xtime operation"""
        a %= 0x100
        b = 0
        if a & 0x80:
            b = 0x1b
        a <<= 1
        a &= 0xff
        a ^= b
        return a


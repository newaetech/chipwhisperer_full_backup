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

from chipwhisperer.analyzer.models.aes.funcs import sbox, inv_sbox
from chipwhisperer.analyzer.models.aes.key_schedule import keyScheduleRounds
try:
    # OrderedDict is new in 2.7
    from collections import OrderedDict
    dicttype = OrderedDict
except ImportError:
    dicttype = dict

LEAK_HW_SBOXOUT_FIRSTROUND = 1
LEAK_HD_LASTROUND_STATE = 2
LEAK_HD_SBOX_IN_OUT = 3
LEAK_HD_SBOX_IN_SUCCESSIVE = 4
LEAK_HD_SBOX_OUT_SUCCESSIVE = 5
LEAK_HW_INVSBOXOUT_FIRSTROUND = 6

leakagemodels = dicttype()
leakagemodels['HW: AES SBox Output, First Round (Enc)'] = 'LEAK_HW_SBOXOUT_FIRSTROUND'
leakagemodels['HW: AES Inv SBox Output, First Round (Dec)'] = 'LEAK_HW_INVSBOXOUT_FIRSTROUND'
leakagemodels['HD: AES Last-Round State'] = 'LEAK_HD_LASTROUND_STATE'
leakagemodels['HD: AES SBox Input to Output'] = 'LEAK_HD_SBOX_IN_OUT'
leakagemodels['HD: AES SBox Input i to i+1'] = 'LEAK_HD_SBOX_IN_SUCCESSIVE'
leakagemodels['HD: AES SBox Output i to i+1'] = 'LEAK_HD_SBOX_OUT_SUCCESSIVE'

numSubKeys = 16

##Generate this table with:
#HW = []
#for n in range(0, 256):
#    HW = HW + [bin(n).count("1")]
HW8Bit = [0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4, 1, 2, 2, 3, 2, 3, 3,
          4, 2, 3, 3, 4, 3, 4, 4, 5, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4,
          4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 1, 2, 2, 3, 2,
          3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5,
          4, 5, 5, 6, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4,
          5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3,
          3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 2,
          3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6,
          4, 5, 5, 6, 5, 6, 6, 7, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5,
          6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 3, 4, 4, 5, 4, 5,
          5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6,
          7, 7, 8]

SHIFT = []

INVSHIFT = [0, 5, 10, 15, 4, 9, 14, 3, 8, 13, 2, 7, 12, 1, 6, 11]


def processKnownKey(setting, inpkey):

    if setting == LEAK_HD_LASTROUND_STATE:
        return keyScheduleRounds(inpkey, 0, 10)

    return inpkey

def leakage(pt, ct, guess, bnum, setting, state):

    if setting == LEAK_HW_SBOXOUT_FIRSTROUND:
        # Classic HW of S-Box output
        return getHW(sbox(pt[bnum] ^ guess))

    elif setting == LEAK_HW_INVSBOXOUT_FIRSTROUND:
        # HW Leakage of inverse S-Box (AES Decryption)
        return getHW(inv_sbox(pt[bnum] ^ guess))

    elif setting == LEAK_HD_LASTROUND_STATE:
        # HD Leakage of AES State between 9th and 10th Round
        # Used to break SASEBO-GII / SAKURA-G
        st10 = ct[INVSHIFT[bnum]]
        st9 =  inv_sbox(ct[bnum] ^ guess)
        return getHW(st9 ^ st10)

    elif setting == LEAK_HD_SBOX_IN_OUT:
        # Leakage from HD of S-Box input to output
        st1 = pt[bnum] ^ guess
        st2 = sbox(st1)
        return getHW(st1 ^ st2)

    elif setting == LEAK_HD_SBOX_IN_SUCCESSIVE:
        pass

    elif setting == LEAK_HD_SBOX_OUT_SUCCESSIVE:
        pass

    else:
        raise ValueError("Invalid setting: %s" % str(setting))

def getHW(var):
    """Given a variable, return the hamming weight (number of 1's)"""
    return HW8Bit[var]

def VccToGnd(var):
    """Convert from number of 1's to number of 0's... used when shunt inserted in GND path"""
    return 8 - var

# TODO: Use this
def xtime(a):
    """xtime operation"""
    a %= 0x100
    b = 0
    if a & 0x80:
        b = 0x1b
    a <<= 1
    a &= 0xff
    a ^= b
    return a

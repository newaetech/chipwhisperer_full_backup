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

import sys
import chipwhisperer.common.aes_tables as aes_tables

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

def HypHW(pt, ct, key, bnum):
    """Given either plaintext or ciphertext (not both) + a key guess, return hypothetical hamming weight of result"""
    if pt != None:
        return getHW(aes_tables.sbox[pt[bnum] ^ key])
    elif ct != None:
        return getHW(aes_tables.i_sbox[ct[bnum] ^ key])
    else:
        raise ValueError("Must specify PT or CT")

def HypHWXtime(pt, keyguess, numguess, keyknown, bnumknown):
    """Given plaintext + a subkey guess + a known subkey + subkey numbers return xtime result"""
    a = aes_tables.sbox[pt[numguess] ^ keyguess]
    b = aes_tables.sbox[pt[bnumknown] ^ keyknown]
    raise ValueError("Should this be HW instead of just xtime()???")
    return getHW(xtime(a^b))

def HypHD(pt, ct, key, bnum):
    """Given either plaintext or ciphertext (not both) + a key guess, return hypothetical hamming distance of result"""
    #Get output
    if pt != None:
        raise ValueError("First-Round HD isn't possible")
    elif ct != None:
        st10 = ct[INVSHIFT[bnum]]
        st9 =  aes_tables.i_sbox[ct[bnum] ^ key]
        return getHW(st9 ^ st10)
    else:
        raise ValueError("Must specify PT or CT")

def getHW(var):
    """Given a variable, return the hamming weight (number of 1's)"""
    return HW8Bit[var]

def VccToGnd(var):
    """Convert from number of 1's to number of 0's... used when shunt inserted in GND path"""
    return 8-var

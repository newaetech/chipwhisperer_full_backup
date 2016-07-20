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
from chipwhisperer.common.utils import util

numSubKeys = 16
permPerSubkey = 256

#Generate Hamming Weight LUT:
HW8Bit = []
for n in range(0, 256):
   HW8Bit += [bin(n).count("1")]


def processKnownKey(setting, inpkey):
    return keyScheduleRounds(inpkey, 0, 10)


def leakage(pt, ct, guess, bnum, setting, state):
        return HW8Bit[sbox(pt[bnum] ^ guess)]


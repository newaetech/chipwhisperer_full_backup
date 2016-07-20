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

LEAK_HW_SBOXOUT_FIRSTROUND = 1
LEAK_HD_LASTROUND_STATE = 2
LEAK_HD_SBOX_IN_OUT = 3
LEAK_HD_SBOX_IN_SUCCESSIVE = 4
LEAK_HD_SBOX_OUT_SUCCESSIVE = 5
LEAK_HW_INVSBOXOUT_FIRSTROUND = 6

leakagemodels = util.DictType()
leakagemodels['HW: AES SBox Output, First Round (Enc)'] = 'LEAK_HW_SBOXOUT_FIRSTROUND'
leakagemodels['HW: AES Inv SBox Output, First Round (Dec)'] = 'LEAK_HW_INVSBOXOUT_FIRSTROUND'
leakagemodels['HD: AES Last-Round State'] = 'LEAK_HD_LASTROUND_STATE'
leakagemodels['HD: AES SBox Input to Output'] = 'LEAK_HD_SBOX_IN_OUT'
leakagemodels['HD: AES SBox Input i to i+1'] = 'LEAK_HD_SBOX_IN_SUCCESSIVE'
leakagemodels['HD: AES SBox Output i to i+1'] = 'LEAK_HD_SBOX_OUT_SUCCESSIVE'

numSubKeys = 16
permPerSubkey = 256

#Generate Hamming Weight LUT:
HW8Bit = []
for n in range(0, 256):
   HW8Bit += [bin(n).count("1")]


def processKnownKey(setting, inpkey):

    if setting == LEAK_HD_LASTROUND_STATE:
        return keyScheduleRounds(inpkey, 0, 10)

    return inpkey


def leakage(pt, ct, guess, bnum, setting, state):

    if setting == LEAK_HW_SBOXOUT_FIRSTROUND:
        # Classic HW of S-Box output
        return HW8Bit[sbox(pt[bnum] ^ guess)]
    else:
        raise ValueError("Invalid setting: %s" % str(setting))


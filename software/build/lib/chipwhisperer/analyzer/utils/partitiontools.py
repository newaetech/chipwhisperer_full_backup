#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2016, NewAE Technology Inc
# All rights reserved.
#
# Authors: Colin O'Flynn
#
# Find this and more at newae.com - this file is part of the chipwhisperer
# project, http://www.chipwhisperer.com
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

from chipwhisperer.analyzer.attacks.models.AES128_8bit import AES128_8bit

import random

class base(object):
    def __init__(self, trace_manager, bnum):
        self.tm = trace_manager
        self._bnum = bnum
        self.num_parts = 2

    def new_run(self):
        pass

    def get_partition(self, trace_num, key_guess=None):
        return random.randint(0, 2)

class HWAES(base):
    def __init__(self, trace_manager, bnum, model, bmask=0xff):
        self.tm = trace_manager
        self._bnum = bnum
        self._bmask = bmask
        self.num_parts = bin(bmask).count('1') + 1
        self.aes = AES128_8bit(model, bmask)

    def get_partition(self, trace_num, key_guess = None):
        bnum = self._bnum
        key = self.tm.getKnownKey(trace_num)
        pt = self.tm.getTextin(trace_num)
        ct = self.tm.getTextout(trace_num)

        key = self.aes.processKnownKey(key)
        state = {"knownkey":key}

        return self.aes.leakage(pt, ct, key_guess, bnum, state)


#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2018, NewAE Technology Inc
# All rights reserved.
#
# Authors: Colin O'Flynn
#
# Find this and more at newae.com - this file is part of the chipwhisperer
# project, http://www.github.com/newaetech/chipwhisperer
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

import numpy as np
from chipwhisperer.common.api.ProjectFormat import Project
from chipwhisperer.common.traces import Trace


def calculate_snr(input, leak_model, bnum=0, db=True):
    """Calculate the SNR based on the leakage model.

    Uses same leakage model as the CPA attack.

    Args:
        input (Iterable of :class:`Traces <chipwhisperer.common.traces.Trace>`): An iterable of traces.
        leak_model (ModelsBase): A leakage model selected from
            :data:`leakage_models <chipwhisperer.analyzer.leakage_models>`.
        bnum (int): Byte number used for leakage model.
        bd (bool): Return signal-to-noise ratio in decibals.
    """

    textin = None
    textout = None
    key = None
    trace = None

    hwarray = []

    tm = None

    ntrace = len(input)
    npoints = len(input[0].wave)

    for tnum in range(0, ntrace):
        trace = input[tnum].wave
        textin = input[tnum].textin
        textout = input[tnum].textout
        key = input[tnum].key

        state = {'knownkey':key}

        leakage = leak_model.leakage(textin, textout, None, bnum, state)

        while leakage >= len(hwarray):
            hwarray.append([])

        hwarray[leakage].append(trace)

    hwmean = np.zeros((len(hwarray), npoints))

    for i in range(0, len(hwarray)):
        hwmean[i] = np.mean(hwarray[i], axis=0)

    inc_list = []
    best_choice = -1
    best_choice_len = 0
    for i in range(0, len(hwarray)):
        num_elements = len(hwarray[i])
        if num_elements > 0:
            inc_list.append(i)
            #Figure out if this is largest list as well
            if num_elements > best_choice_len:
                best_choice = i
                best_choice_len = num_elements


    hwmean_valid = hwmean[inc_list]

    signal_var = np.var(hwmean_valid, axis=0)
    noise_var_onehw = np.var(hwarray[best_choice], axis=0)

    snr = signal_var / noise_var_onehw

    if db:
        return 20*np.log(snr)

    return snr

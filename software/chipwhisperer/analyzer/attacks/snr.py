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

def calculate_snr(trace_manager, leak_model, bnum=0, db=True, trace_data=None, textin_data=None, textout_data=None, key_data=None):
    """Calculate the SNR based on the leakage model. Uses same leakage model as the CPA attack."""

    textin = None
    textout = None
    key = None
    trace = None

    hwarray = []

    if trace_manager:
        ntrace = trace_manager.num_traces()
        npoints = trace_manager.num_points()
    else:
        ntrace = len(trace_data)
        npoints = len(trace_data[0])

    for tnum in range(0, ntrace):

        if trace_manager is None:
            if trace_data:
                trace = trace_data[tnum]
            if textin_data:
                textin = textin_data[tnum]
            if textout_data:
                textout = textout_data[tnum]
            if key_data:
                key = key_data[tnum]
        else:
            trace = trace_manager.get_trace(tnum)
            textin = trace_manager.get_textin(tnum)
            textout = trace_manager.get_textout(tnum)
            key = trace_manager.get_known_key(tnum)

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

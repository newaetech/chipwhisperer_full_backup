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


import numpy as np

def partition_traces(tracemanager, ptool, start_trace=0, end_trace=None, index_only=False, key_guess=None):
    """
    Partitions a bunch of traces into two (or more) groups.

    Args:
        tracemanager: traceManager object.
        ptool: partition tool object.
        start_trace: Starting trace number.
        end_trace: Ending trace number.
        index_only: Specifies if return object is all trace data (huge memory) or just indexes
        key_guess: Overrides known-key with specified guess, None if you want to use known-key for partition.

    Returns: Copy of various partitions.

    """
    ptool.new_run()
    if end_trace is None:
        end_trace = tracemanager.numTraces()

    groups = [list() for _ in xrange(ptool.num_parts)]

    for i in range(start_trace, end_trace):
        gnum = ptool.get_partition(i, key_guess=key_guess)
        if index_only:
            data = i
        else:
            data = tracemanager.getTrace(i)
        groups[gnum].append(data)

    return groups


def wttest(groups, axis=0):
    """
    Performs Welsch T-Test against two groups
    Args:
        groups: List of groups (returned from partition_traces).
        axis: 'axis' argument passed to numpy var() and mean() functions.

    Returns: t-test data (same length of traces).

    """
    if len(groups) != 2:
        raise AttributeError( "Invalid number of groups for t-test, expecting 2, got %d" %len(groups))

    ttest = np.subtract(np.mean(groups[0], axis=axis), np.mean(groups[1], axis=axis))
    ttest /= np.sqrt((np.var(groups[0] ,axis=axis ) /len(groups[0])) + (np.var(groups[1], axis=axis ) /len(groups[1])))
    ttest = np.nan_to_num(ttest)

    return ttest

def dpa(groups, axis=0):
    """
    Performs Difference between two groups.
    Args:
        groups:
        axis:

    Returns:

    """

    if len(groups) != 2:
        raise AttributeError("dpa only works between two groups")

    dpa = np.subtract(np.mean(groups[1], axis=axis), np.mean(groups[0], axis=axis))
    return dpa
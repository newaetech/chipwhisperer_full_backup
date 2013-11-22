#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2013, Colin O'Flynn <coflynn@newae.com>
# All rights reserved.
#
# Find this and more at newae.com - this file is part of the chipwhisperer
# project, http://www.assembla.com/spaces/chipwhisperer
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

# This file takes a bunch of trace files, and generates lists of the measurement
# of current for each hamming weight (0,1,2,...,8). This can be used to then
# look for what the noise distribution is around each of the measurements, to
# check if we have gaussian noise as expected.
#
# Saves results in a .npy and .mat format (NumPY & MATLAB respectively) for
# processing.

import numpy as np
import scipy.io as sio
import os
import sys

sys.path.append('../../../software/analyzerv2')
sys.path.append('../../../software/common')
import attacks.models.AES128_8bit as AES128_8bit

def myFunc(directory, prefix, hwlist, bnum, ptlist):
    traces = np.load(directory + prefix + "traces.npy")
    textin = np.load(directory + prefix + "textin.npy")
    key = np.load(directory + prefix + "knownkey.npy")
    #Following may be required if traces differs from length of textin
    traces = traces[0:-1,:]

    for i,t in enumerate(textin):
        hwans = AES128_8bit.getHW(AES128_8bit.HypHW(t, None, key[bnum], bnum))
        pt = traces[i, ptlist[bnum]]
        hwlist[hwans].append(pt)

    return hwlist

def main(tracedir, bnum, ptlist):    
    preflist = []
    #files = [f for f in os.listdir(tracedir) if os.path.isfile(f)]
    files = os.listdir(tracedir)
    for f in files:
        if "_traces.npy" in f:
            f = f.replace("traces.npy", "")
            preflist.append(f)

    if len(preflist) == 0:
        print "WARNING: No traces found. Check directory."

    hwlist = [[]]*9
    for i in range(0,9):
        hwlist[i] = []

    for indx,pref in enumerate(preflist):
        myFunc(tracedir, pref, hwlist, bnum, ptlist)
        print "File %d/%d"%(indx, len(preflist))

    sio.savemat("hwlist_bnum=%d.mat"%bnum, {'hwlist':hwlist, 'bnum':bnum})
    np.save("hwlist_bnum=%d"%bnum, hwlist)

if __name__ == "__main__":
    # Point this to the 500 000 traces recorded of AES-128 on Mega328P
    # Available from ChipWhisperer.com
    directory = "../../../../traces_500000/traces/"

    # Interesting points, each point corresponds to where in the point-list
    # we've found the expected correlation (e.g. peak). Note some of these
    # are possibly wrong (e.g. ptlist[3] is off). Was found manually by
    # running an attack, and looking where the correlation peak is.
    ptlist = [26, 126, 301, 2594, 426, 526, 626, 726, 826, 926, 1025, 1126,
              1225, 1326, 1501, 1526]

    #Generate for one byte
    #main(directory, 5, ptlist)

    #Generate for all bytes. Takes a while.
    for bnum in range(0,8):
        main(directory, bnum, ptlist)
   


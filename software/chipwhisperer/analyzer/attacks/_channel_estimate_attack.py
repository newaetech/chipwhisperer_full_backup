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
import os
import sys
from operator import itemgetter


class ChannelEstimateAttackOneSubkey(object):
    """ Redos entire attack """

    def __init__(self):        
        self.clearStats()
        
    def clearStats(self):
        pass
    
    def oneSubkey(self, bnum, traces_fit, traces_test, plaintexts_fit, plaintexts_test,
                  model, modeltype="Hamming Weight",
                  key=None, printData=False, aroundStartEnd=False, useSVD=True, tracefitPInv=False,
                  progressBar=None, pbcnt=0):

        if tracefitPInv:
            ntracesfit = len(traces_fit[0,:])
        else:
            ntracesfit = len(traces_fit[:,0])
        ntracestry = len(traces_test[:,0])

        if printData:
            print(" Key %d = %2x"%(bnum, key[bnum]))

        start = 0
        end = 256

        if aroundStartEnd:
            start = key[bnum] - 5
            end = key[bnum] + 6

        if start < 0:
            start = 0
        
        if end > 256:
            end = 256

        result = []

        diffs = [1E99]*256

        for guess in range(start, end):
            if printData:
                print("  %2x "%guess, end=' ')
                
            hwlst = np.zeros( (ntracesfit, 1) )

            for i,t in enumerate(plaintexts_fit):
                hwlst[i][0] = AES128_8bit.HypHW(t, None, guess, bnum)
    
            if useSVD:
                if tracefitPInv:
                    #traces_fit already a pseudoinverse
                    channel = np.dot(traces_fit, hwlst)
                else:
                    #Use pseudo-inverse which is fairly fast, as NumPY uses a SVD decomposition
                    channel = np.dot(np.linalg.pinv(traces_fit), hwlst) 
            else:
                #Classic but VERY SLOW least-squares
                channel = np.linalg.lstsq(traces_fit, hwlst)[0]           

            hwtry = np.zeros( (ntracestry, 1) )
            for i,t in enumerate(plaintexts_test):
                hwtry[i][0] = AES128_8bit.HypHW(t, None, guess, bnum)

            res = np.dot(traces_test,channel)
            res = ((res-hwtry)**2).sum()   

            diffs[guess] = res

            if printData:
                print("%E"%res)
                result.append({'sse':res, 'guess':guess, 'csi':channel})
    
        if printData:        
            sortedresult = sorted(result, key=itemgetter('sse'))
            print("answer @ rank %d"%map(itemgetter('guess'), sortedresult).index(key[bnum]))

        return (diffs, pbcnt)


#Hacks for main()
sys.path.append('../../common')
sys.path.append('../.')
from .models import AES128_8bit as AES128_8bit

def main(tracedir, fitrange, tryrange):    
    preflist = []

    files = os.listdir(tracedir)
    for f in files:
        if "_traces.npy" in f:
            f = f.replace("traces.npy", "")
            preflist.append(f)

    if len(preflist) == 0:
        print("WARNING: No traces found. Check directory.")

    prefix = preflist[0]

    traces = np.load(tracedir + prefix + "traces.npy")
    textin = np.load(tracedir + prefix + "textin.npy")
    key = np.load(tracedir + prefix + "knownkey.npy")
    
    #Majority of traces used in generating estimated channel
    tracesfit = traces[fitrange[0]:fitrange[1],:]
    textinfit = textin[fitrange[0]:fitrange[1],:]

    #Select a few traces to use a test
    tracestry = traces[tryrange[0]:tryrange[1],:]
    textintry = textin[tryrange[0]:tryrange[1],:]

    tracesfit = np.linalg.pinv(tracesfit)

    cea = ChannelEstimateAttackOneSubkey()

    diffs = [0]*16
    blist = list(range(0, 16))

    for bnum in blist:
        print("Attacking byte %d"%bnum)
        cea.clearStats()

        #Example running an attack, don't set 'aroundStartEnd' if you want
        #the full attack and not quick cheater
        diffs[bnum] = cea.oneSubkey(bnum, tracesfit, tracestry, textinfit, textintry, AES128_8bit,
                      key=key, printData=False, aroundStartEnd=False, tracefitPInv=True)[0]

    #Sort Output
    output = [0]*16
    for bnum in blist:
        output[bnum] = []
        for i,d in enumerate(diffs[bnum]):
            output[bnum].append({'hyp':i, 'sse':d})

        output[bnum] = sorted(output[bnum], key=itemgetter('sse'))

    #Print most likely & PGE
    for bnum in blist:
        print(" %02x "%output[bnum][0]['hyp'], end=' ')
    print("")
    for bnum in blist:
        print("%3d "%map(itemgetter('hyp'), output[bnum]).index(key[bnum]), end=' ')
    print("")


if __name__ == "__main__":
    #Point to some example traces, available from ChipWhisperer.com

    #RSM-AES256 (DPAv4 Contest)
    directory = "/home/colin/Documents/chipwhisperer/git/chipwhisperer/software/scripting-examples/default-data-dir/traces-avr-dpav4-vcc/"
    fitrange = (0, 4000)
    tryrange = (4000, 10000)

    #Simple AES
    #directory = "/home/colin/Documents/chipwhisperer/git/chipwhisperer/software/scripting-examples/default-data-dir/traces-avr-aes128-vcc/"
    #fitrange = (0, 100)
    #tryrange = (8000, 8150)

    #Input traces need to be split into a "fitting set" and a "test set"
    #A channel will be generated with the "fitting set", and later we'll
    #try then with the "test set". The two sets are used to avoid fitting
    #random noise, and check how things actually worked
    
    main(directory, fitrange, tryrange)

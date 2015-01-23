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
#=================================================


import sys
from datetime import datetime

try:
    from PySide.QtCore import *
    from PySide.QtGui import *
except ImportError:
    print "ERROR: PySide is required for this program"
    sys.exit()

from subprocess import Popen, PIPE

import numpy as np
import scipy as sp
from openadc.ExtendedParameter import ExtendedParameter

from attacks.AttackStats import DataTypeDiffs

class AttackCPA_Bayesian(QObject):
    """
    Bayesian CPA, as described in .
    """

    def __init__(self, model):
        super(AttackCPA_Bayesian, self).__init__()
        self.model = model

    def setByteList(self, brange):
        self.brange = brange

    def setKeyround(self, keyround):
        self.keyround = keyround

    def setModeltype(self, modeltype):
        self.modeltype = modeltype

    def addTraces(self, traces, plaintexts, ciphertexts, knownkeys=None, progressBar=None, pointRange=None, algo="log", tracesLoop=None):
        keyround=self.keyround
        modeltype=self.modeltype
        brange=self.brange

        traces = np.array(traces)
        plaintexts =np.array(plaintexts)
        ciphertexts =np.array(ciphertexts)

        foundkey = []

        self.all_diffs = range(0,16)

        if progressBar:
            pbcnt = 0
            progressBar.setMinimum(0)
            progressBar.setMaximum(len(brange) * 256)

        #For all bytes of key
        for bnum in brange:
            diffs = [0]*256

            #For each 0..0xFF possible value of the key byte
            for key in range(0, 256):
                #Initialize arrays & variables to zero
                sumstd = np.zeros(len(traces[0,:]), dtype=np.float64)
                hyp = [0] * len(traces[:, 0])

                #Generate hypotheticals
                for tnum in range(len(traces[:,0])):

                    if len(plaintexts) > 0:
                        pt = plaintexts[tnum]

                    if len(ciphertexts) > 0:
                        ct = ciphertexts[tnum]

                    if keyround == "first":
                        ct = None
                    elif keyround == "last":
                        pt = None
                    else:
                        raise ValueError("keyround invalid")

                    #Generate the output of the SBOX
                    if modeltype == "Hamming Weight":
                        hypint = self.model.HypHW(pt, ct, key, bnum);
                    elif modeltype == "Hamming Distance":
                        hypint = self.model.HypHD(pt, ct, key, bnum);
                    else:
                        raise ValueError("modeltype invalid")

                    hyp[tnum] = hypint

                hyp = np.array(hyp)

                #Mean & STD-DEV of hypothesis
                meanh = np.mean(hyp, dtype=np.float64)
                stddevh = np.std(hyp, dtype=np.float64)

                #Mean & STD-DEV of all points in trace
                meant = np.mean(traces, axis=0, dtype=np.float64)
                stddevt = np.std(traces, axis=0, dtype=np.float64)

                #For each trace, do the following
                for tnum in range(len(traces[:,0])):
                    #Make both zero-mean
                    hdiff = np.float64(hyp[tnum]) - meanh
                    tdiff = np.array((np.array(traces[tnum,:], dtype=np.float64) - meant), dtype=np.float64)

                    #Scale by std-dev
                    hdiff = hdiff / np.float64(stddevh)
                    tdiff = tdiff / np.array(stddevt, dtype=np.float64)

                    # Calculate error term
                    error = (hdiff - tdiff)
                    sumstd = sumstd + pow(error, 2)

                q = len(traces[:,0])

                if algo == "original":
                    #Original Algorithm
                    sumstd = pow(np.sqrt((sumstd/q)), -q)
                elif algo == "log":
                    #LOG Algorithm
                    sumstd = -q * ( (0.5*np.log(sumstd)) - np.log(q))
                else:
                    raise RuntimeError("algo not defined")

                if progressBar:
                    progressBar.setValue(pbcnt)
                    #progressBar.setLabelText("Byte %02d: Hyp=%02x"%(bnum, key))
                    pbcnt = pbcnt + 1
                    if progressBar.wasCanceled():
                        raise KeyboardInterrupt

                diffs[key] = sumstd

            #Gotten all stddevs - now process algorithm

            #Original algorithm
            if algo == "original":
                sums = np.sum(diffs, axis=0, dtype=np.float64)
                for key in range(0, 256):
                    diffs[key] = diffs[key] / sums

            elif algo == "log":
            #Logarithm Algorithm
                summation = np.zeros(len(traces[0,:]), dtype=np.float64)
                for key in range(1, 256):
                    diff = diffs[key] - diffs[0]
                    summation = summation + np.exp(diff)
                summation = summation + 1
                summation = np.log(summation)
                summation = summation + diffs[0]

                for key in range(0, 256):
                    diffs[key] = diffs[key] - summation

            else:
                raise RuntimeError("algo not defined")

            self.all_diffs[bnum] = diffs
        self.algo = algo
            #From all the key candidates, select the largest difference as most likely
            #foundbyte = diffs.index(max(diffs))
            #foundkey.append(foundbyte)
#            print "%2x "%foundbyte,



    def _getResult(self, bnum, hyprange=None):
        if hyprange == None:
            hyprange = range(0,256)
        return [self.all_diffs[bnum][i] for i in hyprange];

    def getDiff(self, bnum, hyprange=None):
        if self.algo == "original":
            return self._getResult(bnum, hyprange)
        elif self.algo == "log":
            return np.exp(self._getResult(bnum, hyprange))
        else:
            raise RuntimeError("algo not defined")

    def getStatistics(self):
        t = [0]*16
        for i in self.brange:
            t[i] = self.getDiff(i)
        return t

    def setStatsReadyCallback(self, sr):
        pass

class AttackCPA_SciPyCorrelation(QObject):
    """
    Instead uses correlation functions provided by SciPy.
    WARNING: DOES NOT WORK RIGHT NOW.
    """

    def __init__(self, model):
        super(AttackCPA_SciPyCorrelation, self).__init__()
        self.model = model

    def setByteList(self, brange):
        self.brange = brange

    def setKeyround(self, keyround):
        self.keyround = keyround

    def setModeltype(self, modeltype):
        self.modeltype = modeltype

    def addTraces(self, traces, plaintexts, ciphertexts, knownkeys=None, progressBar=None, pointRange=None, tracesLoop=None):
        keyround=self.keyround
        modeltype=self.modeltype
        brange=self.brange

        traces_all = np.array(traces)
        plaintexts =np.array(plaintexts)
        ciphertexts =np.array(ciphertexts)

        foundkey = []

        self.all_diffs = range(0,16)

        if progressBar:
            pbcnt = 0
            progressBar.setMinimum(0)
            progressBar.setMaximum(len(brange) * 256)

        numtraces = len(traces_all[:,0])
        numpoints =  len(traces_all[0,:])

        #For all bytes of key
        for bnum in brange:

            diffs = [0]*256

            if pointRange == None:
                traces = traces_all
                # padbefore = 0
                # padafter = 0
            else:
                traces = np.array(traces_all[:, pointRange[0] : pointRange[1]])
                # padbefore = pointRange[0]
                # padafter = len(traces_all[0, :]) - pointRange[1]
                # print "%d - %d (%d %d)"%( pointRange[0],  pointRange[1], padbefore, padafter)

            #For each 0..0xFF possible value of the key byte
            for key in range(0, 256):

                hyp = np.empty((numtraces, 1))


                #Generate hypotheticals
                for tnum in range(numtraces):

                    if len(plaintexts) > 0:
                        pt = plaintexts[tnum]

                    if len(ciphertexts) > 0:
                        ct = ciphertexts[tnum]

                    if keyround == "first":
                        ct = None
                    elif keyround == "last":
                        pt = None
                    else:
                        raise ValueError("keyround invalid")

                    #Generate the output of the SBOX
                    if modeltype == "Hamming Weight":
                        hypint = self.model.HypHW(pt, ct, key, bnum);
                    elif modeltype == "Hamming Distance":
                        hypint = self.model.HypHD(pt, ct, key, bnum);
                    else:
                        raise ValueError("modeltype invalid")
                    hyp[tnum, 0] = hypint

                if progressBar:
                    progressBar.setValue(pbcnt)
                    #progressBar.setLabelText("Byte %02d: Hyp=%02x"%(bnum, key))
                    pbcnt = pbcnt + 1
                    if progressBar.wasCanceled():
                        raise KeyboardInterrupt

                diffs[key] = sp.signal.correlate(traces, hyp, 'valid')[0,:]



            self.all_diffs[bnum] = diffs

            #From all the key candidates, select the largest difference as most likely
            #foundbyte = diffs.index(max(diffs))
            #foundkey.append(foundbyte)
#            print "%2x "%foundbyte,


    def getDiff(self, bnum, hyprange=None):
        if hyprange == None:
            hyprange = range(0,256)
        return [self.all_diffs[bnum][i] for i in hyprange];

    def getStatistics(self):
        t = [0]*16
        for i in self.brange:
            t[i] = self.getDiff(i)
        return t

    def setStatsReadyCallback(self, sr):
        pass

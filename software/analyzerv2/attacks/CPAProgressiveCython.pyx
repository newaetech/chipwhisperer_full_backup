#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2013, Colin O'Flynn <coflynn@newae.com>
# All rights reserved.
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

#cython: cdivision=True
#cython: boundscheck=False

import sys
from datetime import datetime

try:
    from PySide.QtCore import *
    from PySide.QtGui import *
except ImportError:
    print "ERROR: PySide is required for this program"
    sys.exit()
    
from subprocess import Popen, PIPE
sys.path.append('../common')
sys.path.append('../../openadc/controlsw/python/common')
sys.path.append('../common/traces')
imagePath = '../common/images/'

import numpy as np
cimport numpy as np
import scipy as sp
#Can't import square?
#from libc.math cimport square
from libc.math cimport sqrt

from ExtendedParameter import ExtendedParameter

#from joblib import Parallel, delayed


try:
    import pyqtgraph as pg
    import pyqtgraph.multiprocess as mp
    import pyqtgraph.parametertree.parameterTypes as pTypes
    from pyqtgraph.parametertree import Parameter, ParameterTree, ParameterItem, registerParameterType
    from pyqtgraph.parametertree.parameterTypes import ListParameter
    #print pg.systemInfo()    
except ImportError:
    print "ERROR: PyQtGraph is required for this program"
    sys.exit()

import attacks.models.AES128_8bit
import attacks.models.AES_RoundKeys
from attacks.AttackBaseClass import AttackBaseClass

from functools import partial

cdef np.float64_t square(np.float64_t input):
    return input*input

cdef class CPAProgressiveOneSubkey(object):

    cdef np.ndarray sumhq
    cdef np.ndarray sumtq
    cdef np.ndarray sumh
    cdef np.ndarray sumt
    cdef np.ndarray sumht
    cdef np.int totalTraces

    """This class is the basic progressive CPA attack, capable of adding traces onto a variable with previous data"""
    def __init__(self):   
        self.totalTraces = 0
    
    def oneSubkey(self, bnum, pointRange, np.ndarray[np.float64_t, ndim=2] traces_all, long numtraces, np.ndarray[np.uint8_t, ndim=2] plaintexts, np.ndarray[np.uint8_t, ndim=2] ciphertexts, keyround, modeltype, progressBar, model, pbcnt):
        
        #cdef np.float64_t[:,:] traces
        cdef np.ndarray[np.float64_t, ndim=2] traces
        cdef np.ndarray[np.float64_t, ndim=2] sumhq = self.sumhq
        cdef np.ndarray[np.float64_t, ndim=2] sumtq = self.sumtq
        cdef np.ndarray[np.float64_t, ndim=2] sumh = self.sumh
        cdef np.ndarray[np.float64_t, ndim=2] sumt = self.sumt
        cdef np.ndarray[np.float64_t, ndim=2] sumht = self.sumht
        
        
        if pointRange == None:
            traces = traces_all
            padbefore = 0
            padafter = 0
        else:
            traces = traces_all[:, pointRange[bnum][0] : pointRange[bnum][1]]
            padbefore = pointRange[bnum][0]
            padafter = len(traces_all[0,:]) - pointRange[bnum][1]
            #print "%d - %d (%d %d)"%( pointRange[bnum][0],  pointRange[bnum][1], padbefore, padafter)
    
        cdef int npoints = len(traces[0,:])
    
        if self.totalTraces == 0:
            sumhq = np.zeros((256,npoints), dtype=np.float64)
            sumtq = np.zeros((256,npoints), dtype=np.float64)
            sumh = np.zeros((256,npoints), dtype=np.float64)
            sumt = np.zeros((256,npoints), dtype=np.float64)
            sumht = np.zeros((256,npoints), dtype=np.float64)
    
        self.totalTraces += numtraces
    
        cdef int key
        cdef int totalTraces = self.totalTraces
        cdef np.float64_t[:] sumnum = np.zeros(npoints)
        cdef np.float64_t[:] sumden1 = np.zeros(npoints)
        cdef np.float64_t[:] sumden2 = np.zeros(npoints)
        cdef np.float64_t[:,:] diffs = np.zeros((256, npoints))
        cdef int i, j
        cdef np.float64_t[:] hyp = np.zeros(numtraces)
    
        #For each 0..0xFF possible value of the key byte
        for key in range(256):                   
    
            #Formula for CPA & description found in "Power Analysis Attacks"
            # by Mangard et al, page 124, formula 6.2.     
            #
            # This has been modified to reduce computational requirements such that adding a new waveform
            # doesn't require you to recalculate everything
            
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
                    hypint = model.HypHW(pt, ct, key, bnum);
                elif modeltype == "Hamming Distance":
                    hypint = model.HypHD(pt, ct, key, bnum);
                else:
                    raise ValueError("modeltype invalid")
                hyp[tnum] = hypint                            
            
            #print "working"
            for j in range(numtraces):
                for i in range(npoints):
                    sumt[key,i] += traces[j,i]
            
            for j in range(numtraces):
                for i in range(npoints):
                    sumh[key,i] += hyp[j]
                    
            for j in range(numtraces):
                for i in range(npoints):
                    sumht[key,i] += traces[j,i] * hyp[j]
            #print "."
    
            #WARNING: not casting to np.float64 causes algorithm degredation... always be careful
            #meanh = self.sumh[key] / np.float64(self.totalTraces)
            #meant = self.sumt[key] / np.float64(self.totalTraces)
    
            #numtraces * meanh * meant = sumh * meant
            #sumnum =  self.sumht[key] - meant*self.sumh[key] - meanh*self.sumt[key] + (self.sumh[key] * meant)
            #sumnum =  self.sumht[key] - meanh*self.sumt[key]
#            sumnum =  self.sumht[key] - meanh*self.sumt[key]
            #sumnum =  self.sumht[key] - self.sumh[key]*self.sumt[key] / np.float64(self.totalTraces)
            
            for i in range(npoints):
                sumnum[i] = totalTraces*sumht[key,i] - sumh[key,i]*sumt[key,i]
    
            for i in range(npoints):
                for j in range(numtraces):
                    sumhq[key,i] += hyp[j]*hyp[j]
                    sumtq[key,i] += traces[j,i]*traces[j,i]
    
            #numtraces * meanh * meanh = sumh * meanh
            #sumden1 = sumhq - (2*meanh*self.sumh) + (numtraces*meanh*meanh)
            #sumden1 = sumhq - (2*meanh*self.sumh) + (self.sumh * meanh)
            #sumden1 = sumhq - meanh*self.sumh    
            #similarly for sumden2     
            #sumden1 = self.sumhq[key] - meanh*self.sumh[key]
            #sumden2 = self.sumtq[key] - meant*self.sumt[key]
            #sumden = sumden1 * sumden2    

            for i in range(npoints):
                sumden1[i] = (square(sumh[key,i]) - totalTraces * sumhq[key,i])
                sumden2[i] = (square(sumt[key,i]) - totalTraces * sumtq[key,i])

    
            if progressBar:
                progressBar.setValue(pbcnt)
                #progressBar.setLabelText("Byte %02d: Hyp=%02x"%(bnum, key))
                pbcnt = pbcnt + 1
                if progressBar.wasCanceled():
                    raise KeyboardInterrupt
    
            #diffs[key] = sumnum / np.sqrt(sumden1 * sumden2)
            
            for i in range(npoints):
                diffs[key][i] = sumnum[i] / sqrt(sumden1[i]*sumden2[i])
    
            if padafter > 0:
                diffs[key] = np.concatenate([diffs[key], np.zeros(padafter)])
    
            if padbefore > 0:
                diffs[key] = np.concatenate([np.zeros(padbefore), diffs[key]])                    
        
        return (diffs, pbcnt)

class AttackCPA_Progressive(QObject):
    """
    CPA Attack done as a loop, but using an algorithm which can progressively add traces & give output stats
    """

    def __init__(self, model):
        super(AttackCPA_Progressive, self).__init__()
        self.model = model
        self.sr = None

    def setByteList(self, brange):
        self.brange = brange

    def setKeyround(self, keyround):
        self.keyround = keyround
    
    def setModeltype(self, modeltype):
        self.modeltype = modeltype
        
        #From all the key candidates, select the largest difference as most likely
        #foundbyte = diffs.index(max(diffs))
        #foundkey.append(foundbyte)
#            print "%2x "%foundbyte,
    
    def addTraces(self, traces, plaintexts, ciphertexts, progressBar=None, pointRange=None, tracesLoop=None):
        keyround=self.keyround
        modeltype=self.modeltype
        brange=self.brange
                                                                   
        traces_all = np.array(traces)
        plaintexts =np.array(plaintexts)
        ciphertexts =np.array(ciphertexts)

        foundkey = []

        self.all_diffs = range(0,16)


        tdiff = tracesLoop

        numtraces = len(traces_all[:,0])

        if progressBar:
            pbcnt = 0
            progressBar.setMinimum(0)
            progressBar.setMaximum(len(brange) * 256 * (numtraces/tdiff + 1))
            progressBar.setWindowFlags(Qt.WindowStaysOnTopHint)

        pbcnt = 0
        #r = Parallel(n_jobs=4)(delayed(traceOneSubkey)(bnum, pointRange, traces_all, numtraces, plaintexts, ciphertexts, keyround, modeltype, progressBar, self.model, pbcnt) for bnum in brange)
        #self.all_diffs, pb = zip(*r)
        #pbcnt = 0
        for bnum in brange:
            #CPAMemoryOneSubkey
            #CPASimpleOneSubkey
            #(self.all_diffs[bnum], pbcnt) = sCPAMemoryOneSubkey(bnum, pointRange, traces_all, numtraces, plaintexts, ciphertexts, keyround, modeltype, progressBar, self.model, pbcnt)
                        
            cpa = CPAProgressiveOneSubkey()
            
            tstart = 0
            tend = tdiff
            
            while tstart < numtraces:                                       
                if tend > numtraces:
                    tend = numtraces
                    
                if tstart > numtraces:
                    tstart = numtraces
                    
                
                (self.all_diffs[bnum], pbcnt) = cpa.oneSubkey(bnum, pointRange, traces_all[tstart:tend], tend-tstart, plaintexts[tstart:tend], ciphertexts[tstart:tend], keyround, modeltype, progressBar, self.model, pbcnt)
                tend += tdiff
                tstart += tdiff
                
                if self.sr is not None:
                    self.sr()

    def getDiff(self, bnum, hyprange=None):
        if hyprange == None:
            hyprange = range(0,256)
        return [self.all_diffs[bnum][i] for i in hyprange];
    
    def getStatistics(self):
        t = [0]*16
        for i in self.brange:
            try:
                t[i] = self.getDiff(i)
            except TypeError:
                t[i] = None
        return t
    
    def setStatsReadyCallback(self, sr):
        self.sr = sr
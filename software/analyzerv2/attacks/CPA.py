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
import scipy as sp
from ExtendedParameter import ExtendedParameter

#from joblib import Parallel, delayed

try:
    from pyqtgraph.parametertree import Parameter
    #print pg.systemInfo()    
except ImportError:
    print "ERROR: PyQtGraph is required for this program"
    sys.exit()

import attacks.models.AES128_8bit
import attacks.models.AES_RoundKeys
from attacks.AttackBaseClass import AttackBaseClass
from attacks.AttackStats import DataTypeDiffs

imagePath = '../common/images/'

from functools import partial

try:
    import pyximport; pyximport.install()
    import attacks.CPACython as CPACython
except ImportError:
    CPACython = None

class CPAProgressiveOneSubkey(object):
    """This class is the basic progressive CPA attack, capable of adding traces onto a variable with previous data"""
    def __init__(self):        
        self.sumhq = [0]*256
        self.sumtq = [0]*256
        self.sumt = [0]*256
        self.sumh = [0]*256
        self.sumht = [0]*256
        self.totalTraces = 0
    
    def oneSubkey(self, bnum, pointRange, traces_all, numtraces, plaintexts, ciphertexts, keyround, modeltype, progressBar, model, pbcnt):
    
        diffs = [0]*256
        self.totalTraces += numtraces
    
        if pointRange == None:
            traces = traces_all
            padbefore = 0
            padafter = 0
        else:
            traces = np.array(traces_all[:, pointRange[bnum][0] : pointRange[bnum][1]])
            padbefore = pointRange[bnum][0]
            padafter = len(traces_all[0,:]) - pointRange[bnum][1]
            #print "%d - %d (%d %d)"%( pointRange[bnum][0],  pointRange[bnum][1], padbefore, padafter)
    
        #For each 0..0xFF possible value of the key byte
        for key in range(0, 256):                
            #Initialize arrays & variables to zero
            sumnum = np.zeros(len(traces[0,:]))
            sumden1 = np.zeros(len(traces[0,:]))
            sumden2 = np.zeros(len(traces[0,:]))
    
            hyp = [0] * numtraces
    
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
                hyp[tnum] = model.getHW(hypint)
                
            hyp = np.array(hyp)                                
                
            self.sumt[key] += np.sum(traces, axis=0)
            self.sumh[key] += np.sum(hyp, axis=0)
            self.sumht[key] += np.sum(np.multiply(np.transpose(traces), hyp), axis=1)
    
            #WARNING: not casting to np.float64 causes algorithm degredation... always be careful
            #meanh = self.sumh[key] / np.float64(self.totalTraces)
            #meant = self.sumt[key] / np.float64(self.totalTraces)
    
            #numtraces * meanh * meant = sumh * meant
            #sumnum =  self.sumht[key] - meant*self.sumh[key] - meanh*self.sumt[key] + (self.sumh[key] * meant)
            #sumnum =  self.sumht[key] - meanh*self.sumt[key]
#            sumnum =  self.sumht[key] - meanh*self.sumt[key]
            #sumnum =  self.sumht[key] - self.sumh[key]*self.sumt[key] / np.float64(self.totalTraces)
            sumnum = self.totalTraces*self.sumht[key] - self.sumh[key]*self.sumt[key]
    
            self.sumhq[key] += np.sum(np.square(hyp),axis=0, dtype=np.float64)
            self.sumtq[key] += np.sum(np.square(traces),axis=0, dtype=np.float64)
    
            #numtraces * meanh * meanh = sumh * meanh
            #sumden1 = sumhq - (2*meanh*self.sumh) + (numtraces*meanh*meanh)
            #sumden1 = sumhq - (2*meanh*self.sumh) + (self.sumh * meanh)
            #sumden1 = sumhq - meanh*self.sumh    
            #similarly for sumden2     
            #sumden1 = self.sumhq[key] - meanh*self.sumh[key]
            #sumden2 = self.sumtq[key] - meant*self.sumt[key]
            #sumden = sumden1 * sumden2    

            sumden1 = (np.square(self.sumh[key]) - self.totalTraces * self.sumhq[key])
            sumden2 = (np.square(self.sumt[key]) - self.totalTraces * self.sumtq[key])
            sumden = sumden1 * sumden2

    
            if progressBar:
                progressBar.setValue(pbcnt)
                progressBar.updateStatus((self.totalTraces-numtraces, self.totalTraces), bnum)
                pbcnt = pbcnt + 1
                if progressBar.wasCanceled():
                    raise KeyboardInterrupt
                
                if progressBar.wasSkipped():
                    return (diffs, pbcnt)
    
            diffs[key] = sumnum / np.sqrt(sumden)
    
            if padafter > 0:
                diffs[key] = np.concatenate([diffs[key], np.zeros(padafter)])
    
            if padbefore > 0:
                diffs[key] = np.concatenate([np.zeros(padbefore), diffs[key]])                    
        
        return (diffs, pbcnt)

class AttackCPA_Progressive(QObject):
    """
    CPA Attack done as a loop, but using an algorithm which can progressively add traces & give output stats
    """
    paramListUpdated = Signal(list)

    def __init__(self, model):
        super(AttackCPA_Progressive, self).__init__()
        
        resultsParams = [{'name':'Reporting Interval', 'key':'reportinterval', 'type':'int', 'value':100},
                         {'name':'Iteration Mode', 'key':'itmode', 'type':'list', 'values':{'Depth-First':'df', 'Breadth-First':'bf'}, 'value':'bf'},
                         {'name':'Skip when PGE=0', 'key':'checkpge', 'type':'bool', 'value':False},                         
                         ]
        self.params = Parameter.create(name='Progressive CPA', type='group', children=resultsParams)
        ExtendedParameter.setupExtended(self.params, self)
        
        self.model = model
        self.sr = None
        
        self.stats = DataTypeDiffs()
        
    def paramList(self):
        return [self.params]

    def setByteList(self, brange):
        self.brange = brange

    def setKeyround(self, keyround):
        self.keyround = keyround
    
    def setModeltype(self, modeltype):
        self.modeltype = modeltype
    
    def addTraces(self, traces, plaintexts, ciphertexts, progressBar=None, pointRange=None):
        keyround=self.keyround
        modeltype=self.modeltype
        brange=self.brange
                                                                   
        traces_all = np.array(traces)
        plaintexts =np.array(plaintexts)
        ciphertexts =np.array(ciphertexts)

        foundkey = []

        self.all_diffs = range(0,16)


        tdiff = self.findParam('reportinterval').value()

        numtraces = len(traces_all[:,0])

        if progressBar:
            pbcnt = 0
            progressBar.setMinimum(0)
            progressBar.setMaximum(len(brange) * 256 * (numtraces/tdiff + 1))

        pbcnt = 0
        #r = Parallel(n_jobs=4)(delayed(traceOneSubkey)(bnum, pointRange, traces_all, numtraces, plaintexts, ciphertexts, keyround, modeltype, progressBar, self.model, pbcnt) for bnum in brange)
        #self.all_diffs, pb = zip(*r)
        #pbcnt = 0
        cpa = [None]*(max(brange)+1)
        for bnum in brange:
            cpa[bnum] = CPAProgressiveOneSubkey()
            
        brangeMap = [None]*(max(brange)+1)
        i = 1
        for bnum in brange:
            brangeMap[bnum] = i
            i += 1
            
        skipPGE = self.findParam('checkpge').value()
        bf = self.findParam('itmode').value() == 'bf'
        
        #bf specifies a 'breadth-first' search. bf means we search across each
        #subkey by only the amount of traces specified. Depth-First means we
        #search each subkey completely, then move onto the next.
        if bf:
            brange_df = [0]
            brange_bf = brange
        else:
            brange_bf = [0]
            brange_df = brange
        
        
        for bnum_df in brange_df:
            #CPAMemoryOneSubkey
            #CPASimpleOneSubkey
            #(self.all_diffs[bnum], pbcnt) = sCPAMemoryOneSubkey(bnum, pointRange, traces_all, numtraces, plaintexts, ciphertexts, keyround, modeltype, progressBar, self.model, pbcnt)
            
            tstart = 0
            tend = tdiff
            
            while tstart < numtraces:                                       
                if tend > numtraces:
                    tend = numtraces
                    
                if tstart > numtraces:
                    tstart = numtraces
                    
                
                for bnum_bf in brange_bf:
                    
                    if bf:
                        bnum = bnum_bf
                    else:
                        bnum = bnum_df
                
                    
                    skip = False
                    if (self.stats.simplePGE(bnum) != 0) or (skipPGE == False):                    
                        (data, pbcnt) = cpa[bnum].oneSubkey(bnum, pointRange, traces_all[tstart:tend], tend-tstart, plaintexts[tstart:tend], ciphertexts[tstart:tend], keyround, modeltype, progressBar, self.model, pbcnt)
                        self.stats.updateSubkey(bnum, data)
                    else:  
                        skip = True
                    
                    if progressBar.wasSkipped() or skip:
                        progressBar.clearSkipped()
                        pbcnt = brangeMap[bnum] * 256 * (numtraces/tdiff + 1)
                        tstart = numtraces
                
                tend += tdiff
                tstart += tdiff
                
                if self.sr is not None:
                    self.sr()
    
    def getStatistics(self):
        return self.stats
    
    def setStatsReadyCallback(self, sr):
        self.sr = sr

class AttackCPA_SimpleLoop(QObject):
    """
    CPA Attack done as a loop - the 'classic' attack provided for familiarity to textbook samples.
    This attack does not provide trace-by-trace statistics however, you can only gather results once
    all the traces have been run through the attack.
    """

    def oneSubkey(self, bnum, pointRange, traces_all, numtraces, plaintexts, ciphertexts, keyround, modeltype, progressBar, model, pbcnt):
    
        diffs = [0]*256
    
        if pointRange == None:
            traces = traces_all
            padbefore = 0
            padafter = 0
        else:
            traces = np.array(traces_all[:, pointRange[bnum][0] : pointRange[bnum][1]])
            padbefore = pointRange[bnum][0]
            padafter = len(traces_all[0,:]) - pointRange[bnum][1]
            #print "%d - %d (%d %d)"%( pointRange[bnum][0],  pointRange[bnum][1], padbefore, padafter)
    
        #For each 0..0xFF possible value of the key byte
        for key in range(0, 256):                
            #Initialize arrays & variables to zero
            sumnum = np.zeros(len(traces[0,:]))
            sumden1 = np.zeros(len(traces[0,:]))
            sumden2 = np.zeros(len(traces[0,:]))
    
            hyp = [0] * numtraces
    
            #Formula for CPA & description found in "Power Analysis Attacks"
            # by Mangard et al, page 124, formula 6.2.                         
            
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
                hyp[tnum] = model.getHW(hypint)
                
            hyp = np.array(hyp)                    
    
            #Mean of hypothesis
            meanh = np.mean(hyp, dtype=np.float64)
    
            #Mean of all points in trace
            meant = np.mean(traces, axis=0, dtype=np.float64)
                               
            #For each trace, do the following
            for tnum in range(numtraces):
                hdiff = (hyp[tnum] - meanh)
                tdiff = traces[tnum,:] - meant
    
                sumnum = sumnum + (hdiff*tdiff)
                sumden1 = sumden1 + hdiff*hdiff 
                sumden2 = sumden2 + tdiff*tdiff             
    
            if progressBar:
                progressBar.setValue(pbcnt)
                #progressBar.setLabelText("Byte %02d: Hyp=%02x"%(bnum, key))
                pbcnt = pbcnt + 1
                if progressBar.wasCanceled():
                    raise KeyboardInterrupt
    
            diffs[key] = sumnum / np.sqrt( sumden1 * sumden2 )
    
            if padafter > 0:
                diffs[key] = np.concatenate([diffs[key], np.zeros(padafter)])
    
            if padbefore > 0:
                diffs[key] = np.concatenate([np.zeros(padbefore), diffs[key]])                    
        
        return (diffs, pbcnt)

    def __init__(self, model):
        super(AttackCPA_SimpleLoop, self).__init__()
        self.model = model

    def setByteList(self, brange):
        self.brange = brange

    def setKeyround(self, keyround):
        self.keyround = keyround
    
    def setModeltype(self, modeltype):
        self.modeltype = modeltype
    
    def addTraces(self, traces, plaintexts, ciphertexts, progressBar=None, pointRange=None, tracesLoop=None):
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

        pbcnt = 0
        for bnum in brange:
            (data, pbcnt) = self.oneSubkey(bnum, pointRange, traces_all, numtraces, plaintexts, ciphertexts, keyround, modeltype, progressBar, self.model, pbcnt)
            #self.all_diffs[bnum]
            self.stats.updateSubkey(bnum, data)

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
    
    def addTraces(self, traces, plaintexts, ciphertexts, progressBar=None, pointRange=None, algo="log", tracesLoop=None):
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
                hyp = [0] * len(traces[:,0])           
                
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
                    
                    hyp[tnum] = self.model.getHW(hypint)
                    
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

                    #Calculate error term                   
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
    
    def addTraces(self, traces, plaintexts, ciphertexts, progressBar=None, pointRange=None, tracesLoop=None):
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
                padbefore = 0
                padafter = 0
            else:
                traces = np.array(traces_all[:, pointRange[bnum][0] : pointRange[bnum][1]])
                padbefore = pointRange[bnum][0]
                padafter = len(traces_all[0,:]) - pointRange[bnum][1]
                #print "%d - %d (%d %d)"%( pointRange[bnum][0],  pointRange[bnum][1], padbefore, padafter)

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
                    hyp[tnum, 0] = self.model.getHW(hypint)       

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

class AttackProgressDialog(QDialog):
    def __init__(self, parent=None):
        super(AttackProgressDialog, self).__init__(parent)
        self.min = 0
        self.max = 10
        self.abort = False
        self.skip = False
        
        #Qt.WindowCloseButtonHint | 
        self.setWindowFlags((self.windowFlags() | Qt.CustomizeWindowHint | Qt.WindowStaysOnTopHint) & ~(Qt.WindowContextHelpButtonHint))
        self.setWindowTitle("Analysis in Progress")
        self.setWindowIcon(QIcon(imagePath+"attack_transp.png"))
        
        layout = QVBoxLayout()
        clayout = QHBoxLayout()
        skip = QPushButton("Next Byte")
        cancel = QPushButton("Abort")
        cancel.clicked.connect(self.setCanceled)
        skip.clicked.connect(self.setSkipped)
        clayout.addStretch()
        clayout.addWidget(skip)
        clayout.addWidget(cancel)        
        self.pbar = QProgressBar()
        
        statusInfo = QVBoxLayout()
        self.byteNum = QLabel("Current Subkey = ?")
        self.traceNum = QLabel("Current Traces = ?") 
        statusInfo.addWidget(self.byteNum)
        statusInfo.addWidget(self.traceNum)       
        
        layout.addLayout(statusInfo)        
        layout.addWidget(self.pbar)
        layout.addLayout(clayout)
        
        self.setLayout(layout)
        
    def updateStatus(self, trace, byte):
        self.byteNum.setText("Current Subkey = %d"%byte)
        try:
            self.traceNum.setText("Current Trace = %d-%d"%(trace[0], trace[1]))
        except:    
            self.traceNum.setText("Current Trace = %d"%trace)        
        
    def setMinimumDuration(self, duration):
        pass
        
    def setMinimum(self, mv):
        self.pbar.setMinimum(mv)
    
    def setMaximum(self, mv):
        self.max = mv
        self.pbar.setMaximum(mv)
    
    def setValue(self, val):
        if self.isVisible() == False:
            self.show()
        
        self.pbar.setValue(val)
        if val == self.max:
            self.close()
            
        QApplication.processEvents()
    
    def setCanceled(self):
        self.abort = True
        
    def wasCanceled(self):
        return self.abort
        
    def setSkipped(self):
        self.skip = True
    
    def clearSkipped(self):
        self.skip = False
    
    def wasSkipped(self):        
        return self.skip
        


#TODO: This should be broken into a separate function I think
class CPA(AttackBaseClass):
    """Correlation Power Analysis Attack"""
            
    def __init__(self, parent=None, console=None):
        super(CPA, self).__init__(parent)
        self.console=console
        
    def debug(self, sr):
        if self.console is not None:
            self.console.append(sr)
        
    def setupParameters(self):      
        cpaalgos = {'Progressive':AttackCPA_Progressive, 'Simple':AttackCPA_SimpleLoop}
        
        if CPACython is not None:
            cpaalgos['Progressive-Cython'] = CPACython.AttackCPA_Progressive
        
        attackParams = [{'name':'CPA Algorithm', 'key':'CPA_algo', 'type':'list', 'values':cpaalgos, 'value':AttackCPA_Progressive, 'set':self.setAlgo},
                        #{'name':'Reporting Interval (if supported)', 'key':'CPA_tracesloop', 'type':'int', 'range':(0,10E9), 'value':100},                        
                        {'name':'Hardware Model', 'type':'group', 'children':[
                        {'name':'Crypto Algorithm', 'key':'hw_algo', 'type':'list', 'values':{'AES-128 (8-bit)':attacks.models.AES128_8bit}, 'value':'AES-128'},
                        {'name':'Key Round', 'key':'hw_round', 'type':'list', 'values':['first', 'last'], 'value':'first'},
                        {'name':'Power Model', 'key':'hw_pwrmodel', 'type':'list', 'values':['Hamming Weight', 'Hamming Distance'], 'value':'Hamming Weight'},
                        ]},
                       {'name':'Take Absolute', 'type':'bool', 'value':True},
                       
                       #TODO: Should be called from the AES module to figure out # of bytes
                       {'name':'Attacked Bytes', 'type':'group', 'children':
                         self.getByteList()                                                 
                        },                    
                      ]
        self.params = Parameter.create(name='Attack', type='group', children=attackParams)
        #Need 'showScriptParameter = None' for setupExtended call below
        self.showScriptParameter = None
        ExtendedParameter.setupExtended(self.params, self)
        
        self.setAlgo(self.findParam('CPA_algo').value())
            
    def setAlgo(self, algo):
        self.attack = algo(self.findParam('hw_algo').value())
        try:
            self.attackParams = self.attack.paramList()[0]
        except:
            self.attackParams = None

        self.paramListUpdated.emit(self.paramList())
                                                
    def processKnownKey(self, inpkey):
        if inpkey is None:
            return None
        
        if self.attackRound == 'last':
            return attacks.models.AES_RoundKeys.AES_RoundKeys().getFinalKey(inpkey)
        else:
            return inpkey
            
    def doAttack(self):        
        
        start = datetime.now()
        
        #TODO: support start/end point different per byte
        (startingPoint, endingPoint) = self.getPointRange(None)
        (startingTrace, endingTrace) = self.getTraceRange()
        
        #print "%d-%d"%(startingPoint, endingPoint)
        
        data = []
        textins = []
        textouts = []
        
        for i in range(startingTrace, endingTrace):
            d = self.trace.getTrace(i)
            
            if d is None:
                continue
            
            d = d[startingPoint:endingPoint]
            
            data.append(d)
            textins.append(self.trace.getTextin(i))
            textouts.append(self.trace.getTextout(i)) 
        
        #self.attack = AttackCPA_SimpleLoop(attacks.models.AES128_8bit)
         
        #Following still needs debugging
        #self.attack = AttackCPA_Bayesian(attacks.models.AES128_8bit)                        
        
        #Following attack DOES NOT WORK
        #self.attack = AttackCPA_SciPyCorrelation(attacks.models.AES128_8bit)
        
        #hwmodel = self.findParam('hw_algo').value()
        #self.attack = self.findParam('CPA_algo').value()(hwmodel)        
        
        self.attack.setByteList(self.bytesEnabled())
        self.attack.setKeyround(self.findParam('hw_round').value())
        self.attack.setModeltype(self.findParam('hw_pwrmodel').value())
        self.attack.setStatsReadyCallback(self.statsReady)
        
        #progress = QProgressDialog("Analyzing", "Abort", 0, 100)
        progress = AttackProgressDialog()
        progress.setWindowModality(Qt.WindowModal)
        progress.setMinimumDuration(1000)
        
        #TODO:  pointRange=self.TraceRangeList[1:17]
        
        try:
            self.attack.addTraces(data, textins, textouts, progress)
            end = datetime.now()
            self.debug("Attack Time: %s"%str(end-start)) 
        except KeyboardInterrupt:
            end = datetime.now()
            self.debug("Attack Aborted after %s"%str(end-start))
        
        self.attackDone.emit()
        
        
    def statsReady(self):
        self.statsUpdated.emit()
        QApplication.processEvents()

    def passTrace(self, powertrace, plaintext=None, ciphertext=None, knownkey=None):
        pass
    
    def getStatistics(self):
        return self.attack.getStatistics()
            
    def paramList(self):
        l = [self.params, self.pointsParams]
        if self.attackParams is not None:
            l.append(self.attackParams)
        return l
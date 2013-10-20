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

try:
    from PySide.QtCore import *
    from PySide.QtGui import *
except ImportError:
    print "ERROR: PySide is required for this program"
    sys.exit()
    
sys.path.append('../common')
sys.path.append('../../openadc/controlsw/python/common')

import numpy as np
import scipy as sp
from ExtendedParameter import ExtendedParameter


try:
    from pyqtgraph.parametertree import Parameter
except ImportError:
    print "ERROR: PyQtGraph is required for this program"
    sys.exit()

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
        self.clearStats()
        
    def clearStats(self):
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

class CPAProgressive(QObject):
    """
    CPA Attack done as a loop, but using an algorithm which can progressively add traces & give output stats
    """
    paramListUpdated = Signal(list)

    def __init__(self, model):
        super(CPAProgressive, self).__init__()
        
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


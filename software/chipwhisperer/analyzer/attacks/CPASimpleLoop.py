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

try:
    from PySide.QtCore import *
    from PySide.QtGui import *
except ImportError:
    print "ERROR: PySide is required for this program"
    sys.exit()

import numpy as np
from chipwhisperer.analyzer.attacks.AttackStats import DataTypeDiffs

class CPASimpleLoop(QObject):
    """
    CPA Attack done as a loop - the 'classic' attack provided for familiarity to textbook samples.
    This attack does not provide trace-by-trace statistics however, you can only gather results once
    all the traces have been run through the attack.
    """

    def __init__(self, model,showScriptParameter=None):
        super(CPASimpleLoop, self).__init__()
        self.model = model
        self.stats = DataTypeDiffs()

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
                hyp[tnum] = hypint
                
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
            self.stats.updateSubkey(bnum, data)
    
    def getStatistics(self):
        return self.stats
    
    def setStatsReadyCallback(self, sr):
        pass

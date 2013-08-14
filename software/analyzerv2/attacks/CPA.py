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
    
from subprocess import Popen, PIPE
sys.path.append('../common')
sys.path.append('../../openadc/controlsw/python/common')
sys.path.append('../common/traces')
imagePath = '../common/images/'

import numpy as np
from ExtendedParameter import ExtendedParameter

try:
    import pyqtgraph as pg
    import pyqtgraph.multiprocess as mp
    import pyqtgraph.parametertree.parameterTypes as pTypes
    from pyqtgraph.parametertree import Parameter, ParameterTree, ParameterItem, registerParameterType
    #print pg.systemInfo()    
except ImportError:
    print "ERROR: PyQtGraph is required for this program"
    sys.exit()

import attacks.models.AES128_8bit
from attacks.AttackBaseClass import AttackBaseClass

from functools import partial

class AttackCPA_SimpleLoop(QObject):
    """
    CPA Attack done as a loop - the 'classic' attack provided for familiarity to textbook samples.
    This attack does not provide trace-by-trace statistics however, you can only gather results once
    all the traces have been run through the attack.
    """

    def __init__(self, model):
        super(AttackCPA_SimpleLoop, self).__init__()
        self.model = model

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

        if progressBar:
            pbcnt = 0
            progressBar.setMinimum(0)
            progressBar.setMaximum(len(brange) * 256)

        numtraces = len(traces_all[:,0])

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
                        hypint = self.model.HypHW(pt, ct, key, bnum);
                    elif modeltype == "Hamming Distance":
                        hypint = self.model.HypHD(pt, ct, key, bnum);
                    else:
                        raise ValueError("modeltype invalid")
                    hyp[tnum] = self.model.getHW(hypint)
                    
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
                        break

                diffs[key] = sumnum / np.sqrt( sumden1 * sumden2 )

                if padafter > 0:
                    diffs[key] = np.concatenate([diffs[key], np.zeros(padafter)])

                if padbefore > 0:
                    diffs[key] = np.concatenate([np.zeros(padbefore), diffs[key]])                    
            
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


#TODO: This should be broken into a separate function I think
class CPA(AttackBaseClass):
    """Correlation Power Analysis Attack"""
            
    def __init__(self, parent=None, log=None):
        super(CPA, self).__init__(parent)
        self.log=log  
        
    def setupParameters(self):      
        attackParams = [{'name':'Hardware Model', 'type':'group', 'children':[
                        {'name':'Crypto Algorithm', 'type':'list', 'values':{'AES-128 (8-bit)':attacks.models.AES128_8bit}, 'value':'AES-128'},
                        {'name':'Key Round', 'type':'list', 'values':['first', 'last'], 'value':'first'},
                        {'name':'Power Model', 'type':'list', 'values':['HW-VCC', 'HW-GND', 'HD-VCC', 'HD-GND'], 'value':'HW-VCC'},
                        ]},
                       {'name':'Take Absolute', 'type':'bool', 'value':True},
                       
                       #TODO: Should be called from the AES module to figure out # of bytes
                       {'name':'Attacked Bytes', 'type':'group', 'children':
                         self.getByteList()                                                 
                        },                    
                      ]
        self.params = Parameter.create(name='Attack', type='group', children=attackParams)
        ExtendedParameter.setupExtended(self.params)
            
    def doAttack(self):        
        #TODO: support start/end point different per byte
        (startingPoint, endingPoint) = self.getPointRange(None)
        (startingTrace, endingTrace) = self.getTraceRange()
        
        #print "%d-%d"%(startingPoint, endingPoint)
        
        data = []
        textins = []
        textouts = []
        
        for i in range(startingTrace, endingTrace):
            d = self.trace.getTrace(i)
            #d = self.preprocess.processOneTrace(d)
            
            data.append(d[startingPoint:endingPoint])
            textins.append(self.trace.getTextin(i))
            textouts.append(self.trace.getTextout(i)) 
                                 
        self.attack = AttackCPA_SimpleLoop(attacks.models.AES128_8bit)
        self.attack.setByteList(self.bytesEnabled())
        self.attack.setKeyround("first")
        self.attack.setModeltype("Hamming Weight")
        
        progress = QProgressDialog("Analyzing", "Abort", 0, 100)
        progress.setWindowModality(Qt.WindowModal)
        progress.setMinimumDuration(1000)
        
        #TODO:  pointRange=self.TraceRangeList[1:17]
        self.attack.addTraces(data, textins, textouts, progress)
        
        self.attackDone.emit()

    def passTrace(self, powertrace, plaintext=None, ciphertext=None, knownkey=None):
        pass
    
    def getStatistics(self):
        return self.attack.getStatistics()
            
    def paramList(self):
        return [self.params, self.pointsParams]
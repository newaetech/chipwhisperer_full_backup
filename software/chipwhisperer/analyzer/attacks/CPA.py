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
sys.path.append('../common/traces')

import numpy as np
import scipy as sp
from openadc.ExtendedParameter import ExtendedParameter

#from joblib import Parallel, delayed

try:
    from pyqtgraph.parametertree import Parameter
    #print pg.systemInfo()    
except ImportError:
    print "ERROR: PyQtGraph is required for this program"
    sys.exit()

import chipwhisperer.analyzer.attacks.models.AES128_8bit as models_AES128_8bit
import chipwhisperer.analyzer.attacks.models.AES256_8bit as models_AES256_8bit
import chipwhisperer.analyzer.attacks.models.AES_RoundKeys as models_AES_RoundKeys
from chipwhisperer.analyzer.attacks.AttackBaseClass import AttackBaseClass
from chipwhisperer.analyzer.attacks.AttackProgressDialog import AttackProgressDialog

from chipwhisperer.analyzer.attacks.CPAProgressive import CPAProgressive
from chipwhisperer.analyzer.attacks.CPASimpleLoop import CPASimpleLoop

#TEMPORARY - NOT FOR REAL USE
from chipwhisperer.analyzer.attacks.CPAExperimentalChannelinfo import CPAExperimentalChannelinfo

from AttackGenericParameters import AttackGenericParameters

class CPA(AttackBaseClass, AttackGenericParameters):
    """Correlation Power Analysis Attack"""
            
    def __init__(self, parent=None, console=None, showScriptParameter=None):
        self.console=console
        self.showScriptParameter=showScriptParameter
        super(CPA, self).__init__(parent)
        
    def debug(self, sr):
        if self.console is not None:
            self.console.append(sr)
        
    def setupParameters(self):      
        cpaalgos = {'Progressive':CPAProgressive,
                    'Simple':CPASimpleLoop,
                    'Channel Info':CPAExperimentalChannelinfo}
        
        #if CPACython is not None:
        #    cpaalgos['Progressive-Cython'] = CPACython.AttackCPA_Progressive
        
        attackParams = [{'name':'CPA Algorithm', 'key':'CPA_algo', 'type':'list', 'values':cpaalgos, 'value':CPAProgressive, 'set':self.setAlgo},                   
                        {'name':'Hardware Model', 'type':'group', 'children':[
                        {'name':'Crypto Algorithm', 'key':'hw_algo', 'type':'list', 'values':{'AES-128 (8-bit)':models_AES128_8bit, 'AES-256 (8-bit)':models_AES256_8bit}, 'value':'AES-128', 'set':self.setHWAlgo},
                        {'name':'Key Round', 'key':'hw_round', 'type':'list', 'values':['first', 'last'], 'value':'first'},
                        {'name':'Power Model', 'key':'hw_pwrmodel', 'type':'list', 'values':['Hamming Weight', 'Hamming Distance', 'Hamming Weight (inverse)'], 'value':'Hamming Weight'},
                        ]},
                       {'name':'Take Absolute', 'type':'bool', 'value':True},
                       
                       #TODO: Should be called from the AES module to figure out # of bytes
                       {'name':'Attacked Bytes', 'type':'group', 'children':
                         self.getByteList()                                                 
                        },                    
                      ]
        self.params = Parameter.create(name='Attack', type='group', children=attackParams)
        ExtendedParameter.setupExtended(self.params, self)
        
        self.setAlgo(self.findParam('CPA_algo').value())
        self.updateBytesVisible()
            
    def setHWAlgo(self, algo):
        self.numsubkeys = algo.numSubKeys
        self.updateBytesVisible()

    def setAlgo(self, algo):        
        self.attack = algo(self.findParam('hw_algo').value(), showScriptParameter=self.showScriptParameter, parent=self)
        try:
            self.attackParams = self.attack.paramList()[0]
        except:
            self.attackParams = None

        self.paramListUpdated.emit(self.paramList())
                                                
    def processKnownKey(self, inpkey):
        if inpkey is None:
            return None
        
        if self.findParam('hw_round').value() == 'last':
            return models_AES_RoundKeys.AES_RoundKeys().getFinalKey(inpkey)
        else:
            return inpkey
            
    def doAttack(self):        
        
        start = datetime.now()
        
        #TODO: support start/end point different per byte
        (startingPoint, endingPoint) = self.getPointRange(None)
        
        self.attack.getStatistics().clear()
        
        for itNum in range(1, self.getIterations()+1):
            startingTrace = self.getTraceNum()*(itNum-1) + self.getTraceStart()
            endingTrace = self.getTraceNum()*itNum + self.getTraceStart()
            
            #print "%d-%d"%(startingPoint, endingPoint)            
            data = []
            textins = []
            textouts = []

            print "%d-%d"%(startingTrace, endingTrace)
            
            for i in range(startingTrace, endingTrace):
                d = self.trace.getTrace(i)
                
                if d is None:
                    continue
                
                d = d[startingPoint:endingPoint]
                
                data.append(d)
                textins.append(self.trace.getTextin(i))
                textouts.append(self.trace.getTextout(i)) 
            
            #self.attack.clearStats()
            self.attack.setByteList(self.bytesEnabled())
            self.attack.setKeyround(self.findParam('hw_round').value())
            self.attack.setModeltype(self.findParam('hw_pwrmodel').value())
            self.attack.setStatsReadyCallback(self.statsReady)
            
            progress = AttackProgressDialog()
            progress.setWindowModality(Qt.WindowModal)
            progress.setMinimumDuration(1000)
            progress.offset = startingTrace
            
            #TODO:  pointRange=self.TraceRangeList[1:17]
            try:
                self.attack.addTraces(data, textins, textouts, progress)
            except KeyboardInterrupt:
                self.debug("Attack ABORTED... stopping")
        
        end = datetime.now()
        self.debug("Attack Time: %s"%str(end-start)) 
        self.attackDone.emit()
        
        
    def statsReady(self):
        self.statsUpdated.emit()
        QApplication.processEvents()

    def passTrace(self, powertrace, plaintext=None, ciphertext=None, knownkey=None):
        pass
    
    def getStatistics(self):
        return self.attack.getStatistics()
            
    def paramList(self):
        l = [self.params, self.pointsParams, self.traceParams]
        if self.attackParams is not None:
            l.append(self.attackParams)
        return l

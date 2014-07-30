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

    def setupParameters(self):      
        cpaalgos = {'Progressive':CPAProgressive,
                    'Simple':CPASimpleLoop}

        #if CPACython is not None:
        #    cpaalgos['Progressive-Cython'] = CPACython.AttackCPA_Progressive
        
        attackParams = [{'name':'CPA Algorithm', 'key':'CPA_algo', 'type':'list', 'values':cpaalgos, 'value':CPAProgressive, 'set':self.updateAlgorithm},
                        {'name':'Hardware Model', 'type':'group', 'children':[
                        {'name':'Crypto Algorithm', 'key':'hw_algo', 'type':'list', 'values':{'AES-128 (8-bit)':models_AES128_8bit, 'AES-256 (8-bit)':models_AES256_8bit}, 'value':'AES-128', 'set':self.updateScript},
                        {'name':'Key Round', 'key':'hw_round', 'type':'list', 'values':['first', 'last'], 'value':'first', 'set':self.updateScript},
                        {'name':'Power Model', 'key':'hw_pwrmodel', 'type':'list', 'values':{'Hamming Weight':'HypHW', 'Hamming Distance':'HypHD'}, 'value':'Hamming Weight', 'set':self.updateScript},
                        {'name':'Direction', 'key':'aes_dir', 'type':'list', 'values':{'Encryption (PT=Input)':'enc', 'Decryption (PT=Input)':'dec'}, 'value':'enc', 'set':self.updateScript},
                        ]},
                       
                       #TODO: Should be called from the AES module to figure out # of bytes
                       {'name':'Attacked Bytes', 'type':'group', 'children':
                         self.getByteList()                                                 
                        },                    
                      ]

        self.params = Parameter.create(name='Attack', type='group', children=attackParams)
        ExtendedParameter.setupExtended(self.params, self)
        
        self.setAnalysisAlgorithm(CPAProgressive, None, None)
        self.updateBytesVisible()
        self.updateScript()

    def updateAlgorithm(self, algo):
        # TODO: this hack required in case don't have setReportingInterval
        # self.delFunction('init', 'attack.setReportingInterval')

        self.setAnalysisAlgorithm(algo, None, None)
        self.updateBytesVisible()
        self.updateScript()

    def setHardwareModel(self, model):
        self.numsubkeys = model.numSubKeys
        self.updateBytesVisible()
        self.updateScript()

    def setKeyround(self, rnd):
        self._keyround = rnd

    def keyround(self):
        return self._keyround

    def setTargetBytes(self, blist):
        self._targetbytes = blist

    def targetBytes(self):
        return self._targetbytes

    def setDirection(self, dir):
        self._direction = dir

    def direction(self):
        return self._direction

    def setAnalysisAlgorithm(self, analysisAlgorithm, hardwareModel, leakageModel):
        self.attack = analysisAlgorithm(hardwareModel, leakageModel, showScriptParameter=self.showScriptParameter, parent=self)

        try:
            self.attackParams = self.attack.paramList()[0]
        except:
            self.attackParams = None

        self.paramListUpdated.emit(self.paramList())
        
        if hasattr(self.attack, 'scriptsUpdated'):
            self.attack.scriptsUpdated.connect(self.updateScript)
            
    def updateScript(self, ignored=None):

        self.importsAppend("from chipwhisperer.analyzer.attacks.CPA import CPA")

        analysAlgoStr = self.findParam('CPA_algo').value().__name__
        hardwareStr = self.findParam('hw_algo').value().__name__
        leakModelStr = hardwareStr + "." + self.findParam('hw_pwrmodel').value()

        self.importsAppend("from chipwhisperer.analyzer.attacks.%s import %s" % (analysAlgoStr, analysAlgoStr))
        self.importsAppend("import %s" % hardwareStr)

        self.addFunction("init", "setAnalysisAlgorithm", "%s,%s,%s" % (analysAlgoStr, hardwareStr, leakModelStr), loc=0)
        self.addFunction("init", "setKeyround", "0")
        self.addFunction("init", "setDirection", "'%s'" % self.findParam('aes_dir').value())

        if hasattr(self.attack, '_smartstatements'):
            self.mergeGroups('init', self.attack, prefix='attack')

        self.addFunction("init", "setTraceManager", "userScript.traceManager()")
        self.addFunction("init", "setProject", "userScript.project()")

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
        
        self.attack.setTargetBytes(self.targetBytes())
        self.attack.setKeyround(self.keyround())
        self.attack.setDirection(self.direction())
        self.attack.setReportingInterval(self.getReportingInterval())

        self.attack.getStatistics().clear()
        self.attack.setStatsReadyCallback(self.statsReady)

        for itNum in range(1, self.getIterations()+1):
            startingTrace = self.getTraceNum() * (itNum - 1) + self.getTraceStart()
            endingTrace = self.getTraceNum() * itNum + self.getTraceStart()
            
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
            
            progress = AttackProgressDialog()
            progress.setWindowModality(Qt.WindowModal)
            progress.setMinimumDuration(1000)
            progress.offset = startingTrace
            
            #TODO:  pointRange=self.TraceRangeList[1:17]
            try:
                self.attack.addTraces(data, textins, textouts, progress)
            except KeyboardInterrupt:
                self.log("Attack ABORTED... stopping")
        
        end = datetime.now()
        self.log("Attack Time: %s" % str(end - start))
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

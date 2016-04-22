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

import chipwhisperer.analyzer.attacks.models.AES128_8bit as models_AES128_8bit
import chipwhisperer.common.utils.pluginmanager
from _base import AttackBaseClass
from chipwhisperer.common.api.config_parameter import ConfigParameter
from _generic_parameters import AttackGenericParameters
from chipwhisperer.common.utils import Util


class CPA(AttackBaseClass, AttackGenericParameters):
    """Correlation Power Analysis Attack"""

    name = "CPA"

    def __init__(self):
        AttackGenericParameters.__init__(self)
        AttackBaseClass.__init__(self)
        self.setAnalysisAlgorithm(self.findParam('CPA_algo').value(), None, None)
        self.updateBytesVisible()
        self.updateScript()

    def setupParameters(self):
        algos = chipwhisperer.common.utils.pluginmanager.getPluginsInDictFromPackage("chipwhisperer.analyzer.attacks.cpa_algorithms", False, False, self)
        return [    {'name':'Algorithm', 'key':'CPA_algo', 'type':'list', 'values':algos, 'value':algos["Progressive"], 'set':self.updateAlgorithm},
                    {'name':'Hardware Model', 'type':'group', 'children':[
                        {'name':'Crypto Algorithm', 'key':'hw_algo', 'type':'list', 'values':{'AES-128 (8-bit)':models_AES128_8bit}, 'value':'AES-128', 'set':self.updateScript},
                        {'name':'Leakage Model', 'key':'hw_leak', 'type':'list', 'values':models_AES128_8bit.leakagemodels, 'value':1, 'set':self.updateScript},
                        ]},

                   #TODO: Should be called from the AES module to figure out # of bytes
                   {'name':'Attacked Bytes', 'type':'group', 'children': self.getByteList()
                    },
                ]

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

    def setTargetBytes(self, blist):
        self._targetbytes = blist

    def targetBytes(self):
        return self._targetbytes

    def setAnalysisAlgorithm(self, analysisAlgorithm, hardwareModel, leakageModel):
        self.attack = analysisAlgorithm(hardwareModel, leakageModel)

        try:
            self.attackParams = self.attack.paramList()[0]
        except:
            self.attackParams = None

        self.paramListUpdated.emit(self.paramList())

        if hasattr(self.attack, 'scriptsUpdated'):
            self.attack.scriptsUpdated.connect(self.updateScript)

    def updateScript(self, ignored=None):
        self.importsAppend("from chipwhisperer.analyzer.attacks.cpa import CPA")

        analysAlgoStr = self.findParam('CPA_algo').value().__name__
        hardwareStr = self.findParam('hw_algo').value().__name__
        leakModelStr = hardwareStr + "." + self.findParam('hw_leak').value()

        self.importsAppend("from chipwhisperer.analyzer.attacks.cpa_algorithms.%s import %s" % (analysAlgoStr, analysAlgoStr))
        self.importsAppend("import %s" % hardwareStr)

        self.addFunction("init", "setAnalysisAlgorithm", "%s,%s,%s" % (analysAlgoStr, hardwareStr, leakModelStr), loc=0)

        if hasattr(self.attack, '_smartstatements'):
            self.mergeGroups('init', self.attack, prefix='attack')

        self.addFunction("init", "setTraceSource", "UserScript.ppOutput")
        # self.addFunction("init", "setProject", "UserScript.project()")

    def processKnownKey(self, inpkey):
        if inpkey is None:
            return None

        if hasattr(self.attack, 'processKnownKey'):
            return self.attack.processKnownKey(inpkey)
        else:
            return inpkey

    def doAttack(self, progressBar):
        self.attackStarted.emit()
        self.attack.setTargetBytes(self.targetBytes())
        self.attack.setReportingInterval(self.getReportingInterval())
        self.attack.getStatistics().clear()
        self.attack.setStatsReadyCallback(self.statsReady)

        for itNum in range(1, self.getIterations()+1):
            startingTrace = self.getTraceNum() * (itNum - 1) + self.getTraceStart()
            endingTrace = startingTrace + self.getTraceNum() - 1
            #TODO:  pointRange=self.TraceRangeList[1:17]
            self.attack.addTraces(self.traceSource(), (startingTrace, endingTrace), progressBar, pointRange=self.getPointRange())
            if progressBar and progressBar.wasAborted():
                return

        self.attackDone.emit()

    def statsReady(self):
        self.statsUpdated.emit()

    def passTrace(self, powertrace, plaintext=None, ciphertext=None, knownkey=None):
        pass

    def getStatistics(self):
        return self.attack.getStatistics()

    def paramList(self):
        l = [self.params, self.pointsParams, self.traceParams]
        if self.attackParams is not None:
            l.append(self.attackParams)
        return l

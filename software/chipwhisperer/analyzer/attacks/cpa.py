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
from chipwhisperer.common.utils import pluginmanager
from ._base import AttackBaseClass
from ._generic_parameters import AttackGenericParameters
from chipwhisperer.common.ui.ProgressBar import ProgressBar
from chipwhisperer.common.utils.parameter import setupSetParam


class CPA(AttackBaseClass, AttackGenericParameters):
    """Correlation Power Analysis Attack"""
    _name = "CPA"

    def __init__(self):
        AttackBaseClass.__init__(self)
        self.attack = None

        algos = pluginmanager.getPluginsInDictFromPackage("chipwhisperer.analyzer.attacks.cpa_algorithms", False, False)
        self.getParams().addChildren([
            {'name':'Algorithm', 'key':'CPA_algo', 'type':'list',  'values':algos, 'value':algos["Progressive"], 'action':self.updateAlgorithm}, #TODO: Should be called from the AES module to figure out # of bytes
        ])
        AttackGenericParameters.__init__(self)
        self.setAnalysisAlgorithm(self.findParam('CPA_algo').getValue(), None, None)
        self.updateBytesVisible()
        self.updateScript()

    def updateAlgorithm(self, parameter):
        self.setAnalysisAlgorithm(parameter.getValue(), None, None)
        self.updateBytesVisible()
        self.updateScript()

    def setHardwareModel(self, model):
        self.numsubkeys = model.numSubKeys
        self.updateBytesVisible()
        self.updateScript()

    def setAnalysisAlgorithm(self, analysisAlgorithm, hardwareModel, leakageModel):
        if self.attack is not None:
            self.attack.getParams().remove()
        self.attack = analysisAlgorithm(hardwareModel, leakageModel)

        try:
            self.attackParams = self.attack.paramList()[0]
        except:
            self.attackParams = None

        if hasattr(self.attack, 'scriptsUpdated'):
            self.attack.scriptsUpdated.connect(self.updateScript)

        self.getParams().append(self.attack.getParams())

    def updateScript(self, _=None):
        self.importsAppend("from chipwhisperer.analyzer.attacks.cpa import CPA")

        analysAlgoStr = self.attack.__class__.__name__
        hardwareStr = self.findParam(['Hardware Model','hw_algo']).getValue().__name__
        leakModelStr = hardwareStr + "." + self.findParam(['Hardware Model','hw_leak']).getValue()

        self.importsAppend("from %s import %s" % (sys.modules[self.attack.__class__.__module__].__name__, analysAlgoStr))
        self.importsAppend("import %s" % hardwareStr)

        if hasattr(self.attack, '_smartstatements'):
            self.mergeGroups('init', self.attack, prefix='attack')

        self.addFunction("init", "setAnalysisAlgorithm", "%s,%s,%s" % (analysAlgoStr, hardwareStr, leakModelStr), loc=0)
        self.addFunction("init", "setTraceSource", "UserScript.traces, blockSignal=True", loc=0)

    def processKnownKey(self, inpkey):
        if inpkey is None:
            return None

        if hasattr(self.attack, 'processKnownKey'):
            return self.attack.processKnownKey(inpkey)
        else:
            return inpkey

    def processTraces(self):
        progressBar = ProgressBar("Analysis in Progress", "Attaking with CPA:")
        with progressBar:
            self.attack.setTargetBytes(self.targetBytes())
            self.attack.setReportingInterval(self.getReportingInterval())
            self.attack.getStatistics().clear()
            self.attack.setStatsReadyCallback(self.sigAnalysisUpdated.emit)

            self.sigAnalysisStarted.emit()
            for itNum in range(1, self.getIterations()+1):
                startingTrace = self.getTracesPerAttack() * (itNum - 1) + self.getTraceStart()
                endingTrace = startingTrace + self.getTracesPerAttack() - 1
                #TODO:  pointRange=self.TraceRangeList[1:17]
                self.attack.addTraces(self.getTraceSource(), (startingTrace, endingTrace), progressBar, pointRange=self.getPointRange())
                if progressBar and progressBar.wasAborted():
                    return

        self.sigAnalysisDone.emit()

    def getStatistics(self):
        return self.attack.getStatistics()

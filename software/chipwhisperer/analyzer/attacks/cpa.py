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
from chipwhisperer.common.utils.parameter import setupSetParam
from ._base import AttackBaseClass
from chipwhisperer.common.ui.ProgressBar import ProgressBar


class CPA(AttackBaseClass):
    """Correlation Power Analysis Attack"""
    _name = "CPA"

    def __init__(self):
        AttackBaseClass.__init__(self)

        algos = pluginmanager.getPluginsInDictFromPackage("chipwhisperer.analyzer.attacks.cpa_algorithms", True, False)
        self.algo = algos["Progressive"]
        self.getParams().addChildren([
            {'name':'Algorithm', 'key':'CPA_algo', 'type':'list', 'values':algos, 'get':self.getAnalysisAlgorithm,
             'set':self.setAnalysisAlgorithm, 'action':self.updateScript, 'childmode':'child'},
        ])
        self.refreshByteList()
        self.setAnalysisAlgorithm(algos["Progressive"])

    def getAnalysisAlgorithm(self):
        return self.algo

    @setupSetParam('CPA_algo')
    def setAnalysisAlgorithm(self, analysisAlgorithm):
        self.algo = analysisAlgorithm

        if hasattr(self.algo, 'scriptsUpdated'):
            self.algo.scriptsUpdated.connect(self.updateScript)

    def updateScript(self, _=None):
        self.clearStatements()
        self.importsAppend("from chipwhisperer.analyzer.attacks.cpa import CPA")

        analysAlgoStr = self.algo.__class__.__name__

        self.importsAppend("from %s import %s" % (sys.modules[self.algo.__class__.__module__].__name__, analysAlgoStr))

        if hasattr(self.algo, '_smartstatements'):
            self.mergeGroups('init', self.algo, prefix='attack')

        self.addFunction("init", "setAnalysisAlgorithm", "%s()" % analysAlgoStr, loc=0)
        self.addFunction("init", "setTraceSource", "UserScript.traces, blockSignal=True", loc=0)
        self.addFunction("init", "setTargetSubkeys", self.getEnabledSubkeys())

    def processKnownKey(self, inpkey):
        if inpkey is None:
            return None

        if hasattr(self.algo, 'processKnownKey'):
            return self.algo.processKnownKey(inpkey)
        else:
            return inpkey

    def processTraces(self):
        progressBar = ProgressBar("Analysis in Progress", "Attaking with CPA:")
        with progressBar:
            self.algo.setReportingInterval(self.getReportingInterval())
            self.algo.getStatistics().clear()
            self.algo.setStatsReadyCallback(self.sigAnalysisUpdated.emit)

            self.sigAnalysisStarted.emit()
            for itNum in range(1, self.getIterations()+1):
                startingTrace = self.getTracesPerAttack() * (itNum - 1) + self.getTraceStart()
                endingTrace = startingTrace + self.getTracesPerAttack() - 1
                self.algo.setModel(self.findParam('hw_algo').getValue())
                self.algo.addTraces(self.getTraceSource(), (startingTrace, endingTrace), progressBar, pointRange=self.getPointRange(), brange=self.getEnabledSubkeys())
                if progressBar and progressBar.wasAborted():
                    return

        self.sigAnalysisDone.emit()

    def getStatistics(self):
        return self.algo.getStatistics()

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
        self._analysisAlgorithm = algos["Progressive"]
        self.getParams().addChildren([
            {'name':'Algorithm', 'key':'CPA_algo', 'type':'list',  'values':algos, 'get':self.getAnalysisAlgorithm, 'set':self.setAnalysisAlgorithm}
        ])
        self.setAnalysisAlgorithm(self.findParam('CPA_algo').getValue())
        self.updateScript()

    def getAnalysisAlgorithm(self):
        return self._analysisAlgorithm

    @setupSetParam('Algorithm')
    def setAnalysisAlgorithm(self, analysisAlgorithm):
        self._analysisAlgorithm = analysisAlgorithm

        if hasattr(self._analysisAlgorithm, 'scriptsUpdated'):
            self._analysisAlgorithm.scriptsUpdated.connect(self.updateScript)

    def updateScript(self, _=None):
        self.importsAppend("from chipwhisperer.analyzer.attacks.cpa import CPA")

        analysAlgoStr = self._analysisAlgorithm.__class__.__name__

        self.importsAppend("from %s import %s" % (sys.modules[self._analysisAlgorithm.__class__.__module__].__name__, analysAlgoStr))

        if hasattr(self._analysisAlgorithm, '_smartstatements'):
            self.mergeGroups('init', self._analysisAlgorithm, prefix='attack')

        self.addFunction("init", "setTraceSource", "UserScript.traces, blockSignal=True", loc=0)

    def processKnownKey(self, inpkey):
        if inpkey is None:
            return None

        if hasattr(self._analysisAlgorithm, 'processKnownKey'):
            return self._analysisAlgorithm.processKnownKey(inpkey)
        else:
            return inpkey

    def processTraces(self):
        progressBar = ProgressBar("Analysis in Progress", "Attaking with CPA:")
        with progressBar:
            self._analysisAlgorithm.setModel(self.findParam('Hardware Model').getValue())
            self._analysisAlgorithm.setTargetBytes(self.getTargetSubkeys())
            self._analysisAlgorithm.setReportingInterval(self.getReportingInterval())
            self._analysisAlgorithm.getStatistics().clear()
            self._analysisAlgorithm.setStatsReadyCallback(self.sigAnalysisUpdated.emit)

            self.sigAnalysisStarted.emit()
            for itNum in range(1, self.getIterations()+1):
                startingTrace = self.getTracesPerAttack() * (itNum - 1) + self.getTraceStart()
                endingTrace = startingTrace + self.getTracesPerAttack() - 1
                #TODO:  pointRange=self.TraceRangeList[1:17]
                self._analysisAlgorithm.addTraces(self.getTraceSource(), (startingTrace, endingTrace), progressBar, pointRange=self.getPointRange())
                if progressBar and progressBar.wasAborted():
                    return

        self.sigAnalysisDone.emit()

    def getStatistics(self):
        return self._analysisAlgorithm.getStatistics()

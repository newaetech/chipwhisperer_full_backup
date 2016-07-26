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
        self.algos = pluginmanager.getPluginsInDictFromPackage("chipwhisperer.analyzer.attacks.cpa_algorithms", True, False)
        self._analysisAlgorithm = self.algos["Progressive"]
        AttackBaseClass.__init__(self)
        self.getParams().addChildren([
            {'name':'Algorithm', 'key':'CPA_algo', 'type':'list',  'values':self.algos, 'get':self.getAnalysisAlgorithm, 'set':self.setAlgorithm}
        ])
        self.setAlgorithm(self.findParam('CPA_algo').getValue())
        self.updateScript()

    def getAnalysisAlgorithm(self):
        return self._analysisAlgorithm

    @setupSetParam('Algorithm')
    def setAlgorithm(self, analysisAlgorithm):
        self._analysisAlgorithm = analysisAlgorithm

        if hasattr(self._analysisAlgorithm, 'scriptsUpdated'):
            self._analysisAlgorithm.scriptsUpdated.connect(self.updateScript)
        self.updateScript()

    def setAnalysisAlgorithm(self, analysisAlgorithm, cryptoalg, hwmodel):
        self.attack = analysisAlgorithm()
        self.attackModel = cryptoalg()
        self.attackModel.setHwModel(hwmodel)

    def updateScript(self, _=None):
        AttackBaseClass.updateScript(self)
        self.importsAppend("from chipwhisperer.analyzer.attacks.cpa import CPA")
        self.importsAppend("import chipwhisperer")

        if hasattr(self._analysisAlgorithm, '_smartstatements'):
            self.mergeGroups('init', self._analysisAlgorithm, prefix='attack')

        analysAlgoStr = sys.modules[self._analysisAlgorithm.__class__.__module__].__name__ + '.' + self._analysisAlgorithm.__class__.__name__
        cryptoalg = sys.modules[self.findParam('Crypto Algorithm').getValue().__class__.__module__].__name__ + '.' + self.findParam('Crypto Algorithm').getValue().__class__.__name__
        hwmodel = self.findParam('Crypto Algorithm').getValue().getHwModelString()

        self.addFunction("init", "setAnalysisAlgorithm", "%s,%s,%s" % (analysAlgoStr, cryptoalg, hwmodel), loc=0)
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
            self.attack.setModel(self.attackModel)
            self.attack.setTargetSubkeys(self.getTargetSubkeys())
            self.attack.setReportingInterval(self.getReportingInterval())
            self.attack.getStatistics().clear()
            self.attack.setStatsReadyCallback(self.sigAnalysisUpdated.emit)

            self.sigAnalysisStarted.emit()
            for itNum in range(1, self.getIterations()+1):
                startingTrace = self.getTracesPerAttack() * (itNum - 1) + self.getTraceStart()
                endingTrace = startingTrace + self.getTracesPerAttack() - 1
                self.attack.addTraces(self.getTraceSource(), (startingTrace, endingTrace), progressBar, pointRange=self.getPointRange())
                if progressBar and progressBar.wasAborted():
                    return

        self.sigAnalysisDone.emit()

    def getStatistics(self):
        return self.attack.getStatistics()

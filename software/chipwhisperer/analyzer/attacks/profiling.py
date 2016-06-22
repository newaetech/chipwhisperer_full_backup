#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2014, NewAE Technology Inc
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
#
# ChipWhisperer is a trademark of NewAE Technology Inc.
#===========================================================

import sys
from chipwhisperer.common.ui.ProgressBar import ProgressBar
from chipwhisperer.common.utils import pluginmanager
import chipwhisperer.analyzer.attacks.models.AES128_8bit as models_AES128_8bit
import chipwhisperer.analyzer.attacks.models.AES256_8bit as models_AES256_8bit
from chipwhisperer.analyzer.attacks._base import AttackBaseClass
from _generic_parameters import AttackGenericParameters


class Profiling(AttackBaseClass, AttackGenericParameters):
    """Profiling Power Analysis Attack"""
    _name = "Profiling"

    def __init__(self):
        AttackBaseClass.__init__(self)
        self._project = None
        self.attack = None

        algos = pluginmanager.getPluginsInDictFromPackage("chipwhisperer.analyzer.attacks.profiling_algorithms", False, False, self)
        self.getParams().addChildren([
            {'name':'Algorithm', 'key':'Prof_algo', 'type':'list', 'values':algos, 'value':algos['Template Attack'], 'action':self.updateAlgorithm}, #TODO: Should be called from the AES module to figure out # of bytes
        ])
        AttackGenericParameters.__init__(self)

        # Do not use absolute
        self.useAbs = False
        self.updateAlgorithm()
        self.updateBytesVisible()
        self.setAbsoluteMode(False)

    def updateAlgorithm(self, _=None):
        self.setAnalysisAlgorithm(self.findParam('Prof_algo').getValue())
        self.updateBytesVisible()
        self.updateScript()

    def updateScript(self, _=None):
        self.importsAppend("from chipwhisperer.analyzer.attacks.profiling import Profiling")

        analysAlgoStr = self.attack.__class__.__name__
        self.importsAppend("from %s import %s" % (sys.modules[self.attack.__class__.__module__].__name__, analysAlgoStr))

        # self.addFunction("init", "setKeyround", "0")

        # Add attack 'other' functions such as template generators etc
        if hasattr(self.attack, '_smartstatements'):
            for k in self.attack._smartstatements:
                self.mergeGroups(k, self.attack, prefix='attack')

            for k in self.attack.getImportStatements():
                self.importsAppend(k)

        self.addFunction("init", "setAnalysisAlgorithm", "%s" % (analysAlgoStr), loc=0)
        self.addFunction("init", "setTraceSource", "UserScript.traces", loc=0)
        self.addFunction("init", "setProject", "UserScript.api.project()", loc=0)

    def setProject(self, project):
        self._project = project

    def project(self):
        return self._project

    def setAnalysisAlgorithm(self, analysisAlgorithm):
        if self.attack is not None:
            self.attack.getParams().remove()

        self.attack = analysisAlgorithm(self)
        self.attack.setTraceSource(self.getTraceSource())
        self.attack.setProject(self.project())
        self.attack.runScriptFunction.connect(self.runScriptFunction.emit)
        self.traceLimitsChanged.connect(self.attack.traceLimitsChanged)

        try:
            self.attackParams = self.attack.paramList()[0]
        except:
            self.attackParams = None

        if hasattr(self.attack, 'scriptsUpdated'):
            self.attack.scriptsUpdated.connect(self.updateScript)

        self.getParams().append(self.attack.getParams())

#    def setAlgo(self, algo):
#        self.attack = algo(self)
#        if self.setTraceSource() is not None:
#            self.attack.setTraceSource(self.setTraceSource())
#
#        if self.project() is not None:
#            self.attack.setProject(self.project())
#
#        try:
#            self.attackParams = self.attack.paramList()[0]
#        except:
#            self.attackParams = None
#
#        self.paramListUpdated.emit(self.paramList())

    def setKeyround(self, rnd):
        self._keyround = rnd

    def keyround(self):
        return self._keyround

    def processTraces(self):
        progressBar = ProgressBar("Analysis in Progress", "Attaking with Profiling:")
        with progressBar:
            self.attack.setReportingInterval(self.getReportingInterval())

            #TODO: support start/end point different per byte
            (startingPoint, endingPoint) = self.getPointRange(None)

            self.attack.getStatistics().clear()

            self.sigAnalysisStarted.emit()
            for itNum in range(1, self.getIterations() + 1):
                startingTrace = self.getTracesPerAttack() * (itNum - 1) + self.getTraceStart()
                endingTrace = self.getTracesPerAttack() * itNum + self.getTraceStart()

                data = []
                textins = []
                textouts = []

                for i in range(startingTrace, endingTrace):
                    d = self.getTraceSource().getTrace(i)

                    if d is None:
                        continue

                    d = d[startingPoint:endingPoint]

                    data.append(d)
                    textins.append(self.getTraceSource().getTextin(i))
                    textouts.append(self.getTraceSource().getTextout(i))

                #self.attack.clearStats()
                self.attack.setByteList(self.targetBytes())
                self.attack.setStatsReadyCallback(self.sigAnalysisUpdated.emit)

                #TODO:  pointRange=self.TraceRangeList[1:17]
                self.attack.addTraces(data, textins, textouts, knownkeys=None, progressBar=progressBar)

        self.sigAnalysisDone.emit()

    def getStatistics(self):
        return self.attack.getStatistics()

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

from chipwhisperer.common.ui.ProgressBar import ProgressBar
import chipwhisperer.analyzer.attacks.models.AES128_8bit as models_AES128_8bit
import chipwhisperer.analyzer.attacks.models.AES256_8bit as models_AES256_8bit
from chipwhisperer.analyzer.attacks._base import AttackBaseClass
from chipwhisperer.analyzer.attacks._profiling_template import ProfilingTemplate
from _generic_parameters import AttackGenericParameters


class Profiling(AttackBaseClass, AttackGenericParameters):
    """Profiling Power Analysis Attack"""
    name = "Profiling"

    def __init__(self):
        AttackBaseClass.__init__(self)
        AttackGenericParameters.__init__(self)

        # Do not use absolute
        self.useAbs = False
        self.updateAlgorithm(self.findParam('Prof_algo').value())
        self.updateBytesVisible()
        self.setAbsoluteMode(False)

    def setupParameters(self):
        profalgos = {'Basic':ProfilingTemplate}

        return [{'name':'Algorithm', 'key':'Prof_algo', 'type':'list', 'values':profalgos, 'value':ProfilingTemplate, 'set':self.updateAlgorithm},

                       #TODO: Should be called from the AES module to figure out # of bytes
                       {'name':'Attacked Bytes', 'type':'group', 'children':self.getByteList()},
                      ]

    def updateAlgorithm(self, algo):
        self.setAnalysisAlgorithm(algo)
        self.updateBytesVisible()
        self.updateScript()

    def updateScript(self, ignored=None):
        self.importsAppend("from chipwhisperer.analyzer.attacks.Profiling import Profiling")

        analysAlgoStr = self.findParam('Prof_algo').value().__name__
        self.importsAppend("from chipwhisperer.analyzer.attacks.%s import %s" % (analysAlgoStr, analysAlgoStr))

        self.addFunction("init", "setAnalysisAlgorithm", "%s" % (analysAlgoStr), loc=0)
        # self.addFunction("init", "setKeyround", "0")

        # Add attack 'other' functions such as template generators etc
        if hasattr(self.attack, '_smartstatements'):
            for k in self.attack._smartstatements:
                self.mergeGroups(k, self.attack, prefix='attack')

            for k in self.attack.getImportStatements():
                self.importsAppend(k)

        self.addFunction("init", "setTraceSource", "UserScript.traces")
        self.addFunction("init", "setProject", "UserScript.project()")

    def setAnalysisAlgorithm(self, analysisAlgorithm):
        self.attack = analysisAlgorithm(self)
        self.attack.setTraceSource(self.traceSource())
        self.attack.runScriptFunction.connect(self.runScriptFunction.emit)
        self.traceLimitsChanged.connect(self.attack.traceLimitsChanged)

        try:
            self.attackParams = self.attack.paramList()[0]
        except:
            self.attackParams = None

        self.paramListUpdated.emit(self.paramList())

        if hasattr(self.attack, 'scriptsUpdated'):
            self.attack.scriptsUpdated.connect(self.updateScript)

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

    def setTargetBytes(self, blist):
        self._targetbytes = blist

    def targetBytes(self):
        return self._targetbytes

    def processKnownKey(self, inpkey):
        return inpkey

        # if inpkey is None:
        #    return None

        # if self.findParam('hw_round').value() == 'last':
        #    return models_AES_RoundKeys.AES_RoundKeys().getFinalKey(inpkey)
        # else:
        #    return inpkey

    def processTraces(self):
        progressBar = ProgressBar("Analysis in Progress", "Attaking with CPA:")
        with progressBar:
            self.sigAnalysisStarted.emit()
            self.attack.setReportingInterval(self.getReportingInterval())

            #TODO: support start/end point different per byte
            (startingPoint, endingPoint) = self.getPointRange(None)

            self.attack.getStatistics().clear()

            for itNum in range(1, self.getIterations() + 1):
                startingTrace = self.getTraceNum() * (itNum - 1) + self.getTraceStart()
                endingTrace = self.getTraceNum() * itNum + self.getTraceStart()

                data = []
                textins = []
                textouts = []

                for i in range(startingTrace, endingTrace):
                    d = self.traceSource().getTrace(i)

                    if d is None:
                        continue

                    d = d[startingPoint:endingPoint]

                    data.append(d)
                    textins.append(self.traceSource().getTextin(i))
                    textouts.append(self.traceSource().getTextout(i))

                #self.attack.clearStats()
                self.attack.setByteList(self.bytesEnabled())
                self.attack.setStatsReadyCallback(self.statsReady)

                #TODO:  pointRange=self.TraceRangeList[1:17]
                self.attack.addTraces(data, textins, textouts, knownkeys=None, progressBar=progressBar)

        self.sigAnalysisDone.emit()

    def statsReady(self):
        self.sigAnalysisUpdated.emit()
        # QApplication.processEvents()

    def passTrace(self, powertrace, plaintext=None, ciphertext=None, knownkey=None):
        pass

    def getStatistics(self):
        return self.attack.getStatistics()

    def paramList(self):
        l = [self.params, self.pointsParams, self.traceParams]
        if self.attackParams is not None:
            l.append(self.attackParams)
        return l

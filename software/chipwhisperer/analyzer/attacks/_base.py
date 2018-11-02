#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2013-2014, NewAE Technology Inc
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
import logging

from chipwhisperer.common.ui.ProgressBar import ProgressBar
from chipwhisperer.common.utils.pluginmanager import Plugin
from chipwhisperer.common.utils.tracesource import PassiveTraceObserver, TraceSource
from chipwhisperer.common.utils.analysissource import AnalysisSource, AnalysisObserver
from chipwhisperer.common.api.autoscript import AutoScript
from chipwhisperer.common.utils.parameter import Parameterized, setupSetParam
from chipwhisperer.common.utils import pluginmanager
from .models.AES128_8bit import AES128_8bit, SBox_output

def enforceLimits(value, limits):
    if value < limits[0]:
        value = limits[0]
    elif value > limits[1]:
        value = limits[1]
    return value


class AttackBaseClass(PassiveTraceObserver, AnalysisSource, Parameterized, AutoScript, Plugin):
    """Generic Attack Interface"""
    _name= 'Attack Settings'
    _algos = {}

    def __init__(self):
        AutoScript.__init__(self)
        AnalysisSource.__init__(self)
        PassiveTraceObserver.__init__(self)
        self._itNum = 0
        self.getParams().getChild("Input").hide()
        self._traceStart = 0
        self._iterations = 1
        self._tracePerAttack = 1
        self._reportingInterval = 10
        self._pointRange = (0,0)
        self._targetSubkeys = []
        self._project = None
        self.useAbs = True
        self.attack = None

        self.getParams().addChildren([
            {'name':'Attack Algorithm', 'type':'list',  'values':self._algos, 'get':self.getAlgorithm, 'set':self.setAlgorithm, 'action':self.updateScript, 'childmode': 'parent'}
        ])
        models = pluginmanager.getPluginsInDictFromPackage("chipwhisperer.analyzer.attacks.models", True, False)
        self.getParams().addChildren([
            {'name':'Crypto Algorithm', 'type':'list', 'values':models, 'value':models['AES 128'], 'action':self.refreshByteList, 'childmode':'child'},
            {'name':'Points Range', 'key':'prange', 'type':'range', 'get':self.getPointRange, 'set':self.setPointRange, 'action':self.updateScript},
        ])
        for m in list(models.values()):
            m.sigParametersChanged.connect(self.updateScript)

        self.getParams().addChildren([
            {'name':'Starting Trace', 'key':'strace', 'type':'int', 'get':self.getTraceStart, 'set':self.setTraceStart, 'action':self.updateScript},
            {'name':'Traces per Attack', 'key':'atraces', 'type':'int', 'limits':(1, 1E6), 'get':self.getTracesPerAttack, 'set':self.setTracesPerAttack, 'action':self.updateScript},
            {'name':'Iterations', 'key':'runs', 'type':'int', 'limits':(1, 1E6), 'get':self.getIterations, 'set':self.setIterations, 'action':self.updateScript},
            {'name':'Reporting Interval', 'key':'reportinterval', 'type':'int', 'get':self.getReportingInterval, 'set':self.setReportingInterval, 'action':self.updateScript},
        ])
        self.getParams().init()

        self.setAlgorithm(self._analysisAlgorithm)
        self.refreshByteList()
        self.updateScript()
        if __debug__: logging.debug('Created: ' + str(self))

    def tracesUpdated(self):
        if self._analysisAlgorithm is not None:
            self._analysisAlgorithm.tracesUpdated(self._traceSource)
        self.updateTraceLimits()
        self.updateScript()

    def getAlgorithm(self):
        return self._analysisAlgorithm

    @setupSetParam('Attack Algorithm')
    def setAlgorithm(self, analysisAlgorithm):
        self._analysisAlgorithm = analysisAlgorithm
        self._analysisAlgorithm.tracesUpdated(self._traceSource)

        if hasattr(self._analysisAlgorithm, 'scriptsUpdated'):
            self._analysisAlgorithm.scriptsUpdated.connect(self.updateScript)
        if hasattr(self._analysisAlgorithm, 'runScriptFunction'):
            self._analysisAlgorithm.runScriptFunction.connect(self.runScriptFunction.emit)

    def setAnalysisAlgorithm(self, analysisAlgorithm, leakage_object=None):
        """Called from the script to setup the attack"""
        self.attack = analysisAlgorithm()
        self.attack.setProject(self._project)
        self.attackModel = leakage_object

    def processKnownKey(self, inpkey):
        """
        Passes known first-round key (if available, may pass None).
        Returns key under attack which should be highlighted in graph
        """
        return inpkey

    def processTracesNoGUI(self, callback=None):
        self.attack.setModel(self.attackModel)
        self.attack.getStatistics().clear()
        self.attack.setReportingInterval(self.getReportingInterval())
        self.attack.setTargetSubkeys(self.getTargetSubkeys())
        self.attack.setStatsReadyCallback(callback)

        for itNum in range(self.getIterations()):
            startingTrace = self.getTracesPerAttack() * itNum + self.getTraceStart()
            endingTrace = startingTrace + self.getTracesPerAttack() - 1

            # TODO:support start/end point different per byte
            self.attack.addTraces(self.getTraceSource(), (startingTrace, endingTrace), None, pointRange=self.getPointRange(None))
        return self.attack.getStatistics()

    def processTraces(self):
        progressBar = ProgressBar("Analysis in Progress", "Attacking with " + self.getName())
        with progressBar:
            self.attack.setModel(self.attackModel)
            self.attack.getStatistics().clear()
            self.attack.setReportingInterval(self.getReportingInterval())
            self.attack.setTargetSubkeys(self.getTargetSubkeys())
            self.attack.setStatsReadyCallback(self.sigAnalysisUpdated.emit)
            self.sigAnalysisStarted.emit()

            for itNum in range(self.getIterations()):
                startingTrace = self.getTracesPerAttack() * itNum + self.getTraceStart()
                endingTrace = startingTrace + self.getTracesPerAttack() - 1

                # TODO:support start/end point different per byte
                self.attack.addTraces(self.getTraceSource(), (startingTrace, endingTrace), progressBar, pointRange=self.getPointRange(None))

                if progressBar and progressBar.wasAborted():
                    return

        self.sigAnalysisDone.emit()

    def setProject(self, project):
        self._project = project

    def project(self):
        return self._project

    def getTraceStart(self):
        return self._traceStart

    @setupSetParam("Starting Trace")
    def setTraceStart(self, tnum):
        self._traceStart = tnum

    def getIterations(self):
        return self._iterations

    @setupSetParam("Iterations")
    def setIterations(self, its):
        self._iterations = its

    def getTracesPerAttack(self):
        return self._tracePerAttack

    @setupSetParam("Traces per Attack")
    def setTracesPerAttack(self, trace):
        if trace < 0:
            #Get maximum traces from source
            ts = self.getTraceSource()
            if ts is None:
                raise ValueError("traceSource not yet set in attack - set TraceSource first to use automatic getTracesPerAttack")
            trace = self.getTraceSource().numTraces()
        self._tracePerAttack = trace

    def getReportingInterval(self):
        return self._reportingInterval

    @setupSetParam("Reporting Interval")
    def setReportingInterval(self, ri):
        self._reportingInterval = ri

    def getPointRange(self, bnum=None):
        return self._pointRange

    @setupSetParam("Points Range")
    def setPointRange(self, rng):
        if rng[1] < 0:
            ts = self.getTraceSource()
            if ts is None:
                raise ValueError("traceSource not yet set in attack - set TraceSource first to use automatic setPointRange")
            rng = (rng[0], ts.numPoints())
        self._pointRange = rng

    def knownKey(self):
        """Get the known key via attack"""
        key = self.processKnownKey(self.getTraceSource().getKnownKey(self.getTraceStart()))
        if key is None:
            key = [None] * len(self.getStatistics().diffs)
        return key

    def setTargetSubkeys(self, blist):
        self._targetSubkeys = blist

    def getTargetSubkeys(self):
        return self._targetSubkeys

    def __del__(self):
        if __debug__: logging.debug('Deleted: ' + str(self))

    def getAbsoluteMode(self):
        return self.useAbs

    def refreshByteList(self, _=None):
        try:
            self.getParams().addChildren([
                {'name':'Attacked Subkeys', 'type':'group', 'children': [
                    dict(name='Subkey %d' % bnum, type='bool', key='bnumenabled%d' % bnum, value=True,
                         action=self.updateScript) for bnum in range(0, self.findParam('Crypto Algorithm').getValue().getNumSubKeys())
                ]}])
            self.updateScript()
        except KeyError:
            pass

    def getEnabledSubkeys(self):
        blist = []
        try:
            for bnum in range(self.findParam('Crypto Algorithm').getValue().getNumSubKeys()):
                if self.findParam(['Attacked Subkeys', ('Subkey %d' % bnum)]).getValue():
                    blist.append(bnum)
        except KeyError:
            pass
        return blist

    def getStatistics(self):
        return self.attack.getStatistics()

    def updateScript(self, _=None):
        self.importsAppend("import chipwhisperer")
        if self._traceSource is None:
            return

        # Add attack 'other' functions such as template generators etc
        if hasattr(self._analysisAlgorithm, '_smartstatements'):
            for k in self._analysisAlgorithm._smartstatements:
                self.mergeGroups(k, self._analysisAlgorithm, prefix='attack')

            for k in self._analysisAlgorithm.getImportStatements():
                self.importsAppend(k)

        runs = self.findParam('runs')
        atraces = self.findParam('atraces')
        strace = self.findParam('strace')
        ri = self.findParam('reportinterval')

        #print "runs = %d\natraces= %d\nstrace = %d\n"%(runs.value(), atraces.value(), strace.value())

        if (runs.getValue() * atraces.getValue() + strace.getValue()) > self._traceSource.numTraces() or atraces.getValue()<=0:
            solv = (self._traceSource.numTraces() - strace.getValue()) / runs.getValue()
            solv = int(solv)
            atraces.setValue(1, blockAction = True)
            atraces.setLimits((1, solv))
            atraces.setValue(solv, blockAction = True)
        else:
            atraces.setLimits((1, self._traceSource.numTraces()))

        pointrng = self.findParam('prange').getValue()

        self.addFunction("init", "setTraceSource", "UserScript.traces, blockSignal=True", loc=0)
        self.addFunction("init", "setProject", "UserScript.api.project()", loc=0)
        self.addFunction("init", "setTargetSubkeys", "%s" % str(self.getEnabledSubkeys()))
        self.addFunction("init", "setTraceStart", "%d" % strace.getValue())
        self.addFunction("init", "setTracesPerAttack", "%d" % atraces.getValue())
        self.addFunction("init", "setIterations", "%d" % runs.getValue())
        self.addFunction("init", "setReportingInterval", "%d" % ri.getValue())
        self.addFunction("init", "setPointRange", "(%d,%d)" % (pointrng[0], pointrng[1]))

    def updateTraceLimits(self):
        if self._traceSource is None:
            return

        self.findParam('prange').setLimits((0, self._traceSource.numPoints()-1))
        self.findParam('prange').setValue((0, self._traceSource.numPoints()-1))

        self.addFunction("init", "setPointRange", "(%d,%d)" % (0, self._traceSource.numPoints()))

        strace = self.findParam('strace')
        self.findParam('runs').setValue(1)
        atrace = self.findParam('atraces')

        strace.setLimits((0, self._traceSource.numTraces()-1))
        atrace.setValue(1, blockAction=True)
        atrace.setLimits((1, self._traceSource.numTraces()))
        atrace.setValue(self._traceSource.numTraces(), blockAction=True)


class AttackObserver(AnalysisObserver):
    """"It is an AnalysisObserver with methods to get information from attacks"""
    highlightedKeyColor = 255, 0, 0
    traceColor = 0, 255, 0

    def setAnalysisSource(self, analysisSource):
        if issubclass(analysisSource.__class__, AttackBaseClass):
            AnalysisObserver.setAnalysisSource(self, analysisSource)
        else:
            AnalysisObserver.setAnalysisSource(self, None)

    def _highlightedKeys(self):
        return self._analysisSource.knownKey()

    def _numPerms(self, key):
        try:
            return len(self._analysisSource.getStatistics().diffs[key])
        except Exception:
            return 0

    def _maxNumPerms(self):
        try:
            return self._analysisSource.getStatistics().numPerms
        except Exception:
            return 0

    def _numKeys(self):
        try:
            return len(self._analysisSource.getStatistics().diffs)
        except Exception:
            return 0

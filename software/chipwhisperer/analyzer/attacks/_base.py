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

import sys

from chipwhisperer.common.utils.pluginmanager import Plugin
from chipwhisperer.common.utils.tracesource import PassiveTraceObserver, ActiveTraceObserver, TraceSource
from chipwhisperer.common.utils.analysissource import AnalysisSource, AnalysisObserver
from chipwhisperer.common.api.autoscript import AutoScript
from chipwhisperer.common.utils import util
from chipwhisperer.common.utils.parameter import Parameterized, Parameter, setupSetParam
from chipwhisperer.common.utils import pluginmanager

def enforceLimits(value, limits):
    if value < limits[0]:
        value = limits[0]
    elif value > limits[1]:
        value = limits[1]
    return value


class AttackBaseClass(PassiveTraceObserver, AnalysisSource, Parameterized, AutoScript, Plugin):
    """Generic Attack Interface"""
    _name= 'Attack Settings'

    def __init__(self):
        AutoScript.__init__(self)
        AnalysisSource.__init__(self)
        PassiveTraceObserver.__init__(self)
        # self.getParams().getChild("Input").hide()
        TraceSource.sigRegisteredObjectsChanged.connect(self.traceSourcesChanged)
        self.traceLimitsChanged = util.Signal()
        self._traceStart = 0
        self._iterations = 1
        self._tracePerAttack = 1
        self._reportingInterval = 10
        self._pointRange = (0,0)
        self._targetSubkeys = []
        self.useAbs = True

        models = pluginmanager.getPluginsInDictFromPackage("chipwhisperer.analyzer.attacks.models", True, False)
        self.getParams().addChildren([
            {'name':'Crypto Algorithm', 'type':'list', 'values':models, 'value':models['AES Model'], 'action':self.refreshByteList, 'childmode':'child'},
            {'name':'Take Absolute', 'type':'bool', 'get':self.getAbsoluteMode, 'set':self.setAbsoluteMode},
            {'name':'Points Range', 'key':'prange', 'type':'range', 'get':self.getPointRange, 'set':self.setPointRange, 'action':self.updateScript},
        ])
        self.getParams().init()
        self.getParams().addChildren([
            {'name':'Starting Trace', 'key':'strace', 'type':'int', 'get':self.getTraceStart, 'set':self.setTraceStart, 'action':self.updateScript},
            {'name':'Traces per Attack', 'key':'atraces', 'type':'int', 'limits':(1, 1E6), 'get':self.getTracesPerAttack, 'set':self.setTracesPerAttack, 'action':self.updateScript},
            {'name':'Iterations', 'key':'runs', 'type':'int', 'limits':(1, 1E6), 'get':self.getIterations, 'set':self.setIterations, 'action':self.updateScript},
            {'name':'Reporting Interval', 'key':'reportinterval', 'type':'int', 'get':self.getReportingInterval, 'set':self.setReportingInterval, 'action':self.updateScript},
        ])
        self.refreshByteList()
        if __debug__: logging.debug('Created: ' + str(self))

    @setupSetParam('Input')
    def setTraceSource(self, traceSource):
        if self._traceSource:
            self._traceSource.sigTracesChanged.disconnect(self.updateTraceLimits)
        if traceSource:
            traceSource.sigTracesChanged.connect(self.updateTraceLimits)
        self._traceSource = traceSource
        self.updateScript()
        self.updateTraceLimits()

    def processKnownKey(self, inpkey):
        """
        Passes known first-round key (if available, may pass None).
        Returns key under attack which should be highlighted in graph
        """
        return inpkey

    def processTraces(self):
        """Do the attack"""
        self.sigAnalysisStarted.emit()
        # ...
        self.sigAnalysisDone.emit()

    def getStatistics(self):
        return None

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

    @setupSetParam("Take Absolute")
    def setAbsoluteMode(self, mode):
        self.useAbs = mode

    def refreshByteList(self, _=None):
        self.getParams().addChildren([
            {'name':'Attacked Subkeys', 'type':'group', 'children': [
                dict(name='Subkey %d' % bnum, type='bool', key='bnumenabled%d' % bnum, value=True,
                     action=self.updateScript) for bnum in range(0, self.findParam('Crypto Algorithm').getValue().getNumSubKeys())
            ]}])
        self.updateScript()

    def getEnabledSubkeys(self):
        blist = []
        for bnum in range(self.findParam('Crypto Algorithm').getValue().getNumSubKeys()):
                if self.findParam(['Attacked Subkeys', ('Subkey %d' % bnum)]).getValue():
                    blist.append(bnum)
        return blist

    def updateScript(self, _=None):
        if self._traceSource is None:
            return
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

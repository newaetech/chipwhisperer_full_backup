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

from chipwhisperer.common.utils.tracesource import PassiveTraceObserver
from chipwhisperer.common.utils.parameter import setupSetParam
from chipwhisperer.common.utils.util import camel_case_deprecated

def enforceLimits(value, limits):
    if value < limits[0]:
        value = limits[0]
    elif value > limits[1]:
        value = limits[1]
    return value


class AttackBaseClass(PassiveTraceObserver):
    """Generic Attack Interface"""
    _name= 'Attack Settings'
    _algos = {} # type: ignore

    def __init__(self):
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
        self.attackModel = None

        self.getParams().addChildren([
            {'name':'Attack Algorithm', 'type':'list',  'values':self._algos, 'get':self.getAlgorithm, 'set':self.setAlgorithm, 'action':self.updateScript, 'childmode': 'parent'}
        ])
        models = None
        self.getParams().addChildren([
            {'name':'Crypto Algorithm', 'type':'list', 'values':models, 'action':self.refreshByteList, 'childmode':'child'},
            {'name':'Points Range', 'key':'prange', 'type':'range', 'get':self.get_point_range, 'set':self.set_point_range, 'action':self.updateScript},
        ])

        self.getParams().addChildren([
            {'name':'Starting Trace', 'key':'strace', 'type':'int', 'get':self.get_trace_start, 'set':self.set_trace_start, 'action':self.updateScript},
            {'name':'Traces per Attack', 'key':'atraces', 'type':'int', 'limits':(1, 1E6), 'get':self.get_traces_per_attack, 'set':self.set_traces_per_attack, 'action':self.updateScript},
            {'name':'Iterations', 'key':'runs', 'type':'int', 'limits':(1, 1E6), 'get':self.get_iterations, 'set':self.set_iterations, 'action':self.updateScript},
            {'name':'Reporting Interval', 'key':'reportinterval', 'type':'int', 'get':self.get_reporting_interval, 'set':self.set_reporting_interval, 'action':self.updateScript},
        ])
        # self.getParams().init()

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

    def set_leak_model(self, leakage_object):
        """Set the leak model to leakage_object"""
        self.attackModel = leakage_object

    setLeakModel = camel_case_deprecated(set_leak_model)

    def get_leak_model(self):
        """Get the leak model for the attack"""
        return self.attackModel

    leakModel = get_leak_model

    def set_analysis_algorithm(self, analysis_algorithm, leakage_object=None):
        """Sets the algorithm used for analyzing the trace data

        You probably want cpa_algorithms.Progressive

        Args:
            analysis_algorithm (AlgorithmsBase): Algorithm used for analyzing
                trace data. Only use cpa_algorithms.Progressive for now.
            leakage_object (ModelsBase, optional): Model used to get the
                leakage of the target (i.e. the sbox output). Needs to be set
                before the attack can happen
        """
        self.attack = analysis_algorithm()
        self.attack.setProject(self._project)
        self.attackModel = leakage_object

    setAnalysisAlgorithm = camel_case_deprecated(set_analysis_algorithm)
    def process_known_key(self, inpkey):

        """
        Passes known first-round key (if available, may pass None).
        Returns key under attack which should be highlighted in graph
        """
        return inpkey

    def process_traces(self, callback=None):
        """ Run the attack!

        Args:
            callback (function(), optional): Function called every reporting
                interval. Not called if None. Defaults to None.

        Returns:
            Statistics object for the attack
        """
        progressBar = None

        self.attack.setModel(self.attackModel)
        self.attack.get_statistics().clear()
        self.attack.setReportingInterval(self.get_reporting_interval())
        self.attack.setTargetSubkeys(self.get_target_subkeys())
        self.attack.setStatsReadyCallback(callback)

        for itNum in range(self.get_iterations()):
            startingTrace = self.get_traces_per_attack() * itNum + self.get_trace_start()
            endingTrace = startingTrace + self.get_traces_per_attack() - 1

            # TODO:support start/end point different per byte
            self.attack.addTraces(self.getTraceSource(), (startingTrace, endingTrace), progressBar, pointRange=self.get_point_range(None))

        return self.attack.get_statistics()

    processTraces = camel_case_deprecated(process_traces)

    # for backwards compatability
    def processTracesNoGUI(self, callback=None, show_progress_bar=False):
        return self.processTraces(callback, show_progress_bar)

    def setProject(self, project):
        self._project = project

    def project(self):
        return self._project

    def get_trace_start(self):
        return self._traceStart

    getTraceStart = camel_case_deprecated(get_trace_start)
    @setupSetParam("Starting Trace")
    def set_trace_start(self, tnum):
        self._traceStart = tnum

    setTraceStart = camel_case_deprecated(set_trace_start)
    def get_iterations(self):
        return self._iterations

    getIterations = camel_case_deprecated(get_iterations)
    @setupSetParam("Iterations")
    def set_iterations(self, its):
        self._iterations = its

    setIterations = camel_case_deprecated(set_iterations)
    def get_traces_per_attack(self):
        return self._tracePerAttack

    getTracesPerAttack = camel_case_deprecated(get_traces_per_attack)
    @setupSetParam("Traces per Attack")
    def set_traces_per_attack(self, trace):
        if trace < 0:
            #Get maximum traces from source
            ts = self.getTraceSource()
            if ts is None:
                raise ValueError("traceSource not yet set in attack - set TraceSource first to use automatic getTracesPerAttack")
            trace = self.getTraceSource().num_traces()
        self._tracePerAttack = trace

    setTracesPerAttack = camel_case_deprecated(set_traces_per_attack)
    def get_reporting_interval(self):
        return self._reportingInterval

    getReportingInterval = camel_case_deprecated(get_reporting_interval)
    @setupSetParam("Reporting Interval")
    def set_reporting_interval(self, ri):
        self._reportingInterval = ri

    setReportingInterval = camel_case_deprecated(set_reporting_interval)
    def get_point_range(self, bnum=None):
        return self._pointRange

    getPointRange = camel_case_deprecated(get_point_range)
    @setupSetParam("Points Range")
    def set_point_range(self, rng):
        if rng[1] < 0:
            ts = self.getTraceSource()
            if ts is None:
                raise ValueError("traceSource not yet set in attack - set TraceSource first to use automatic setPointRange")
            rng = (rng[0], ts.num_points())
        self._pointRange = rng

    setPointRange = camel_case_deprecated(set_point_range)
    def known_key(self):
        """Get the known key via attack"""
        key = self.process_known_key(self.getTraceSource().get_known_key(self.get_trace_start()))
        if key is None:
            key = [None] * len(self.get_statistics().diffs)
        return key

    knownKey = camel_case_deprecated(known_key)
    def set_target_subkeys(self, blist):
        self._targetSubkeys = blist

    setTargetSubkeys = camel_case_deprecated(set_target_subkeys)
    def get_target_subkeys(self):
        return self._targetSubkeys

    getTargetSubkeys = camel_case_deprecated(get_target_subkeys)

    def __del__(self):
        if __debug__: logging.debug('Deleted: ' + str(self))

    def getAbsoluteMode(self):
        return self.useAbs

    def refreshByteList(self, _=None):
        pass

    def getEnabledSubkeys(self):
        return None

    def get_statistics(self):
        return self.attack.getStatistics()

    getStatistics = camel_case_deprecated(get_statistics)
    def updateScript(self, _=None):
        if self._traceSource is None:
            return

        # Add attack 'other' functions such as template generators etc
        runs = self.findParam('runs')
        atraces = self.findParam('atraces')
        strace = self.findParam('strace')
        ri = self.findParam('reportinterval')

        #print "runs = %d\natraces= %d\nstrace = %d\n"%(runs.value(), atraces.value(), strace.value())

        if (runs.getValue() * atraces.getValue() + strace.getValue()) > self._traceSource.num_traces() or atraces.getValue()<=0:
            solv = (self._traceSource.num_traces() - strace.getValue()) / runs.getValue()
            solv = int(solv)
            atraces.setValue(1, blockAction = True)
            atraces.setLimits((1, solv))
            atraces.setValue(solv, blockAction = True)
        else:
            atraces.setLimits((1, self._traceSource.num_traces()))

        pointrng = self.findParam('prange').getValue()


    def updateTraceLimits(self):
        if self._traceSource is None:
            return

        self.findParam('prange').setLimits((0, self._traceSource.num_points()-1))
        self.findParam('prange').setValue((0, self._traceSource.num_points()-1))


        strace = self.findParam('strace')
        self.findParam('runs').setValue(1)
        atrace = self.findParam('atraces')

        strace.setLimits((0, self._traceSource.num_traces()-1))
        atrace.setValue(1, blockAction=True)
        atrace.setLimits((1, self._traceSource.num_traces()))
        atrace.setValue(self._traceSource.num_traces(), blockAction=True)

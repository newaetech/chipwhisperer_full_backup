#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2013-2016, NewAE Technology Inc
# All rights reserved.
#
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
from datetime import datetime

from .base import ResultsBase
from chipwhisperer.common.api.CWCoreAPI import CWCoreAPI
from chipwhisperer.common.utils.tracesource import TraceSource, PassiveTraceObserver
from chipwhisperer.common.utils.pluginmanager import Plugin
from chipwhisperer.common.utils.parameter import setupSetParam


class TraceRecorder(ResultsBase, PassiveTraceObserver, Plugin):
    _name = 'Trace Recorder'
    _description = 'Saves the traces for a given input.'

    def __init__(self, name=None):
        PassiveTraceObserver.__init__(self)

        self.getParams().addChildren([
            {'name':'Trace Format', 'key':'tracefmt', 'type':'list', 'values':CWCoreAPI.getInstance().valid_traces, 'value':None},
            {'name':'Trace Range', 'key':'tracerng', 'type':'range', 'limits':(0, 0), 'value':(0, 0)},
            {'name':'Point Range', 'key':'pointrng', 'type':'rangegraph', 'limits':(0, 0), 'value':(0, 0), 'graphwidget':ResultsBase.registeredObjects["Trace Output Plot"]},
            {'name':'Save', 'type':'action', 'action':self.processTraces},
        ])

        self.findParam('input').setValue("Trace Management")
        TraceSource.sigRegisteredObjectsChanged.connect(self.traceSourcesChanged)
        TraceSource.sigRegisteredObjectsChanged.connect(self.resetTraceLimits)

    @setupSetParam('Input')
    def setTraceSource(self, traceSource):
        self._traceSource = traceSource
        self.resetTraceLimits()

    def resetTraceLimits(self):
        if self._traceSource:
            lastTrace = self._traceSource.numTraces()-1
            lastPoint = self._traceSource.numPoints()-1
        else:
            lastTrace = -1
            lastPoint = -1

        traceRange = self.findParam('tracerng').getValue()
        pointRange = self.findParam('pointrng').getValue()
        self.findParam('tracerng').setLimits((0, lastTrace))
        self.findParam('tracerng').setValue((max(0, traceRange[0]), min(lastTrace, traceRange[1] if traceRange[1] >= 0 else lastTrace)))
        self.findParam('pointrng').setLimits((0, lastPoint))
        self.findParam('pointrng').setValue((max(0, pointRange[0]), min(lastPoint,  pointRange[1] if pointRange[1] >= 0 else pointRange)))

    def processTraces(self, _=None):
        tstart = self.findParam('tracerng').getValue()[0]
        tend = self.findParam('tracerng').getValue()[1]
        pstart = self.findParam('pointrng').getValue()[0]
        pend = self.findParam('pointrng').getValue()[1]

        trace = CWCoreAPI.getInstance().getNewTrace(self.findParam('tracefmt').getValue())
        trace.config.setAttr("notes", "Recorded from \"%s\" output: Traces (%s,%s). Points (%s,%s)" % (self.findParam('Input').getValueKey(), tstart, tend, pstart, pend))
        for tnum in range(tstart, tend+1):
            trace.addTrace(self.getTraceSource().getTrace(tnum)[pstart:pend+1], self.getTraceSource().getTextin(tnum), self.getTraceSource().getTextout(tnum), self.getTraceSource().getKnownKey(tnum))
        trace.closeAll()
        CWCoreAPI.getInstance().project().traceManager().appendSegment(trace, enabled=False)

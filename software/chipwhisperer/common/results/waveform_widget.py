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

from _base import ResultsWidgetBase
from chipwhisperer.common.ui.GraphWidget import GraphWidget
from chipwhisperer.common.utils import util
from chipwhisperer.common.utils.tracesource import TraceSource, ActiveTraceObserver


class WaveFormWidget(GraphWidget, ResultsWidgetBase, ActiveTraceObserver):
    _name = 'Trace Output Plot'
    _description = 'Plots the waveform for a given trace source'

    def __init__(self, parentParam=None, name=None):
        GraphWidget.__init__(self)
        ResultsWidgetBase.__init__(self, parentParam, name)
        ActiveTraceObserver.__init__(self)

        self.redrawAfterEach = False
        self.params.addChildren([
            {'name':'Redraw after Each', 'type':'bool', 'value':self.redrawAfterEach, 'set':self.setRedrawAfterEach},
            {'name':'Trace Range', 'key':'tracerng', 'type':'range', 'limits':(0, 0), 'default':(0, 0)},
            {'name':'Point Range', 'key':'pointrng', 'type':'rangegraph', 'limits':(0, 0), 'default':(0, 0), 'graphwidget':self},
            {'name':'Redraw', 'type':'action', 'action':self.plotInputTrace},
        ])

        self.findParam('input').setValue(TraceSource.registeredObjects["Trace Manager"])
        TraceSource.sigRegisteredObjectsChanged.connect(self.traceSourcesChanged)

        self.resetTraceLimits()
        self.setDefaultYRange(-0.5, 0.5)
        self.YDefault()

    def resetTraceLimits(self):
        if self._traceSource:
            lastTrace = self._traceSource.numTraces()-1
            lastPoint = self._traceSource.numPoints()-1
        else:
            lastTrace = -1
            lastPoint = -1
        self.findParam('tracerng').setLimits((0 if lastTrace>-1 else -1, lastTrace))
        self.findParam('tracerng').setValue((0, min(lastTrace, 0)))
        self.findParam('pointrng').setLimits((0 if lastPoint>-1 else -1, lastPoint))
        self.findParam('pointrng').setValue((0, lastPoint))

    def setRedrawAfterEach(self, enabled):
        self.redrawAfterEach = enabled

    def plotInputTrace(self):
        #print "Plotting %d-%d for points %d-%d"%(params[0].value(), params[1].value(), params[2].value(), params[3].value())
        initialPersist = self.persistant
        if not self.persistant:
            self.clearPushed()

        tstart = self.findParam('tracerng').value()[0]
        tend = self.findParam('tracerng').value()[1]
        pstart = self.findParam('pointrng').value()[0]
        pend = self.findParam('pointrng').value()[1]
        ttotal = 0

        if tend - tstart + 1 > 1:
            self.setPersistance(True)

        for tnum in range(tstart, tend+1):
            trace = self._traceSource.getTrace(tnum)
            if trace is not None:
                ttotal += 1
                self.passTrace(trace[pstart:pend+1], pstart, self._traceSource.offset())

                if self.redrawAfterEach:
                    util.updateUI()

        self.setPersistance(initialPersist)

    def processTraces(self):
        self.resetTraceLimits()
        self.plotInputTrace()
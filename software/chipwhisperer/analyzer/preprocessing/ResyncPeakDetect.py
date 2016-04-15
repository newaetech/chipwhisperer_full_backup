#!/usr/bin/pythonh
# -*- coding: utf-8 -*-
#
# Copyright (c) 2013-2014, NewAE Technology Inc
# All rights reserved.
#
# Author: Colin O'Flynn
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

import numpy as np
from _base import PreprocessingBase


class ResyncPeakDetect(PreprocessingBase):
    """
    Resyncronize based on peak value.
    """
    name = "Resync: Peak Detect"
    description = "Line up traces so peak (either max positive or max negative) within" \
    " some given range of points all aligns. For each trace the following must hold or the trace is rejected:\n" \
    "   (1-valid limit) < (peak value from candidate trace) / (peak value from reference) < (1+valid limit)\n" \
    "If 'valid limit' is 0 then this is ignored, and all traces are kept."

    def _setupParameters(self):
        self.rtrace = 0
        self.debugReturnCorr = False
        self.ccStart = 0
        self.ccEnd = 0
        self.limit = 0
        self.type = max
        return [ {'name':'Ref Trace #', 'key':'reftrace', 'type':'int', 'value':0, 'set':self.updateScript},
                 {'name':'Peak Type', 'key':'peaktype', 'type':'list', 'value':'Max', 'values':['Max', 'Min'], 'set':self.updateScript},
                 {'name':'Point Range', 'key':'ptrange', 'type':'rangegraph', 'graphwidget':self.graphWidget, 'set':self.updateScript, 'default':(0, 0)},
                 {'name':'Valid Limit', 'key':'vlimit', 'type':'float', 'value':0, 'step':0.1, 'limits':(-10, 10), 'set':self.updateScript},
                ]

    def updateScript(self, ignored=None):
        self.addFunction("init", "setEnabled", "%s" % self.findParam('enabled').value())

        pt = self.findParam('ptrange').value()
        if pt is None: pt = (0, 0)

        self.addFunction("init", "setReference", "rtraceno=%d, peaktype='%s', refrange=(%d, %d), validlimit=%f" % (
                            self.findParam('reftrace').value(),
                            self.findParam('peaktype').value(),
                            pt[0], pt[1],
                            self.findParam('vlimit').value()
                            ))

    def setReference(self, rtraceno=0, peaktype='max', refrange=(0, 0), validlimit=0):
        self.rtrace = rtraceno
        self.limit = validlimit
        self.type = peaktype
        self.ccStart = refrange[0]
        self.ccEnd = refrange[1]
        self.init()

    def getTrace(self, n):
        if self.enabled:
            #TODO: fftconvolve
            trace = self.traceSource.getTrace(n)
            if trace is None:
                return None
            if str.lower(self.type) == 'max':
                newmaxloc = np.argmax(trace[self.ccStart:self.ccEnd])
                maxval = max(trace[self.ccStart:self.ccEnd])
            else:
                newmaxloc = np.argmin(trace[self.ccStart:self.ccEnd])
                maxval = min(trace[self.ccStart:self.ccEnd])

            if self.limit:
                if (maxval > self.refmaxsize * (1.0 + self.limit)) | (maxval < self.refmaxsize * (1.0 - self.limit)):
                    return None

            diff = newmaxloc-self.refmaxloc
            if diff < 0:
                trace = np.append(np.zeros(-diff), trace[:diff])
            elif diff > 0:
                trace = np.append(trace[diff:], np.zeros(diff))
            return trace

        else:
            return self.traceSource.getTrace(n)

    def init(self):
        try:
            self.calcRefTrace(self.rtrace)
        except ValueError:
            self.findParam('enabled').setValue(False)

    def calcRefTrace(self, tnum):
        reftrace = self.traceSource.getTrace(tnum)[self.ccStart:self.ccEnd]
        if self.type == 'Max':
            self.refmaxloc = np.argmax(reftrace)
            self.refmaxsize = max(reftrace)
        else:
            self.refmaxloc = np.argmin(reftrace)
            self.refmaxsize = min(reftrace)

#!/usr/bin/python
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

from ._base import PreprocessingBase
import numpy as np
import scipy as sp
from chipwhisperer.common.results._base import ResultsBase


class ResyncCrossCorrelation(PreprocessingBase):
    """
    Cross-Correlation Resyncronization
    """
    _name = "Resync: Cross Correlation"
    _description = "Uses cross-correlation to detect shift between a 'reference trace' and every input trace. "\
                  "In practice the other resync methods seem to work better."

    def __init__(self, parentParam=None, traceSource=None):
        PreprocessingBase.__init__(self, parentParam, traceSource)
        self.rtrace = 0
        self.debugReturnCorr = False
        self.ccStart = 0
        self.ccEnd = 0

        self.params.addChildren([
            {'name':'Ref Trace', 'key':'reftrace', 'type':'int', 'value':0, 'set':self.updateScript},
            {'name':'Window', 'key':'rwindow', 'type':'rangegraph', 'graphwidget':ResultsBase.registeredObjects["Trace Output Plot"], 'set':self.updateScript, 'default':(0, 0)},
            # {'name':'Output Correlation (DEBUG)', 'type':'bool', 'value':False, 'set':self.setOutputCorr}
        ])
        self.updateScript()

    def updateScript(self, ignored=None):
        self.addFunction("init", "setEnabled", "%s" % self.findParam('enabled').value())
        rtrace = self.findParam('reftrace').value()
        rrange = self.findParam('rwindow').value()

        if rrange is None:
            rrange = (0, 0)

        self.addFunction("init", "setReference", "refno=%d, refWindow=(%d, %d)" % (rtrace, rrange[0], rrange[1]))

    def setReference(self, refno=0, refWindow=(0, 0)):
        self.ccStart = refWindow[0]
        self.ccEnd = refWindow[1]
        self.rtrace = refno
        self.init()

    # def setOutputCorr(self, enabled):
    #    self.debugReturnCorr = enabled
   
    def getTrace(self, n):
        if self.enabled:
            #TODO: fftconvolve
            trace = self._traceSource.getTrace(n)
            if trace is None:
                return None
            cross = sp.signal.fftconvolve(trace, self.reftrace, mode='valid')
            if self.debugReturnCorr:
                return cross
            newmaxloc = np.argmax(cross[self.ccStart:self.ccEnd])
            # maxval = max(cross[self.ccStart:self.ccEnd])
            # if (maxval > self.refmaxsize * 1.01) | (maxval < self.refmaxsize * 0.99):
            #    return None
            
            diff = newmaxloc-self.refmaxloc
            if diff < 0:
                trace = np.append(np.zeros(-diff), trace[:diff])
            elif diff > 0:
                trace = np.append(trace[diff:], np.zeros(diff))
            return trace
            
        else:
            return self._traceSource.getTrace(n)
   
    def init(self):
        try:
            self.calcRefTrace(self.rtrace)
        except ValueError:
            self.findParam('enabled').setValue(False)
        
    def calcRefTrace(self, tnum):
        # If not enabled stop
        if self.enabled == False:
            return

        self.reftrace = self._traceSource.getTrace(tnum)[self.ccStart:self.ccEnd]
        self.reftrace = self.reftrace[::-1]
        #TODO: fftconvolve
        cross = sp.signal.fftconvolve(self._traceSource.getTrace(tnum), self.reftrace, mode='valid')
        self.refmaxloc = np.argmax(cross[self.ccStart:self.ccEnd])
        self.refmaxsize = max(cross[self.ccStart:self.ccEnd])

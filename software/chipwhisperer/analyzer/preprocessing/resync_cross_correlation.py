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

import numpy as np
import scipy as sp # type: ignore

from ._base import PreprocessingBase
from chipwhisperer.common.utils.parameter import setupSetParam


class ResyncCrossCorrelation(PreprocessingBase):
    """
    Cross-Correlation Resyncronization
    """
    _name = "Resync: Cross Correlation"
    _description = "Use cross-correlation to detect shift between a 'reference trace' and every input trace. "\
                  "In practice the other resync methods seem to work better."

    def __init__(self, traceSource=None, name=None):
        PreprocessingBase.__init__(self, traceSource, name=name)
        self._rtrace = 0
        self._debugReturnCorr = False
        self._ccStart = 0
        self._ccEnd = 0

        self.params.addChildren([
            {'name':'Ref Trace', 'key':'reftrace', 'type':'int', 'get':self._getRefTrace, 'set':self._setRefTrace},
            {'name':'Window', 'key':'rwindow', 'type':'rangegraph',  'get':self._getWindow, 'set':self._setWindow},
            # {'name':'Output Correlation (DEBUG)', 'type':'bool', 'value':False, 'set':self.setOutputCorr}
        ])
        self._calculateRef()

    @setupSetParam("Ref Trace")
    def _setRefTrace(self, num):
        self._rtrace = num
        self._calculateRef()

    def _getRefTrace(self):
        return self._rtrace

    @property
    def ref_trace(self):
        """The trace being used as a reference.

        Setter raises TypeError unless value is an integer."""
        return self._getRefTrace()

    @ref_trace.setter
    def ref_trace(self, num):
        if not isinstance(num, int):
            raise TypeError("Expected int; got %s" % type(num), num)
        self._setRefTrace(num)

    @setupSetParam("Window")
    def _setWindow(self, window):
        self._ccStart, self._ccEnd = window
        self._calculateRef()

    def _getWindow(self):
        return (self._ccStart, self._ccEnd)

    @property
    def window(self):
        """The section of the reference trace used for comparison.

        This must be a tuple of (first point, last point).

        Setter raises TypeError if value is not a tuple or if points are not
        integers.
        """
        return self._getWindow()

    @window.setter
    def window(self, win):
        if not isinstance(win, tuple):
            raise TypeError("Expected tuple; got %s" % type(win), win)
        if not isinstance(win[0], int):
            raise TypeError("Expected int; got %s" % type(win[0]), win[0])
        if not isinstance(win[1], int):
            raise TypeError("Expected int; got %s" % type(win[1]), win[1])
        self._setWindow(win)
   
    def getTrace(self, n):
        if self.enabled:
            #TODO: fftconvolve
            trace = self._traceSource.getTrace(n)
            if trace is None:
                return None
            cross = sp.signal.fftconvolve(trace, self._reftrace, mode='valid')
            if self._debugReturnCorr:
                return cross
            newmaxloc = np.argmax(cross[self._ccStart:self._ccEnd])
            # maxval = max(cross[self.ccStart:self.ccEnd])
            # if (maxval > self.refmaxsize * 1.01) | (maxval < self.refmaxsize * 0.99):
            #    return None
            
            diff = newmaxloc-self._refmaxloc
            if diff < 0:
                trace = np.append(np.zeros(-diff), trace[:diff])
            elif diff > 0:
                trace = np.append(trace[diff:], np.zeros(diff))
            return trace
            
        else:
            return self._traceSource.getTrace(n)
   
    def _calculateRef(self):
        try:
            self.calcRefTrace(self._rtrace)
        except ValueError:
            self.findParam('enabled').setValue(False)
        
    def calcRefTrace(self, tnum):
        # If not enabled stop
        if self.enabled == False:
            return

        self._reftrace = self._traceSource.getTrace(tnum)[self._ccStart:self._ccEnd]
        self._reftrace = self._reftrace[::-1]
        #TODO: fftconvolve
        cross = sp.signal.fftconvolve(self._traceSource.getTrace(tnum), self._reftrace, mode='valid')
        self._refmaxloc = np.argmax(cross[self._ccStart:self._ccEnd])
        self._refmaxsize = max(cross[self._ccStart:self._ccEnd])

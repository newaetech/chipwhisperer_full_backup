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

from chipwhisperer.common.results.base import ResultsBase
from ._base import PreprocessingBase
from chipwhisperer.common.utils.parameter import setupSetParam


class ResyncSAD(PreprocessingBase):
    """Resynchronize traces by shifting them to match in a certain window.
    """
    _name = "Resync: Sum-of-Difference"
    _description = "Minimizes the 'Sum of Absolute Difference' (SAD), also known as 'Sum of Absolute Error'. Uses "\
                  "a portion of one of the traces as the 'reference'. This reference is then slid over the 'input "\
                  "window' for each trace, and the amount of shift resulting in the minimum SAD criteria is selected "\
                  "as the shift amount for that trace."

    def __init__(self, traceSource=None, connectTracePlot=True, name=None):
        PreprocessingBase.__init__(self, traceSource, name=name)
        self._rtrace = 0
        self._debugReturnSad = False
        self._ccStart = 0
        self._ccEnd = 1
        self._wdStart = 0
        self._wdEnd = 1

        if connectTracePlot:
            traceplot = ResultsBase.registeredObjects["Trace Output Plot"]
        else:
            traceplot = None

        self.params.addChildren([
            {'name':'Ref Trace', 'key':'reftrace', 'type':'int', 'get':self._getRefTrace, 'set':self._setRefTrace},
            {'name':'Reference Points', 'key':'refpts', 'type':'rangegraph', 'graphwidget':traceplot, 'get':self._getRefPoints, 'set':self._setRefPoints},
            {'name':'Input Window', 'key':'windowpt', 'type':'rangegraph', 'graphwidget':traceplot, 'get':self._getWindow, 'set':self._setWindow},
             #{'name':'Valid Limit', 'type':'float', 'value':0, 'step':0.1, 'limits':(0, 10), 'set':self.setValidLimit},
            # {'name':'Output SAD (DEBUG)', 'type':'bool', 'value':False, 'set':self.setOutputSad}
        ])
        self.updateLimits()
        self.sigTracesChanged.connect(self.updateLimits)

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
        if not isinstance(num, (int, long)):
            raise TypeError("Expected int; got %s" % type(num), num)
        self._setRefTrace(num)

    @setupSetParam("Reference Points")
    def _setRefPoints(self, window):
        self._ccStart, self._ccEnd = window
        self._calculateRef()

    def _getRefPoints(self):
        return (self._ccStart, self._ccEnd)

    @property
    def ref_points(self):
        """The section of the input trace swept over the reference section.

        This must be a tuple of (first point, last point).

        Setter raises TypeError if value is not a tuple or if points are not
        integers.
        """
        return self._getRefPoints()

    @ref_points.setter
    def ref_points(self, win):
        if not isinstance(win, tuple):
            raise TypeError("Expected tuple; got %s" % type(win), win)
        if not isinstance(win[0], (int, long)):
            raise TypeError("Expected int; got %s" % type(win[0]), win[0])
        if not isinstance(win[1], (int, long)):
            raise TypeError("Expected int; got %s" % type(win[1]), win[1])
        self._setRefPoints(win)

    @setupSetParam("Input Window")
    def _setWindow(self, window):
        self._wdStart, self._wdEnd = window
        self._calculateRef()

    def _getWindow(self):
        return (self._wdStart, self._wdEnd)

    @property
    def input_window(self):
        """The section of the input trace swept over the reference section.

        This must be a tuple of (first point, last point).

        Setter raises TypeError if value is not a tuple or if points are not
        integers.
        """
        return self._getWindow()

    @input_window.setter
    def input_window(self, win):
        if not isinstance(win, tuple):
            raise TypeError("Expected tuple; got %s" % type(win), win)
        if not isinstance(win[0], (int, long)):
            raise TypeError("Expected int; got %s" % type(win[0]), win[0])
        if not isinstance(win[1], (int, long)):
            raise TypeError("Expected int; got %s" % type(win[1]), win[1])
        self._setWindow(win)

    def updateLimits(self):
        if self._traceSource:
            self.findParam('refpts').setLimits((0, self._traceSource.numPoints()-1))
            self.findParam('windowpt').setLimits((0, self._traceSource.numPoints()-1))

    def setOutputSad(self, enabled):
        self._debugReturnSad = enabled
   
    def getTrace(self, n):
        if self.enabled:
            trace = self._traceSource.getTrace(n)
            if trace is None:
                return None
            sad = self._findSAD(trace)
            
            if self._debugReturnSad:
                return sad
            
            if len(sad) == 0:
                return None            
            
            newmaxloc = np.argmin(sad)
            maxval = min(sad)
            #if (maxval > self.refmaxsize * 1.01) | (maxval < self.refmaxsize * 0.99):
            #    return None
            
            if maxval > self.maxthreshold:
                return None
            
            diff = newmaxloc-self.refmaxloc
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
        #Probably shouldn't do this, but deals with user enabling preprocessing
        #before trace management setup
        except ValueError:
            pass
        
    def _findSAD(self, inputtrace):
        reflen = self._ccEnd - self._ccStart
        sadlen = self._wdEnd - self._wdStart
        sadarray = np.empty(sadlen-reflen)
        
        for ptstart in range(self._wdStart, self._wdEnd-reflen):
            #Find SAD        
            sadarray[ptstart - self._wdStart] = np.sum(np.abs(inputtrace[ptstart:(ptstart + reflen)] - self.reftrace))
            
        return sadarray
        
    def calcRefTrace(self, tnum):
        if self.enabled == False:
            return
        
        self.reftrace = self._traceSource.getTrace(tnum)[self._ccStart:self._ccEnd]
        sad = self._findSAD(self._traceSource.getTrace(tnum))
        self.refmaxloc = np.argmin(sad)
        self.refmaxsize = min(sad)
        self.maxthreshold = np.mean(sad)

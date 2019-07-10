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

from ._base import PreprocessingBase
from chipwhisperer.common.utils.parameter import setupSetParam


class ResyncPeakDetect(PreprocessingBase):
    """
    Resyncronize based on peak value.
    """
    _name = "Resync: Peak Detect"
    _description = "Line up traces so peak (either max positive or max negative) within" \
    " some given range of points all aligns. For each trace the following must hold or the trace is rejected:\n" \
    "   (1-valid limit) < (peak value from candidate trace) / (peak value from reference) < (1+valid limit)\n" \
    "If 'valid limit' is 0 then this is ignored, and all traces are kept."

    def __init__(self, traceSource=None, name=None):
        PreprocessingBase.__init__(self, traceSource, name=name)
        self._rtrace = 0
        self._ccStart = 0
        self._ccEnd = 0
        self._limit = 0
        self._type = "max"

        self.params.addChildren([
            {'name':'Ref Trace', 'key':'reftrace', 'type':'int', 'get':self._getRefTrace, 'set':self._setRefTrace},
            {'name':'Peak Type', 'key':'peaktype', 'type':'list', 'values':['max', 'min'], 'get':self._getType, 'set':self._setType},
            {'name':'Point Range', 'key':'ptrange', 'type':'rangegraph',  'get':self._getWindow, 'set':self._setWindow},
            {'name':'Valid Limit', 'key':'vlimit', 'type':'float', 'step':0.1, 'limits':(0, 10), 'set':self._setLimit, 'get':self._getLimit},
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

    @setupSetParam("Peak Type")
    def _setType(self, type):
        self._type = type
        self._calculateRef()

    def _getType(self):
        return self._type

    @property
    def type(self):
        """Which type of peak to look for.

        Valid values are "max" and "min".

        Setter raises ValueError if value isn't recognized.
        """
        return self._getType()

    @type.setter
    def type(self, type):
        valid = ["min", "max"]
        if type not in valid:
            raise ValueError("Unrecognized peak type; expected 'min' or 'max'", type)

    @setupSetParam("Point Range")
    def _setWindow(self, window):
        self._ccStart, self._ccEnd = window
        self._calculateRef()

    def _getWindow(self):
        return (self._ccStart, self._ccEnd)

    @property
    def range(self):
        """The section of the reference trace used for comparison.

        This must be a tuple of (first point, last point).

        Setter raises TypeError if value is not a tuple or if points are not
        integers.
        """
        return self._getWindow()

    @range.setter
    def range(self, win):
        if not isinstance(win, tuple):
            raise TypeError("Expected tuple; got %s" % type(win), win)
        if not isinstance(win[0], int):
            raise TypeError("Expected int; got %s" % type(win[0]), win[0])
        if not isinstance(win[1], int):
            raise TypeError("Expected int; got %s" % type(win[1]), win[1])
        self._setWindow(win)

    @setupSetParam("Valid Limit")
    def _setLimit(self, lim):
        self._limit = lim
        self._calculateRef()

    def _getLimit(self):
        return self._limit

    @property
    def valid_limit(self):
        """The tolerance level on the peak-matching search.

        A lower valid limit means that the peaks have to be closer in value for
        a trace to be accepted.

        A valid limit of 0 indicates that all traces should be kept.

        Setter raises TypeError if value isn't a float.
        """
        return self._getLimit()

    @valid_limit.setter
    def valid_limit(self, lim):
        if not isinstance(lim, float):
            raise TypeError("Expected float; got %s" % type(lim), lim)
        self._setLimit(lim)

    def updateLimits(self):
        if self._traceSource:
            self.findParam('ptrange').setLimits((0, self._traceSource.numPoints()))

    def updateScript(self, _=None):

        pt = self.findParam('ptrange').getValue()

        self.updateLimits()

    def setReference(self, rtraceno=0, peaktype='max', refrange=(0, 0), validlimit=0):
        self._rtrace = rtraceno
        self._limit = validlimit
        self._type = peaktype
        self._ccStart = refrange[0]
        self._ccEnd = refrange[1]
        self.init()

    def getTrace(self, n):
        if self.enabled:
            trace = self._traceSource.getTrace(n)
            if trace is None:
                return None
            if str.lower(self._type) == 'max':
                newmaxloc = np.argmax(trace[self._ccStart:self._ccEnd])
                maxval = max(trace[self._ccStart:self._ccEnd])
            else:
                newmaxloc = np.argmin(trace[self._ccStart:self._ccEnd])
                maxval = min(trace[self._ccStart:self._ccEnd])

            if self._limit:
                if (maxval > self._refmaxsize * (1.0 + self._limit)) | (maxval < self._refmaxsize * (1.0 - self._limit)):
                    return None

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

        reftrace = self._traceSource.getTrace(tnum)[self._ccStart:self._ccEnd]
        if self._type == 'max':
            self._refmaxloc = np.argmax(reftrace)
            self._refmaxsize = max(reftrace)
        else:
            self._refmaxloc = np.argmin(reftrace)
            self._refmaxsize = min(reftrace)

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

from ._base import PreprocessingBase
from collections import OrderedDict


class ResyncSAD(PreprocessingBase):
    """Resynchronize traces by shifting them to match in a certain window.

    This method of pre-processing uses the Sum of Absolute Difference
    (SAD) algorithm, also known as the Sum of Absolute Error. Uses a portion
    of one of the traces as a reference. This reference is then slid over the
    input "window" of each trace, and the amount of the shift resulting in the
    minimum SAD criteria is used as the shift amount for each trace.

    Args:
        trace_source (Project): A project containing the traces to preprocess.
    """
    _name = "Resync: Sum-of-Difference"
    _description = "Minimizes the 'Sum of Absolute Difference' (SAD), also known as 'Sum of Absolute Error'. Uses "\
                  "a portion of one of the traces as the 'reference'. This reference is then slid over the 'input "\
                  "window' for each trace, and the amount of shift resulting in the minimum SAD criteria is selected "\
                  "as the shift amount for that trace."

    def __init__(self, trace_source=None):
        PreprocessingBase.__init__(self, trace_source, name=None)
        self._rtrace = None
        self._debugReturnSad = False
        self._wdStart = 0
        self._wdEnd = 1
        self._maxshift = 1
        self._init_not_done = True

    def _setRefTrace(self, num):
        self._rtrace = num
        self._calculateRef()

    def _getRefTrace(self):
        return self._rtrace

    def _setMaxShift(self, shift):
        self._maxshift = shift

    def _getMaxShift(self):
        return self._maxshift

    @property
    def ref_trace(self):
        """The trace being used as a reference.

        Raises:
            TypeError: If attempting to set to non integer value.
        """
        return self._getRefTrace()

    @ref_trace.setter
    def ref_trace(self, num):
        if not isinstance(num, int):
            raise TypeError("Expected int; got %s" % type(num), num)
        self._setRefTrace(num)

    @property
    def max_shift(self):
        """Maximum amount to shift around a trace around the target_window"""

        return self._getMaxShift()

    @max_shift.setter
    def max_shift(self, m):
        if not isinstance(m, int):
            raise TypeError("Expected int; got %s" % type(m), m)
        self._setMaxShift(m)

    def _setWindow(self, window):
        self._wdStart, self._wdEnd = window
        self._calculateRef()

    def _getWindow(self):
        return (self._wdStart, self._wdEnd)

    @property
    def target_window(self):
        """Section of the trace we are trying to minimize SAD for.

        This must be a tuple of (first point, last point).

        Raises:
            TypeError: If attempting to set to a non tuple or if points are not
                integers.
        """
        return self._getWindow()

    @target_window.setter
    def target_window(self, win):
        if not isinstance(win, tuple):
            raise TypeError("Expected tuple; got %s" % type(win), win)
        if not isinstance(win[0], int):
            raise TypeError("Expected int; got %s" % type(win[0]), win[0])
        if not isinstance(win[1], int):
            raise TypeError("Expected int; got %s" % type(win[1]), win[1])
        self._setWindow(win)

    @property
    def input_window(self):
        raise NotImplementedError("SAD interface changed. See target_window now.")

    @input_window.setter
    def input_window(self, _):
        raise NotImplementedError("SAD interface changed. See target_window now.")

    def updateLimits(self):
        if self._traceSource:
            self.findParam('windowpt').setLimits((0, self._traceSource.numPoints()-1))
            self._calculateRef()

    def setOutputSad(self, enabled):
        self._debugReturnSad = enabled

    def get_trace(self, n):
        if self.enabled:
            if self._init_not_done:
                self._calculateRef()
                self._init_not_done = False

            trace = self._traceSource.get_trace(n)
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
            print("Not enabled")
            return self._traceSource.get_trace(n)

    getTrace = get_trace

    def _calculateRef(self):
        try:
            self.calcRefTrace(self._rtrace)
        #Probably shouldn't do this, but deals with user enabling preprocessing
        #before trace management setup
        except ValueError:
            pass

    def _findSAD(self, inputtrace):
        sadlen = self._maxshift * 2
        sadarray = np.ones(sadlen)*1E6

        wdlen = self._wdEnd - self._wdStart

        #Shift values
        minshift = -self._maxshift
        maxshift = self._maxshift

        if minshift + self._wdStart < 0:
            raise ValueError("Invalid size or maximum shift, starting search location is < 0")

        if maxshift + self._wdEnd > len(inputtrace):
            raise ValueError("Invalid size or maximum shift, ending search location is outside trace")

        for ptoffset in range(minshift, maxshift):
            ptstart = ptoffset + self._wdStart
            diff_data = inputtrace[ptstart:(ptstart+wdlen)] - self.reftrace
            abs_data = np.abs(diff_data)
            sadarray[ptoffset + self._maxshift] = np.sum(abs_data)
        return sadarray

    def calcRefTrace(self, tnum):
        if self.enabled == False:
            return

        self.reftrace = self._traceSource.get_trace(tnum)[self._wdStart:self._wdEnd]
        sad = self._findSAD(self._traceSource.get_trace(tnum))
        self.refmaxloc = np.argmin(sad)
        self.refmaxsize = min(sad)
        self.maxthreshold = np.mean(sad)


    def _dict_repr(self):
        dict = OrderedDict()
        dict['enabled'] = self.enabled
        dict['ref_trace'] = self.ref_trace
        dict['max_shift'] = self.max_shift
        dict['target_window']    = self.target_window
        return dict

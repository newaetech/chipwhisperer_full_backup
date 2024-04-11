#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2013-2020, NewAE Technology Inc
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
from chipwhisperer.common.utils.util import camel_case_deprecated
try:
    from fastdtw import fastdtw # type: ignore
except ModuleNotFoundError:
    from chipwhisperer.analyzer.utils.fasterdtw import fastdtw
import numpy as np

class ResyncDTW(PreprocessingBase):
    """Align traces using the Dynamic Time Warp algorithm. Doesn't play well
    with noisy traces, but can remove random per-trace delays and synchronize
    entire trace.
    """
    _name = "Resync: Dynamic Time Warp"
    _description = "Aligns traces to match a reference trace using the Fast Dynamic Time Warp algorithm."

    def __init__(self, traceSource=None, name=None):
        PreprocessingBase.__init__(self, traceSource, name=name)
        self._rtrace = 0
        self._radius = 3


    def _setRefTrace(self, num):
        self._rtrace = num

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

    def _setRadius(self, radius):
        self._radius = radius

    def _getRadius(self):
        return self._radius

    @property
    def radius(self):
        """The radius used in the DTW calculation.

        This is an integer value that roughly describes how much the DTW
        algorithm is allowed to backtrack in its search. High values take
        longer but are better at dealing with many smaller delays.
        """
        return self._getRadius()

    @radius.setter
    def radius(self, radius):
        if not isinstance(radius, int):
            raise TypeError("Expected int; got %s" % type(radius), radius)
        self._setRadius(radius)
   
    def get_trace(self, n):
        if not self.enabled:
            return self._traceSource.get_trace(n)

        trace = self._traceSource.get_trace(n)
        ref_trace = self._traceSource.get_trace(self._rtrace)
        if trace is None or ref_trace is None:
            return None

        aligned = self._align_traces(ref_trace, trace)
        return aligned


    getTrace = camel_case_deprecated(get_trace)
        
    def _align_traces(self, ref, trace):
        N = self._traceSource.num_points()
        r = self._radius
        #try:
        # cython fastdtw can't take numpy.memmap inputs, so we convert them to arrays:
        aref = np.array(list(ref))
        atrace = np.array(list(trace))
        dist, path = fastdtw(aref, atrace, radius=r, dist=None)
        #except:
        #    return None
        px = [x for x, y in path]
        py = [y for x, y in path]
        n = [0] * N
        s = [0.0] * N
        for x, y in path:
            s[x] += trace[y]
            n[x] += 1

        ret = [s[i] / n[i] for i in range(N)]
        return ret

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
import time
import pprofile

from ._base import PreprocessingBase
from chipwhisperer.analyzer.utils.fasterdtw import fastdtw

class ResyncDTW(PreprocessingBase):
    _name = "Resync: Dynamic Time Warp"
    _description = "Aligns traces to match a reference trace using the Fast Dynamic Time Warp algorithm."

    def __init__(self, traceSource=None):
        PreprocessingBase.__init__(self, traceSource)
        self.rtrace = 0
        self.debugReturnSad = False
        self.ccStart = 0
        self.ccEnd = 1
        self.wdStart = 0
        self.wdEnd = 1

        self.params.addChildren([
            {'name':'Ref Trace', 'key':'reftrace', 'type':'int', 'value':0, 'action':self.updateScript},
            {'name':'Radius', 'key':'radius', 'type':'int', 'value':3}
        ])
        self.updateScript()

    def updateScript(self, _=None):
        self.addFunction("init", "setEnabled", "%s" % self.findParam('enabled').getValue())
        self.addFunction("init", "setReference", "rtraceno=%d" % (self.findParam('reftrace').getValue()))

    def setReference(self, rtraceno=0):
        self.rtrace = rtraceno
   
    def getTrace(self, n):
        if not self.enabled:
            return self._traceSource.getTrace(n)

        trace = self._traceSource.getTrace(n)
        ref_trace = self._traceSource.getTrace(self.rtrace)
        if trace is None or ref_trace is None:
            return None

        aligned = self.alignTraces(ref_trace, trace)
        return aligned
        
    def alignTraces(self, ref, trace):
        N = self._traceSource.numPoints()
        r = self.findParam('radius').getValue()
        #try:
        dist, path = fastdtw(ref, trace, radius=r, dist=None)
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
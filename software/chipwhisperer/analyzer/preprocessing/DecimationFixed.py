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

        
class DecimationFixed(PreprocessingBase):
    _name = "Decimation: Fixed"
    _description = "Decimate by a fixed factor"

    def __init__(self, parentParam=None, traceSource=None):
        PreprocessingBase.__init__(self, parentParam, traceSource)
        self.setDecimationFactor(2)
        self.params.addChildren([
            {'name':'Decimation = N:1', 'key':'decfactor', 'type':'int', 'default':self._decfactor, 'value':self._decfactor, 'limit':(1, 1000), 'action':lambda _:self.updateScript()}
        ])
        self.updateScript()

    def updateScript(self, ignored=None):
        self.addFunction("init", "setEnabled", "%s" % self.findParam('enabled').getValue())
        self.addFunction("init", "setDecimationFactor", self.findParam('decfactor').getValue())

    def setDecimationFactor(self, decfactor=1):
        self._decfactor = decfactor

    def getTrace(self, n):
        if self.enabled:
            trace = self._traceSource.getTrace(n)
            if trace is None:
                return None

            outtrace = np.zeros(len(range(0, len(trace), self._decfactor)))

            for idx, val in enumerate(range(0, len(trace), self._decfactor)):
                outtrace[idx] = trace[val]

            return outtrace
        else:
            return self._traceSource.getTrace(n)

    def numPoints(self):
        return len(range(0, self._traceSource.numPoints(), self._decfactor))
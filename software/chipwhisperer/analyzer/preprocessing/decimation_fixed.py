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
from chipwhisperer.common.utils.parameter import setupSetParam

        
class DecimationFixed(PreprocessingBase):
    """Throw out some points in each trace, keeping only one in N points."""
    _name = "Decimation: Fixed"
    _description = "Decimate (reduce the sample rate) by a fixed factor."

    def __init__(self, traceSource=None, name=None):
        PreprocessingBase.__init__(self, traceSource, name=name)
        self._dec_factor = 2
        self.params.addChildren([
            {'name':'Decimation = N:1', 'key':'decfactor', 'type':'int', 'get':self._getDecFactor, 'set':self._setDecFactor, 'limits':(1, 1000)}
        ])

    @setupSetParam("Decimation = N:1")
    def _setDecFactor(self, dec=1):
        self._dec_factor = dec

    def _getDecFactor(self):
        return self._dec_factor

    @property
    def dec_factor(self):
        """The current decimation factor. Only one in <dec_factor> samples are
        returned.

        Setter raises ValueError if new factor isn't an int"""
        return self._getDecFactor()

    @dec_factor.setter
    def dec_factor(self, dec):
        if not isinstance(dec, int):
            raise TypeError("Expected int; got %s" % type(dec), dec)
        self._setDecFactor(dec)

    def getTrace(self, n):
        if self.enabled:
            trace = self._traceSource.getTrace(n)
            if trace is None:
                return None

            outtrace = np.zeros(len(range(0, len(trace), self._dec_factor)))

            for idx, val in enumerate(range(0, len(trace), self._dec_factor)):
                outtrace[idx] = trace[val]

            return outtrace
        else:
            return self._traceSource.getTrace(n)

    def numPoints(self):
        if self.enabled:
            return len(range(0, self._traceSource.numPoints(), self._dec_factor))
        else:
            return self._traceSource.numPoints()

    def getSampleRate(self):
        return self._traceSource.getSampleRate()/self._dec_factor

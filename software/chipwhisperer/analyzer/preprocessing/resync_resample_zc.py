#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2013-2018, NewAE Technology Inc
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
from matplotlib.mlab import find # type: ignore
import scipy.signal as sig # type: ignore
from chipwhisperer.common.utils.parameter import setupSetParam


class ResyncResampleZC(PreprocessingBase):
    """
    Resync using Resampling based on Zero-Crossing Bins.
    """
    _name = "Resync: Resample based on Zero-Crossing"
    _description = "Deal with resampling 'bins' based on zero-crossing detection."

    def __init__(self, traceSource=None, name=None):
        PreprocessingBase.__init__(self, traceSource, name=name)
        self._rtrace = 0
        self._zcoffset = 0.0
        self._binlen = 0

        self.params.addChildren([
            {'name':'Ref Trace', 'key':'reftrace', 'type':'int', 'get':self._getRefTrace, 'set':self._setRefTrace},
            {'name':'Zero-Crossing Level', 'key':'zclevel', 'type':'float', 'get':self._getZcLevel, 'set':self._setZcLevel},
            {'name':'Bin Sample Length', 'key':'binlen', 'type':'int', 'limits':(0, 10000), 'get':self._getBinLength, 'set':self._setBinLength},
        ])

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

    @setupSetParam("Zero-Crossing Level")
    def _setZcLevel(self, offset):
        self._zcoffset = offset
        self._calculateRef()

    def _getZcLevel(self):
        return self._zcoffset

    @property
    def zc_offset(self):
        """The level to use as the zero-crossing point.

        Setter raises TypeError if value isn't a float.
        """
        return self._getZcLevel()

    @zc_offset.setter
    def zc_offset(self, offset):
        if not isinstance(offset, float):
            raise TypeError("Expected float; got %s" % type(offset), offset)
        self._setZcLevel(offset)

    @setupSetParam("Bin Sample Length")
    def _setBinLength(self, offset):
        self._zcoffset = offset
        self._calculateRef()

    def _getBinLength(self):
        return self._zcoffset

    @property
    def bin_length(self):
        """The number of samples to be resampled at once. This should be close
        to the number of samples between two zero-crossings.

        If 0, a bin length is automatically calculated.

        Setter raises TypeError if value isn't an int.
        """
        return self._getBinLength()

    @bin_length.setter
    def bin_length(self, len):
        if not isinstance(len, int):
            raise TypeError("Expected float; got %s" % type(len), len)
        self._setBinLength(len)

    def getTrace(self, n):
        if self.enabled:
            trace = self._traceSource.getTrace(n)
            if trace is None:
                return None
            
            trace = trace - self._zcoffset
    
            ind = self._findZerocrossing(trace)
            return self._resampleResize(trace, ind, self._binlen)
        else:
            return self._traceSource.getTrace(n)
   
    def _calculateRef(self):
        try:
            self.calcRefTrace(self._rtrace)
        #Probably shouldn't do this, but deals with user enabling preprocessing
        #before trace management setup
        except ValueError:
            pass

    def calcRefTrace(self, tnum):
        #If not enabled stop
        if self.enabled == False:
            return
        
        if self._binlen == 0:
            self._reftrace = self._traceSource.getTrace(tnum) - self._zcoffset
            ind = self._findZerocrossing(self._reftrace)
            self._binlen = self._findAvgLength(ind)

    def _findZerocrossing(self, a):
        indices = find((a[1:] >= 0) & (a[:-1] < 0))
        return indices

    def _findAvgLength(self, indices):
        diff = 0
        num = 0

        for i in range(1, len(indices)):
            diff += indices[i] - indices[i - 1]
            num += 1

        return diff / num

    def _resampleResize(self, data, indices, targlen):
        targdata = np.zeros(targlen * len(indices))

        # Shift each segment to fit this size, let it ride
        for i in range(1, len(indices)):
            targdata[(i - 1) * targlen:i * targlen] = sig.resample(data[indices[i - 1]:indices[i]], targlen)

        return targdata

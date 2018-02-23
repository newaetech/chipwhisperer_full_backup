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


class NormBase(object):
    """Base Class for Normalization"""
    UseF1Coeff = False
    UseF2Coeff = False
    UseZSource = False
    f1coeff = None
    f2coeff = None
    zdata = None

    def loadF1File(self, f):
        pass

    def loadF2File(self, f):
        pass

    def loadZFile(self, f):
        pass


class NormMean(NormBase):
    """Normalize by mean (e.g. make traces zero-mean)"""
    def processTrace(self, t, tindex):
        return t - np.mean(t)


class NormMeanStd(NormBase):
    """Normalize by mean & std-dev """
    def processTrace(self, t, tindex):
        return (t - np.mean(t)) / np.std(t)


class Normalize(PreprocessingBase):
    """
    Normalize traces by a variety of methods
    """
    _name = "Normalize"
    _description = "Normalize by standard deviation."

    def __init__(self, traceSource=None, name=None):
        PreprocessingBase.__init__(self, traceSource, name=name)
        self._norm = NormMean()

        self.params.addChildren([
            {'name':'Mode', 'key':'type', 'type':'list', 'values':{"y=x-mean(x)":NormMean, "y=(x-mean(x))/stddev(x)":NormMeanStd}, 'get':self._getNormMode, 'set':self._setNormMode}
        ])

    @setupSetParam("Mode")
    def _setNormMode(self, cls):
        self._norm = cls()

    def _getNormMode(self):
        return self._norm.__class__

    @property
    def mode(self):
        """The current normalization method. Options are:
        - "mean": Subtract the mean from the traces (ie: change to 0-mean)
        - "mean_stddev": Subtract the mean and divide by the standard deviation
          (ie: change to mean=0, stddev=1)

        Setter raises ValueError if mode isn't one of these
        """
        mode = self._getNormMode()
        if mode == NormMean:
            return "mean"
        elif mode == NormMeanStd:
            return "mean_stddev"
        else:
            return "?"

    @mode.setter
    def mode(self, mode):
        if mode == "mean":
            self._setNormMode(NormMean)
        elif mode == "mean_stddev":
            self._setNormMode(NormMeanStd)
        else:
            raise ValueError("Unrecognized mode; expected one of 'mean', 'mean_stddev'", mode)


    def getTrace(self, n):
        if self.enabled:
            trace = self._traceSource.getTrace(n)

            if trace is None:
                return None
            proc = self._norm.processTrace(trace, n)

            return proc
        else:
            return self._traceSource.getTrace(n)
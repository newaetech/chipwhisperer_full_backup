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
from chipwhisperer.common.utils.parameter import setupSetParam


class AddNoiseRandom(PreprocessingBase):
    """Add Gaussian white noise to each of the traces.
    """
    _name = "Add Noise: Amplitude"
    _description = "Add random noise."

    def __init__(self, traceSource=None, name=None):
        PreprocessingBase.__init__(self, traceSource, name=name)
        self._noise_std_dev = 0.005
        self.params.addChildren([
            {'name':'Noise Std-Dev', 'key':'noisestddev', 'type':'float', 'step':0.001, 'limits':(0, 1.0), 'get':self._getNoise, 'set':self._setNoise}
        ])

    @setupSetParam("Noise Std-Dev")
    def _setNoise(self, std_dev):
        self._noise_std_dev = std_dev

    def _getNoise(self):
        return self._noise_std_dev

    @property
    def noise(self):
        """The standard deviation of the noise.

        If 0, the traces are passed through without being modified. Otherwise,
        random values from N(0, noise) are added to each of the points.

        Setter raises TypeError if noise level isn't a number
        """
        return self._getNoise()

    @noise.setter
    def noise(self, std_dev):
        if not isinstance(std_dev, (int, float)):
            raise TypeError("Expected number; got %s" % type(std_dev), std_dev)
        self._setNoise(std_dev)

    def getTrace(self, n):
        if self.enabled:
            trace = self._traceSource.getTrace(n)
            if trace is None:
                return None

            if self._noise_std_dev == 0:
                return trace
            else:
                return trace + np.random.normal(scale=self._noise_std_dev, size=len(trace))
        else:
            return self._traceSource.getTrace(n)

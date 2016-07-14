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

import random
import numpy as np
from ._base import PreprocessingBase


class AddNoiseJitter(PreprocessingBase):
    """
    Generic filter, pulls in from SciPy for doing the actual filtering of things
    """
    _name = "Add Noise: Time Jitter"
    _description = "Add random jitter. This module is used for testing resyncronization modules, and has no use " \
                  "in actual analysis."

    def __init__(self, traceSource=None):
        PreprocessingBase.__init__(self, traceSource)
        self.maxJitter = 0
        self.params.addChildren([
            {'name':'Max Jitter (+/- cycles)', 'key':'jitter', 'type':'int', 'value':self.maxJitter, 'limits':(0, 1000), 'action':self.updateScript}
        ])
        self.updateScript()

    def updateScript(self, _=None):
        self.addFunction("init", "setEnabled", "%s" % self.findParam('enabled').getValue())
        jit = self.findParam('jitter').getValue()
        self.addFunction("init", "setMaxJitter", "%d" % jit)
   
    def setMaxJitter(self, jit):
        self.maxJitter = jit
   
    def getTrace(self, n):
        if self.enabled:
            trace = self._traceSource.getTrace(n)
            if trace is None:
                return None
            
            jit = random.randint(-self.maxJitter, self.maxJitter)

            return roll_zeropad(trace, jit)
        else:
            return self._traceSource.getTrace(n)

        
# This function stolen from: http://stackoverflow.com/questions/2777907/python-numpy-roll-with-padding
def roll_zeropad(a, shift, axis=None):
    """
    Roll array elements along a given axis. Elements rolled off the end of the array are treated as zeros.

    :param a: Input Array
    :type a: array_like
    
    :param shift: The number of places by which elements are shifted.
    :type shift: int

    :param axis: The axis along which elements are shifted.  By default, the array
                 is flattened before shifting, after which the original
                 shape is restored.
    :type axis: int, optional      

    :rtype: ndarray, with the same shape as ``a``

    """
    a = np.asanyarray(a)
    if shift == 0: return a
    if axis is None:
        n = a.size
        reshape = True
    else:
        n = a.shape[axis]
        reshape = False
    if np.abs(shift) > n:
        res = np.zeros_like(a)
    elif shift < 0:
        shift += n
        zeros = np.zeros_like(a.take(np.arange(n - shift), axis))
        res = np.concatenate((a.take(np.arange(n - shift, n), axis), zeros), axis)
    else:
        zeros = np.zeros_like(a.take(np.arange(n - shift, n), axis))
        res = np.concatenate((zeros, a.take(np.arange(n - shift), axis)), axis)
    if reshape:
        return res.reshape(a.shape)
    else:
        return res

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
from _base import PreprocessingBase
from matplotlib.mlab import find
import scipy.signal as sig


class ResyncResampleZC(PreprocessingBase):
    """
    Resync using Resampling based on Zero-Crossing Bins.
    """
    name = "Resync: Resample based on Zero-Crossing"
    description = "Deals with resampling 'bins' based on zero-crossing detection"

    def _setupParameters(self):
        self.rtrace = 0
        self.debugReturnSad = False
        return [ {'name':'Ref Trace', 'key':'reftrace', 'type':'int', 'value':0, 'set':self.updateScript},
                 {'name':'Zero-Crossing Level', 'key':'zclevel', 'type':'float', 'value':0.0, 'set':self.updateScript},
                 {'name':'Bin Sample Length', 'key':'binlen', 'type':'int', 'value':0, 'limits':(0, 10000), 'set':self.updateScript},
                ]

    def updateScript(self, ignored=None):
        self.addFunction("init", "setEnabled", "%s" % self.findParam('enabled').value())

        zclevel = self.findParam('zclevel').value()
        binlength = self.findParam('binlen').value()


        self.addFunction("init", "setReference", "rtraceno=%d, zcoffset=%f, binlength=%d" % (
                            self.findParam('reftrace').value(),
                            zclevel,
                            binlength
                            ))

    def setReference(self, rtraceno=0, zcoffset=0.0, binlength=0):
        self.rtrace = rtraceno
        self.zcoffset = zcoffset
        self.binlen = binlength
        self.init()

    def getTrace(self, n):
        if self.enabled:
            trace = self.traceSource.getTrace(n)
            if trace is None:
                return None
            
            trace = trace - self.zcoffset
    
            ind = self.findZerocrossing(trace)
            return self.resampleResize(trace, ind, self.binlen)
        else:
            return self.traceSource.getTrace(n)
   
    def init(self):
        try:
            self.calcRefTrace(self.rtrace)
        #Probably shouldn't do this, but deals with user enabling preprocessing
        #before trace management setup
        except ValueError:
            pass

    def calcRefTrace(self, tnum):
        #If not enabled stop
        if self.enabled == False:
            return
        
        if self.binlen == 0:
            self.reftrace = self.traceSource.getTrace(tnum) - self.zcoffset
            ind = self.findZerocrossing(self.reftrace)
            self.binlen = self.findAvgLength(ind)

    def findZerocrossing(self, a):
        indices = find((a[1:] >= 0) & (a[:-1] < 0))
        return indices

    def findAvgLength(self, indices):
        diff = 0
        num = 0

        for i in range(1, len(indices)):
            diff += indices[i] - indices[i - 1]
            num += 1

        return diff / num

    def resampleResize(self, data, indices, targlen):
        targdata = np.zeros(targlen * len(indices))

        # Shift each segment to fit this size, let it ride
        for i in range(1, len(indices)):
            targdata[(i - 1) * targlen:i * targlen] = sig.resample(data[indices[i - 1]:indices[i]], targlen)

        return targdata

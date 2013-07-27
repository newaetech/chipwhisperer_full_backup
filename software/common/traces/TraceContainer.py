#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2013, Colin O'Flynn <coflynn@newae.com>
# All rights reserved.
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

__author__ = "Colin O'Flynn"

import sys
sys.path.append('../common')

import numpy as np
from TraceContainerConfig import TraceContainerConfig

#For profiling support (not 100% needed)
import pstats, cProfile

class TraceContainer():
    def __init__(self):
        self.textints = None
        self.textouts = None
        self.knownkey = None
        self.directory = None
        self.dirty = False
        self.tracedtype = np.double  
        self.traces = None
        
        self.config = TraceContainerConfig()

    def setDirty(self, dirty):
        self.dirty = dirty

    def numPoints(self):
        try:        
            return self.traces.shape[1]
        except:
            return 0

    def numTraces(self):
        try:        
            return self.traces.shape[0]
        except:
            return 0 

    def addTrace(self, trace, textin, textout, key, dtype=np.double):
        self.addWave(trace, dtype)
        self.addTextin(textin)
        self.addTextout(textout)

    def addWave(self, trace, dtype=None):
        if self.traces is None:
            if dtype is None:
                dtype = np.double
            
            self.tracedtype = dtype            
            self.traces = np.array(trace, dtype=self.tracedtype, ndim=2)
        else:
            self.traces.append(trace, 0)
            
        self.setDirty(True)

    def addTextin(self, data):
        self.textins.append(data)
        
    def addTextout(self, data):
        self.textouts.append(data)

    def getTrace(self, n):
        data = self.traces[n]

        #Following line will normalize all traces relative to each
        #other by mean & standard deviation
        #data = (data - np.mean(data)) / np.std(data)
        return data

    def getTextin(self, n):
        return self.textins[n]

    def getTextout(self, n):
        return self.textouts[n]

    def getKnownKey(self, n=None):    
        return self.knownkey
    
    def loadAllTraces(self, directory=None, prefix=""):
        """Placeholder for load command. May not actually read everything into memory depending on format."""
        pass
    
    def saveAllTraces(self, directory, prefix=""):
        """Placeholder for save command. May not actually read everything into memory depending on format."""
        pass
    
    def copyTo(self, srcTraces=None, srcFormat=None):
        """Placeholder for copy/import command. Different from load as copies data INTO this classes format, possibly from another format"""
        pass
        
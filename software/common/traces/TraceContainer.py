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

class TraceContainer(object):
    """
    TraceContainer holds traces for the system to operate on. This can include both reading in traces for analysis, and
    writing traces to disk.
    
    This class is normally used as a base class for some specific format. For example the 'TraceFormatNative' class
    adds functions for reading/storing data in the 'native' ChipWhisperer format.
    """
    
    def __init__(self):
        self.textins = []
        self.textouts = []
        self.knownkey = None
        self.directory = None
        self.dirty = False
        self.tracedtype = np.double  
        self.traces = None
        self.tracehint = 1
        self.pointhint = 0
        self._numTraces = 0
        
        self.config = TraceContainerConfig()

    def setDirty(self, dirty):
        self.dirty = dirty

    def numPoints(self):
        try:        
            return self.traces.shape[1]
        except:
            return 0
        
    def setTraceHint(self, traces):
        self.tracehint = traces
        
    def setPointHint(self, points):
        self.pointhint = points

    def numTraces(self):
        cfint = int(self.config.attr("numTraces"))
        if cfint != self._numTraces:
            self._numTraces = max(cfint, self._numTraces)
        return self._numTraces

    def addTrace(self, trace, textin, textout, key, dtype=np.double):
        self.addWave(trace, dtype)
        self.addTextin(textin)
        self.addTextout(textout)
        
    def writeDataToConfig(self):
        self.config.setAttr("numTraces", self.numTraces())
        self.config.setAttr("numPoints", self.numPoints())      

    def addWave(self, trace, dtype=None):                
        if self.traces is None:
            if dtype is None:
                dtype = np.double            
            self.tracedtype = dtype    
            self.traces = np.zeros((self.tracehint, len(trace)), dtype=dtype)           
            self.traces[self._numTraces][:] = trace        
        else:
            #Check can fit this
            if self.traces.shape[0] <= self._numTraces:
                if self._numTraces >= self.tracehint:
                    #Tracehint wrong - increase by 25
                    self.tracehint += 25
                    
                #Do a resize now to allocate more memory
                self.traces.resize((self.tracehint, self.traces.shape[1]))
                        
            self.traces[self._numTraces][:] = trace
            
        self._numTraces += 1
        self.setDirty(True)
        self.writeDataToConfig()

    def setKnownKey(self, key):
        self.knownkey = key

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
    
    def prepareDisk(self):
        """Placeholder called after creating a new file setup, but before actually writing traces to it"""
        
        #if self.traces is None:
        #    self.traces = np.zeros((self.tracehint, self.pointhint))
        
        
    
    def loadAllTraces(self, directory=None, prefix=""):
        """Placeholder for load command. May not actually read everything into memory depending on format."""
        raise AttributeError("%s doesn't have this method implemented"%self.__class__.__name__)
    
    def saveAllTraces(self, directory, prefix=""):
        """Placeholder for save command."""
        raise AttributeError("%s doesn't have this method implemented"%self.__class__.__name__)
    
    def copyTo(self, srcTraces=None, srcFormat=None):
        """Placeholder for copy/import command. Different from load as copies data INTO this classes format, possibly from another format"""
        raise AttributeError("%s doesn't have this method implemented"%self.__class__.__name__)
    
    def closeAll(self):
        """Writer is done, can close/save any files."""               
        raise AttributeError("%s doesn't have this method implemented"%self.__class__.__name__)
        
        
if __name__ == "__main__":
    test = TraceContainer()
    wave = [1,2,3,4,5,6]
    test.addTrace(wave, None, None, None)
    test.addTrace(wave, None, None, None)
    test.addTrace(wave, None, None, None)
    print test.numTraces()
    print test.numPoints()

        
#!/usr/bin/pythonh
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

import sys

try:
    from PySide.QtCore import *
    from PySide.QtGui import *
except ImportError:
    print "ERROR: PySide is required for this program"
    sys.exit()

import numpy as np
from chipwhisperer.analyzer.preprocessing.PreprocessingBase import PreprocessingBase
from openadc.ExtendedParameter import ExtendedParameter
from pyqtgraph.parametertree import Parameter
        
class ResyncPeakDetect(PreprocessingBase):
    """
    Resyncronize based on peak value.
    """
    
    descrString = "Line up traces so peak (either max positive or max negative) within" \
    " some given range of points all aligns. For each trace the following must hold or the trace is rejected:\n" \
    "   (1-valid limit) < (peak value from candidate trace) / (peak value from reference) < (1+valid limit)\n" \
    "If 'valid limit' is 0 then this is ignored, and all traces are kept."   
     
    def setupParameters(self):
        self.rtrace = 0
        self.debugReturnCorr = False
        resultsParams = [{'name':'Enabled', 'key':'enabled', 'type':'bool', 'value':True, 'set':self.setEnabled},
                         {'name':'Ref Trace #', 'type':'int', 'value':0, 'set':self.setRefTrace},
                         {'name':'Peak Type', 'type':'list', 'value':'Max', 'values':['Max', 'Min'],  'set':self.setType},
                         {'name':'Point Range', 'type':'rangegraph', 'graphwidget':self.parent.waveformDock.widget(), 'set':self.setPointRange},
                         {'name':'Valid Limit', 'type':'float', 'value':0, 'step':0.1, 'limits':(0,10), 'set':self.setValidLimit}, 
                         {'name':'Desc', 'type':'text', 'value':self.descrString}
                      ]
        
        self.params = Parameter.create(name='Peak Detect', type='group', children=resultsParams)
        ExtendedParameter.setupExtended(self.params, self)
        self.ccStart = 0
        self.ccEnd = 0
        self.limit = 0
        self.type = max
        
    def setValidLimit(self, limit):
        self.limit = limit
        
    def setType(self, t):
        self.type = t
    
    def setPointRange(self, val):
        self.ccStart = val[0]
        self.ccEnd = val[1]
   
    def setOutputCorr(self, enabled):
        self.debugReturnCorr = enabled
   
    def getTrace(self, n):
        if self.enabled:
            #TODO: fftconvolve
            trace = self.trace.getTrace(n)
            if trace is None:
                return None
            if self.type == 'Max':
                newmaxloc = np.argmax(trace[self.ccStart:self.ccEnd])
                maxval = max(trace[self.ccStart:self.ccEnd])                    
            else:
                newmaxloc = np.argmin(trace[self.ccStart:self.ccEnd])
                maxval = min(trace[self.ccStart:self.ccEnd])

            if self.limit:
                if (maxval > self.refmaxsize * (1.0 + self.limit)) | (maxval < self.refmaxsize * (1.0 - self.limit)):
                    return None
            
            diff = newmaxloc-self.refmaxloc
            if diff < 0:
                trace = np.append(np.zeros(-diff), trace[:diff])
            elif diff > 0:
                trace = np.append(trace[diff:], np.zeros(diff))
            return trace
            
        else:
            return self.trace.getTrace(n)       
    
    def init(self):
        try:
            self.calcRefTrace(self.rtrace)
        except ValueError:
            self.findParam('enabled').setValue(False)
   
    def setRefTrace(self, tnum):
        self.rtrace = tnum
        
    def calcRefTrace(self, tnum):
        reftrace = self.trace.getTrace(tnum)[self.ccStart:self.ccEnd]
        if self.type == 'Max':
            self.refmaxloc = np.argmax(reftrace)
            self.refmaxsize = max(reftrace)
        else:
            self.refmaxloc = np.argmin(reftrace)
            self.refmaxsize = min(reftrace)

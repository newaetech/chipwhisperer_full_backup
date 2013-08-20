#!/usr/bin/pythonh
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
#=================================================

import sys

try:
    from PySide.QtCore import *
    from PySide.QtGui import *
except ImportError:
    print "ERROR: PySide is required for this program"
    sys.exit()
    
sys.path.append('../common')
sys.path.append('../../openadc/controlsw/python/common')
imagePath = '../common/images/'

import numpy as np
from ExtendedParameter import ExtendedParameter

try:
    import pyqtgraph as pg
    import pyqtgraph.multiprocess as mp
    import pyqtgraph.parametertree.parameterTypes as pTypes
    from pyqtgraph.parametertree import Parameter, ParameterTree, ParameterItem, registerParameterType
    #print pg.systemInfo()    
except ImportError:
    print "ERROR: PyQtGraph is required for this program"
    sys.exit()


from functools import partial
import scipy as sp
import numpy as np
        
class ResyncPeakDetect(QObject):
    """
    Use maximum point in trace to resync
    """
    paramListUpdated = Signal(list)
     
    def __init__(self, parent):
        super(ResyncPeakDetect, self).__init__()
                
        self.enabled = True
        self.rtrace = 0
        self.debugReturnCorr = False
        resultsParams = [{'name':'Enabled', 'type':'bool', 'value':True, 'set':self.setEnabled},
                         {'name':'Ref Trace', 'type':'int', 'value':0, 'set':self.setRefTrace},
                         {'name':'Ref Point Start', 'type':'int', 'set':self.setRefPointStart},
                         {'name':'Ref Point End', 'type':'int', 'set':self.setRefPointEnd}       
                      ]
        
        self.params = Parameter.create(name='Peak Detect', type='group', children=resultsParams)
        ExtendedParameter.setupExtended(self.params)
        self.parent = parent
        self.setTraceManager(parent.manageTraces.iface)
        self.ccStart = 0
        self.ccEnd = 0
        
    def paramList(self):
        return [self.params]
    
    def setRefPointStart(self, start):
        self.ccStart = start
        
    def setRefPointEnd(self, end):
        self.ccEnd = end
    
    def setEnabled(self, enabled):
        self.enabled = enabled
   
    def setOutputCorr(self, enabled):
        self.debugReturnCorr = enabled
   
    def getTrace(self, n):
        if self.enabled:
            #TODO: fftconvolve
            trace = self.trace.getTrace(n)
            if trace is None:
                return None
            newmaxloc = np.argmin(trace[self.ccStart:self.ccEnd])
            maxval = min(trace[self.ccStart:self.ccEnd])
            if (maxval > self.refmaxsize * 1.1) | (maxval < self.refmaxsize * 0.9):
                return None
            
            diff = newmaxloc-self.refmaxloc
            if diff < 0:
                trace = np.append(np.zeros(-diff), trace[:diff])
            elif diff > 0:
                trace = np.append(trace[diff:], np.zeros(diff))
            return trace
            
        else:
            return self.trace.getTrace(n)       
    
    def getTextin(self, n):
        return self.trace.getTextin(n)

    def getTextout(self, n):
        return self.trace.getTextout(n)
    
    def getKnownKey(self, n=None):
        return self.trace.getKnownKey()
   
    def init(self):
        self.calcRefTrace(self.rtrace)
   
    def setTraceManager(self, tmanager):
        self.trace = tmanager    
    
    def setRefTrace(self, tnum):
        self.rtrace = tnum
        
    def calcRefTrace(self, tnum):
        reftrace = self.trace.getTrace(tnum)[self.ccStart:self.ccEnd]
        self.refmaxloc = np.argmin(reftrace)
        self.refmaxsize = min(reftrace)
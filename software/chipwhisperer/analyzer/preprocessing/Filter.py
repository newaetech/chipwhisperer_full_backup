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
#=================================================

import sys

try:
    from PySide.QtCore import *
    from PySide.QtGui import *
except ImportError:
    print "ERROR: PySide is required for this program"
    sys.exit()
    
imagePath = '../common/images/'

from openadc.ExtendedParameter import ExtendedParameter

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
        
class Filter(QObject):
    """
    Generic filter, pulls in from SciPy
    """
    paramListUpdated = Signal(list)
     
    def __init__(self, parent):
        super(Filter, self).__init__()
                
        self.enabled = True
        resultsParams = [{'name':'Enabled', 'type':'bool', 'value':True, 'set':self.setEnabled},
                         {'name':'Form', 'key':'form', 'type':'list', 'values':{"Butterworth":sp.signal.butter}, 'set':self.updateFilter},
                         {'name':'Type', 'key':'type', 'type':'list', 'values':["low", "high"], 'value':'low', 'set':self.updateFilter},
                         {'name':'Critical Freq (0-1)', 'key':'freq', 'type':'float', 'limits':(0,1), 'value':0.1, 'set':self.updateFilter},
                         {'name':'Order', 'key':'order', 'type':'int', 'limits':(1,32), 'value':5, 'set':self.updateFilter},                                
                      ]
        
        self.params = Parameter.create(name='Filter', type='group', children=resultsParams)
        ExtendedParameter.setupExtended(self.params, self)
        self.parent = parent
        self.setTraceManager(parent.manageTraces.iface)
        self.updateFilter()
        
        self.NumTrace = 1

    def paramList(self):
        return [self.params]
        
    def setEnabled(self, enabled):
        self.enabled = enabled
   
    def updateFilter(self, param1=None):
        filt = self.findParam('form').value()
        N = self.findParam('order').value()
        ftype = self.findParam('type').value()
        freq = self.findParam('freq').value()
        b, a = filt(N, freq, ftype)
        self.b = b
        self.a = a
   
    def getTrace(self, n):
        if self.enabled:
            trace = self.trace.getTrace(n)
            if trace is None:
                return None
            
            filttrace = sp.signal.lfilter(self.b, self.a, trace)
            
            #print len(trace)
            #print len(filttrace)
            
            return filttrace
           
            
        else:
            return self.trace.getTrace(n)       
    
    def getTextin(self, n):
        return self.trace.getTextin(n)

    def getTextout(self, n):
        return self.trace.getTextout(n)
    
    def getKnownKey(self, n=None):
        return self.trace.getKnownKey()
   
    def init(self):
        pass
   
    def setTraceManager(self, tmanager):
        self.trace = tmanager    

        
    
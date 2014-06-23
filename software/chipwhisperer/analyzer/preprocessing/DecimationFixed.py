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

import sys

try:
    from PySide.QtCore import *
    from PySide.QtGui import *
except ImportError:
    print "ERROR: PySide is required for this program"
    sys.exit()

from chipwhisperer.analyzer.preprocessing.PreprocessingBase import PreprocessingBase
from openadc.ExtendedParameter import ExtendedParameter
from pyqtgraph.parametertree import Parameter

# from functools import partial
import scipy as sp
import numpy as np

        
class DecimationFixed(PreprocessingBase):
    """
    Decimate by fixed amount
    """

    descrString = "Decimate by a fixed factor"
     
    def setupParameters(self):

        resultsParams = [{'name':'Enabled', 'type':'bool', 'value':True, 'set':self.setEnabled},
                         {'name':'Decimation = N:1', 'key':'decfactor', 'type':'int', 'value':1, 'limit':(1, 1000)},
                      ]
        
        self.params = Parameter.create(name='Fixed Decimation', type='group', children=resultsParams)
        ExtendedParameter.setupExtended(self.params, self)


    def getTrace(self, n):
        if self.enabled:
            trace = self.trace.getTrace(n)
            if trace is None:
                return None
            
            
            outtrace = np.zeros(len(trace))

            decfactor = self.findParam('decfactor').value()
            for idx, val in enumerate(range(0, len(trace), decfactor)):
                outtrace[idx] = trace[val]

            return outtrace
            
        else:
            return self.trace.getTrace(n)       
    
   

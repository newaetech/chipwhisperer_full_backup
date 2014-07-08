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
    
import random
import numpy as np

from pyqtgraph.parametertree import Parameter
from openadc.ExtendedParameter import ExtendedParameter
from chipwhisperer.analyzer.preprocessing.PreprocessingBase import PreprocessingBase


class AddNoiseRandom(PreprocessingBase):
    """
    Does some crap
    """

    descrString = "Add random noise"
     
    def setupParameters(self):
        ssParams = [{'name':'Enabled', 'type':'bool', 'value':True, 'set':self.setEnabled},
                         {'name':'Noise Std-Dev', 'key':'noise std-dev', 'type':'float', 'limits':(0, 1.0), 'set':self.setMaxNoise},
                         {'name':'Desc', 'type':'text', 'value':self.descrString}
                      ]
        self.params = Parameter.create(name='Add Random Noise', type='group', children=ssParams)
        ExtendedParameter.setupExtended(self.params, self)
        self.maxJitter = 0
   
    def setMaxNoise(self, maxNoise):
        self.maxNoise = maxNoise
   
    def getTrace(self, n):
        if self.enabled:
            trace = self.trace.getTrace(n)
            if trace is None:
                return None
            
            return trace + np.random.normal(scale=self.maxNoise, size=len(trace))
            
        else:
            return self.trace.getTrace(n)

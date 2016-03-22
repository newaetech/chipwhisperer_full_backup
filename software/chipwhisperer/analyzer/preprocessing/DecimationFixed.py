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

from chipwhisperer.analyzer.preprocessing.PreprocessingBase import PreprocessingBase
from chipwhisperer.common.api.ExtendedParameter import ExtendedParameter
from pyqtgraph.parametertree import Parameter
import numpy as np

def getClass():
    """"Returns the Main Class in this Module"""
    return DecimationFixed

        
class DecimationFixed(PreprocessingBase):
    """
    Decimate by fixed amount
    """

    name = "Decimation: Fixed"
    descrString = "Decimate by a fixed factor"

    def setupParameters(self):
        self.setDecimationFactor(1)
        resultsParams = [{'name':'Enabled', 'key':'enabled', 'type':'bool', 'value':self.enabled, 'set':self.updateScript},
                         {'name':'Decimation = N:1', 'key':'decfactor', 'type':'int', 'value':self._decfactor, 'limit':(1, 1000), 'set':self.updateScript},
                         # {'name':'Decimation Type', 'values':''}
                         {'name':'Description', 'type':'text', 'value':self.descrString, 'readonly':True}
                      ]
        
        self.params = Parameter.create(name=self.name, type='group', children=resultsParams)
        ExtendedParameter.setupExtended(self.params, self)
        self.updateScript()

    def updateScript(self, ignored=None):
        self.addFunction("init", "setEnabled", "%s" % self.findParam('enabled').value())
        self.addFunction("init", "setDecimationFactor", self.findParam('decfactor').value())

    def setDecimationFactor(self, decfactor=1):
        self._decfactor = decfactor

    def getTrace(self, n):
        if self.enabled:
            trace = self.trace.getTrace(n)
            if trace is None:
                return None

            decfactor = self._decfactor

            # outtrace = np.zeros(len(trace))

            outtrace = np.zeros(len(range(0, len(trace), decfactor)))


            for idx, val in enumerate(range(0, len(trace), decfactor)):
                outtrace[idx] = trace[val]

            return outtrace

        else:
            return self.trace.getTrace(n)
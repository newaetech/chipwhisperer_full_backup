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
from scipy import signal

def getClass():
    """"Returns the Main Class in this Module"""
    return Filter


class Filter(PreprocessingBase):
    """
    Generic filter, pulls in from SciPy for doing the actual filtering of things
    """
    name = "Digital Filter"
    descrString = "..." #TODO: put something here

    def setupParameters(self):
        ssParams = [{'name':'Enabled', 'key':'enabled', 'type':'bool', 'value':True, 'set':self.updateScript},
                         {'name':'Form', 'key':'form', 'type':'list', 'values':{"Butterworth":"signal.butter"}, 'set':self.updateScript},
                         {'name':'Type', 'key':'type', 'type':'list', 'values':["low", "high", "bandpass"], 'value':'low', 'set':self.updateScript},
                         {'name':'Critical Freq #1 (0-1)', 'key':'freq1', 'type':'float', 'limits':(0, 1), 'step':0.05, 'value':0.1, 'set':self.updateScript},
                         {'name':'Critical Freq #2 (0-1)', 'key':'freq2', 'type':'float', 'limits':(0, 1), 'step':0.05, 'value':0.8, 'set':self.updateScript},
                         {'name':'Order', 'key':'order', 'type':'int', 'limits':(1, 32), 'value':5, 'set':self.updateScript},
                         {'name':'Description', 'type':'text', 'value':self.descrString, 'readonly':True}
                      ]
        self.params = Parameter.create(name=self.getName(), type='group', children=ssParams)
        ExtendedParameter.setupExtended(self.params, self)

        self.updateScript()
        
        # Setup imports required
        self.importsAppend("import scipy as sp")

    def setFilterForm(self, filtform=signal.butter):
        """Set the filter type in object"""
        self.filterForm = filtform

    def setFilterParams(self, form='low', freq=0.8, order=5):
        self.b, self.a = self.filterForm(order, freq, form)

    def updateScript(self, param1=None):
        self.addFunction("init", "setEnabled", "%s" % self.findParam('enabled').value())
        
        ftype = self.findParam('type').value()
        freq1 = self.findParam('freq1').value()
        freq2 = self.findParam('freq2').value()
        
        if ftype == "bandpass":
            self.findParam('freq2').show()
            freqs = "(%f, %f)" % (freq1, freq2)
        else:
            self.findParam('freq2').hide()
            freqs = "%f" % freq1

        self.addFunction("init", "setFilterForm", self.findParam('form').value())
        self.addFunction("init", "setFilterParams", "form='%s', freq=%s, order=%d" % (
                                ftype,
                                freqs,
                                self.findParam('order').value()
                            ))
   
    def getTrace(self, n):
        if self.enabled:
            trace = self.trace.getTrace(n)
            if trace is None:
                return None
            
            filttrace = signal.lfilter(self.b, self.a, trace)
            
            return filttrace
            
        else:
            return self.trace.getTrace(n)

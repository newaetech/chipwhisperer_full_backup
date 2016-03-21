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

from chipwhisperer.common.api.ExtendedParameter import ExtendedParameter
from pyqtgraph.parametertree import Parameter
from chipwhisperer.common.api.autoscript import AutoScript
from chipwhisperer.common.utils import util

def getClass():
    """"Returns the Main Class in this Module"""
    return PreprocessingBase


class PreprocessingBase(AutoScript):
    """
    Base Class for all preprocessing modules
    """
    name = "None"
    descrString = ""

    def __init__(self, traceSource, graphwidget):
        """Pass None if you don't want graphwidget"""
        super(PreprocessingBase, self).__init__()
        self.graphwidget = graphwidget
        self.enabled = True
        self.setTraceSource(traceSource)
        self.setupParameters()
        self.paramListUpdated = util.Signal()

    def setupParameters(self):
        """Setup parameters specific to preprocessing module"""
        ssParams = [{'name':'Enabled', 'type':'bool', 'value':self.enabled, 'set':self.setEnabled},
                    # PUT YOUR PARAMETERS HERE
                    {'name':'Description', 'type':'text', 'value':self.descrString, 'readonly':True}]
        self.params = Parameter.create(name=self.name, type='group', children=ssParams)
        ExtendedParameter.setupExtended(self.params, self)

    def updateScript(self, ignored=None):
        pass

    def paramList(self):
        """Returns the parameter list"""
        return [self.params]

    def setEnabled(self, enabled):
        """Turn on/off this preprocessing"""
        self.enabled = enabled

    def getTrace(self, n):
        """Get trace number n"""
        if self.enabled:
            trace = self.trace.getTrace(n)

            # Do your preprocessing here

            return trace
        else:
            return self.trace.getTrace(n)

    def getTextin(self, n):
        """Get text-in number n"""
        return self.trace.getTextin(n)

    def getTextout(self, n):
        """Get text-out number n"""
        return self.trace.getTextout(n)

    def getKnownKey(self, n=None):
        """Get known-key number n"""
        return self.trace.getKnownKey(n)

    def init(self):
        """Do any initilization required once all traces are loaded"""
        pass

    def setTraceSource(self, tmanager):
        """Set the input trace source"""
        self.trace = tmanager

    def numPoint(self):
        return self.trace.numPoint()

    def numTrace(self):
        return self.trace.numTrace()

    def getSegmentList(self):
        return self.trace.getSegmentList()

    def getAuxData(self, n, auxDic):
        return self.trace.getAuxData(n, auxDic)

    def findMappedTrace(self, n):
        return self.trace.findMappedTrace(n)

    def getName(self):
        return self.name

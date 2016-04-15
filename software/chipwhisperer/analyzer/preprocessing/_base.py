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

from chipwhisperer.common.api.autoscript import AutoScript
from chipwhisperer.common.utils import plugin


class PreprocessingBase(AutoScript, plugin.PluginTemplate):
    """
    Base Class for all preprocessing modules
    Derivate Classes work like this:
        - setupParameters and updateScript are used by the GUI to create the parameters list and generate the API scripts
          You need to pass the graphWidget reference in the constructor in order to allow access to it
        - the other methods are used by the API to apply the preprocessing filtering
          You need to pass the traceSource reference in the constructor in order to apply the preprocessing step
    """
    name = "None"
    description = ""

    def __init__(self, traceSource = None, graphWidget=None):
        super(PreprocessingBase, self).__init__()
        self.graphWidget = graphWidget
        self.enabled = True
        self.traceSource = traceSource
        self.updateScript()

    def setupParameters(self):
        """Setup parameters specific to preprocessing module"""
        return [{'name':'Enabled', 'type':'bool', 'value':self.enabled, 'set':self.setEnabled},
                {'name':'Description', 'type':'text', 'value':self.description, 'readonly':True}].extend(self._setupParameters())

    def _setupParameters(self):
        # PUT YOUR CUSTOMIZED PARAMETERS HERE
        return []

    def updateScript(self, ignored=None):
        pass

    def setEnabled(self, enabled):
        """Turn on/off this preprocessing"""
        self.enabled = enabled

    def getTrace(self, n):
        """Get trace number n"""
        if self.enabled:
            trace = self.traceSource.getTrace(n)
            # Do your preprocessing here
            return trace
        else:
            return self.traceSource.getTrace(n)

    def getTextin(self, n):
        """Get text-in number n"""
        return self.traceSource.getTextin(n)

    def getTextout(self, n):
        """Get text-out number n"""
        return self.traceSource.getTextout(n)

    def getKnownKey(self, n=None):
        """Get known-key number n"""
        return self.traceSource.getKnownKey(n)

    def init(self):
        """Do any initilization required once all traces are loaded"""
        pass

    def numPoint(self):
        return self.traceSource.numPoints()

    def numTrace(self):
        return self.traceSource.numTraces()

    def getSegmentList(self):
        return self.traceSource.getSegmentList()

    def getAuxData(self, n, auxDic):
        return self.traceSource.getAuxData(n, auxDic)

    def findMappedTrace(self, n):
        return self.traceSource.findMappedTrace(n)

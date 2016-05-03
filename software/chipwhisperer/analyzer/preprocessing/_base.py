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
from chipwhisperer.common.utils.pluginmanager import Plugin
from chipwhisperer.common.utils.tracesource import TraceSource, ActiveTraceObserver


class PreprocessingBase(TraceSource, ActiveTraceObserver, AutoScript, Plugin):
    """
    Base Class for all preprocessing modules
    Derivate Classes work like this:
        - updateScript is used by the GUI to create the parameters list and generate the API scripts
        - the other methods are used by the API to apply the preprocessing filtering
          You need to pass the traceSource reference in the constructor in order to apply the preprocessing step
    """
    name = "None"

    def __init__(self, parentParam=None, traceSource=None):
        self.enabled = True
        ActiveTraceObserver.__init__(self, parentParam=parentParam)
        TraceSource.__init__(self)
        AutoScript.__init__(self)
        self.setTraceSource(traceSource)
        if traceSource:
            traceSource.sigTracesChanged.connect(self.sigTracesChanged.emit)  # Forwards the traceChanged signal to the next observer in the chain
        self.params.addChildren([
                 {'name':'Enabled', 'key':'enabled', 'type':'bool', 'value':self.enabled, 'set':self.setEnabled}
        ])
        self.findParam('input').hide()

    def updateScript(self, ignored=None):
        pass

    def setEnabled(self, enabled):
        """Turn on/off this preprocessing"""
        self.enabled = enabled
        self.updateScript()

    def getTrace(self, n):
        """Get trace number n"""
        if self.enabled:
            trace = self._traceSource.getTrace(n)
            # Do your preprocessing here
            return trace
        else:
            return self._traceSource.getTrace(n)

    def getTextin(self, n):
        """Get text-in number n"""
        return self._traceSource.getTextin(n)

    def getTextout(self, n):
        """Get text-out number n"""
        return self._traceSource.getTextout(n)

    def getKnownKey(self, n=None):
        """Get known-key number n"""
        return self._traceSource.getKnownKey(n)

    def init(self):
        """Do any initilization required once all traces are loaded"""
        pass

    def getSegmentList(self):
        return self._traceSource.getSegmentList()

    def getAuxData(self, n, auxDic):
        return self._traceSource.getAuxData(n, auxDic)

    def findMappedTrace(self, n):
        return self._traceSource.findMappedTrace(n)

    def numTraces(self):
        return self._traceSource.numTraces()

    def numPoints(self):
        return self._traceSource.numPoints()
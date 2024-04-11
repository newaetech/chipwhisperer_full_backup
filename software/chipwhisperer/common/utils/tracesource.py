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
import logging
import uuid
from chipwhisperer.common.utils import util
from chipwhisperer.common.utils.parameter import Parameterized, setupSetParam
from typing import Dict

class TraceSource(object):
    """
    It has traces as output
    Keeps a dictionary with all the registered objets and emits a signal when a new one is added
    """
    registeredObjects : Dict[str, object] = {}
    registeredObjects["None"] = None
    sigRegisteredObjectsChanged = util.Signal()

    def __init__(self, name="Unknown"):
        self.sigTracesChanged = util.Signal()
        self.name = name
        self.uuid = str(uuid.uuid4())

    def getTrace(self, n):
        """Return the trace with number n in the current TraceSource object"""
        return None

    def numPoints(self):
        return 0

    def numTraces(self):
        return 0

    def offset(self):
        return 0

    def getSampleRate(self):
        """Return the Sample Rate used to generate the traces"""
        return 0

    def getTextin(self, n):
        """Get text-in number n"""
        raise NotImplementedError

    def getTextout(self, n):
        """Get text-out number n"""
        raise NotImplementedError

    def getKnownKey(self, n=None):
        """Get known-key number n"""
        raise NotImplementedError

    def getSegmentList(self):
        """Return a list of segments."""
        raise NotImplementedError

    def getAuxData(self, n, auxDic):
        """Return data about a segment"""
        raise NotImplementedError

    def getSegment(self, n):
        """Return the trace segment with the specified trace in the list with all enabled segments."""
        raise NotImplementedError

    def register(self):
        """Register the current TraceSource object in a list and emit a signal to inform it was updated"""
        self.registeredObjects[self.name] = self
        self.sigRegisteredObjectsChanged.emit()
        return self

    def deregister(self):
        """Deregister the current TraceSource and emit a signal to inform this event"""
        try:
            #Only deregister if UUID matches (NB: don't use direct comparison in case later we adjust repr() )
            if TraceSource.registeredObjects[self.name].uuid == self.uuid:
                if TraceSource.registeredObjects.pop(self.name, None):
                    TraceSource.sigRegisteredObjectsChanged.emit()
        except KeyError:
            pass

    @classmethod
    def deregisterObject(cls, name, uuid = None):
        """Deregister the TraceSource and emit a signal to inform this event"""
        try:
            if (uuid is None) or (cls.registeredObjects[name].uuid == uuid):
                if cls.registeredObjects.pop(name, None):
                    cls.sigRegisteredObjectsChanged.emit()
        except KeyError:
            pass

class PassiveTraceObserver(Parameterized):
    """Processes data from a TraceSource when requested """

    def __init__(self):
        self._traceSource = None
        self.getParams().addChildren([
            {'name':'Input', 'key':'input', 'type':'list', 'values':TraceSource.registeredObjects, 'default':None, 'get':self.getTraceSource, 'set':self.setTraceSource}
        ])

    @setupSetParam('Input')
    def set_trace_source(self, traceSource):
        if self._traceSource:
            self._traceSource.sigTracesChanged.disconnect(self.tracesUpdated)
        if traceSource:
            traceSource.sigTracesChanged.connect(self.tracesUpdated)
        self._traceSource = traceSource
        self.tracesUpdated()

    setTraceSource = set_trace_source

    def get_trace_source(self):
        return self._traceSource

    getTraceSource = get_trace_source

    def tracesUpdated(self):
        """Trace source was updated. Time to do something about"""
        pass

    def processTraces(self):
        """Process the Traces acording to its current state"""
        pass

    def traceSourcesChanged(self):
        """Update the Input parameter values (in the combobox).
        Usually called when TraceSource.sigRegisteredObjectsChanged emits a Signal. Should be connected manually though.
         """
        par = self.findParam('input')
        par.setLimits({})  # Will not update if the obj is the same :(
        par.setLimits(TraceSource.registeredObjects)
        if par.getValue() not in list(TraceSource.registeredObjects.values()):
            par.setValue(None)


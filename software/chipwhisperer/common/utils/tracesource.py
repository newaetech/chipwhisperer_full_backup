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

from chipwhisperer.common.utils import util


class TraceSource(object):
    """ It has traces as output """

    def __init__(self):
        self.sigTracesChanged = util.Signal()

    def getTrace(self, n):
        return None

    def numPoints(self):
        return 0

    def numTraces(self):
        return 0


class PassiveTraceObserver(object):
    """ It process data from a TraceSource when requested """

    def __init__(self, traceSource=None):
        self.setTraceSource(traceSource)

    def setTraceSource(self, traceSource):
        self._traceSource = traceSource

    def traceSource(self):
        return self._traceSource

    def processTraces(self):
        pass


class ActiveTraceObserver(PassiveTraceObserver):
    """ It observes a TraceSource for state changes and process the Traces actively """

    def setTraceSource(self, traceSource):
        if traceSource:
            traceSource.sigTracesChanged.connect(self.processTraces)
        self._traceSource = traceSource
        self.processTraces()
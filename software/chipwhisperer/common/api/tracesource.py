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

    def __init__(self):
        self.tracesChanged = util.Signal()

    def getTrace(self, n):
        return None

    def numPoint(self):
        return 0

    def numTrace(self):
        return 0


class TraceObserver(object):

    def __init__(self):
        self.traceSource = None

    def setObservedTraceSource(self, traceSource):
        if traceSource:
            traceSource.tracesChanged.connect(self.processTraces)
        self.traceSource = traceSource
        self.processTraces()

    def processTraces(self):
        pass
#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2013-2014, NewAE Technology Inc
# All rights reserved.
#
# Authors: Colin O'Flynn
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

from chipwhisperer.common.utils import Util, pluginmanager


class AttackBaseClass(pluginmanager.Plugin):
    """Generic Attack Interface"""

    name = "None"

    def __init__(self):
        self._traceSource = None
        super(AttackBaseClass, self).__init__()
        self.attackStarted = Util.Signal()
        self.statsUpdated = Util.Signal()
        self.attackDone = Util.Signal()

    def processKnownKey(self, inpkey):
        """Passes known first-round key (if available, may pass None). Returns key under attack which should be highlighted in graph"""
        return inpkey

    def doAttack(self):
        self.attackStarted.emit()
        # Do the attack
        self.attackDone.emit()

    def passTrace(self, powertrace, plaintext=None, ciphertext=None, knownkey=None):
        self.statsUpdated.emit()
    
    def getStatistics(self):
        return None

    def setTraceStart(self, tnum):
        self._traceStart = tnum

    def setIterations(self, its):
        self._iterations = its

    def setTracesPerAttack(self, trace):
        self._tracePerAttack = trace

    def setReportingInterval(self, ri):
        self._reportinginterval = ri

    def getTraceStart(self):
        return self._traceStart

    def getTraceNum(self):
        return self._tracePerAttack

    def getIterations(self):
        return self._iterations

    def getReportingInterval(self):
        return self._reportinginterval

    def setPointRange(self, rng):
        self._pointRange = rng

    def getPointRange(self, bnum=None):
        if isinstance(self._pointRange, list) and bnum is not None:
            return self._pointRange[bnum]
        else:
            return self._pointRange

    def setTraceSource(self, traceSource):
        """Set the input trace source"""
        self._traceSource = traceSource

    def traceSource(self):
        return self._traceSource

    def knownKey(self):
        """Get the known key via attack"""
        try:
            return self.processKnownKey(self.traceSource().getKnownKey(self.getTraceStart()))
        except AttributeError as e:
            print "WARNING: Failed to find KnownKey, error = %s" % str(e)

    def getName(self):
        return self.name

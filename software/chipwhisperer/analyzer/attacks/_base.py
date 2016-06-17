#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2013-2014, NewAE Technology Inc
# All rights reserved.
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

from chipwhisperer.common.utils.pluginmanager import Plugin
from chipwhisperer.common.utils.tracesource import PassiveTraceObserver
from chipwhisperer.common.utils.analysissource import AnalysisSource, AnalysisObserver


class AttackBaseClass(PassiveTraceObserver, AnalysisSource, Plugin):
    """Generic Attack Interface"""
    _name = "None"

    def __init__(self):
        AnalysisSource.__init__(self)
        PassiveTraceObserver.__init__(self)
        self.getParams().getChild("Input").hide()

    def processKnownKey(self, inpkey):
        """
        Passes known first-round key (if available, may pass None).
        Returns key under attack which should be highlighted in graph
        """
        return inpkey

    def processTraces(self):
        self.sigAnalysisStarted.emit()
        # Do the attack
        self.sigAnalysisDone.emit()

    def getStatistics(self):
        return None

    def getTraceStart(self):
        return self._traceStart

    def setTraceStart(self, tnum):
        self._traceStart = tnum

    def getIterations(self):
        return self._iterations

    def setIterations(self, its):
        self._iterations = its

    def getTracesPerAttack(self):
        return self._tracePerAttack

    def setTracesPerAttack(self, trace):
        self._tracePerAttack = trace

    def getReportingInterval(self):
        return self._reportinginterval

    def setReportingInterval(self, ri):
        self._reportinginterval = ri

    def getPointRange(self, bnum=None):
        if isinstance(self._pointRange, list) and bnum is not None:
            return self._pointRange[bnum]
        else:
            return self._pointRange

    def setPointRange(self, rng):
        self._pointRange = rng

    def knownKey(self):
        """Get the known key via attack"""
        key = self.processKnownKey(self.getTraceSource().getKnownKey(self.getTraceStart()))
        if key is None:
            key = [None] * len(self.getStatistics().diffs)
        return key

    def setTargetBytes(self, blist):
        self._targetbytes = blist

    def targetBytes(self):
        return self._targetbytes


class AttackObserver(AnalysisObserver):
    """"It is an AnalysisObserver with methods to get information from attacks"""
    highlightedKeyColor = 255, 0, 0
    traceColor = 0, 255, 0

    def setAnalysisSource(self, analysisSource):
        if issubclass(analysisSource.__class__, AttackBaseClass):
            AnalysisObserver.setAnalysisSource(self, analysisSource)
        else:
            AnalysisObserver.setAnalysisSource(self, None)

    def _highlightedKeys(self):
        return self._analysisSource.knownKey()

    def _numPerms(self, key):
        try:
            return len(self._analysisSource.getStatistics().diffs[key])
        except Exception:
            return 0

    def _maxNumPerms(self):
        try:
            return self._analysisSource.getStatistics().numPerms
        except Exception:
            return 0

    def _numKeys(self):
        try:
            return len(self._analysisSource.getStatistics().diffs)
        except Exception:
            return 0

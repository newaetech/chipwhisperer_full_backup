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

from ._stats import Results
from chipwhisperer.common.utils.parameter import Parameterized


class AlgorithmsBase(Parameterized):

    def __init__(self):
        self.sr = None
        self.stats = None
        self._project = None

    def setProject(self, proj):
        self._project = proj

    def project(self):
        return self._project

    def tracesUpdated(self, traceSource):
        pass

    def updateScript(self, ignored=None):
        pass

    def set_target_subkeys(self, brange):
        self.brange = brange

    setTargetSubkeys = set_target_subkeys

    def set_reporting_interval(self, ri):
        self._reportingInterval = ri

    setReportingInterval = set_reporting_interval

    def setModel(self, model):
        self.model = model
        if model:
            self.stats = Results(model.getNumSubKeys(), model.getPermPerSubkey())

    def process_known_key(self, inpkey):
        return self.model.process_known_key(inpkey)

    def get_statistics(self):
        return self.stats

    getStatistics = get_statistics
    def setStatsReadyCallback(self, sr):
        self.sr = sr

    def addTraces(self, traceSource, tracerange, progressBar=None, pointRange=None):
        pass

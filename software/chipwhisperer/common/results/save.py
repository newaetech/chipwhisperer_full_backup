#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2013-2016, NewAE Technology Inc
# All rights reserved.
#
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

import copy
from datetime import datetime
import numpy as np
from chipwhisperer.analyzer.attacks._base import AttackObserver
from .base import ResultsBase
from chipwhisperer.common.utils.pluginmanager import Plugin


class ResultsSave(ResultsBase, AttackObserver, Plugin):
    _name = "Save to Files"
    _description = "Save correlation output to files."

    def __init__(self, parentParam=None, name=None):
        ResultsBase.__init__(self, parentParam, name)
        AttackObserver.__init__(self)
        self._filename = None
        self._enabled = False
        self.dataarray = None

        self.params.addChildren([
            {'name':'Save Raw Results', 'type':'bool', 'value':False, 'set':self.setEnabled}
        ])

    def analysisUpdated(self):
        """Stats have been updated"""
        if self._enabled == False:
            return

        attackStats = self._analysisSource.getStatistics()
        # attackStats.setKnownkey(nk)
        # attackStats.findMaximums(useAbsolute=self.useAbs)

        # attackStats.diffs[i][hypkey]
        # attackStats.diffs_tnum[i]

        if self._filename is None:
            # Generate filename
            self._filename = "tempstats_%s.npy" % datetime.now().strftime('%Y%m%d_%H%M%S')

            # Generate Array
            self.dataarray = []

        # Record max & min, used as we don't know if user wanted absolute mode or not

        tempmin = np.ndarray((self._numKeys(), self._maxNumPerms()))
        tempmax = np.ndarray((self._numKeys(), self._maxNumPerms()))

        for i in range(0, self._numKeys()):
            for j in range(0, self._numPerms(i)):
                tempmax[i][j] = np.nanmax(attackStats.diffs[i][j])
                tempmin[i][j] = np.nanmin(attackStats.diffs[i][j])

        newdata = {"tracecnt":copy.deepcopy(attackStats.diffs_tnum), "diffsmax":tempmax, "diffsmin":tempmin}

        self.dataarray.append(newdata)
        np.save(self._filename, self.dataarray)

    def processAnalysis(self):
        """Attack is done"""
        self._filename = None
        self.dataarray = None

    def setEnabled(self, enabled):
        self._enabled = enabled

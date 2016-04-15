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

import numpy as np
import copy
from datetime import datetime
from _base import ResultsBase


class ResultsSave(ResultsBase):
    """Save Correlation Output to Files"""
    name = "Save to Files"

    def __init__(self):
        super(ResultsSave, self).__init__()
        self._filename = None
        self._enabled = False
        self.dataarray = None

    def setupParameters(self):
        return [{'name':'Save Raw Results', 'type':'bool', 'value':False, 'set':self.setEnabled}]

    def attackStatsUpdated(self):
        """Stats have been updated"""
        if self._enabled == False:
            return

        attackStats = self.attack.getStatistics()
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
        numkeys = len(attackStats.diffs)
        numhyps = len(attackStats.diffs[0])

        tempmin = np.ndarray((numkeys, numhyps))
        tempmax = np.ndarray((numkeys, numhyps))

        for i in range(0, numkeys):
            for j in range(0, numhyps):
                tempmax[i][j] = np.nanmax(attackStats.diffs[i][j])
                tempmin[i][j] = np.nanmin(attackStats.diffs[i][j])

        newdata = {"tracecnt":copy.deepcopy(attackStats.diffs_tnum), "diffsmax":tempmax, "diffsmin":tempmin}

        self.dataarray.append(newdata)
        np.save(self._filename, self.dataarray)

    def attackDone(self):
        """Attack is done"""
        self._filename = None
        self.dataarray = None

    def setEnabled(self, enabled):
        self._enabled = enabled

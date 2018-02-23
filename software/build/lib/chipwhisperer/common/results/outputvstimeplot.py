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

from ._plotdata import AttackResultPlot
from chipwhisperer.common.ui.ProgressBar import ProgressBar
from chipwhisperer.common.utils.pluginmanager import Plugin


class OutputVsTime(AttackResultPlot, Plugin):
    _name = "Output vs Point Plot"
    _description = "Output vs Point Plot Description."

    def __init__(self, name=None):
        AttackResultPlot.__init__(self, name)
        self.setLabels("Attack Output vs. Sample for Subkey Guesses", "Sample Number", "Attack Output")

    def getPrange(self, bnum, diffs):
        """Get a list of all points for a given byte number statistic"""

        prange = self._analysisSource.getPointRange(bnum)
        prange = list(prange)

        if len(diffs[0]) == 1:
            prange[0] = prange[0] + bnum

        # Certain attack types (e.g. template) don't generate an output
        # for each point value
        if (prange[1] - prange[0]) != len(diffs[0]):
            prange[1] = prange[0] + len(diffs[0])

        return range(prange[0], prange[1])

    def redrawPlot(self):
        progress = ProgressBar("Redrawing " + OutputVsTime._name, "Status:")

        with progress:
            data = self._analysisSource.getStatistics().diffs

            enabledlist = []
            for bnum in range(0, len(self.enabledbytes)):
                if self.enabledbytes[bnum]:
                    enabledlist.append(bnum)

            xrangelist = [0] * self._numKeys()
            for bnum in enabledlist:
                diffs = data[bnum]

                if diffs is not None:
                    if not hasattr(diffs[0], '__iter__'):
                        diffs = [[t] for t in diffs]

                    prange = self.getPrange(bnum, diffs)
                    xrangelist[bnum] = prange

            self.drawData(progress, xrangelist, data, enabledlist)

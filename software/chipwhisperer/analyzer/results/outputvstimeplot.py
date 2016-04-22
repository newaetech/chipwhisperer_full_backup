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

from _plotdata import ResultsPlotData


class OutputVsTime(ResultsPlotData):
    """
    Generic data plotting stuff. Adds ability to highlight certain guesses, used in plotting for example the
    correlation over all data points, or the most likely correlation over number of traces
    """
    name = "Output vs Point Plot"

    def __init__(self, parentParam=None, subkeys=16, permPerSubkey=256):
        super(OutputVsTime, self).__init__(parentParam)

        self.setLabels("Attack Output vs. Sample for Subkey Guesses", "Sample Number", "Attack Output")
        self.numKeys = subkeys
        self.numPerms = permPerSubkey

    def getPrange(self, bnum, diffs):
        """Get a list of all points for a given byte number statistic"""

        prange = self.attack.getPointRange(bnum)
        prange = list(prange)

        if len(diffs[0]) == 1:
            prange[0] = prange[0] + bnum

        # Certain attack types (e.g. template) don't generate an output
        # for each point value
        if (prange[1] - prange[0]) != len(diffs[0]):
            prange[1] = prange[0] + len(diffs[0])

        prange = range(prange[0], prange[1])

        return prange

    def redrawPlot(self):

        data = self.attack.getStatistics().diffs

        enabledlist = []
        for i in range(0, self.numKeys):
            if self.enabledbytes[i]:
                enabledlist.append(i)

        xrangelist = [0] * self.numKeys
        for bnum in enabledlist:
            diffs = data[bnum]

            if not hasattr(diffs[0], '__iter__'):
                diffs = [[t] for t in diffs]

            prange = self.getPrange(bnum, diffs)
            xrangelist[bnum] = prange

        self.drawData(xrangelist, data, enabledlist)

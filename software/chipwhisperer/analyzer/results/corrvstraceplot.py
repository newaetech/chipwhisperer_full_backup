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
import numpy as np


class CorrelationVsTrace(ResultsPlotData):
    """
    Plots maximum correlation vs number of traces in attack.
    """
    name = 'Correlation vs Traces in Attack'

    def __init__(self, parentParam=None, subkeys=16, permPerSubkey=256):
        ResultsPlotData.__init__(self, parentParam)
        self.setLabels(self.name, "Traces", self.name)
        self.numKeys = subkeys
        self.numPerms = permPerSubkey

    def redrawPlot(self):
        """Redraw the plot, loading data from attack"""
        if not self.attack:
            return

        data = self.attack.getStatistics().maxes_list

        enabledlist = []
        for i in range(0, self.numKeys):
            if self.enabledbytes[i]:
                enabledlist.append(i)

        xrangelist = [0] * self.numKeys
        newdata = [0] * self.numKeys
        for bnum in enabledlist:
            maxdata = data[bnum]
            tlist = []
            for m in maxdata:
                tlist.append(m['trace'])

            maxlist = np.zeros((self.numPerms, len(tlist)))
            for i, m in enumerate(maxdata):
                for j in range(0, self.numPerms):
                    maxlist[m['maxes'][j][0], i] = m['maxes'][j][2]

            newdata[bnum] = maxlist
            xrangelist[bnum] = tlist

        self.drawData(xrangelist, newdata, enabledlist)

    def attackDone(self):
        self.redrawPlot()

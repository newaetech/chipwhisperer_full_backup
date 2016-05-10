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
import numpy as np
from chipwhisperer.common.ui.ProgressBar import ProgressBar
from chipwhisperer.common.utils.pluginmanager import Plugin


class CorrelationVsTrace(AttackResultPlot, Plugin):
    _name = 'Correlation vs Traces in Attack'
    _description = "Plots maximum correlation vs number of traces in attack."

    def __init__(self, parentParam=None, name=None):
        AttackResultPlot.__init__(self, parentParam, name)
        self.setLabels(self.getName(), "Traces", self.getName())

    def redrawPlot(self):
        """Redraw the plot, loading data from attack"""
        if not self._analysisSource:
            return

        progress = ProgressBar("Redrawing " + CorrelationVsTrace._name, "Status:")

        with progress:
            data = self._analysisSource.getStatistics().maxes_list

            enabledlist = []
            for bnum in range(0, len(self.enabledbytes)):
                if self.enabledbytes[bnum]:
                    enabledlist.append(bnum)

            xrangelist = [0] * self._numKeys()
            newdata = [0] * self._numKeys()
            for bnum in enabledlist:
                maxdata = data[bnum]
                tlist = []
                for m in maxdata:
                    tlist.append(m['trace'])

                maxlist = np.zeros((self._numPerms(bnum), len(tlist)))
                for i, m in enumerate(maxdata):
                    for j in range(0, self._numPerms(bnum)):
                        maxlist[m['maxes'][j][0], i] = m['maxes'][j][2]

                newdata[bnum] = maxlist
                xrangelist[bnum] = tlist

            self.drawData(progress, xrangelist, newdata, enabledlist)
            self.pw.setYRange(0, 1, update=True)

    def processAnalysis(self):
        self.redrawPlot()

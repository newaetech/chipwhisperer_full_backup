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
from PySide.QtGui import *
from chipwhisperer.common.utils import util
from chipwhisperer.common.utils.pluginmanager import Plugin

class PGEVsTrace(AttackResultPlot, Plugin):
    _name = "PGE vs Trace Plot"
    _description = "Plots Partial Guessing Entropy (PGE) vs Traces in Attack"

    def __init__(self, parentParam=None, name=None):
        AttackResultPlot.__init__(self, parentParam, name)
        self.setLabels("Partial Guessing Entropy vs. Traces", "Traces", "Partial Guessing Entropy")

        self.params.addChildren([
            {'name':'Copy PGE Data to Clipboard', 'type':'action', 'action':self.copyPGE},
            {'name':'Clipboard Format', 'key':'fmt', 'type':'list', 'values':['CSV', 'MATLAB'], 'value':'CSV'}
        ])

    def copyPGE(self, dontCopy=False, addPlotMatlab=True):
        """Copy the Partial Guessing Entropy (PGE) to clipboard for use in other programs"""
        allpge = self.calculatePGE()
        cb = QClipboard()

        fmt = self.findParam('fmt').value()

        if fmt == 'CSV':
            spge = "Trace Number, "
            for i in range(0, self._numKeys()):
                spge += "Subkey %d, "%i
            spge += "\n"
            for (tnum, plist) in allpge.iteritems():
                spge += "%d, "%tnum
                for j in plist:
                    if j['trials'] > 0:
                        spge += "%f, "%j['pge']
                    else:
                        spge += "NaN, "
                spge += "\n"
        elif fmt == 'MATLAB':
            tracestr = "tnum = ["
            spge = "pge = ["
            trials = 0
            for (tnum, plist) in allpge.iteritems():
                tracestr += "%d "%tnum
                for j in plist:
                    if j['trials'] > 0:
                        spge += "%f "%j['pge']
                        trials = max(trials, j['trials'])
                    else:
                        spge += "NaN, "
                spge += ";\n"
            tracestr += "];\n"
            spge += "];\n"
            spge += tracestr
            spge += "\n"
            if addPlotMatlab:
                spge += "plot(tnum, pge)\n"
                spge += "xlabel('Trace Number')\n"
                spge += "ylabel('Average PGE (%d Trials)')\n" % trials
                spge += "title('Average Partial Guessing Entropy (PGE) via ChipWhisperer')\n"
                spge += "legend("
                for k in range(0, self._numKeys()):
                    spge += "'Subkey %d'"%k
                    if k != (self._numKeys()-1):
                        spge += ", "
                spge += ")\n"
        else:
            raise ValueError("Invalid fmt: %s"%fmt)

        if dontCopy is False:
            cb.setText(spge)
        return spge

    def calculatePGE(self):
        """Calculate the Partial Guessing Entropy (PGE)"""
        if not self._analysisSource:
            raise Warning("Attack not set/executed yet")

        stats = self._analysisSource.getStatistics()
        pge = stats.pge_total
        allpge = util.DictType()

        for i in pge:
            tnum = i['trace']
            if not tnum in allpge:
                allpge[tnum] = [{'pgesum':0, 'trials':0} for z in range(0,stats.numSubkeys)]

            allpge[tnum][i['subkey']]['pgesum'] += i['pge']
            allpge[tnum][i['subkey']]['trials'] += 1

        for (tnum, plist) in allpge.iteritems():
            for j in plist:
                if j['trials'] > 0:
                    j['pge'] = float(j['pgesum']) / float(j['trials'])
                    # print "%d "%j['trials'],
                else:
                    j['pge'] = None

        #print ""

        return allpge

    def redrawPlot(self):
        """Recalculate the PGE and redraw the PGE plot"""
        allpge = self.calculatePGE()

        self.clearPushed()
        self.setupHighlights()
        #prange = range(self.pstart[bnum], self.pend[bnum])

        try:
            for bnum in range(0, len(self.enabledbytes)):
                if self.enabledbytes[bnum]:
                    trace = []
                    pge = []
                    for (tnum, plist) in allpge.iteritems():
                        if plist[bnum]['pge'] is not None:
                            trace.append(tnum)
                            pge.append(plist[bnum]['pge'])
                    p = self.pw.plot(trace, pge, pen='r')
                    p.curve.setClickable(True)
                    p.id = str(bnum)
                    p.sigClicked.connect(self.selectTrace)
        except StopIteration:
            pass

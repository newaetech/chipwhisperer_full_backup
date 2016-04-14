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
from PySide.QtGui import *
from chipwhisperer.common.utils import Util


def getClass():
    """"Returns the Main Class in this Module"""
    return PGEVsTrace


class PGEVsTrace(ResultsPlotData):
    """
    Plots Partial Guessing Entropy (PGE) vs Traces in Attack
    """

    name = "PGE vs Trace Plot"

    def __init__(self, subkeys=16, permPerSubkey=256):
        super(PGEVsTrace, self).__init__()
        self.setLabels("Partial Guessing Entropy vs. Traces", "Traces", "Partial Guessing Entropy")
        self.numKeys = subkeys
        self.numPerms = permPerSubkey

    def setupParameters(self):
        return [{'name':'Show', 'type':'bool', 'key':'show', 'value':False, 'set':self.visibilityChanged.emit},
                         {'name':'Copy PGE Data to Clipboard', 'type':'action', 'action':self.copyPGE},
                         {'name':'Clipboard Format', 'key':'fmt', 'type':'list', 'values':['CSV', 'MATLAB'], 'value':'CSV'},
                      ]

    def copyPGE(self, dontCopy=False, addPlotMatlab=True):
        """Copy the Partial Guessing Entropy (PGE) to clipboard for use in other programs"""
        allpge = self.calculatePGE()
        cb = QClipboard()

        fmt = self.findParam('fmt').value()

        if fmt == 'CSV':
            spge = "Trace Number, "
            for i in range(0,self.numKeys):
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
                for k in range(0, self.numKeys):
                    spge += "'Subkey %d'"%k
                    if k != (self.numKeys-1):
                        spge += ", "
                spge += ")\n"
        else:
            raise ValueError("Invalid fmt: %s"%fmt)

        if dontCopy is False:
            cb.setText(spge)
        return spge

    def calculatePGE(self):
        """Calculate the Partial Guessing Entropy (PGE)"""
        if not self.attack:
            raise Warning("Attack not set/executed yet")

        stats = self.attack.getStatistics()
        pge = stats.pge_total
        allpge = Util.OrderedDict()

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
            for bnum in range(0, self.numKeys):
                if self.enabledbytes[bnum]:
                    trace = []
                    pge = []
                    for (tnum, plist) in allpge.iteritems():
                        trace.append(tnum)
                        pge.append(plist[bnum]['pge'])
                    self.pw.plot(trace, pge, pen='r')

                # penclr = self._highlightColour( self.highlights[bnum].index(i) )
                #                   self.pw.plot(prange, diffs[i], pen=penclr )

                #if self.highlightTop:
                #    #Plot the highlighted byte(s) on top
                #    for bnum in range(0, self.numKeys):
                #        prange = self.attack.getPointRange(bnum)
                #        prange = range(prange[0], prange[1])
        except StopIteration:
            pass

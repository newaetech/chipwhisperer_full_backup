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

from functools import partial
import numpy as np
from PySide.QtGui import *
from chipwhisperer.analyzer.attacks._base import AttackObserver
from .base import ResultsBase
from chipwhisperer.common.ui.GraphWidget import GraphWidget
from chipwhisperer.common.utils.timer import Timer


class AttackResultPlot(GraphWidget, ResultsBase, AttackObserver):
    """
    Generic data plotting stuff. Adds ability to highlight certain guesses, used in plotting for example the
    correlation over all data points, or the most likely correlation over number of traces
    """

    def __init__(self, parentParam=None, name=None):
        GraphWidget.__init__(self)
        ResultsBase.__init__(self, parentParam, name)

        self.params.addChildren([
            {'name':'Draw Type', 'type':'list', 'key':'drawtype', 'values':['Fastest', 'Normal', 'Detailed'], 'value':'Normal'},
        ])

        self.setObjectName(self.getName())
        self.bselection = QToolBar()
        self.layout().addWidget(self.bselection)
        self.highlightTop = True
        self.doRedraw = True
        self.enabledbytes = []
        AttackObserver.__init__(self)
        self.initUI(True)

        # Setup the redrawPlot() to be delayed when pressing the Key buttons
        self.delayedRedrawPlot = Timer()
        self.delayedRedrawPlot.timeout.connect(self.redrawPlot)
        self.delayedRedrawPlot.setSingleShot(True)
        self.delayedRedrawPlot.setInterval(1000)

    def initUI(self, firstTime=False):
        if firstTime or self._numKeys() != len(self.enabledbytes):
            self.enabledbytes = [False]*self._numKeys()
            self.bselection.clear()
            self.byteNumAct=[]
            if self._analysisSource:
                for i in range(0, self._numKeys()):
                    newAct = QAction('%d' % i, self)
                    newAct.triggered[bool].connect(partial(self.setBytePlot, i))
                    newAct.setCheckable(True)
                    self.bselection.addAction(newAct)
                    self.byteNumAct.append(newAct)

            byteNumAllOn = QAction('All On', self)
            byteNumAllOn.triggered.connect(partial(self.setByteAll, False))
            self.bselection.addAction(byteNumAllOn)

            byteNumAllOff = QAction('All Off', self)
            byteNumAllOff.triggered.connect(partial(self.setByteAll, True))
            self.bselection.addAction(byteNumAllOff)

    def analysisStarted(self):
        self.initUI()
        for i in range(0, self._numKeys()):
            if i in self._analysisSource.targetBytes():
                self.byteNumAct[i].setVisible(True)
            else:
                self.byteNumAct[i].setVisible(False)

    def setBytePlot(self, num, sel):
        """Set which bytes to plot"""
        self.enabledbytes[num] = sel
        if self.doRedraw:
            self.delayedRedrawPlot.start()

    def setByteAll(self, status):
        """Enable all bytes in plot"""
        self.doRedraw = False
        for t in self.byteNumAct:
            t.setChecked(status)
            t.trigger()
        self.doRedraw = True
        self.redrawPlot()

    def setupHighlights(self):
        """Initialize the highlights based on the known key"""
        self.highlights = []

        highlights = self._highlightedKey()

        for i in range(0, self._numKeys()):
            if highlights is not None and i < len(highlights):
                self.highlights.append([highlights[i]])
            else:
                self.highlights.append([None])

    def _highlightColour(self, index):
        if index == 0:
            return 'r'
        else:
            return 'b'

    # def backgroundplot(self, prange, data, pen=None, highres=False):
    #    datalen =  max(prange)-min(prange)+1
    #    if data is None:
    #        # Setup call
    #        if highres is False:
    #            if pen is None:
    #                #No pen specified - init call
    #                self.backgroundplotMax = np.empty((datalen,1))
    #                self.backgroundplotMax[:] = np.NAN
    #                self.backgroundplotMin = np.empty((datalen,1))
    #                self.backgroundplotMin[:] = np.NAN
    #            else:
    #                print "Plotting"
    #                self.pw.plot(prange, self.backgroundplotMax, pen)
    #                self.pw.plot(prange, self.backgroundplotMin, pen)
    #
    #    else:
    #        #Store min/max
    #        self.backgroundplotMax = np.fmax(self.backgroundplotMax, data)
    #        self.backgroundplotMin = np.fmin(self.backgroundplotMin, data)

    def redrawPlot(self):
        pass

    def drawData(self, progress, xdatalst, ydatalst, enabledBytes=None):
        """Redraw the plot"""

        progress.setMaximum(len(enabledBytes))
        progress.setStatusMask("Clearing previous plotting...")
        self.clearPushed()
        progress.setStatusMask("Plotting...")

        self.setupHighlights()
        drawtype = self.findParam('drawtype').value().lower()
        pvalue = 0

        for bnum in enabledBytes:
            if not xdatalst[bnum] or len(xdatalst[bnum])==0:
                break

            if bnum != -1:
                ydataptr = ydatalst[bnum]
                xdataptr = xdatalst[bnum]

            else:
                ydataptr = ydatalst
                xdataptr = xdatalst

            pointargsg = {}

            if len(ydataptr) == 0 or ydataptr[0] is None:
                continue

            if not hasattr(ydataptr[0], '__iter__'):
                ydataptr = [[t] for t in ydataptr]
                pointargsg = {'symbol':'t', 'symbolPen':'b', 'symbolBrush':'g'}

            if drawtype.startswith('fast'):
                if self.highlightTop:
                    newdiff = np.array(ydataptr)
                    for j in self.highlights[bnum]:
                        newdiff[j] = 0
                else:
                    newdiff = ydataptr

                maxlimit = np.amax(newdiff, 0)
                minlimit = np.amin(newdiff, 0)
                self.pw.plot(xdataptr, maxlimit, pen='g', fillLevel=0.0, brush='g', **pointargsg)
                if len(pointargsg) > 0:
                    pointargsg["symbol"] = 'd'
                self.pw.plot(xdataptr, minlimit, pen='g', fillLevel=0.0, brush='g', **pointargsg)

            elif drawtype.startswith('norm'):
                tlisttst = []
                maxlisttst = []
                if len(pointargsg) == 0:
                    tlist_fixed = [xdataptr[-1], xdataptr[0]]
                    tlist_fixed[:0] = xdataptr
                else:
                    tlist_fixed = xdataptr
                for i in range(0, self._numPerms(bnum)):

                    if self.highlightTop and i in self.highlights[bnum]:
                        continue

                    tlisttst.extend(tlist_fixed)
                    if len(pointargsg) == 0:
                        newmax = [0, 0]
                        newmax[:0] = ydataptr[i]
                    else:
                        newmax = ydataptr[i]
                    maxlisttst.extend(newmax)

                self.pw.plot(tlisttst, maxlisttst, pen='g', **pointargsg)

            elif drawtype.startswith('detail'):
                for i in range(0, self._numPerms(bnum)):
                    self.pw.plot(xdataptr, ydataptr[i], pen='g', **pointargsg)
                    if progress.wasAborted():
                        break

            if self.highlightTop:
                # Plot the highlighted byte(s) on top
                for bnum in enabledBytes:

                    if bnum != -1:
                        ydataptr = ydatalst[bnum]
                        xdataptr = xdatalst[bnum]

                    else:
                        ydataptr = ydatalst
                        xdataptr = xdatalst

                    pointargsr = {}

                    if not hasattr(ydataptr[0], '__iter__'):
                        ydataptr = [[t] for t in ydataptr]
                        pointargsr = {'symbol':'o', 'symbolPen':'b', 'symbolBrush':'r'}

                    for i in range(0, self._numPerms(bnum)):
                        if i in self.highlights[bnum]:
                            penclr = self._highlightColour(self.highlights[bnum].index(i))
                            self.pw.plot(xdataptr, ydataptr[i], pen=penclr, **pointargsr)

                    pvalue += 1
            progress.updateStatus(pvalue)
            if progress.wasAborted():
                break

    def processAnalysis(self):
        self.redrawPlot()

    def getWidget(self):
        return self
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

from PySide.QtCore import *
from PySide.QtGui import *
from chipwhisperer.common.ui.GraphWidget import GraphWidget
from chipwhisperer.common.outputwidgets._base import ResultsWidgetBase
from functools import partial
import numpy as np


class ResultsPlotData(GraphWidget, ResultsWidgetBase):
    """
    Generic data plotting stuff. Adds ability to highlight certain guesses, used in plotting for example the
    correlation over all data points, or the most likely correlation over number of traces
    """
    name = "Some Descriptive Name"

    def __init__(self, parentParam=None, subkeys=16, permPerSubkey=256):
        GraphWidget.__init__(self)
        ResultsWidgetBase.__init__(self, parentParam)

        self.setObjectName(self.name)
        self.numKeys = subkeys
        self.numPerms = permPerSubkey
        self.enabledbytes = [False]*subkeys
        self.doRedraw = True

        self.bselection = QToolBar()

        self.byteNumAct = []
        for i in range(0,self.numKeys):
            self.byteNumAct.append(QAction('%d'%i, self))
            self.byteNumAct[i].triggered[bool].connect(partial(self.setBytePlot, i))
            self.byteNumAct[i].setCheckable(True)
            self.bselection.addAction(self.byteNumAct[i])

        byteNumAllOn = QAction('All On', self)
        byteNumAllOn.triggered.connect(partial(self.setByteAll, False))
        self.bselection.addAction(byteNumAllOn)

        byteNumAllOff = QAction('All Off', self)
        byteNumAllOff.triggered.connect(partial(self.setByteAll, True))
        self.bselection.addAction(byteNumAllOff)

        showGrid = QAction('Show Grid', self)
        showGrid.setCheckable(True)
        showGrid.triggered.connect(lambda: self.pw.showGrid(showGrid.isChecked(), showGrid.isChecked(), 0.1))
        self.bselection.addAction(showGrid)

        self.layout().addWidget(self.bselection)
        self.highlightTop = True

    def _setupParameters(self):
        return [{'name':'Draw Type', 'type':'list', 'key':'drawtype', 'values':['Fastest', 'Normal', 'Detailed'], 'value':'Normal'},
                {'name':'Hide During Redraw', 'type':'bool', 'key':'hide', 'value':True},
                # {'name':''}
                ]

    def setBytePlot(self, num, sel):
        """Set which bytes to plot"""
        self.enabledbytes[num] = sel
        if self.doRedraw:
            self.redrawPlot()

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

        highlights = self.highlightedKey()

        for i in range(0, self.numKeys):
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

    def drawData(self, xdatalst, ydatalst, enabledBytes=[-1]):
        """Redraw the plot"""

        # Do Redraw
        progress = QProgressDialog("Redrawing", "Abort", 0, 100)
        progress.setWindowModality(Qt.WindowModal)
        progress.setMinimumDuration(1000)  # _callSync='off'
        progress.setMinimum(0)  # _callSync='off'
        progress.setMaximum(len(enabledBytes) * self.numPerms)  # _callSync='off'
        progress.show()

        self.clearPushed()
        self.setupHighlights()

        drawtype = self.findParam('drawtype').value().lower()

        pvalue = 0

        if self.findParam('hide').value():
            self.pw.setVisible(False)

        try:
            for bnum in enabledBytes:
                if not xdatalst[bnum] or len(xdatalst[bnum])==0:
                    raise StopIteration

                progress.setValue(pvalue)

                if bnum != -1:
                    ydataptr = ydatalst[bnum]
                    xdataptr = xdatalst[bnum]

                else:
                    ydataptr = ydatalst
                    xdataptr = xdatalst

                pointargsg = {}

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
                    pvalue += self.numPerms
                    progress.setValue(pvalue)

                elif drawtype.startswith('norm'):
                    tlisttst = []
                    maxlisttst = []
                    if len(pointargsg) == 0:
                        tlist_fixed = [xdataptr[-1], xdataptr[0]]
                        tlist_fixed[:0] = xdataptr
                    else:
                        tlist_fixed = xdataptr
                    for i in range(0, self.numPerms):

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
                    pvalue += self.numPerms
                    progress.setValue(pvalue)

                elif drawtype.startswith('detail'):
                    for i in range(0, self.numPerms):
                        self.pw.plot(xdataptr, ydataptr[i], pen='g', **pointargsg)
                        pvalue += 1
                        progress.setValue(pvalue)

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

                        for i in range(0, self.numPerms):
                            if i in self.highlights[bnum]:
                                penclr = self._highlightColour(self.highlights[bnum].index(i))
                                self.pw.plot(xdataptr, ydataptr[i], pen=penclr, **pointargsr)
        except StopIteration:
            pass

        self.pw.setVisible(True)

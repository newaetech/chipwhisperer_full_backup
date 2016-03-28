#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2013-2014, NewAE Technology Inc
# All rights reserved.
#
# Authors: Colin O'Flynn
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

from collections import OrderedDict
from datetime import datetime
from functools import partial
import copy
import numpy as np

from PySide.QtCore import *
from PySide.QtGui import *
from chipwhisperer.common.ui.GraphWidget import GraphWidget
from chipwhisperer.common.utils.util import hexstr2list
from chipwhisperer.common.api.config_parameter import ConfigParameter

class ResultsPlotting(QObject):
    paramListUpdated = Signal(list)

    """Interface to main program, various routines for plotting output data"""
    def __init__(self):
        super(ResultsPlotting, self).__init__()

        self.override = None

        #ResultsTable manages class
        self.table = ResultsTable()
        self.table.setKeyOverride(self.processKnownKey)

        self.graphoutput = OutputVsTime()
        self.GraphOutputDock = QDockWidget(self.graphoutput.name)
        self.GraphOutputDock.setObjectName(self.graphoutput.name)
        self.GraphOutputDock.setAllowedAreas(Qt.BottomDockWidgetArea | Qt.RightDockWidgetArea| Qt.LeftDockWidgetArea)
        self.GraphOutputDock.setWidget(self.graphoutput)
        self.graphoutput.setDock(self.GraphOutputDock)
        self.graphoutput.setKeyOverride(self.processKnownKey)

        self.pgegraph = PGEVsTrace()
        self.PGEGraphDock = QDockWidget(self.pgegraph.name)
        self.PGEGraphDock.setObjectName(self.pgegraph.name)
        self.PGEGraphDock.setAllowedAreas(Qt.BottomDockWidgetArea | Qt.RightDockWidgetArea| Qt.LeftDockWidgetArea)
        self.PGEGraphDock.setWidget(self.pgegraph)
        self.pgegraph.setDock(self.PGEGraphDock)
        self.pgegraph.setKeyOverride(self.processKnownKey)
        
        self.corrgraph = CorrelationVsTrace()
        self.CorrelationOutputDock = QDockWidget(self.corrgraph.name)
        self.CorrelationOutputDock.setObjectName(self.corrgraph.name)
        self.CorrelationOutputDock.setAllowedAreas(Qt.BottomDockWidgetArea | Qt.RightDockWidgetArea| Qt.LeftDockWidgetArea)
        self.CorrelationOutputDock.setWidget(self.corrgraph)
        self.corrgraph.setDock(self.CorrelationOutputDock)
        self.corrgraph.setKeyOverride(self.processKnownKey)

        self.saveresults = ResultsSave()

        resultsParams = [{'name':'Knownkey Source', 'type':'list', 'values':{'Attack Module':'attack', 'GUI Override':'gui'},
                                                    'value':'attack', 'set':self.setKnownKeySrc},

                         {'name':'Override Key', 'type':'str', 'key':'knownkey', 'value':'', 'set':self.setKnownKeyStr, 'readonly':True},
                         {'name':'Save Raw Results', 'type':'bool', 'value':False, 'set':self.saveresults.setEnabled}
                      ]
        self.params = ConfigParameter.create_extended(self, name="General Parameters", type='group', children=resultsParams)

    def paramList(self):
        """Returns list of parameters for parameter tree GUI display"""

        p = [self.table.params, self.graphoutput.params, self.pgegraph.params, self.corrgraph.params, self.params]
        return p

    def dockList(self):
        """Return list of docks"""

        return [self.table.ResultsTable, self.GraphOutputDock, self.PGEGraphDock, self.CorrelationOutputDock]

    def setAttack(self, attack):
        """Pass the attack to all plotting devices. They pull stats from the attack directly, and listen to attackDone/statusUpdated signals."""

        self.attack = attack
        self.table.setAttack(attack)
        self.attack.attackDone.connect(self.attackDone)
        self.attack.statsUpdated.connect(self.attackStatsUpdated)
        self.attack.settingsChanged.connect(self.attackSettingsChanged)
        self.graphoutput.setAttack(attack)
        self.pgegraph.setAttack(attack)
        self.corrgraph.setAttack(attack)
        self.saveresults.setAttack(attack)
        self.attackSettingsChanged()

    def attackSettingsChanged(self):
        """Attack settings have changed, so pass required changes to other modules such as plotting"""

        # Update possible settings
        self.table.setAbsoluteMode(self.attack.useAbs)

    def setTraceManager(self, tmanager):
        """Set the trace manager"""

        self.trace = tmanager

    def updateKnownKey(self):
        """Re-Read the Known Key from saved traces"""

        try:
            nk = self.trace.getKnownKey(self.startTrace)
            nk = self.attack.processKnownKey(nk)
            self.graphoutput.setKnownKey(nk)
            self.corrgraph.setKnownKey(nk)
        except AttributeError, e:
            print str(e)

    def attackDone(self):
        """Attack is done, update everything"""

        self.attackStatsUpdated()
        #self.table.setBytesEnabled(self.attack.bytesEnabled())
        self.table.updateTable(attackDone=True)

    def attackStatsUpdated(self):
        """New attack statistics available, replot/redraw graphs"""

        self.startTrace = self.attack.getTraceStart()
        self.table.setBytesEnabled(self.attack.bytesEnabled())
        self.table.setStartTrace(self.startTrace)
        self.table.updateTable()
        self.corrgraph.updateData()
        self.updateKnownKey()

    def setKnownKeySrc(self, keysrc):
        """Set key as 'attack' or 'override'"""
        if keysrc == 'attack':
            self.findParam('knownkey').setReadonly(True)
            self.override = None
        elif keysrc == 'gui':
            self.findParam('knownkey').setReadonly(False)
        else:
            raise ValueError("Key Source Error")

    def setKnownKeyStr(self, strkey):
        """Override known key by user selection"""
        try:
            hexkey = hexstr2list(strkey)
            self.override = hexkey

        except ValueError:
            QMessageBox.warning(None, "Key Selection", "Could not convert '%s' to hex, key unchanged!" % strkey)

    def processKnownKey(self, key):
        """Process known key in case of override by user"""

        if self.override:
            return self.override
        return key


class ResultsPlotData(GraphWidget):
    """
    Generic data plotting stuff. Adds ability to highlight certain guesses, used in plotting for example the
    correlation over all data points, or the most likely correlation over number of traces
    """

    showDockSignal = Signal(bool)
    name = "Some Descriptive Name"

    def __init__(self, subkeys=16, permPerSubkey=256):
        super(ResultsPlotData, self).__init__()

        self.numKeys = subkeys
        self.numPerms = permPerSubkey
        self._knownkey = None
        self.enabledbytes = [False]*subkeys
        self.doRedraw = True
        self.orfunction = None

        self.byteNumAct = []
        for i in range(0,self.numKeys):
            self.byteNumAct.append(QAction('%d'%i, self))
            self.byteNumAct[i].triggered[bool].connect(partial(self.setBytePlot, i))
            self.byteNumAct[i].setCheckable(True)

        byteNumAllOn = QAction('All On', self)
        byteNumAllOff = QAction('All Off', self)
        byteNumAllOn.triggered.connect(partial(self.setByteAll, False))
        byteNumAllOff.triggered.connect(partial(self.setByteAll, True))

        self.bselection = QToolBar()

        for i in range(0, self.numKeys):
            self.bselection.addAction(self.byteNumAct[i])
        self.bselection.addAction(byteNumAllOn)
        self.bselection.addAction(byteNumAllOff)
        self.layout().addWidget(self.bselection)

        self.highlightTop = True

        resultsParams = self.genericParameters()
        self.params = ConfigParameter.create_extended(self, name=self.name, type='group', children=resultsParams)

    def genericParameters(self):
        return [{'name':'Show', 'type':'bool', 'key':'show', 'value':False, 'set':self.showDockSignal.emit},
                {'name':'Draw Type', 'type':'list', 'key':'drawtype', 'values':['Fastest', 'Normal', 'Detailed'], 'value':'Normal'},
                {'name':'Hide During Redraw', 'type':'bool', 'key':'hide', 'value':True},
                # {'name':''}
            ]

    def paramList(self):
        """Returns parameter list"""
        return [self.params]

    def setDock(self, dock):
        """Set the dock holding this widget, used for visibility control"""
        self.dock = dock
        self.showDockSignal.connect(dock.setVisible)
        dock.visibilityChanged.connect(self.visibleChanged)
        self.visibleChanged()

    def visibleChanged(self):
        """Dock visability changed"""
        visible = self.dock.isVisible()
        self.findParam('show').setValue(visible)

    def setKnownKey(self, knownkey):
        """Set the known encryption key (used for highlighting)"""
        self._knownkey = knownkey

    def knownkey(self):
        """Get the known key"""
        if self.orfunction:
            return self.orfunction(self._knownkey)
        else:
            return self._knownkey

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

    def setAttack(self, attack):
        """Set the source of statistics (i.e. the attack)"""
        self.attack = attack

    def setupHighlights(self):
        """Initialize the highlights based on the known key"""
        self.highlights = []

        highlights = self.knownkey()

        for i in range(0, self.numKeys):
            if highlights is not None:
                self.highlights.append([highlights[i]])
            else:
                self.highlights.append([None])

    def setKeyOverride(self, orfunction):
        self.orfunction = orfunction

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

        self.clearPushed()
        self.setupHighlights()

        drawtype = self.findParam('drawtype').value().lower()


        pvalue = 0

        if self.findParam('hide').value():
            self.pw.setVisible(False)

        try:
            for bnum in enabledBytes:
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


class OutputVsTime(ResultsPlotData):
    """
    Generic data plotting stuff. Adds ability to highlight certain guesses, used in plotting for example the
    correlation over all data points, or the most likely correlation over number of traces
    """

    name = "Output vs Point Plot"

    def __init__(self, subkeys=16, permPerSubkey=256):
        super(OutputVsTime, self).__init__()

        self.setLabels("Attack Output vs. Sample for Subkey Guesses", "Sample Number", "Attack Output")

        self.numKeys = subkeys
        self.numPerms = permPerSubkey

        # resultsParams = [{'name':'Show', 'type':'bool', 'key':'show', 'value':False, 'set':self.showDockSignal.emit},
        #                 {'name':'Fast Draw', 'type':'bool', 'key':'fast', 'value':True},
        #                 {'name':'Hide during Redraw', 'type':'bool', 'key':'hide', 'value':True}
        #              ]
        resultsParams = self.genericParameters()
        self.params = ConfigParameter.create_extended(self, name=self.name, type='group', children=resultsParams)

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

        resultsParams = [{'name':'Show', 'type':'bool', 'key':'show', 'value':False, 'set':self.showDockSignal.emit},
                         {'name':'Copy PGE Data to Clipboard', 'type':'action', 'action':self.copyPGE},
                         {'name':'Clipboard Format', 'key':'fmt', 'type':'list', 'values':['CSV', 'MATLAB'], 'value':'CSV'},
                      ]
        self.params = ConfigParameter.create_extended(self, name=self.name, type='group', children=resultsParams)

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

        stats = self.attack.getStatistics()
        pge = stats.pge_total
        allpge = OrderedDict()

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


class CorrelationVsTrace(ResultsPlotData):
    """
    Plots maximum correlation vs number of traces in attack.
    """

    name = "Correlation vs Traces in Attack"

    def __init__(self, subkeys=16, permPerSubkey=256):
        super(CorrelationVsTrace, self).__init__()
        
        self.setLabels(self.name, "Traces", "Max Correlation Output for Subkey")

        self.numKeys = subkeys
        self.numPerms = permPerSubkey

        resultsParams = self.genericParameters()
        self.params = ConfigParameter.create_extended(self, name=self.name, type='group', children=resultsParams)

    def updateData(self):
        pass

    def redrawPlot(self):
        """Redraw the plot, loading data from attack"""

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


class ResultsTable(QObject):
    """Table of results, showing all guesses based on sorting output of attack"""

    def __init__(self, subkeys=16, permPerSubkey=256, useAbs=True):
        super(ResultsTable, self).__init__()

        self.orfunction = None
        self.table = QTableWidget(permPerSubkey+1, subkeys)
        self.table.setSizePolicy(QSizePolicy.MinimumExpanding, QSizePolicy.MinimumExpanding)
        self.pgeBrush = QBrush(QColor(253,255,205))
        pgehdr = QTableWidgetItem("PGE")
        self.table.setVerticalHeaderItem(0,pgehdr)
        for i in range(1,permPerSubkey+1):
            self.table.setVerticalHeaderItem(i, QTableWidgetItem("%d"%(i-1)))

        for i in range(0,subkeys):
            fi = QTableWidgetItem("")
            fi.setBackground(self.pgeBrush)
            self.table.setItem(0,i,fi)

        for i in range(0,subkeys):
            self.table.setHorizontalHeaderItem(i, QTableWidgetItem("%d"%i))

        self.table.resizeColumnsToContents()

        fullTable = QWidget()
        fullLayout = QVBoxLayout()
        fullTable.setLayout(fullLayout)

        fullLayout.addWidget(self.table)

        self.ResultsTable = QDockWidget("Results Table")
        self.ResultsTable.setObjectName("Results Table")
        self.ResultsTable.setAllowedAreas(Qt.BottomDockWidgetArea | Qt.RightDockWidgetArea| Qt.LeftDockWidgetArea)
        self.ResultsTable.setWidget(fullTable)
        self.ResultsTable.setVisible(True)
        self.ResultsTable.visibilityChanged.connect(self.visibleChanged)

        self.numKeys = subkeys
        self.numPerms = permPerSubkey
        self.setBytesEnabled([])
        self.useAbs = useAbs
        self._knownkey = None
        self.useSingle = False

        resultsParams = [
                         {'name':'Show', 'type':'bool', 'key':'show', 'value':False, 'set':self.ResultsTable.setVisible},
                         {'name':'Use Absolute Value for Rank', 'type':'bool', 'value':True, 'set':self.setAbsoluteMode},
                         {'name':'Use single point for Rank', 'type':'bool', 'value':False, 'set':self.setSingleMode},
                         {'name':'Update Mode', 'key':'updateMode', 'type':'list', 'values':{'Entire Table (Slow)':'all', 'PGE Only (faster)':'pge'}, 'set':self.setUpdateMode},
                      ]

        self.params = ConfigParameter.create_extended(self, name='Ranked Table', type='group', children=resultsParams)

        self.updateMode = self.findParam('updateMode').value()

        #Update parameter tree
        self.visibleChanged()

    def paramList(self):
        """Returns list for parameter tree in settings dock"""

        return [self.params]

    def visibleChanged(self):
        """Called when visibility changed, ensures GUI matches real setting"""

        visible = self.ResultsTable.isVisible()
        self.findParam('show').setValue(visible)

    def setUpdateMode(self, mode):
        """Set if we update entire table or just PGE on every statistics update"""

        self.updateMode = mode

    def setBytesEnabled(self, enabledbytes):
        """Set what bytes to include in table"""

        self.enabledBytes = enabledbytes

    def setStartTrace(self, starttrace):
        """Set starting trace number"""

        self.startTrace = starttrace

    def setAttack(self, attack):
        """Set source of statistics (i.e. attack)"""

        self.attack = attack

    def setKnownKey(self, knownkey):
        """Set the known encryption key, required for PGE calculation"""

        self._knownkey = knownkey


    def knownkey(self):
        """Get the known key"""
        if self.orfunction:
            return self.orfunction(self._knownkey)
        else:
            return self._knownkey

    def setKeyOverride(self, orfunc):
        """Set key override function in case we don't want to use one from attack"""

        self.orfunction = orfunc


    def setAbsoluteMode(self, enabled):
        """If absolute mode is enabled, table is sorted based on absolute value of statistic"""

        self.useAbs = enabled

    def setSingleMode(self, enabled):
        """Single mode uses the same point across all traces, not useful normally"""

        self.useSingle = enabled

    def updateTable(self, attackDone=False):
        """Resort data and redraw the table. If update-mode is 'pge' we only redraw entire table
        when 'attackDone' is True."""

        # Process known key via attack
        nk = self.attack.traceSource().getKnownKey(self.startTrace)
        nk = self.attack.processKnownKey(nk)

        # If GUI has override, process it too
        self.setKnownKey(nk)
        nk = self.knownkey()

        attackStats = self.attack.getStatistics()
        attackStats.setKnownkey(nk)
        attackStats.findMaximums(useAbsolute=self.useAbs)

        for bnum in range(0, self.numKeys):
            if bnum in self.enabledBytes and attackStats.maxValid[bnum]:
                self.table.setColumnHidden(bnum, False)
                maxes = attackStats.maxes[bnum]

                pgitm = QTableWidgetItem("%3d"%attackStats.pge[bnum])
                pgitm.setBackground(self.pgeBrush)
                self.table.setItem(0,bnum,pgitm)

                if (self.updateMode == 'all') or attackDone:
                    for j in range(0,self.numPerms):
                        self.table.setItem(j+1,bnum,QTableWidgetItem("%02X\n%.4f"%(maxes[j]['hyp'],maxes[j]['value'])))

                        highlights = self.knownkey()

                        if highlights is not None:
                            try:
                                if maxes[j]['hyp'] == highlights[bnum]:
                                    itm = self.table.item(j+1, bnum)
                                    itm.setForeground(QBrush(Qt.red))
                            except IndexError:
                                pass
            else:
                self.table.setColumnHidden(bnum, True)

        self.table.resizeColumnsToContents()
        self.table.resizeRowsToContents()
        self.ResultsTable.setVisible(True)


class ResultsSave(QObject):
    """Save Correlation Output to Files"""

    def __init__(self):
        super(ResultsSave, self).__init__()

        self._filename = None
        self._enabled = False
        self.dataarray = None

    def paramList(self):
        """Returns list for parameter tree in settings dock"""

        return []

    def setBytesEnabled(self, enabledbytes):
        """Set what bytes to include in table"""

        self.enabledBytes = enabledbytes

    def setStartTrace(self, starttrace):
        """Set starting trace number"""

        self.startTrace = starttrace

    def setAttack(self, attack):
        """Set source of statistics (i.e. attack)"""

        self.attack = attack
        self.attack.attackDone.connect(self.attackDone)
        self.attack.statsUpdated.connect(self.attackStatsUpdated)

    def setKnownKey(self, knownkey):
        """Set the known encryption key, required for PGE calculation"""

        self._knownkey = knownkey

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

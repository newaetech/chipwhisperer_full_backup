#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2013, Colin O'Flynn <coflynn@newae.com>
# All rights reserved.
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
import sys
from collections import OrderedDict

try:
    from PySide.QtCore import *
    from PySide.QtGui import *
except ImportError:
    print "ERROR: PySide is required for this program"
    sys.exit()

sys.path.append('../common')
sys.path.append('../../openadc/controlsw/python/common')
sys.path.append('../common/traces')
imagePath = '../common/images/'

from GraphWidget import GraphWidget

from functools import partial

import numpy as np

try:
    import pyqtgraph as pg
    import pyqtgraph.multiprocess as mp
    import pyqtgraph.parametertree.parameterTypes as pTypes
    from pyqtgraph.parametertree import Parameter, ParameterTree, ParameterItem, registerParameterType
    #print pg.systemInfo()    
except ImportError:
    print "ERROR: PyQtGraph is required for this program"
    sys.exit()

from ExtendedParameter import ExtendedParameter
   
class ResultsPlotting(QObject):
    paramListUpdated = Signal(list)
    
    """Interface to main program, various routines for plotting output data"""
    def __init__(self):
        super(ResultsPlotting, self).__init__()

       
        #No generic ones
        #resultsParams = [
        #              ]

        #self.params = Parameter.create(name='Results', type='group', children=resultsParams)
        #ExtendedParameter.setupExtended(self.params)
        
        #ResultsTable manages class
        self.table = ResultsTable()        
        
        
        self.graphoutput = ResultsPlotData(imagePath)
        self.GraphOutputDock = QDockWidget("Output vs Point Plot")
        self.GraphOutputDock.setObjectName("Output vs Point Plot")
        self.GraphOutputDock.setAllowedAreas(Qt.BottomDockWidgetArea | Qt.RightDockWidgetArea| Qt.LeftDockWidgetArea)
        self.GraphOutputDock.setWidget(self.graphoutput)
        self.graphoutput.setDock(self.GraphOutputDock)

        
    def paramList(self):
        p = [self.table.params, self.graphoutput.params] 
        return p            
            
    def dockList(self):
        return [self.table.ResultsTable, self.GraphOutputDock]
    
    def setAttack(self, attack):
        """Pass the attack to all plotting devices. They pull stats from the attack directly, and listen to attackDone/statusUpdated signals."""
        self.attack = attack
        self.table.setAttack(attack)        
        self.attack.attackDone.connect(self.attackDone)
        self.attack.statsUpdated.connect(self.attackStatsUpdated)
        
        self.graphoutput.setAttack(attack)
        
    def setTraceManager(self, tmanager):        
        self.trace = tmanager        
        self.graphoutput.setKnownKey(self.trace.getKnownKey())
        
    def attackDone(self):
        self.attackStatsUpdated()
        #self.table.setBytesEnabled(self.attack.bytesEnabled())
        self.table.updateTable(attackDone=True)
        self.dumpPGE()
    
    def dumpPGE(self):
        stats = self.attack.getStatistics()
        
        pge = stats.pge_total
        
        allpge = OrderedDict()
                
        for i in pge:
            tnum = i['trace']
            if not tnum in allpge:
                allpge[tnum] = [255]*stats.numSubkeys                
            allpge[tnum][i['subkey']] = i['pge']          
        
        for (tnum, plist) in allpge.iteritems():
            print "%6d "%tnum,
            for j in plist:
                print "%3d "%j,
            print ""
        
    
    def attackStatsUpdated(self):
        self.table.setBytesEnabled(self.attack.bytesEnabled())
        self.table.updateTable()
        
        
class ResultsPlotData(GraphWidget):
    """
    Generic data plotting stuff. Adds ability to highlight certain guesses, used in plotting for example the
    correlation over all data points, or the most likely correlation over number of traces
    """
     
    showDockSignal = Signal(bool)
     
    def __init__(self, imagepath, subkeys=16, permPerSubkey=256):
        super(ResultsPlotData, self).__init__(imagepath)
        
        self.numKeys = subkeys
        self.numPerms = permPerSubkey
        self.knownkey = None
        self.enabledbytes = [False]*subkeys
        
        self.byteNumAct = []
        for i in range(0,self.numKeys):
            self.byteNumAct.append(QAction('%d'%i, self))
            self.byteNumAct[i].triggered[bool].connect(partial(self.setBytePlot, i))
            self.byteNumAct[i].setCheckable(True)
                
        self.bselection = QToolBar()
        
        for i in range(0, self.numKeys):
            self.bselection.addAction(self.byteNumAct[i])
        self.layout().addWidget(self.bselection)
        
        self.highlightTop = True
        
        resultsParams = [{'name':'Show', 'type':'bool', 'key':'show', 'value':False, 'set':self.showDockSignal.emit},                       
                      ]
        
        self.params = Parameter.create(name='Plot of Output vs Time', type='group', children=resultsParams)
        ExtendedParameter.setupExtended(self.params, self)
        
    def paramList(self):
        return [self.params]
   
    def setDock(self, dock):
        self.dock = dock
        self.showDockSignal.connect(dock.setVisible)      
        dock.visibilityChanged.connect(self.visibleChanged)
        self.visibleChanged() 
   
    def visibleChanged(self):
        visible = self.dock.isVisible()
        self.findParam('show').setValue(visible)
   
    def setKnownKey(self, knownkey):
        self.knownkey = knownkey
   
    def setBytePlot(self, num, sel):
        self.enabledbytes[num] = sel
        self.redrawPlot()
   
    def setAttack(self, attack):
        self.attack = attack
        
    def setupHighlights(self):
        self.highlights = []
        
        highlights = self.knownkey
        
        for i in range(0, self.numKeys):
            if highlights is not None:
                self.highlights.append([highlights[i]])
            else:
                self.highlights.append([None])
 
    def highlightColour(self, index):
        if index == 0:
            return 'r'
        else:
            return 'b'
        
    def backgroundplot(self, prange, data, pen=None, highres=False):
        datalen =  max(prange)-min(prange)+1
        if data is None:
            #Setup call 
            if highres is False:
                if pen is None:
                    #No pen specified - init call
                    self.backgroundplotMax = np.empty((datalen,1))
                    self.backgroundplotMax[:] = np.NAN
                    self.backgroundplotMin = np.empty((datalen,1))
                    self.backgroundplotMin[:] = np.NAN
                else:
                    print "Plotting"
                    self.pw.plot(prange, self.backgroundplotMax, pen)
                    self.pw.plot(prange, self.backgroundplotMin, pen)
                    
        else:
            #Store min/max
            self.backgroundplotMax = np.fmax(self.backgroundplotMax, data)
            self.backgroundplotMin = np.fmin(self.backgroundplotMin, data)
        
        
    def redrawPlot(self):
        data = self.attack.getStatistics()
        data = data.diffs
       
        #Do Redraw
        progress = QProgressDialog("Redrawing", "Abort", 0, 100)
        progress.setWindowModality(Qt.WindowModal)
        progress.setMinimumDuration(1000) #_callSync='off'
        progress.setMinimum(0) #_callSync='off'
        progress.setMaximum(256) #_callSync='off'
        
        self.clearPushed()
        self.setupHighlights()
        #prange = range(self.pstart[bnum], self.pend[bnum])
        
        
        #self.backgroundplot(prange, None)
        
        try:
            for bnum in range(0, self.numKeys):
                if self.enabledbytes[bnum]:
                    diffs = data[bnum]
                    
                    maxlimit = np.amax(diffs, 0)
                    minlimit = np.amin(diffs, 0)
                    
                    prange = self.attack.getPointRange(bnum)
                    prange = range(prange[0], prange[1])                    
                    self.pw.plot(prange, maxlimit, pen='g', fillLevel=0.0, brush='g')
                    self.pw.plot(prange, minlimit, pen='g', fillLevel=0.0, brush='g')                   
                    
                    #for i in range(0,self.numPerms):
                    #    canceled = progress.wasCanceled() #_callSync='off'
                    #    if self.highlightTop:
                    #        if i not in self.highlights[bnum]:
                    #            self.backgroundplot(prange, diffs[i])
                    #    else:               
                    #            self.pw.plot(prange, diffs[i], pen=(i%8,8))
                    #    if (i % 32) == 0:
                    #        progress.setValue(i)
                    #    if canceled:
                    #        raise StopIteration
                        
                        
                if self.highlightTop:
                    #Plot the highlighted byte(s) on top
                    for bnum in range(0, self.numKeys):
                        prange = self.attack.getPointRange(bnum)
                        prange = range(prange[0], prange[1])  
                    
                        if self.enabledbytes[bnum]:
                            diffs = data[bnum]
                            
                            for i in range(0,256):   
                                if i in self.highlights[bnum]:
                                    penclr = self.highlightColour( self.highlights[bnum].index(i) )
                                    self.pw.plot(prange, diffs[i], pen=penclr )
                                        
                                #if canceled:
                                #    raise StopIteration
        except StopIteration:
            pass
        
   
class ResultsTable(QObject):
    """Table of results, showing all guesses based on sorting output of attack"""
    def __init__(self, subkeys=16, permPerSubkey=256, useAbs=True):
        super(ResultsTable, self).__init__()

        self.table = QTableWidget(permPerSubkey+1, subkeys)
        self.table.setSizePolicy(QSizePolicy.MinimumExpanding, QSizePolicy.MinimumExpanding)
        
        self.pgeBrush = QBrush(QColor(253,255,205))
        pgehdr =  QTableWidgetItem("PGE")        
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
        self.ResultsTable.setVisible(False)
        self.ResultsTable.visibilityChanged.connect(self.visibleChanged)
        
        self.numKeys = subkeys
        self.numPerms = permPerSubkey
        self.setBytesEnabled([])
        self.useAbs = useAbs
        self.knownkey = None
        self.useSingle = False
        
        resultsParams = [
                         {'name':'Show', 'type':'bool', 'key':'show', 'value':False, 'set':self.ResultsTable.setVisible},
                         {'name':'Use Absolute Value for Rank', 'type':'bool', 'value':True, 'set':self.setAbsoluteMode},
                         {'name':'Use single point for Rank', 'type':'bool', 'value':False, 'set':self.setSingleMode},
                         {'name':'Update Mode', 'key':'updateMode', 'type':'list', 'values':{'Entire Table (Slow)':'all', 'PGE Only (faster)':'pge'}, 'set':self.setUpdateMode},
                      ]

        self.params = Parameter.create(name='Ranked Table', type='group', children=resultsParams)
        ExtendedParameter.setupExtended(self.params, self)
   
        self.updateMode = self.findParam('updateMode').value()
                
        #Update parameter tree
        self.visibleChanged()
        
    def paramList(self):
        return [self.params]
        
    def visibleChanged(self):
        visible = self.ResultsTable.isVisible()
        self.findParam('show').setValue(visible)
        
    def setUpdateMode(self, mode):
        self.updateMode = mode
        
    def setBytesEnabled(self, enabledbytes):
        self.enabledBytes = enabledbytes
        
    def setAttack(self, attack):
        self.attack = attack
        
    def setKnownKey(self, knownkey):
        self.knownkey = knownkey
        
    def setAbsoluteMode(self, enabled):
        self.useAbs = enabled    
        
    def setSingleMode(self, enabled):
        self.useSingle = enabled    

    def updateTable(self, attackDone=False):
        self.setKnownKey(self.attack.trace.getKnownKey())
        
        attackStats = self.attack.getStatistics()
        attackStats.setKnownkey(self.attack.trace.getKnownKey())
        attackStats.findMaximums()
        
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
    
                        highlights = self.knownkey
    
                        if highlights is not None:
                            if maxes[j]['hyp'] == highlights[bnum]:
                                itm = self.table.item(j+1, bnum)
                                itm.setForeground(QBrush(Qt.red))
            else:
                self.table.setColumnHidden(bnum, True)

        self.table.resizeRowsToContents()
        self.table.resizeColumnsToContents()
        self.ResultsTable.setVisible(True)

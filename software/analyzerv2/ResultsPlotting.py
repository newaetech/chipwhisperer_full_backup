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
        self.table.setBytesEnabled(self.attack.bytesEnabled())
        self.table.updateTable()
    
    def attackStatsUpdated(self):
        #self.graphoutput.
        pass
        
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
        
        resultsParams = [{'name':'Show', 'type':'bool', 'value':False, 'set':self.showDockSignal.emit},                       
                      ]
        
        self.params = Parameter.create(name='Plot of Output vs Time', type='group', children=resultsParams)
        ExtendedParameter.setupExtended(self.params)
        
    def paramList(self):
        return [self.params]
   
    def setDock(self, dock):
        self.dock = dock
        self.showDockSignal.connect(dock.setVisible)      
        dock.visibilityChanged.connect(self.visibleChanged) 
   
    def visibleChanged(self):
        #TODO: Update the status of the parameter tree with this
        visible = self.dock.isVisible()
        pass
   
    def setKnownKey(self, knownkey):
        self.knownkey = knownkey
   
    def setBytePlot(self, num, sel):
        self.enabledbytes[num] = sel
        self.redrawPlot()
   
    def setAttack(self, attack):
        self.attack = attack
        
    def setupHighlights(self):
        self.highlights = []
        
        for i in range(0, self.numKeys):
            if self.knownkey is not None:
                self.highlights.append([self.knownkey[0]])
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
       
        #Do Redraw
        progress = QProgressDialog("Redrawing", "Abort", 0, 100)
        progress.setWindowModality(Qt.WindowModal)
        progress.setMinimumDuration(1000) #_callSync='off'
        progress.setMinimum(0) #_callSync='off'
        progress.setMaximum(256) #_callSync='off'
        
        self.clearPushed()
        self.setupHighlights()
        #prange = range(self.pstart[bnum], self.pend[bnum])
        
        prange = range(0, 1500)
        
        #self.backgroundplot(prange, None)
        
        try:
            for bnum in range(0, self.numKeys):
                if self.enabledbytes[bnum]:
                    diffs = data[bnum]
                    
                    maxlimit = np.amax(diffs, 0)
                    minlimit = np.amin(diffs, 0)
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

        self.table = QTableWidget(permPerSubkey, subkeys)
        self.ResultsTable = QDockWidget("Results Table")
        self.ResultsTable.setObjectName("Results Table")
        self.ResultsTable.setAllowedAreas(Qt.BottomDockWidgetArea | Qt.RightDockWidgetArea| Qt.LeftDockWidgetArea)
        self.ResultsTable.setWidget(self.table)       
        self.ResultsTable.setVisible(False)
        self.ResultsTable.visibilityChanged.connect(self.visibleChanged)
        
        self.numKeys = subkeys
        self.numPerms = permPerSubkey
        self.setBytesEnabled([])
        self.useAbs = useAbs
        self.knownkey = None
        self.useSingle = False
        
        resultsParams = [
                         {'name':'Show', 'type':'bool', 'value':False, 'set':self.ResultsTable.setVisible},
                         {'name':'Use Absolute Value for Rank', 'type':'bool', 'value':True, 'set':self.setAbsoluteMode},
                         {'name':'Use single point for Rank', 'type':'bool', 'value':False, 'set':self.setSingleMode}
                      ]

        self.params = Parameter.create(name='Ranked Table', type='group', children=resultsParams)
        ExtendedParameter.setupExtended(self.params)
        
    def paramList(self):
        return [self.params]
        
    def visibleChanged(self):
        #TODO: Update the status of the parameter tree with this
        visible = self.ResultsTable.isVisible()
        
    def setBytesEnabled(self, enabledbytes):
        self.enabledBytes = enabledbytes
        
    def setAttack(self, attack):
        self.attack = attack
        
    def setKnownkey(self, knownkey):
        self.knownkey = knownkey
        
    def setAbsoluteMode(self, enabled):
        self.useAbs = enabled    
        
    def setSingleMode(self, enabled):
        self.useSingle = enabled    

    def updateTable(self):
        self.pge = [self.numPerms-1]*self.numKeys
        data = self.attack.getStatistics()
        
        for bnum in range(0, self.numKeys):
            if bnum in self.enabledBytes:
                self.table.setColumnHidden(bnum, False)
                diffs = data[bnum]                

                maxes = np.zeros(256,dtype=[('hyp','i2'),('point','i4'),('value','f8')] )

                for hyp in range(0, 256):
                    if self.useAbs:
                        v = np.amax(np.fabs(diffs[hyp]))
                    else:
                        v = np.amax(diffs[hyp])                    
                    
                    #Get maximum value for this hypothesis
                    mvalue = v
                    mindex = np.amin(np.where(v == mvalue))
                    maxes[hyp] = (hyp,mindex,mvalue)

                maxes.sort(order='value')
                maxes = maxes[::-1]

                if self.useSingle:
                    #All table values are taken from same point MAX is taken from
                    where = maxes[0]['point']
                    for j in range(0,256):
                        maxes[j]['point'] = where
                        maxes[j]['value'] = diffs[maxes[j]['hyp']][where]
            
                for j in range(0,self.numPerms):
                    self.table.setItem(j,bnum,QTableWidgetItem("%02X\n%.4f"%(maxes[j]['hyp'],maxes[j]['value'])))

                    if self.knownkey is not None:
                        if maxes[j]['hyp'] == self.knownkey[bnum]:
                            self.pge[bnum] = j
            else:
                self.table.setColumnHidden(bnum, True)

        self.table.resizeRowsToContents()
        self.table.resizeColumnsToContents()
        self.ResultsTable.setVisible(True)

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

from openadc.ExtendedParameter import ExtendedParameter
   
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
        
        self.graphoutput = OutputVsTime(imagePath)
        self.GraphOutputDock = QDockWidget(self.graphoutput.name)
        self.GraphOutputDock.setObjectName(self.graphoutput.name)
        self.GraphOutputDock.setAllowedAreas(Qt.BottomDockWidgetArea | Qt.RightDockWidgetArea| Qt.LeftDockWidgetArea)
        self.GraphOutputDock.setWidget(self.graphoutput)
        self.graphoutput.setDock(self.GraphOutputDock)
        
        self.pgegraph = PGEVsTrace(imagePath)
        self.PGEGraphDock = QDockWidget(self.pgegraph.name)
        self.PGEGraphDock.setObjectName(self.pgegraph.name)
        self.PGEGraphDock.setAllowedAreas(Qt.BottomDockWidgetArea | Qt.RightDockWidgetArea| Qt.LeftDockWidgetArea)
        self.PGEGraphDock.setWidget(self.pgegraph)
        self.pgegraph.setDock(self.PGEGraphDock)
        
        
    def paramList(self):
        p = [self.table.params, self.graphoutput.params, self.pgegraph.params] 
        return p            
            
    def dockList(self):
        return [self.table.ResultsTable, self.GraphOutputDock, self.PGEGraphDock]
    
    def setAttack(self, attack):
        """Pass the attack to all plotting devices. They pull stats from the attack directly, and listen to attackDone/statusUpdated signals."""
        self.attack = attack
        self.table.setAttack(attack)        
        self.attack.attackDone.connect(self.attackDone)
        self.attack.statsUpdated.connect(self.attackStatsUpdated)
        
        self.graphoutput.setAttack(attack)
        
        self.pgegraph.setAttack(attack)
        
    def setTraceManager(self, tmanager):        
        self.trace = tmanager       
        
    def updateKnownKey(self):
        try:
            nk = self.trace.getKnownKey()
            nk = self.attack.processKnownKey(nk)
            self.graphoutput.setKnownKey(nk)
        except AttributeError, e:
            print str(e)
        
    def attackDone(self):
        self.attackStatsUpdated()
        #self.table.setBytesEnabled(self.attack.bytesEnabled())
        self.table.updateTable(attackDone=True)        
    
    def attackStatsUpdated(self):
        self.table.setBytesEnabled(self.attack.bytesEnabled())
        self.table.updateTable()
        self.updateKnownKey()
               
class ResultsPlotData(GraphWidget):
    """
    Generic data plotting stuff. Adds ability to highlight certain guesses, used in plotting for example the
    correlation over all data points, or the most likely correlation over number of traces
    """
     
    showDockSignal = Signal(bool)
    name = "Some Descriptive Name"
     
    def __init__(self, imagepath, subkeys=16, permPerSubkey=256):
        super(ResultsPlotData, self).__init__(imagepath)
        
        self.numKeys = subkeys
        self.numPerms = permPerSubkey
        self.knownkey = None
        self.enabledbytes = [False]*subkeys
        self.doRedraw = True
        
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
        
        resultsParams = [{'name':'Show', 'type':'bool', 'key':'show', 'value':False, 'set':self.showDockSignal.emit},                      
                      ]
        self.params = Parameter.create(name=self.name, type='group', children=resultsParams)
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
        if self.doRedraw:
            self.redrawPlot()
        
    def setByteAll(self, status):
        self.doRedraw = False
        for t in self.byteNumAct:
            t.setChecked(status)
            t.trigger()
        self.doRedraw = True
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
        
class OutputVsTime(ResultsPlotData):
    """
    Generic data plotting stuff. Adds ability to highlight certain guesses, used in plotting for example the
    correlation over all data points, or the most likely correlation over number of traces
    """
    
    name = "Output vs Point Plot"

    def __init__(self, imagepath, subkeys=16, permPerSubkey=256):
        super(OutputVsTime, self).__init__(imagepath)
        
        self.numKeys = subkeys
        self.numPerms = permPerSubkey
        
        resultsParams = [{'name':'Show', 'type':'bool', 'key':'show', 'value':False, 'set':self.showDockSignal.emit},       
                         {'name':'Fast Draw', 'type':'bool', 'key':'fast', 'value':True},
                         {'name':'Hide during Redraw', 'type':'bool', 'key':'hide', 'value':True}                 
                      ]
        self.params = Parameter.create(name=self.name, type='group', children=resultsParams)
        ExtendedParameter.setupExtended(self.params, self)
        
                  
    def redrawPlot(self):
        data = self.attack.getStatistics()
        data = data.diffs
       
        byteson = 0
        for i in range(0, self.numKeys):
            if self.enabledbytes[i]:
                byteson += 1
       
        #Do Redraw
        progress = QProgressDialog("Redrawing", "Abort", 0, 100)
        progress.setWindowModality(Qt.WindowModal)
        progress.setMinimumDuration(1000) #_callSync='off'
        progress.setMinimum(0) #_callSync='off'
        progress.setMaximum(byteson*self.numPerms) #_callSync='off'
        
        self.clearPushed()
        self.setupHighlights()
        #prange = range(self.pstart[bnum], self.pend[bnum])
        
        fastDraw = self.findParam('fast').value()
                
        pvalue = 0
                
        if self.findParam('hide').value():
            self.pw.setVisible(False)
        try:
            for bnum in range(0, self.numKeys):
                progress.setValue(pvalue)
                if self.enabledbytes[bnum]:
                    diffs = data[bnum]
                                        
                    prange = self.attack.getPointRange(bnum)
                    prange = range(prange[0], prange[1])                    
                    
                    if fastDraw:             
                        if self.highlightTop:
                            newdiff = np.array(diffs)
                            for j in self.highlights[bnum]:
                                newdiff[j] = 0
                        else:
                            newdiff = diffs
                                
                                  
                        maxlimit = np.amax(newdiff, 0)
                        minlimit = np.amin(newdiff, 0)
                        self.pw.plot(prange, maxlimit, pen='g', fillLevel=0.0, brush='g')
                        self.pw.plot(prange, minlimit, pen='g', fillLevel=0.0, brush='g')
                        pvalue += self.numPerms
                        progress.setValue(pvalue)
                    else:
                        for i in range(0,256):
                            self.pw.plot(prange, diffs[i], pen='g')     
                            pvalue += 1
                            progress.setValue(pvalue)                  
                        
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
        except StopIteration:
            pass
        
        self.pw.setVisible(True)
      
class PGEVsTrace(ResultsPlotData):
    """
    Plots Partial Guessing Entropy (PGE) vs Number of Traces in Attack
    """
    
    name = "PGE vs Trace Plot"

    def __init__(self, imagepath, subkeys=16, permPerSubkey=256):
        super(PGEVsTrace, self).__init__(imagepath)
        
        self.numKeys = subkeys
        self.numPerms = permPerSubkey
        
        resultsParams = [{'name':'Show', 'type':'bool', 'key':'show', 'value':False, 'set':self.showDockSignal.emit},   
                         {'name':'Copy PGE Data to Clipboard', 'type':'action', 'action':self.copyPGE},
                         {'name':'Clipboard Format', 'key':'fmt', 'type':'list', 'values':['CSV', 'MATLAB'], 'value':'CSV'},               
                      ]
        self.params = Parameter.create(name=self.name, type='group', children=resultsParams)
        ExtendedParameter.setupExtended(self.params, self)
        
    def copyPGE(self, dontCopy=False, addPlotMatlab=True):        
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
                spge += "ylabel('Average PGE (%d Trials)')\n"%trials       
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
                    #print "%d "%j['trials'],             
                else:
                    j['pge'] = None
                
        #print ""
                
        return allpge
                  
    def redrawPlot(self):
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
                 
                #penclr = self.highlightColour( self.highlights[bnum].index(i) )
                #                   self.pw.plot(prange, diffs[i], pen=penclr )
                        
                #if self.highlightTop:
                #    #Plot the highlighted byte(s) on top
                #    for bnum in range(0, self.numKeys):
                #        prange = self.attack.getPointRange(bnum)
                #        prange = range(prange[0], prange[1])                                      
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
        nk = self.attack.trace.getKnownKey()
        nk = self.attack.processKnownKey(nk)
        self.setKnownKey(nk)
                
        attackStats = self.attack.getStatistics()
        attackStats.setKnownkey(nk)
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
                            try:
                                if maxes[j]['hyp'] == highlights[bnum]:
                                    itm = self.table.item(j+1, bnum)
                                    itm.setForeground(QBrush(Qt.red))
                            except IndexError:
                                pass
            else:
                self.table.setColumnHidden(bnum, True)

        self.table.resizeRowsToContents()
        self.table.resizeColumnsToContents()
        self.ResultsTable.setVisible(True)

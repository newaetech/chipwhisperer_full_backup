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

import sys
from PySide.QtCore import *
from PySide.QtGui import *
import os.path
sys.path.append('../common')
import pyqtgraph as pg
import pyqtgraph.multiprocess as mp
import tracereader_dpacontestv3
import attack_dpav1
import attack_cpav1
import attack_bayesiancpa
import numpy


class PATab(QWidget):
    def __init__(self, previewtab, MainWindow=None):
        QWidget.__init__(self)
        layout = QVBoxLayout()
        #self.dpa = attack_dpav1.attack_DPAAESv1()
        #self.dpa = attack_cpav1.attack_CPAAESv1()
        self.dpa = None

        self.redrawInProgress = False

        ###This GB/Layout contains all the generic analysis options
        setupGB = QGroupBox("Analysis Options")
        setupLayout = QVBoxLayout()
        setupGB.setLayout(setupLayout)

        pbAttack = QPushButton("Attack!")
        pbAttack.clicked.connect(self.attackPushed)
        setupLayout.addWidget(pbAttack)

        gensettings = QHBoxLayout()
        setupLayout.addLayout(gensettings)

        ##This layout contains the trace & point selection
        tracepointLayout = QVBoxLayout()
        gensettings.addLayout(tracepointLayout)

        #Traces to include in attack
        self.startTracePrint = QSpinBox()        
        self.startTracePrint.setMinimum(0)
        self.startTracePrint.setMaximum(1000000)
        self.endTracePrint = QSpinBox()        
        self.endTracePrint.setMinimum(0)
        self.endTracePrint.setMaximum(1000000)
        tlayout = QHBoxLayout()
        tlayout.addWidget(QLabel("Traces: "))
        tlayout.addWidget(self.startTracePrint)
        tlayout.addWidget(QLabel(" to "))
        tlayout.addWidget(self.endTracePrint)
        tlayout.addStretch()
        tracepointLayout.addLayout(tlayout)

        #Points to include in attack
        self.startPointPrint = QSpinBox()        
        self.startPointPrint.setMinimum(0)
        self.startPointPrint.setMaximum(1000000)
        self.endPointPrint = QSpinBox()        
        self.endPointPrint.setMinimum(0)
        self.endPointPrint.setMaximum(1000000)
        playout = QHBoxLayout()
        playout.addWidget(QLabel("Points: "))
        playout.addWidget(self.startPointPrint)
        playout.addWidget(QLabel(" to "))
        playout.addWidget(self.endPointPrint)
        playout.addStretch()
        tracepointLayout.addLayout(playout)

        tracepointLayout.addStretch()

        ##Select Attack Method
        self.attackMode = QComboBox()        
        self.attackMode.addItem("Correlation (Simple)")
        self.attackMode.addItem("Differential (DPA)")
        self.attackMode.addItem("Correlation (Bayesian)")
        tracepointLayout.addWidget(self.attackMode)
        tracepointLayout.addStretch()
        
        self.attackOptsGB = QGroupBox("Attack Options")
        self.attackLayoutGB = QVBoxLayout()
        self.attackOptsGB.setLayout(self.attackLayoutGB)
        tracepointLayout.addWidget(self.attackOptsGB)
        self.attackMode.currentIndexChanged.connect(self.attackModeChanged)
        self.attackModeChanged(0)

        ##Byte Selection
        btnAll = QPushButton("All")
        btnAll.clicked.connect(self.checkAll)
        btnNone = QPushButton("None")
        btnNone.clicked.connect(self.checkNone)
        bselectLayoutPB = QHBoxLayout()
        bselectLayoutPB.addWidget(btnAll, 0, 6)
        bselectLayoutPB.addWidget(btnNone,0, 7)
        bselectLayoutCB = QGridLayout()
        self.do = []
        for i in range(0,16):
            self.do.append(QCheckBox("%2d"%i))
            self.do[i].setChecked(True)
            bselectLayoutCB.addWidget(self.do[i], i/8, i%8)
        bselectLayout = QVBoxLayout()
        bselectLayout.addLayout(bselectLayoutPB)
        bselectLayout.addLayout(bselectLayoutCB)
        bselectGB = QGroupBox("Byte Selection")
        bselectGB.setLayout(bselectLayout)
        gensettings.addWidget(bselectGB)
        gensettings.addStretch()

        layout.addWidget(setupGB)
        

        self.table = QTableWidget(256, 16)
        self.ResultsTable = QDockWidget("Results Table")
        self.ResultsTable.setAllowedAreas(Qt.BottomDockWidgetArea | Qt.RightDockWidgetArea| Qt.LeftDockWidgetArea)
        self.ResultsTable.setWidget(self.table)
        MainWindow.addDockWidget(Qt.RightDockWidgetArea, self.ResultsTable)
        self.ResultsTable.setVisible(False)

        ###This GB/Layout contains all the View options
        viewGB = QGroupBox("View Options")
        viewLayout = QVBoxLayout()
        viewGB.setLayout(viewLayout)
        
        ##Select the byte to highlight
        hlbGB = QGroupBox("Byte Highlight Option")
        hlbLayout = QVBoxLayout()
        hlbGB.setLayout(hlbLayout)
        
        #Put the options down in a row
        hlbOptionsLayout = QHBoxLayout()
        self.highlightByte_copySB = QSpinBox()
        self.highlightByte_copySB.setMinimum(1)
        self.highlightByte_copySB.setMaximum(32)
        self.highlightByte_levelSB = QSpinBox()
        self.highlightByte_levelSB.setMinimum(1)
        self.highlightByte_levelSB.setMaximum(32)
        highlightByte_copyPB = QPushButton("Copy top")
        highlightByte_copyPB.clicked.connect(self.highlightsCopyTop)
        hlbOptionsLayout.addWidget(highlightByte_copyPB)
        hlbOptionsLayout.addWidget(self.highlightByte_copySB)
        hlbOptionsLayout.addWidget(QLabel("levels from table."))
        hlbOptionsLayout.addStretch()
        
        highlightByte_copyKeyPB = QPushButton("Copy Key to Level")
        highlightByte_copyKeyPB.clicked.connect(self.highlightsCopyKey)
        hlbOptionsLayout.addWidget(highlightByte_copyKeyPB)
        hlbOptionsLayout.addStretch()
                
        hlbOptionsLayout.addWidget(QLabel("Modify Highlight Level:"))
        hlbOptionsLayout.addWidget(self.highlightByte_levelSB)
        self.highlightByte_levelSB.valueChanged.connect(self.highlightsLevelChanged)

        hlbOptionsLayout.addStretch()

        highlightByte_clearPB = QPushButton("Clear ALL")
        highlightByte_clearPB.clicked.connect(self.highlightsClear)
        hlbOptionsLayout.addWidget(highlightByte_clearPB)
                
        #Put the drop-down menus for each byte & labels
        hlbByteLayout = QGridLayout()
        self.highlightByte = []
        for i in range(0, 16):
            self.highlightByte.append(QComboBox())
            self.highlightByte[i].addItem(" ")
            for j in range(0, 255):
                self.highlightByte[i].addItem("%02x"%j)
            hlbByteLayout.addWidget(QLabel("%02d"%i), 0, i)
            hlbByteLayout.addWidget(self.highlightByte[i], 1, i)
            self.highlightByte[i].currentIndexChanged.connect(self.highlightsBytesChanged)

        hlbLayout.addLayout(hlbByteLayout)
        hlbLayout.addLayout(hlbOptionsLayout)
        viewLayout.addWidget(hlbGB, 0, 0)

        self.changingLevels = False
        self.highlightsClear()

        ##Table Options
        tableGB = QGroupBox("Table Options")
        tableLayout = QVBoxLayout()
        tableGB.setLayout(tableLayout)

        self.tableOnePtCB = QCheckBox("Point = Max Only")
        tableLayout.addWidget(self.tableOnePtCB)
        viewLayout.addWidget(tableGB)

        ##Buttons etc for view
        viewButtons = QHBoxLayout()
        viewLayout.addLayout(viewButtons)
        
        redrawPB = QPushButton("Redraw")
        redrawPB.clicked.connect(self.redraw)
        viewButtons.addWidget(redrawPB)

        viewButtons.addStretch()

        layout.addWidget(viewGB)
       
        ###Setup trace viewer
        self.AnalysisViewDocks = []
        for bnum in range(0,16):
            pw = pg.PlotWidget(name="'Analysis Byte %d'%bnum")
            pw.setTitle('Analysis Byte %d'%bnum)
            pw.setLabel('bottom', 'Sample Number')
            pw.setLabel('left', 'Difference')
            vb = pw.getPlotItem().getViewBox()
            vb.setMouseMode(vb.RectMode)
            
            dock = QDockWidget('B=%d'%bnum, self)
            dock.setAllowedAreas(Qt.BottomDockWidgetArea | Qt.RightDockWidgetArea| Qt.LeftDockWidgetArea)
            dock.setWidget(pw)
            self.AnalysisViewDocks.append(dock)
            MainWindow.addDockWidget(Qt.BottomDockWidgetArea, dock)            
            if(bnum > 0):
                MainWindow.tabifyDockWidget(self.AnalysisViewDocks[bnum-1], self.AnalysisViewDocks[bnum])
            dock.setVisible(False)                

        #This is an abomination I know. But I don't know how to check
        #for visability (just isVisible() isn't good enough) besides this event,
        #since need to catch things like tab selection, being dragged into its
        #own window, etc. Deal with it for now
        self.AnalysisViewDocks[0].visibilityChanged.connect(self.vchanged0)
        self.AnalysisViewDocks[1].visibilityChanged.connect(self.vchanged1)
        self.AnalysisViewDocks[2].visibilityChanged.connect(self.vchanged2)
        self.AnalysisViewDocks[3].visibilityChanged.connect(self.vchanged3)
        self.AnalysisViewDocks[4].visibilityChanged.connect(self.vchanged4)
        self.AnalysisViewDocks[5].visibilityChanged.connect(self.vchanged5)
        self.AnalysisViewDocks[6].visibilityChanged.connect(self.vchanged6)
        self.AnalysisViewDocks[7].visibilityChanged.connect(self.vchanged7)
        self.AnalysisViewDocks[8].visibilityChanged.connect(self.vchanged8)
        self.AnalysisViewDocks[9].visibilityChanged.connect(self.vchanged9)
        self.AnalysisViewDocks[10].visibilityChanged.connect(self.vchanged10)
        self.AnalysisViewDocks[11].visibilityChanged.connect(self.vchanged11)
        self.AnalysisViewDocks[12].visibilityChanged.connect(self.vchanged12)
        self.AnalysisViewDocks[13].visibilityChanged.connect(self.vchanged13)
        self.AnalysisViewDocks[14].visibilityChanged.connect(self.vchanged14)
        self.AnalysisViewDocks[15].visibilityChanged.connect(self.vchanged15)
        
        #layout.addWidget(self.pw)
        layout.addStretch()
        self.setLayout(layout)

        self.redrawRequired = []
        for i in range(0,16):
            self.redrawRequired.append(True)

    def removeAllWidgets(self, layout):
        child = layout.takeAt(0)
        while child:
            if child.layout() != None:
                sublayout = child.layout()
                self.removeAllWidgets(sublayout)
                del sublayout
            elif child.widget() != None:
                widget = child.widget()
                widget.hide()
                del widget
            else:                
                del child
            child = layout.takeAt(0)      

    def attackModeChanged(self, index):
        self.removeAllWidgets(self.attackLayoutGB)
    
        if index==0:
            self.dpa = attack_cpav1.attack_CPAAESv1()
        elif index==1:
            self.dpa = attack_dpav1.attack_DPAAESv1()
        elif index==2:
            self.dpa = attack_bayesiancpa.attack_BAYCPAAES()
        else:
            raise ValueError("Unknown Index value %d"%index)

        opts = self.dpa.getOptions()       

        for opt in opts:
            optlayout = QHBoxLayout()
            optlayout.addWidget(QLabel(opt))

            if opts[opt]["Type"] == "CB":
                cb = QComboBox()
                for name in opts[opt]["Opts"]:
                    cb.addItem(name)
                optlayout.addWidget(cb)

            elif opts[opt]["Type"] == "SB":
                sb = QSpinBox()
                sb.setMinimum(opts[opt]["Range"][0])
                sb.setMaximum(opts[opt]["Range"][1])
                optlayout.addWidget(sb)
                
            self.attackLayoutGB.addLayout(optlayout)

    
    def getOpts(self, layout=None):
        if layout == None:
            layout = self.attackLayoutGB

        opts = {}

        i = 0
        child = layout.itemAt(i)
        while child:
            if child.layout() != None:
                sublayout = child.layout()
                opts.update(self.getOpts(sublayout))
            elif child.widget() != None:
                widget = child.widget()
                if type(widget) == type(QLabel()):
                    name = widget.text()
                elif type(widget) == type(QComboBox()):
                    opts[name] = widget.currentText()
                elif type(widget) == type(QSpinBox()):
                    opts[name] = widget.value()
                else:
                    raise ValueError("Unknown widget type %s"%type(widget))
                    
            i = i + 1
            child = layout.itemAt(i)

        return opts

    def showAnalysisWidgets(self, visible=True):
        self.ResultsTable.setVisible(visible)

    def checkAll(self):
        for i in range(0,16):
            self.do[i].setChecked(True)

    def checkNone(self):
        for i in range(0,16):
            self.do[i].setChecked(False)
            
    def passTrace(self, trace):
        self.trace = trace
        self.startTracePrint.setMaximum(trace.NumTrace)
        self.endTracePrint.setMaximum(trace.NumTrace)
        self.endTracePrint.setValue(trace.NumTrace)

        self.startPointPrint.setMaximum(trace.NumPoint)
        self.endPointPrint.setMaximum(trace.NumPoint)
        self.endPointPrint.setValue(trace.NumPoint)

    def loadPushed(self):
        return

    def redraw(self):
        for req in self.redrawRequired:
            req = True

        for i in range(len(self.redrawRequired)):
            if self.do[i].isChecked():
                self.redrawOneResult(i)

    def highlightsBytesChanged(self, index=None):
        if self.changingLevels:
            return
        i = 0
        for b in self.highlightByte:
            self.highlights[i][self.highlightByte_levelSB.value()-1] = b.currentIndex()-1
            i = i + 1

        return

    def highlightsLevelChanged(self, index=None):        
        self.changingLevels = True

        i = 0
        for b in self.highlightByte:
            value = self.highlights[i][self.highlightByte_levelSB.value()-1] + 1
            b.setCurrentIndex(value)
            i = i + 1
            
        self.changingLevels = False
        return

    def highlightsCopyKey(self):
        for i in range(0,16):
            if self.trace:
                self.highlights[i][self.highlightByte_levelSB.value()-1] = self.trace.knownkey[i];
        self.highlightsLevelChanged()

    def highlightsCopyTop(self):        
        return

    def highlightsClear(self):
        #256 levels, each level has 1 byte
        #Rem 0 = NO highlight, actual byte to highlight is -1 from value
        self.changingLevels = False
        self.highlights = [[-1]*32 for i in range(16)]
        self.highlightByte_levelSB.setValue(0)
        self.highlightsBytesChanged(0)

    def updateTable(self):
        for bnum in range(0,len(self.do)):
            if self.do[bnum].isChecked():
                self.table.setColumnHidden(bnum, False)
                diffs = self.dpa.getDiff(bnum)

                maxes = numpy.zeros(256,dtype=[('hyp','i2'),('point','i4'),('value','f8')] )

                for hyp in range(0, 256):
                    #Get maximum value for this hypothesis
                    mvalue = numpy.amax(numpy.fabs(diffs[hyp]))
                    mindex = numpy.amin(numpy.where(numpy.fabs(diffs[hyp]) == mvalue))
                    maxes[hyp] = (hyp,mindex,mvalue)

                maxes.sort(order='value')
                maxes = maxes[::-1]

                if self.tableOnePtCB.isChecked():
                    #All table values are taken from same point MAX is taken from
                    where = maxes[0]['point']
                    for j in range(0,256):
                        maxes[j]['point'] = where
                        maxes[j]['value'] = diffs[maxes[j]['hyp']][where]
            
                for j in range(0,256):
                    self.table.setItem(j,bnum,QTableWidgetItem("%02X\n%.4f"%(maxes[j]['hyp'],maxes[j]['value'])))
            else:
                self.table.setColumnHidden(bnum, True)

        self.table.resizeRowsToContents()
        self.table.resizeColumnsToContents()
        self.ResultsTable.setVisible(True)   

    def vchanged0(self, visible):
        if (visible):
            self.redrawOneResult(0)
    def vchanged1(self, visible):
        if (visible):
            self.redrawOneResult(1)
    def vchanged2(self, visible):
        if (visible):
            self.redrawOneResult(2)
    def vchanged3(self, visible):
        if (visible):
            self.redrawOneResult(3)
    def vchanged4(self, visible):
        if (visible):
            self.redrawOneResult(4)
    def vchanged5(self, visible):
        if (visible):
            self.redrawOneResult(5)
    def vchanged6(self, visible):
        if (visible):
            self.redrawOneResult(6)
    def vchanged7(self, visible):
        if (visible):
            self.redrawOneResult(7)
    def vchanged8(self, visible):
        if (visible):
            self.redrawOneResult(8)
    def vchanged9(self, visible):
        if (visible):
            self.redrawOneResult(9)
    def vchanged10(self, visible):
        if (visible):
            self.redrawOneResult(10)
    def vchanged11(self, visible):
        if (visible):
            self.redrawOneResult(11)
    def vchanged12(self, visible):
        if (visible):
            self.redrawOneResult(12)
    def vchanged13(self, visible):
        if (visible):
            self.redrawOneResult(13)
    def vchanged14(self, visible):
        if (visible):
            self.redrawOneResult(14)
    def vchanged15(self, visible):
        if (visible):
            self.redrawOneResult(15)

    def redrawOneResult(self, bnum, highlightByte=True, Force=False):
        if self.redrawInProgress:
            return
        
        if (self.redrawRequired[bnum] == False) and (Force == False):
            return

        self.redrawRequired[bnum] = False
        self.redrawInProgress = True
        
        diffs = self.dpa.getDiff(bnum)
        #Do Redraw
        #proc = mp.QtProcess()
        #rqt = proc._import('PySide.QtGui')        
        #progress = rqt.QProgressDialog("Redrawing", "Abort", 0, 100)
        progress = QProgressDialog("Redrawing", "Abort", 0, 100)
        progress.setWindowModality(Qt.WindowModal)
        progress.setMinimumDuration(1000) #_callSync='off'
        progress.setMinimum(0) #_callSync='off'
        progress.setMaximum(256) #_callSync='off'

        self.AnalysisViewDocks[bnum].widget().clear
       #self.AnalysisViewDocks[bnum].setVisible(False)
       
        for i in range(0,256):
            canceled = progress.wasCanceled() #_callSync='off'
            if highlightByte:
                if i not in self.highlights[bnum]:
                    self.AnalysisViewDocks[bnum].widget().plot(diffs[i], pen='g')
            else:               
                    self.AnalysisViewDocks[bnum].widget().plot(diffs[i], pen=(i%8,8))
            if (i % 32) == 0:
                progress.setValue(i) #, _callSync='off')
            #if canceled.hasResult() and canceled.result() is True:
            if canceled:
                break

        #Plot the highlighted byte(s) on top
        for i in range(0,256):
            if highlightByte:
                if i in self.highlights[bnum]:
                    penclr = self.whatColour( self.highlights[bnum].index(i) )
                    self.AnalysisViewDocks[bnum].widget().plot(diffs[i], pen=penclr )
            
        self.AnalysisViewDocks[bnum].setVisible(True)

        self.redrawInProgress = False

    def whatColour(self, index):
        if index == 0:
            return 'r'
        else:
            return 'b'       
            
    def attackPushed(self):
        data = []
        textins = []
        textouts = []

        for i in range(0, 16):
            self.redrawRequired[i] = True

        for i in range(self.startTracePrint.value(), self.endTracePrint.value()):
            data.append(self.trace.getTrace(i)[self.startPointPrint.value():self.endPointPrint.value()])
            textins.append(self.trace.getTextin(i))
            textouts.append(self.trace.getTextout(i)) 

        rangeDo = []
        for i in range(0,len(self.do)):
            if self.do[i].isChecked():
                rangeDo.append(i)
        self.rangeDo = rangeDo
        
        progress = QProgressDialog("Analyzing", "Abort", 0, 100)
        progress.setWindowModality(Qt.WindowModal)
        progress.setMinimumDuration(1000)
        opts = self.getOpts()
        #print opts
        self.dpa.doDPA(rangeDo, data, textins, textouts, progress, encodedopts=opts)

        self.updateTable()

        for i in range(0,16):
            if (self.do[i].isChecked()):
                self.AnalysisViewDocks[i].widget().clear()  
                self.AnalysisViewDocks[i].setVisible(True)                              

class PreviewTab(QWidget):
    def __init__(self):
        QWidget.__init__(self)
        layout = QVBoxLayout()

        setupGB = QGroupBox("Load Options")
        setupLayout = QGridLayout()
        setupGB.setLayout(setupLayout)

        pbLoad = QPushButton("Load All")
        pbLoad.clicked.connect(self.loadPushed)
        setupLayout.addWidget(pbLoad, 0, 0)

        pbRedraw = QPushButton("Redraw")
        pbRedraw.clicked.connect(self.redrawPushed)
        setupLayout.addWidget(pbRedraw, 0, 1)

        self.startTracePrint = QSpinBox()        
        self.startTracePrint.setMinimum(0)
        self.endTracePrint = QSpinBox()        
        self.endTracePrint.setMinimum(0)
        self.startPointPrint = QSpinBox()        
        self.startPointPrint.setMinimum(0)
        self.endPointPrint = QSpinBox()        
        self.endPointPrint.setMinimum(0)

        setupLayout.addWidget(QLabel("Traces: "), 1, 0)
        setupLayout.addWidget(self.startTracePrint, 1, 1)
        setupLayout.addWidget(QLabel(" to "), 1, 2)
        setupLayout.addWidget(self.endTracePrint, 1, 3)
        
        setupLayout.addWidget(QLabel("Points: "), 2, 0)
        setupLayout.addWidget(self.startPointPrint, 2, 1)
        setupLayout.addWidget(QLabel(" to "), 2, 2)
        setupLayout.addWidget(self.endPointPrint, 2, 3)
        
        layout.addWidget(setupGB)

        #Setup plot widget
        self.pw = pg.PlotWidget(name="Power Trace View")
        self.pw.setLabel('top', 'Power Trace View')
        self.pw.setLabel('bottom', 'Samples')
        self.pw.setLabel('left', 'Data')
        vb = self.pw.getPlotItem().getViewBox()
        vb.setMouseMode(vb.RectMode)
        
        layout.addWidget(self.pw)        
        self.setLayout(layout)

    def passTrace(self, trace):
        self.trace = trace
        self.startTracePrint.setMaximum(trace.NumTrace)
        self.endTracePrint.setMaximum(trace.NumTrace)
        self.endTracePrint.setValue(trace.NumTrace)

        self.startPointPrint.setMaximum(trace.NumPoint)
        self.endPointPrint.setMaximum(trace.NumPoint)
        self.endPointPrint.setValue(trace.NumPoint)

    def loadPushed(self):
        self.trace.loadAllTraces()
        #Preview one full trace for now
        data = self.trace.getTrace(0)
        self.pw.plot(data, pen='r')        

    def redrawPushed(self):
        progress = QProgressDialog("Redrawing", "Abort", 0, 100)
        progress.setWindowModality(Qt.WindowModal)
        progress.setMinimumDuration(1000)
        progress.setMinimum(self.startTracePrint.value())
        progress.setMaximum(self.endTracePrint.value())        

        #self.pw.setVisible(False)
        for i in range(self.startTracePrint.value(), self.endTracePrint.value()):
            data = self.trace.getTrace(i)
            self.pw.plot(data[self.startPointPrint.value():self.endPointPrint.value()], pen=(i%8,8))            
            progress.setValue(i)
            if progress.wasCanceled():
                break
        #self.pw.setVisible(True)

class MainChip(QMainWindow):
    MaxRecentFiles = 4
    
    def __init__(self):       
        super(MainChip, self).__init__()

        pg.setConfigOption('background', 'w')
        pg.setConfigOption('foreground', 'k')
        
        self.trace = tracereader_dpacontestv3.tracereader_dpacontestv3()
        self.initUI()

        #Settings
        settings = QSettings('ChipWhisperer', 'chipwhisperer-analyzer')
        self.restoreGeometry(settings.value("geometry"));

    def closeEvent(self, event):
        settings = QSettings('ChipWhisperer', 'chipwhisperer-analyzer')
        settings.setValue("geometry", self.saveGeometry());
        QMainWindow.closeEvent(self, event)

    def createActions(self):
        self.openAct = QAction(QIcon('open.png'), '&Open Input Files', self,
                               shortcut=QKeySequence.Open,
                               statusTip='Open Input Files (waveform, etc)',
                               triggered=self.open)

        for i in range(MainChip.MaxRecentFiles):
            self.recentFileActs.append(QAction(self, visible=False, triggered=self.openRecentFile))

    def createMenus(self):
        self.fileMenu= self.menuBar().addMenu('&File')
        self.fileMenu.addAction(self.openAct)
        self.separatorAct = self.fileMenu.addSeparator()
        for i in range(MainChip.MaxRecentFiles):
            self.fileMenu.addAction(self.recentFileActs[i])
            
        
    def initUI(self):        
        self.statusBar()
        self.setWindowTitle("Chip Whisperer: Analyzer")

        self.recentFileActs = []
        self.createActions()
        self.createMenus()

        self.updateRecentFileActions()
    
        
        #self.setGeometry(300, 300, 350, 300)
        self.show()

        #Preview Tab Setup
        self.preview = PreviewTab()

        #DPA Tab Setup
        self.dpa = PATab(self.preview, self)

        #Tab Widget Setup
        self.tw = QTabWidget()
        self.tw.currentChanged.connect(self.curTabChange) 
        self.tw.addTab(self.preview, "&Trace View")
        self.tw.addTab(self.dpa, "&Power Analysis")
        self.curTabChange(0)

        self.setCentralWidget(self.tw)

    def setCurrentFile(self, fname):
        settings = QSettings('ChipWhisperer', 'chipwhisperer-analyzer')
        files = settings.value('recentFileList', [])

        try:
            files.remove(fname)
        except ValueError:
            pass

        files.insert(0, fname)
        del files[MainChip.MaxRecentFiles:]

        settings.setValue('recentFileList', files)
        for widget in QApplication.topLevelWidgets():
            if isinstance(widget, MainChip):
                widget.updateRecentFileActions()

    def updateRecentFileActions(self):
        settings = QSettings('ChipWhisperer', 'chipwhisperer-analyzer')
        files = settings.value('recentFileList')
        files_no = 0

        if files:
            files_no = len(files)

        numRecentFiles = min(files_no, MainChip.MaxRecentFiles)

        for i in range(numRecentFiles):
            text = "&%d %s" % (i + 1, self.strippedName(files[i]))
            self.recentFileActs[i].setText(text)
            self.recentFileActs[i].setData(files[i])
            self.recentFileActs[i].setVisible(True)

        for j in range(numRecentFiles, MainChip.MaxRecentFiles):
            self.recentFileActs[j].setVisible(False)

        self.separatorAct.setVisible((numRecentFiles > 0))

    def strippedName(self, fullFileName):
        (filepath, filename) = os.path.split(fullFileName)
        (base, toplevel) = os.path.split(filepath)
        return toplevel + "/" + filename
        
        #return QFileInfo(fullFileName).fileName()
        
    def loadFile(self, fname):
        self.trace.loadInfo(os.path.dirname(fname))
        self.preview.passTrace(self.trace)
        self.dpa.passTrace(self.trace)
        self.setCurrentFile(fname)
        
    def open(self):
        fname, _ = QFileDialog.getOpenFileName(self, 'Open file','.','info.xml')
        if fname:
            self.loadFile(fname)

    def openRecentFile(self):
        action = self.sender()
        if action:
            self.loadFile(action.data())
        

    def curTabChange(self, index):
        for i in range(self.tw.count()):
            if i == index:
                self.tw.widget(i).setSizePolicy(QSizePolicy.Preferred, QSizePolicy.Preferred)
            else:
                self.tw.widget(i).setSizePolicy(QSizePolicy.Ignored, QSizePolicy.Ignored)
                                                   
        
def main():
    
    app = QApplication(sys.argv)
    ex = MainChip()
    sys.exit(app.exec_())


if __name__ == '__main__':
    main()

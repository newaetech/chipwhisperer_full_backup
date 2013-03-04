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

__author__ = "Colin O'Flynn"

import sys
from PySide.QtCore import *
from PySide.QtGui import *
import os.path
sys.path.append('../common')
import pyqtgraph as pg
import pyqtgraph.multiprocess as mp
import tracereader_dpacontestv3
import tracereader_native
import attack_dpav1
import attack_cpav1_loopy
import attack_bayesiancpa
#import attack_cpa_dpav2wrapper
import re
import numpy as np
from scipy import signal

#For profiling support (not 100% needed)
import pstats, cProfile

import ConfigParser

try:    
    import attack_cpaiterative
except:
    attack_cpaiterative = None


from cwanalyzer_common import CW_VERSION
from cwanalyzer_common import GlobalSettings
from cwanalyzer_common import noProject

class PATab(QWidget):
    def __init__(self, previewtab, MainWindow=None):
        QWidget.__init__(self)
        layout = QVBoxLayout()
        #self.dpa = attack_dpav1.attack_DPAAESv1()
        #self.dpa = attack_cpav1.attack_CPAAESv1()
        self.dpa = None
        self.preprocess = MainWindow.preprocess

        #Index 0 = ALL bytes
        #Index 1...16 = Bytes 0...15
        self.TraceRangeList = [[0]*2]*17
        self.dontUpdateSB = False

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
        self.endPointPrint.valueChanged.connect(self.pointVChanged)
        self.startPointPrint.valueChanged.connect(self.pointVChanged)
        self.pByteSel = QComboBox()
        self.pByteSel.addItem("All Bytes")
        self.pByteSel.addItem("Byte 0")
        self.pByteSel.addItem("Byte 1")
        self.pByteSel.addItem("Byte 2")
        self.pByteSel.addItem("Byte 3")
        self.pByteSel.addItem("Byte 4")
        self.pByteSel.addItem("Byte 5")
        self.pByteSel.addItem("Byte 6")
        self.pByteSel.addItem("Byte 7")
        self.pByteSel.addItem("Byte 8")
        self.pByteSel.addItem("Byte 9")
        self.pByteSel.addItem("Byte 10")
        self.pByteSel.addItem("Byte 11")
        self.pByteSel.addItem("Byte 12")
        self.pByteSel.addItem("Byte 13")
        self.pByteSel.addItem("Byte 14")
        self.pByteSel.addItem("Byte 15")
        self.pByteSel.currentIndexChanged.connect(self.pointIndexChanged)       
        playout = QHBoxLayout()
        playout.addWidget(QLabel("Points: "))
        playout.addWidget(self.startPointPrint)
        playout.addWidget(QLabel(" to "))
        playout.addWidget(self.endPointPrint)
        playout.addWidget(self.pByteSel)
        
        playout.addStretch()
        tracepointLayout.addLayout(playout)

        tracepointLayout.addStretch()

        ##Select Attack Method
        self.attackMode = QComboBox()        
        self.attackMode.addItem("Correlation (Simple)")
        self.attackMode.addItem("Differential (DPA)")
        self.attackMode.addItem("Correlation (Bayesian - EXPERIMENTAL)")
        self.attackMode.addItem("Correlation (Iterative - EXPERIMENTAL)")
        self.attackMode.addItem("Correlation (DPAv2 C++ Reference Code)")
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

        ##Advanced Graphing
        advgpLayout = QVBoxLayout()
        self.advgpEnabledCB = QCheckBox("Enable")
        self.advgpEnabledCB.clicked.connect(self.advgpEnabled)
        advgpLayout.addWidget(self.advgpEnabledCB)

        self.advTraceRange = QSpinBox()
        self.advTraceRange.setMinimum(0)
        self.advTraceRange.setMaximum(1000000)
        self.advTraceRange.setEnabled(False)
        advTLayout = QHBoxLayout()
        advTLayout.addWidget(QLabel("Traces/Round"))
        advTLayout.addWidget(self.advTraceRange)
        advgpLayout.addLayout(advTLayout)
        self.advTraceRange.valueChanged.connect(self.advTraceChanged)
        
        self.advTraceStep = QSpinBox()
        self.advTraceStep.setMinimum(1)
        self.advTraceStep.setMaximum(1000000)
        self.advTraceStep.setEnabled(False)
        advTLayout = QHBoxLayout()
        advTLayout.addWidget(QLabel("Traces/Step"))
        advTLayout.addWidget(self.advTraceStep)
        advgpLayout.addLayout(advTLayout)
        
        self.advTraceRepeat = QSpinBox()
        self.advTraceRepeat.setMinimum(1)
        self.advTraceRepeat.setMaximum(10000)
        self.advTraceRepeat.setEnabled(False)
        advTLayout = QHBoxLayout()
        advTLayout.addWidget(QLabel("Total Rounds"))
        advTLayout.addWidget(self.advTraceRepeat)
        advgpLayout.addLayout(advTLayout)
        
        advgpGB = QGroupBox("Attack Statistics")
        advgpGB.setLayout(advgpLayout)

        self.advPB = QPushButton("Go")
        self.advPB.setEnabled(False)
        self.advPB.setCheckable(True)
        self.advPB.clicked.connect(self.advgpGo)
        advgpLayout.addWidget(self.advPB)

        gensettings.addWidget(advgpGB)


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

    def returnSettings(self):
        return

    def loadSettings(self, settings):
        return

### Functions dealing with advanced graphing
    def advTraceChanged(self):
        maxRounds = int(self.trace.NumTrace / self.advTraceRange.value())
        self.advTraceRepeat.setMaximum(maxRounds)
        self.advTraceRepeat.setValue(maxRounds)
        
    def advgpEnabled(self):
        if self.advgpEnabledCB.isChecked():
            self.startTracePrint.setEnabled(False)
            self.endTracePrint.setEnabled(False)
            self.advTraceRange.setEnabled(True)
            self.advTraceStep.setEnabled(True)
            self.advTraceRepeat.setEnabled(True)
            self.advPB.setEnabled(True)
        else:
            self.startTracePrint.setEnabled(True)
            self.endTracePrint.setEnabled(True)
            self.advTraceRange.setEnabled(False)
            self.advTraceStep.setEnabled(False)
            self.advTraceRepeat.setEnabled(False)
            self.advPB.setEnabled(False)

    def advgpGo(self):
        traces_per_round = self.advTraceRange.value()
        traces_per_step = self.advTraceStep.value()
        total_rounds = self.advTraceRepeat.value()
        #print "t/round: %d\nt/step: %d\nrounds: %d"%(traces_per_round, traces_per_step, total_rounds)

        pgeStats = np.array([[0.0]*16] * (traces_per_round/traces_per_step), dtype="float")


        for rnum in range(0, total_rounds):
            print "Round %d - Partial Guessing Entropy"%(rnum+1)
            strace = traces_per_round * rnum
            cnt = 0
            for toffset in range(strace, traces_per_round+strace, traces_per_step):
                #print "Attack %d - %d"%(strace, toffset + traces_per_step)
                #self.startTracePrint.setValue(toffset)                
                self.startTracePrint.setValue(strace)
                self.endTracePrint.setValue(toffset+traces_per_step)
                self.attackPushed()

                print "%5d   "%(toffset+traces_per_step - strace),
                self.printPGE()
                
                pgeStats[cnt] = (pgeStats[cnt]*rnum + self.pge) / (rnum + 1)
                cnt = cnt + 1

                if  self.advPB.isChecked() == False:
                    break
                
            if  self.advPB.isChecked() == False:
                break

        f = open('pge2.txt', 'w')
        print "Partial Guessing Entropy - Average of All Rounds"
        for indx, pgestat in enumerate(pgeStats):
            step = indx * traces_per_step + traces_per_step
            print "%5d   "%step,
            self.printPGE(pgestat, usefloat=True)

            f.write("%5d   "%step)
            for j in pgestat:
                f.write("%3.2f "%j)
            f.write("\n");
        f.close()

        self.advPB.setChecked(False)

### Functions dealing with Main Analysis Options
    def pointVChanged(self, newpt):
        if self.dontUpdateSB == False:
            self.TraceRangeList[self.pByteSel.currentIndex()] = list([self.startPointPrint.value(), self.endPointPrint.value()])

    def pointIndexChanged(self, newIndx):
        self.validatePointSB()

### Functions dealing with specific attack options
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
            self.dpa = attack_cpav1_loopy.attack_CPAAESv1()
        elif index==1:
            self.dpa = attack_dpav1.attack_DPAAESv1()
        elif index==2:
            self.dpa = attack_bayesiancpa.attack_BAYCPAAES()
        elif index==3:
            if attack_cpaiterative:
                reload(attack_cpaiterative)
                self.dpa = attack_cpaiterative.attack_CPAAESit()
            else:
                self.dpa = None
        elif index==4:
            self.dpa = attack_cpa_dpav2wrapper.attack_CPAAES_dpav2wrapper()
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

### Analysis Options
    def showAnalysisWidgets(self, visible=True):
        self.ResultsTable.setVisible(visible)

    def checkAll(self):
        for i in range(0,16):
            self.do[i].setChecked(True)

    def checkNone(self):
        for i in range(0,16):
            self.do[i].setChecked(False)

    def validatePointSB(self):
        self.dontUpdateSB = True
        self.startPointPrint.setValue(self.TraceRangeList[self.pByteSel.currentIndex()][0])
        self.endPointPrint.setValue(self.TraceRangeList[self.pByteSel.currentIndex()][1])
        self.dontUpdateSB = False
            
    def passTrace(self, trace):
        self.trace = trace
        if trace.NumTrace:
            self.startTracePrint.setMaximum(trace.NumTrace)
            self.endTracePrint.setMaximum(trace.NumTrace)
            self.endTracePrint.setValue(trace.NumTrace)

            self.advTraceRange.setMaximum(trace.NumTrace)
            self.advTraceRange.setValue(trace.NumTrace)
            self.advTraceStep.setMaximum(trace.NumTrace)
        
            for i in xrange(0,17):
                self.TraceRangeList[i] = [0, trace.NumPoint]

            self.startPointPrint.setMaximum(trace.NumPoint)
            self.endPointPrint.setMaximum(trace.NumPoint)
            #self.endPointPrint.setValue(trace.NumPoint)

        self.validatePointSB()

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
        self.pge = [255]*16
        
        for bnum in range(0,len(self.do)):
            if self.do[bnum].isChecked():
                self.table.setColumnHidden(bnum, False)
                diffs = self.dpa.getDiff(bnum)

                maxes = np.zeros(256,dtype=[('hyp','i2'),('point','i4'),('value','f8')] )

                for hyp in range(0, 256):
                    #Get maximum value for this hypothesis
                    mvalue = np.amax(np.fabs(diffs[hyp]))
                    mindex = np.amin(np.where(np.fabs(diffs[hyp]) == mvalue))
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

                    if maxes[j]['hyp'] == self.trace.knownkey[bnum]:
                        self.pge[bnum] = j
            else:
                self.table.setColumnHidden(bnum, True)

        self.table.resizeRowsToContents()
        self.table.resizeColumnsToContents()
        self.ResultsTable.setVisible(True)

    def printPGE(self, stats=None, usefloat=False):
        if stats == None:
            stats = self.pge
            
        for j in stats:
            if usefloat:
                print "%3.2f "%j,
            else:
                print "%3d "%j,
        print ""        

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

        prange = range(self.pstart[bnum], self.pend[bnum])
       
        for i in range(0,256):
            canceled = progress.wasCanceled() #_callSync='off'
            if highlightByte:
                if i not in self.highlights[bnum]:
                    self.AnalysisViewDocks[bnum].widget().plot(prange, diffs[i], pen='g')
            else:               
                    self.AnalysisViewDocks[bnum].widget().plot(prange, diffs[i], pen=(i%8,8))
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
                    self.AnalysisViewDocks[bnum].widget().plot(prange, diffs[i], pen=penclr )
            
        self.AnalysisViewDocks[bnum].setVisible(True)

        self.redrawInProgress = False

    def whatColour(self, index):
        if index == 0:
            return 'r'
        else:
            return 'b'       
            
    def attackPushed(self, redraw=True):
        data = []
        textins = []
        textouts = []

        for i in range(0, 16):
            self.redrawRequired[i] = redraw

        self.pend = [self.endPointPrint.value()]*16
        self.pstart = [self.startPointPrint.value()]*16

        for i in range(self.startTracePrint.value(), self.endTracePrint.value()):
            d = self.trace.getTrace(i)
            d = self.preprocess.processOneTrace(d)
            data.append(d[self.TraceRangeList[0][0]:self.TraceRangeList[0][1]])
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

        #Do the DPA attack
        #cProfile.runctx('self.dpa.doDPA(rangeDo, data, textins, textouts, progress, encodedopts=opts)', globals(), locals(), 'Profile.prof')
        #s = pstats.Stats("Profile.prof")
        #s.strip_dirs().sort_stats("time").print_stats()

        self.dpa.setOptions(opts, rangeDo)
   
        self.dpa.addTraces(data, textins, textouts, progress, pointRange=self.TraceRangeList[1:17])

        self.updateTable()

        for i in range(0,16):
            if (self.do[i].isChecked()):
                self.AnalysisViewDocks[i].widget().clear()  
                self.AnalysisViewDocks[i].setVisible(True)
            else:
                self.AnalysisViewDocks[i].widget().clear()  
                self.AnalysisViewDocks[i].setVisible(False)
            self.redrawRequired[i] = True

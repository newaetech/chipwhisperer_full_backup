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


CW_VERSION = "0.001BETA"
GlobalSettings = QSettings('ChipWhisperer', 'chipwhisperer-analyzer')


def noProject():
    msg = QMessageBox()
    msg.setText("No Project Open")
    msg.setInformativeText("Project must be open for this function. Open existing or create new")
    msg.setStandardButtons(QMessageBox.Ok)
    msg.exec_()

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

class Preprocess(QWidget):
    def __init__(self):
        QWidget.__init__(self)
        layout = QVBoxLayout()

        ppGB = QGroupBox("Preprocessing Options")
        ppLayout = QVBoxLayout()
        ppGB.setLayout(ppLayout)

        self.ppEnabled = QCheckBox("Enable Preprocessing")
        ppLayout.addWidget(self.ppEnabled)

        h = QHBoxLayout()
        self.dsEnabled = QCheckBox("Downsampling Enabled")
        h.addWidget(self.dsEnabled)
        self.dsRate = QSpinBox()
        self.dsRate.setMinimum(1)
        h.addWidget(QLabel("Downsample Rate:"))
        h.addWidget(self.dsRate)
        self.dsFilter = QCheckBox("Use Filter for downsample")
        h.addWidget(self.dsFilter)
        h.addStretch()
        ppLayout.addLayout(h)

        h = QHBoxLayout()
        self.normEnabled = QCheckBox("Normalize Traces by Mean and Std-Dev")
        h.addWidget(self.normEnabled)
        ppLayout.addLayout(h)

        layout.addWidget(ppGB)
        self.setLayout(layout)

    def processOneTrace(self, inptrace):
        if self.ppEnabled.isChecked() == False:
            return inptrace

        trace = inptrace

        if self.dsEnabled.isChecked():
            if self.dsFilter.isChecked():
                trace = signal.decimate(trace, self.dsRate.value())
            else:
                trace = trace[range(0, len(trace), self.dsRate.value())]
                
            trace = np.concatenate((trace, np.zeros(len(inptrace)-len(trace))))

        if self.normEnabled.isChecked():
            trace = (trace - np.mean(trace)) / np.std(trace)

        return trace        

class PreviewTab(QWidget):
    def __init__(self, ppWidget=None):
        QWidget.__init__(self)
        layout = QVBoxLayout()

        setupGB = QGroupBox("View Options")
        setupLayout = QVBoxLayout()
        setupGB.setLayout(setupLayout)

        hl = QHBoxLayout()
        pbRedraw = QPushButton("Redraw")
        pbRedraw.clicked.connect(self.redrawPushed)
        hl.addWidget(pbRedraw)

        pbClear = QPushButton("Clear All")
        pbClear.clicked.connect(self.clearPushed)
        hl.addWidget(pbClear)
        hl.addStretch()
        #setupLayout.addLayout(hl)
        

        #hl = QHBoxLayout()
        self.startTracePrint = QSpinBox()        
        self.startTracePrint.setMinimum(0)
        self.endTracePrint = QSpinBox()        
        self.endTracePrint.setMinimum(0)
        self.startPointPrint = QSpinBox()        
        self.startPointPrint.setMinimum(0)
        self.endPointPrint = QSpinBox()        
        self.endPointPrint.setMinimum(0)

        hl.addWidget(QLabel("Traces: "))
        hl.addWidget(self.startTracePrint)
        hl.addWidget(QLabel(" to "))
        hl.addWidget(self.endTracePrint)
        hl.addStretch()
        #setupLayout.addLayout(hl)

        #hl = QHBoxLayout()
        hl.addWidget(QLabel("Points: "))
        hl.addWidget(self.startPointPrint, )
        hl.addWidget(QLabel(" to "))
        hl.addWidget(self.endPointPrint)
        hl.addStretch()
        setupLayout.addLayout(hl)
        
        layout.addWidget(setupGB)

        #Preprocessing Stuff
        if ppWidget:
            self.pp = ppWidget
            layout.addWidget(ppWidget)
        else:
            self.pp = None

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
        if trace.NumTrace:
            self.startTracePrint.setMaximum(trace.NumTrace)
            self.endTracePrint.setMaximum(trace.NumTrace)
            self.endTracePrint.setValue(trace.NumTrace)

            self.startPointPrint.setMaximum(trace.NumPoint)
            self.endPointPrint.setMaximum(trace.NumPoint)
            self.endPointPrint.setValue(trace.NumPoint)

            self.pw.clear()
            data = self.trace.getTrace(0)
            if self.pp:
                data = self.pp.processOneTrace(data)
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
            if self.pp:
                data = self.pp.processOneTrace(data)
            sp = self.startPointPrint.value()
            ep = self.endPointPrint.value()
            self.pw.plot(range(sp,ep),data[sp:ep], pen=(i%8,8))            
            progress.setValue(i)
            if progress.wasCanceled():
                break
        #self.pw.setVisible(True)

    def clearPushed(self):
        self.pw.clear()

class CWProject(object):   
    def __init__(self, parent=None):
        self.folderlocation = None
        self.traceslocation = None
        self.statslocation = None
        self.projectLoaded = False
        self.traces = tracereader_native.tracereader_native()
        self.manageTraces = parent.manageTraces
        self.fname = None

        self.projName = "default"
        self.projDesc = "A ChipWhisperer Project"

    def setFilename(self, fname):
        self.fname = fname
        self.folderlocation = os.path.dirname(fname) + "/" + os.path.splitext(os.path.basename(fname))[0] + "_data"
        self.traceslocation = self.folderlocation + "/traces"
        self.statslocation  = self.folderlocation + "/stats"

        #Create any directories if they don't exist
        if os.path.exists(self.folderlocation) == False:
            os.mkdir(self.folderlocation)

        if os.path.exists(self.traceslocation) == False:
            os.mkdir(self.traceslocation)

        if os.path.exists(self.statslocation) == False:
            os.mkdir(self.statslocation)

    def saveProject(self, fname=None):
        if fname:
            self.setFilename(fname)
        else:
            fname = self.fname

        config = ConfigParser.RawConfigParser()

        config.add_section('ChipWhisperer')
        config.set('ChipWhisperer', 'version', CW_VERSION)

        config.add_section('Project')
        config.set('Project', 'name', self.projName)
        config.set('Project', 'description', self.projDesc)

        #Save trace manager stuff
        self.manageTraces.saveProject(config, fname)

        #if self.traces.tracesSaved == False:
        #    self.traces.saveAllTraces(self.traceslocation)

        with open(fname, 'wb') as configfile:
            config.write(configfile)  

    def saveTraces(self):
        self.traces.saveAllTraces(self.traceslocation)        
        
    def loadProject(self, fname):
        self.setFilename(fname)

        config = ConfigParser.RawConfigParser()
        config.read(self.fname)
        
        self.manageTraces.loadProject(self.fname)
        
        #self.traces.loadAllTraces(self.traceslocation)

        #Open project file & read in everything
        

class traceItem():
    def __init__(self, configfile=None):
        self.enabled = False
        self.mappedRange = None
        self.numTraces = 0
        self.date = ""
        self.prefix = ""
        self.points = 0
        self.targetHW = ""
        self.targetSW = ""
        self.scope = ""
        self.samplerate = 0
        self.yscale = 1
        self.yunits = "digits"
        self.notes = ""

        self.configfile = None

    def loadTrace(self, configfile=None):

        if configfile:
            self.configfile = configfile

        config = ConfigParser.RawConfigParser()
        config.read(self.configfile)
        
        sname = "Trace Config"
        
        self.numTraces = config.getint(sname, 'NumTraces')
        self.date = config.get(sname, 'Date')
        self.prefix = config.get(sname, 'Prefix')
        self.points = config.getint(sname, 'Points')
        self.targetHW = config.get(sname, 'TargetHW')
        self.targetSW = config.get(sname, 'TargetSW')
        self.scope = config.get(sname, 'ScopeName')
        self.samplerate = config.get(sname, 'ScopeSampleRate')
        self.yscale = config.getfloat(sname, 'ScopeYScale')
        self.yunits = config.get(sname, 'ScopeYUnits')
        self.notes = config.get(sname, 'Notes')

    def saveTrace(self, configfile = None):
        if configfile:
            self.configfile = configfile

        config = ConfigParser.RawConfigParser()
        sname = "Trace Config"
        
        config.add_section(sname)
        config.set(sname, 'NumTraces', self.numTraces)
        config.set(sname, 'Date', self.date)
        config.set(sname, 'Prefix', self.prefix)
        config.set(sname, 'Points', self.points)
        config.set(sname, 'TargetHW', self.targetHW)
        config.set(sname, 'TargetSW', self.targetSW)
        config.set(sname, 'ScopeName', self.scope)
        config.set(sname, 'ScopeSampleRate', self.samplerate)
        config.set(sname, 'ScopeYScale', self.yscale)
        config.set(sname, 'ScopeYUnits', self.yunits)
        config.set(sname, 'Notes', self.notes)

        configfile = open(self.configfile, 'wb')
        config.write(configfile)
        configfile.flush()
        configfile.close()

class ImportDPAv3(QDialog):   
    def __init__(self, parent=None):
        super(ImportDPAv3, self).__init__(parent)
        self.parent = parent
        
        self.setWindowTitle("DPA Contest v3 Trace Format Importer")
        layout = QVBoxLayout()
        self.setLayout(layout)

        ### Selection of info.xml to import

        fileL = QHBoxLayout()
        fileL.addWidget(QLabel("info.xml file:"))
        self.fileLE = QLineEdit()
        #self.fileLE.setEnabled(False)
        fileL.addWidget(self.fileLE)
        filePB = QPushButton("Select info.xml")
        filePB.clicked.connect(self.selectInfo)
        fileL.addWidget(filePB)

        setupGB = QGroupBox("File Selection")
        setupGB.setLayout(fileL)
        layout.addWidget(setupGB)

        ### Settings found in info.xml, can be changed by user

        self.LETraces = QLineEdit()
        self.LETraces.setEnabled(False)

        self.LEPoints = QLineEdit()
        self.LEPoints.setEnabled(False)

        self.LEScope = QLineEdit()
        self.LETargetHW = QLineEdit()
        self.LETargetSW = QLineEdit()
        self.LEDate = QLineEdit()

        self.LENotes = QLineEdit()

        settingsL = QGridLayout()
        settingsL.addWidget(QLabel("Traces:"), 0, 0)
        settingsL.addWidget(self.LETraces, 0, 1)

        settingsL.addWidget(QLabel("Points:"), 0, 2)
        settingsL.addWidget(self.LEPoints, 0, 3)

        settingsL.addWidget(QLabel("Date:"), 1, 0)
        settingsL.addWidget(self.LEDate, 1, 1)

        settingsL.addWidget(QLabel("TargetHW:"), 2, 0)
        settingsL.addWidget(self.LETargetHW, 2, 1)

        settingsL.addWidget(QLabel("TargetSW:"), 2, 2)
        settingsL.addWidget(self.LETargetSW, 2, 3)

        settingsL.addWidget(QLabel("ScopeName:"), 3, 0)
        settingsL.addWidget(self.LEScope, 3, 1)

        settingsL.addWidget(QLabel("Notes:"), 4, 0)
        settingsL.addWidget(self.LENotes, 4, 1)

        settingsGB = QGroupBox("Imported Parameters")
        settingsGB.setLayout(settingsL)
        layout.addWidget(settingsGB)


        ### Import Options
        importL = QGridLayout()

        importL.addWidget(QLabel("Target Dir:"), 0, 0)
        targetDirLE = QLineEdit()
        targetDirLE.setText("./traces/")
        targetDirLE.setEnabled(False)
        importL.addWidget(targetDirLE, 0, 1)

        self.prefixDirLE = QLineEdit()
        self.prefixDirLE.setText("001")
        importL.addWidget(QLabel("Prefix:"), 0, 2)
        importL.addWidget(self.prefixDirLE, 0, 3)
        
        importPB = QPushButton("Run Import")
        importPB.clicked.connect(self.doImport)
        importL.addWidget(importPB, 4, 0)

        importGB = QGroupBox("Import Options")
        importGB.setLayout(importL)
        layout.addWidget(importGB)

    def getTraceCfgFile(self):
        return self.parent.parent.cwp.traceslocation + "/" + "config_" + self.prefixDirLE.text() + ".cfg"
       
    def selectInfo(self):
        fname, _ = QFileDialog.getOpenFileName(self, 'Open file',GlobalSettings.value("dpav3_last_file"),'info.xml')
        if fname:
            GlobalSettings.setValue("dpav3_last_file", fname)
            trimport = tracereader_dpacontestv3.tracereader_dpacontestv3()
            trimport.loadInfo(os.path.dirname(fname))
            self.trimport = trimport

            self.fileLE.setText(fname)

            self.LEPoints.setText("%d"%trimport.NumPoint)
            self.LETraces.setText("%d"%trimport.NumTrace)

            self.LEScope.setText(trimport.xmlroot.findall('Instrument')[0].text)
            self.LEDate.setText(trimport.xmlroot.findall('Date')[0].text)
            self.LETargetHW.setText(trimport.xmlroot.findall('Module')[0].text)
            self.LETargetSW.setText(trimport.xmlroot.findall('Cipher')[0].text)

            #May not have notes
            try:
                self.LENotes.setText(trimport.xmlroot.findall('Notes')[0].text)
            except:
                pass

            #You really need a project file open, but let user experiment without it...
            if self.parent.parent.cwp:
                self.validatePrefix()

    def validatePrefix(self, change=True):              
        tracedir = self.parent.parent.cwp.traceslocation
        configOK = False

        while configOK == False:
            if os.path.exists(self.getTraceCfgFile()):
                if change:
                    try:
                        cur = int(self.prefixDirLE.text())
                        cur = cur + 1
                        self.prefixDirLE.setText("%03d"%cur)
                    except:
                        change = False
                        
                if change == False:
                        msg = QMessageBox()
                        msg.setText("Settings Error")
                        msg.setInformativeText("Prefix already in use, try another prefix")
                        msg.setStandardButtons(QMessageBox.Ok)
                        msg.exec_()
                        return False
            else:
                configOK = True

        return True

    def doImport(self):
        if self.parent.parent.cwp == None:
            noProject()
            return

        if self.validatePrefix(False) == False:
            return
        
        ti = traceItem()
        ti.numTraces = int(self.LETraces.text())
        ti.points = int(self.LEPoints.text())
        ti.date = self.LEDate.text()
        ti.scope = self.LEScope.text()
        ti.targetHW = self.LETargetHW.text()
        ti.targetSW = self.LETargetSW.text()
        ti.notes = self.LENotes.text()

        msg = QMessageBox()
        msg.setText("Message from ChipWhisperer")
        msg.setInformativeText("Importing, could take a while. This message dissappears when done")
        msg.setStandardButtons(0)
        msg.show()
        self.trimport.loadAllTraces()
    
        tmp = tracereader_native.tracereader_native()
        tmp.copyTo(self.trimport)
        msg.close()

        #Save file automatically
        tmp.saveAllTraces(self.parent.parent.cwp.traceslocation + "/", prefix=self.prefixDirLE.text() + "_")

        ti.prefix = self.prefixDirLE.text() + "_"
        
        ti.saveTrace(self.getTraceCfgFile())     
        self.close()

class ManageTraces(QDialog):
    secName = "Trace Management"
    def __init__(self, parent=None):
        super(ManageTraces, self).__init__(parent)
        self.parent = parent
        layout = QVBoxLayout()

        self.table = QTableWidget(0, 11)
        self.table.setHorizontalHeaderLabels(["Enabled", "Mapped Range", "Trace Num", "Points", "Date Captured", "File", "Target HW", "Target SW", "Scope", "Sample Rate", "Notes"])
       
        layout.addWidget(self.table)

        temp = QPushButton("Add Blank")
        temp.clicked.connect(self.addRow)
        layout.addWidget(temp)

        importDPAv3 = QPushButton("Import DPAv3")
        importDPAv3.clicked.connect(self.importDPAv3)
        layout.addWidget(importDPAv3)

        importExisting = QPushButton("Add Existing")
        importExisting.clicked.connect(self.importExisting)
        layout.addWidget(importExisting)

        # Set dialog layout
        self.setLayout(layout)

        self.setWindowTitle("Trace Management")

        self.newProject()

    def newProject(self):
        self.traceList = []
        return

    def saveProject(self, config, configfilename):
        config.add_section(self.secName)
        for indx, t in enumerate(self.traceList):
            config.set(self.secName, 'tracefile%d'%indx, os.path.relpath(t.configfile, os.path.split(configfilename)[0]))
            config.set(self.secName, 'enabled%d'%indx, str(t.enabled))

    def loadProject(self, configfilename):
        config = ConfigParser.RawConfigParser()
        config.read(configfilename)
        alltraces = config.items(self.secName)

        self.newProject()

        fdir = os.path.split(configfilename)[0] + "/"

        for t in alltraces:
            if t[0].startswith("tracefile"):
                fname = fdir + t[1]
                print "Opening %s"%fname
                ti = traceItem()
                ti.loadTrace(fname)
                self.traceList.append(ti)
                self.addRow(ti)

            if t[0].startswith("enabled"):
                tnum = re.findall(r'[0-9]+', t[0])
                self.table.cellWidget(int(tnum[0]), self.findCol("Enabled")).setChecked(t[1] == "True")

    def findCol(self, name):
        """ Function is a hack/cheat to deal with movable headers if they become enabled """
        cols = self.table.columnCount()

        for i in range(0, cols):
            if self.table.horizontalHeaderItem(i).text() == name:
                return i

        raise ValueError("findCol argument not in table: %s"%name)

        
    def addRow(self, trace=None, location=None):
        if location == None:
            location = self.table.rowCount()
            
        self.table.insertRow(location)
        row = self.table.rowCount()-1
        cb = QCheckBox()
        cb.clicked.connect(self.validateTable)
        self.table.setCellWidget(row, self.findCol("Enabled"), cb)

        if trace:
            temp = QTableWidgetItem("%d"%trace.numTraces)
            temp.setFlags(temp.flags() & ~Qt.ItemIsEditable)
            self.table.setItem(row, self.findCol("Trace Num"), temp)
            self.table.setItem(row, self.findCol("Date Captured"), QTableWidgetItem("%s"%trace.date))
            self.table.setItem(row, self.findCol("File"), QTableWidgetItem("%s"%trace.configfile))
            temp = QTableWidgetItem("%d"%trace.points)
            temp.setFlags(temp.flags() & ~Qt.ItemIsEditable)
            self.table.setItem(row, self.findCol("Points"), temp)
            self.table.setItem(row, self.findCol("Target HW"), QTableWidgetItem("%s"%trace.targetHW))
            self.table.setItem(row, self.findCol("Target SW"), QTableWidgetItem("%s"%trace.targetSW))
            self.table.setItem(row, self.findCol("Scope"), QTableWidgetItem("%s"%trace.scope))
            self.table.setItem(row, self.findCol("Sample Rate"), QTableWidgetItem("%s"%trace.samplerate))
            self.table.setItem(row, self.findCol("Notes"), QTableWidgetItem("%s"%trace.notes))

        self.validateTable()

    def validateTable(self):
        startTrace = 0
        for i in range(0, self.table.rowCount()):
            if self.table.cellWidget(i, self.findCol("Enabled")).isChecked():
                self.traceList[i].enabled = True
                tlen = self.traceList[i].numTraces
                self.traceList[i].mappedRange = [startTrace, startTrace+tlen]
                self.table.setItem(i, self.findCol("Mapped Range"), QTableWidgetItem("%d-%d"%(startTrace, startTrace+tlen)))
                startTrace = startTrace + tlen + 1
            else:
                self.traceList[i].enabled = False
                self.traceList[i].mappedRange = None
                self.table.setItem(i, self.findCol("Mapped Range"), QTableWidgetItem(""))
        
    def importDPAv3(self):
        imp = ImportDPAv3(self)
        imp.exec_()
        self.importExisting(imp.getTraceCfgFile())

    def importExisting(self, fname=None):
        if fname == None:
            fname, _ = QFileDialog.getOpenFileName(self, 'Open file',GlobalSettings.value("trace_last_file"),'*.cfg')
            if fname:
                GlobalSettings.setValue("trace_last_file", fname)

        if fname:
            #Add to file list
            ti = traceItem()
            ti.loadTrace(fname)
            self.traceList.append(ti)
            self.addRow(ti)

class MainChip(QMainWindow):
    MaxRecentFiles = 4
    
    def __init__(self):       
        super(MainChip, self).__init__()
        self.cwp = None
        pg.setConfigOption('background', 'w')
        pg.setConfigOption('foreground', 'k')
        self.initUI()

        #Settings
        self.restoreGeometry(GlobalSettings.value("geometry"));
        
    def closeEvent(self, event):
        GlobalSettings.setValue("geometry", self.saveGeometry());
        QMainWindow.closeEvent(self, event)

    def createActions(self):
#        self.importAct = QAction(QIcon('open.png'), '&Import Input Files', self,
#                               statusTip='Import Input Files (waveform, etc)',
#                               triggered=self.importFile)

        self.openAct = QAction(QIcon('open.png'), '&Open Project', self,
                               shortcut=QKeySequence.Open,
                               statusTip='Open Project File',
                               triggered=self.openProject)

        self.saveAct = QAction(QIcon('save.png'), '&Save Project', self,
                               shortcut=QKeySequence.Save,
                               statusTip='Save current project to Disk',
                               triggered=self.saveProject)

        self.newAct = QAction(QIcon('new.png'), '&New Project', self,
                               shortcut=QKeySequence.New,
                               statusTip='Create new Project',
                               triggered=self.newProject)

        for i in range(MainChip.MaxRecentFiles):
            self.recentFileActs.append(QAction(self, visible=False, triggered=self.openRecentFile))

        self.traceManageAct = QAction('&Manage', self, statusTip='Add/Remove Traces from Project', triggered=self.manageTraces.show)

    def createMenus(self):
        self.fileMenu= self.menuBar().addMenu("&File")
        self.fileMenu.addAction(self.newAct)
        self.fileMenu.addAction(self.openAct)
        self.fileMenu.addAction(self.saveAct)
#        self.fileMenu.addAction(self.importAct)
        self.separatorAct = self.fileMenu.addSeparator()
        for i in range(MainChip.MaxRecentFiles):
            self.fileMenu.addAction(self.recentFileActs[i])

        self.tracesMenu= self.menuBar().addMenu("&Traces")
        self.tracesMenu.addAction(self.traceManageAct)
        
            
    def initUI(self):        
        self.statusBar()
        self.setWindowTitle("Chip Whisperer: Analyzer")
        self.setWindowIcon(QIcon("../common/cwicon.png"))

        self.manageTraces = ManageTraces(self)

        self.recentFileActs = []
        self.createActions()
        self.createMenus()

        self.updateRecentFileActions()
        
        #self.setGeometry(300, 300, 350, 300)
        self.show()

        #Preprocess Settings
        self.preprocess = Preprocess()

        #Preview Tab Setup
        self.preview = PreviewTab(self.preprocess)

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
        files = GlobalSettings.value('recentFileList', [])

        try:
            files.remove(fname)
        except ValueError:
            pass

        files.insert(0, fname)
        del files[MainChip.MaxRecentFiles:]

        GlobalSettings.setValue('recentFileList', files)
        for widget in QApplication.topLevelWidgets():
            if isinstance(widget, MainChip):
                widget.updateRecentFileActions()

    def updateRecentFileActions(self):
        files = GlobalSettings.value('recentFileList')
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

    def openProject(self, fname=None):
        self.cwp = CWProject(self)

        if fname == None:
            fname, _ = QFileDialog.getOpenFileName(self, 'Open file','.','*.cwp')
        self.setCurrentFile(fname)
        self.cwp.loadProject(fname)

        self.preview.passTrace(self.cwp.traces)
        self.dpa.passTrace(self.cwp.traces)

    def saveProject(self):
        self.cwp.saveProject()
        self.setCurrentFile(self.cwp.fname)

    def newProject(self):
        self.cwp = CWProject(self)
        fname, _ = QFileDialog.getSaveFileName(self, 'New Project File', '.', '*.cwp')
        self.cwp.setFilename(fname)
                
    def openRecentFile(self):
        action = self.sender()
        if action:
            self.openProject(action.data())        

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

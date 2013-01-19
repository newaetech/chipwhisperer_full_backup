#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2013, Colin O'Flynn <coflynn@newae.com>
# All rights reserved.
#
# Find this and more at newae.com - this file is part of the chipwhisperer
# project, http://www.assembla.com/spaces/chipwhisperer
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#    * Redistributions of source code must retain the above copyright
#      notice, this list of conditions and the following disclaimer.
#    * Redistributions in binary form must reproduce the above copyright
#     notice, this list of conditions and the following disclaimer in the
#      documentation and/or other materials provided with the distribution.
#    * Neither the name of the authors nor the
#      names of its contributors may be used to endorse or promote products
#      derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL COLIN O'FLYNN BE LIABLE FOR ANY
# DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#

import sys
from PySide.QtCore import *
from PySide.QtGui import *
import os.path
sys.path.append('../common')
import pyqtgraph as pg
import tracereader_dpacontestv3
import attack_dpav1
import numpy


class DPATab(QWidget):
    def __init__(self, previewtab, MainWindow=None):
        QWidget.__init__(self)
        layout = QVBoxLayout()
        self.dpa = attack_dpav1.attack_DPAAESv1()

        setupGB = QGroupBox("Analysis Options")
        setupLayout = QGridLayout()
        setupGB.setLayout(setupLayout)

        pbAttack = QPushButton("Attack!")
        pbAttack.clicked.connect(self.attackPushed)
        setupLayout.addWidget(pbAttack, 0, 0)

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

        btnAll = QPushButton("All")
        btnAll.clicked.connect(self.checkAll)
        btnNone = QPushButton("None")
        btnNone.clicked.connect(self.checkNone)
        setupLayout.addWidget(btnAll, 0, 6)
        setupLayout.addWidget(btnNone,0, 7)
        self.do = []
        for i in range(0,16):
            self.do.append(QCheckBox("%2d"%i))
            self.do[i].setChecked(True)
            setupLayout.addWidget(self.do[i], i/4+1, i%4+8)            
        
        layout.addWidget(setupGB)

        self.table = QTableWidget(256, 16)
        self.ResultsTable = QDockWidget("Results Table")
        self.ResultsTable.setAllowedAreas(Qt.BottomDockWidgetArea | Qt.RightDockWidgetArea| Qt.LeftDockWidgetArea)
        self.ResultsTable.setWidget(self.table)
        MainWindow.addDockWidget(Qt.RightDockWidgetArea, self.ResultsTable)
        self.ResultsTable.setVisible(False)
        
        viewGB = QGroupBox("View Options")
        viewLayout = QGridLayout()
        viewGB.setLayout(viewLayout)

        layout.addWidget(viewGB)
       
        #Setup trace viewer
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
        self.setLayout(layout)

        self.redrawRequired = []
        for i in range(0,16):
            self.redrawRequired.append(True)

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

    def updateTable(self):

        for bnum in range(0,len(self.do)):
            if self.do[bnum].isChecked():
                self.table.setColumnHidden(bnum, False)
                diffs = self.dpa.getDiff(bnum)

                maxes = numpy.zeros(256,dtype=[('hyp','i2'),('point','i4'),('value','f8')] )

                for hyp in range(0, 256):
                    #Get maximum value for this hypothesis
                    mvalue = max(numpy.fabs(diffs[hyp]))
                    mindex = min(numpy.where(numpy.fabs(diffs[hyp]) == mvalue))
                    maxes[hyp] = (hyp,mindex,mvalue)

                maxes.sort(order='value')
                maxes = maxes[::-1]
            
                for j in range(0,256):
                    self.table.setItem(j,bnum,QTableWidgetItem("%02x=%f"%(maxes[j]['hyp'],maxes[j]['value'])))
            else:
                self.table.setColumnHidden(bnum, True)

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

    def redrawOneResult(self, bnum, highlightByte=True):
        if (self.redrawRequired[bnum] == False):
            return

        self.redrawRequired[bnum] = False
        
        diffs = self.dpa.getDiff(bnum)
        #Do Redraw
        progress = QProgressDialog("Redrawing", "Abort", 0, 100)
        progress.setWindowModality(Qt.WindowModal)
        progress.setMinimumDuration(1000)
        progress.setMinimum(0)
        progress.setMaximum(256)        

        self.AnalysisViewDocks[bnum].widget().clear
        self.AnalysisViewDocks[bnum].setVisible(False)
       
        for i in range(0,256):
            if highlightByte:
                if self.trace.knownkey[bnum] != i:
                    self.AnalysisViewDocks[bnum].widget().plot(diffs[i], pen='g')
            else:
                    self.AnalysisViewDocks[bnum].widget().plot(diffs[i], pen=(i%8,8))
                   
            progress.setValue(i)
            if progress.wasCanceled():
                break

        #Plot the highlighted byte on top
        if highlightByte:
            self.AnalysisViewDocks[bnum].widget().plot(diffs[bnum], pen='r')
            
        self.AnalysisViewDocks[bnum].setVisible(True)
            
    def attackPushed(self):
        data = []
        texts = []

        for i in range(0, 16):
            self.redrawRequired[i] = True

        for i in range(self.startTracePrint.value(), self.endTracePrint.value()):
            data.append(self.trace.getTrace(i)[self.startPointPrint.value():self.endPointPrint.value()])
            texts.append(self.trace.getTextin(i))          

        rangeDo = []
        for i in range(0,len(self.do)):
            if self.do[i].isChecked():
                rangeDo.append(i)
        self.rangeDo = rangeDo
        
        progress = QProgressDialog("Analyzing", "Abort", 0, 100)
        progress.setWindowModality(Qt.WindowModal)
        progress.setMinimumDuration(1000)
        self.dpa.doDPA(0, rangeDo, data, texts, progress)

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

    def redrawPushed(self):
        progress = QProgressDialog("Redrawing", "Abort", 0, 100)
        progress.setWindowModality(Qt.WindowModal)
        progress.setMinimumDuration(1000)
        progress.setMinimum(self.startTracePrint.value())
        progress.setMaximum(self.endTracePrint.value())        

        self.pw.setVisible(False)
        for i in range(self.startTracePrint.value(), self.endTracePrint.value()):
            data = self.trace.getTrace(i)
            self.pw.plot(data[self.startPointPrint.value():self.endPointPrint.value()], pen=(i%8,8))            
            progress.setValue(i)
            if progress.wasCanceled():
                break
        self.pw.setVisible(True)

class MainChip(QMainWindow):
    MaxRecentFiles = 4
    
    def __init__(self):
        super(MainChip, self).__init__()        
        self.trace = tracereader_dpacontestv3.tracereader_dpacontestv3()
        self.initUI()

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
        self.dpa = DPATab(self.preview, self)

        #Tab Widget Setup
        self.tw = QTabWidget()
        self.tw.currentChanged.connect(self.curTabChange) 
        self.tw.addTab(self.preview, "&Trace View")
        self.tw.addTab(self.dpa, "&DPA")
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
#        try:
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

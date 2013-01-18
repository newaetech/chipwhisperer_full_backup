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
import pysidegraph
import tracereader_dpacontestv3
import attack_dpav1


class DPATab(QWidget):
    def __init__(self, previewtab):
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
        
        layout.addWidget(setupGB)

        self.preview = pysidegraph.pysideGraph("Diff View")
        layout.addWidget(self.preview.getWidget())
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
        return
    #self.preview

    def attackPushed(self):
        data = []
        texts = []

        for i in range(self.startTracePrint.value(), self.endTracePrint.value()):
            data.append(self.trace.getTrace(i)[self.startPointPrint.value():self.endPointPrint.value()])
            texts.append(self.trace.getTextin(i))          
        
        progress = QProgressDialog("Analyzing", "Abort", 0, 100)
        progress.setWindowModality(Qt.WindowModal)
        progress.setMinimumDuration(1000)
        self.dpa.doDPA(0, range(0,16), data, texts, progress)

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

        self.preview = pysidegraph.pysideGraph("Trace View")
        layout.addWidget(self.preview.getWidget())
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
        hold=False

        progress = QProgressDialog("Redrawing", "Abort", 0, 100)
        progress.setWindowModality(Qt.WindowModal)
        progress.setMinimumDuration(1000)
        progress.setMinimum(self.startTracePrint.value())
        progress.setMaximum(self.endTracePrint.value())
        
        for i in range(self.startTracePrint.value(), self.endTracePrint.value()):
            data = self.trace.getTrace(i)
            self.preview.updateData(data[self.startPointPrint.value():self.endPointPrint.value()], holdOn=hold)
            hold=True
            progress.setValue(i)
            if progress.wasCanceled():
                break

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
        self.dpa = DPATab(self.preview)

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

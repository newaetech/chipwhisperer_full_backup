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

try:
    import pyqtgraph as pg
    import pyqtgraph.multiprocess as mp
except ImportError:
    print "ERROR: PyQtGraph is required for this program"
    sys.exit()
    
import tracereader_dpacontestv3
import tracereader_native
import re

try:
    import numpy as np
    from scipy import signal
except ImportError:
    print "ERROR: numpy & scipy are required for this program"

#For profiling support (not 100% needed)
import pstats, cProfile

import ConfigParser

from cwanalyzer_common import CW_VERSION
from cwanalyzer_common import GlobalSettings
from cwanalyzer_common import noProject
from cwanalyzer_patab import PATab

from cwanalyzer_traces import ManageTracesDialog

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
        self.traces = self.manageTraces.iface
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

    def checkProject(self):
        if self.fname == None:
            return

        self.manageTraces.checkProject()

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

        with open(fname, 'wb') as configfile:
            config.write(configfile)  
        
    def loadProject(self, fname):
        self.setFilename(fname)

        config = ConfigParser.RawConfigParser()
        config.read(self.fname)

        #Open project file & read in everything
        self.manageTraces.loadProject(self.fname)

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

        self.manageTraces = ManageTracesDialog(self)

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
        self.closeCurrentProject()
        self.cwp = CWProject(self)

        if fname == None:
            fname, _ = QFileDialog.getOpenFileName(self, 'Open file','.','*.cwp')
        self.setCurrentFile(fname)
        self.cwp.loadProject(fname)

        #TODO: improve?
        self.manageTraces.validateTable()
        self.preview.passTrace(self.cwp.traces)
        self.dpa.passTrace(self.cwp.traces)

    def saveProject(self):
        self.cwp.saveProject()
        self.setCurrentFile(self.cwp.fname)

    def newProject(self):
        self.closeCurrentProject()
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

    def closeCurrentProject(self):
        if self.cwp:
            self.cwp.checkProject()
                                                       
def main():
    
    app = QApplication(sys.argv)
    ex = MainChip()
    sys.exit(app.exec_())


if __name__ == '__main__':
    main()

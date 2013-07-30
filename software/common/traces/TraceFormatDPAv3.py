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
sys.path.append('../.')
import pyqtgraph as pg
import pyqtgraph.multiprocess as mp
import tracereader_dpacontestv3
import tracereader_native
import re
import numpy as np
from scipy import signal

#For profiling support (not 100% needed)
import pstats, cProfile

from TraceContainer import TraceContainer

class ImportDPAv3Dialog(QDialog):   
    def __init__(self, parent=None):
        super(ImportDPAv3Dialog, self).__init__(parent)
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
        fname, _ = QFileDialog.getOpenFileName(self, 'Open file',QSettings().value("dpav3_last_file"),'info.xml')
        if fname:
            QSettings().setValue("dpav3_last_file", fname)
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
        """
        Import traces from DPAv3 Contest Format into internal native ChipWhisperer format.
        
        NOTE: This won't actually be required in the future... for now you have to convert
        into the CW format, but the idea is you can just read them from disk in the DPAv3 format.
        """
        
        if self.parent.parent.cwp == None:
            print "No project open for import? Hmm..."
            return

        if self.validatePrefix(False) == False:
            return
        
        ti = TraceContainer()
        ti.numTraces = int(self.LETraces.text())
        ti.numPoints = int(self.LEPoints.text())
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

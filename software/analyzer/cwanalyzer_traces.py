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
import re
import numpy as np
from scipy import signal

#For profiling support (not 100% needed)
import pstats, cProfile

import ConfigParser

from cwanalyzer_common import CW_VERSION
from cwanalyzer_common import GlobalSettings
from cwanalyzer_common import noProject

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

#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2013-2014, NewAE Technology Inc
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
import os.path
import shutil
import glob
from PySide.QtCore import *
from PySide.QtGui import *
import ConfigParser
import pstats, cProfile #For profiling support (not 100% needed)
import chipwhisperer.common.traces.TraceContainerConfig
import chipwhisperer.common.traces.TraceContainerNative
from chipwhisperer.common.traces.TraceContainerDPAv3 import ImportDPAv3Dialog
from TraceManagerImport import TraceManagerImport
from chipwhisperer.common.api.TraceManager import TraceManager

sys.path.append('../common')

class TraceManagerDialog(QDialog, TraceManager):
    """Manages traces associated with some project"""

    def __init__(self, parent):
        QDialog.__init__(self, parent)
        TraceManager.__init__(self)
        self.parent = parent
        #This module is interface for others
        layout = QVBoxLayout()

        #Get labels in use
        exampleConfig = chipwhisperer.common.traces.TraceContainerConfig.TraceContainerConfig()
        attrs = exampleConfig.attrHeaderValues()
        attrHeaders = [i["header"] for i in attrs]
        attrHeaders.insert(0, "Mapped Range")
        attrHeaders.insert(0, "Enabled")
        self.table = QTableWidget(0, len(attrHeaders))
        self.table.setHorizontalHeaderLabels(attrHeaders)
        layout.setContentsMargins(5,5,5,5)
        layout.setSpacing(5)
        layout.addWidget(self.table)

        #temp = QPushButton("Add Blank")
        #temp.clicked.connect(self.addRow)
        #layout.addWidget(temp)

        #importDPAv3 = QPushButton("Import DPAv3")
        #importDPAv3.clicked.connect(self.importDPAv3)
        #layout.addWidget(importDPAv3)

        #copyExisting = QPushButton("Copy Existing and Add")
        #copyExisting.clicked.connect(self.copyExisting)
        #layout.addWidget(copyExisting)

        importExisting = QPushButton("Add Reference to Existing")
        importExisting.clicked.connect(self.importExisting)
        layout.addWidget(importExisting)

        # Set dialog layout
        self.setLayout(layout)
        self.setWindowTitle("Trace Management")
        self.resize(800, 300)

    def newProject(self):
        super(TraceManagerDialog,self).newProject()
        self.table.clearContents()
        self.table.setRowCount(0)

    def checkProject(self, ask=True):
        #Check trace attributes
        for i in range(0, self.table.rowCount()):
            self.traceList[i].checkTrace()

        #Check out config

    def loadProject(self, configfilename):
        super(TraceManagerDialog, self).loadProject(configfilename)
        for indx, t in enumerate(self.traceList):
            self.addRow(t)
        self.validateTable()

    def findCol(self, name):
        """ Function is a hack/cheat to deal with movable headers if they become enabled """
        for i in range(0, self.table.columnCount()):
            if self.table.horizontalHeaderItem(i).text() == name:
                return i

        raise ValueError("findCol argument not in table: %s" % name)

    def addRow(self, trace=None, location=None):
        if location == None:
            location = self.table.rowCount()

        self.table.insertRow(location)
        row = self.table.rowCount()-1
        cb = QCheckBox()
        cb.setChecked(trace.enabled)
        cb.clicked.connect(self.validateTable)
        self.table.setCellWidget(row, self.findCol("Enabled"), cb)
        if trace:
            for t in trace.config.attrHeaderValues():
                try:
                    col = self.findCol(t["header"])
                    wid = QTableWidgetItem("%s"%trace.config.attr(t["name"]))
                    attrDict = trace.config.attrDict(t["name"])
                    try:
                        isEditable = attrDict["editable"]
                    except KeyError:
                        isEditable = False

                    if isEditable == False:
                        wid.setFlags(wid.flags() & ~Qt.ItemIsEditable)

                    self.table.setItem(row, col, wid)

                except ValueError:
                    pass

    def validateTable(self):
        startTrace = 0
        for i in range(0, self.table.rowCount()):
            if self.table.cellWidget(i, self.findCol("Enabled")).isChecked():
                if hasattr(self.traceList[i], 'enabled') and self.traceList[i].enabled == False:
                    self.dirty.setValue(True)
                self.traceList[i].enabled = True
                tlen = self.traceList[i].numTraces()
                self.traceList[i].mappedRange = [startTrace, startTrace+tlen-1]
                self.table.setItem(i, self.findCol("Mapped Range"), QTableWidgetItem("%d-%d"%(startTrace, startTrace+tlen-1)))
                startTrace = startTrace + tlen

                if self.traceList[i].traces is None:
                    if self.traceList[i].config.configFilename() is not None:
                        path = os.path.split(self.traceList[i].config.configFilename())[0]
                        pref = self.traceList[i].config.attr("prefix")
                    else:
                        path = None
                        pref = None
                    self.traceList[i].directory = path
                    self.traceList[i].prefix = pref
                    # self.traceList[i].loadAllTraces(path, pref)

            else:
                if hasattr(self.traceList[i], 'enabled') and self.traceList[i].enabled == True:
                    self.dirty.setValue(True)
                self.traceList[i].enabled = False
                self.traceList[i].mappedRange = None
                self.table.setItem(i, self.findCol("Mapped Range"), QTableWidgetItem(""))

        self.updateTraces()
        #self.updatePreview()
        self.table.resizeColumnsToContents()
        self.table.resizeRowsToContents()

    def importDPAv3(self):
        imp = ImportDPAv3Dialog(self)
        imp.exec_()
        self.importExisting(imp.getTraceCfgFile())
        #self.updatePreview()

    def append(self, ti):
        super(TraceManagerDialog,self).append(ti)
        self.addRow(ti)
        self.validateTable()

    def importExisting(self, fname=None):
        tmi = TraceManagerImport(self)
        tmi.exec_()

        if tmi.getTrace() is not None:
            tmi.updateConfigData()
            self.append(tmi.getTrace())

    def copyExisting(self, fname=None):
        if fname == None:
            fname, _ = QFileDialog.getOpenFileName(self, 'Open file', QSettings().value("trace_last_file"),'*.cfg')
            if fname:
                QSettings().setValue("trace_last_file", fname)

        if fname:
            #Get our project directory
            targetdir = self.parent.cwp.traceslocation + "/"
            cfgname = os.path.split(fname)[1]
            srcdir = os.path.split(fname)[0] + "/"
            newcfgname = targetdir + cfgname

            #Get prefix from config file
            config = ConfigParser.RawConfigParser()
            config.read(fname)
            prefix = config.get("Trace Config", "prefix")
            newprefix = prefix

            #Check if we'll be overwriting things
            newprefix = prefix
            while(os.path.exists(newcfgname)):
                npstripped = newprefix.rstrip("_")
                newprefix = QInputDialog.getText(self, "Enter New Prefix", "Project already contains trace with\nprefix %s. Please enter new prefix:"%npstripped, text=npstripped)
                if newprefix[1] == True:
                    newcfgname = targetdir + cfgname.replace(prefix.rstrip("_"), newprefix[0])
                    newprefix = newprefix[0] + "_"
                else:
                    return

            #Change prefix in config file & write new one
            config.set("Trace Config", "prefix", newprefix)
            configfile = open(newcfgname, 'wb')
            config.write(configfile)
            configfile.close()

            # Copy anything with same prefix, changing as
            for filename in glob.glob(os.path.join(srcdir, '%s*'%prefix)):
                if os.path.isfile(filename):
                    targetfile = os.path.split(filename)[1].replace(prefix, newprefix)
                    shutil.copy(filename, targetdir + targetfile)

            #Add new trace to file list
            ti = TraceFormatNative()
            ti.config.loadTrace(newcfgname)
            self.append(ti)



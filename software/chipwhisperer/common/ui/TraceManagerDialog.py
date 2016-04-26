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

import os.path
import shutil
import glob
from functools import partial
from PySide.QtCore import *
from PySide.QtGui import *
import chipwhisperer.common.utils.qt_tweaks as QtFixes
import ConfigParser
import pstats, cProfile #For profiling support (not 100% needed)
import chipwhisperer.common.traces._cfgfile
import chipwhisperer.common.traces.TraceContainerNative
from chipwhisperer.common.traces.TraceContainerDPAv3 import ImportDPAv3Dialog
from TraceManagerImport import TraceManagerImport


class TraceManagerDialog(QtFixes.QDialog):
    """Manages traces associated with some project's trace manager."""

    def __init__(self, parent):
        QDialog.__init__(self, parent)
        self.setWindowTitle("Traces Management")
        layout = QVBoxLayout()

        #Get labels in use
        self.attrs = chipwhisperer.common.traces._cfgfile.TraceContainerConfig().attrHeaderValues()
        attrHeaders = [i["header"] for i in self.attrs]
        attrHeaders.insert(0, "Options")
        attrHeaders.insert(1, "Mapped Range")
        self.table = QTableWidget(0, len(attrHeaders))
        self.table.setHorizontalHeaderLabels(attrHeaders)
        layout.setContentsMargins(5,5,5,5)
        layout.setSpacing(5)
        layout.addWidget(QLabel("Trace Sets:"))
        layout.addWidget(self.table)
        layout.addWidget(QLabel("* Cell editions are saved immediately"))

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
        self.resize(950, 400)
        self.table.resizeColumnsToContents()
        self.table.resizeRowsToContents()

    def setTraceManager(self, traceManager):
        """Defines the current observed TraceManager."""
        self._traceManager = traceManager
        self._traceManager.tracesChanged.connect(self.refresh)

    def checkProject(self, ask=True):
        """Checks trace attributes."""
        for i in range(0, self.table.rowCount()):
            self._traceManager.traceSets[i].checkTrace()

    def refresh(self):
        """Populates the table."""
        self.disconnect(self.table, SIGNAL("cellChanged(int, int)"), self.cellChanged)
        self.table.setRowCount(len(self._traceManager.traceSets))
        for p, t in enumerate(self._traceManager.traceSets):
            if not self.table.cellWidget(p, 0): # check if the option buttons already exist
                self.table.setVerticalHeaderItem(p, QTableWidgetItem("%d" % p))
                cb = QCheckBox()
                cb.setChecked(t.enabled)
                cb.clicked.connect(partial(self.changeTraceStatus, p))
                cb.setToolTip("Enable/disable trace set")
                tb = QPushButton("x")
                tb.setFixedSize(14,14)
                tb.setToolTip("Remove trace set")
                tb.clicked.connect(partial(self.removeTrace, p))
                tmp = QWidget()
                pLayout = QHBoxLayout(tmp)
                pLayout.addWidget(cb)
                pLayout.addWidget(tb)
                pLayout.setAlignment(Qt.AlignHCenter)
                pLayout.setContentsMargins(0,0,0,0)
                self.table.setCellWidget(p, self.findCol("Options"), tmp)
            if t:
                rangeWidget = QTableWidgetItem(("%d-%d" % (t.mappedRange[0], t.mappedRange[1])) if (t.enabled and t.numTraces()>0) else "")
                rangeWidget.setFlags(rangeWidget.flags() & ~Qt.ItemIsEditable)
                self.table.setItem(p, self.findCol("Mapped Range"), rangeWidget)
                for column in t.config.attrHeaderValues():
                    col = self.findCol(column["header"])
                    if col:
                        wid = QTableWidgetItem("%s" % t.config.attr(column["name"]))
                        traceAattribute = t.config.attrDict(column["name"])
                        if not traceAattribute.get("editable", False):
                            wid.setFlags(wid.flags() & ~Qt.ItemIsEditable)
                        self.table.setItem(p, col, wid)
                    else:
                        print "Internal Error: Column doesn't exists: " + column["header"]
            else:
                print "Internal Error: Set #%d should never be None: " % p # Otherwise there will be an empty line at the end of the table

        self.table.horizontalHeader().setStretchLastSection(True)
        self.connect(self.table, SIGNAL("cellChanged(int, int)"), self.cellChanged)

    def cellChanged(self, row, column):
        """Saves cell edition to the traceManager and .cfg file"""
        for attr in self.attrs:
            attrName = attr["name"]
            if attrName == self.table.horizontalHeaderItem(column).text():
                self._traceManager.traceSets[row].config.setAttr(attrName, self.table.item(row,column).text())
                self._traceManager.traceSets[row].config.saveTrace()
                break

    def changeTraceStatus(self, pos):
        self._traceManager.setTraceSetStatus(pos, not self._traceManager.traceSets[pos].enabled)

    def removeTrace(self, pos):
        """Confirm before removing traces at pos."""
        ret = QMessageBox.question(self, "Remove traces", "Trace set #%d will be removed from the project.\nDo you confirm?" % (pos), QMessageBox.Yes | QMessageBox.No)
        if ret == QMessageBox.Yes:
            self._traceManager.removeTraceSet(pos)

    def findCol(self, name):
        """Function is a hack/cheat to deal with movable headers if they become enabled."""
        for i in range(0, self.table.columnCount()):
            if self.table.horizontalHeaderItem(i).text() == name:
                return i
        return None

    def importDPAv3(self):
        imp = ImportDPAv3Dialog(self)
        imp.exec_()
        self.importExisting(imp.getTraceCfgFile())
        #self.updatePreview()

    def importExisting(self, fname=None):
        tmi = TraceManagerImport(self)
        tmi.exec_()

        if tmi.getTrace() is not None:
            tmi.updateConfigData()
            self._traceManager.appendTraceSet(tmi.getTrace())

    def copyExisting(self, fname=None):
        if fname == None:
            fname, _ = QFileDialog.getOpenFileName(self, 'Open file', QSettings().value("trace_last_file"),'*.cfg')

        if fname:
            QSettings().setValue("trace_last_file", fname)
            #Get our project directory
            targetdir = self.parent.cwp.traceslocation + "/"
            cfgname = os.path.split(fname)[1]
            srcdir = os.path.split(fname)[0] + "/"
            newcfgname = targetdir + cfgname

            #Get prefix from config file
            config = ConfigParser.RawConfigParser()
            config.read(fname)
            prefix = config.get("Trace Config", "prefix")

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
            self._traceManager.appendTraceSet(ti)

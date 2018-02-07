#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2014, NewAE Technology Inc
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
#=================================================

# Python standard imports
import logging
import math
import pickle
import sys
from collections import OrderedDict
from datetime import datetime
from PySide.QtCore import *
from PySide.QtGui import *
from pyqtgraph.parametertree import ParameterTree
import chipwhisperer.common.utils.qt_tweaks as QtFixes
from chipwhisperer.common.utils.parameter import Parameterized, Parameter
from chipwhisperer.common.utils import util
from chipwhisperer.common.results.base import ResultsBase


class GlitchExplorerDialog(Parameterized, QtFixes.QDialog):
    _name='Glitch Explorer'

    def __init__(self, parent):
        super(GlitchExplorerDialog, self).__init__(parent)
        self.setWindowTitle("Glitch Explorer")
        self.setMinimumWidth((500 * self.logicalDpiX())//100)
        self.setMinimumHeight((650 * self.logicalDpiY())//100)

        self.mainLayout = QVBoxLayout()
        self.mainSplitter = QSplitter(self)
        self.mainSplitter.setOrientation(Qt.Vertical)

        self.tableList = []
        self.tune_parameter_list = OrderedDict()

        #Add default table
        self.table = QTableWidget(1,1)
        # self.table.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding)
        self.mainSplitter.addWidget(self.table)

        self.getParams().register()
        self.getParams().addChildren([
            {'name':'Clear Output Table', 'type':'action', 'action':self.clearTable},
            {'name':'Plot Widget', 'type':'action', 'action':self.openPlotWidget},
            {'name':'Normal Response', 'type':'str', 'key':'normalresp', 'value':'s.startswith("Bad")'},
            {'name':'Successful Response', 'type':'str', 'key':'successresp', 'value':'s.startswith("Welcome")'},

            {'name':'Recordings', 'type':'group', 'expanded':False, 'children':[
                {'name':'Load existing', 'type':'action', 'key':'open', 'action':lambda _:self.loadRecordings()},
                {'name':'Autosave Multi-Capture Results', 'type':'bool', 'key':'saveresults', 'value':True},
                {'name':'Autosaved filename', 'type':'str', 'key':'savefilename', 'value':'', "readonly":True},
                {'name':'Notes', 'type':'text', 'key':'savenotes', 'value':""},
            ]},
        ])

        self.paramTree = ParameterTree()
        self.paramTree.addParameters(self.getParams()._PyQtGraphParameter)
        self.mainSplitter.addWidget(self.paramTree)

        self.statusLabel = QLabel("")

        self.mainSplitter.addWidget(self.statusLabel)

        # self.mainSplitter.setHandleWidth(100)

        self.mainLayout.addWidget(self.mainSplitter)

        self.mainLayout.addWidget(QLabel("Glitch Explorer Changed in V4.0 - see wiki.newae.com/cw3to4."))

        self.setLayout(self.mainLayout)
        self.hide()

        #Do an update
        self.table.setSizePolicy(QSizePolicy.MinimumExpanding, QSizePolicy.MinimumExpanding)
        self.table.horizontalHeader().setResizeMode(QHeaderView.Interactive)
        self.clearTable()
        self._campaignRunning = False

    def add_data(self, name, value):
        doupdate = False

        if name not in self.tune_parameter_list.keys():
            doupdate = True

        self.tune_parameter_list[name] = value

        if doupdate:
            self.updateTableHeaders()

    def openPlotWidget(self, _):
        if "Glitch Explorer" not in ResultsBase.registeredObjects:
            ResultsBase.createNew("Scatter Plot", "Glitch Explorer")
        widget = ResultsBase.registeredObjects["Glitch Explorer"]
        widget.raise_()
        widget.yLocked(False)
        self.clearPlotWidget()

    def clearPlotWidget(self):
        widget = ResultsBase.registeredObjects.get("Glitch Explorer", None)
        if widget is not None:
            widget.clearPushed()
            widget.setLabels(top="Glitch Map", xaxis=self.tune_parameter_list.keys()[0] if len(self.tune_parameter_list)>0 else "",
                             yaxis=self.tune_parameter_list.keys()[1] if len(self.tune_parameter_list)>1 else "")

    def campaignStart(self, prefixname):
        """Called when acqusition campaign (multi-api) starts, generates filename"""
        self._autosavefname = self.parent().api.project().getDataFilepath(prefixname + "_glitchresults.p", subdirectory="glitchresults")["abs"]
        self._autosavef = None
        self._campaignRunning = True
        self.clearPlotWidget()

    def campaignDone(self):
        self._campaignRunning = False

        if self._autosavef:
            self._autosavef.close()
            self._autosavef = None
        self.table.setSortingEnabled(True)

    def updateStatus(self):
        okcnt = 0
        for t in self.tableList:
            if t["success"]:
                okcnt += 1

        lbl = "Total %d, Glitches Successful %d" % (len(self.tableList), okcnt)
        self.statusLabel.setText(lbl)

    def reset(self, ignored=None):
        self.tune_parameter_list = OrderedDict()

    def clearTable(self, ignored=None):
        self.tableList = []
        self.table.clear()
        self.table.setRowCount(0)
        self.table.setColumnCount(0)
        self.updateTableHeaders()
        self.table.resizeColumnsToContents()
        self.table.horizontalHeader().setStretchLastSection(True)
        self.table.setColumnWidth(0, 80)
        self.table.setColumnWidth(2, 120)
        self.table.setColumnWidth(3, 80)
        self.table.sortByColumn(3, Qt.DescendingOrder)

    def updateTableHeaders(self, ignored=None, ignoredmore=None, override=None):
        headerlist = ["Status", "Sent", "Received", "Date"]

        if override is not None:
            for c in override:
                headerlist.append(c)
        else:
            for t in self.tune_parameter_list.keys():
                headerlist.append(t)

        self.table.setColumnCount(len(headerlist))
        self.table.setHorizontalHeaderLabels(headerlist)

    def traceDone(self):
        """ Single api done """

        #Trace is done - write values to table as was done OK

    def appendToTable(self, newdata):
        """ Append a result to the display table """

        try:
            self.table.setSortingEnabled(False)
            self.table.insertRow(0)

            outdata = QTableWidgetItem(repr(newdata["output"]))
            if newdata["success"]:
                color = Qt.green
                status = "Success"
            elif newdata["normal"] == False:
                color = Qt.red
                status = "Failed"
            else:
                color = Qt.white
                status = "Normal"

            outdata.setBackground(color)
            self.table.setItem(0, 0, QTableWidgetItem(status))
            self.table.setItem(0, 1, QTableWidgetItem(""))
            self.table.setItem(0, 2, outdata)
            self.table.setItem(0, 3, QTableWidgetItem(newdata["date"].strftime('%H:%M:%S')))
            for i, v in enumerate(newdata["settings"]):
                self.table.setItem(0, 4 + i, QTableWidgetItem(str(v)))

            self.table.resizeRowToContents(0)

            widget = ResultsBase.registeredObjects.get("Glitch Explorer", None)
            if widget is not None:
                widget.plot([newdata["settings"][0] if len(newdata["settings"])>0 else 0],
                    [newdata["settings"][1] if len(newdata["settings"])>1 else 0],
                     ((0, 255, 0, 75) if status=="Success" else
                     (255, 0, 75) if status=="Failed" else
                     (200, 200, 200, 50)), str(newdata["settings"]))

        except AttributeError as e:
            raise StopIteration("Error when adding data to the table. Plese clear it and try again. Details:" + str(e))

    def addResponse(self, resp):
        """ Add a response from the system to glitch table + logs """

        normeval = self.findParam('normalresp').getValue()
        succeval = self.findParam('successresp').getValue()

        if len(normeval) > 0:
            #Check if Normal
            normresult = eval(normeval, {'s':resp}, {})
        else:
            normresult = False

        #Check if Successful
        if len(succeval) > 0:
            #Check if Normal
            succresult = eval(succeval, {'s':resp}, {})
        else:
            succresult = False

        #Check ?
        if not isinstance(normresult, bool):
            raise ValueError("Result of 'normal' eval() not a bool, got %s (result: %s)" % (type(normresult), normresult))

        if not isinstance(succresult, bool):
            raise ValueError("Result of 'success' eval() not a bool, got %s (result: %s)" % (type(succresult), succresult))

        if normresult and succresult:
            logging.warning('Both normresult and succresult True!')

        starttime = datetime.now()

        respstr = str(bytearray(resp))
        # respstr = ' '.join(["%02x" % t for t in bytearray(resp)])

        settingsList = self.tune_parameter_list.values()
        newdata = {"input":"", "output":respstr, "normal":normresult, "success":succresult, "settings":settingsList, "date":starttime}

        self.tableList.append(newdata)
        self.appendToTable(newdata)
        self.updateStatus()

        if self._campaignRunning and self.findParam(["Recordings","saveresults"]).getValue():
            if self._autosavef is None:
                # File previously not open
                try:
                    self._autosavef = open(self._autosavefname, "w")
                except Exception as e:
                    self.findParam(["Recordings","saveresults"]).setValue(False)
                    raise Warning("Could not save recordings to file: %s. Reason: %s. Disabling it in order to continue." % (self._autosavefname, str(e)))
                self.findParam(["Recordings",'savefilename']).setValue(self._autosavefname, ignoreReadonly=True)

                # Add notes
                pickle.dump({"notes":self.findParam(["Recordings",'savenotes']).getValue()}, self._autosavef)

                # Add headers
                cmds = self.tune_parameter_list.keys()
                pickle.dump({"commands":cmds}, self._autosavef)

            # Add data
            pickle.dump({"data":newdata}, self._autosavef)

    def loadRecordings(self, fname=None):
        if fname == None:
            fname, _ = QFileDialog.getOpenFileName(self, 'Open file', QSettings().value("open_glitch_file"),'*.p')

        if fname:
            self.clearTable()
            file = open(fname, 'r')
            self.findParam(["Recordings",'savenotes']).setValue(pickle.load(file)['notes'])
            self.updateTableHeaders(override=pickle.load(file)['commands'])
            while 1:
                try:
                    self.appendToTable(pickle.load(file)['data'])
                except EOFError:
                    break
            file.close()


def main():
    # Create the Qt Application
    app = QApplication(sys.argv)

    std = GlitchExplorerDialog(None)
    std.show()

    app.exec_()

if __name__ == '__main__':
    main()

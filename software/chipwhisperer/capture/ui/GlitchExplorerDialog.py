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
from datetime import datetime
from PySide.QtCore import *
from PySide.QtGui import *
from pyqtgraph.parametertree import ParameterTree
import chipwhisperer.common.utils.qt_tweaks as QtFixes
from chipwhisperer.common.utils.parameter import Parameterized, Parameter
from chipwhisperer.common.utils import util
from chipwhisperer.common.results.base import ResultsBase


class TuningParameter(Parameterized):

    def __init__(self, num):
        super(TuningParameter, self).__init__()
        self._name = 'Tuning Parameter %d' % num
        self.paramNum = num
        self.rangeComplete = util.Signal()
        self.nameChanged = util.Signal()
        self.tracesreqChanged = util.Signal()

        self.getParams().addChildren([
            {'name':'Name', 'type':'str', 'key':'humanname', 'value':'Param #%d' % num, 'action':lambda p:self.nameChange(p.getValue())},
            {'name':'Parameter Path', 'type':'str', 'key':'parampath', 'value':'[]', 'action':self.updateParams},
            {'name':'Data Format', 'type':'list', 'key':'datatype', 'values':{'Int':int, 'Float':float}, 'value':int},
            {'name':'Range', 'type':'range', 'key':'range', 'limits':(-1E6, 1E6), 'value':(0, 10), 'default':(0, 10), 'action':self.updateParams},
            {'name':'Value', 'type':'float', 'key':'curval', 'value':1.0},
            {'name':'Step', 'type':'float', 'key':'step', 'value':1.0, 'action':self.updateParams},
            {'name':'Repeat', 'type':'int', 'key':'repeat', 'value':1, 'action':self.updateParams},
            {'name':'Mode', 'type':'list', 'key':'mode', 'values':["Linear"], 'value':"Linear", 'action':self.updateParams},
        ])
        self.cnt = 0
        self.updateParams()

    def nameChange(self, newname):
        self.nameChanged.emit(self.paramNum, newname)

    def name(self):
        return self.findParam('humanname').getValue()

    def reset(self):
        self.findParam('curval').setValue(self.findParam('range').getValue()[0])
        self.updateParams()

    def updateParams(self, _=None):
        rng = self.findParam('range').getValue()

        # Force to be valid values
        curval = self.findParam('curval').getValue()
        curval = max(curval, rng[0])
        curval = min(curval, rng[1])
        self.findParam('curval').setLimits(rng)
        self.findParam('curval').setValue(curval)

        self.paramRange = rng
        self.paramValueItem = self.findParam('curval')
        self.paramStep = self.findParam('step').getValue()
        self.paramRepeat = self.findParam('repeat').getValue()
        self.paramType = self.findParam('datatype').getValue()
        try:
            self.paramScript = eval(self.findParam('parampath').getValue())
        except SyntaxError as e:
            logging.error('Syntax Error: %s' % str(e))

        try:
            Parameter.setParameter(self.paramScript+[curval])
        except:
            pass
        self.tracesrequired = math.ceil(((self.paramRange[1] - self.paramRange[0]) / self.paramStep) * self.paramRepeat)+1
        self.tracesreqChanged.emit(self.paramNum, self.tracesrequired)

    def findNewValue(self, mode="linear"):
        """ Find new value for this parameter """

        if str.lower(mode) == "linear":
            self.cnt += 1
            if self.cnt == self.paramRepeat:
                # Done this one, next step
                self.cnt = 0
                newval = self.paramValueItem.getValue() + self.paramStep

                if newval > self.paramRange[1]:
                    newval = self.paramRange[0]
                    self.rangeComplete.emit(self.paramNum)

                # Cast type to required value
                newval = self.paramType(newval)

                self.paramValueItem.setValue(newval)

                parameter = self.paramScript+[newval]
                try:
                    Parameter.setParameter(parameter)
                except:
                    raise StopIteration("Choose a valid Parameter Path/Value combination. Got: " + str(parameter))
        else:
            raise ValueError("Unknown Increment Type %s" % mode)


class GlitchExplorerDialog(Parameterized, QtFixes.QDialog):
    _name='Glitch Explorer'

    def __init__(self, parent):
        super(GlitchExplorerDialog, self).__init__(parent)
        self.setWindowTitle("Glitch Explorer")
        self.setMinimumWidth(500)

        self.mainLayout = QVBoxLayout()
        self.mainSplitter = QSplitter(self)
        self.mainSplitter.setOrientation(Qt.Vertical)

        self.tableList = []
        self.tuneParamList = []

        #Add default table
        self.table = QTableWidget(1,1)
        # self.table.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding)
        self.mainSplitter.addWidget(self.table)

        self.getParams().register()
        self.getParams().addChildren([
            {'name':'Clear Output Table', 'type':'action', 'action':self.clearTable},
            {'name':'Plot Widget', 'type':'action', 'action':self.openPlotWidget},
            {'name':'Reset', 'type':'action', 'action':self.reset, 'tip':"Resets all Tuning Parameters to its minimum value."},
            {'name':'Tuning Parameters', 'key':'numtune', 'type':'int', 'value':0, 'limits':(0, 4), 'action':self.updateParameters, 'readonly':False},
            {'name':'Traces Required', 'key':'tracesreq', 'type':'int', 'value':1, 'limits':(1, 1E99), 'readonly':True, 'children':[
                {'name':'Use this value', 'type':'action', 'action':lambda _: Parameter.setParameter(['Generic Settings', 'Acquisition Settings', 'Number of Traces',self.findParam('tracesreq').getValue()])},
             ]},
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
        self.setLayout(self.mainLayout)
        self.hide()

        #Do an update
        self.table.setSizePolicy(QSizePolicy.MinimumExpanding, QSizePolicy.MinimumExpanding)
        self.table.horizontalHeader().setResizeMode(QHeaderView.Interactive)
        self.clearTable()
        self._campaignRunning = False

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
            widget.setLabels(top="Glitch Map", xaxis=self.tuneParamList[0].name() if len(self.tuneParamList)>0 else "",
                             yaxis=self.tuneParamList[1].name() if len(self.tuneParamList)>1 else "")

    def tuneEnabled(self, status):
        self.findParam('numtune').setReadonly(not status)

    def campaignStart(self, prefixname):
        """Called when acqusition campaign (multi-api) starts, generates filename"""
        self._autosavefname = self.parent().api.project().getDataFilepath(prefixname + "_glitchresults.p", subdirectory="glitchresults")["abs"]
        self._autosavef = None
        self._campaignRunning = True
        self.tuneEnabled(False)
        self.clearPlotWidget()

    def campaignDone(self):
        self._campaignRunning = False
        self.tuneEnabled(True)

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
        for t in self.tuneParamList:
            t.reset()

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

    def updateParameters(self, ignored=None):
        numparams = self.findParam('numtune').getValue()

        # Did number change? Adjust if needed
        while numparams < len(self.tuneParamList):
            #Shed parameters
            self.tuneParamList[-1].getParams().delete()
            self.tuneParamList.pop()

        while numparams > len(self.tuneParamList):
            #Add parameters
            #p = Parameter.create(name='Tuning Parameter %d' % len(self.tuneParamList), type='group', children=self.glitchTuneParamTemplate)
            p = TuningParameter(len(self.tuneParamList))
            self.tuneParamList.append(p)
            self.getParams().append(p.getParams())

            # Do stuff
            p.nameChanged.connect(self.updateTableHeaders)
            p.rangeComplete.connect(self.rangeDone)
            p.tracesreqChanged.connect(self.tracesreqChanged)

        self.updateTableHeaders()
        self.tracesreqChanged()
        
    def tracesreqChanged(self, pnum=None, newnum=None):
        treq = 1
        for t in self.tuneParamList:
                treq *= t.tracesrequired
        self.findParam('tracesreq').setValue(treq, ignoreReadonly=True)

    def updateTableHeaders(self, ignored=None, ignoredmore=None, override=None):
        headerlist = ["Status", "Sent", "Received", "Date"]

        if override is not None:
            for c in override:
                headerlist.append(c)
        else:
            for t in self.tuneParamList:
                headerlist.append(t.name())

        self.table.setColumnCount(len(headerlist))
        self.table.setHorizontalHeaderLabels(headerlist)

    def rangeDone(self, pnum):
        if (pnum + 1) < len(self.tuneParamList):
            self.tuneParamList[pnum + 1].findNewValue()

    def traceDone(self):
        """ Single api done """

        # TODO: Improve how looping is done
        if len(self.tuneParamList) > 0:
            # Always increment lowest, triggers upper values
            # try:
                self.tuneParamList[0].findNewValue()
            # except Exception as e:
            #     raise StopIteration(e)

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

            self.table.resizeRowsToContents()

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

        respstr = str(bytearray(resp.encode('utf-8')))
        # respstr = ' '.join(["%02x" % t for t in bytearray(resp)])

        settingsList = []
        for i in range(0, len(self.tuneParamList)):
            try:
                settingsList.append(Parameter.getParameter(self.tuneParamList[i].paramScript))
            except:
                raise StopIteration("Choose a valid Parameter Path for Tuning Parameter \"%s\" . Got: %s" % (self.tuneParamList[i].name(), self.tuneParamList[i].paramScript))
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
                cmds = [self.tuneParamList[i].findParam('parampath').getValue() for i in range(0, len(self.tuneParamList))]
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

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

import sys
from PySide.QtCore import *
from PySide.QtGui import *
import pyqtgraph as pg
import pyqtgraph.multiprocess as mp
import pyqtgraph.parametertree.parameterTypes as pTypes
from pyqtgraph.parametertree import Parameter, ParameterTree, ParameterItem, registerParameterType
from openadc.ExtendedParameter import ExtendedParameter
import chipwhisperer.common.ParameterTypesCustom


from datetime import datetime

class TuningParameter(QObject):

    rangeComplete = Signal(int)
    newScriptCommand = Signal(int, list)
    nameChanged = Signal(int, str)

    def __init__(self, num):
        super(TuningParameter, self).__init__()

        self.paramNum = num

        paramTemplate = [
            {'name':'Name', 'type':'str', 'key':'humanname', 'value':'Param #%d' % num, 'set':self.nameChange},
            {'name':'Script Command', 'type':'str', 'key':'script', 'value':'[]', 'set':self.updateParams},
            {'name':'Data Format', 'type':'list', 'key':'datatype', 'values':{'Int':int, 'Float':float}, 'value':int},
            {'name':'Range', 'type':'range', 'key':'range', 'limits':(-1E6, 1E6), 'value':(0, 10), 'set':self.updateParams},
            {'name':'Value', 'type':'float', 'key':'curval', 'value':1.0},
            {'name':'Step', 'type':'float', 'key':'step', 'value':1.0, 'set':self.updateParams},
            {'name':'Repeat', 'type':'int', 'key':'repeat', 'value':1, 'set':self.updateParams},
            {'name':'Mode', 'type':'list', 'key':'mode', 'values':["Linear"], 'set':self.updateParams},
            ]

        self.params = Parameter.create(name='Tuning Parameter %d' % num, type='group', children=paramTemplate)
        ExtendedParameter.setupExtended(self.params, self)
        self.cnt = 0

    def nameChange(self, newname):
        self.nameChanged.emit(self.paramNum, newname)

    def name(self):
        return self.findParam('humanname').value()

    def updateParams(self, ignored=None):

        rng = self.findParam('range').value()

        # Force to be valid values
        curval = self.findParam('curval').value()
        curval = max(curval, rng[0])
        self.findParam('curval').setValue(curval)

        curval = min(curval, rng[1])
        self.findParam('curval').setValue(curval)
        self.findParam('curval').setLimits(rng)

        self.paramRange = rng
        self.paramValueItem = self.findParam('curval')
        self.paramStep = self.findParam('step').value()
        self.paramRepeat = self.findParam('repeat').value()
        self.paramType = self.findParam('datatype').value()
        try:
            self.paramScript = eval(self.findParam('script').value())
        except SyntaxError, e:
            print "Syntax Error: %s" % str(e)

    def findNewValue(self, mode="linear"):
        if str.lower(mode) == "linear":

            self.cnt += 1
            if self.cnt == self.paramRepeat:
                # Done this one, next step
                self.cnt = 0
                newval = self.paramValueItem.value() + self.paramStep

                if newval > self.paramRange[1]:
                    newval = self.paramRange[0]
                    self.rangeComplete.emit(self.paramNum)

                newval = self.paramType(newval)
                # print newval

                self.paramScript[-1] = newval
                self.paramValueItem.setValue(newval)
                # print self.paramScript
                self.newScriptCommand.emit(self.paramNum, self.paramScript)


        else:
            raise ValueError("Unknown Increment Type %s" % mode)



class GlitchExplorerDialog(QDialog):
    def __init__(self, parent, showScriptParameter=None):
        super(GlitchExplorerDialog, self).__init__(parent)

        # HACK FOR NOW
        self.glitchfile = open("glitchresults.txt", "a")
        self.glitchfile.write("\n\n")
        self.glitchfile.write("#*******FILE OPENED************\n")

        self.showScriptParameter = showScriptParameter

        self.setWindowTitle("Glitch Explorer")

        self.parent = parent

        self.mainLayout = QVBoxLayout()

        self.tableList = []
        self.tuneParamList = []

        #Add default table
        self.table = QTableWidget(1,1)
        self.mainLayout.addWidget(self.table)

        self.glitchParams =[{'name':'Clear Output Table', 'type':'action', 'action':self.clearTable},
                            {'name':'Tuning Parameters ', 'key':'numtune', 'type':'int', 'value':0, 'limits':(0, 1), 'set':self.updateParameters},
                            {'name':'Normal Response', 'type':'str', 'key':'normalresp', 'value':'s.startswith("Bad")'},
                            {'name':'Successful Response', 'type':'str', 'key':'successresp', 'value':'s.startswith("Welcome")'}
                            ]


        self.params = Parameter.create(name='Glitch Explorer', type='group', children=self.glitchParams)
        ExtendedParameter.setupExtended(self.params, self)
        self.paramTree = ParameterTree()

        self.reloadParameters()

        self.mainLayout.addWidget(self.paramTree)

        self.statusLabel = QLabel("")

        self.mainLayout.addWidget(self.statusLabel)

        self.setLayout(self.mainLayout)
        self.hide()

        #Do an update
        self.table.setSizePolicy(QSizePolicy.MinimumExpanding, QSizePolicy.MinimumExpanding)
        self.table.horizontalHeader().setResizeMode(QHeaderView.Interactive)  # setStretchLastSection(True)
        self.clearTable()


    def updateStatus(self):
        okcnt = 0
        for t in self.tableList:
            if t["success"]:
                okcnt += 1

        lbl = "Total %d, Glitches Successful %d" % (len(self.tableList), okcnt)
        self.statusLabel.setText(lbl)

    def clearTable(self, ignored=None):
        self.tableList = []
        self.table.clear()
        self.table.setRowCount(0)
        self.updateTableHeaders()

    def executeScriptCommand(self, paramNum, script):
        print script
        self.parent.setParameter(script)

    def updateParameters(self, ignored=None):
        numparams = self.findParam('numtune').value()

        # Did number change? Adjust if needed
        while numparams < len(self.tuneParamList):
            #Shed parameters
            self.tuneParamList.pop()

        while numparams > len(self.tuneParamList):
            #Add parameters
            #p = Parameter.create(name='Tuning Parameter %d' % len(self.tuneParamList), type='group', children=self.glitchTuneParamTemplate)
            p = TuningParameter(len(self.tuneParamList))
            self.tuneParamList.append(p)

            # Do stuff
            p.nameChanged.connect(self.updateTableHeaders)
            p.newScriptCommand.connect(self.executeScriptCommand)

        self.updateTableHeaders()
        self.reloadParameters()

    def reloadParameters(self):
        ExtendedParameter.reloadParams(self.paramList(), self.paramTree)

    def paramList(self):
        p = [self.params]

        # for i in range(0, self.findParam('numtune').value()):
        #    p.append(self.tuneParamList[i])

        for t in self.tuneParamList:
            p.append(t.params)

        return p

    def updateTableHeaders(self, ignored=None, ignoredmore=None):

        headerlist = ["Sent", "Received", "Date"]

        for t in self.tuneParamList:
            headerlist.append(t.name())

        self.table.setColumnCount(len(headerlist))
        self.table.setHorizontalHeaderLabels(headerlist)

        for i in range(0, len(self.tuneParamList)):
            self.glitchfile.write("# Script %d = %s\n" % (i, self.tuneParamList[i].findParam('script').value()))

    def traceDone(self):

        # TODO: Currently only works on one parameter, should improve this
        # to work with multiple. Need to think about how that should work
        # depending on looping method etc?
        if len(self.tuneParamList) > 0:
            self.tuneParamList[0].findNewValue()


    def appendToTable(self, newdata):
        self.table.insertRow(0)

        outdata = QTableWidgetItem(newdata["output"])

        if newdata["success"]:
            outdata.setBackground(QBrush(QColor(0,255,0)))

        elif newdata["normal"] == False:
            outdata.setBackground(QBrush(QColor(255,0,0)))

        self.table.setItem(0, 1, outdata)
        self.table.setItem(0, 2, QTableWidgetItem(newdata["date"].strftime('%H:%M:%S')))

        for i in range(0, len(self.tuneParamList)):
            value = self.tuneParamList[i].paramValueItem.value()
            self.table.setItem(0, 3 + i, QTableWidgetItem(str(value)))

    def addResponse(self, resp):

        normeval = self.findParam('normalresp').value()
        succeval = self.findParam('successresp').value()

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
        if isinstance(normresult, bool) == False:
            raise ValueError("Result of 'normal' eval() not a bool, got %s (result: %s)"%(type(normresult), normresult))

        if isinstance(succresult, bool) == False:
            raise ValueError("Result of 'success' eval() not a bool, got %s (result: %s)"%(type(succresult), succresult))


        if normresult and succresult:
            print "WARNING: Both normresult and succresult True!"

        starttime = datetime.now()

        respstr = str(bytearray(resp.encode('utf-8')))
        # respstr = ' '.join(["%02x" % t for t in bytearray(resp)])

        newdata = {"input":"", "output":respstr, "normal":normresult, "success":succresult, "settings":{}, "date":starttime}
        self.tableList.append(newdata)
        self.appendToTable(newdata)
        self.updateStatus()


        # Write to file
        filestr = "%s %s"%(starttime.strftime('%H:%M:%S'), respstr)
        for p in self.tuneParamList:
            filestr += "%s "%p.paramValueItem.value()
        self.glitchfile.write(filestr + "\n")


def main():
    # Create the Qt Application
    app = QApplication(sys.argv)

    std = GlitchExplorerDialog(None)
    std.show()

    app.exec_()

if __name__ == '__main__':
    main()

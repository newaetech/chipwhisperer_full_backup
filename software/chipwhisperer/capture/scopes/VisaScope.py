#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2013-2014, NewAE Technology Inc
# All rights reserved.
#
# Authors: Colin O'Flynn
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

try:
    from PySide.QtCore import *
    from PySide.QtGui import *
except ImportError:
    print "ERROR: PySide is required for this program"
    sys.exit()

from openadc.ExtendedParameter import ExtendedParameter

try:
    from pyqtgraph.parametertree import Parameter
except ImportError:
    print "ERROR: PyQtGraph is required for this program"
    sys.exit()

from visa import *
import time

class VisaScope(QWidget):
    paramListUpdated = Signal(list)
    dataUpdated = Signal(list, int)

    xScales = {"500 mS":500E-3, "200 mS":200E-3, "100 mS":100E-3, "50 mS":50E-3,
               "20 mS":20E-3, "10 mS":10E-3, "5 mS":5E-3, "2 mS":2E-3, "1 mS":1E-3,
               "500 uS":500E-6, "200 uS":200E-6, "100 uS":100E-6, "50 uS":50E-6,
               "20 uS":20E-6, "10 uS":10E-6, "5 uS":5E-6, "2 uS":2E-6}

    yScales = {"10 V":10, "5 V":5, "2 V":2, "500 mV":500E-3, "250 mV":250E-3, "100 mV":100E-3, "50 mV":50E-3}

    header =        ":SYSTem:HEADer OFF\n"

    def __init__(self,console=None,showScriptParameter=None):
        super(VisaScope, self).__init__()
        self.showScriptParameter = showScriptParameter
        self.console = console
        self.visaInst = None

        scopeParams = [
                      {'name':'X-Scale', 'key':'xscale', 'type':'list', 'values':self.xScales},
                      {'name':'Y-Scale', 'key':'yscale', 'type':'list', 'values':self.yScales},
                      {'name':'Y-Offset', 'key':'yoffset', 'type':'float', 'step':1E-3, 'siPrefix': True, 'suffix': 'V'},
                      {'name':'X-Offset', 'key':'xoffset', 'type':'float', 'step':1E-6, 'siPrefix': True, 'suffix': 'S'},
                      {'name':'Download Offset', 'key':'xdisporigin', 'type':'int',  'limits':(0,1E9)},
                      {'name':'Download Size', 'key':'xdisprange', 'type':'int', 'limits':(0,1E9)},
                  ]




        for t in self.getAdditionalParams():
            scopeParams.append(t)



        self.params = Parameter.create(name='Scope Settings', type='group', children=scopeParams)
        ExtendedParameter.setupExtended(self.params, self)

    def getAdditionalParams(self):
        """Override this to define additional parameters"""
        return []

    def con(self, constr):
        self.visaInst = instrument(constr)
        print self.visaInst.ask("*IDN?")
        for cmd in self.header:
            self.visaInst.write(cmd)
            print "VISA: %s" % cmd
            time.sleep(0.1)
        self.updateCurrentSettings()

    def paramList(self):
        p = [self.params]
        return p

    def updateCurrentSettings(self):
        self.currentSettings()

        self.findParam('xscale').setValue(self.XScale)
        self.findParam('yscale').setValue(self.YScale)
        self.findParam('xoffset').setValue(self.XOffset)
        self.findParam('yoffset').setValue(self.YOffset)

    def currentSettings(self):
        """You MUST implement this"""
        pass

    def arm(self):
        """Example arm implementation works on most"""
        self.visaInst.write(":DIGitize")

    def capture(self, update=True, NumberPoints=None):
        """You MUST implement this"""
        self.dataUpdated.emit(self.datapoints, 0)
        pass

class VisaScopeInterface_DSO1024A(VisaScope):

    # TODO: What scales & ranges are allowed on the DSO1024A?
    xScales = {"500 mS":500E-3, "200 mS":200E-3, "100 mS":100E-3, "50 mS":50E-3,
               "20 mS":20E-3, "10 mS":10E-3, "5 mS":5E-3, "2 mS":2E-3, "1 mS":1E-3,
               "500 uS":500E-6, "200 uS":200E-6, "100 uS":100E-6, "50 uS":50E-6,
               "20 uS":20E-6, "10 uS":10E-6, "5 uS":5E-6, "2uS":2E-6, "1 uS":1E-6}

    yScales = {"10 V":10, "5 V":5, "2 V":2, "500 mV":500E-3, "200 mV":200E-3, "100 mV":100E-3,
               "50 mV":50E-3, "20 mV":20E-3, "10 mV":10E-3, "5 mV":5E-3}

    header = [  ":CHANnel1:PROBe 1X\n",
                ":CHANnel1:DISPlay ON\n",
                ":CHANnel1:COUPling DC\n",
                ":CHANnel2:PROBe 10X\n",
                ":CHANnel2:SCALe 1\n",
                ":CHANnel2:OFFSet 0\n",
                ":CHANnel2:DISPLay ON\n",
                ":TRIGger:MODE EDGE\n",
                ":TRIGger:EDGE:SOURce CHANnel2\n",
                ":TRIGger:EDGE:SLOPe NEGative\n",
                ":TRIGger:EDGE:LEVel 2.0\n",
                ":TRIGger:EDGE:SWEep NORMal\n",
                ":WAVeform:SOURce CHANnel1\n",
                ":WAVeform:FORMat WORD\n",
                ]

    def currentSettings(self):

        # TODO: Delete these?
        self.XScale = self.visaInst.ask_for_values(":TIMebase:SCALe?")
        self.XScale = self.XScale[0]
        self.XOffset = self.visaInst.ask_for_values(":TIMebase:POSition?")
        self.XOffset = self.XOffset[0]
        self.YOffset = self.visaInst.ask_for_values(":CHANnel1:OFFSet?")
        self.YOffset = self.YOffset[0]
        self.YScale = self.visaInst.ask_for_values(":CHANnel1:SCALe?")
        self.YScale = self.YScale[0]

    def arm(self):
        self.visaInst.write(":RUN\n")

        result = "fake"
        while ((result.startswith("WAIT") == False) and (result.startswith("RUN") == False)):
            result = self.visaInst.ask(":TRIGger:STATus?")
            time.sleep(0.1)
            print "1Waiting..."

    def capture(self, Update=False, N=None, waitingCallback=None):

        # Wait?
        while (self.visaInst.ask("*OPC?\n") != "1"):
            time.sleep(0.1)
            waitingCallback()
            print "2Waiting..."

        # print command
        self.visaInst.write(":WAVeform:DATA?")
        data = self.visaInst.read_raw()

        # Find '#' which is start of frame
        start = data.find('#')

        if start < 0:
            print "Error in header"
            return

        start = start + 1
        hdrlen = data[start]
        hdrlen = int(hdrlen)

        # print hdrlen

        start = start + 1
        datalen = data[start:(start + hdrlen)]
        datalen = int(datalen)
        # print datalen

        start = start + hdrlen

        # Each is two bytes
        wavdata = bytearray(data[start:(start + datalen)])

        self.datapoints = []

        for j in range(0, datalen, 2):
            data = wavdata[j] | (wavdata[j + 1] << 8)

            if (data & 0x8000):
                data = -65536 + data

            self.datapoints.append(data)

        self.dataUpdated.emit(self.datapoints, 0)


class VisaScopeInterface_MSO54831D(VisaScope):

    xScales = {"500 mS":500E-3, "200 mS":200E-3, "100 mS":100E-3, "50 mS":50E-3,
               "20 mS":20E-3, "10 mS":10E-3, "5 mS":5E-3, "2 mS":2E-3, "1 mS":1E-3,
               "500 uS":500E-6, "200 uS":200E-6, "100 uS":100E-6, "50 uS":50E-6,
               "20 uS":20E-6, "10 uS":10E-6, "5 uS":5E-6, "2uS":2E-6, "1 uS":1E-6}

    yScales = {"10 V":10, "5 V":5, "2 V":2, "500 mV":500E-3, "200 mV":200E-3, "100 mV":100E-3,
               "50 mV":50E-3, "20 mV":20E-3, "10 mV":10E-3, "5 mV":5E-3}

    header = [":SYSTem:HEADer OFF\n",
                ":CHANnel1:INPut DCFifty\n",
                ":CHANnel1:DISPlay ON\n",
                ":CHANnel2:DISPLay ON\n",
                ":CHANnel2:SCALe 1.0\n",
                ":TRIGger:MODE EDGE\n",
                ":TRIGger:EDGE:SOURce CHANnel2\n",
                ":TRIGger:EDGE:SLOPe NEGative\n",
                ":TRIGger:LEVel CHANnel2,2.0\n",
                ":TRIGger:SWEep TRIGgered\n",
                ":WAVeform:FORMat WORD\n",
                ":WAVeform:BYTeorder LSBFirst\n",
                ":WAVeform:SOURce CHANnel1\n",
                ":ACQuire:COMPlete 100\n"
                ]

    def currentSettings(self):
        test = self.visaInst.ask_for_values(":WAVeform:PREamble?")

        if test[4] != 0.0:
            self.XDisplayedOrigin = (test[12] - test[5]) / test[4]
            srange = test[11] / test[4]
        else:
            self.XDisplayedOrigin = 0.0
            srange = 0.0

        spoints = test[2]

        if srange < spoints:
            self.XDisplayedRange = srange
        else:
            self.XDisplayedRange = spoints

        self.XScale = self.visaInst.ask_for_values(":TIMebase:SCALe?")
        self.XScale = self.XScale[0]
        self.XOffset = self.visaInst.ask_for_values(":TIMebase:POSition?")
        self.XOffset = self.XOffset[0]
        self.YOffset = self.visaInst.ask_for_values(":CHANnel1:OFFSet?")
        self.YOffset = self.YOffset[0]
        self.YScale = self.visaInst.ask_for_values(":CHANnel1:SCALe?")
        self.YScale = self.YScale[0]

    def arm(self):
        self.visaInst.write(":DIGitize")

    def capture(self, Update=False, N=None, waitingCallback=None):
        xdstart = self.findParam("xdisporigin").value()
        xdrange = self.findParam("xdisprange").value()

        command = ":WAVeform:DATA?"

        if (xdstart != 0) or (xdrange != 0):
            command += " %d"%xdstart

            if xdrange != 0:
                command += ",%d"%xdrange

        #print command
        self.visaInst.write(command)
        data = self.visaInst.read_raw()

        #Find '#' which is start of frame
        start = data.find('#')

        if start < 0:
            print "Error in header"
            return

        start = start+1
        hdrlen = data[start]
        hdrlen = int(hdrlen)

        #print hdrlen

        start = start+1
        datalen = data[start:(start+hdrlen)]
        datalen = int(datalen)
        #print datalen

        start = start+hdrlen

        #Each is two bytes
        wavdata = bytearray(data[start:(start + datalen)])

        self.datapoints = []

        for j in range(0, datalen, 2):
            data = wavdata[j] | (wavdata[j+1] << 8)

            if (data & 0x8000):
                data = -65536 + data

            self.datapoints.append(data)

        self.dataUpdated.emit(self.datapoints, 0)



class VisaScopeInterface(QObject):
    connectStatus = Signal(bool)
    dataUpdated = Signal(list, int)
    paramListUpdated = Signal(list)

    def __init__(self, parent=None, console=None, showScriptParameter=None):
        super(VisaScopeInterface, self).__init__(parent)
        self.parent = parent
        self.scopetype = None
        self.datapoints = []

        try:
            mso54831d = VisaScopeInterface_MSO54831D(console=console, showScriptParameter=showScriptParameter)
        except ImportError:
            mso54831d = None

        try:
            dso1024A = VisaScopeInterface_DSO1024A(console=console, showScriptParameter=showScriptParameter)
        except ImportError:
            dso1024A = None

        self.setCurrentScope(mso54831d, False)
        defscope = mso54831d

        scope_cons = {}

        if mso54831d:
            mso54831d.paramListUpdated.connect(self.emitParamListUpdated)
            mso54831d.dataUpdated.connect(self.passUpdated)
            scope_cons["Agilent MSO 54831D"] = mso54831d

        if dso1024A:
            dso1024A.paramListUpdated.connect(self.emitParamListUpdated)
            dso1024A.dataUpdated.connect(self.passUpdated)
            scope_cons["Agilent DSO 1024A"] = dso1024A



        self.advancedSettings = None

        scopeParams = [{'name':'Scope Type', 'type':'list', 'values':scope_cons, 'value':defscope, 'set':self.setCurrentScope},
                       {'name':'Connect String', 'key':'connStr', 'type':'str', 'value':''},
                       {'name':'Example Strings', 'type':'list', 'values':['', 'TCPIP0::192.168.2.100::inst0::INSTR'], 'value':'', 'set':self.exampleString},
                      ]

        self.params = Parameter.create(name='VISA Scope Interface', type='group', children=scopeParams)
        ExtendedParameter.setupExtended(self.params, self)
        self.showScriptParameter = showScriptParameter
        self.setCurrentScope(defscope)

    def exampleString(self, newstr):
        self.findParam('connStr').setValue(newstr)

    def emitParamListUpdated(self):
        self.paramListUpdated.emit(self.paramList())

    # def paramTreeChanged(self, param, changes):
    #    if self.showScriptParameter is not None:
    #        self.showScriptParameter(param, changes, self.params)

    def passUpdated(self, lst, offset):
        self.datapoints = lst
        self.offset = offset
        self.dataUpdated.emit(lst, offset)

    def setCurrentScope(self, scope, update=True):
        self.scopetype = scope
        if update:
            self.paramListUpdated.emit(self.paramList())

    def con(self):
        if self.scopetype is not None:
            self.scopetype.con(self.findParam('connStr').value())
            self.connectStatus.emit(True)


    def dis(self):
        if self.scopetype is not None:
            self.scopetype.dis()
            self.connectStatus.emit(True)

    def doDataUpdated(self,  l, offset=0):
        self.datapoints = l
        self.offset = offset
        if len(l) > 0:
            self.dataUpdated.emit(l, offset)

    def arm(self):
        self.scopetype.arm()

    def capture(self, update=True, NumberPoints=None):
        """Raises IOError if unknown failure, returns 'False' if successful, 'True' if timeout"""
        return self.scopetype.capture(update, NumberPoints)

    def paramList(self):
        p = []
        p.append(self.params)

        if self.scopetype is not None:
            for a in self.scopetype.paramList(): p.append(a)


        #if self.advancedSettings is not None:
        #    for a in self.advancedSettings.paramList(): p.append(a)

        return p

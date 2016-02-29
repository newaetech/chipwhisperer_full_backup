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


import time
from chipwhisperer.capture.api.ExtendedParameter import ExtendedParameter
from pyqtgraph.parametertree import Parameter
from chipwhisperer.common.utils import util
from chipwhisperer.capture.scopes.ScopeTemplate import ScopeTemplate
from visa import *

def getInstance(*args):
    return VisaScopeInterface(*args)

class VisaScope(object):
    paramListUpdated = util.Signal()
    dataUpdated = util.Signal()

    xScales = {"500 mS":500E-3, "200 mS":200E-3, "100 mS":100E-3, "50 mS":50E-3,
               "20 mS":20E-3, "10 mS":10E-3, "5 mS":5E-3, "2 mS":2E-3, "1 mS":1E-3,
               "500 uS":500E-6, "200 uS":200E-6, "100 uS":100E-6, "50 uS":50E-6,
               "20 uS":20E-6, "10 uS":10E-6, "5 uS":5E-6, "2 uS":2E-6}

    yScales = {"10 V":10, "5 V":5, "2 V":2, "500 mV":500E-3, "250 mV":250E-3, "100 mV":100E-3, "50 mV":50E-3}

    header = ":SYSTem:HEADer OFF\n"

    def __init__(self,showScriptParameter=None):
        super(VisaScope, self).__init__()
        self.showScriptParameter = showScriptParameter
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
        self.visaInst.write("*RST")
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

    header = [  ":CHANnel1:PROBe 1X",
                ":CHANnel1:DISPlay ON",
                ":CHANnel1:COUPling DC",
                ":CHANnel2:PROBe 10X",
                ":CHANnel2:SCALe 1",
                ":CHANnel2:OFFSet 0",
                ":CHANnel2:DISPLay ON",
                ":TRIGger:MODE EDGE",
                ":TRIGger:EDGE:SOURce CHANnel2",
                ":TRIGger:EDGE:SLOPe NEGative",
                ":TRIGger:EDGE:LEVel 2.0",
                ":TRIGger:EDGE:SWEep NORMal",
                ":WAVeform:SOURce CHANnel1",
                ":WAVeform:FORMat WORD",
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

    header = [":SYSTem:HEADer OFF",
                ":CHANnel1:INPut DCFifty",
                ":CHANnel1:DISPlay ON",
                ":CHANnel2:INPut DC"
                ":CHANnel2:DISPLay ON",
                ":CHANnel2:SCALe 1.0",
                ":TRIGger:MODE EDGE",
                ":TRIGger:EDGE:SOURce CHANnel2",
                ":TRIGger:EDGE:SLOPe NEGative",
                ":TRIGger:LEVel CHANnel2,2.0",
                ":TRIGger:SWEep TRIGgered",
                ":WAVeform:FORMat WORD",
                ":WAVeform:BYTeorder LSBFirst",
                ":WAVeform:SOURce CHANnel1",
                ":ACQuire:COMPlete 100"
                ]

    def currentSettings(self):

        self.visaInst.write(":TRIG:SWE AUTO")
        self.visaInst.write(":RUN")
        time.sleep(0.5)
        test = self.visaInst.ask_for_values(":WAVeform:PREamble?")
        self.visaInst.write(":TRIG:SWE TRIG")

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



class VisaScopeInterface(ScopeTemplate):
    dataUpdated = util.Signal()

    def __init__(self, showScriptParameter=None):
        super(VisaScopeInterface, self).__init__()
        self.scopetype = None
        self.datapoints = []

        try:
            mso54831d = VisaScopeInterface_MSO54831D(showScriptParameter=showScriptParameter)
        except ImportError:
            mso54831d = None

        try:
            dso1024A = VisaScopeInterface_DSO1024A(showScriptParameter=showScriptParameter)
        except ImportError:
            dso1024A = None

        self.setCurrentScope(mso54831d, False)
        defscope = mso54831d

        scope_cons = {}

        if mso54831d:
            mso54831d.paramListUpdated.connect(self.paramListUpdated)
            mso54831d.dataUpdated.connect(self.passUpdated)
            scope_cons["Agilent MSO 54831D"] = mso54831d

        if dso1024A:
            dso1024A.paramListUpdated.connect(self.paramListUpdated)
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
            self.paramListUpdated.emit()

    def con(self):
        if self.scopetype is not None:
            self.scopetype.con(self.findParam('connStr').value())
            self.connectStatus.setValue(True)


    def dis(self):
        if self.scopetype is not None:
            self.scopetype.dis()
            self.connectStatus.setValue(False)

    def doDataUpdated(self,  l, offset=0):
        self.datapoints = l
        self.offset = offset
        if len(l) > 0:
            self.dataUpdated.emit(l, offset)

    def arm(self):
        self.scopetype.arm()

    def capture(self, update=True, NumberPoints=None, waitingCallback=None):
        """Raises IOError if unknown failure, returns 'False' if successful, 'True' if timeout"""
        return self.scopetype.capture(update, NumberPoints, waitingCallback)

    def paramList(self):
        p = []
        p.append(self.params)

        if self.scopetype is not None:
            for a in self.scopetype.paramList(): p.append(a)

        #if self.advancedSettings is not None:
        #    for a in self.advancedSettings.paramList(): p.append(a)

        return p

    def validateSettings(self):
        return []

    def getName(self):
        return "VISA Scope"
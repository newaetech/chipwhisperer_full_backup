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
import chipwhisperer.capture.scopes.cwhardware.ChipWhispererDigitalPattern as ChipWhispererDigitalPattern
import chipwhisperer.capture.scopes.cwhardware.ChipWhispererExtra as ChipWhispererExtra
import chipwhisperer.capture.scopes.cwhardware.ChipWhispererSAD as ChipWhispererSAD
import chipwhisperer.capture.ui.qt as openadc_qt
from chipwhisperer.capture.scopes.ScopeTemplate import ScopeTemplate
from chipwhisperer.capture.scopes.cwhardware.ChipWhispererFWLoader import FWLoaderConfig
from chipwhisperer.capture.scopes.cwhardware.ChipWhispererFWLoader import CWCRev2_Loader
from chipwhisperer.capture.scopes.cwhardware.ChipWhispererFWLoader import CWLite_Loader
from chipwhisperer.capture.scopes.cwhardware.ChipWhispererFWLoaderGUI import FWLoaderConfigGUI
from chipwhisperer.capture.utils.AVRProgrammer import AVRProgrammerDialog
from chipwhisperer.capture.utils.XMEGAProgrammer import XMEGAProgrammerDialog
from chipwhisperer.common.api.config_parameter import ConfigParameter
from chipwhisperer.common.utils import util

try:
    # OrderedDict is new in 2.7
    from collections import OrderedDict
    dicttype = OrderedDict
except ImportError:
    dicttype = dict
try:
    import ftd2xx as ft
except:
    ft = None
    ft_str = sys.exc_info()

try:
    import usb
except ImportError:
    usb = None
    usb_str = sys.exc_info()

try:
    import serial
    import openadc.scan as scan
except ImportError:
    serial = None

try:
    import chipwhisperer.capture.scopes.cwhardware.ChipWhispererLite as CWL
except ImportError:
    CWL = None


def getInstance(*args):
    return OpenADCInterface(*args)


class OpenADCInterface_NAEUSBChip():
    paramListUpdated = util.Signal()

    def __init__(self, oadcInstance):
        ztexParams = [
                      # No Parameters for NAEUSBChip
                  ]

        self.ser = None
        self._toolActs = []

        if (openadc_qt is None) or (usb is None):
            missingInfo = ""
            if openadc_qt is None:
                missingInfo += "openadc.qt "
            if usb is None:
                missingInfo += " usb"
            raise ImportError("Needed imports for ChipWhisperer missing: %s" % missingInfo)
        else:
            self.cwFirmwareConfig = FWLoaderConfig(CWLite_Loader())
            self.scope = oadcInstance
            self.params = ConfigParameter.create_extended(self, name='OpenADC-NAEUSBChip', type='group', children=ztexParams)

        # if target_chipwhisperer_extra is not None:
        #    self.cwAdvancedSettings = target_chipwhisperer_extra.QtInterface()
        # else:
        #    self.cwAdvancedSettings = None

    def __del__(self):
        if self.ser != None:
            self.ser.close()

    def con(self):
        if self.ser == None:
            dev = CWL.CWLiteUSB()

            try:
                dev.con()
            except IOError, e:
                exctype, value = sys.exc_info()[:2]
                raise IOError("ChipWhisperer USB "+ str(exctype) + str(value))

            if dev is None:
                raise IOError("Could not open USB Device")
            
            self.cwFirmwareConfig.setInterface(dev)
            self.cwFirmwareConfig.loadRequired()

            self.cwliteXMEGA.setUSBInterface(dev)
            self.cwliteAVR.setUSBInterface(dev)

            self.ser = dev

        try:
            self.scope.con(self.ser)
            print("OpenADC Found, Connecting")
        except IOError, e:
            exctype, value = sys.exc_info()[:2]
            raise IOError("OpenADC: " + (str(exctype) + str(value)) + " - Did you download FPGA data to ChipWhisperer?")

    def dis(self):
        if self.ser != None:
            self.ser.close()
            self.ser = None

    def getTextName(self):
        try:
            return self.ser.name
        except:
            return "None?"

    def paramList(self):
        return [self.params]

    def guiActions(self, mainWindow):
        if not hasattr(self, 'cwliteXMEGA'):
            self.cwliteXMEGA = XMEGAProgrammerDialog(mainWindow)
        if not hasattr(self, 'cwliteAVR'):
            self.cwliteAVR = AVRProgrammerDialog(mainWindow)
        self.fwLoaderConfigGUI = FWLoaderConfigGUI(mainWindow, self.cwFirmwareConfig)
        return [['CW Firmware Preferences','Configure ChipWhisperer FW Paths', self.fwLoaderConfigGUI.show], # Can' use Config... name with MacOS
                ['Download CW Firmware', 'Download Firmware+FPGA To Hardware', self.cwFirmwareConfig.loadRequired],
                ['CW-Lite XMEGA Programmer', 'Open XMEGA Programmer (ChipWhisperer-Lite Only)',self.cwliteXMEGA.show],
                ['CW-Lite AVR Programmer', 'Open AVR Programmer (ChipWhisperer-Lite Only)',self.cwliteAVR.show]]

class OpenADCInterface_FTDI():
    paramListUpdated = util.Signal()

    def __init__(self, oadcInstance):
        ftdiParams = [
                      {'name':'Refresh Device List', 'type':'action', 'action':self.serialRefresh},
                      {'name':'Serial Number', 'type':'list', 'values':[''], 'value':None, 'set':self.setSerialNumber},
                  ]

        self.serialNumber = None

        if (openadc_qt is None) or (ft is None):
            self.ser = None
            raise ImportError("Needed imports for FTDI missing")
        else:
            self.ser = None
            self.scope = oadcInstance
            self.params = ConfigParameter.create_extended(self, name='OpenADC-FTDI', type='group', children=ftdiParams)

        #if target_chipwhisperer_extra is not None:
        #    self.cwAdvancedSettings = target_chipwhisperer_extra.QtInterface()
        #else:
        #    self.cwAdvancedSettings = None

    def setSerialNumber(self, snum):
        self.serialNumber = snum

    def __del__(self):
        if self.ser != None:
            self.ser.close()

    def con(self):
        if self.ser == None:
            try:
                self.dev = ft.openEx(str(self.serialNumber), ft.ftd2xx.OPEN_BY_SERIAL_NUMBER)
                self.dev.setBitMode(0x00, 0x40)
                self.dev.setTimeouts(500, 500)
                self.dev.setLatencyTimer(2)
                self.ser = self
            except ft.ftd2xx.DeviceError, e:
                self.ser = None
                raise IOError("Could not open %s: %s"%(self.serialNumber,e))

        try:
            self.scope.con(self.ser)
            print("OpenADC Found, Connecting")
        except IOError,e:
            exctype, value = sys.exc_info()[:2]
            raise IOError("OpenADC Error: %s"%(str(exctype) + str(value)) + " - " + e.message)

        #if self.cwAdvancedSettings:
        #    self.cwAdvancedSettings.setOpenADC(self.scope)

    def dis(self):
        if self.ser != None:
            self.ser.close()
            self.ser = None

    def serialRefresh(self):
        serialnames = ft.listDevices()
        if serialnames == None:
            serialnames = [" "]

        for p in self.params.children():
            if p.name() == 'Serial Number':
                p.setLimits(serialnames)
                p.setValue(serialnames[0])

        self.paramListUpdated.emit()

    def read(self, N=0, debug=False):
        return bytearray(self.dev.read(N))

    def write(self, data, debug=False):
        return self.dev.write(data)

    def getTextName(self):
        try:
            return self.ser.name
        except:
            return "None?"

    def paramList(self):
        p = [self.params]
        #if self.cwAdvancedSettings is not None:
        #    for a in self.cwAdvancedSettings.paramList(): p.append(a)
        return p

class OpenADCInterface_Serial():
    paramListUpdated = util.Signal()

    def __init__(self, oadcInstance):
        ftdiParams = [
                      {'name':'Refresh List', 'type':'action', 'action':self.serialRefresh},
                      {'name':'Port', 'type':'list', 'values':[''], 'value':None, 'set':self.setPortName},
                  ]

        self.ser = None

        if (openadc_qt is None) or (serial is None):
            self.ser = None
            raise ImportError("Needed imports for serial missing")
        else:
            self.ser = None
            self.scope = oadcInstance
            self.params = ConfigParameter.create_extended(self, name='OpenADC-Serial', type='group', children=ftdiParams)

    def setPortName(self, snum):
        self.portName = snum

    def __del__(self):
        if self.ser != None:
            self.ser.close()

    def con(self):
        if self.ser == None:
            self.ser = serial.Serial()
            self.ser.port     = self.portName
            self.ser.baudrate = 512000
            self.ser.timeout  = 2     # 2 second timeout

            attempts = 4
            while attempts > 0:
                try:
                    self.ser.open()
                    attempts = 0
                except serial.SerialException, e:
                    attempts = attempts - 1
                    self.ser = None
                    if attempts == 0:
                        raise IOError("Could not open %s"%self.ser.name)

        try:
            self.scope.con(self.ser)
            print("OpenADC Found, Connecting")
        except IOError,e:
            exctype, value = sys.exc_info()[:2]
            raise IOError("OpenADC Error (Serial Port): %s"%(str(exctype) + str(value)))

    def dis(self):
        if self.ser != None:
            self.ser.close()
            self.ser = None

    def serialRefresh(self):
        serialnames = scan.scan()
        if serialnames == None:
            serialnames = [" "]

        for p in self.params.children():
            if p.name() == 'Port':
                p.setLimits(serialnames)
                p.setValue(serialnames[0])

        self.paramListUpdated.emit()

    def getTextName(self):
        try:
            return self.ser.name
        except:
            return "None?"

    def paramList(self):
        p = [self.params]
        return p


class OpenADCInterface_ZTEX():
    paramListUpdated = util.Signal()

    def __init__(self, oadcInstance):
        ztexParams = [
                      #No Parameters for ZTEX
                  ]

        self.ser = None
        self._toolActs = []

        if (openadc_qt is None) or (usb is None):
            missingInfo = ""
            if openadc_qt is None:
                missingInfo += "openadc.qt "
            if usb is None:
                missingInfo += " usb"
            raise ImportError("Needed imports for ChipWhisperer missing: %s" % missingInfo)
        else:
            self.scope = oadcInstance
            self.params = ConfigParameter.create_extended(self, name='OpenADC-ZTEX', type='group', children=ztexParams)
            self.cwFirmwareConfig = FWLoaderConfig(CWCRev2_Loader())

        #if target_chipwhisperer_extra is not None:
        #    self.cwAdvancedSettings = target_chipwhisperer_extra.QtInterface()
        #else:
        #    self.cwAdvancedSettings = None

    def __del__(self):
        if self.ser != None:
            self.ser.close()

    def con(self):
        if self.ser == None:

            # Download firmware if required
            self.cwFirmwareConfig.loadRequired()

            try:
                dev = usb.core.find(idVendor=0x221A, idProduct=0x0100)
            except IOError, e:
                exctype, value = sys.exc_info()[:2]
                raise IOError("FX2 Port " +  str(exctype) + str(value))

            if dev is None:
                raise IOError("FX2 Port. Could not open USB Device")

            dev.set_configuration()

            self.dev = dev
            self.writeEP = 0x06
            self.readEP = 0x82
            self.interface = 0
            self.ser = self

        try:
            self.scope.con(self.ser)
            print("OpenADC Found, Connecting")
        except IOError,e:
            exctype, value = sys.exc_info()[:2]
            raise IOError("OpenADC Error (FX2 Port): " + (str(exctype) + str(value)) + " - Did you download firmware/FPGA data to ChipWhisperer?")

    def dis(self):
        if self.ser != None:
            self.ser.close()
            self.ser = None

    def read(self, N=0, debug=False):
        try:
            # self.interface removed from call for latest API compatability
            data = self.dev.read(self.readEP, N, timeout=100)
        except IOError:
            return []

        data = bytearray(data)
        if debug:
            print "RX: ",
            for b in data:
                print "%02x "%b,
            print ""
        return data

    def write(self, data, debug=False):
        data = bytearray(data)
        if debug:
            print "TX: ",
            for b in data:
                print "%02x "%b,
            print ""
        # self.interface removed from call for latest API compatability
        self.dev.write(self.writeEP, data, timeout=500)

    def getTextName(self):
        try:
            return self.ser.name
        except:
            return "None?"

    def paramList(self):
        p = [self.params]
        return p

    def guiActions(self, mainWindow):
        self.fwLoaderConfigGUI = FWLoaderConfigGUI(mainWindow, self.cwFirmwareConfig)
        return [['CW Firmware Preferences','Configure ChipWhisperer FW Paths', self.fwLoaderConfigGUI.show],  # Can' use Config/Setup... name with MacOS
               ['Download CW Firmware','Download Firmware+FPGA To Hardware', self.cwFirmwareConfig.loadRequired]]


class OpenADCInterface(ScopeTemplate):
    dataUpdated = util.Signal()

    def __init__(self):
        super(OpenADCInterface, self).__init__()
        self.qtadc = openadc_qt.OpenADCQt(includePreview=False,  setupLayout=False)
        self.qtadc.setupParameterTree(False)
        self.qtadc.dataUpdated.connect(self.doDataUpdated)
        self.scopetype = None
        self.datapoints = []

        try:
            cwrev2 = OpenADCInterface_ZTEX(self.qtadc)
        except ImportError, e:
            print "Failed to enable CWRev2, Error: %s" % str(e)
            cwrev2 = None

        try:
            ftdi = OpenADCInterface_FTDI(self.qtadc)
        except ImportError:
            ftdi = None

        try:
            cwser = OpenADCInterface_Serial(self.qtadc)
        except ImportError:
            cwser = None

        try:
            cwlite = OpenADCInterface_NAEUSBChip(self.qtadc)
        except ImportError, e:
            print "Failed to enable CW-Lite, Error: %s" % str(e)
            cwlite = None

        self.setCurrentScope(cwrev2, False)
        defscope = cwrev2

        cw_cons = dicttype()

        if cwrev2:
            cwrev2.paramListUpdated.connect(self.paramListUpdated.emit)
            cw_cons["ChipWhisperer Rev2"] = cwrev2

        if cwlite:
            cwlite.paramListUpdated.connect(self.paramListUpdated.emit)
            cw_cons["ChipWhisperer Lite"] = cwlite

        if ftdi:
            ftdi.paramListUpdated.connect(self.paramListUpdated.emit)
            cw_cons["FTDI (SASEBO-W/SAKURA-G)"] = ftdi

        if cwser:
            cwser.paramListUpdated.connect(self.paramListUpdated.emit)
            cw_cons["Serial Port (LX9)"] = cwser

        if cw_cons == {}:
            # If no scopes could be found, add a dummy entry so the
            # app can at least start up
            cw_cons["None"] = None
            print("OpenADC: No supported scope found!")


        # Bonus Modules for ChipWhisperer
        self.advancedSettings = None
        self.advancedSAD = None
        self.digitalPattern = None

        scopeParams = [{'name':'Connection', 'type':'list', 'values':cw_cons, 'value':defscope, 'set':self.setCurrentScope},
                       # {'name':'Auto-Refresh DCM Status', 'type':'bool', 'value':True, 'set':self.setAutorefreshDCM}
                      ]

        self.params = ConfigParameter.create_extended(self, name='OpenADC Interface', type='group', children=scopeParams)
        self.setCurrentScope(defscope)

    #     self.refreshTimer = QTimer()
    #     self.refreshTimer.timeout.connect(self.dcmTimeout)
    #     self.refreshTimer.setInterval(1000)
    #
    # def dcmTimeout(self):
    #     if self.parent:
    #         self.parent.setParameter(['OpenADC', 'Clock Setup', 'Refresh Status', None])
    #         self.parent.setParameter(['OpenADC', 'Trigger Setup', 'Refresh Status', None])

    # def setAutorefreshDCM(self, enabled):
    #     if enabled:
    #         self.refreshTimer.start()
    #     else:
    #         self.refreshTimer.stop()

    def setCurrentScope(self, scope, update=True):
        self.scopetype = scope
        if update:
            self.paramListUpdated.emit()

    def con(self):
        if self.scopetype is not None:
            self.scopetype.con()
            # self.refreshTimer.start()

            # TODO Fix this hack
            if hasattr(self.scopetype, "ser") and hasattr(self.scopetype.ser, "_usbdev"):
                self.qtadc.sc.usbcon = self.scopetype.ser._usbdev

            if "ChipWhisperer" in self.qtadc.sc.hwInfo.versions()[2]:

                if "Lite" in self.qtadc.sc.hwInfo.versions()[2]:
                    cwtype = "cwlite"
                else:
                    cwtype = "cwrev2"

                #For OpenADC: If we have CW Stuff, add that now
                self.advancedSettings = ChipWhispererExtra.ChipWhispererExtra(cwtype, self.scopetype)
                self.advancedSettings.setOpenADC(self.qtadc)

                util.chipwhisperer_extra = self.advancedSettings

                if "Lite" not in self.qtadc.sc.hwInfo.versions()[2]:
                    self.advancedSAD = ChipWhispererSAD.ChipWhispererSAD()
                    self.advancedSAD.setOpenADC(self.qtadc)

                    self.digitalPattern = ChipWhispererDigitalPattern.ChipWhispererDigitalPattern()
                    self.digitalPattern.setOpenADC(self.qtadc)

                self.paramListUpdated.emit()

            self.connectStatus.setValue(True)

    def dis(self):
        if self.scopetype is not None:
            self.scopetype.dis()
            # self.refreshTimer.stop()
            self.connectStatus.setValue(False)

    def doDataUpdated(self,  l, offset=0):
        self.datapoints = l
        self.offset = offset
        if len(l) > 0:
            self.dataUpdated.emit(l, offset)

    def arm(self):
        if self.connectStatus.value() is False:
            raise Exception("Scope \"" + self.getName() + "\" is not connected. Connect it first...")
        # self.advancedSettings.glitch.resetDCMs()
        if self.advancedSettings:
            self.advancedSettings.armPreScope()

        try:
            self.qtadc.arm()
        except Exception, e:
            self.dis()
            raise e

        if self.advancedSettings:
            self.advancedSettings.armPostScope()

    def capture(self, update=True, NumberPoints=None, waitingCallback=None):
        """Raises IOError if unknown failure, returns 'True' if timeout, 'False' if no timeout"""
        return self.qtadc.capture(update, NumberPoints, waitingCallback)

    def paramList(self):
        p = []
        p.append(self.params)

        if self.scopetype is not None:
            for a in self.scopetype.paramList(): p.append(a)

        p.append(self.qtadc.params)

        # TODO: make this one list with modules that can be added elsewhere
        if self.advancedSettings is not None:
            for a in self.advancedSettings.paramList(): p.append(a)

        if self.advancedSAD is not None:
            for a in self.advancedSAD.paramList(): p.append(a)

        if self.digitalPattern is not None:
            for a in self.digitalPattern.paramList(): p.append(a)

        return p

    def guiActions(self, mainWindow):
        if self.scopetype and hasattr(self.scopetype, "guiActions"):
            return self.scopetype.guiActions(mainWindow)
        else:
            return []

    def validateSettings(self):
        return []

    def getName(self):
        return "ChipWhisperer/OpenADC"


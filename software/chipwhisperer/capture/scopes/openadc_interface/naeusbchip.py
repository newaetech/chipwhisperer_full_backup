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

import chipwhisperer.capture.scopes._qt as openadc_qt
from chipwhisperer.capture.scopes.cwhardware.ChipWhispererFWLoader import CWLite_Loader
from chipwhisperer.capture.scopes.cwhardware.ChipWhispererFWLoader import FWLoaderConfig
from chipwhisperer.capture.scopes.cwhardware.ChipWhispererFWLoaderGUI import FWLoaderConfigGUI
from chipwhisperer.capture.utils.AVRProgrammer import AVRProgrammerDialog
from chipwhisperer.capture.utils.XMEGAProgrammer import XMEGAProgrammerDialog
from chipwhisperer.common.utils.pluginmanager import Plugin
from chipwhisperer.common.utils.parameter import Parameterized, Parameter

try:
    import chipwhisperer.capture.scopes.cwhardware.ChipWhispererLite as CWL
except ImportError:
    CWL = None

try:
    import usb
except ImportError:
    usb = None


class OpenADCInterface_NAEUSBChip(Parameterized, Plugin):
    _name = "ChipWhisperer-Lite"

    def __init__(self, parentParam, oadcInstance):
        self.ser = None
        self._toolActs = []

        self.params = Parameter(name=self.getName(), type='group')

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

    def __del__(self):
        if self.ser:
            self.ser.close()

    def con(self):
        if self.ser == None:
            dev = CWL.CWLiteUSB()

            try:
                dev.con()
            except IOError as e:
                exctype, value = sys.exc_info()[:2]
                raise IOError("ChipWhisperer USB "+ str(exctype) + str(value))

            self.cwFirmwareConfig.setInterface(dev.fpga)
            self.cwFirmwareConfig.loadRequired()

            if hasattr(self, 'cwliteXMEGA'):
                self.cwliteXMEGA.setUSBInterface(dev.xmega)

            if hasattr(self, 'cwliteAVR'):
                self.cwliteAVR.setUSBInterface(dev.avr)

            self.ser = dev.usbdev()

        try:
            self.scope.con(self.ser)
            print("OpenADC Found, Connecting")
        except IOError, e:
            exctype, value = sys.exc_info()[:2]
            raise IOError("OpenADC: " + (str(exctype) + str(value)))

    def dis(self):
        if self.ser != None:
            self.ser.close()
            self.ser = None

    def getTextName(self):
        try:
            return self.ser.name
        except:
            return "None?"

    def setupGuiActions(self, mainWindow):
        if not hasattr(self, 'cwliteXMEGA'):
            self.cwliteXMEGA = XMEGAProgrammerDialog(mainWindow)
        if not hasattr(self, 'cwliteAVR'):
            self.cwliteAVR = AVRProgrammerDialog(mainWindow)
        self.fwLoaderConfigGUI = FWLoaderConfigGUI(mainWindow, self.cwFirmwareConfig)
        return [['CW Firmware Preferences','Configure ChipWhisperer FW Paths', self.fwLoaderConfigGUI.show], # Can' use Config... name with MacOS
                ['Download CW Firmware', 'Download Firmware+FPGA To Hardware', self.cwFirmwareConfig.loadRequired],
                ['CW-Lite XMEGA Programmer', 'Open XMEGA Programmer (ChipWhisperer-Lite Only)',self.cwliteXMEGA.show],
                ['CW-Lite AVR Programmer', 'Open AVR Programmer (ChipWhisperer-Lite Only)',self.cwliteAVR.show]]

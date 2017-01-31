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
import logging
import sys
import chipwhisperer.capture.scopes._qt as openadc_qt
from chipwhisperer.capture.scopes.cwhardware.ChipWhispererFWLoader import CWLite_Loader, CW1200_Loader
from chipwhisperer.capture.scopes.cwhardware.ChipWhispererFWLoader import FWLoaderConfig
from chipwhisperer.capture.scopes.cwhardware.ChipWhispererFWLoaderGUI import FWLoaderConfigGUI
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
    _name = "NewAE USB (CWLite/CW1200)"

    def __init__(self, oadcInstance):
        self.ser = None
        self.dev = None
        self.scope = None

        self.getParams().addChildren([
            {'name':"CW Firmware Preferences", 'tip':"Configure ChipWhisperer FW Paths", 'type':"menu", "action":lambda _:self.getFwLoaderConfigGUI().show()}, # Can' use Config... name with MacOS
            {'name':"Download CW Firmware", 'tip':"Download Firmware+FPGA To Hardware", 'type':"menu", "action":lambda _:self.cwFirmwareConfig.loadRequired()},
        ])

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

    def con(self):
        if self.ser is None:
            self.dev = CWL.CWLiteUSB()
            self.getParams().append(self.dev.getParams())

            try:
                found_id = self.dev.con(idProduct=[0xACE2, 0xACE3])
            except IOError:
                raise Warning('Could not connect to "%s". It may have been disconnected or is being used by another tool.' % self.getName())

            if (found_id == 0xACE3):
                logging.warning('Found CW1200. FPGA dialog being switched, if you made changes they are lost. '
                                'If you need a different bitstream loaded, edit the dialog now and reconnect.')

                self.cwFirmwareConfig = FWLoaderConfig(CW1200_Loader())

            self.cwFirmwareConfig.setInterface(self.dev.fpga)
            try:
                self.cwFirmwareConfig.loadRequired()
            except:
                self.dev.dis()
                self.dev.usbdev().close()
                raise
            self.ser = self.dev.usbdev()

        try:
            self.scope.con(self.ser)
            logging.info('OpenADC Found, Connecting')
        except IOError, e:
            exctype, value = sys.exc_info()[:2]
            raise IOError("OpenADC: " + (str(exctype) + str(value)))

    def dis(self):
        if self.ser is not None:
            self.cwFirmwareConfig.setInterface(None)
            self.scope.close()
            self.ser.close()
            self.ser = None
        if self.dev is not None:
            self.dev.dis()
            self.dev = None

    def __del__(self):
        if self.ser is not None:
            self.ser.close()

    def getFwLoaderConfigGUI(self):
        if not hasattr(self, 'fwLoaderConfigGUI'):
            self.fwLoaderConfigGUI = FWLoaderConfigGUI(self.cwFirmwareConfig)
        return self.fwLoaderConfigGUI

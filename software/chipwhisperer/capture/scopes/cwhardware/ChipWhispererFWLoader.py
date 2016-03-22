#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2016, NewAE Technology Inc
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

from chipwhisperer.common.api.CWCoreAPI import CWCoreAPI
from chipwhisperer.capture.scopes.cwhardware.ztex_fwloader import Ztex1v1, IhxFile
import time
import zipfile
import os.path


class CWCRev2_Loader(object):
    def __init__(self):
        self.name = "cwcrev2"
        self.driver = Ztex1v1()
        self.fwFLoc = CWCoreAPI.getInstance().getRootDir() + "/../../../hardware/capture/chipwhisperer-rev2/ezusb-firmware/ztex-sdk/examples/usb-fpga-1.11/1.11c/openadc/OpenADC.ihx"
        self.bsZipLoc = CWCoreAPI.getInstance().getRootDir() + "/../../../hardware/capture/chipwhisperer-rev2/cwrev2_firmware.zip"
        self.bsZipLoc_filename = "interface.bit"
        self.bsLoc = CWCoreAPI.getInstance().getRootDir() + "/../../../hardware/capture/chipwhisperer-rev2/hdl/ztex_rev2_1.11c_ise/interface.bit"

    def loadRequired(self, callback, forceFirmware=False):
        self.driver.probe(True)

        if self.driver.firmwareProgrammed == False or forceFirmware:
            self.loadFirmware(self.fwFLoc)
        else:
            print "EZ-USB Microcontroller: Skipped firmware download (already done)"

        if self.driver.deviceInfo["interfaceVersion"] != 1:
            raise IOError("Unknown interface version, invalid ZTEX Firmware?. Device info: %s" % str(self.driver.deviceInfo))

        if self.driver.deviceInfo["productId"] != [10, 12, 0, 0]:
            raise IOError("Unknown productId, invalid ZTEX Firmware/Module?. Device info: %s" % str(self.driver.deviceInfo))

        self.driver.getFpgaState()
        if self.driver.fpgaConfigured == False:
            callback()
            self.driver.getFpgaState()
            print "FPGA: Programmed bitstream successfully"
        else:
            print "FPGA: Skipped configuration (already done)"

    def loadFirmware(self, file):
        f = IhxFile(file)
        self.driver.uploadFirmware(f)
        time.sleep(1)
        self.driver.probe()

    def loadFPGA(self, file):
        self.driver.configureFpgaLS(file.open("interface.bit"))


class CWLite_Loader(object):
    def __init__(self):
        self.name = "cwlite"
        self.driver = None
        self.fwFLoc = ""
        self.bsZipLoc = CWCoreAPI.getInstance().getRootDir() + "/../../../hardware/capture/chipwhisperer-lite/cwlite_firmware.zip"
        self.bsZipLoc_filename = "cwlite_interface.bit"
        self.bsLoc = CWCoreAPI.getInstance().getRootDir() + "/../../../hardware/capture/chipwhisperer-lite/hdl/cwlite_ise/cwlite_interface.bit"

    def loadRequired(self, callback, forceFirmware=False):
        callback()

    def loadFPGA(self, file):
        self.driver.FPGAProgram(file)

    def setInterface(self, driver):
        self.driver = driver


class FWLoaderConfig(object):
    def __init__(self, loader):
        self.loader = loader
        self.useFPGAZip = True

    def loadRequired(self, forceFirmware=False):
        """Load firmware file or FPGA file only as required, skip otherwise"""
        self.loader.loadRequired(self.loadFPGA, forceFirmware)

    def loadFirmware(self):
        if self.loader.driver is None: raise Exception("Driver not loaded. Connect hardware before loading.")

        if hasattr(self.loader, 'loadFirmware'):
            self.loader.loadFirmware(self.loader.fwFLoc)
            print "Firmware loaded"

    def setFPGAMode(self, useFPGAZip):
        self.useFPGAZip = useFPGAZip

    def loadFPGA(self):
        """Load the FPGA bitstream"""
        if self.loader.driver is None: raise Exception("Driver not loaded. Connect hardware before loading.")

        if self.useFPGAZip:
            zfile = zipfile.ZipFile(self.loader.bsZipLoc, "r")
            self.loader.loadFPGA(zfile.open(self.loader.bsZipLoc_filename))

        else:
            # Get bitstream date, print for user
            bsdate = os.path.getmtime(self.loader.bsLoc)
            print "FPGA: DEBUG MODE: Using .bit file, date: %s" % time.ctime(bsdate)
            self.loader.loadFPGA(open(self.loader.bsLoc, "rb"))
        print "FPGA programmed"

    def setInterface(self, dev):
        self.loader.setInterface(dev)

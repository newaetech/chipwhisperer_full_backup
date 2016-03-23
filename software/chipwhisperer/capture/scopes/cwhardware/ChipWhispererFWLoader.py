#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2016, NewAE Technology Inc
# All rights reserved.
#
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

class CW_Loader(object):
    """ Base class for ChipWhisperer targets that help loading of FPGA data """
    
    def fpga_bitstream_date(self):
        """ In 'debug' mode returns date bitstream was modified, returns 'None' in release mode """
        
        if self._release_mode:
            return None
        else:
            bsdate = os.path.getmtime(self._bsLoc)
            return time.ctime(bsdate)
        
    def fpga_bitstream(self):
        """ Returns FPGA bitstream in use (either debug or release) """
        
        if self._release_mode:
            if not os.path.isfile(self._bsZipLoc):
                raise IOError("FPGA Zip-File NOT set to valid value - check paths or reconfigure. Path='%s'"%self._bsZipLoc)
            zfile = zipfile.ZipFile(self._bsZipLoc, "r")
            return zfile.open(self._bsZipLoc_filename)
        else:
            if not os.path.isfile(self._bsLoc):
                raise IOError("FPGA bit-File NOT set to valid value - check paths or reconfigure. Path='%s'"%self._bsLoc)
            return open(self._bsLoc, "rb")
            
    def setFPGAMode(self, release_mode):
        """ Release mode uses a zip-file with everything, otherwise expect seperate .bit file """
        
        self._release_mode = release_mode

    

class CWCRev2_Loader(CW_Loader):
    def __init__(self):
        self.name = "cwcrev2"
        self.driver = Ztex1v1()
        self._fwFLoc = os.path.join(CWCoreAPI.getInstance().getRootDir(), os.path.normpath("../../../hardware/capture/chipwhisperer-rev2/ezusb-firmware/ztex-sdk/examples/usb-fpga-1.11/1.11c/openadc/OpenADC.ihx"))
        self._bsZipLoc = os.path.join(CWCoreAPI.getInstance().getRootDir(), os.path.normpath("../../../hardware/capture/chipwhisperer-rev2/cwrev2_firmware.zip"))
        self._bsZipLoc_filename = "interface.bit"
        self._bsLoc = os.path.join(CWCoreAPI.getInstance().getRootDir(), os.path.normpath("../../../hardware/capture/chipwhisperer-rev2/hdl/ztex_rev2_1.11c_ise/interface.bit"))
    
    def loadRequired(self, callback, forceFirmware=False):
        self.driver.probe(True)

        if self.driver.firmwareProgrammed == False or forceFirmware:
            self.loadFirmware()
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

    def loadFirmware(self):
        if not os.path.isfile(self._fwFLoc):
                raise IOError("Firmware ihx-File NOT set to valid value - check paths or reconfigure. Path='%s'"%self._fwFLoc)
        f = IhxFile(self._fwFLoc)
        self.driver.uploadFirmware(f)
        time.sleep(1)
        self.driver.probe()

    def loadFPGA(self, filelike):
        self.driver.configureFpgaLS(self.fpga_bitstream())


class CWLite_Loader(CW_Loader):
    def __init__(self):
        self.name = "cwlite"
        self.driver = None
        self._bsZipLoc = os.path.join(CWCoreAPI.getInstance().getRootDir(), os.path.normpath("../../../hardware/capture/chipwhisperer-lite/cwlite_firmware.zip"))
        self._bsZipLoc_filename = "cwlite_interface.bit"
        self._bsLoc = os.path.join(CWCoreAPI.getInstance().getRootDir(), os.path.normpath("../../../hardware/capture/chipwhisperer-lite/hdl/cwlite_ise/cwlite_interface.bit"))


    def loadRequired(self, callback, forceFirmware=False):
        callback()

    def loadFPGA(self):
        self.driver.FPGAProgram(self.fpga_bitstream())

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
            self.loader.loadFirmware()
            print "Firmware loaded"

    def setFPGAMode(self, useFPGAZip):
        self.loader.setFPGAMode(useFPGAZip)

    def loadFPGA(self):
        """Load the FPGA bitstream"""
        if self.loader.driver is None: raise Exception("Driver not loaded. Connect hardware before loading.")

        #Print if in debug mode
        if self.loader.fpga_bitstream_date():
            strdate = self.fpga_bitstream_date()
            print "FPGA: DEBUG MODE: Using .bit file, date: %s" % strdate
        
        self.loader.loadFPGA(self.loader.fpga_bitstream())
        print "FPGA programmed"

    def setInterface(self, dev):
        self.loader.setInterface(dev)

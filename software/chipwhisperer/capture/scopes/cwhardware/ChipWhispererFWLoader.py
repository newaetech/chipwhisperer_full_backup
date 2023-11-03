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
import logging
import time
import zipfile
import os.path

#from chipwhisperer.capture.scopes.cwhardware.ztex_fwloader import Ztex1v1, IhxFile
from ....common.utils import util
from ....hardware.firmware.cwlite import getsome as cwlite_getsome
from ....hardware.firmware.cwcr2 import getsome as cwcr2_getsome
from ....hardware.firmware.cw1200 import getsome as cw1200_getsome
from ....hardware.firmware.cwhusky import getsome as husky_getsome

from chipwhisperer.common.api.settings import Settings

from chipwhisperer.logging import *

class CW_Loader:
    """ Base class for ChipWhisperer targets that help loading of FPGA data """
    name = ""

    def __init__(self):
        self._release_mode = self.read_setting("fpga-bitstream-mode","builtin")
        self._fwFLoc = ""
        self._bsLoc = " "
        self._bsZipLoc = ""
        self._bsZipLoc_filename = ""
        self._bsBuiltinData = None
        self._registers = None

    def read_setting(self, settingname, default):
        """ Returns a setting if saved, otherwise defaults """

        fullsettingname = self.name + "-" + settingname
        return Settings().value(fullsettingname, default)

    def write_setting(self, settingname, value):
        """ Saves a setting """

        fullsettingname = self.name + "-" + settingname
        Settings().setValue(fullsettingname, value)

    def save_bsLoc(self):
        self.write_setting('debugbitstream-location', self._bsLoc)

    def save_bsZipLoc(self):
        self.write_setting('zipbitstream-location', self._bsZipLoc)

    def fpga_bitstream_date(self):
        """ In 'debug' mode returns date bitstream was modified, returns 'None' in release mode """

        if self._release_mode != "debug":
            return None
        else:
            bsdate = os.path.getmtime(self._bsLoc)
            return time.ctime(bsdate)

    def fpga_bitstream(self):
        """ Returns FPGA bitstream in use (either debug or release) """
        if self._release_mode == "builtin":
            scope_logger.debug('FPGA: Builtin mode')
            filelike = self._bsBuiltinData
            zfile = zipfile.ZipFile(filelike)
            return zfile.open(self._bsZipLoc_filename)
        elif self._release_mode == "zipfile":
            scope_logger.debug('FPGA: Zipfile mode')
            if not os.path.isfile(self._bsZipLoc):
                raise IOError("FPGA Zip-File NOT set to valid value - check paths or reconfigure. Path='%s'"%self._bsZipLoc)
            zfile = zipfile.ZipFile(self._bsZipLoc, "r")
            scope_logger.debug("FPGA: Using file %s"%self._bsZipLoc)
            return zfile.open(self._bsZipLoc_filename)
        elif self._release_mode == "debug":
            scope_logger.debug('FPGA: Raw bitstream mode')
            if not os.path.isfile(self._bsLoc):
                raise IOError("FPGA bit-File NOT set to valid value - check paths or reconfigure. Path='%s'"%self._bsLoc)

            scope_logger.debug("FPGA: Using file %s"%self._bsLoc)
            return open(self._bsLoc, "rb")
        else:
            raise ValueError("Internal Error - self._release_mode set to invalid value: %s"%str(self._release_mode))

    def setFPGAMode(self, release_mode):
        """
        Selects where configuration data comes from:
        * 'builtin' uses data from python release.
        * 'zipfile' uses seperate zip-file with everything
        * 'debug' expects a seperate .bit file
        """

        if (release_mode != "builtin") and (release_mode != "debug") and (release_mode != "zipfile"):
            scope_logger.warning('FPGA mode switched to \'builtin\' from invalid setting of \'%s\'' % release_mode)
            release_mode = "builtin"

        self.write_setting("fpga-bitstream-mode", release_mode)
        self._release_mode = release_mode


class CWLite_Loader(CW_Loader):
    name = "cwlite"

    def __init__(self):
        super(CWLite_Loader, self).__init__()
        self.driver = None

        def_bsZipLoc = os.path.join(util.getRootDir(), os.path.normpath("../hardware/capture/chipwhisperer-lite/cwlite_firmware.zip"))
        def_bsLoc = os.path.join(util.getRootDir(), os.path.normpath("../hardware/capture/chipwhisperer-lite/hdl/cwlite_interface.bit"))

        self._bsZipLoc = self._bsZipLoc = self.read_setting('zipbitstream-location', def_bsZipLoc)
        self._bsZipLoc_filename = "cwlite_interface.bit"
        self._bsLoc = self.read_setting('debugbitstream-location', def_bsLoc)
        self._fwFLoc = ""
        self._bsBuiltinData = cwlite_getsome("cwlite_firmware.zip", filelike=True)
        self._registers = cwlite_getsome("registers.v")

    def loadRequired(self, callback, forceFirmware=False):
        callback()

    def loadFPGA(self):
        self.save_bsLoc()
        self.save_bsZipLoc()

        if self.driver.isFPGAProgrammed() is False:
            self.driver.FPGAProgram(self.fpga_bitstream())
        else:
            scope_logger.info("FPGA Configuration skipped - detected already programmed")

    def setInterface(self, driver):
        self.driver = driver


class CW1200_Loader(CW_Loader):
    name = "cw1200"

    def __init__(self):
        super(CW1200_Loader, self).__init__()
        self.driver = None

        def_bsZipLoc = os.path.join(util.getRootDir(), os.path.normpath("../hardware/capture/chipwhisperer-cw1200/cw1200_firmware.zip"))
        def_bsLoc = os.path.join(util.getRootDir(), os.path.normpath("../hardware/capture/chipwhisperer-cw1200/hdl/cw1200_ise/cw1200_interface.bit"))

        self._bsZipLoc = self._bsZipLoc = self.read_setting('zipbitstream-location', def_bsZipLoc)
        self._bsZipLoc_filename = "cw1200_interface.bit"
        self._bsLoc = self.read_setting('debugbitstream-location', def_bsLoc)
        self._fwFLoc = ""
        self._bsBuiltinData = cw1200_getsome("cw1200_firmware.zip", filelike=True)
        self._registers = cwlite_getsome("registers.v")


    def loadRequired(self, callback, forceFirmware=False):
        callback()

    def loadFPGA(self):
        self.save_bsLoc()
        self.save_bsZipLoc()

        if self.driver.isFPGAProgrammed() == False:
            self.driver.FPGAProgram(self.fpga_bitstream())
        else:
            scope_logger.info("FPGA Configuration skipped - detected already programmed")

    def setInterface(self, driver):
        self.driver = driver

class CWHusky_Loader(CW_Loader):
    name = "husky"

    def __init__(self):
        super().__init__()
        self.driver = None

        def_bsZipLoc = os.path.join(util.getRootDir(), os.path.normpath("../hardware/capture/chipwhisperer-cw1200/cw1200_firmware.zip"))
        def_bsLoc = os.path.join(util.getRootDir(), os.path.normpath("../hardware/capture/chipwhisperer-cw1200/hdl/cw1200_ise/cw1200_interface.bit"))

        self._bsZipLoc = self._bsZipLoc = self.read_setting('zipbitstream-location', def_bsZipLoc)
        self._bsZipLoc_filename = "cwhusky_top.bit"
        self._bsLoc = self.read_setting('debugbitstream-location', def_bsLoc)
        self._fwFLoc = ""
        self._bsBuiltinData = husky_getsome("husky_firmware.zip", filelike=True)
        self._registers = husky_getsome("registers.v")


    def loadRequired(self, callback, forceFirmware=False):
        callback()

    def loadFPGA(self):
        self.save_bsLoc()
        self.save_bsZipLoc()

        if self.driver.isFPGAProgrammed() == False:
            self.driver.FPGAProgram(self.fpga_bitstream())
        else:
            logging.info("FPGA Configuration skipped - detected already programmed")

    def setInterface(self, driver):
        self.driver = driver

class FWLoaderConfig(object):
    def __init__(self, loader):
        self.loader = loader
        self.firmware_mode = self.loader._release_mode #"builtin"

    def loadRequired(self, forceFirmware=False):
        """Load firmware file or FPGA file only as required, skip otherwise"""
        self.loader.loadRequired(self.loadFPGA, forceFirmware)

    def loadFirmware(self):
        if self.loader.driver is None: raise Warning("Driver not loaded. Connect hardware before loading.")

        if hasattr(self.loader, 'loadFirmware'):
            self.loader.loadFirmware()
            scope_logger.info('Firmware loaded')

    def setFPGAMode(self, fpgamode):
        self.loader.setFPGAMode(fpgamode)
        self.firmware_mode = self.loader._release_mode

    def setFPGABitstream(self, bsLoc):
        """Set a manual bitstream file to load"""
        self.loader.setFPGAMode("debug")
        self.loader._bsLoc = bsLoc

    def loadFPGA(self):
        """Load the FPGA bitstream specified previously"""
        if self.loader.driver is None: raise Warning("Driver not loaded. Connect hardware before loading.")

        #Print if in debug mode
        if self.loader.fpga_bitstream_date():
            strdate = self.loader.fpga_bitstream_date()
            scope_logger.info('FPGA: DEBUG MODE: Using .bit file, date: %s' % strdate)

        self.loader.loadFPGA()
        scope_logger.info('FPGA programmed')

    def setInterface(self, dev):
        self.loader.setInterface(dev)

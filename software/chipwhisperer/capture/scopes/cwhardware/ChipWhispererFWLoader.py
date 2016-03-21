#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2014, NewAE Technology Inc
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


import os.path
import time
import zipfile
from chipwhisperer.capture.scopes.cwhardware.ztex_fwloader import Ztex1v1, IhxFile
from chipwhisperer.capture.scopes.cwhardware.ChipWhispererSAM3Update import SAM3LoaderConfig
from chipwhisperer.common.api.CWCoreAPI import CWCoreAPI


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
        if hasattr(self.loader, 'loadFirmware'):
            self.loader.loadFirmware(self.loader.fwFLoc)
            print "Firmware loaded"

    def setFPGAMode(self, useFPGAZip):
        self.useFPGAZip = useFPGAZip

    def loadFPGA(self):
        """Load the FPGA bitstream"""
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


from PySide.QtGui import *
from PySide.QtCore import *

class FWLoaderConfigGUI(QDialog):
    def __init__(self, parent, fwLoaderConfig):
        super(FWLoaderConfigGUI, self).__init__(parent)
        self.fwLoaderConfig = fwLoaderConfig
        self.setWindowTitle("ChipWhisperer (%s) Firmware Loader Configuration " % self.fwLoaderConfig.loader.name)

        layout = QVBoxLayout()

        gbFPGAMode = QGroupBox("FPGA Mode Selection")
        radioRelease = QRadioButton("Official Release (.zip)")
        radioDebug = QRadioButton("Debug - no partial reconfig (.bit)")
        layoutGB = QHBoxLayout()
        layoutGB.addWidget(radioRelease)
        layoutGB.addWidget(radioDebug)
        layoutGB.addStretch(1)
        gbFPGAMode.setLayout(layoutGB)
        layout.addWidget(gbFPGAMode)
        radioRelease.clicked.connect(lambda: self.setFPGAModeRelease(True))
        radioDebug.clicked.connect(lambda: self.setFPGAModeRelease(False))
        radioRelease.setChecked(self.fwLoaderConfig.useFPGAZip)
        radioDebug.setChecked(not self.fwLoaderConfig.useFPGAZip)

        layoutFW = QHBoxLayout()
        self.firmwareLocation = QLineEdit()
        firmwareButton = QPushButton("Find")
        firmwareButton.clicked.connect(self.findFirmware)
        layoutFW.addWidget(QLabel("USB Firmware"))
        layoutFW.addWidget(self.firmwareLocation)
        layoutFW.addWidget(firmwareButton)

        if hasattr(self.fwLoaderConfig.loader, 'loadFirmware'):
            layout.addLayout(layoutFW)

        layoutBitZip = QHBoxLayout()
        self.bitZipLocation = QLineEdit()
        bitZipButton = QPushButton("Find")
        bitZipButton.clicked.connect(self.findZipBitstream)
        layoutBitZip.addWidget(QLabel("FPGA .zip (Release)"))
        layoutBitZip.addWidget(self.bitZipLocation)
        layoutBitZip.addWidget(bitZipButton)
        layout.addLayout(layoutBitZip)

        layoutBit = QHBoxLayout()
        self.bitDebugLocation = QLineEdit()
        bitButton = QPushButton("Find")
        bitButton.clicked.connect(self.findDebugBitstream)
        layoutBit.addWidget(QLabel("FPGA .bit file (DEBUG ONLY)"))
        layoutBit.addWidget(self.bitDebugLocation)
        layoutBit.addWidget(bitButton)
        layout.addLayout(layoutBit)

        layoutProgramNow = QHBoxLayout()
        self.programNow = QPushButton("Program FPGA (MUST be connected already)")
        self.programNow.clicked.connect(self.loadFPGA)
        layoutProgramNow.addWidget(self.programNow)
        layout.addLayout(layoutProgramNow)

        gbSAMFW = QGroupBox("SAM3U Firmware")
        layoutSAMFW = QVBoxLayout()
        pbSAMFW = QPushButton("Open SAM3U Update Widget")
        pbSAMFW.clicked.connect(self.upgradeSAM3)
        self.samfwVersion = QLabel("Detected FW Version: unknown")
        self.samfwVersionLatest = QLabel("Latest FW Version: unknown")
        layoutSAMFW.addWidget(self.samfwVersion)
        layoutSAMFW.addWidget(self.samfwVersionLatest)
        layoutSAMFW.addWidget(pbSAMFW)
        gbSAMFW.setLayout(layoutSAMFW)
        self.readFirmwareVersion()

        if isinstance(self.fwLoaderConfig.loader, CWLite_Loader):
            layout.addWidget(gbSAMFW)

        self.setFPGAModeRelease(QSettings().value("%s-fpga-bitstream-mode" % self.fwLoaderConfig.loader.name) == "zip")
        self.bitZipLocation.setText(QSettings().value("%s-debugbitstream-location" % self.fwLoaderConfig.loader.name) if QSettings().value("%s-debugbitstream-location" % self.fwLoaderConfig.loader.name)!="" else self.fwLoaderConfig.loader.bsZipLoc)
        self.bitDebugLocation.setText(QSettings().value("%s-zipbitstream-location" % self.fwLoaderConfig.loader.name) if QSettings().value("%s-zipbitstream-location" % self.fwLoaderConfig.loader.name)!="" else self.fwLoaderConfig.loader.bsLoc)
        self.firmwareLocation.setText(QSettings().value("%s-firmware-location" % self.fwLoaderConfig.loader.name) if QSettings().value("%s-firmware-location" % self.fwLoaderConfig.loader.name)!="" else self.fwLoaderConfig.loader.fwFLoc)
        self.setLayout(layout)

    def readFirmwareVersion(self):
        if self.fwLoaderConfig.loader.driver is not None and hasattr(self.fwLoaderConfig.loader.driver, "readFwVersion"):
            fwver = self.fwLoaderConfig.loader.driver.readFwVersion()
            self.samfwVersion.setText("Detected FW Version: %d.%d b%d" % (fwver[0], fwver[1], fwver[2]))
            self.samfwVersionLatest.setText("Latest FW Version: %d.%d" % (self.fwLoaderConfig.loader.driver.fwversion_latest[0], self.fwLoaderConfig.loader.driver.fwversion_latest[1]))

    def loadFPGA(self):
        self.fwLoaderConfig.loadFPGA(self.bitZipLocation.isEnabled())

    def setFPGAModeRelease(self, mode):
        self.bitZipLocation.setEnabled(mode)
        self.bitDebugLocation.setEnabled(not mode)
        self.fwLoaderConfig.setFPGAMode(mode)
        QSettings().setValue("%s-fpga-bitstream-mode" % self.fwLoaderConfig.loader.name, "zip" if mode==True else "debug")

    def findDebugBitstream(self):
        fname, _ = QFileDialog.getOpenFileName(self, 'Find Bitstream', QSettings().value("%s-debugbitstream-location" % self.fwLoaderConfig.loader.name), '*.bit')
        if fname:
            self.bitDebugLocation.setText(fname)
            self.fwLoaderConfig.loader.bsLoc = fname
            QSettings().setValue("%s-debugbitstream-location" % self.fwLoaderConfig.loader.name, fname)

    def findZipBitstream(self):
        fname, _ = QFileDialog.getOpenFileName(self, 'Find Zip Firmware', QSettings().value("%s-zipbitstream-location" % self.fwLoaderConfig.loader.name), '*.zip')
        if fname:
            self.bitZipLocation.setText(fname)
            self.fwLoaderConfig.loader.bsZipLoc = fname
            QSettings().setValue("%s-zipbitstream-location" % self.fwLoaderConfig.loader.name, fname)

    def findFirmware(self):
        fname, _ = QFileDialog.getOpenFileName(self, 'Find Firmware', QSettings().value("%s-firmware-location" % self.fwLoaderConfig.loader.name), '*.ihx')
        if fname:
            self.firmwareLocation.setText(fname)
            self.fwLoaderConfig.loader.fwFLoc = fname
            QSettings().setValue("%s-firmware-location" % self.fwLoaderConfig.loader.name, fname)

    def upgradeSAM3(self):
        SAM3LoaderConfig(self, self.fwLoaderConfig.loader.driver).show()

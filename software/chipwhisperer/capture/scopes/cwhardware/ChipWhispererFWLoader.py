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


import sys

try:
    from PySide.QtCore import *
    from PySide.QtGui import *
except ImportError:
    print "ERROR: PySide is required for this program"
    sys.exit()

import os.path
import time
import zipfile
from chipwhisperer.capture.scopes.cwhardware.ztex_fwloader import Ztex1v1, IhxFile
from chipwhisperer.capture.scopes.cwhardware.ChipWhispererSAM3Update import SAM3LoaderConfig
from chipwhisperer.common.utils import util


class CWCRev2:
    def __init__(self):
        self.name = "cwcrev2"
        self.ztex = Ztex1v1()
        self.defLocfwF = "../../../hardware/capture/chipwhisperer-rev2/ezusb-firmware/ztex-sdk/examples/usb-fpga-1.11/1.11c/openadc/OpenADC.ihx"
        self.defLocZipBS = "../../../hardware/capture/chipwhisperer-rev2/cwrev2_firmware.zip"
        self.defLocZipBS_filename = "interface.bit"
        self.defLocbs = "../../../hardware/capture/chipwhisperer-rev2/hdl/ztex_rev2_1.11c_ise/interface.bit"

    def loadRequired(self, callback, forceFirmware=False):
        self.ztex.probe(True)

        if self.ztex.firmwareProgrammed == False or forceFirmware:
            self.loadFirmware()
        else:
            print "EZ-USB Microcontroller: Skipped firmware download (already done)"

        if self.ztex.deviceInfo["interfaceVersion"] != 1:
            raise IOError("Unknown interface version, invalid ZTEX Firmware?. Device info: %s" % str(self.ztex.deviceInfo))

        if self.ztex.deviceInfo["productId"] != [10, 12, 0, 0]:
            raise IOError("Unknown productId, invalid ZTEX Firmware/Module?. Device info: %s" % str(self.ztex.deviceInfo))

        self.ztex.getFpgaState()
        if self.ztex.fpgaConfigured == False:
            callback()
            self.ztex.getFpgaState()
            print "FPGA: Programmed bitstream successfully"
        else:
            print "FPGA: Skipped configuration (already done)"

    def loadFirmware(self, file):
        f = IhxFile(file)
        self.ztex.uploadFirmware(f)
        time.sleep(1)
        self.ztex.probe()

    def loadFPGA(self, file):
        self.ztex.configureFpgaLS(file.open("interface.bit"))

    def setUSBInterface(self, dev):
        pass

class CWLite:
    def __init__(self):
        self.name = "cwlite"
        self.cwliteUSB = None
        self.sam3loader = None
        self.defLocfwF = ""
        self.defLocZipBS = "../../../hardware/capture/chipwhisperer-lite/cwlite_firmware.zip"
        self.defLocZipBS_filename = "cwlite_interface.bit"
        self.defLocbs = "../../../hardware/capture/chipwhisperer-lite/hdl/cwlite_ise/cwlite_interface.bit"

    def loadRequired(self, callback, forceFirmware=False):
        callback()

    def loadFirmware(self):
        pass

    def loadFPGA(self, file):
        self.cwliteUSB.FPGAProgram(file)

    def setUSBInterface(self, cwliteusb):
        self.cwliteUSB = cwliteusb

        if self.sam3loader:
            self.sam3loader.cwLiteUSB = cwliteusb

class FWLoaderConfig(object):
    def __init__(self, driver):
        self.driver = driver
        settings = util.Settings()

        self.bsLoc = settings.value("%s-debugbitstream-location" % self.driver.name)
        self.bsZipLoc = settings.value("%s-zipbitstream-location" % self.driver.name)
        self.fwFLoc = settings.value("%s-firmware-location" % self.driver.name)

        mode = settings.value("%s-fpga-bitstream-mode" % self.driver.name)
        if mode == "debug":
            self.setFPGAModeDebug()
        else:
            self.setFPGAModeZip()

        rootprefix = util.globalSettings.value("cwcapture-starting-root") + "/"

        defLocfwF = rootprefix + self.driver.defLocfwF
        if os.path.isfile(defLocfwF):
            self.fwFLoc = str(defLocfwF)

        defLocZipBS = rootprefix + self.driver.defLocZipBS
        if os.path.isfile(defLocZipBS):
            self.bsZipLoc = str(defLocZipBS)
            settings.setValue("%s-zipbitstream-location" % self.driver.name, self.bsZipLoc)

        defLocbs = rootprefix + self.driver.defLocbs
        if os.path.isfile(defLocbs):
            self.bsLoc = str(defLocbs)

    def loadRequired(self, forceFirmware=False):
        """Load firmware file or FPGA file only as required, skip otherwise"""
        self.driver.loadRequired(self.loadFPGA, forceFirmware)

    def loadFirmware(self):
        self.driver.loadFirmware()

    def loadFPGA(self):
        """Load the FPGA bitstream"""
        if self.useFPGAZip:
            fileloc = self.bsZipLoc
            if fileloc.startswith("."):
                fileloc = util.globalSettings.value("cwcapture-starting-root") + "/" + fileloc
            zfile = zipfile.ZipFile(fileloc, "r")

            self.driver.loadFPGA(zfile.open(self.driver.defLocZipBS_filename))
        else:
            fileloc = self.bsLoc
            if fileloc.startswith("."):
                fileloc = util.globalSettings.value("cwcapture-starting-root") + "/" + fileloc

            # Get bitstream date, print for user
            bsdate = os.path.getmtime(fileloc)
            print "FPGA: DEBUG MODE: Using .bit file, date: %s" % time.ctime(bsdate)

            self.driver.loadFPGA(open(self.bsLoc, "rb"))

    def setFPGAModeZip(self):
        self.useFPGAZip = True
        util.globalSettings.setValue("%s-fpga-bitstream-mode" % self.driver.name, "zip")

    def setFPGAModeDebug(self):
        self.useFPGAZip = False
        util.globalSettings.setValue("%s-fpga-bitstream-mode" % self.driver.name, "debug")

    def setUSBInterface(self, dev):
        self.driver.setUSBInterface(dev)


class FWLoaderConfigGUI(QDialog):
    def __init__(self, fwlc):
        super(FWLoaderConfigGUI, self).__init__(None)
        self.fwlc = fwlc
        self.setWindowTitle("ChipWhisperer (%s) Firmware Loader Configuration " % self.fwlc.driver.name)

        layout = QVBoxLayout()

        gbFPGAMode = QGroupBox("FPGA Mode Selection")
        radioOfficial = QRadioButton("Official Release (.zip)")
        radioDebug = QRadioButton("Debug - no Partial Reconfig (.bit)")
        layoutGB = QHBoxLayout()
        layoutGB.addWidget(radioOfficial)
        layoutGB.addWidget(radioDebug)
        layoutGB.addStretch(1)
        gbFPGAMode.setLayout(layoutGB)
        layout.addWidget(gbFPGAMode)
        radioOfficial.clicked.connect(self.setFPGAModeZip)
        radioDebug.clicked.connect(self.setFPGAModeDebug)

        layoutFW = QHBoxLayout()
        self.firmwareLocation = QLineEdit()
        firmwareButton = QPushButton("Find")
        firmwareButton.clicked.connect(self.findFirmware)
        layoutFW.addWidget(QLabel("USB Firmware"))
        layoutFW.addWidget(self.firmwareLocation)
        layoutFW.addWidget(firmwareButton)
        if self.fwlc.driver.name == "cwcrev2":
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
        self.bitLocation = QLineEdit()
        bitButton = QPushButton("Find")
        bitButton.clicked.connect(self.findDebugBitstream)
        layoutBit.addWidget(QLabel("FPGA .bit file (DEBUG ONLY)"))
        layoutBit.addWidget(self.bitLocation)
        layoutBit.addWidget(bitButton)
        layout.addLayout(layoutBit)

        layoutProgramNow = QHBoxLayout()
        self.programNow = QPushButton("Program FPGA (MUST be connected already)")
        self.programNow.clicked.connect(self.fwlc.loadFPGA)
        layoutProgramNow.addWidget(self.programNow)
        layout.addLayout(layoutProgramNow)

        if self.fwlc.driver.name == "cwlite":
            gbSAMFW = QGroupBox("SAM3U Firmware")
            layoutSAMFW = QVBoxLayout()
            self.samfwVersion = QLabel("Detected FW Version: unknown")
            self.samfwVersionLatest = QLabel("Latest FW Version: unknown")
            pbSAMFW = QPushButton("Open SAM3U Update Widget")
            pbSAMFW.clicked.connect(self.upgradeSAM3)

            layoutSAMFW.addWidget(self.samfwVersion)
            layoutSAMFW.addWidget(self.samfwVersionLatest)
            layoutSAMFW.addWidget(pbSAMFW)
            gbSAMFW.setLayout(layoutSAMFW)
            layout.addWidget(gbSAMFW)

        if not self.fwlc.useFPGAZip:
            radioDebug.setChecked(True)
        else:
            radioOfficial.setChecked(True)

        self.setLayout(layout)

        self.bitLocation.setText(self.fwlc.bsLoc)
        self.bitZipLocation.setText(self.fwlc.bsZipLoc)
        self.firmwareLocation.setText(self.fwlc.fwFLoc)

    def setUSBInterface(self, cwliteusb):
        self.fwlc.setUSBInterface(cwliteusb)
        # Get versions
        fwver = cwliteusb.readFwVersion()
        self.samfwVersion.setText("Detected FW Version: %d.%d b%d" % (fwver[0], fwver[1], fwver[2]))
        self.samfwVersionLatest.setText("Latest FW Version: %d.%d" % (cwliteusb.fwversion_latest[0], cwliteusb.fwversion_latest[1]))

    def setFPGAModeZip(self):
        self.fwlc.setFPGAModeZip()
        self.bitZipLocation.setEnabled(True)
        self.bitLocation.setEnabled(False)

    def setFPGAModeDebug(self):
        self.fwlc.setFPGAModeDebug()
        self.bitZipLocation.setEnabled(False)
        self.bitLocation.setEnabled(True)

    def findDebugBitstream(self):
        fname, _ = QFileDialog.getOpenFileName(self, 'Find Bitstream', '.', '*.bit')
        if fname:
            self.bitLocation.setText(fname)
            util.globalSettings.setValue("%s-debugbitstream-location" % self.name, fname)

    def findZipBitstream(self):
        fname, _ = QFileDialog.getOpenFileName(self, 'Find Zip Firmware', '.', '*.zip')
        if fname:
            self.bitZipLocation.setText(fname)
            util.globalSettings.setValue("%s-zipbitstream-location" % self.name, fname)

    def findFirmware(self):
        fname, _ = QFileDialog.getOpenFileName(self, 'Find Firmware', '.', '*.ihx')
        if fname:
            self.firmwareLocation.setText(fname)
            util.globalSettings.setValue("%s-firmware-location" % self.name, fname)

    def upgradeSAM3(self):
        if self.sam3loader is None:
            self.sam3loader = SAM3LoaderConfig(self, self.console, self.cwliteUSB)
        self.sam3loader.show()
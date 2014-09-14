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

from chipwhisperer.capture.scopes.ztex_fwloader import Ztex1v1, IhxFile

class FWLoaderConfig(QDialog):
    def __init__(self, parent=None, console=None):
        super(FWLoaderConfig, self).__init__(parent)

        self.console = console

        self.setWindowTitle("ChipWhisperer Firmware Loader Configuration")
        settings = QSettings()

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

        mode = settings.value("fpga-bitstream-mode")
        if mode is not None and mode == "debug":
            radioDebug.setChecked(True)
            self.setFPGAModeDebug()
        else:
            radioOfficial.setChecked(True)
            self.setFPGAModeZip()

        # Set dialog layout
        self.setLayout(layout)

        self.ztex = Ztex1v1()

        bsLoc = settings.value("debugbitstream-location")
        bsZipLoc = settings.value("zipbitstream-location")
        fwFLoc = settings.value("firmware-location")

        # Defaults?
        # print os.getcwd()

        if not fwFLoc:
            defLocfwF = "../../../hardware/capture/chipwhisperer-rev2/ezusb-firmware/ztex-sdk/examples/usb-fpga-1.11/1.11c/openadc/OpenADC.ihx"
            # defLocfwF = os.path.realpath(defLocfwF)
            if os.path.isfile(defLocfwF):
                fwFLoc = str(defLocfwF)

        if not bsZipLoc:
            defLocZipBS = "../../../hardware/capture/chipwhisperer-rev2/cwrev2_firmware.zip"
            if os.path.isfile(defLocZipBS):
                bsZipLoc = str(defLocZipBS)
                settings.setValue("zipbitstream-location", bsZipLoc)

        if not bsLoc:
            defLocbs = "../../../hardware/capture/chipwhisperer-rev2/hdl/ztex_rev2_1.11c_ise/interface.bit"
            # defLocbs = os.path.realpath(defLocbs)
            if os.path.isfile(defLocbs):
                bsLoc = str(defLocbs)

        self.bitLocation.setText(bsLoc)
        self.bitZipLocation.setText(bsZipLoc)
        self.firmwareLocation.setText(fwFLoc)

    def setFPGAModeZip(self):
        self.bitZipLocation.setEnabled(True)
        self.bitLocation.setEnabled(False)
        self.useFPGAZip = True
        QSettings().setValue("fpga-bitstream-mode", "zip")

    def setFPGAModeDebug(self):
        self.bitZipLocation.setEnabled(False)
        self.bitLocation.setEnabled(True)
        self.useFPGAZip = False
        QSettings().setValue("fpga-bitstream-mode", "debug")

    def findDebugBitstream(self):
        fname, _ = QFileDialog.getOpenFileName(self, 'Find Bitstream', '.', '*.bit')
        if fname:
            self.bitLocation.setText(fname)
            QSettings().setValue("debugbitstream-location", fname)

    def findZipBitstream(self):
        fname, _ = QFileDialog.getOpenFileName(self, 'Find Zip Firmware', '.', '*.zip')
        if fname:
            self.bitZipLocation.setText(fname)
            QSettings().setValue("zipbitstream-location", fname)

    def findFirmware(self):
        fname, _ = QFileDialog.getOpenFileName(self, 'Find Firmware', '.', '*.ihx')
        if fname:
            self.firmwareLocation.setText(fname)
            QSettings().setValue("firmware-location", fname)

    def loadRequired(self, forceFirmware=False):
        """Load firmware file or FPGA file only as required, skip otherwise"""
        self.ztex.probe(True)

        if self.ztex.firmwareProgrammed == False or forceFirmware:
            # Load firmware
            self.loadFirmware()
        else:
            if self.console:
                self.console.append("Skipped firmware download")

        if self.ztex.deviceInfo["interfaceVersion"] != 1:
            raise IOError("Unknown interface version, invalid ZTEX Firmware?. Device info: %s" % str(self.ztex.deviceInfo))

        if self.ztex.deviceInfo["productId"] != [10, 12, 0, 0]:
            raise IOError("Unknown productId, invalid ZTEX Firmware/Module?. Device info: %s" % str(self.ztex.deviceInfo))

        self.ztex.getFpgaState()
        if self.ztex.fpgaConfigured == False:
            self.loadFPGA()
            self.ztex.getFpgaState()
            print "Programmed FPGA bistream"
        else:
            print "FPGA already configured, skipped configuration"

    def loadFirmware(self):
        """Load the USB microcontroller firmware file setup in the dialog"""
        f = IhxFile(self.firmwareLocation.text())
        self.ztex.uploadFirmware(f)
        time.sleep(1)
        self.ztex.probe()


    def loadFPGA(self):
        """Load the FPGA bitstream"""

        if self.useFPGAZip:
            zfile = zipfile.ZipFile(self.bitZipLocation.text(), "r")
            self.ztex.configureFpgaLS(zfile.open("interface.bit"))
        else:
            # Get bitstream date, print for user
            bsdate = os.path.getmtime(self.bitLocation.text())
            print "Using FPGA Bitstream, date: %s" % time.ctime(bsdate)
            self.ztex.configureFpgaLS(open(self.bitLocation.text(), "rb"))


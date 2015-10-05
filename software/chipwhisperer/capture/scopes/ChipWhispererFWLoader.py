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

# Connection for ZTEX Board
from chipwhisperer.capture.scopes.ztex_fwloader import Ztex1v1, IhxFile
from chipwhisperer.capture.scopes.ChipWhispererSAM3Update import SAM3LoaderConfig

class FWLoaderConfig(QDialog):
    def __init__(self, parent=None, console=None, mode="cwcrev2"):
        super(FWLoaderConfig, self).__init__(parent)

        if (mode != "cwcrev2") and (mode != "cwlite"):
            raise ValueError("Invalid mode: %s" % mode)

        self._mode = mode

        self.console = console

        self.cwliteUSB = None
        self.sam3loader = None

        self.setWindowTitle("ChipWhisperer (%s) Firmware Loader Configuration" % mode)
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
        if self._mode == "cwcrev2":
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
        self.programNow.clicked.connect(self.loadFPGA)
        layoutProgramNow.addWidget(self.programNow)
        layout.addLayout(layoutProgramNow)

        if self._mode == "cwlite":
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

        mode = settings.value("%s-fpga-bitstream-mode" % self._mode)
        if mode is not None and mode == "debug":
            radioDebug.setChecked(True)
            self.setFPGAModeDebug()
        else:
            radioOfficial.setChecked(True)
            self.setFPGAModeZip()

        # Set dialog layout
        self.setLayout(layout)

        if self._mode == "cwcrev2":
            self.ztex = Ztex1v1()

        bsLoc = settings.value("%s-debugbitstream-location" % self._mode)
        bsZipLoc = settings.value("%s-zipbitstream-location" % self._mode)
        fwFLoc = settings.value("%s-firmware-location" % self._mode)

        # Defaults?
        # print os.getcwd()

        rootprefix = QSettings().value("cwcapture-starting-root") + "/"

        if not fwFLoc:
            if self._mode == "cwcrev2":
                defLocfwF = rootprefix + "../../../hardware/capture/chipwhisperer-rev2/ezusb-firmware/ztex-sdk/examples/usb-fpga-1.11/1.11c/openadc/OpenADC.ihx"
            elif self._mode == "cwlite":
                defLocfwF = ""
            # defLocfwF = os.path.realpath(defLocfwF)
            if os.path.isfile(defLocfwF):
                fwFLoc = str(defLocfwF)

        if not bsZipLoc:
            if self._mode == "cwcrev2":
                defLocZipBS = rootprefix + "../../../hardware/capture/chipwhisperer-rev2/cwrev2_firmware.zip"
            elif self._mode == "cwlite":
                defLocZipBS = rootprefix + "../../../hardware/capture/chipwhisperer-lite/cwlite_firmware.zip"
            if os.path.isfile(defLocZipBS):
                bsZipLoc = str(defLocZipBS)
                settings.setValue("%s-zipbitstream-location" % self._mode, bsZipLoc)

        if not bsLoc:
            if self._mode == "cwcrev2":
                defLocbs = rootprefix + "../../../hardware/capture/chipwhisperer-rev2/hdl/ztex_rev2_1.11c_ise/interface.bit"
            elif self._mode == "cwlite":
                defLocbs = rootprefix + "../../../hardware/capture/chipwhisperer-lite/hdl/cwlite_ise/cwlite_interface.bit"
            # defLocbs = os.path.realpath(defLocbs)
            if os.path.isfile(defLocbs):
                bsLoc = str(defLocbs)

        self.bitLocation.setText(bsLoc)
        self.bitZipLocation.setText(bsZipLoc)
        self.firmwareLocation.setText(fwFLoc)

    def setCWLiteUSBInterface(self, cwliteusb):
        self.cwliteUSB = cwliteusb

        # Get versions
        fwver = cwliteusb.readFwVersion()
        self.samfwVersion.setText("Detected FW Version: %d.%d b%d" % (fwver[0], fwver[1], fwver[2]))
        self.samfwVersionLatest.setText("Latest FW Version: %d.%d" % (cwliteusb.fwversion_latest[0], cwliteusb.fwversion_latest[1]))

    def setFPGAModeZip(self):
        self.bitZipLocation.setEnabled(True)
        self.bitLocation.setEnabled(False)
        self.useFPGAZip = True
        QSettings().setValue("%s-fpga-bitstream-mode" % self._mode, "zip")

    def setFPGAModeDebug(self):
        self.bitZipLocation.setEnabled(False)
        self.bitLocation.setEnabled(True)
        self.useFPGAZip = False
        QSettings().setValue("%s-fpga-bitstream-mode" % self._mode, "debug")

    def findDebugBitstream(self):
        fname, _ = QFileDialog.getOpenFileName(self, 'Find Bitstream', '.', '*.bit')
        if fname:
            self.bitLocation.setText(fname)
            QSettings().setValue("%s-debugbitstream-location" % self._mode, fname)

    def findZipBitstream(self):
        fname, _ = QFileDialog.getOpenFileName(self, 'Find Zip Firmware', '.', '*.zip')
        if fname:
            self.bitZipLocation.setText(fname)
            QSettings().setValue("%s-zipbitstream-location" % self._mode, fname)

    def findFirmware(self):
        fname, _ = QFileDialog.getOpenFileName(self, 'Find Firmware', '.', '*.ihx')
        if fname:
            self.firmwareLocation.setText(fname)
            QSettings().setValue("%s-firmware-location" % self._mode, fname)

    def loadRequired(self, forceFirmware=False):
        """Load firmware file or FPGA file only as required, skip otherwise"""
        
        if self._mode == "cwcrev2":
            self.ztex.probe(True)

            if self.ztex.firmwareProgrammed == False or forceFirmware:
                # Load firmware
                self.loadFirmware()
            else:
                print "EZ-USB Microcontroller: Skipped firmware download (already done)"

            if self.ztex.deviceInfo["interfaceVersion"] != 1:
                raise IOError("Unknown interface version, invalid ZTEX Firmware?. Device info: %s" % str(self.ztex.deviceInfo))

            if self.ztex.deviceInfo["productId"] != [10, 12, 0, 0]:
                raise IOError("Unknown productId, invalid ZTEX Firmware/Module?. Device info: %s" % str(self.ztex.deviceInfo))

            self.ztex.getFpgaState()
            if self.ztex.fpgaConfigured == False:
                self.loadFPGA()
                self.ztex.getFpgaState()
                print "FPGA: Programmed bitstream successfully"
            else:
                print "FPGA: Skipped configuration (already done)"

        elif self._mode == "cwlite":
            self.loadFPGA()
            print "Programmed FPGA"

        else:
            raise ValueError("Internal Error: Invalid setting of mode: %s" % self._mode)


    def loadFirmware(self):
        """Load the USB microcontroller firmware file setup in the dialog"""

        if self._mode == "cwcrev2":

            fileloc = self.firmwareLocation.text()
            if fileloc.startswith("."):
                fileloc = QSettings().value("cwcapture-starting-root") + "/" + fileloc

            f = IhxFile(fileloc)
            self.ztex.uploadFirmware(f)
            time.sleep(1)
            self.ztex.probe()
        elif self._mode == "cwlite":
            pass
        else:
            raise ValueError("Internal Error: Invalid setting of mode: %s" % self._mode)


    def loadFPGA(self):
        """Load the FPGA bitstream"""

        if self.useFPGAZip:
            fileloc = self.bitZipLocation.text()
            if fileloc.startswith("."):
                fileloc = QSettings().value("cwcapture-starting-root") + "/" + fileloc
            zfile = zipfile.ZipFile(fileloc, "r")

            if self._mode == "cwcrev2":
                self.ztex.configureFpgaLS(zfile.open("interface.bit"))
            elif self._mode == "cwlite":
                self.cwliteUSB.FPGAProgram(zfile.open("cwlite_interface.bit"))
            else:
                raise ValueError("Internal Error: Invalid setting of mode: %s" % self._mode)
        else:

            fileloc = self.bitLocation.text()
            if fileloc.startswith("."):
                fileloc = QSettings().value("cwcapture-starting-root") + "/" + fileloc

            # Get bitstream date, print for user
            bsdate = os.path.getmtime(fileloc)
            print "FPGA: DEBUG MODE: Using .bit file, date: %s" % time.ctime(bsdate)

            if self._mode == "cwcrev2":
                self.ztex.configureFpgaLS(open(self.bitLocation.text(), "rb"))
            elif self._mode == "cwlite":
                self.cwliteUSB.FPGAProgram(open(self.bitLocation.text(), "rb"))
            else:
                raise ValueError("Internal Error: Invalid setting of mode: %s" % self._mode)

    def upgradeSAM3(self):
        if self.sam3loader is None:
            self.sam3loader = SAM3LoaderConfig(self, self.console, self.cwliteUSB)
        self.sam3loader.show()


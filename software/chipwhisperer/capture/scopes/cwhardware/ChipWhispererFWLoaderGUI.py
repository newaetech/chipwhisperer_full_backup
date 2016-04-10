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

from chipwhisperer.capture.scopes.cwhardware.ChipWhispererSAM3Update import SAM3LoaderConfig
import chipwhisperer.capture.scopes.cwhardware.ChipWhispererFWLoader as ChipWhispererFWLoader
from PySide.QtGui import *
from PySide.QtCore import *
import chipwhisperer.common.utils.qt_tweaks as QtFixes


class FWLoaderConfigGUI(QtFixes.QDialog):
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
        self.firmwareLocation = QtFixes.QLineEdit()
        firmwareButton = QPushButton("Find")
        firmwareButton.clicked.connect(self.findFirmware)
        layoutFW.addWidget(QLabel("USB Firmware"))
        layoutFW.addWidget(self.firmwareLocation)
        layoutFW.addWidget(firmwareButton)

        if hasattr(self.fwLoaderConfig.loader, 'loadFirmware'):
            layout.addLayout(layoutFW)

        layoutBitZip = QHBoxLayout()
        self.bitZipLocation = QtFixes.QLineEdit()
        bitZipButton = QPushButton("Find")
        bitZipButton.clicked.connect(self.findZipBitstream)
        layoutBitZip.addWidget(QLabel("FPGA .zip (Release)"))
        layoutBitZip.addWidget(self.bitZipLocation)
        layoutBitZip.addWidget(bitZipButton)
        layout.addLayout(layoutBitZip)

        layoutBit = QHBoxLayout()
        self.bitDebugLocation = QtFixes.QLineEdit()
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

        if isinstance(self.fwLoaderConfig.loader, ChipWhispererFWLoader.CWLite_Loader):
            layout.addWidget(gbSAMFW)

        self.setFPGAModeRelease(QSettings().value("%s-fpga-bitstream-mode" % self.fwLoaderConfig.loader.name) != "debug")
        self.fwLoaderConfig.loader._bsZipLoc = QSettings().value("%s-zipbitstream-location" % self.fwLoaderConfig.loader.name, self.fwLoaderConfig.loader._bsZipLoc)
        self.bitZipLocation.setText(self.fwLoaderConfig.loader._bsZipLoc)
        self.fwLoaderConfig.loader._bsLoc = QSettings().value("%s-debugbitstream-location" % self.fwLoaderConfig.loader.name, self.fwLoaderConfig.loader._bsLoc)
        self.bitDebugLocation.setText(self.fwLoaderConfig.loader._bsLoc)
        self.fwLoaderConfig.loader._fwFLoc = QSettings().value("%s-firmware-location" % self.fwLoaderConfig.loader.name, self.fwLoaderConfig.loader._fwFLoc)
        self.firmwareLocation.setText(self.fwLoaderConfig.loader._fwFLoc)
        self.setLayout(layout)

    def readFirmwareVersion(self):
        if self.fwLoaderConfig.loader.driver is not None and hasattr(self.fwLoaderConfig.loader.driver, "readFwVersion"):
            fwver = self.fwLoaderConfig.loader.driver.readFwVersion()
            self.samfwVersion.setText("Detected FW Version: %d.%d b%d" % (fwver[0], fwver[1], fwver[2]))
            self.samfwVersionLatest.setText("Latest FW Version: %d.%d" % (self.fwLoaderConfig.loader.driver.fwversion_latest[0], self.fwLoaderConfig.loader.driver.fwversion_latest[1]))

    def loadFPGA(self):
        self.fwLoaderConfig.loadFPGA()

    def setFPGAModeRelease(self, mode):
        self.bitZipLocation.setEnabled(mode)
        self.bitDebugLocation.setEnabled(not mode)
        self.fwLoaderConfig.setFPGAMode(mode)
        QSettings().setValue("%s-fpga-bitstream-mode" % self.fwLoaderConfig.loader.name, "zip" if mode==True else "debug")

    def findDebugBitstream(self):
        fname, _ = QFileDialog.getOpenFileName(self, 'Find Bitstream', QSettings().value("%s-debugbitstream-location" % self.fwLoaderConfig.loader.name), '*.bit')
        if fname:
            self.bitDebugLocation.setText(fname)
            self.fwLoaderConfig.loader._bsLoc = fname
            QSettings().setValue("%s-debugbitstream-location" % self.fwLoaderConfig.loader.name, fname)

    def findZipBitstream(self):
        fname, _ = QFileDialog.getOpenFileName(self, 'Find Zip Firmware', QSettings().value("%s-zipbitstream-location" % self.fwLoaderConfig.loader.name), '*.zip')
        if fname:
            self.bitZipLocation.setText(fname)
            self.fwLoaderConfig.loader._bsZipLoc = fname
            QSettings().setValue("%s-zipbitstream-location" % self.fwLoaderConfig.loader.name, fname)

    def findFirmware(self):
        fname, _ = QFileDialog.getOpenFileName(self, 'Find Firmware', QSettings().value("%s-firmware-location" % self.fwLoaderConfig.loader.name), '*.ihx')
        if fname:
            self.firmwareLocation.setText(fname)
            self.fwLoaderConfig.loader._fwFLoc = fname
            QSettings().setValue("%s-firmware-location" % self.fwLoaderConfig.loader.name, fname)

    def upgradeSAM3(self):
        SAM3LoaderConfig(self, self.fwLoaderConfig.loader.driver).show()

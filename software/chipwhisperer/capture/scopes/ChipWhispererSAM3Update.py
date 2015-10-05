#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2015, NewAE Technology Inc
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
from sys import platform as _platform
from subprocess import Popen, PIPE
import platform
import shlex


class SAM3LoaderConfig(QDialog):
    def __init__(self, parent=None, console=None, cwliteUSB=None):
        super(SAM3LoaderConfig, self).__init__(parent)

        self.console = console
        self.cwLiteUSB = cwliteUSB

        self.setWindowTitle("SAM3U Firmware Loader")
        settings = QSettings()

        layout = QVBoxLayout()

        gbSAMMode = QGroupBox("Step 1. Enable Bootloader")
        layoutGB = QVBoxLayout()
        pbBootmode = QPushButton("Enable Bootloader Mode")
        pbBootmode.clicked.connect(self.enableBootloader)
        labelWarnBoot1 = QLabel("WARNING: Bootload mode will stay enabled until new firmware is loaded.")
        labelWarnBoot2 = QLabel("NOTE: This only works on firmware 0.11 or later. Short 'ERASE' jumper to enable bootloader mode on earlier firmwares.")
        labelWarnBoot1.setWordWrap(True)
        labelWarnBoot2.setWordWrap(True)
        layoutGB.addWidget(pbBootmode)
        layoutGB.addWidget(labelWarnBoot1)
        layoutGB.addWidget(labelWarnBoot2)
        gbSAMMode.setLayout(layoutGB)
        layout.addWidget(gbSAMMode)

        gbSAMProgram = QGroupBox("Step 2. Program Binary")

        BOSSADesc = QLabel("The following will run the command 'bossac -e -w -v -b SAM3UFW.bin'." +
                           " The ChipWhisperer system comes with a 'bossac' binary for Windows " +
                           " and Linux 32-bit. You can download other versions from the BOSSA " +
                           "website.")
        BOSSADesc.setWordWrap(True)

        layoutSAMGB = QVBoxLayout()

        layoutBOSSA = QHBoxLayout()
        self.BOSSALocation = QLineEdit()
        BOSSAButton = QPushButton("Find")
        BOSSAButton.clicked.connect(self.findBOSSA)
        layoutBOSSA.addWidget(QLabel("bossac binary"))
        layoutBOSSA.addWidget(self.BOSSALocation)
        layoutBOSSA.addWidget(BOSSAButton)

        layoutFW = QHBoxLayout()
        self.firmwareLocation = QLineEdit()
        firmwareButton = QPushButton("Find")
        firmwareButton.clicked.connect(self.findFirmware)
        layoutFW.addWidget(QLabel("SAM3U Firmware"))
        layoutFW.addWidget(self.firmwareLocation)
        layoutFW.addWidget(firmwareButton)

        pbProgram = QPushButton("Run bossac")
        pbProgram.clicked.connect(self.runBossa)

        self.bossaOut = QTextEdit("BOSSA Output here\n\n")
        self.bossaOut.setReadOnly(True)

        layoutSAMGB.addWidget(BOSSADesc)
        layoutSAMGB.addLayout(layoutBOSSA)
        layoutSAMGB.addLayout(layoutFW)
        layoutSAMGB.addWidget(pbProgram)
        layoutSAMGB.addWidget(self.bossaOut)
        gbSAMProgram.setLayout(layoutSAMGB)
        layout.addWidget(gbSAMProgram)

        bossaLoc = settings.value("bossa-location")
        sam3uFWLoc = settings.value("cwlite-sam3u-firmware-location")

        rootprefix = QSettings().value("cwcapture-starting-root") + "/"

        if not sam3uFWLoc:
            defLocfwF = rootprefix + "../../../hardware/capture/chipwhisperer-lite/sam3u_fw/SAM3U_VendorExample/Debug/SAM3U_CW1173.bin"
            if os.path.isfile(defLocfwF):
                sam3uFWLoc = str(defLocfwF)
                settings.setValue("cwlite-sam3u-firmware-location", sam3uFWLoc)

        if not bossaLoc:
            if _platform == "linux" or _platform == "linux2":
                suffix = "lin32"
            else:
                suffix = "win32.exe"
            defbossaLoc = rootprefix + "../../../hardware/capture/chipwhisperer-lite/sam3u_fw/bossa/bossac-%s" % suffix
            if os.path.isfile(defbossaLoc):
                bossaLoc = str(defbossaLoc)
                settings.setValue("bossa-location", bossaLoc)

        self.BOSSALocation.setText(bossaLoc)
        self.firmwareLocation.setText(sam3uFWLoc)


        gbDone = QGroupBox("Step 3. Reset")
        gbDoneLayout = QHBoxLayout()
        gbDone.setLayout(gbDoneLayout)
        gbDoneLabel = QLabel("If the above succeeds, you need to power-cycle the target. Unplug & replug target. You will also " +
                             "need to close and re-open ChipWhisperer-Capture.")
        gbDoneLabel.setWordWrap(True)
        gbDoneLayout.addWidget(gbDoneLabel)
        layout.addWidget(gbDone)

        self.setLayout(layout)

    def findBOSSA(self):
        fname, _ = QFileDialog.getOpenFileName(self, 'Find bossac Executable', '.', '*')
        if fname:
            self.BOSSALocation.setText(fname)
            QSettings().setValue("bossa-location", fname)


    def findFirmware(self):
        fname, _ = QFileDialog.getOpenFileName(self, 'Find Firmware', '.', '*.bin')
        if fname:
            self.firmwareLocation.setText(fname)
            QSettings().setValue("cwlite-sam3u-firmware-location", fname)

    def runBossa(self):
        # Windows seems to require this?
        if platform.system() == 'Windows':
            self.shellRequired = True
        else:
            self.shellRequired = False

        cmd = [os.path.normpath(self.BOSSALocation.text()),
              '-e', '-w', '-v', '-b',
              os.path.normpath(self.firmwareLocation.text())]

        process = Popen(cmd, stdout=PIPE, stderr=PIPE, shell=self.shellRequired)
        stdout, stderr = process.communicate()
        process.wait()

        self.bossaOut.append(stdout + stderr)

    def enableBootloader(self, ask=True):
        if ask:
            # Check user isn't dumb
            reply = QMessageBox.warning(self, "Bootloader Warning", "Clicking 'YES' will enable bootloader mode. To disable this mode you " +
                                              "must load new firmware, it will not be cleared by power cycling. Please check bossac works " +
                                              "on your platform first. If you have trouble see documentation, or ask at newae.com/forum.", QMessageBox.Yes | QMessageBox.No,
                                              QMessageBox.No)

            if reply != QMessageBox.Yes:
                return

        self.cwLiteUSB.sendCtrl(0x22, 3)


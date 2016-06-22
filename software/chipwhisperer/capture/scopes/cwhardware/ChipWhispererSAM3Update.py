#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2015-2016, NewAE Technology Inc
# All rights reserved.
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
from PySide.QtCore import *
from PySide.QtGui import *
try:
    import chipwhisperer.common.utils.serialport as scan
except ImportError:
    class scan:
        @staticmethod
        def scan():
            return ["pyserial not installed"]
import chipwhisperer.common.utils.qt_tweaks as QtFixes
from chipwhisperer.common.utils import util
from chipwhisperer.hardware.naeusb.bootloader_sam3u import Samba


class SAM3LoaderConfig(QtFixes.QDialog):
    def __init__(self, parent=None, cwliteUSB=None):
        super(SAM3LoaderConfig, self).__init__(parent)

        if cwliteUSB:
            self.cwLiteUSB = cwliteUSB._usb
        else:
            self.cwLiteUSB = None

        self.setWindowTitle("SAM3U Firmware Loader")
        layout = QVBoxLayout()

        gbSAMMode = QGroupBox("Step 1. Enable Bootloader")
        layoutGB = QVBoxLayout()

        if self.cwLiteUSB:
            pbBootmode = QPushButton("Enable Bootloader Mode")
            pbBootmode.clicked.connect(self.enableBootloader)
            layoutGB.addWidget(pbBootmode)
        else:
            labelUnconnected = QLabel("USB not connected. If you have not yet enabled bootload mode: close this dialog, " +
                                      "connect to the scope/target, and open it again. If you have already enabled bootload " +
                                      "mode skip to Step 2.")
            labelUnconnected.setWordWrap(True)
            layoutGB.addWidget(labelUnconnected)

        labelWarnBoot1 = QLabel("WARNING: Bootload mode will stay enabled until new firmware is loaded.")
        labelWarnBoot2 = QLabel("NOTE: This only works on firmware 0.11 or later. Short 'ERASE' jumper to enable bootloader mode on earlier firmwares.")
        labelWarnBoot1.setWordWrap(True)
        labelWarnBoot2.setWordWrap(True)
        layoutGB.addWidget(labelWarnBoot1)
        layoutGB.addWidget(labelWarnBoot2)
        gbSAMMode.setLayout(layoutGB)
        layout.addWidget(gbSAMMode)

        gbSAMProgram = QGroupBox("Step 2. Program Binary")

        layoutSAMGB = QVBoxLayout()

        layoutFW = QHBoxLayout()
        self.firmwareLocation = QtFixes.QLineEdit()
        firmwareButton = QPushButton("Find")
        firmwareButton.clicked.connect(self.findFirmware)
        layoutFW.addWidget(QLabel("SAM3U Firmware"))
        layoutFW.addWidget(self.firmwareLocation)
        layoutFW.addWidget(firmwareButton)

        self.serlist = QComboBox()
        serrefresh = QPushButton("Update List")
        serrefresh.clicked.connect(self.serialRefresh)

        layoutSerList = QHBoxLayout()
        layoutSerList.addWidget(self.serlist)
        layoutSerList.addWidget(serrefresh)

        pbProgram = QPushButton("Run Program")
        pbProgram.clicked.connect(self.runSamba)

        self.programStatus = QTextEdit("You will have to wait for device to attach as a serial port. Once this " +
                                       "happens, press the 'Update List' button and select the proper serial port." +
                                       " Then hit 'Run Program' to update your device.")
        self.programStatus.setReadOnly(True)

        layoutSAMGB.addLayout(layoutFW)
        layoutSAMGB.addLayout(layoutSerList)
        layoutSAMGB.addWidget(pbProgram)
        layoutSAMGB.addWidget(self.programStatus)
        #layoutSAMGB.addWidget(self.bossaOut)
        gbSAMProgram.setLayout(layoutSAMGB)
        layout.addWidget(gbSAMProgram)

        sam3uFWLoc = QSettings().value("cwlite-sam3u-firmware-location")

        rootprefix = util.getRootDir() + "/"

        if not sam3uFWLoc:
            defLocfwF = rootprefix + "../../../hardware/capture/chipwhisperer-lite/sam3u_fw/SAM3U_VendorExample/Debug/SAM3U_CW1173.bin"
            if os.path.isfile(defLocfwF):
                sam3uFWLoc = str(defLocfwF)
                QSettings().setValue("cwlite-sam3u-firmware-location", sam3uFWLoc)

        self.firmwareLocation.setText(sam3uFWLoc)

        gbDone = QGroupBox("Step 3. Reset")
        gbDoneLayout = QHBoxLayout()
        gbDone.setLayout(gbDoneLayout)
        gbDoneLabel = QLabel("If the above succeeds, you need to power-cycle the target. Unplug & replug target. You may also " +
                             "need to close and re-open ChipWhisperer-Capture.")
        gbDoneLabel.setWordWrap(True)
        gbDoneLayout.addWidget(gbDoneLabel)
        layout.addWidget(gbDone)

        self.setLayout(layout)

    def findFirmware(self):
        fname, _ = QFileDialog.getOpenFileName(self, 'Find Firmware',  QSettings().value("cwlite-sam3u-firmware-location"), '*.bin')
        if fname:
            self.firmwareLocation.setText(fname)
            QSettings().setValue("cwlite-sam3u-firmware-location", fname)

    def enableBootloader(self, ask=True):
        if ask:
            # Check user isn't dumb
            reply = QMessageBox.warning(self, "Bootloader Warning", "Clicking 'YES' will enable bootloader mode. To disable this mode you " +
                                              "must load new firmware, it will not be cleared by power cycling. If you have trouble see " +
                                              "documentation, or ask at https://newae.com/forum.", QMessageBox.Yes | QMessageBox.No,
                                              QMessageBox.No)

            if reply != QMessageBox.Yes:
                return

        self.cwLiteUSB.enterBootloader(True)

    def runSamba(self):
        sam = Samba()

        self.programStatus.setText("Attempting to connect to %s\n"%self.serlist.currentText())
        sam.con(self.serlist.currentText())
        self.programStatus.append("Erasing...\n")
        sam.erase()
        self.programStatus.append("Programming %s\n"%(self.firmwareLocation.text()))
        data = open(self.firmwareLocation.text(),'rb').read()
        sam.write(data)
        if sam.verify(data):
            self.programStatus.append("Program OK (Verify Passed!)\n")
            sam.flash.setBootFlash(True)
        else:
            self.programStatus.append("Verify FAILED\n")
        sam.ser.close()

    def serialRefresh(self):
        serialnames = scan.scan()
        self.serlist.clear()
        for s in serialnames:
            self.serlist.addItem(s)
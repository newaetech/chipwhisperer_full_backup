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
import importlib
from functools import partial

try:
    from PySide.QtCore import *
    from PySide.QtGui import *
except ImportError:
    print "ERROR: PySide is required for this program"
    sys.exit()

import os.path
import time
from subprocess import Popen, PIPE
import shlex

class FWLoaderConfig(QDialog):
    def __init__(self, parent=None, console=None):
        super(FWLoaderConfig, self).__init__(parent)

        self.console = console

        self.setWindowTitle("ChipWhisperer Firmware Loader Configuration")

        layout = QVBoxLayout()

        layoutFWLoader = QHBoxLayout()
        self.fwLocation = QLineEdit()
        fwButton = QPushButton("Find")
        fwButton.clicked.connect(self.findFWLoader)
        layoutFWLoader.addWidget(QLabel("FWLoader Location"))
        layoutFWLoader.addWidget(self.fwLocation)
        layoutFWLoader.addWidget(fwButton)
        layout.addLayout(layoutFWLoader)

        layoutBit = QHBoxLayout()
        self.bitLocation = QLineEdit()
        bitButton = QPushButton("Find")
        bitButton.clicked.connect(self.findBitstream)
        layoutBit.addWidget(QLabel("FPGA Bitstream Location"))
        layoutBit.addWidget(self.bitLocation)
        layoutBit.addWidget(bitButton)
        layout.addLayout(layoutBit)

        layoutFW = QHBoxLayout()
        self.firmwareLocation = QLineEdit()
        firmwareButton = QPushButton("Find")
        firmwareButton.clicked.connect(self.findFirmware)
        layoutFW.addWidget(QLabel("Firmware Location"))
        layoutFW.addWidget(self.firmwareLocation)
        layoutFW.addWidget(firmwareButton)
        layout.addLayout(layoutFW)

        # Set dialog layout
        self.setLayout(layout)

        settings = QSettings()

        self.javaCmd = 'java'

        # Override this if path-specific java required, e.g.:
        # self.javaCmd = '"C:/Program Files/Java/jre7/bin/java"'

        fwLLoc = settings.value("fwloader-location")
        bsLoc = settings.value("bitstream-location")
        fwFLoc = settings.value("firmware-location")

        # Defaults?
        # print os.getcwd()

        if not fwLLoc:
            defLocfwL = "../../../hardware/capture/chipwhisperer-rev2/ezusb-firmware/ztex-sdk/fwloader/FWLoader.jar"
            # defLocfwL = os.path.realpath(defLocfwL)
            if os.path.isfile(defLocfwL):
                fwLLoc = str(defLocfwL)

        if not fwFLoc:
            defLocfwF = "../../../hardware/capture/chipwhisperer-rev2/ezusb-firmware/ztex-sdk/examples/usb-fpga-1.11/1.11c/openadc/OpenADC.ihx"
            # defLocfwF = os.path.realpath(defLocfwF)
            if os.path.isfile(defLocfwF):
                fwFLoc = str(defLocfwF)

        if not bsLoc:
            defLocbs = "../../../hardware/capture/chipwhisperer-rev2/hdl/ztex_rev2_1.11c_ise/interface.bit"
            # defLocbs = os.path.realpath(defLocbs)
            if os.path.isfile(defLocbs):
                bsLoc = str(defLocbs)

        self.fwLocation.setText(fwLLoc)
        self.bitLocation.setText(bsLoc)
        self.firmwareLocation.setText(fwFLoc)

    def findFWLoader(self):
        fname, _ = QFileDialog.getOpenFileName(self, 'Find FWLoader', '.', 'FWLoader.jar')
        if fname:
            self.fwLocation.setText(fname)
            QSettings().setValue("fwloader-location", fname)

    def findBitstream(self):
        fname, _ = QFileDialog.getOpenFileName(self, 'Find Bitstream', '.', '*.bit')
        if fname:
            self.bitLocation.setText(fname)
            QSettings().setValue("bitstream-location", fname)

    def findFirmware(self):
        fname, _ = QFileDialog.getOpenFileName(self, 'Find Firmware', '.', '*.ihx')
        if fname:
            self.firmwareLocation.setText(fname)
            QSettings().setValue("firmware-location", fname)

    def getStatus(self):
        """Get the output of the FWLoader command with the -i (info) option"""
        # Check Status
        cmd = '%s -cp %s FWLoader -c -i' % (self.javaCmd, self.fwLocation.text())
        # print shlex.split(cmd)
        process = Popen(shlex.split(cmd), stdout=PIPE, stderr=PIPE, shell=True)
        stdout, stderr = process.communicate()
        process.wait()

        if "devices" in stderr:
            if self.console:
                self.console.append(stderr)
        return stdout

    def loadRequired(self, forceFirmware=False):
        """Load firmware file or FPGA file only as required, skip otherwise"""
        stdout = self.getStatus()

        if "OpenADC" not in stdout or forceFirmware:
            # Load firmware
            self.loadFirmware()
            stdout = self.getStatus()
        else:
            if self.console:
                self.console.append("Skipped firmware download")

        if "FPGA configured" not in stdout:
            self.loadFPGA()
        else:
            if self.console:
                self.console.append("Skipped FPGA download")

    def loadFirmware(self):
        """Load the USB microcontroller firmware file setup in the dialog"""
        cmd = "%s -cp %s FWLoader -c -f -uu %s" % (self.javaCmd, self.fwLocation.text(), self.firmwareLocation.text())
        # print shlex.split(cmd)
        process = Popen(shlex.split(cmd), stdout=PIPE, stderr=PIPE, shell=True)
        stdout, stderr = process.communicate()
        exit_code = process.wait()
        if self.console:
            self.console.append(stdout)
            self.console.append(stderr)

    def loadFPGA(self):
        """Load the FPGA bitfile set up in the dialog"""
        cmd = "%s -cp %s FWLoader -f -uf %s" % (self.javaCmd, self.fwLocation.text(), self.bitLocation.text())
        # print shlex.split(cmd)

        # Get bitstream date, print for user
        bsdate = os.path.getmtime(self.bitLocation.text())
        print "FPGA Bitstream Created: %s" % time.ctime(bsdate)
        
        # Save last loaded FPGA bitstream
        QSettings().setValue("bitstream-date", bsdate)

        process = Popen(shlex.split(cmd), stdout=PIPE, stderr=PIPE, shell=True)
        stdout, stderr = process.communicate()
        exit_code = process.wait()
        if self.console:
            self.console.append(stdout)
            self.console.append(stderr)

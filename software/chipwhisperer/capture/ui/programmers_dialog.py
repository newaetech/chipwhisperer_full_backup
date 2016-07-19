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
from datetime import datetime
from PySide.QtCore import *
from PySide.QtGui import *
from PySide.QtGui import QVBoxLayout, QHBoxLayout, QPushButton, QLabel, QPlainTextEdit, QFileDialog

from chipwhisperer.common.ui.CWMainGUI import CWMainGUI
from chipwhisperer.capture.api.programmers import AVRProgrammer, XMEGAProgrammer
from chipwhisperer.common.utils import qt_tweaks as QtFixes


class AVRProgrammerDialog(QtFixes.QDialog):
    def __init__(self):
        super(AVRProgrammerDialog, self).__init__(CWMainGUI.getInstance())
        # self.setWindowFlags(self.windowFlags() | Qt.WindowStaysOnTopHint)

        self.avr = AVRProgrammer()

        self.setWindowTitle("ChipWhisperer-Lite AVR Programmer")
        layout = QVBoxLayout()

        layoutFW = QHBoxLayout()
        self.flashLocation = QtFixes.QLineEdit()
        flashFileButton = QPushButton("Find")
        flashFileButton.clicked.connect(self.findFlash)
        layoutFW.addWidget(QLabel("FLASH File"))
        layoutFW.addWidget(self.flashLocation)
        layoutFW.addWidget(flashFileButton)
        layout.addLayout(layoutFW)

        self.flashLocation.setText(QSettings().value("avr-flash-location"))

        # Add buttons
        readSigBut = QPushButton("Check Signature")
        readSigBut.clicked.connect(self.readSignature)
        verifyFlashBut = QPushButton("Verify FLASH")
        verifyFlashBut.clicked.connect(self.verifyFlash)
        verifyFlashBut.setEnabled(False)
        progFlashBut = QPushButton("Erase/Program/Verify FLASH")
        progFlashBut.clicked.connect(self.writeFlash)

        layoutBut = QHBoxLayout()
        layoutBut.addWidget(readSigBut)
        layoutBut.addWidget(verifyFlashBut)
        layoutBut.addWidget(progFlashBut)
        layout.addLayout(layoutBut)

        layoutFuse = QHBoxLayout()

        readFuseBut = QPushButton("Read Fuses")
        readFuseBut.clicked.connect(self.readFuses)
        writeFuseBut = QPushButton("Write Fuses")
        writeFuseBut.clicked.connect(self.writeFuses)

        self.lowfuseLine = QtFixes.QLineEdit("?")
        self.lowfuseLine.setMaxLength(2)
        self.lowfuseLine.setFixedWidth(25)
        self.highfuseLine = QtFixes.QLineEdit("?")
        self.highfuseLine.setMaxLength(2)
        self.highfuseLine.setFixedWidth(25)
        self.extfuseLine = QtFixes.QLineEdit("?")
        self.extfuseLine.setMaxLength(2)
        self.extfuseLine.setFixedWidth(25)

        # Don't allow use to change these fuses
        self.highfuseLine.setReadOnly(True)
        self.extfuseLine.setReadOnly(True)

        layoutFuse.addWidget(readFuseBut)
        layoutFuse.addWidget(QLabel("LOW:"))
        layoutFuse.addWidget(self.lowfuseLine)
        layoutFuse.addWidget(QLabel("HIGH:"))
        layoutFuse.addWidget(self.highfuseLine)
        layoutFuse.addWidget(QLabel("EXT:"))
        layoutFuse.addWidget(self.extfuseLine)
        layoutFuse.addWidget(writeFuseBut)
        layout.addLayout(layoutFuse)

        layoutExtra = QHBoxLayout()
        self.clockMode = QPushButton("Enable Slow Clock Mode")
        self.clockMode.setCheckable(True)
        self.clockMode.clicked.connect(self.toggleSlowClock)
        layoutExtra.addWidget(self.clockMode)
        layoutExtra.addStretch()
        layout.addLayout(layoutExtra)

        # Add status stuff
        self.statusLine = QPlainTextEdit()
        self.statusLine.setReadOnly(True)
        # self.statusLine.setFixedHeight(QFontMetrics(self.statusLine.font()).lineSpacing() * 5 + 10)
        layout.addWidget(self.statusLine)
        self.avr.newTextLog.connect(self.append)

        # Set dialog layout
        self.setLayout(layout)

    def append(self, text):
        self.statusLine.appendPlainText(text)

    def toggleSlowClock(self):
        if self.clockMode.isChecked():
            self.avr.avr.enableSlowClock(True)
            self.clockMode.setText("Disable Slow Clock Mode")
        else:
            self.avr.avr.enableSlowClock(False)
            self.clockMode.setText("Enable Slow Clock Mode")

    def reject(self):
        #Ensure we disable slow-clock mode
        if self.clockMode.isChecked():
            self.clockMode.setChecked(False)
            self.toggleSlowClock()
            
        #Release AVR
        self.avr.close()

        QDialog.reject(self)

    def findFlash(self):
        fname, _ = QFileDialog.getOpenFileName(self, 'Find FLASH File', QSettings().value("avr-flash-location"), '*.hex')
        if fname:
            self.flashLocation.setText(fname)
            QSettings().setValue("avr-flash-location", fname)

    def readSignature(self, close=True):
        self.avr.find()
        if close:
            self.avr.close()

    def readFuses(self):
        try:
            self.readSignature(close=False)
            self.statusLine.appendPlainText("Reading fuses")
            lfuse = self.avr.avr.readFuse("low")
            hfuse = self.avr.avr.readFuse("high")
            efuse = self.avr.avr.readFuse("extended")
            self.statusLine.appendPlainText("OK: %02x %02x %02x" % (lfuse, hfuse, efuse))
            self.lowfuseLine.setText("%02x" % lfuse)
            self.highfuseLine.setText("%02x" % hfuse)
            self.extfuseLine.setText("%02x" % efuse)
        except IOError:
            self.statusLine.appendPlainText("Reading fuses failed")
            self.lowfuseLine.setText("?")
            self.highfuseLine.setText("?")
            self.extfuseLine.setText("?")
            self.avr.close()

    def writeFuses(self):
        lfuse = int(self.lowfuseLine.text(), 16)
        # hfuse = int(self.highfuseLine.text(), 16)
        # efuse = int(self.extfuseLine.text(), 16)

        self.statusLine.appendPlainText("Writing fuse: not only 'low' fuse is written, as hfuse/efuse can disable device\n")

        try:
            self.readSignature(close=False)
            self.avr.avr.writeFuse("low", lfuse)
            # self.avr.avr.writeFuse("high", hfuse)
            # self.avr.avr.writeFuse("extended", efuse)
            # print("%x %x %x" % (lfuse, hfuse, efuse))
            self.avr.close()
        except:
            self.avr.close()
            raise

    def verifyFlash(self):
        self.statusLine.appendPlainText("Verify not implemented")

    def writeFlash(self, erase=True, verify=True):
        status = "FAILED"
        fname = self.flashLocation.text()
        self.statusLine.appendPlainText("***Starting FLASH program process at %s***" % datetime.now().strftime('%H:%M:%S'))
        if (os.path.isfile(fname)):
            self.statusLine.appendPlainText("File %s last changed on %s" % (fname, time.ctime(os.path.getmtime(fname))))
            QCoreApplication.processEvents()

            try:
                self.statusLine.appendPlainText("Entering Programming Mode")
                QCoreApplication.processEvents()
                self.readSignature(close=False)

                if erase:
                    self.avr.erase()
                    self.avr.close()
                    self.readSignature(close=False)

                QCoreApplication.processEvents()
                self.avr.program(self.flashLocation.text(), memtype="flash", verify=verify)
                QCoreApplication.processEvents()
                self.statusLine.appendPlainText("Exiting programming mode")
                self.avr.close()
                QCoreApplication.processEvents()

                status = "SUCCEEDED"

            except IOError as e:
                self.statusLine.appendPlainText("FAILED: %s" % str(e))
                try:
                    self.avr.close()
                except IOError:
                    pass

        else:
            self.statusLine.appendPlainText("%s does not appear to be a file, check path" % fname)

        self.statusLine.appendPlainText("***FLASH Program %s at %s***" % (status, datetime.now().strftime('%H:%M:%S')))

    def setUSBInterface(self, iface):
        self.avr.setUSBInterface(iface)


class XMEGAProgrammerDialog(QtFixes.QDialog):
    def __init__(self):
        super(XMEGAProgrammerDialog, self).__init__(CWMainGUI.getInstance())
        # self.setWindowFlags(self.windowFlags() | Qt.WindowStaysOnTopHint)
        self.xmega = XMEGAProgrammer()

        self.setWindowTitle("ChipWhisperer-Lite XMEGA Programmer")
        layout = QVBoxLayout()

        layoutFW = QHBoxLayout()
        self.flashLocation = QtFixes.QLineEdit()
        flashFileButton = QPushButton("Find")
        flashFileButton.clicked.connect(self.findFlash)
        layoutFW.addWidget(QLabel("FLASH File"))
        layoutFW.addWidget(self.flashLocation)
        layoutFW.addWidget(flashFileButton)
        layout.addLayout(layoutFW)

        self.flashLocation.setText(QSettings().value("xmega-flash-location"))

        # Add buttons
        readSigBut = QPushButton("Check Signature")
        readSigBut.clicked.connect(self.readSignature)
        verifyFlashBut = QPushButton("Verify FLASH")
        verifyFlashBut.clicked.connect(self.verifyFlash)
        verifyFlashBut.setEnabled(False)
        progFlashBut = QPushButton("Erase/Program/Verify FLASH")
        progFlashBut.clicked.connect(self.writeFlash)

        layoutBut = QHBoxLayout()
        layoutBut.addWidget(readSigBut)
        layoutBut.addWidget(verifyFlashBut)
        layoutBut.addWidget(progFlashBut)
        layout.addLayout(layoutBut)

        # Add status stuff
        self.statusLine = QPlainTextEdit()
        self.statusLine.setReadOnly(True)
        # self.statusLine.setFixedHeight(QFontMetrics(self.statusLine.font()).lineSpacing() * 5 + 10)
        self.xmega.newTextLog.connect(self.append)
        layout.addWidget(self.statusLine)

        # Set dialog layout
        self.setLayout(layout)

    def append(self, text):
        self.statusLine.appendPlainText(text)

    def findFlash(self):
        fname, _ = QFileDialog.getOpenFileName(self, 'Find FLASH File', QSettings().value("xmega-flash-location"), '*.hex')
        if fname:
            self.flashLocation.setText(fname)
            QSettings().setValue("xmega-flash-location", fname)

    def readSignature(self, close=True):
        self.xmega.find()
        if close:
            self.xmega.close()

    def verifyFlash(self):
        pass

    def writeFlash(self, erase=True, verify=True):
        status = "FAILED"

        fname = self.flashLocation.text()
        self.statusLine.appendPlainText("***Starting FLASH program process at %s***" % datetime.now().strftime('%H:%M:%S'))
        if (os.path.isfile(fname)):
            self.statusLine.appendPlainText("File %s last changed on %s" % (fname, time.ctime(os.path.getmtime(fname))))
            QCoreApplication.processEvents()

            try:
                self.statusLine.appendPlainText("Entering Programming Mode")
                QCoreApplication.processEvents()
                self.readSignature(close=False)

                if erase:
                    try:
                        self.xmega.erase()
                    except IOError:
                        self.statusLine.appendPlainText("**chip-erase timeout, erasing application only**")
                        QCoreApplication.processEvents()
                        self.xmega.xmega.enablePDI(False)
                        self.xmega.xmega.enablePDI(True)
                        self.xmega.erase("app")

                QCoreApplication.processEvents()
                self.xmega.program(self.flashLocation.text(), memtype="flash", verify=verify)
                QCoreApplication.processEvents()
                self.statusLine.appendPlainText("Exiting programming mode")
                self.xmega.close()
                QCoreApplication.processEvents()

                status = "SUCCEEDED"

            except IOError, e:
                self.statusLine.appendPlainText("FAILED: %s" % str(e))
                try:
                    self.xmega.close()
                except IOError:
                    pass

        else:
            self.statusLine.appendPlainText("%s does not appear to be a file, check path" % fname)

        self.statusLine.appendPlainText("***FLASH Program %s at %s***" % (status, datetime.now().strftime('%H:%M:%S')))

    def setUSBInterface(self, iface):
        self.xmega.setUSBInterface(iface)

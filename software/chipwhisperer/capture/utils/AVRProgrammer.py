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
from chipwhisperer.capture.scopes.cwhardware.ChipWhispererLite import AVRISP
from chipwhisperer.capture.scopes.cwhardware.ChipWhispererLite import CWLiteUSB
from chipwhisperer.capture.scopes.cwhardware.ChipWhispererLite_progdevice import supported_avr
from chipwhisperer.capture.utils.IntelHex import IntelHex
from chipwhisperer.common.utils import util


# import chipwhisperer.capture.global_mod as global_mod

class AVRProgrammerDialog(QDialog):
    def __init__(self, parent=None):
        super(AVRProgrammerDialog, self).__init__(parent)
        # self.setWindowFlags(self.windowFlags() | Qt.WindowStaysOnTopHint)

        self.avr = AVRProgrammer()

        self.setWindowTitle("ChipWhisperer-Lite AVR Programmer")
        layout = QVBoxLayout()

        layoutFW = QHBoxLayout()
        self.flashLocation = QLineEdit()
        flashFileButton = QPushButton("Find")
        flashFileButton.clicked.connect(self.findFlash)
        layoutFW.addWidget(QLabel("FLASH File"))
        layoutFW.addWidget(self.flashLocation)
        layoutFW.addWidget(flashFileButton)
        layout.addLayout(layoutFW)

        self.flashLocation.setText(util.globalSettings["avr-flash-location"])

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

        self.lowfuseLine = QLineEdit("?")
        self.lowfuseLine.setMaxLength(2)
        self.lowfuseLine.setFixedWidth(25)
        self.highfuseLine = QLineEdit("?")
        self.highfuseLine.setMaxLength(2)
        self.highfuseLine.setFixedWidth(25)
        self.extfuseLine = QLineEdit("?")
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
        self.statusLine.append = self.statusLine.appendPlainText
        layout.addWidget(self.statusLine)

        self.avr._logging = self.statusLine.append

        # Set dialog layout
        self.setLayout(layout)

    def toggleSlowClock(self):
        if self.clockMode.isChecked():
            self.avr.avr.enableSlowClock(True)
            self.clockMode.setText("Disable Slow Clock Mode")
        else:
            self.avr.avr.enableSlowClock(False)
            self.clockMode.setText("Enable Slow Clock Mode")

    def reject(self):
        if self.clockMode.isChecked():
            self.clockMode.setChecked(False)
            self.toggleSlowClock()

        QDialog.reject(self)


    def findFlash(self):
        fname, _ = QFileDialog.getOpenFileName(self, 'Find FLASH File', util.globalSettings["avr-flash-location"], '*.hex')
        if fname:
            self.flashLocation.setText(fname)
            util.globalSettings["avr-flash-location"] = fname

    def readSignature(self, close=True):
        self.avr.find()
        if close:
            self.avr.close()

    def readFuses(self):
        try:
            self.readSignature(close=False)
            self.statusLine.append("Reading fuses")
            lfuse = self.avr.avr.readFuse("low")
            hfuse = self.avr.avr.readFuse("high")
            efuse = self.avr.avr.readFuse("extended")
            self.statusLine.append("OK: %02x %02x %02x" % (lfuse, hfuse, efuse))
            self.lowfuseLine.setText("%02x" % lfuse)
            self.highfuseLine.setText("%02x" % hfuse)
            self.extfuseLine.setText("%02x" % efuse)
        except IOError:
            self.statusLine.append("Reading fuses failed")
            self.lowfuseLine.setText("?")
            self.highfuseLine.setText("?")
            self.extfuseLine.setText("?")
            self.avr.close()

    def writeFuses(self):
        lfuse = int(self.lowfuseLine.text(), 16)
        # hfuse = int(self.highfuseLine.text(), 16)
        # efuse = int(self.extfuseLine.text(), 16)

        self.statusLine.append("Writing fuse: not only 'low' fuse is written, as hfuse/efuse can disable device\n")

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
        self.statusLine.append("Verify not implemented")

    def writeFlash(self, erase=True, verify=True):
        status = "FAILED"
        fname = self.flashLocation.text()
        self.statusLine.append("***Starting FLASH program process at %s***" % datetime.now().strftime('%H:%M:%S'))
        if (os.path.isfile(fname)):
            self.statusLine.append("File %s last changed on %s" % (fname, time.ctime(os.path.getmtime(fname))))
            QCoreApplication.processEvents()

            try:
                self.statusLine.append("Entering Programming Mode")
                QCoreApplication.processEvents()
                self.readSignature(close=False)

                if erase:
                    self.statusLine.append("Erasing Chip")
                    QCoreApplication.processEvents()
                    self.avr.erase()
                    self.avr.close()
                    self.readSignature(close=False)

                QCoreApplication.processEvents()
                self.avr.program(self.flashLocation.text(), memtype="flash", verify=verify)
                QCoreApplication.processEvents()
                self.statusLine.append("Exiting programming mode")
                self.avr.close()
                QCoreApplication.processEvents()

                status = "SUCCEEDED"

            except IOError, e:
                self.statusLine.append("FAILED: %s" % str(e))
                try:
                    self.avr.close()
                except IOError:
                    pass

        else:
            self.statusLine.append("%s does not appear to be a file, check path" % fname)

        self.statusLine.append("***FLASH Program %s at %s***" % (status, datetime.now().strftime('%H:%M:%S')))

    def setUSBInterface(self, iface):
        self.avr.setUSBInterface(iface._usbdev)


class AVRProgrammer(object):
    
    def __init__(self):
        super(AVRProgrammer, self).__init__()
        self._usbiface = None
        self.supported_chips = []
        self.avr = AVRISP()
        self._logging = None
        self._foundchip = False

    def setUSBInterface(self, iface):
        self._usbiface = iface
        self._foundchip = False
        self.avr.setUSB(iface)
        # self.avr.setChip(self.supported_chips[0])

    def find(self):
        self._foundchip = False

        self.avr.enableISP(True)
        sig = self.avr.readSignature()

        # self.log("Signature = %02x %02x %02x" % (sig[0], sig[1], sig[2]))

        # Figure out which device?
        # Check if it's one we know about?
        for t in supported_avr:
            if ((sig[0] == t.signature[0]) and
                (sig[1] == t.signature[1]) and
                (sig[2] == t.signature[2])):

                self._foundchip = True

                self.log("Detected %s" % t.name)
                self.avr.setChip(t)
                break

        # Print signature of unknown device
        if self._foundchip == False:
            self.log("Detected Unknown Chip, sig=%2x %2x %2x" % (sig[0], sig[1], sig[2]))


    def erase(self):
        self.avr.eraseChip()

    def program(self, filename, memtype="flash", verify=True):
        f = IntelHex(filename)

        maxsize = self.avr._chip.memtypes[memtype]["size"]
        fsize = f.maxaddr() - f.minaddr()

        if fsize > maxsize:
            raise IOError("File %s appears to be %d bytes, larger than %s size of %d" % (filename, fsize, memtype, maxsize))

        self.log("AVR Programming %s..." % memtype)
        QCoreApplication.processEvents()
        fdata = f.tobinarray(start=0)
        self.avr.writeMemory(0, fdata, memtype)

        self.log("AVR Reading %s..." % memtype)
        QCoreApplication.processEvents()
        # Do verify run
        rdata = self.avr.readMemory(0, len(fdata))  # memtype ?

        for i in range(0, len(fdata)):
            if fdata[i] != rdata[i]:
                raise IOError("Verify failed at 0x%04x, %x != %x" % (i, fdata[i], rdata[i]))

        self.log("Verified %s OK, %d bytes" % (memtype, fsize))

    
    def close(self):
        self.avr.enableISP(False)

    def log(self, text):
        if self._logging is None:
            print text
        else:
            self._logging(text)


if __name__ == '__main__':
    cwtestusb = CWLiteUSB()
    cwtestusb.con()

    fname = r"C:\E\Documents\academic\sidechannel\chipwhisperer\hardware\victims\firmware\xmega-glitch\simpleserial.hex"

    xmega = AVRProgrammer()
    xmega.setUSBInterface(cwtestusb._usbdev)
    xmega.find()
    try:
        print "Erasing"
        xmega.erase("chip")
    except IOError:
        print "**chip-erase timeout, workaround enabled**"
        time.sleep(0.1)
        xmega.xmega.enablePDI(False)
        xmega.xmega.enablePDI(True)
    # xmega.program(fname, "flash")
    print "%02x" % xmega.xmega.readMemory(0x8f0025, 1)[0]
    xmega.close()


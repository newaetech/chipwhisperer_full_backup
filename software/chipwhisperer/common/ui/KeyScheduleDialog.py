#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2014, NewAE Technology Inc
# All rights reserved.
#
# Author: Colin O'Flynn
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

from PySide.QtGui import *
import chipwhisperer.common.utils.qt_tweaks as QtFixes
from chipwhisperer.common.utils import util
from chipwhisperer.analyzer.attacks.models.AES128_8bit import AES128_8bit
from chipwhisperer.analyzer.attacks.models.DES import DES


class AesKeyScheduleDialog(QtFixes.QDialog):

    def __init__(self, parent=None):
        super(AesKeyScheduleDialog, self).__init__(parent)
        self.model = AES128_8bit()

        layout = QVBoxLayout()
        self.setLayout(layout)

        self.outmode = QComboBox()
        self.outmode.addItem("AABBCC...EEFF", ["", ""])
        self.outmode.addItem("AA:BB:CC...EE:FF", ["", ":"])
        self.outmode.addItem("AA BB CC...EE FF", ["", " "])
        self.outmode.addItem("AA,BB,CC...EE,FF", ["", ","])
        self.outmode.addItem("0xAA, 0xBB, 0xCC... 0xEE, 0xFF", ["0x", ", "])
        self.outmode.currentIndexChanged.connect(self.inTextChanged)

        self.indata = QtFixes.QLineEdit("")
        self.indata.setFont(QFont("Courier"))

        self.keysched = QTextEdit("")
        self.keysched.setFont(QFont("Courier"))

        self.outkey = QtFixes.QLineEdit("")
        self.outkey.setReadOnly(True)
        self.outkey.setFont(QFont("Courier"))

        outmodeL = QHBoxLayout()
        outmodeL.addWidget(QLabel("Format:"))
        outmodeL.addWidget(self.outmode)
        outmodeL.addStretch()

        self.inprnd = QComboBox()
        self.inprnd.currentIndexChanged.connect(self.inTextChanged)
        self.setKeyLength(128)

        indataL = QHBoxLayout()
        indataL.addWidget(QLabel("Key:"))
        indataL.addWidget(self.indata)
        indataL.addWidget(self.inprnd)
        self.indata.textChanged.connect(self.inTextChanged)

        outdataL = QHBoxLayout()
        outdataL.addWidget(QLabel("Key:"))
        outdataL.addWidget(self.outkey)

        gbIndata = QGroupBox("Input Known Key")
        gbIndata.setLayout(indataL)
        layout.addWidget(gbIndata)

        gbOutdata = QGroupBox("Output Desired Key")
        outdataTotalL = QVBoxLayout()
        outdataTotalL.addLayout(outmodeL)
        outdataTotalL.addLayout(outdataL)
        gbOutdata.setLayout(outdataTotalL)
        layout.addWidget(gbOutdata)

        gbKeySched = QGroupBox("Full Key Schedule")
        keyschedL = QVBoxLayout()
        keyschedL.addWidget(self.keysched)
        gbKeySched.setLayout(keyschedL)

        layout.addWidget(gbKeySched)

        self.setWindowTitle("AES-128/AES-256 Key Schedule Calculator")
        self.setObjectName("AES Key Schedule")
        self.setWindowIcon(QIcon(":/images/cwiconA.png"))
        self.setMinimumSize(800,400)

    def aesmodeChanged(self, indx):
        self.setKeyLength(self.aesmode.itemData(indx))

    def setKeyLength(self, klen):
        pi = self.inprnd.currentIndex()
        self.inprnd.blockSignals(True)
        if klen == 128:
            self.inprnd.clear()
            self.inprnd.addItem("0  (Initial Enc.)", 0)
            self.inprnd.addItem("1  (2nd-Round Enc.)", 1)
            self.inprnd.addItem("10 (Initial Dec.)", 10)
        elif klen == 256:
            self.inprnd.clear()
            self.inprnd.addItem("0/1   (Initial Enc.)", 0)
            self.inprnd.addItem("13/14 (Initial Dec.)", 13)
        else:
            raise ValueError("Invalid keylength: %d" % klen)

        if pi > -1:
            self.inprnd.setCurrentIndex(pi)
        self.inprnd.blockSignals(False)

    def inTextChanged(self, data=None):
        data = self.indata.text()
        try:
            newdata = util.hexstr2list(data)

            if len(newdata) != 16 and len(newdata) != 32:
                err = "ERR: Len=%d: %s" % (len(newdata), newdata)
                self.outkey.setText(err)
                self.keysched.setText(err)
            else:
                if len(newdata) == 16:
                    self.setKeyLength(128)
                elif len(newdata) == 32:
                    self.setKeyLength(256)

                #Read settings
                delim = self.outmode.itemData(self.outmode.currentIndex())
                desired = 0
                inpround = self.inprnd.itemData(self.inprnd.currentIndex())

                key = newdata

                # Get initial key
                result = self.model.keyScheduleRounds(key, inpround, desired)
                if len(key) == 32:
                    result.extend(self.model.keyScheduleRounds(key, inpround, desired + 1))

                rstr = ["%02x" % t for t in result]
                rstr = (delim[1] + delim[0]).join(rstr)
                rstr = delim[0] + rstr

                self.outkey.setText(rstr)

                # Get entire key schedule
                if len(key) == 16:
                    rnds = 10
                elif len(key) == 32:
                    rnds = 14

                totalrndstr = ''
                for r in range(0, rnds+1):
                    result = self.model.keyScheduleRounds(key, inpround, r)
                    str = ["%02x" % t for t in result]
                    str = (delim[1] + delim[0]).join(str)
                    str = delim[0] + str
                    totalrndstr += "%2d: " % r
                    totalrndstr += str + "\n"

                self.keysched.setText(totalrndstr)

        except ValueError:
            self.outkey.setText("ERR in HEX: %s" % data)


class DesKeyScheduleDialog(QtFixes.QDialog):

    def __init__(self, parent=None):
        super(DesKeyScheduleDialog, self).__init__(parent)
        self.model = DES()

        layout = QVBoxLayout()
        self.setLayout(layout)

        self.indata = QtFixes.QLineEdit("")
        self.indata.setFont(QFont("Courier"))

        self.keysched = QTextEdit("")
        self.keysched.setFont(QFont("Courier"))

        self.inprnd = QComboBox()
        self.inprnd.currentIndexChanged.connect(self.inTextChanged)

        indataL = QHBoxLayout()
        indataL.addWidget(QLabel("Key:"))
        indataL.addWidget(self.indata)
        indataL.addWidget(QLabel("Round:"))
        indataL.addWidget(self.inprnd)
        self.indata.textChanged.connect(self.inTextChanged)

        gbIndata = QGroupBox("Round Key/Subkey")
        gbIndata.setLayout(indataL)
        layout.addWidget(gbIndata)

        gbKeySched = QGroupBox("Full Key Schedule")
        keyschedL = QVBoxLayout()
        keyschedL.addWidget(self.keysched)
        keyschedL.addWidget(QLabel("X - Parity bits (not used); ? - Unknown/lost bits (could not be recovered)"))
        gbKeySched.setLayout(keyschedL)

        layout.addWidget(gbKeySched)

        self.setWindowTitle("DES Key Schedule Calculator")
        self.setObjectName("DES Key Schedule")
        self.refreshRoundKeysLength()
        self.setMinimumSize(800,400)

    def refreshRoundKeysLength(self):
        self.inprnd.blockSignals(True)
        self.inprnd.clear()
        for n in range(self.model.getNumRoundKeys()+1):
            self.inprnd.addItem(str(n), n)
        self.inprnd.setCurrentIndex(0)
        self.inprnd.blockSignals(False)

    def inTextChanged(self, _=None):
        try:
            key = util.hexstr2list(self.indata.text())
            key = [int(d) for d in key]

            #Read settings
            inpround = self.inprnd.itemData(self.inprnd.currentIndex())

            # Get entire key schedule
            totalrndstr = ""
            roundKeys = self.model.getRoundKeys(key, inpround)
            for i, key in enumerate(roundKeys):
                totalrndstr += "%2d: " % i
                for j, bit in enumerate(key):
                    if bit is not None:
                        totalrndstr += str(bit)
                    elif i == 0 and j % 8 == 7:
                        totalrndstr += 'X'
                    else:
                        totalrndstr += '?'
                totalrndstr += "\n"

            self.keysched.setText(totalrndstr)

        except:
            self.keysched.clear()

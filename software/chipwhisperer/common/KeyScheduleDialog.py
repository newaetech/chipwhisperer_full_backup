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

import sys

try:
    from PySide.QtCore import *
    from PySide.QtGui import *
except ImportError:
    print "ERROR: PySide is required for this program"
    sys.exit()

from chipwhisperer.common.aes_key_schedule import keyScheduleRounds

class KeyScheduleDialog(QDialog):

    def __init__(self, parent=None):
        super(KeyScheduleDialog, self).__init__(parent)

        layout = QVBoxLayout()
        self.setLayout(layout)

        self.outmode = QComboBox()
        self.outmode.addItem("AABBCC...EEFF", ["", ""])
        self.outmode.addItem("AA:BB:CC...EE:FF", ["", ":"])
        self.outmode.addItem("AA BB CC...EE FF", ["", " "])
        self.outmode.addItem("AA,BB,CC...EE,FF", ["", ","])
        self.outmode.addItem("0xAA, 0xBB, 0xCC... 0xEE, 0xFF", ["0x", ", "])
        self.outmode.currentIndexChanged.connect(self.inTextChanged)

        self.indata = QLineEdit("")
        self.indata.setFont(QFont("Courier"))

        self.keysched = QTextEdit("")
        self.keysched.setFont(QFont("Courier"))

        self.outkey = QLineEdit("")
        self.outkey.setReadOnly(True)
        self.outkey.setFont(QFont("Courier"))


        outmodeL = QHBoxLayout()
        outmodeL.addWidget(QLabel("Desired Format:"))
        outmodeL.addWidget(self.outmode)
        outmodeL.addStretch()


        self.inprnd = QComboBox()
        self.inprnd.currentIndexChanged.connect(self.inTextChanged)
        self.setKeyLength(128)

        indataL = QHBoxLayout()
        indataL.addWidget(QLabel("Known Key:"))
        indataL.addWidget(self.indata)
        indataL.addWidget(self.inprnd)
        self.indata.textChanged.connect(self.inTextChanged)

        outdataL = QHBoxLayout()
        outdataL.addWidget(QLabel("Desired Key:"))
        outdataL.addWidget(self.outkey)

        layout.addLayout(indataL)
        layout.addLayout(outmodeL)
        layout.addLayout(outdataL)
        layout.addWidget(self.keysched)

        self.setWindowTitle("AES Key Schedule")
        self.setObjectName("AES Key Schedule")

    def aesmodeChanged(self, indx):
        self.setKeyLength(self.aesmode.itemData(indx))

    def setKeyLength(self, klen):
        pi = self.inprnd.currentIndex()
        self.inprnd.blockSignals(True)
        if klen == 128:
            self.inprnd.clear()
            self.inprnd.addItem("0  (Initial Enc.)", 0)
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

        newdata = data.lower()
        newdata = newdata.replace("0x", "")
        newdata = newdata.replace(",", "")
        newdata = newdata.replace(" ", "")
        newdata = newdata.replace("[", "")
        newdata = newdata.replace("]", "")
        newdata = newdata.replace("(", "")
        newdata = newdata.replace(")", "")
        newdata = newdata.replace("{", "")
        newdata = newdata.replace("}", "")
        newdata = newdata.replace(":", "")
        newdata = newdata.replace("-", "")

        if len(newdata) != 32 and len(newdata) != 64:
            self.outkey.setText("ERR: Len=%d: %s" % (len(newdata), newdata))
        else:
            if len(newdata) == 32:
                self.setKeyLength(128)
            elif len(newdata) == 64:
                self.setKeyLength(256)

            # Convert to hex
            try:
                key = [int(newdata[i:(i + 2)], 16) for i in range(0, len(newdata), 2)]
            except ValueError:
                self.outkey.setText("ERR in HEX: %s" % newdata)

            #Read settings
            delim = self.outmode.itemData(self.outmode.currentIndex())
            desired = 0
            inpround = self.inprnd.itemData(self.inprnd.currentIndex())

            # Get initial key
            result = keyScheduleRounds(key, inpround, desired)
            if len(newdata) == 64:
                result.extend(keyScheduleRounds(key, inpround, desired + 1))

            rstr = ["%02x" % t for t in result]
            rstr = (delim[1] + delim[0]).join(rstr)
            rstr = delim[0] + rstr

            self.outkey.setText(rstr)

            # Get entire key schedule
            if len(newdata) == 32:
                rnds = 10
            elif len(newdata)== 64:
                rnds = 14

            totalrndstr = ""
            for r in range(0, rnds+1):
                result = keyScheduleRounds(key, inpround, r)
                rstr = ["%02x" % t for t in result]
                rstr = (delim[1] + delim[0]).join(rstr)
                rstr = delim[0] + rstr
                totalrndstr += rstr + "\n"

            self.keysched.setText(totalrndstr)



if __name__ == '__main__':
    # Create the Qt Application
    app = QApplication(sys.argv)
    # Create and show the form
    form = KeyScheduleDialog()
    form.show()
    # Run the main Qt loop
    sys.exit(app.exec_())


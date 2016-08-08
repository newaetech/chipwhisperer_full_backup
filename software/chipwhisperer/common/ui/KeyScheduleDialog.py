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

from PySide.QtGui import *

import chipwhisperer.common.utils.qt_tweaks as QtFixes
from chipwhisperer.common.utils import pluginmanager
from chipwhisperer.common.utils import util


class KeyScheduleDialog(QtFixes.QDialog):

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

        models = pluginmanager.getPluginsInDictFromPackage("chipwhisperer.analyzer.attacks.models", True, False)
        self.algo = QComboBox()
        for k, v in models.iteritems():
            self.algo.addItem(k, v)
        self.algo.currentIndexChanged.connect(self.refreshRoundKeysLength)

        indataL = QHBoxLayout()
        indataL.addWidget(self.algo)
        indataL.addWidget(QLabel("Key:"))
        indataL.addWidget(self.indata)
        indataL.addWidget(QLabel("Round:"))
        indataL.addWidget(self.inprnd)
        self.indata.textChanged.connect(self.inTextChanged)

        outdataL = QHBoxLayout()
        outdataL.addWidget(QLabel("Subkeys:"))
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

        self.setWindowTitle("Key Schedule Calculator")
        self.setObjectName("Key Schedule")
        self.refreshRoundKeysLength()

    def refreshRoundKeysLength(self):
        model = self.algo.itemData(self.algo.currentIndex())
        self.inprnd.blockSignals(True)
        self.inprnd.clear()
        for n in range(model.getNumRoundKeys()+1):
            self.inprnd.addItem(str(n), n)
        self.inprnd.setCurrentIndex(0)
        self.inprnd.blockSignals(False)

    def inTextChanged(self, _=None):
        try:
            model = self.algo.itemData(self.algo.currentIndex())
            newdata = util.hexstr2list(self.indata.text())
            newdata = [int(d) for d in newdata]

            #Read settings
            delim = self.outmode.itemData(self.outmode.currentIndex())
            desired = 0
            inpround = self.inprnd.itemData(self.inprnd.currentIndex())

            key = newdata

            # Get initial key
            self.outkey.setText(str(model.keyScheduleRounds(key, inpround, 0, returnSubkeys=False)))

            # Get entire key schedule
            totalrndstr = ""
            roundKeys = model.getRoundKeys(key, inpround)
            for i, key in enumerate(roundKeys):
                totalrndstr += "%2d: " % i
                for bit in key:
                    totalrndstr += (str(bit) if bit is not None else '?')
                totalrndstr += "\n"

            self.keysched.setText(totalrndstr)

        except:
            pass


if __name__ == '__main__':
    # Create the Qt Application
    app = QApplication(sys.argv)
    # Create and show the form
    form = KeyScheduleDialog()
    form.show()
    # Run the main Qt loop
    sys.exit(app.exec_())


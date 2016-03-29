#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2016, NewAE Technology Inc
# All rights reserved.
##
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

from PySide.QtCore import *
from PySide.QtGui import *
import chipwhisperer.common.utils.QtFixes as QtFixes
from chipwhisperer.common.ui.ProgressBar import ProgressBar

class ProgressBarGUI(QtFixes.QDialog, ProgressBar):
    def __init__(self, parent, title = "Progress", textMask ="Initializing...", textValues = None):
        QtFixes.QDialog.__init__(self, None)
        ProgressBar.__init__(self, title, textMask, textValues)
        self.setWindowModality(Qt.WindowModal)

        self.setWindowFlags((self.windowFlags() | Qt.CustomizeWindowHint) & (not Qt.WindowContextHelpButtonHint))
        self.setWindowTitle(title)

        clayout = QHBoxLayout()
        clayout.addStretch()
        cancel = QPushButton("Abort")
        cancel.clicked.connect(self.abort)
        clayout.addWidget(cancel)

        self.pbar = QProgressBar()
        self.pbar.setMinimum(0)
        self.pbar.setMaximum(100)
        self.textLabel = QLabel(self.textMask)

        layout = QVBoxLayout()
        layout.addWidget(self.textLabel)
        layout.addWidget(self.pbar)
        layout.addLayout(clayout)
        self.setLayout(layout)
        self.show()

    def updateStatus(self, currentProgress, textValues = None):
        super(ProgressBarGUI, self).updateStatus(currentProgress, textValues)
        self.textLabel.setText(self.getText())
        self.pbar.setValue((self.currentProgress/self.maximum) * 100)
        QApplication.processEvents()

    def close(self):
        ProgressBar.close(self)
        QtFixes.QDialog.close(self)


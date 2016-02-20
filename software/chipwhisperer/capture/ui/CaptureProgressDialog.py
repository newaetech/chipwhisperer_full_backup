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

try:
    from PySide.QtCore import *
    from PySide.QtGui import *
except ImportError:
    print "ERROR: PySide is required for this program"
    sys.exit()


class CaptureProgressDialog(QDialog):

    abortCapture = Signal(bool)

    def __init__(self, parent=None, ntraces=0, nsegs=0):
        super(CaptureProgressDialog, self).__init__(parent)

        self.abort = False

        # Qt.WindowCloseButtonHint |
        # | Qt.WindowStaysOnTopHint
        self.setWindowFlags((self.windowFlags() | Qt.CustomizeWindowHint) & ~(Qt.WindowContextHelpButtonHint))
        self.setWindowTitle("Capture in Progress")
        # self.setWindowIcon(QIcon(imagePath + "attack_transp.png"))

        layout = QVBoxLayout()
        clayout = QHBoxLayout()
        cancel = QPushButton("Abort")
        cancel.clicked.connect(self.setCanceled)
        clayout.addStretch()
        clayout.addWidget(cancel)
        self.pbar = QProgressBar()

        statusInfo = QGridLayout()
        self.segNum = QLabel("Current Segment = ?")
        self.traceNum = QLabel("Current Traces = ?")
        # self.capTime = QLabel("Capture Time = ?")
        statusInfo.addWidget(self.segNum, 0, 0)
        statusInfo.addWidget(self.traceNum, 0, 1)
        # statusInfo.addWidget(self.capTime, 1, 0)

        layout.addWidget(self.pbar)
        layout.addLayout(statusInfo)
        layout.addLayout(clayout)

        self.setLayout(layout)

        self.setMinimum(0)
        self.setMaximum(ntraces)

    def traceDoneSlot(self, num, data, offset=0):
        self.incTrace()

    def startCapture(self):
        self.tnum = 0
        self.segnum = 0

    def updateLabels(self):
        self.segNum.setText("Current Segment = %d" % self.segnum)
        self.traceNum.setText("Current Trace = %d" % self.tnum)
        # self.capTime.setText("Capture Time = %d" % self.segnum)

    def incSeg(self):
        self.segnum += 1
        self.updateLabels()

    def incTrace(self):
        self.tnum += 1
        self.setValue(self.tnum)
        self.updateLabels()

    def setMinimumDuration(self, duration):
        pass

    def setMinimum(self, mv):
        self.pbar.setMinimum(mv)

    def setMaximum(self, mv):
        self.max = mv
        self.pbar.setMaximum(mv)

    def setValue(self, val):
        if self.isVisible() == False:
            self.show()

        self.pbar.setValue(val)
        if val == self.max:
            self.close()

        QApplication.processEvents()

    def setCanceled(self):
        self.abort = True
        self.abortCapture.emit(True)

    def wasCanceled(self):
        return self.abort

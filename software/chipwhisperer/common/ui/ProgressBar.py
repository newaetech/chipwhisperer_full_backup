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

from datetime import *


class ProgressBarText(object):
    def __init__(self, title = "Progress", text = None, statusMask ="Initializing...", textValues = None):
        self.title = title
        self.last = self.currentProgress = 0
        self.maximum = 100
        self.text = text
        self.statusMask = statusMask
        self.textValues = textValues
        self.startTime = datetime.now()
        self.aborted = False
        self.printAll = False
        self.setText(text)
        print self.title + ": " + self.getStatusText()

    def setText(self, text):
        if text:
            self.text = text
            print self.text

    def getText(self):
        if self.text:
            return self.text + ":"
        else:
            return ":"

    def setStatusMask(self, statusTextMask):
        self.statusMask = statusTextMask
        self.textValues = None

    def getStatusText(self):
        if self.textValues and not self.wasAborted():
            return self.statusMask % self.textValues
        return self.statusMask

    def printStatus(self):
        print self.title + (": %.1f" % ((self.currentProgress/self.maximum) * 100)) + "% (" + self.getStatusText() + ")"

    def updateStatus(self, currentProgress, textValues = None):
        self.textValues = textValues
        self.currentProgress = currentProgress
        if self.printAll or self.currentProgress == 0 or self.currentProgress == self.maximum\
                or self.currentProgress/self.maximum - self.last/self.maximum >= 0.1:
            self.last = currentProgress
            self.printStatus()

    def abort(self, message = None):
        if not message:
            message = "User request."
        self.aborted = True
        self.setStatusMask("Aborted. Reason = " + message)
        print self.title + ": " + self.getStatusText()

    def wasAborted(self):
        return self.aborted

    def close(self):
        if not self.wasAborted() and self.currentProgress != self.maximum:
            print self.title + ": Warning, closing before or after 100%: " + "currentProgress = %d and maximum = %d" % (self.currentProgress, self.maximum)

        print self.title + ": Done. Total time = " + (str(datetime.now() - self.startTime))

    def setMaximum(self, value):
        self.maximum = float(value)

    def printAll(self, value):
        self.printAll = value

try:
    from PySide.QtCore import *
    from PySide.QtGui import *

    class ProgressBarGUI(QDialog, ProgressBarText):
        def __init__(self, title = "Progress", text=None, statusMask ="Initializing...", textValues = None):

            ProgressBarText.__init__(self, title = title, text=text, statusMask= statusMask, textValues = textValues)
            QDialog.__init__(self, None)

            self.setModal(False)
            self.setWindowFlags(self.windowFlags() | Qt.WindowStaysOnTopHint)
            # self.setWindowFlags((self.windowFlags() | Qt.CustomizeWindowHint) & (not Qt.WindowContextHelpButtonHint))
            self.setWindowTitle(title)
            self.resize(200,100)

            clayout = QHBoxLayout()
            clayout.addStretch()
            cancel = QPushButton("Abort")
            cancel.clicked.connect(self.abort)
            clayout.addWidget(cancel)

            self.pbar = QProgressBar()
            self.pbar.setTextVisible(True)
            self.textLabel = QLabel(self.text)
            self.statusLabel = QLabel(self.statusMask)

            layout = QVBoxLayout()
            layout.addWidget(self.textLabel)
            layout.addWidget(self.pbar)
            layout.addWidget(self.statusLabel)
            layout.addLayout(clayout)
            self.setLayout(layout)
            self.show()
            QCoreApplication.processEvents()

        def abort(self, message = None):
            ProgressBarText.abort(self, message)
            if message:
                QMessageBox.warning(self, "Warning", "Could not complete the execution:\n\n" + self.getStatusText())


        def updateStatus(self, currentProgress, textValues = None):
            super(ProgressBarGUI, self).updateStatus(currentProgress, textValues)
            if self.getText():
                self.textLabel.setText(self.getText())
            self.statusLabel.setText(self.getStatusText())
            self.pbar.setValue((self.currentProgress/self.maximum) * 100)
            QCoreApplication.processEvents()

        def close(self):
            ProgressBarText.close(self)
            QDialog.close(self)

    ProgressBar = ProgressBarGUI

except ImportError:
    ProgressBar = ProgressBarText

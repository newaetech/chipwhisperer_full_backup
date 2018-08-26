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
import logging
import sys
from datetime import *
from ..utils import util


class ProgressBarText(object):
    def __init__(self, title="Progress", text=None, statusMask="Initializing...", textValues=None, show=True, parent=None):
        self.title = title
        self.last = self.currentProgress = 0
        self.maximum = 100
        self.text = text
        self.statusMask = statusMask
        self.textValues = textValues
        self.startTime = datetime.now()
        self.eta = 0
        self.aborted = False
        self.printAll = False
        ProgressBarText.setText(self, text)
        logging.info(self.title + ": " + self.getStatusText())
        if __debug__:
            logging.debug('Created: ' + str(self))

    def __enter__(self):
        pass

    def __exit__(self, exc_type, exc_val, exc_tb):
        if exc_type == Warning:
            self.abort(str(exc_val))

        self.close()
        return exc_type == Warning

    def __del__(self):
        if __debug__:
            logging.debug('Deleted: ' + str(self))

    def setText(self, text):
        if text:
            self.text = text
            logging.info(self.title + ": " + self.text)
        else:
            self.text = ""

    def getText(self):
        return self.text

    def setStatusMask(self, statusTextMask, textValues = None):
        self.statusMask = statusTextMask
        self.textValues = textValues

    def getStatusText(self):
        if self.textValues is not None and not self.wasAborted():
            return self.statusMask % self.textValues
        return self.statusMask

    def printStatus(self):
        if self.maximum != 0:
            logging.info(self.title + (": %.1f" % ((self.currentProgress / self.maximum) * 100)) + "% (" + self.getStatusText() + ")")
            logging.info(self.title + ": ETA " + self.getETA())

    def getETA(self):
        if self.maximum == 0 or self.currentProgress == 0:
            return "n/a"
        spent = (datetime.now() - self.startTime).total_seconds()
        eta = spent * (self.maximum - self.currentProgress) / self.currentProgress
        try:
            eta = timedelta(seconds=int(eta))
        except OverflowError:
            return "n/a"
        if eta.days > 9:
            # avoid displaying too big values
            return "n/a"
        return str(eta)

    def updateStatus(self, currentProgress, textValues=None):
        self.textValues = textValues
        self.currentProgress = currentProgress
        if self.printAll or self.currentProgress == 0 or self.currentProgress == self.maximum\
                or self.currentProgress/self.maximum - self.last/self.maximum >= 0.2:
            self.last = self.currentProgress
            self.printStatus()
        util.updateUI()

    def abort(self, message=None):
        if not message:
            message = "User request."
        self.aborted = True
        self.setStatusMask("Aborted. Reason = " + message)
        logging.warn(self.title + ": " + self.getStatusText())

    def wasAborted(self):
        return self.aborted

    def close(self):
        #assert self.currentProgress == self.maximum or self.wasAborted(), \
        if (self.currentProgress != self.maximum) and (self.wasAborted() == False):
            logging.warn('ProgressBar ' + self.title + ' closed not in 100%%: progress = %d and maximum = %d' %
                            (self.currentProgress, self.maximum))

        logging.info(self.title + ": Done. Total time = " + (str(datetime.now() - self.startTime)))

    def setMaximum(self, value):
        self.maximum = float(value)

    def setMinimum(self, value):
        assert value == 0

    def printAll(self, value):
        self.printAll = value


ProgressBar = ProgressBarText

try:
    import tqdm

    class TqdmLogger(object):
        def __init__(self, stream=sys.stderr):
            self.stream = stream

        def write(self, x):
            x = x.rstrip()
            if x:
                tqdm.tqdm.write(x, file=self.stream)


    class ProgressBarTqdm(ProgressBarText):
        def __init__(self, title="Progress", text=None, statusMask="Initializing...", textValues=None, show=True, parent=None):
            ProgressBarText.__init__(self, title=title, text=text, statusMask=statusMask, textValues=textValues)
            self.tqdm_last = 0
            self.tqdm = tqdm.tqdm(total=self.maximum, unit="traces",
                                  initial=self.currentProgress,
                                  dynamic_ncols=True)
            self.tqdm.set_description(self.title, refresh=False)
            self.tqdm.set_postfix_str(self.getStatusText(), refresh=False)

        def __enter__(self):
            ProgressBarText.__enter__(self)
            self.orig_logger = logging.root.handlers[0].stream
            logging.root.handlers[0].stream = TqdmLogger(self.orig_logger)

        def __exit__(self, exc_type, exc_val, exc_tb):
            logging.root.handlers[0].stream = self.orig_logger
            return ProgressBarText.__exit__(self, exc_type, exc_val, exc_tb)

        def printStatus(self):
            self.tqdm.refresh()

        def updateStatus(self, currentProgress, textValues=None):
            self.textValues = textValues
            self.currentProgress = currentProgress
            if self.tqdm_last != currentProgress:
                self.tqdm.set_postfix_str(self.getStatusText())
                self.tqdm.update(currentProgress - self.tqdm_last)
                self.tqdm_last = currentProgress
            util.updateUI()

        def close(self):
            self.tqdm.close()
            ProgressBarText.close(self)

        def setMaximum(self, value):
            ProgressBarText.setMaximum(self, value)
            if value != self.tqdm.total:
                self.tqdm.leave = False
                self.tqdm.close()
                self.tqdm = tqdm.tqdm(total=value, unit="traces",
                                      initial=self.currentProgress,
                                      dynamic_ncols=True)
                self.tqdm.set_description(self.title)
                self.tqdm.set_postfix_str(self.getStatusText())
                self.tqdm.refresh()

    ProgressBar = ProgressBarTqdm

except ImportError:
    ProgressBarTqdm = ProgressBarText


try:
    from PySide.QtCore import *
    from PySide.QtGui import *
    from ..utils import qt_tweaks

    class ProgressBarGUI(qt_tweaks.QDialog, ProgressBarText):
        def __init__(self, title="Progress", text=None, statusMask="Initializing...", textValues=None, show=True, parent=None):
            ProgressBarText.__init__(self, title=title, text=text, statusMask=statusMask, textValues=textValues)
            QDialog.__init__(self, parent)

            self.setAttribute(Qt.WA_DeleteOnClose)  # Close and delete all windows/QObj that has it as a parent when closing
            self.setModal(False)
            self.setWindowFlags(Qt.WindowStaysOnTopHint)
            self.setWindowTitle(title)
            self.resize((200 * self.logicalDpiX()) // 100, (100 * self.logicalDpiY()) // 100)
            self.etaLabel = QLabel("ETA: n/a")
            clayout = QHBoxLayout()
            clayout.addWidget(self.etaLabel)
            clayout.addStretch()
            cancel = QPushButton("Abort")
            cancel.clicked.connect(self.abort)
            clayout.addWidget(cancel)

            self.textLabel = QLabel(self.getText())
            self.pbar = QProgressBar()
            self.pbar.setTextVisible(True)
            self.statusLabel = QLabel(self.statusMask)

            layout = QVBoxLayout()
            layout.addWidget(self.textLabel)
            layout.addWidget(self.pbar)
            layout.addWidget(self.statusLabel)
            layout.addLayout(clayout)
            self.setLayout(layout)
            if show:
                self.show()
                self.raise_()

        def setText(self, text):
            ProgressBarText.setText(self, text)
            self.textLabel.setText(self.getText())

        def setStatusMask(self, statusTextMask, textValues=None):
            ProgressBarText.setStatusMask(self, statusTextMask, textValues)
            self.updateStatus(self.currentProgress, textValues)

        def abort(self, message=None):
            ProgressBarText.abort(self, message)
            if message:
                QMessageBox.warning(self, "Warning", "Could not complete the execution:\n\n" + self.getStatusText())

        def updateETA(self):
            eta = self.getETA().rjust(16)
            self.etaLabel.setText("ETA: %s" % eta)

        def updateStatus(self, currentProgress, textValues=None):
            ProgressBarText.updateStatus(self, currentProgress, textValues)
            self.statusLabel.setText(self.getStatusText())
            if self.maximum!=0:
                self.pbar.setValue((self.currentProgress/self.maximum) * 100)
                self.updateETA()
            util.updateUI()

        def close(self):
            ProgressBarText.close(self)
            QDialog.close(self)

    ProgressBar = ProgressBarGUI

except ImportError:
    pass

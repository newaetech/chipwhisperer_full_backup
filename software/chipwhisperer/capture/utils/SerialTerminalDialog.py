#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2014, NewAE Technology Inc
# All rights reserved.
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
from PySide.QtCore import *
from PySide.QtGui import *

class SerialTerminalDialog(QDialog):
    def __init__(self, parent):
        super(SerialTerminalDialog, self).__init__(parent)

        self.parent = parent

        self.mainLayout = QVBoxLayout()

        ### Layout for text boxes
        self.textLayouts = QGridLayout()

        self.textOut = QTextEdit()
        self.textIn = QLineEdit()
        self.textIn.returnPressed.connect(self.returnPressedIn)

        self.textInSend = QPushButton("Send")
        self.textInSend.clicked.connect(self.returnPressedIn)

        # 3 = rowSpan
        self.textLayouts.addWidget(self.textOut, 0, 0)
        self.textLayouts.addWidget(self.textIn, 1, 0)
        self.textLayouts.addWidget(self.textInSend, 1, 1)

        self.textOut.setReadOnly(True)

        ### Layout for line end & printing characters
        self.mainLayout.addLayout(self.textLayouts)

        self.settingsLineLayout = QHBoxLayout()
        self.settingsLineEnd = QComboBox()
        self.settingsLineEnd.addItem("\\n", '\n')
        self.settingsLineEnd.addItem("\\r", '\r')
        self.settingsLineEnd.addItem("\\n\\r", '\n\r')
        self.settingsLineEnd.addItem("None", '')
        self.settingsLineLayout.addWidget(QLabel("TX on Enter: "))
        self.settingsLineLayout.addWidget(self.settingsLineEnd)
        self.settingsLineLayout.addStretch()

        self.convNonAscii = QCheckBox()

        self.settingsLineLayout.addWidget(QLabel("RX: Show non-ASCII as hex"))
        self.settingsLineLayout.addWidget(self.convNonAscii)

        self.mainLayout.addLayout(self.settingsLineLayout)
        
        hsep = QFrame()
        hsep.setFrameShape(QFrame.HLine)
        self.mainLayout.addWidget(hsep)
        
        ### Layout for connection settings
        self.conLayout = QHBoxLayout()
        
        self.conPB = QPushButton("Connect")
        self.conLayout.addWidget(self.conPB)
        self.conLayout.addWidget(QLabel("Set target in main GUI"))
        self.conLayout.addStretch()
        self.conPB.clicked.connect(self.tryCon)

        self.mainLayout.addLayout(self.conLayout)

        # ## Final Setup

        self.textIn.setEnabled(False)
        self.textOut.setEnabled(False)

        self.setLayout(self.mainLayout)
        self.hide()

        self.serialGood = False
        self.timerRead = QTimer()
        self.timerRead.timeout.connect(self.checkRead)

        self.driver = None

    def addTextOut(self, data, color=Qt.black):
        
        cmain = QColor(color)

        if self.convNonAscii.isChecked():
            for c in data:
                h = ord(c)
                if h < 32 or h > 126:
                    self.textOut.moveCursor(QTextCursor.End)
                    self.textOut.setTextColor(QColor(Qt.red))
                    self.textOut.insertPlainText("%02x" % h)
                    self.textOut.moveCursor(QTextCursor.End)

                if (c == '\n') or (h >= 32 and h <= 126):
                    self.textOut.moveCursor(QTextCursor.End)
                    self.textOut.setTextColor(cmain)
                    self.textOut.insertPlainText(c)
                    self.textOut.moveCursor(QTextCursor.End)
        else:
            self.textOut.moveCursor(QTextCursor.End)
            self.textOut.setTextColor(cmain)
            self.textOut.insertPlainText (data)
            self.textOut.moveCursor(QTextCursor.End)

        self.textOut.setTextColor(QColor(Qt.black))


    def returnPressedIn(self):
        toSend = self.textIn.text()
        self.textIn.setText("")

        toSend = toSend + self.settingsLineEnd.itemData(self.settingsLineEnd.currentIndex())
        self.addTextOut(toSend, Qt.blue)
        
        if self.timerRead.isActive():
            self.driver.write(toSend)

    def checkRead(self):
        bavail = self.driver.inWaiting()
        while bavail > 0:
            s = self.driver.read(bavail)
            self.addTextOut(s)
            bavail = self.driver.inWaiting()
        
    def tryCon(self):
        self.driver = None
        self.driver = self.parent.target.driver.ser
        self.driver.con()

        self.textIn.setEnabled(True)
        self.textOut.setEnabled(True)
        
        self.timerRead.start(500)

    def tryDis(self):
        self.textIn.setEnabled(False)
        self.textOut.setEnabled(False)
        self.timerRead.stop()
        
        if self.driver is not None:
            self.driver.dis()
            self.driver = None

    def closeEvent(self, ev):
        self.tryDis()

        QDialog.closeEvent(ev)


def main():
    # Create the Qt Application
    app = QApplication(sys.argv)

    std = SerialTerminalDialog(None)
    std.show()

    app.exec_()

if __name__ == '__main__':
    main()

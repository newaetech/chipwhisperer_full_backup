#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2014, NewAE Technology Inc
# All rights reserved.
#
# Author: Colin O'Flynn <coflynn@newae.com>
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

try:
    from PySide.QtCore import *
    from PySide.QtGui import *
except ImportError:
    print "ERROR: PySide is required for this program"
    sys.exit()


class ValidationDialog(QDialog):

    def __init__(self, parent=None, onlyOkButton=False):
        super(ValidationDialog, self).__init__(parent)
        self.setWindowTitle("Results of Validation Check")
        self.setObjectName("Results of Validation Check")
        # self.setWindowFlags((self.windowFlags() | Qt.CustomizeWindowHint | Qt.WindowStaysOnTopHint) & ~(Qt.WindowContextHelpButtonHint))
        self.setupLayout(onlyOkButton)

        self.messageList = []
        self.nWarnings = 0

    def addMessage(self, level, module, note, fix, uuid):

        if (level != "info") and (level != "warn"):
            raise ValueError("Invalid level: %s" % level)

        self.messageList.append({"level":level, "module":module, "note":note, "fix":fix, "uuid":uuid})
        self.reloadTables()

    def reloadTables(self):
        for m in self.messageList:
            # TODO: Check promote/demote table to override the default settings
            if m["level"] == "warn":
                table = self.tableWarnings
                pbtext = "Demote"
                self.nWarnings += 1
            else:
                table = self.tableInfos
                pbtext = "Promote"

        newrow = table.rowCount()
        table.insertRow(newrow)

        table.setCellWidget(newrow, 0, QTextEdit(m["module"]))
        table.setCellWidget(newrow, 1, QTextEdit(m["note"]))
        table.setCellWidget(newrow, 2, QTextEdit(m["fix"]))

        # pb = QPushButton(pbtext)
        # table.setCellWidget(newrow, 3, pb)

        # table.resizeRowsToContents()
        # table.resizeColumnsToContents()


    def demoteItem(self, uuid):
        """Demote item with given UUID to Info"""
        pass

    def promoteItem(self, uuid):
        """Promote item with given UUID to Warning"""
        pass

    def clearDefaultErrorLevels(self):
        pass

    def numWarnings(self):
        return self.nWarnings

    def setupLayout(self, onlyOkButton=False):
        self.tableWarnings = QTableWidget(0, 4)
        self.tableWarnings.setSizePolicy(QSizePolicy.MinimumExpanding, QSizePolicy.MinimumExpanding)
        self.tableWarnings.setHorizontalHeaderLabels(["Module", "Note", "Fix", "Demote"])
        self.tableWarnings.horizontalHeader().setResizeMode(QHeaderView.Stretch)  # setStretchLastSection(True)

        self.tableInfos = QTableWidget(0, 4)
        self.tableInfos.setSizePolicy(QSizePolicy.MinimumExpanding, QSizePolicy.MinimumExpanding)
        self.tableInfos.setHorizontalHeaderLabels(["Module", "Note", "Fix", "Promote"])
        self.tableInfos.horizontalHeader().setResizeMode(QHeaderView.Stretch)  # .setStretchLastSection(True)

        fullTable = QWidget()
        fullLayout = QVBoxLayout()
        fullTable.setLayout(fullLayout)
        fullLayout.addWidget(QLabel("Warning Messages"))
        fullLayout.addWidget(self.tableWarnings)

        fullLayout.addWidget(QLabel("Informational Messages"))
        fullLayout.addWidget(self.tableInfos)


        pbLayouts = QHBoxLayout()
        if onlyOkButton:
            okButton = QPushButton("OK")
            pbLayouts.addWidget(okButton)
            okButton.clicked.connect(self.accept)

        else:
            okButton = QPushButton("Continue Anyway")
            abortButton = QPushButton("Abort Capture")
            pbLayouts.addWidget(okButton)
            pbLayouts.addWidget(abortButton)
            okButton.clicked.connect(self.accept)
            abortButton.clicked.connect(self.reject)

        pbLayouts.addStretch()

        clearButton = QPushButton("Default Error Levels")
        clearButton.clicked.connect(self.clearDefaultErrorLevels)
        pbLayouts.addWidget(clearButton)

        fullLayout.addLayout(pbLayouts)
        self.setLayout(fullLayout)

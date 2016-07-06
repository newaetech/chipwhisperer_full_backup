#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2013-2014, NewAE Technology Inc
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
from functools import partial

from PySide.QtGui import *
from PySide.QtCore import *
from projectdiffwidget import ProjectDiffWidget


class SaveProjectDialog(QDialog):

    def __init__(self, parent, project):
        super(SaveProjectDialog, self).__init__(parent)
        self.setAttribute(Qt.WA_DeleteOnClose)  # Close and delete all windows/QObj that has it as a parent when closing
        self.setWindowTitle("Unsaved Changes Detected")
        layout = QVBoxLayout()
        layout.addWidget(QLabel("Save unsaved changes?"))
        self.buttonBox = QDialogButtonBox(QDialogButtonBox.Yes | QDialogButtonBox.No | QDialogButtonBox.Cancel)
        layout.addWidget(self.buttonBox)

        detailedHidableWidget = ProjectDiffWidget(self, project)
        detailedHidableWidget.setContentsMargins(0, 0, 0, 0)
        detailedHidableWidget.setFixedSize(600,200)
        detailedHidableWidget.hide()

        pbShowDetails = QPushButton("Show Details")
        pbShowDetails.clicked.connect(detailedHidableWidget.show)
        detailpblayout = QHBoxLayout()
        detailpblayout.addWidget(pbShowDetails)
        detailpblayout.addStretch()
        layout.addLayout(detailpblayout)
        layout.addWidget(detailedHidableWidget)

        self.setLayout(layout)

        self.buttonBox.button(QDialogButtonBox.Yes).clicked.connect(partial(self.done, QDialogButtonBox.YesRole))
        self.buttonBox.button(QDialogButtonBox.No).clicked.connect(partial(self.done, QDialogButtonBox.NoRole))
        self.buttonBox.button(QDialogButtonBox.Cancel).clicked.connect(partial(self.done, QDialogButtonBox.RejectRole))

    @staticmethod
    def getSaveProjectDialog(parent, project):
        if not project.hasDiffs():
            return QDialogButtonBox.NoRole
        dialog = SaveProjectDialog(parent, project)
        return dialog.exec_()

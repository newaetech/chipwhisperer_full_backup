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

from PySide.QtGui import *
from projectdiffwidget import ProjectDiffWidget


class SaveProjectDialog(QDialog):

    def __init__(self, parent):
        super(SaveProjectDialog, self).__init__(parent)
        self.setWindowTitle("Unsaved Changes Detected")
        self.setModal(True)
        layout = QVBoxLayout()
        layout.addWidget(QLabel("Save unsaved changes?"))
        self.buttonBox = QDialogButtonBox(QDialogButtonBox.Yes | QDialogButtonBox.No | QDialogButtonBox.Cancel)
        layout.addWidget(self.buttonBox)

        detailedWidget = ProjectDiffWidget(self, project=self.parent().api.project())
        detailedWidget.checkDiff(updateGUI=True)
        detailedLayout = QVBoxLayout()
        detailedLayout.addWidget(detailedWidget)

        detailedHidableWidget = QWidget()
        detailedHidableWidget.setLayout(detailedLayout)
        detailedHidableWidget.hide()

        pbShowDetails = QPushButton("Show Details")
        pbShowDetails.clicked.connect(detailedHidableWidget.show)
        detailpblayout = QHBoxLayout()
        detailpblayout.addWidget(pbShowDetails)
        detailpblayout.addStretch()
        layout.addLayout(detailpblayout)
        layout.addWidget(detailedHidableWidget)

        self.setLayout(layout)

        self._lastpushed = QDialogButtonBox.RejectRole
        self.buttonBox.clicked.connect(self.setValue)
        self.buttonBox.accepted.connect(self.accept)
        self.buttonBox.rejected.connect(self.reject)

    def setValue(self, but):
        self._lastpushed = self.buttonBox.buttonRole(but)

    def value(self):
        return self._lastpushed

    @staticmethod
    def getSaveProjectDialog(parent, project):
        if not project.hasDiffs():
            return QDialogButtonBox.NoRole
        dialog = SaveProjectDialog(parent)
        dialog.exec_()
        return dialog.value()
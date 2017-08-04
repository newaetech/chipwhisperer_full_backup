#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2016, NewAE Technology Inc
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
from pyqtgraph.parametertree import ParameterTree
import chipwhisperer.common.utils.qt_tweaks as QtFixes
from chipwhisperer.common.utils.parameter import Parameterized


class CWPreferencesDialog(QtFixes.QDialog):

    def __init__(self, parent, settings):
        super(CWPreferencesDialog, self).__init__(parent)

        self.setWindowTitle("ChipWhisperer Preferences")

        self._generalTab = GeneralTab(self, settings)
        self._windowsTab = WindowTab(self)

        tabWidget = QTabWidget()
        tabWidget.addTab(self._generalTab, "General")
        tabWidget.addTab(self._windowsTab, "Window Layout")

        buttonBox = QDialogButtonBox(QDialogButtonBox.Ok)
        buttonBox.accepted.connect(self.accept)

        mainLayout = QVBoxLayout()

        mainLayout.addWidget(tabWidget)
        mainLayout.addWidget(buttonBox)

        self.setLayout(mainLayout)


class GeneralTab(QWidget, Parameterized):
    _name = "Preferences"

    def __init__(self, parent, settings):
        super(GeneralTab, self).__init__(parent)

        self.settings = settings

        self.getParams().addChildren([
            {'name': "Project Folder", 'type': "file", "filter": "dir",
             "get": lambda: self.settings.value("project-home-dir"),
             "set": lambda v: self.settings.setValue("project-home-dir", v), 'psync': False},
            {'name': "External Text Editor", 'type': "file",
             "get": lambda: self.settings.value("text-editor"),
             "set": lambda v: self.settings.setValue("text-editor", v), 'psync': False}
        ])

        parameterTree = ParameterTree()
        parameterTree.addParameters(self.getParams()._PyQtGraphParameter)

        defdirLayout = QHBoxLayout()
        defdirLayout.addWidget(parameterTree)
        mainLayout = QVBoxLayout()
        mainLayout.addLayout(defdirLayout)
        mainLayout.addStretch(1)
        self.setLayout(mainLayout)


class WindowTab(QWidget):
    def __init__(self, parent):
        super(WindowTab, self).__init__(parent)

        layout = QVBoxLayout()

        saveButton = QPushButton("Save Settings", clicked=parent.parent().saveSettings)
        saveButton.setToolTip('Save all settings')
        layout.addWidget(saveButton)

        restoreButton = QPushButton("Restore Settings", clicked=parent.parent().restoreSettings)
        restoreButton.setToolTip('Restore all settings to previous saved state')
        layout.addWidget(restoreButton)

        resetButton = QPushButton("Reset Settings and &Exit", clicked=parent.parent().reset)
        resetButton.setToolTip('Clear all settings and exit. Useful to not save the window geometry when exiting.')
        layout.addWidget(resetButton)

        layout.addStretch(1)
        self.setLayout(layout)

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

import os
from PySide.QtCore import *
from PySide.QtGui import *
import chipwhisperer.common.utils.qt_tweaks as QtFixes
from chipwhisperer.common.api.settings import Settings

class CWPreferencesDialog(QtFixes.QDialog):

    def __init__(self, parent, settings):
        super(CWPreferencesDialog, self).__init__(parent)

        self.setWindowTitle("ChipWhisperer Preferences")

        self._generalTab = GeneralTab(self, settings)
        self._windowsTab = WindowTab(self, settings)

        tabWidget = QTabWidget()
        tabWidget.addTab(self._generalTab, "General")
        tabWidget.addTab(self._windowsTab, "Window Layout")

        buttonBox = QDialogButtonBox(QDialogButtonBox.Ok | QDialogButtonBox.Cancel)

        buttonBox.accepted.connect(self.accept)
        buttonBox.rejected.connect(self.reject)

        mainLayout = QVBoxLayout()

        mainLayout.addWidget(tabWidget)
        mainLayout.addWidget(buttonBox)

        self.setLayout(mainLayout)

        #Get all settings
        self.readSettings()

    def readSettings(self):
        """Reads all settings, and set unspecified to defaults. Settings are re-saved to disk to sync defaults back."""
        self._generalTab.readSettings()
        self._windowsTab.readSettings()

        #Resync settings
        self.saveSettings()

    def saveSettings(self):
        self._generalTab.saveSettings()
        self._windowsTab.saveSettings()


class GeneralTab(QWidget):
    def __init__(self, parent, settings):
        super(GeneralTab, self).__init__(parent)

        self.settings = settings

        defdirLayout = QHBoxLayout()
        self.defaultDir = QLineEdit()
        self.defaultDir.setReadOnly(True)
        pbDefaultDir = QPushButton("Set")
        pbDefaultDir.clicked.connect(self.setDefaultDir)
        defdirLayout.addWidget(self.defaultDir)
        defdirLayout.addWidget(pbDefaultDir)
        mainLayout = QVBoxLayout()
        mainLayout.addLayout(defdirLayout)
        mainLayout.addStretch(1)
        self.setLayout(mainLayout)

    def setDefaultDir(self):
        pass

    def readSettings(self):
        """Read settings from 'settings' object"""
        homedir = self.settings.value("project-home-dir")
        self.defaultDir.setText(homedir)

    def saveSettings(self):
        """Save settings to 'settings' object"""

        homedir = self.defaultDir.text()
        if not os.path.isdir(homedir):
            reply = QMessageBox.question(self, "ChipWhisperer Preferences", "Default project directory %s does not exist - should we create it?\n\n"%(homedir) +
                                 "If you press `NO' please change default directory in `Preferences'.", QMessageBox.Yes | QMessageBox.No)
            if reply == QMessageBox.Yes:
                os.mkdir(homedir)
        self.settings.setValue("project-home-dir", homedir)



class WindowTab(QWidget):
    def __init__(self, parent, settings):
        super(WindowTab, self).__init__(parent)


    def readSettings(self):
        """Read settings from 'settings' object"""
        pass


    def saveSettings(self):
        """Save settings to 'settings' object"""
        pass
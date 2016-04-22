#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2013-2016, NewAE Technology Inc
# All rights reserved.
#
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

from datetime import *
import sys
#DO NOT REMOVE PYSIDE IMPORTS - Required for pyqtgraph to select correct version on some platforms
from PySide.QtCore import *
from PySide.QtGui import *
from chipwhisperer.common.ui.KeyScheduleDialog import KeyScheduleDialog
from chipwhisperer.common.ui.CWMainGUI import CWMainGUI
from chipwhisperer.common.ui.ProgressBar import ProgressBar
from chipwhisperer.common.api.CWCoreAPI import CWCoreAPI
from chipwhisperer.common.api.ExtendedParameter import ExtendedParameter
from chipwhisperer.common.utils import Util
import chipwhisperer.common.ui.ParameterTypesCustom  # DO NOT REMOVE!!
# from chipwhisperer.analyzer.utils.Partition import Partition, PartitionDialog
from chipwhisperer.analyzer.utils.TraceExplorerDialog import TraceExplorerDialog
from chipwhisperer.analyzer.utils.scripteditor import MainScriptEditor
from pyqtgraph.parametertree import ParameterTree
from chipwhisperer.common.api.attackscriptgen import AttackScriptGen


class CWAnalyzerGUI(CWMainGUI):
    """ Main ChipWhisperer Analyzer GUI Window Class.
    This is a front-end to the CWCoreAPI.
    """

    def __init__(self, cwapi):
        super(CWAnalyzerGUI, self).__init__(cwapi, name="ChipWhisperer" + u"\u2122" + " Analyzer " + CWCoreAPI.__version__, icon="cwiconA")

        self.plotInputEach = False
        self.traceExplorerDialog = TraceExplorerDialog(self)
        self.traceExplorerDialog.scriptsUpdated.connect(self.reloadScripts)
        self.traceExplorerDialog.runScriptFunction.connect(self.runScriptFunction)
        self.keyScheduleDialog = KeyScheduleDialog(self)
        self.utilList = [self.traceExplorerDialog]
        self.attackScriptGen = AttackScriptGen(self, self)

        self.addToolbars()
        self.addSettingsDocks()
        self.editorDocks()
        self.restoreSettings()

        self.api.signals.attackChanged.connect(self.attackChanged)
        self.api.signals.reloadAttackParamList.connect(self.reloadAttackParamList)
        self.reloadScripts()

        self.paramTree = ParameterTree()
        self.paramTree.setParameters(self.params, showTop=False)

        for k, v in self.api.results.iteritems():
            v.paramListUpdated.connect(self.reloadParamListResults)

        self.reloadParamListResults()
        self.reloadAttackParamList()
        self.reloadParamListPreprocessing()

    def attackChanged(self):
        self.reloadScripts()
        self.api.getAttack().scriptsUpdated.connect(self.reloadScripts)
        self.api.getAttack().runScriptFunction.connect(self.runScriptFunction)

    def editorControl(self, filename, filedesc, default=False, bringToFront=True):
        """This is the call-back from the script editor file list, which opens editors"""

        # Find filename
        thisEditor = None
        for e in self.scriptList:
            if e['filename'] == filename:
                thisEditor = e
                break

        if thisEditor is None:
            thisEditor = {'widget':MainScriptEditor(parent=self, filename=filename)}
            thisEditor['filename'] = filename
            thisEditor['dockname'] = filedesc
            self.scriptList.append(thisEditor)

        # Update all docks if required
        thisEditor['dockname'] = filedesc
        self.editorDocks()

        if bringToFront:
            thisEditor['dock'].show()
            thisEditor['dock'].raise_()

        if default:
            # Set as default for attacks etc
            self.defaultEditor = thisEditor

    def addToolbars(self):
        """Add toolbars and menus to the main window"""

        attack = QAction(QIcon(':/images/attack.png'), 'Start Attack', self)
        attack.triggered.connect(self.doAttack)

        self.AttackToolbar = self.addToolBar('Attack Tools')
        self.AttackToolbar.setObjectName('Attack Toolbar')
        self.AttackToolbar.addAction(attack)
        self.AttackToolbar.show()

        # Add utilities
        self.UtilitiesTraceExplorer = QAction('Trace Explorer', self,
                               statusTip='Get information on traces',
                               triggered=self.traceExplorerDialog.show)

        self.UtilitiesAESSchedule = QAction('AES Key Schedule', self,
                               statusTip='Show AES Key Schedule calculator',
                               triggered=self.keyScheduleDialog.show)

        # self.UtilitiesPartition = QAction('Generate Partitions', self,
        #                       statusTip='Generate Partitions for Template Attacks',
        #                       triggered=self.PartitionDialog.exec_)

        self.toolMenu.addSeparator()
        self.toolMenu.addAction(self.UtilitiesTraceExplorer)
        self.toolMenu.addAction(self.UtilitiesAESSchedule)
        # self.toolMenu.addAction(self.UtilitiesPartition)
        self.toolMenu.addSeparator()

    def reloadAttackParamList(self, list=None):
        """Reloads parameter tree in GUI when attack changes"""

        ExtendedParameter.reloadParams(self.api.getAttack().paramList(), self.attackParamTree, help_window=self.helpbrowser.helpwnd)

    def reloadParamListResults(self):
        """Reload parameter tree for results settings, ensuring GUI matches loaded modules."""

        plist = []
        for k, v in self.api.results.iteritems():
            if v:
                plist.extend(v.paramList())

        ExtendedParameter.reloadParams(plist, self.resultsParamTree, help_window=self.helpbrowser.helpwnd)

    def reloadParamList(self, lst=None):
        """Reload parameter trees in a given list, ensuring GUI matches loaded modules."""

        ExtendedParameter.reloadParams(self.paramList(), self.paramTree, help_window=self.helpbrowser.helpwnd)

    def getTraceSource(self):
        raise self.setupScriptModule().ppOutput

    def doAttack(self):
        """Called when the 'Do Attack' button is pressed, or can be called via API to cause attack to run"""

        if self.api.project().traceManager().numTraces() == 0:
            ret = QMessageBox.question(self, "Attack Error", "No traces enabled in project.\nOpen Trace Manager?", QMessageBox.Yes | QMessageBox.No)
            if ret == QMessageBox.Yes:
                self.traceManagerDialog.show()
            return

        self.api.doAttack(self.setupScriptModule(), ProgressBar("Analysis in Progress", "Analyzing:"))

    def addSettingsDocks(self):
        self.settingsGeneralDock = self.addSettings(self.api.generalParamTree, "General Settings")
        self.settingsPreprocessingDock = self.addSettings(self.preprocessingParamTree, "Preprocessing")
        self.settingsAttackDock = self.addSettings(self.attackParamTree, "Attack")
        self.settingsPostProcessingDock = self.addSettings(self.postprocessingParamTree, "Postprocessing")
        self.settingsResultsDock = self.addSettings(self.resultsParamTree, "Results")

        self.tabifyDocks([self.settingsNormalDock, self.settingsPreprocessingDock, self.settingsAttackDock,
                          self.settingsPostProcessingDock, self.settingsResultsDock])

    def setAttack(self, module):
        self.api.setAttack(module)
        self.api.getAttack().scriptsUpdated.connect(self.reloadScripts)


def makeApplication():
    # Create the Qt Application
    app = QApplication(sys.argv)
    app.setOrganizationName("ChipWhisperer")
    app.setApplicationName("Analyzer V2")
    return app

def main():
    # Create the Qt Application
    app = makeApplication()
    # Create and show the GUI
    window = CWAnalyzerGUI(CWCoreAPI())
    window.show()

    # Run the main Qt loop
    sys.exit(app.exec_())

if __name__ == '__main__':
    main()

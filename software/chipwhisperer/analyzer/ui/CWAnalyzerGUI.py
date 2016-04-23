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

import sys
#DO NOT REMOVE PYSIDE IMPORTS - Required for pyqtgraph to select correct version on some platforms
from PySide.QtCore import *
from PySide.QtGui import *
from chipwhisperer.common.ui.KeyScheduleDialog import KeyScheduleDialog
from chipwhisperer.common.ui.CWMainGUI import CWMainGUI
from chipwhisperer.common.ui.ProgressBar import ProgressBar
from chipwhisperer.common.api.CWCoreAPI import CWCoreAPI
from chipwhisperer.common.api.ExtendedParameter import ExtendedParameter
import chipwhisperer.common.ui.ParameterTypesCustom  # DO NOT REMOVE!!
from chipwhisperer.analyzer.utils.TraceExplorerDialog import TraceExplorerDialog
from chipwhisperer.analyzer.utils.scripteditor import MainScriptEditor
from pyqtgraph.parametertree import ParameterTree
from chipwhisperer.common.api.attackscriptgen import AttackScriptGen


class CWAnalyzerGUI(CWMainGUI):
    """ Main ChipWhisperer Analyzer GUI Window Class.
    This is a front-end to the CWCoreAPI.
    """

    def __init__(self, api):
        super(CWAnalyzerGUI, self).__init__(api, name="ChipWhisperer" + u"\u2122" + " Analyzer " + CWCoreAPI.__version__, icon="cwiconA")
        self.api.signals.attackChanged.connect(self.attackChanged)
        self.attackScriptGen.reloadScripts()

    def loadExtraModules(self):
        self.keyScheduleDialog = KeyScheduleDialog(self)
        self.attackScriptGen = AttackScriptGen(None, self)

        self.traceExplorerDialog = TraceExplorerDialog(self)
        self.traceExplorerDialog.scriptsUpdated.connect(self.attackScriptGen.reloadScripts)
        self.traceExplorerDialog.runScriptFunction.connect(self.attackScriptGen.runScriptFunction)
        self.utilList = [self.traceExplorerDialog]

    def attackChanged(self):
        self.attackScriptGen.reloadScripts()
        self.api.getAttack().scriptsUpdated.connect(self.attackScriptGen.reloadScripts)
        self.api.getAttack().runScriptFunction.connect(self.attackScriptGen.runScriptFunction)

    def addToolbarItems(self, toolbar):
        attack = QAction(QIcon(':/images/attack.png'), 'Start Attack', self)
        attack.triggered.connect(self.doAttack)
        toolbar.addAction(attack)

    def addToolMenuItems(self):
        self.traceExplorerAct = QAction('Trace Explorer', self, statusTip='Get information on traces',
                                        triggered=self.traceExplorerDialog.show)
        self.toolMenu.addAction(self.traceExplorerAct)
        self.aesScheduleAct = QAction('AES Key Schedule', self, statusTip='Show AES Key Schedule calculator',
                                      triggered=self.keyScheduleDialog.show)
        self.toolMenu.addAction(self.aesScheduleAct)

    def getTraceSource(self):
        raise self.setupScriptModule().ppOutput

    def doAttack(self):
        """Called when the 'Do Attack' button is pressed"""
        if self.api.project().traceManager().numTraces() == 0:
            ret = QMessageBox.question(self, "Attack Error", "No traces enabled in project.\nOpen Trace Manager?", QMessageBox.Yes | QMessageBox.No)
            if ret == QMessageBox.Yes:
                self.traceManagerDialog.show()
            return

        self.api.doAttack(self.attackScriptGen.setupScriptModule(), ProgressBar("Analysis in Progress", "Analyzing:"))

    def addSettingsDocks(self):
        self.api.addActiveParams(self.attackScriptGen)
        self.settingsGeneralDock = self.addSettings(self.api.generalParamTree, "General Settings")
        self.settingsScriptDock = self.addSettings(self.attackScriptGen.scriptParamTree, "Script")
        self.settingsPreprocessingDock = self.addSettings(self.attackScriptGen.preprocessingParamTree, "Preprocessing")
        self.settingsAttackDock = self.addSettings(self.attackScriptGen.attackParamTree, "Attack")
        self.settingsPostProcessingDock = self.addSettings(self.attackScriptGen.postprocessingParamTree, "Postprocessing")
        self.settingsResultsDock = self.addSettings(self.api.resultsParamTree, "Results")

        self.tabifyDocks([self.settingsScriptDock, self.settingsGeneralDock, self.settingsPreprocessingDock, self.settingsAttackDock,
                          self.settingsPostProcessingDock, self.settingsResultsDock])
        self.attackScriptGen.editorDocks()

    def setAttack(self, module):
        self.api.setAttack(module)
        self.api.getAttack().scriptsUpdated.connect(self.reloadScripts)

    def tracesChanged(self):
        """Traces changed due to loading new project or adjustment in trace manager,
        so adjust limits displayed and re-plot the new input trace"""

        self.api.getAttack().setTraceLimits(traces, points)
        self.setTraceLimits(self.api.project().traceManager().numTraces(), self.api.project().traceManager().numPoints())
        self.plotInputTrace()
        self.reloadScripts()

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

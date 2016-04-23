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
#=================================================

import sys
from chipwhisperer.common.ui.CWMainGUI import CWMainGUI
from chipwhisperer.common.ui.ValidationDialog import ValidationDialog
from chipwhisperer.common.ui.ProgressBar import *
from chipwhisperer.common.api.CWCoreAPI import CWCoreAPI
from chipwhisperer.capture.utils.GlitchExplorerDialog import GlitchExplorerDialog as GlitchExplorerDialog
from chipwhisperer.capture.utils.SerialTerminalDialog import SerialTerminalDialog as SerialTerminalDialog


class CWCaptureGUI(CWMainGUI):
    def __init__(self, api):
        super(CWCaptureGUI, self).__init__(api, name=("ChipWhisperer" + u"\u2122" + " Capture " + CWCoreAPI.__version__), icon="cwiconC")

        # Observers (callback methods)
        self.api.signals.newInputData.connect(self.newTargetData)
        self.api.signals.connectStatus.connect(self.connectStatusChanged)
        self.api.signals.traceDone.connect(self.glitchMonitor.traceDone)
        self.api.signals.campaignStart.connect(self.glitchMonitor.campaignStart)
        self.api.signals.campaignDone.connect(self.glitchMonitor.campaignDone)
        self.api.signals.newScopeData.connect(self.newScopeData)

    def loadExtraModules(self):
        self.serialTerminal = SerialTerminalDialog(self, self.api)
        self.glitchMonitor = GlitchExplorerDialog(self)

    def addSettingsDocks(self):
        self.settingsGeneralDock = self.addSettings(self.api.generalParamTree, "General Settings")
        self.settingsResultsDock = self.addSettings(self.api.resultsParamTree, "Results")
        self.settingsScopeDock = self.addSettings(self.api.scopeParamTree, "Scope Settings")
        self.settingsTargetDock = self.addSettings(self.api.targetParamTree, "Target Settings")
        self.settingsTraceDock = self.addSettings(self.api.traceParamTree, "Trace Settings")
        self.settingsAuxDock = self.addSettings(self.api.auxParamTree, "Aux Settings")

        self.tabifyDocks([self.settingsGeneralDock, self.settingsResultsDock, self.settingsScopeDock, self.settingsTargetDock,
                          self.settingsTraceDock, self.settingsAuxDock])

    def connectStatusChanged(self):
        """Callback when scope/target status change"""
        if self.api.getScope() and self.api.getScope().getStatus():
            self.scopeStatus.setDefaultAction(self.scopeStatusActionCon)
        else:
            self.scopeStatus.setDefaultAction(self.scopeStatusActionDis)

        if self.api.getTarget() and self.api.getTarget().getStatus():
            self.targetStatus.setDefaultAction(self.targetStatusActionCon)
        else:
            self.targetStatus.setDefaultAction(self.targetStatusActionDis)

        if self.api.getScope() and self.api.getTarget() and \
                (self.scopeStatus.defaultAction() == self.scopeStatusActionCon or
                 self.targetStatus.defaultAction() == self.targetStatusActionCon):
            self.captureStatus.setDefaultAction(self.captureStatusActionCon)
        else:
            self.captureStatus.setDefaultAction(self.captureStatusActionDis)

    def newTargetData(self, data):
        self.glitchMonitor.addResponse(data)

    def addToolMenuItems(self):
        self.terminalAct = QAction('Open Terminal', self, statusTip='Open Simple Serial Terminal',
                                   triggered=self.serialTerminal.show)

        self.toolMenu.addAction(self.terminalAct)
        self.glitchMonitorAct = QAction('Open Glitch Monitor', self, statusTip='Open Glitch Monitor Table',
                                        triggered=self.glitchMonitor.show)
        self.toolMenu.addAction(self.glitchMonitorAct)

    def newScopeData(self, data=None, offset=0):
        self.api.getGraphWidget().passTrace(data, offset)

    def addToolbarItems(self, toolbar):
        # Capture
        self.capture1Act = QAction(QIcon(':/images/play1.png'), 'Capture 1', self)
        self.capture1Act.triggered.connect(lambda: self.doCapture(self.capture1))
        self.captureMAct = QAction(QIcon(':/images/playM.png'), 'Capture Trace Set', self)
        self.captureMAct.triggered.connect(lambda: self.doCapture(self.captureM))

        self.captureStatus = QToolButton()
        self.captureStatusActionDis = QAction(QIcon(':/images/status_disconnected.png'), 'Master: Disconnected', self)
        self.captureStatusActionDis.triggered.connect(self.doConDis)
        self.captureStatusActionCon = QAction(QIcon(':/images/status_connected.png'), 'Master: Connected', self)
        self.captureStatusActionCon.triggered.connect(self.doConDis)
        self.captureStatus.setDefaultAction(self.captureStatusActionDis)

        # Scope
        self.scopeStatus = QToolButton()
        self.scopeStatusActionDis = QAction(QIcon(':/images/status_disconnected.png'), 'Scope: Disconnected', self)
        self.scopeStatusActionDis.triggered.connect(self.doConDisScope)
        self.scopeStatusActionCon = QAction(QIcon(':/images/status_connected.png'), 'Scope: Connected', self)
        self.scopeStatusActionCon.triggered.connect(self.doConDisScope)
        self.scopeStatus.setDefaultAction(self.scopeStatusActionDis)

        # Target
        self.targetStatus = QToolButton()
        self.targetStatusActionDis = QAction(QIcon(':/images/status_disconnected.png'), 'Target: Disconnected', self)
        self.targetStatusActionDis.triggered.connect(self.doConDisTarget)
        self.targetStatusActionCon = QAction(QIcon(':/images/status_connected.png'), 'Target: Connected', self)
        self.targetStatusActionCon.triggered.connect(self.doConDisTarget)
        self.targetStatus.setDefaultAction(self.targetStatusActionDis)

        # Other Stuff
        self.miscValidateAction = QAction(QIcon(':/images/validate.png'), 'Validate', self)
        self.miscValidateAction.triggered.connect(self.validateSettings)

        toolbar.addAction(self.capture1Act)
        toolbar.addAction(self.captureMAct)
        toolbar.addSeparator()
        toolbar.addWidget(QLabel('Master:'))
        toolbar.addWidget(self.captureStatus)
        toolbar.addWidget(QLabel('Scope:'))
        toolbar.addWidget(self.scopeStatus)
        toolbar.addWidget(QLabel('Target:'))
        toolbar.addWidget(self.targetStatus)
        toolbar.addSeparator()
        toolbar.addAction(self.miscValidateAction)

    def doConDisScope(self):
        if self.scopeStatus.defaultAction() == self.scopeStatusActionDis:
            if self.api.connectScope():
                self.updateStatusBar("Scope Connected")
        else:
            self.api.disconnectScope()
            self.updateStatusBar("Scope Disconnected")

    def doConDisTarget(self):
        if self.targetStatus.defaultAction() == self.targetStatusActionDis:
            if self.api.connectTarget():
                self.updateStatusBar("Target Connected")
        else:
            self.api.disconnectTarget()
            self.updateStatusBar("Target Disconnected")

    def doConDis(self):
        """Toggle connect button pushed (master): attempts both target & scope connection"""
        if self.captureStatus.defaultAction() == self.captureStatusActionDis:
            if self.api.connect():
                self.updateStatusBar("Target and Scope Connected")
        else:
            if self.api.disconnect():
                self.updateStatusBar("Target and Scope Disconnected")

    def validateSettings(self, warnOnly=False):
        # Validate settings from all modules before starting multi-api
        vw = ValidationDialog(onlyOkButton=not warnOnly)

        ret = []
        try:
            ret.extend(self.api.getTarget().validateSettings())
        except AttributeError:
            ret.append(("info", "Target Module", "Target has no validateSettings()", "Internal Error", "73b08424-3865-4274-8fd7-dd213ede2c46"))
        except Exception as e:
            ret.append(("warn", "General Settings", e.message, "Specify Target Module", "2351e3b0-e5fe-11e3-ac10-0800200c9a66"))

        try:
            ret.extend(self.api.getScope().validateSettings())
        except AttributeError:
            ret.append(("info", "Scope Module", "Scope has no validateSettings()", "Internal Error", "d19be31d-ad1a-4533-80dc-9423dfa92753"))
        except Exception as e:
            ret.append(("warn", "General Settings", e.message, "Specify Scope Module", "325de1cf-0d47-4ed8-8e9f-77d8f9cf2d5f"))

        try:
            ret.extend(self.api.getTraceClass()().validateSettings())
        except AttributeError:
            ret.append(("info", "Writer Module", "Writer has no validateSettings()", "Internal Error", "d7b3a9a1-83f0-4b4d-92b9-3d7dcf6304ae"))
        except Exception as e:
            ret.append(("warn", "General Settings", e.message, "Specify Trace Writer Module", "57a3924d-3794-4ca6-9693-46a7b5243727"))

        tracesPerRun = self.api.tracesPerSet()
        if tracesPerRun > 10E3:
            ret.append(("warn", "General Settings", "Very Long Capture (%d traces)" % tracesPerRun, "Set 'Capture Segments' to '%d'" % (self.numTraces / 10E3), "1432bf95-9026-4d8c-b15d-9e49147840eb"))

        for i in ret:
            vw.addMessage(*i)

        if self.api.project().isUntitled():
             vw.addMessage("info", "File Menu", "Project not saved, using default-data-dir", "Save project file before api", "8c9101ff-7553-4686-875d-b6a8a3b1d2ce")

        if vw.numWarnings() > 0 or warnOnly == False:
            return vw.exec_()
        else:
            return True

    def doCapture(self, callback):
        try:
            self.capture1Act.setEnabled(False)
            self.captureMAct.setEnabled(False)
            if callback():
                self.updateStatusBar("Capture completed")
        finally:
            self.capture1Act.setEnabled(True)
            self.captureMAct.setEnabled(True)
            self.capture1Act.setChecked(False)
            self.captureMAct.setChecked(False)

    def capture1(self):
        self.api.capture1()

    def captureM(self):
        self.api.captureM(ProgressBar("Capture in Progress", "Capturing:"))


def makeApplication():
    # Create the Qt Application
    app = QApplication(sys.argv)
    app.setOrganizationName(CWCoreAPI.__organization__)
    app.setApplicationName(CWCoreAPI.__name__ + " - Capture ")
    return app


def main():
    # Create the Qt Application
    app = makeApplication()
    # Create and show the GUI
    window = CWCaptureGUI(CWCoreAPI())
    window.show()

    # Run the main Qt loop
    sys.exit(app.exec_())

if __name__ == '__main__':
    main()

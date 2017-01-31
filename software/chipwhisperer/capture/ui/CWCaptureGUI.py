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

from chipwhisperer.capture.ui.EncryptionStatusMonitor import EncryptionStatusMonitor
from chipwhisperer.capture.ui.GlitchExplorerDialog import GlitchExplorerDialog as GlitchExplorerDialog
from chipwhisperer.capture.utils.SerialTerminalDialog import SerialTerminalDialog as SerialTerminalDialog
from chipwhisperer.common.api.CWCoreAPI import CWCoreAPI
from chipwhisperer.common.results.base import ResultsBase
from chipwhisperer.common.ui.CWMainGUI import CWMainGUI, makeApplication
from chipwhisperer.common.ui.ProgressBar import *
from chipwhisperer.common.ui.ValidationDialog import ValidationDialog
from chipwhisperer.common.utils import pluginmanager
from chipwhisperer.common.utils.parameter import Parameter
from chipwhisperer.common.utils.tracesource import ActiveTraceObserver


class CWCaptureGUI(CWMainGUI):
    def __init__(self, api):
        super(CWCaptureGUI, self).__init__(api, name=("ChipWhisperer" + u"\u2122" + " Capture " + CWCoreAPI.__version__), icon="cwiconC")
        self.addExampleScripts(pluginmanager.getPluginsInDictFromPackage("chipwhisperer.capture.scripts", False, False, self))

        # Observers (callback methods)
        self.api.sigNewInputData.connect(self.newTargetData)
        self.api.sigConnectStatus.connect(self.connectStatusChanged)
        self.api.sigTraceDone.connect(self.glitchMonitor.traceDone)
        self.api.sigCampaignStart.connect(self.glitchMonitor.campaignStart)
        self.api.sigCampaignDone.connect(self.glitchMonitor.campaignDone)

    def loadExtraModules(self):
        self.serialTerminal = SerialTerminalDialog(self, self.api)
        self.glitchMonitor = GlitchExplorerDialog(self)

    def addSettingsDocks(self):
        self.settingsGeneralDock = self.addSettings(self.api.params)
        self.settingsResultsDock = self.addSettings(ResultsBase.getClassParameter())
        self.settingsScopeDock = self.addSettings(self.api.scopeParam)
        self.settingsTargetDock = self.addSettings(self.api.targetParam)
        # self.settingsTraceDock = self.addSettings(self.api.traceParam)
        self.settingsAuxDock = self.addSettings(self.api.auxParam)

        # Load all ActiveTraceObservers
        self.windowMenu.addSeparator()
        for k, v in ResultsBase.getClasses().iteritems():
            if issubclass(v, ActiveTraceObserver):
                ResultsBase.createNew(k)

        self.tabifyDocks([self.settingsGeneralDock, self.settingsResultsDock, self.settingsScopeDock, self.settingsTargetDock,
                          # self.settingsTraceDock,
                          self.settingsAuxDock])

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
        self.terminalAct = QAction('Terminal', self, statusTip='Open Simple Serial Terminal',
                                   triggered=self.serialTerminal.show)

        self.toolMenu.addAction(self.terminalAct)
        self.glitchMonitorAct = QAction('Glitch Monitor', self, statusTip='Open Glitch Monitor Table',
                                        triggered=self.glitchMonitor.show)
        self.toolMenu.addAction(self.glitchMonitorAct)

        self.encryptionStatusMonitor = EncryptionStatusMonitor(self)
        self.api.sigNewTextResponse.connect(self.encryptionStatusMonitor.newData)
        self.encryptionStatusMonitorAct = QAction('Encryption Status Monitor', self, statusTip='Encryption Status Monitor',
                                        triggered=self.encryptionStatusMonitor.show)
        self.toolMenu.addAction(self.encryptionStatusMonitorAct)

    def addToolbarItems(self, toolbar):
        # Capture
        self.capture1Act = QAction(QIcon(':/images/play1.png'), 'Capture 1', self, triggered=lambda: self.doCapture(self.api.capture1))
        self.captureMAct = QAction(QIcon(':/images/playM.png'), 'Capture M', self, triggered=lambda: self.doCapture(self.captureM))
        self.stopCaptureMAct = QAction(QIcon(':/images/stopM.png'), 'Stop Capture', self, triggered=lambda: self.capturingProgressBar.abort(), enabled=False)

        # Master
        self.captureStatus = QToolButton()
        self.captureStatusActionDis = QAction(QIcon(':/images/status_disconnected.png'), 'Master: Disconnected', self, triggered=self.doConDis)
        self.captureStatusActionCon = QAction(QIcon(':/images/status_connected.png'), 'Master: Connected', self, triggered=self.doConDis)
        self.captureStatus.setDefaultAction(self.captureStatusActionDis)

        # Scope
        self.scopeStatus = QToolButton()
        self.scopeStatusActionDis = QAction(QIcon(':/images/status_disconnected.png'), 'Scope: Disconnected', self, triggered=self.doConDisScope)
        self.scopeStatusActionCon = QAction(QIcon(':/images/status_connected.png'), 'Scope: Connected', self, triggered=self.doConDisScope)
        self.scopeStatus.setDefaultAction(self.scopeStatusActionDis)

        # Target
        self.targetStatus = QToolButton()
        self.targetStatusActionDis = QAction(QIcon(':/images/status_disconnected.png'), 'Target: Disconnected', self, triggered=self.doConDisTarget)
        self.targetStatusActionCon = QAction(QIcon(':/images/status_connected.png'), 'Target: Connected', self, triggered=self.doConDisTarget)
        self.targetStatus.setDefaultAction(self.targetStatusActionDis)

        toolbar.addAction(self.capture1Act)
        toolbar.addAction(self.captureMAct)
        toolbar.addAction(self.stopCaptureMAct)
        toolbar.addSeparator()
        toolbar.addWidget(QLabel('Master:'))
        toolbar.addWidget(self.captureStatus)
        toolbar.addWidget(QLabel('Scope:'))
        toolbar.addWidget(self.scopeStatus)
        toolbar.addWidget(QLabel('Target:'))
        toolbar.addWidget(self.targetStatus)
        toolbar.addSeparator()
        toolbar.addAction(QAction(QIcon(':/images/validate.png'), 'Validate', self, triggered=self.validateSettings))

    def doConDisScope(self):
        if self.scopeStatus.defaultAction() == self.scopeStatusActionDis:
            if self.api.connectScope():
                logging.info("Scope Connected")
        else:
            self.api.disconnectScope()
            logging.info("Scope Disconnected")

    def doConDisTarget(self):
        if self.targetStatus.defaultAction() == self.targetStatusActionDis:
            if self.api.connectTarget():
                logging.info("Target Connected")
        else:
            self.api.disconnectTarget()
            logging.info("Target Disconnected")

    def doConDis(self):
        """Toggle connect button pushed (master): attempts both target & scope connection"""
        if self.captureStatus.defaultAction() == self.captureStatusActionDis:
            if self.api.connect():
                logging.info("Target and Scope Connected")
        else:
            if self.api.disconnect():
                logging.info("Target and Scope Disconnected")

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
            vw.addMessage("info", "File Menu", "Project not saved, using default-data-dir", "Save project file before capture.", "8c9101ff-7553-4686-875d-b6a8a3b1d2ce")

        if vw.numWarnings() > 0 or warnOnly is False:
            return vw.exec_()
        else:
            return True

    def doCapture(self, callback):
        self.clearFocus()
        try:
            self.capture1Act.setEnabled(False)
            self.captureMAct.setEnabled(False)
            util.updateUI()  # Hide the buttons right away
            if callback():
                logging.info("Capture completed.")
        finally:
            self.capture1Act.setEnabled(True)
            self.captureMAct.setEnabled(True)
            self.capture1Act.setChecked(False)
            self.captureMAct.setChecked(False)

    def captureM(self):
        try:
            self.stopCaptureMAct.setEnabled(True)
            self.capturingProgressBar = ProgressBar("Capture in Progress", "Capturing:")
            ret = self.api.captureM(self.capturingProgressBar)
        finally:
            self.stopCaptureMAct.setEnabled(False)
        return ret


def main():
    # Create the Qt Application
    app = makeApplication("Capture")
    Parameter.usePyQtGraph = True

    # Create and show the GUI
    window = CWCaptureGUI(CWCoreAPI())
    window.show()

    # Run the main Qt loop
    app.exec_()


if __name__ == '__main__':
    main()

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

import os.path
import sys
from functools import partial
from chipwhisperer.capture.api.ExtendedParameter import ExtendedParameter
from chipwhisperer.capture.api.captureapi import CWCaptureAPI
from chipwhisperer.capture.ui.CaptureProgressDialog import CaptureProgressDialog
from chipwhisperer.capture.ui.EncryptionStatusMonitor import EncryptionStatusMonitor
from chipwhisperer.capture.utils.GlitchExplorerDialog import GlitchExplorerDialog as GlitchExplorerDialog
from chipwhisperer.capture.utils.SerialTerminalDialog import SerialTerminalDialog as SerialTerminalDialog
from chipwhisperer.common.api.ProjectFormat import ProjectFormat
from chipwhisperer.common.ui.MainChip import MainChip
from chipwhisperer.common.ui.ValidationDialog import ValidationDialog
from chipwhisperer.common.utils import util
from PySide.QtGui import *
from pyqtgraph.parametertree import Parameter, ParameterTree

__name__ = "ChipWhisperer - Capture"
__organization__ = "NewAE"
__version__ = "V3"
__author__ = "Colin O'Flynn"


class ChipWhispererCapture(MainChip):
    def __init__(self, rootdir="."):
        super(ChipWhispererCapture, self).__init__(name=("ChipWhisperer" + u"\u2122" + " Capture " + __version__), icon="cwiconC")
        self.console = self.addConsole()

        # This is a hack for paths hardcoded into the application. todo: fix this properly.
        util.globalSettings.setValue("cwcapture-starting-root", rootdir)
        self.esm = EncryptionStatusMonitor(self)
        self.serialTerminal = SerialTerminalDialog(self)
        self.glitchMonitor = GlitchExplorerDialog(self, showScriptParameter=self.showScriptParameter)
        self.paramTrees.append(self.glitchMonitor.paramTree)

        self.manager = CWCaptureAPI()
        self.manager.setupParameters(rootdir, self.showScriptParameter)
        self.manager.setTraceManager(self.manageTraces)
        self.newProject()

        self.addExampleScripts(CWCaptureAPI.getExampleScripts(rootdir + "/scripts"))
        self.addToolbar()
        self.addWaveforms()
        self.addToolMenu()
        self.addSettingsDocks()
        self.dockifySettings()
        self.reloadParamList()
        self.readSettings()

        # Observers (callback methods)
        self.manager.signals.paramListUpdated.connect(self.reloadTargetParamList)
        self.manager.signals.newInputData.connect(self.newTargetData)
        self.manager.signals.connectStatus.connect(self.targetStatusChanged)
        self.manager.signals.newTextResponse.connect(self.esm.newData)
        self.manager.signals.traceDone.connect(self.glitchMonitor.traceDone)
        self.manager.signals.campaignStart.connect(self.glitchMonitor.campaignStart)
        self.manager.signals.campaignDone.connect(self.glitchMonitor.campaignDone)
        self.manager.signals.connectStatus.connect(self.targetStatusChanged)
        self.manager.signals.scopeChanged.connect(self.scopeChanged)
        self.manager.signals.targetChanged.connect(self.targetChanged)
        self.manager.signals.traceChanged.connect(self.traceChanged)
        self.manager.signals.auxChanged.connect(self.auxChanged)
        self.manager.signals.acqPatternChanged.connect(self.reloadParamList)

    def scopeChanged(self):
        if self.manager.hasScope():
            self.manager.getScope().paramListUpdated.connect(self.reloadScopeParamList)
            self.manager.getScope().dataUpdated.connect(self.newScopeData)
            self.manager.getScope().connectStatus.connect(self.scopeStatusChanged)
            self.reloadScopeParamList()

    def targetChanged(self):
        if self.manager.hasTarget():
            self.manager.getTarget().connectStatus.connect(self.targetStatusChanged)
            self.reloadTargetParamList()

    def traceChanged(self):
        self.reloadTraceParamList()

    def auxChanged(self):
        self.reloadAuxParamList()

    def masterStatusChanged(self):
        # Deals with multiple
        if self.scopeStatus.defaultAction() == self.scopeStatusActionCon or self.targetStatus.defaultAction() == self.targetStatusActionCon:
            self.captureStatus.setDefaultAction(self.captureStatusActionCon)
        else:
            self.captureStatus.setDefaultAction(self.captureStatusActionDis)

    def scopeStatusChanged(self):
        """Callback when scope connection successful"""
        if self.manager.getScope().getStatus():
            self.scopeStatus.setDefaultAction(self.scopeStatusActionCon)
        else:
            self.scopeStatus.setDefaultAction(self.scopeStatusActionDis)

        self.masterStatusChanged()

    def targetStatusChanged(self):
        """Callback when target connection successful"""
        if self.manager.getTarget().getStatus():
            self.targetStatus.setDefaultAction(self.targetStatusActionCon)
        else:
            self.targetStatus.setDefaultAction(self.targetStatusActionDis)

        self.masterStatusChanged()

    def newTargetData(self, data):
        self.glitchMonitor.addResponse(data)

    def addExampleScripts(self, scripts):
        self.exampleScriptAct = QAction('&Example Scripts', self, statusTip='Predefined Scripts')
        self.projectMenu.addSeparator()
        self.projectMenu.addAction(self.exampleScriptAct)
        subMenu = QMenu("Submenu", self)

        for script in scripts:
            subMenu.addAction(QAction(script.name(), self, statusTip=script.tip(), triggered=partial(self.runScript, script)))

        self.exampleScriptAct.setMenu(subMenu)

    def runScript(self, mod):
        self.updateStatusBar("Running Script: %s" % mod.name())
        m = mod.userScript(self)
        m.run()
        self.updateStatusBar("Finished Script: %s" % mod.name())

    def addToolMenu(self):
        self.TerminalAct = QAction('Open Terminal', self,
                                   statusTip='Open Simple Serial Terminal',
                                   triggered=self.serialTerminal.show)

        self.toolMenu.addAction(self.TerminalAct)
        self.GlitchMonitorAct = QAction('Open Glitch Monitor', self,
                                     statusTip='Open Glitch Monitor Table',
                                     triggered=self.glitchMonitor.show)
        self.toolMenu.addAction(self.GlitchMonitorAct)

        # Keep track of add-ins
        self._scopeToolMenuItems = []


    def addWaveforms(self):
        self.waveformDock = self.addTraceWidget()

        #TODO: FIX THIS HACK
        #Should be something in ScopeInterface class maybe
        self.waveformDock.setDefaultYRange(-0.5, 0.5)
        self.waveformDock.YDefault()

    def addSettingsDocks(self):
        self.setupParametersTree()
        self.settingsGeneralDock = self.addSettings(self.generalParamTree, "General Settings")
        self.settingsScopeDock = self.addSettings(self.scopeParamTree, "Scope Settings")
        self.settingsTargetDock = self.addSettings(self.targetParamTree, "Target Settings")
        self.settingsTraceDock = self.addSettings(self.traceParamTree, "Trace Settings")
        self.settingsAuxDock = self.addSettings(self.auxParamTree, "Aux Settings")

    def setupParametersTree(self):
        self.generalParamTree = ParameterTree()
        self.scopeParamTree = ParameterTree()
        self.targetParamTree = ParameterTree()
        self.traceParamTree = ParameterTree()
        self.auxParamTree = ParameterTree()

        self.params = Parameter.create(name='Generic Settings', type='group', children=self.manager.cwParams)
        ExtendedParameter.setupExtended(self.params, self)
        self.generalParamTree.setParameters(self.params, showTop=False)

    def reloadScopeParamList(self):
        # Remove all old scope actions that don't apply for new selection
        for act in self._scopeToolMenuItems:
            self.toolMenu.removeAction(act)

        self._scopeToolMenuItems = []

        if self.manager.hasScope():
            ExtendedParameter.reloadParams(self.manager.getScope().paramList(), self.scopeParamTree, help_window=self.helpbrowser.helpwnd)

            # Check for any tools to add too
            if hasattr(self.manager.getScope(), "guiActions"):
                self._scopeToolMenuItems.append(self.toolMenu.addSeparator())
                for act in self.manager.getScope().guiActions(self):
                    self._scopeToolMenuItems.append(QAction(act[0], self, statusTip=act[1], triggered=act[2]))

        for act in self._scopeToolMenuItems:
            self.toolMenu.addAction(act)

    def reloadTargetParamList(self):
        ExtendedParameter.reloadParams(self.manager.getTarget().paramList(), self.targetParamTree, help_window=self.helpbrowser.helpwnd)

    def reloadTraceParamList(self):
        ExtendedParameter.reloadParams(self.manager.getTraceClass().getParams.paramList(), self.traceParamTree, help_window=self.helpbrowser.helpwnd)

    def reloadAuxParamList(self):
        ExtendedParameter.reloadParams(self.manager.getAuxiliaryModules()[0].paramList(), self.auxParamTree, help_window=self.helpbrowser.helpwnd)

    def reloadParamList(self):
        ExtendedParameter.reloadParams(self.paramList(), self.generalParamTree, help_window=self.helpbrowser.helpwnd)

    def paramList(self):
        p = []
        p.append(self.params)

        if self.manager.acqPattern is not None:
            for par in self.manager.acqPattern.paramList():
                p.append(par)
        return p

    def newScopeData(self, data=None, offset=0):
        self.waveformDock.passTrace(data, offset)

    def setConfigWidget(self, widget):
        self.configdock.setWidget(widget)

    def addToolbar(self):
        # Capture
        self.capture1Act = QAction(QIcon(':/images/play1.png'), 'Capture 1', self)
        self.capture1Act.triggered.connect(lambda: self.doCapture(self.capture1))
        self.captureMAct = QAction(QIcon(':/images/playM.png'), 'Capture Multi', self)
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

        self.toolBar = self.addToolBar('Tools')
        self.toolBar.setObjectName('Tools')
        self.toolBar.addAction(self.capture1Act)
        self.toolBar.addAction(self.captureMAct)
        self.toolBar.addSeparator()
        self.toolBar.addWidget(QLabel('Master:'))
        self.toolBar.addWidget(self.captureStatus)
        self.toolBar.addWidget(QLabel('Scope:'))
        self.toolBar.addWidget(self.scopeStatus)
        self.toolBar.addWidget(QLabel('Target:'))
        self.toolBar.addWidget(self.targetStatus)
        self.toolBar.addSeparator()
        self.toolBar.addAction(self.miscValidateAction)
        self.toolBar.show()

    def doConDisScope(self):
        if self.scopeStatus.defaultAction() == self.scopeStatusActionDis:
            self.manager.connectScope()
            self.updateStatusBar("Scope Connected")
        else:
            self.manager.disconnectScope()
            self.updateStatusBar("Scope Disconnected")

    def doConDisTarget(self):
        if self.targetStatus.defaultAction() == self.targetStatusActionDis:
            self.manager.connectTarget()
            self.updateStatusBar("Target Connected")
        else:
            self.manager.disconnectTarget()
            self.updateStatusBar("Target Disconnected")

    def doConDis(self):
        """Toggle connect button pushed (master): attempts both target & scope connection"""
        if self.captureStatus.defaultAction() == self.captureStatusActionDis:
            self.manager.connect()
            self.updateStatusBar("Target and Scope Connected")
        else:
            self.manager.disconnect()
            self.updateStatusBar("Target and Scope Disconnected")

    def validateSettings(self, warnOnly=False):
        # Validate settings from all modules before starting multi-capture
        vw = ValidationDialog(onlyOkButton=not warnOnly)

        for i in self.manager.validateSettings():
            vw.addMessage(*i)

        if self.project().dataDirIsDefault:
             vw.addMessage("info", "File Menu", "Project not saved, using default-data-dir", "Save project file before capture", "8c9101ff-7553-4686-875d-b6a8a3b1d2ce")

        if vw.numWarnings() > 0 or warnOnly == False:
            return vw.exec_()
        else:
            return True

    def newProject(self):
        self.setProject(ProjectFormat(self))
        self.project().setProgramName(__name__)
        self.project().setProgramVersion(__version__)
        self.project().addParamTree(self)
        self.project().addParamTree(self.manager.getScope())
        self.project().addParamTree(self.manager.getTarget())
        self.project().setTraceManager(self.manager.getTraceManager())
        self.updateTitleBar()

    def saveProject(self):
        if self.project().hasFilename() == False :
            # fname = QFileDialog.getSaveFileName(self, 'Save New File', '.','*.cwp')[0]      # File Dialog sometimes will not show up with Mac OS
            fname, _ = QFileDialog.getSaveFileName(self, 'Save New File', '.','*.cwp','', QFileDialog.DontUseNativeDialog)
            if not fname: return

            self.project().setFilename(fname)

        self.project().save()
        self.dirty = False
        self.updateStatusBar("Project Saved")
        self.updateTitleBar()

    def doCapture(self, callback):
        try:
            self.capture1Act.setEnabled(False)
            self.captureMAct.setEnabled(False)
            self.updateStatusBar(callback())
        finally:
            self.capture1Act.setEnabled(True)
            self.captureMAct.setEnabled(True)
            self.capture1Act.setChecked(False)
            self.captureMAct.setChecked(False)

    def capture1(self):
        self.manager.capture1()
        return "Capture-One Completed"

    def captureM(self):
        cprog = CaptureProgressDialog(ntraces=self.manager.getNumTraces(), nsegs=self.manager.getNumSegments())
        cprog.startCapture()
        self.manager.signals.traceDone.connect(cprog.traceDoneSlot)
        self.manager.signals.campaignDone.connect(cprog.incSeg)
        cprog.abortCapture.connect(self.manager.signals.abortCapture.emit)
        self.manager.captureM(self.project().datadirectory)
        self.manager.signals.campaignDone.disconnect(cprog.incSeg)
        self.manager.signals.traceDone.disconnect(cprog.traceDoneSlot)
        return "Capture-M Completed"


def makeApplication():
    # Create the Qt Application
    app = QApplication(sys.argv)
    app.setOrganizationName(__organization__)
    app.setApplicationName(__name__ + " " + __version__)
    return app

def main():
    app = makeApplication()

    # Create and show the form
    window = ChipWhispererCapture(os.path.join('chipwhisperer', 'capture'))
    window.show()

    # Run the main Qt loop
    sys.exit(app.exec_())

if __name__ == '__main__':
    main()
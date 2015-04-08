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
import importlib
from functools import partial

try:
    # OrderedDict is new in 2.7
    from collections import OrderedDict
    dicttype = OrderedDict
except ImportError:
    dicttype = dict

try:
    from PySide.QtCore import *
    from PySide.QtGui import *
except ImportError:
    print "ERROR: PySide is required for this program"
    sys.exit()

from datetime import datetime
import random
import os.path
import traceback

from openadc.ExtendedParameter import ExtendedParameter
import chipwhisperer.common.qrc_resources
import chipwhisperer.common.ParameterTypesCustom

try:
    import writer_dpav3
except ImportError:
    writer_dpav3 = None
    writer_dpav3_str = sys.exc_info()

try:
    import pyqtgraph as pg
    import pyqtgraph.multiprocess as mp
    import pyqtgraph.parametertree.parameterTypes as pTypes
    from pyqtgraph.parametertree import Parameter, ParameterTree, ParameterItem, registerParameterType
    #print pg.systemInfo()

except ImportError:
    print "ERROR: PyQtGraph is required for this program"
    sys.exit()

from chipwhisperer.capture.utils.SerialTerminalDialog import SerialTerminalDialog as SerialTerminalDialog
from chipwhisperer.capture.utils.GlitchExplorerDialog import GlitchExplorerDialog as GlitchExplorerDialog
from chipwhisperer.capture.scopes.OpenADC import OpenADCInterface as OpenADCInterface
import chipwhisperer.capture.global_mod as global_mod

try:
    from  chipwhisperer.capture.scopes.VisaScope import VisaScopeInterface as VisaScopeInterface
except:
    # VISA Scope uses WindowsError it seems? If so catch on OSError to work on Linux too
    VisaScopeInterface = None

try:
    from  chipwhisperer.capture.scopes.PicoScope import PicoScopeInterface as PicoScopeInterface
except ImportError:
    PicoScopeInterface = None

try:
    import  chipwhisperer.capture.targets.SimpleSerial as target_SimpleSerial
except ImportError:
    target_SimpleSerial = None
    target_SimpleSerial_str = sys.exc_info()

try:
    import  chipwhisperer.capture.targets.SmartCard as target_SmartCard
except ImportError:
    target_SmartCard = None
    target_SmartCard_str = sys.exc_info()
    print "SmartCard targets disabled: %s"%str(target_SmartCard_str)

try:
    import  chipwhisperer.capture.targets.SASEBOGII as target_SASEBOGII
except ImportError:
    target_SASEBOGII = None
    target_SASEBOGII_str = sys.exc_info()

try:
    import  chipwhisperer.capture.targets.SAKURAG as target_SAKURAG
except ImportError:
    target_SAKURAG = None
    target_SAKURAG_str = sys.exc_info()

try:
    import  chipwhisperer.capture.targets.ChipWhispererSPI as target_CWSPI
except ImportError:
    target_CWSPI = None
    target_CWSPI_str = sys.exc_info()

try:
    import  chipwhisperer.capture.auxiliary.FrequencyMeasure as aux_FrequencyMeasure
except ImportError:
    aux_FrequencyMeasure = None

try:
    import  chipwhisperer.capture.auxiliary.ResetAVR as aux_ResetAVR
except ImportError:
    aux_ResetAVR = None

try:
    import  chipwhisperer.capture.auxiliary.GPIOToggle as aux_GPIOToggle
except ImportError:
    aux_GPIOToggle = None

from chipwhisperer.common.MainChip import MainChip
from chipwhisperer.common.ProjectFormat import ProjectFormat
from chipwhisperer.common.traces.TraceContainerNative import TraceContainerNative
from chipwhisperer.common.traces.TraceContainerDPAv3 import TraceContainerDPAv3

try:
    from chipwhisperer.common.traces.TraceContainerMySQL import TraceContainerMySQL
except ImportError:
    TraceContainerMySQL = None

from chipwhisperer.capture.ListAllModules import ListAllModules
from chipwhisperer.common.ValidationDialog import ValidationDialog
from chipwhisperer.capture.CaptureProgressDialog import CaptureProgressDialog
from chipwhisperer.capture.AcquisitionController import AcquisitionController, AcqKeyTextPattern_Basic, AcqKeyTextPattern_CRITTest

class TargetInterface(QObject):
    """This is a standard target interface, which controls various supported lower-level hardware interfaces"""
    paramListUpdated = Signal(list)
    targetUpdated = Signal(bool)
    connectStatus = Signal(bool)
    newInputData = Signal(list)

    def __init__(self, parent=None, log=None,showScriptParameter=None):
        super(TargetInterface, self).__init__(parent)
        valid_targets = dicttype()
        self.driver = None
        self.log=log
        self.showScriptParameter = showScriptParameter

        valid_targets["None"] = None

        if target_SimpleSerial is not None:
            valid_targets["Simple Serial"] = target_SimpleSerial.SimpleSerial(self.log, showScriptParameter=showScriptParameter)

        if target_SmartCard is not None:
            valid_targets["Smart Card"] = target_SmartCard.SmartCard(self.log, showScriptParameter=showScriptParameter)

        if target_SASEBOGII is not None:
            valid_targets["SASEBO GII"] = target_SASEBOGII.SaseboGII(self.log, showScriptParameter=showScriptParameter)

        if target_SAKURAG is not None:
            valid_targets["SAKURA G"] = target_SAKURAG.SakuraG(self.log, showScriptParameter=showScriptParameter)

        if target_CWSPI is not None:
            valid_targets["ChipWhisperer SPI"] = target_CWSPI.ChipWhispererSPI(self.log, showScriptParameter=showScriptParameter)

        self.toplevel_param = {'name':'Target Module', 'type':'list', 'values':valid_targets, 'value':valid_targets["None"], 'set':self.setDriver}

    def setOpenADC(self, oadc):
        '''Declares OpenADC Instance in use. Only for openadc-integrated targets'''
        try:  # if hasattr(oadc, "scope.sc"):
            self.oadc = oadc.scope.sc
        except AttributeError:
            self.oadc = oadc
        if hasattr(self.driver, "setOpenADC"):
            self.driver.setOpenADC(self.oadc)

    def con(self):
        if self.driver is not None:
            self.driver.con()
            self.connectStatus.emit(True)

    def dis(self):
        if self.driver is not None:
            self.driver.dis()
            self.connectStatus.emit(False)

    def setDriver(self, driver):
        self.driver = driver
        if hasattr(self.driver, "setOpenADC") and hasattr(self, "oadc") and self.oadc:
            self.driver.setOpenADC(self.oadc)

        if self.driver is None:
            self.paramListUpdated.emit(None)
            self.targetUpdated.emit(False)
        else:
            self.driver.paramListUpdated.connect(self.paramListUpdated.emit)
            self.driver.newInputData.connect(self.newInputData.emit)
            self.paramListUpdated.emit(self.driver.paramList())
            self.targetUpdated.emit(True)

    def paramList(self):
        if self.driver is None:
            return [None]
        else:
            return self.driver.paramList()

class EncryptionStatusMonitor(QDialog):
    def __init__(self, parent):
        super(EncryptionStatusMonitor, self).__init__(parent)

        self.dLayout = QVBoxLayout()

        self.textResultsLayout = QGridLayout()
        self.textInLine = QLineEdit()
        self.textOutLine = QLineEdit()
        self.textResultsLayout.addWidget(QLabel("Text In "), 0, 0)
        self.textInLine.setReadOnly(True)
        self.textResultsLayout.addWidget(self.textInLine, 0, 1)
        self.textResultsLayout.addWidget(QLabel("Text Out"), 1, 0)
        self.textOutLine.setReadOnly(True)
        self.textResultsLayout.addWidget(self.textOutLine, 1, 1)
        self.textResultsLayout.addWidget(QLabel("Expected"), 2, 0)
        self.textOutExpected = QLineEdit()
        self.textOutExpected.setReadOnly(True)
        self.textResultsLayout.addWidget(self.textOutExpected, 2, 1)

        self.textResultsLayout.addWidget(QLabel("Enc. Key"), 3, 0)
        self.textEncKey = QLineEdit()
        self.textEncKey.setReadOnly(True)
        self.textResultsLayout.addWidget(self.textEncKey, 3, 1)

        self.dLayout.addLayout(self.textResultsLayout)

        # Count of OK passes
        cntLayout = QHBoxLayout()

        self.totalOps = QLabel("0")
        self.totalOK = QLabel("?")
        self.totalFail = QLabel("?")
        self.clearPB = QPushButton("Clear")
        self.clearPB.clicked.connect(self.clrCnt)

        cntLayout.addWidget(QLabel("Total Ops: "))
        cntLayout.addWidget(self.totalOps)
        cntLayout.addStretch()
        cntLayout.addWidget(QLabel("Total OK: "))
        cntLayout.addWidget(self.totalOK)
        cntLayout.addStretch()
        cntLayout.addWidget(QLabel("Total Failed: "))
        cntLayout.addWidget(self.totalFail)
        cntLayout.addStretch()
        cntLayout.addWidget(self.clearPB)
        self.clrCnt()

        self.dLayout.addLayout(cntLayout)

        self.setLayout(self.dLayout)
        self.hide()
        
    def clrCnt(self, ignored=False):
        self._cntops = 0
        self._okops = 0
        self._failops = 0
        self.totalOps.setText("%d" % self._cntops)
        self.totalOK.setText("%d" % self._okops)
        self.totalFail.setText("%d" % self._failops)

    def setHexText(self, lineedit, data):
        if data is not None:
            text = ""
            for t in data: text += "%02X " % t
            lineedit.setText(text)
        else:
            lineedit.setText("?")

    def newData(self, key, pt, ct, expected):
        self.setHexText(self.textOutLine, ct)
        self.setHexText(self.textInLine, pt)
        self.setHexText(self.textEncKey, key)
        self.setHexText(self.textOutExpected, expected)
        
        self._cntops += 1

        if ct and expected and list(ct) == list(expected):
            self._okops += 1
        else:
            self._failops += 1

        self.totalOps.setText("%d" % self._cntops)
        self.totalOK.setText("%d" % self._okops)
        self.totalFail.setText("%d" % self._failops)


class ChipWhispererCapture(MainChip):
    MaxRecentFiles = 4
    def __init__(self, rootdir="."):
        global_mod.main_window = self
        super(ChipWhispererCapture, self).__init__(name=("ChipWhisperer" + u"\u2122" + " Capture V2"), icon="cwiconC")
        self.console = self.addConsole()

        # This is a hack for paths hardcoded into the application. todo: fix this properly.
        QSettings().setValue("cwcapture-starting-root", rootdir)
        self._rootdir = rootdir
        self._scriptRootDir = rootdir + "/scripts"

        self.scope = None
        self.trace = None
        self.auxList = None
        self.target = TargetInterface(log=self.console, showScriptParameter=self.showScriptParameter)
        self.target.paramListUpdated.connect(self.reloadTargetParamList)
        self.target.newInputData.connect(self.newTargetData)

        valid_scopes = dicttype()
        valid_traces = dicttype()
        valid_aux = dicttype()

        
        valid_scopes["None"] = None
        valid_scopes["ChipWhisperer/OpenADC"] = OpenADCInterface(parent=self, console=self.console, showScriptParameter=self.showScriptParameter)
        
        valid_traces["None"] = None
        valid_traces["ChipWhisperer/Native"] = TraceContainerNative
        valid_traces["DPAContestv3"] = TraceContainerDPAv3
        valid_aux["None"] = None

        # If you want to add a 'hacked-in' module, you can do that in the 'aux' system. The aux system is designed to make
        # it very easy to add some code that does something like measure an external instrument, or control some other
        # system. Useful if you are wanting to do something like script different core voltages, frequencies, or otherwise
        # control some external device for every capture run.

        if aux_FrequencyMeasure is not None:
            valid_aux["Frequency Counter"] = aux_FrequencyMeasure.FrequencyMeasure(console=self.console, showScriptParameter=self.showScriptParameter)

        if aux_ResetAVR is not None:
            valid_aux["Reset AVR via ISP-MKII"] = aux_ResetAVR.ResetAVR(console=self.console, showScriptParameter=self.showScriptParameter)

        if aux_GPIOToggle is not None:
            valid_aux["Toggle FPGA-GPIO Pins"] = aux_GPIOToggle.GPIOToggle(self, console=self.console, showScriptParameter=self.showScriptParameter)

        if TraceContainerMySQL is not None:
            valid_traces["MySQL"] = TraceContainerMySQL

        if VisaScopeInterface is not None:
            valid_scopes["VISA Scope"] = VisaScopeInterface(parent=self, console=self.console, showScriptParameter=self.showScriptParameter)

        if PicoScopeInterface is not None:
            valid_scopes["PicoScope"] = PicoScopeInterface(parent=self, console=self.console, showScriptParameter=self.showScriptParameter)

        self.esm = EncryptionStatusMonitor(self)

        self.serialTerminal = SerialTerminalDialog(self)

        self.glitchMonitor = GlitchExplorerDialog(self, showScriptParameter=self.showScriptParameter)
        self.paramTrees.append(self.glitchMonitor.paramTree)

        valid_acqPatterns = {"Basic":AcqKeyTextPattern_Basic(console=self.console, showScriptParameter=self.showScriptParameter)}

        if AcqKeyTextPattern_CRITTest:
            valid_acqPatterns['CRI T-Test'] = AcqKeyTextPattern_CRITTest(console=self.console, showScriptParameter=self.showScriptParameter)

        self.setAcqPattern(valid_acqPatterns['Basic'], reloadList=False)

        self.cwParams = [
                {'name':'Scope Module', 'type':'list', 'values':valid_scopes, 'value':valid_scopes["None"], 'set':self.scopeChanged},
                self.target.toplevel_param,
                {'name':'Trace Format', 'type':'list', 'values':valid_traces, 'value':valid_traces["None"], 'set':self.traceChanged},

                {'name':'Auxilary Module', 'type':'list', 'values':valid_aux, 'value':valid_aux["None"], 'set':self.auxChanged },

                # {'name':'Key Settings', 'type':'group', 'children':[
                #        {'name':'Encryption Key', 'type':'str', 'value':self.textkey, 'set':self.setKey},
                #        {'name':'Send Key to Target', 'type':'bool', 'value':True},
                #        {'name':'New Encryption Key/Trace', 'key':'newKeyAlways', 'type':'bool', 'value':False},
                #    ]},

                {'name':'Acquisition Settings', 'type':'group', 'children':[
                        {'name':'Number of Traces', 'type':'int', 'limits':(1, 1E9), 'value':100, 'set':self.setNumTraces, 'get':self.getNumTraces},
                        {'name':'Capture Segments', 'type':'int', 'limits':(1, 1E6), 'value':1, 'set':self.setNumSegments, 'get':self.getNumSegments, 'tip':'Break capture into N segments, '
                         'which may cause data to be saved more frequently. The default capture driver requires that NTraces/NSegments is small enough to avoid running out of system memory '
                         'as each segment is buffered into RAM before being written to disk.'},
                        {'name':'Open Monitor', 'type':'action', 'action':self.esm.show},
                        {'name':'Key/Text Pattern', 'type':'list', 'values':valid_acqPatterns, 'value':valid_acqPatterns['Basic'], 'set':self.setAcqPattern},
                        # {'name':'Fixed Plaintext', 'type':'bool', 'value':False, 'set':self.setFixedPlain },
                        # {'name':'Fixed Plaintext Value', 'type':'str', 'value':self.plaintextStr, 'set':self.setPlaintext},
                    ]},

                # {'name':''}
                ]

        self.da = None
        self.numTraces = 100
        self.numSegments = 1

        self.addToolbars()
        self.addSettingsDocks()
        self.addWaveforms()
        self.addToolMenu()

        self.addExampleScripts()

        self.restoreDockGeometry()
        self.dockifySettings()
        self.settingsAuxDock.setVisible(False)

        self.newProject()

        self.newFile.connect(self.newProject)
        self.saveFile.connect(self.saveProject)

        self.fixedPlain = False
        self.target.targetUpdated.connect(self.targetUpdated)
        self.target.connectStatus.connect(self.targetStatusChanged)

        self.targetConnected = False
        self.reloadParamList()

    def listModules(self):
        """Overload this to test imports"""
        return ListAllModules()

    def newTargetData(self, data):
        self.glitchMonitor.addResponse(data)

    def targetUpdated(self, enabled):
        self.TargetToolbar.setEnabled(enabled)

        if enabled:
            self.acqPattern.setTarget(self.target.driver)

    def setAcqPattern(self, pat, reloadList=True):
        self.acqPattern = pat
        self.acqPattern.setTarget(self.target.driver)

        if reloadList:
            self.reloadParamList()

    def setFixedPlain(self, x):
        self.fixedPlain = x

    def getNumTraces(self):
        return self.numTraces

    def setNumTraces(self, t):
        self.numTraces = t

    def setNumSegments(self, s):
        self.numSegments = s

    def getNumSegments(self):
        return self.numSegments

    def FWLoaderGo(self):
        print "NOTE: Call to cap.FWLoaderGo() not required anymore, will be removed in future versions"

    def addExampleScripts(self):
        self.exampleScriptAct = QAction('&Example Scripts', self, statusTip='Predefined Scripts')

        self.projectMenu.addSeparator()
        self.projectMenu.addAction(self.exampleScriptAct)

        subMenu = QMenu("Submenu", self)

        self.scriptList = []

        if os.path.isdir(self._scriptRootDir):
            for fn in os.listdir(self._scriptRootDir + '/.'):
                fnfull = self._scriptRootDir + '/' + fn
                if os.path.isfile(fnfull) and fnfull.lower().endswith('.py'):
                    try:
                        m = importlib.import_module('chipwhisperer.capture.scripts.' + os.path.splitext(fn)[0])
                        m.name()
                        self.scriptList.append(m)
                    except ImportError, e:
                        #for debugging uncomment this:
                        #print str(e)
                        pass

                    except AttributeError,e:
                        #for debugging uncomment this:
                        #print str(e)
                        pass

        for t in self.scriptList:
            subMenu.addAction(QAction(t.name(), self, statusTip=t.tip(), triggered=partial(self.runScript, t)))

        self.exampleScriptAct.setMenu(subMenu)

    def runScript(self, mod):
        self.console.append( "****Running Script: %s"%mod.name() )
        m = mod.userScript(self)
        m.run()
        self.console.append( "****Finished Script: %s"%mod.name() )


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
        self.waveformDock = self.addTraceDock("Capture Waveform (Channel 1)")

        #TODO: FIX THIS HACK
        #Should be something in ScopeInterface class maybe
        self.waveformDock.widget().setDefaultYRange(-0.5, 0.5)
        self.waveformDock.widget().YDefault()

    def addSettingsDocks(self):
        self.setupParametersTree()
        self.settingsNormalDock = self.addSettings(self.paramTree, "General Settings")
        self.settingsScopeDock = self.addSettings(self.scopeParamTree, "Scope Settings")
        self.settingsTargetDock = self.addSettings(self.targetParamTree, "Target Settings")
        self.settingsTraceDock = self.addSettings(self.traceParamTree, "Trace Settings")
        self.settingsAuxDock = self.addSettings(self.auxParamTree, "Aux Settings")
        self.settingsAuxDock.setVisible(False)

    def setupParametersTree(self):
        self.params = Parameter.create(name='Generic Settings', type='group', children=self.cwParams)
        ExtendedParameter.setupExtended(self.params, self)
        self.paramTree = ParameterTree()
        self.paramTree.setParameters(self.params, showTop=False)

        self.scopeParamTree = ParameterTree()
        self.targetParamTree = ParameterTree()
        self.traceParamTree = ParameterTree()
        self.auxParamTree = ParameterTree()

    def reloadScopeParamList(self, lst=None):
        
        # Remove all old scope actions that don't apply for new selection
        for act in self._scopeToolMenuItems:
            self.toolMenu.removeAction(act)
        self._scopeToolMenuItems = []

        if self.scope is not None:
            ExtendedParameter.reloadParams(self.scope.paramList(), self.scopeParamTree)

            # Check for any tools to add too
            if hasattr(self.scope, "guiActions") and len(self.scope.guiActions()) > 0:
                sep = self.toolMenu.addSeparator()
                acts = self.scope.guiActions()
                self.toolMenu.addActions(acts)
                self._scopeToolMenuItems.extend(acts)
                self._scopeToolMenuItems.append(sep)

    def reloadTargetParamList(self, lst=None):
        if self.target is not None:
            ExtendedParameter.reloadParams(self.target.paramList(), self.targetParamTree)

    def reloadTraceParamList(self, lst=None):
        if self.traceparams is not None:
            try:
                ExtendedParameter.reloadParams(self.traceparams.paramList(), self.traceParamTree)
            except AttributeError:
                #Some trace writers have no configuration options
                pass

    def reloadAuxParamList(self, lst=None):
        if self.auxList is not None:
            try:
                ExtendedParameter.reloadParams(self.auxList[0].paramList(), self.auxParamTree)
            except AttributeError:
                # Some trace writers have no configuration options
                pass

    def reloadParamList(self, lst=None):
        ExtendedParameter.reloadParams(self.paramList(), self.paramTree)

    def paramList(self):
        p = []
        p.append(self.params)

        if self.acqPattern is not None:
            acqParams = self.acqPattern.paramList()
            for ap in acqParams:
                p.append(ap)
        return p

    def newScopeData(self,  data=None, offset=0):
        self.waveformDock.widget().passTrace(data, offset)

    def setConfigWidget(self, widget):
        self.configdock.setWidget(widget)

    def addToolbars(self):
        self.addCaptureTools()

    def addCaptureTools(self):
        self.capture1Act = QAction(QIcon(':/images/play1.png'), 'Capture 1', self)
        self.capture1Act.triggered.connect(self.capture1)
        self.capture1Act.setCheckable(True)
        self.captureMAct = QAction(QIcon(':/images/playM.png'), 'Capture Multi', self)
        self.captureMAct.triggered.connect(self.captureM)
        self.captureMAct.setCheckable(True)

        self.captureStatus = QToolButton()
        self.captureStatusActionDis = QAction(QIcon(':/images/status_disconnected.png'), 'Master: Disconnected', self)
        self.captureStatusActionDis.triggered.connect(self.doConDis)
        self.captureStatusActionCon = QAction(QIcon(':/images/status_connected.png'), 'Master: Connected', self)
        self.captureStatus.setDefaultAction(self.captureStatusActionDis)

        self.CaptureToolbar = self.addToolBar('Capture Tools')
        self.CaptureToolbar.setObjectName('Capture Tools')
        self.CaptureToolbar.addAction(self.capture1Act)
        self.CaptureToolbar.addAction(self.captureMAct)
        self.CaptureToolbar.addWidget(QLabel('Master:'))
        self.CaptureToolbar.addWidget(self.captureStatus)
        #self.CaptureToolbar.setEnabled(False)

        # Scope Toolbar
        self.scopeStatus = QToolButton()
        self.scopeStatusActionDis = QAction(QIcon(':/images/status_disconnected.png'), 'Scope: Disconnected', self)
        self.scopeStatusActionDis.triggered.connect(self.doConDisScope)
        self.scopeStatusActionCon = QAction(QIcon(':/images/status_connected.png'), 'Scope: Connected', self)
        self.scopeStatus.setDefaultAction(self.scopeStatusActionDis)

        self.ScopeToolbar = self.addToolBar('Scope Toolbar')
        self.ScopeToolbar.setObjectName('Scope Toolbar')
        self.ScopeToolbar.addWidget(QLabel('Scope:'))
        self.ScopeToolbar.addWidget(self.scopeStatus)
        self.ScopeToolbar.setEnabled(False)

        # Target Toolbar
        self.targetStatus = QToolButton()
        self.targetStatusActionDis = QAction(QIcon(':/images/status_disconnected.png'), 'Target: Disconnected', self)
        self.targetStatusActionDis.triggered.connect(self.doConDisTarget)
        self.targetStatusActionCon = QAction(QIcon(':/images/status_connected.png'), 'Target: Connected', self)
        self.targetStatus.setDefaultAction(self.targetStatusActionDis)

        self.TargetToolbar = self.addToolBar('Target Toolbar')
        self.TargetToolbar.setObjectName('Target Toolbar')
        self.TargetToolbar.addWidget(QLabel('Target:'))
        self.TargetToolbar.addWidget(self.targetStatus)
        self.TargetToolbar.setEnabled(False)

        # Other Stuff
        self.miscValidateAction = QAction(QIcon(':/images/validate.png'), 'Validate', self)
        self.miscValidateAction.triggered.connect(self.validateSettings)

        self.MiscToolbar = self.addToolBar('Misc Tools')
        self.MiscToolbar.setObjectName('Misc Tools')
        self.MiscToolbar.addAction(self.miscValidateAction)


    def masterStatusChanged(self):
        # Deal with multiple

        if self.scopeStatus.defaultAction() == self.scopeStatusActionCon:
            scopeStat = True
        else:
            scopeStat = False

        if self.targetStatus.defaultAction() == self.targetStatusActionCon:
            targetStat = True
        else:
            targetStat = False

        if targetStat or scopeStat:
            self.captureStatus.setDefaultAction(self.captureStatusActionCon)
        else:
            self.captureStatus.setDefaultAction(self.captureStatusActionDis)

    def scopeStatusChanged(self, status=True, text=None):
        """Callback when scope connection successful"""
        # self.CaptureToolbar.setEnabled(status)

        if status:
            self.scopeStatus.setDefaultAction(self.scopeStatusActionCon)
        else:
            self.scopeStatus.setDefaultAction(self.scopeStatusActionDis)

        self.masterStatusChanged()

    def targetStatusChanged(self, status=True, text=None):
        """Callback when target connection successful"""
        #self.CaptureToolbar.setEnabled(status)

        self.targetConnected = status

        if status:
            self.targetStatus.setDefaultAction(self.targetStatusActionCon)
        else:
            self.targetStatus.setDefaultAction(self.targetStatusActionDis)

        self.masterStatusChanged()

    def doConDisTarget(self, con=None):
        """Toggle connect button pushed (target), alternatively can use via API by setting 'con' to True or False"""

        if self.target is None:
            return

        #Triggered from GUI
        if con is None:
            if self.targetStatus.defaultAction() == self.targetStatusActionDis:
                con = True
            else:
                con = False

        #Triggered from API
        try:
            if con:
                self.target.con()
                self.statusBar().showMessage("Target Connected")
            else:
                self.target.dis()
        except IOError, e:
            self.console.append("Target Error: %s"%str(e))


    def doConDisScope(self, con=None):
        """Toggle connect button pushed (scope), alternatively can use via API by setting 'con' to True or False"""
        if self.scope is None:
            return

        # Triggered from GUI
        if con is None:
            if self.scopeStatus.defaultAction() == self.scopeStatusActionDis:
                con = True
            else:
                con = False

        # Triggered from API
        try:
            if con:
                self.scope.con()
                self.statusBar().showMessage("Scope Connected")
                #Pass to target if required
                if hasattr(self.target, "setOpenADC"):
                    self.target.setOpenADC(self.scope.qtadc.ser)

            else:
                self.scope.dis()
        except IOError, e:
            self.console.append("Target Error: %s" % str(e))


    def doConDis(self):
        """Toggle connect button pushed (master): attempts both target & scope connection"""
        if self.captureStatus.defaultAction() == self.captureStatusActionDis:
            self.doConDisScope(True)
            self.doConDisTarget(True)
        else:
            self.doConDisScope(False)
            self.doConDisTarget(False)

    def capture1(self):

        if self.target.driver and self.targetConnected:
            target = self.target.driver
        else:
            target = None

        try:
            ac = AcquisitionController(self.scope, target, writer=None, auxList=self.auxList, keyTextPattern=self.acqPattern)
            ac.newTextResponse.connect(self.esm.newData)

            self.capture1Act.setEnabled(False)
            self.captureMAct.setEnabled(False)

            ac.doSingleReading()
            self.statusBar().showMessage("One Capture Complete")

        except IOError, e:
            self.statusBar().showMessage("Error: %s" % str(e))
            print "Exception caught: %s" % str(e)
            print traceback.format_exc()

        self.capture1Act.setChecked(False)
        self.capture1Act.setEnabled(True)
        self.captureMAct.setEnabled(True)


    def validateSettings(self, warnOnly=False):
        # Validate settings from all modules before starting multi-capture
        vw = ValidationDialog(onlyOkButton=not warnOnly)

        if self.target.driver:
            target = self.target.driver
        else:
            target = None

        # Basic Validation of settings from the main GUI
        if target is None:
            vw.addMessage("warn", "General Settings", "No Target Module", "Specify Target Module", "2351e3b0-e5fe-11e3-ac10-0800200c9a66")
        else:
            try:
                target.validateSettings(vw)
            except AttributeError:
                vw.addMessage("info", "Target Module", "Target has no validateSettings()", "Internal Error", "73b08424-3865-4274-8fd7-dd213ede2c46")

        if self.scope is None:
            vw.addMessage("warn", "General Settings", "No Scope Module", "Specify Scope Module", "325de1cf-0d47-4ed8-8e9f-77d8f9cf2d5f")
        else:
            try:
                self.scope.validateSettings(vw)
            except AttributeError:
                vw.addMessage("info", "Scope Module", "Scope has no validateSettings()", "Internal Error", "d19be31d-ad1a-4533-80dc-9423dfa92753")

        if self.trace is not None:
            writer = self.trace(self.traceparams)
        else:
            writer = None

        if writer is None:
            vw.addMessage("warn", "General Settings", "No Writer Module", "Specify Trace Writer Module", "57a3924d-3794-4ca6-9693-46a7b5243727")
        else:
            try:
                writer.validateSettings(vw)
            except AttributeError:
                vw.addMessage("info", "Writer Module", "Writer has no validateSettings()", "Internal Error", "d7b3a9a1-83f0-4b4d-92b9-3d7dcf6304ae")

        if self.project().dataDirIsDefault:
            vw.addMessage("info", "File Menu", "Project not saved, using default-data-dir", "Save project file before capture", "8c9101ff-7553-4686-875d-b6a8a3b1d2ce")

        tracesPerRun = int(self.numTraces / self.numSegments)

        if tracesPerRun > 10E3:
            vw.addMessage("warn", "General Settings", "Very Long Capture (%d traces)" % tracesPerRun, "Set 'Capture Segments' to '%d'" % (self.numTraces / 10E3), "1432bf95-9026-4d8c-b15d-9e49147840eb")

        if vw.numWarnings() > 0 or warnOnly == False:
            return vw.exec_()
        else:
            return True

    def captureM(self):
        if self.target.driver:
            target = self.target.driver
        else:
            target = None

        if self.validateSettings(True) == False:
            return

        overallstarttime = datetime.now()

        tcnt = 0
        writerlist = []

        tracesPerRun = int(self.numTraces / self.numSegments)

        cprog = CaptureProgressDialog(ntraces=self.numTraces, nsegs=self.numSegments)

        cprog.startCapture()

        # This system re-uses one wave buffer a bunch of times. This is required since the memory will become
        # fragmented, even though you are just freeing & reallocated the same size buffer. It's slightly less
        # clear but it ensures you don't suddently have a capture interrupted with a memory error. This can
        # happen even if you have loads of memory free (e.g. are only using ~200MB for the buffer), well before
        # the 1GB limit that a 32-bit process would expect to give you trouble at.
        waveBuffer = None

        for i in range(0, self.numSegments):

            cprog.incSeg()

            if self.trace is not None:
                writer = self.trace(self.traceparams)
            else:
                writer = None

            starttime = datetime.now()
            baseprefix = starttime.strftime('%Y.%m.%d-%H.%M.%S')
            prefix = baseprefix + "_"

            # Load trace writer information
            if writer:
                writer.config.setAttr("prefix", prefix)
                writer.config.setConfigFilename(self.project().datadirectory + "traces/config_" + prefix + ".cfg")
                writer.config.setAttr("date", starttime.strftime('%Y-%m-%d %H:%M:%S'))
                writer.setTraceHint(tracesPerRun)

                if waveBuffer is not None:
                    writer.setTraceBuffer(waveBuffer)


            if self.auxList is not None:
                for aux in self.auxList:
                    aux.setPrefix(baseprefix)

            ac = AcquisitionController(self.scope, target, writer, auxList=self.auxList, keyTextPattern=self.acqPattern)
            ac.newTextResponse.connect(self.esm.newData)
            ac.traceDone.connect(cprog.traceDoneSlot)
            ac.traceDone.connect(self.glitchMonitor.traceDone)
            self.glitchMonitor.campaignStart(baseprefix)
            ac.setMaxtraces(tracesPerRun)
            cprog.abortCapture.connect(ac.abortCapture)

            self.capture1Act.setEnabled(False)
            self.captureMAct.setEnabled(False)

            ac.doReadings(addToList=self.manageTraces)

            tcnt += tracesPerRun
            self.statusBar().showMessage("%d Captures Completed" % tcnt)
            self.glitchMonitor.campaignDone()

            stoptime = datetime.now()

            # Re-use the wave buffer for later segments
            if writer is not None:
                waveBuffer = writer.traces
                writerlist.append(writer)

        self.console.append("Capture delta time: %s" % str(stoptime - overallstarttime))

        self.capture1Act.setEnabled(True)
        self.captureMAct.setChecked(False)
        self.captureMAct.setEnabled(True)

        return writerlist

    def scopeChanged(self, newscope):
        self.scope = newscope
        global_mod.active_scope = newscope
        if self.scope is not None:
            self.scope.paramListUpdated.connect(self.reloadScopeParamList)
            self.scope.dataUpdated.connect(self.newScopeData)
            self.scope.connectStatus.connect(self.scopeStatusChanged)
            self.reloadScopeParamList()
            self.ScopeToolbar.setEnabled(True)
        else:
            self.ScopeToolbar.setEnabled(False)

    def traceChanged(self, newtrace):
        self.trace = newtrace
        try:
            self.traceparams = self.trace.getParams
        except AttributeError:
            self.traceparams = None
        except TypeError:
            self.traceparams = None

        self.reloadTraceParamList()

    def auxChanged(self, newaux):
        self.auxList = [newaux]
        try:
            newaux.paramList()
            newaux.paramListUpdated.connect(self.reloadAuxParamList)
            self.reloadAuxParamList()
            self.settingsAuxDock.setVisible(True)
        except AttributeError:
            self.settingsAuxDock.setVisible(False)
        except TypeError:
            self.settingsAuxDock.setVisible(False)

    def newProject(self):
        self.setProject(ProjectFormat(self))
        self.project().setProgramName("ChipWhisperer-Capture")
        self.project().setProgramVersion("2.00")
        self.project().addParamTree(self)
        self.project().addParamTree(self.scope)
        self.project().addParamTree(self.target)
        self.project().setTraceManager(self.manageTraces)
        self.setCurrentFile(None)

    def saveProject(self):
        if self.project().hasFilename() == False:
            fd = QFileDialog(self, 'Save New File', '.', '*.cwp')
            fd.setDefaultSuffix('cwp')
            fd.setAcceptMode(QFileDialog.AcceptSave)
            fd.setViewMode(QFileDialog.Detail)
            if fd.exec_() == QDialog.Accepted:
                fname = fd.selectedFiles()[0]
            else:
                return

            self.project().setFilename(fname)
            self.setCurrentFile(fname)

        self.project().save()
        self.dirty = False
        self.updateTitleBar()
        self.statusBar().showMessage("Project Saved")

def main(scriptDir="."):
    app = makeApplication()

    # Create and show the form
    window = ChipWhispererCapture(scriptDir)
    window.show()

    # Run the main Qt loop
    sys.exit(app.exec_())


def makeApplication():
    # Create the Qt Application
    app = QApplication(sys.argv)
    app.setOrganizationName("ChipWhisperer")
    app.setApplicationName("Capture V2")
    return app

if __name__ == '__main__':
    main()

#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2013-2016, NewAE Technology Inc
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

import traceback
import sys
from chipwhisperer.common.api.ProjectFormat import ProjectFormat
from chipwhisperer.common.utils import Util, pluginmanager
from chipwhisperer.common.ui.ProgressBar import *
from chipwhisperer.capture.api.AcquisitionController import AcquisitionController
from chipwhisperer.capture.acq_patterns.basic import AcqKeyTextPattern_Basic
from chipwhisperer.common.traces.TraceContainerNative import TraceContainerNative
from chipwhisperer.capture.ui.EncryptionStatusMonitor import EncryptionStatusMonitor
from chipwhisperer.common.api.ExtendedParameter import ExtendedParameter
from PySide.QtGui import *
from pyqtgraph.parametertree import ParameterTree


class CWCoreAPI(pluginmanager.Parameterized):
    __name__ = "ChipWhisperer"
    __organization__ = "NewAE Technology Inc."
    __version__ = "V3.0"
    name = 'Generic Settings'
    instance = None

    class Signals(object):
        def __init__(self):
            self.newProject = Util.Signal()
            self.newScopeData = Util.Signal()
            self.connectStatus = Util.Signal()
            self.acqPatternChanged = Util.Signal()
            self.newInputData = Util.Signal()
            self.newTextResponse = Util.Signal()
            self.traceDone = Util.Signal()
            self.campaignStart = Util.Signal()
            self.campaignDone = Util.Signal()

    def __init__(self):
        super(CWCoreAPI, self).__init__()
        self.signals = self.Signals()
        CWCoreAPI.instance = self
        self._helpWidget = None
        self.generalParamTree = ParameterTree()
        self.generalParamTree.setParameters(self.params, showTop=False)
        self.scopeParamTree = ParameterTree()
        self.targetParamTree = ParameterTree()
        self.traceParamTree = ParameterTree()
        self.auxParamTree = ParameterTree()
        self.attackParamTree = ParameterTree()
        self.paramTrees = [self.generalParamTree, self.scopeParamTree, self.targetParamTree,
                           self.traceParamTree, self.auxParamTree, self.attackParamTree]
        self.reloadGeneralParamList()
        self.reloadScopeParamList()
        self.reloadTargetParamList()
        self.reloadTraceFormatParamList()
        self.reloadAuxParamList()
        self.reloadAttackParamList()
        self.paramListUpdated.connect(self.reloadGeneralParamList)

    def setHelpWidget(self, widget):
        self._helpWidget = widget

    def getParamList(self, parametrized):
        return parametrized.paramList() if parametrized else []

    def reloadGeneralParamList(self):
        ExtendedParameter.reloadParams(self.getParamList(self), self.generalParamTree, help_window=self._helpWidget)

    def reloadScopeParamList(self):
        ExtendedParameter.reloadParams(self.getParamList(self.getScope()), self.scopeParamTree, help_window=self._helpWidget)

    def reloadTargetParamList(self):
        ExtendedParameter.reloadParams(self.getParamList(self.getTarget()), self.targetParamTree, help_window=self._helpWidget)

    def reloadTraceFormatParamList(self):
        ExtendedParameter.reloadParams(self.getParamList(self.getTraceFormat()), self.traceParamTree, help_window=self._helpWidget)

    def reloadAuxParamList(self):
        ExtendedParameter.reloadParams(self.getParamList(self.getAuxList()[0]), self.auxParamTree, help_window=self._helpWidget)

    def reloadAttackParamList(self):
        ExtendedParameter.reloadParams(self.getParamList(self.getAttack()), self.attackParamTree, help_window=self._helpWidget)

    def setupParameters(self):
        self._project = None
        self._scope = None
        self._target = None
        self._attack = None
        self._traceFormat = TraceContainerNative()
        self._acqPattern = AcqKeyTextPattern_Basic(self)
        self._auxList = [None]
        self._numTraces = 100
        self._numTraceSets = 1
        self.results = []
        self.setupChildParamsOrder([lambda: self._acqPattern])
        self.encryptionStatusMonitor = None

        valid_scopes = pluginmanager.getPluginsInDictFromPackage("chipwhisperer.capture.scopes", True, True)
        valid_targets =  pluginmanager.getPluginsInDictFromPackage("chipwhisperer.capture.targets", True, True)
        valid_traces = pluginmanager.getPluginsInDictFromPackage("chipwhisperer.common.traces", True, True)
        valid_aux = pluginmanager.getPluginsInDictFromPackage("chipwhisperer.capture.auxiliary", True, True)
        valid_acqPatterns =  pluginmanager.getPluginsInDictFromPackage("chipwhisperer.capture.acq_patterns", True, False, self)

        return [
                {'name':'Scope Module', 'key':'scopeMod', 'type':'list', 'values':valid_scopes, 'value':None, 'set':self.setScope, 'get':self.getScope},
                {'name':'Target Module', 'key':'targetMod', 'type':'list', 'values':valid_targets, 'value':"None", 'set':self.setTarget, 'get':self.getTarget},
                {'name':'Trace Format', 'type':'list', 'values':valid_traces, 'value':self.getTraceFormat(), 'set':self.setTraceFormat},
                {'name':'Auxiliary Module', 'type':'list', 'values':valid_aux, 'value':self.getAuxList()[0], 'set':self.setAux},

                # {'name':'Key Settings', 'type':'group', 'children':[
                #        {'name':'Encryption Key', 'type':'str', 'value':self.textkey, 'set':self.setKey},
                #        {'name':'Send Key to Target', 'type':'bool', 'value':True},
                #        {'name':'New Encryption Key/Trace', 'key':'newKeyAlways', 'type':'bool', 'value':False},
                #    ]},

                {'name':'Acquisition Settings', 'type':'group', 'children':[
                        {'name':'Number of Traces', 'type':'int', 'limits':(1, 1E9), 'value':self.numTraces(), 'set':self.setNumTraces, 'get':self.numTraces, 'linked':['Traces per Set']},
                        {'name':'Number of Sets', 'type':'int', 'limits':(1, 1E6), 'value':self.numTraceSets(), 'set':self.setNumTraceSets, 'get':self.numTraceSets, 'linked':['Traces per Set'], 'tip': 'Break api into N set, '
                         'which may cause data to be saved more frequently. The default capture driver requires that NTraces/NSets is small enough to avoid running out of system memory '
                         'as each segment is buffered into RAM before being written to disk.'}, #TODO: tip is not working
                        {'name':'Traces per Set', 'type':'int', 'value':self.tracesPerSet(), 'readonly':True, 'get':self.tracesPerSet},
                        {'name':'Open Monitor', 'type':'action', 'action':lambda: self.encryptionStatusMonitor.show()},
                        {'name':'Key/Text Pattern', 'type':'list', 'values':valid_acqPatterns, 'value':self.getAcqPattern, 'set':self.setAcqPattern},
                    ]},
                ]

    def getScope(self):
        return self._scope

    def setScope(self, driver):
        if self.getScope(): self.getScope().dis()
        self._scope = driver
        if self.getScope():
            self.getScope().paramListUpdated.connect(self.reloadScopeParamList)
            self.getScope().dataUpdated.connect(self.signals.newScopeData.emit)
            self.getScope().connectStatus.connect(self.signals.connectStatus)
        self.reloadScopeParamList()

    def getTarget(self):
        return self._target

    def setTarget(self, driver):
        if self.getTarget(): self.getTarget().dis()
        self._target = driver
        if self.getTarget():
            self.getTarget().paramListUpdated.connect(self.reloadTargetParamList)
            self.getTarget().newInputData.connect(self.signals.newInputData.emit)
            self.getTarget().connectStatus.connect(self.signals.connectStatus)
        self.reloadTargetParamList()

    def setAux(self, aux):
        self._auxList = [aux]
        if self.getAuxList():
            self.getAuxList()[0].paramListUpdated.connect(self.reloadAuxParamList)
        self.reloadAuxParamList()

    def getAuxList(self):
        return self._auxList

    def setAcqPattern(self, pat):
        self._acqPattern = pat
        self.paramListUpdated.emit()

    def getAcqPattern(self):
        return self._acqPattern

    def connectScope(self):
        try:
            self.getScope().con()
        except Warning:
            sys.excepthook(*sys.exc_info())
            return False
        return True

    def connectTarget(self):
        try:
            self.getTarget().con(scope = self.getScope())
        except Warning:
            sys.excepthook(*sys.exc_info())
            return False
        return True

    def doConDis(self):
        """DEPRECATED: Is here just for compatibility reasons"""
        print "Method doConDis() is deprecated... use connect() or disconnect() instead"
        return self.connect()

    def connect(self):
        return self.connectScope() and self.connectTarget()

    def disconnectScope(self):
        self.getScope().dis()

    def disconnectTarget(self):
        self.getTarget().dis()

    def disconnect(self):
        self.disconnectScope()
        self.disconnectTarget()

    def numTraces(self):
        return self._numTraces

    def setNumTraces(self, t):
        self._numTraces = t

    def numTraceSets(self):
        return self._numTraceSets

    def setNumTraceSets(self, s):
        self._numTraceSets = s

    def tracesPerSet(self):
        return int(self._numTraces / self._numTraceSets)

    def capture1(self):
        """Captures only one trace"""
        try:
            ac = AcquisitionController(self.getScope(), self.getTarget(), writer=None, auxList=self._auxList, keyTextPattern=self.getAcqPattern())
            ac.signals.newTextResponse.connect(self.signals.newTextResponse.emit)
            ac.doSingleReading()
        except Warning:
            sys.excepthook(*sys.exc_info())
            return False
        return True

    def captureM(self, progressBar = None):
        """Captures multiple traces and saves it in the Trace Manager"""
        if not progressBar: progressBar = ProgressBarText()

        writerlist = []
        with progressBar:
            progressBar.setStatusMask("Current Segment = %d Current Trace = %d")
            progressBar.setMaximum(self._numTraces - 1)

            waveBuffer = None
            tcnt = 0
            setSize = self.tracesPerSet()
            for i in range(0, self._numTraceSets):
                if progressBar.wasAborted(): break
                currentTrace = self._traceFormat.clone()

                # Load trace writer information
                starttime = datetime.now()
                baseprefix = starttime.strftime('%Y.%m.%d-%H.%M.%S')
                prefix = baseprefix + "_"
                currentTrace.config.setConfigFilename(self.project().datadirectory + "traces/config_" + prefix + ".cfg")
                currentTrace.config.setAttr("prefix", prefix)
                currentTrace.config.setAttr("date", starttime.strftime('%Y-%m-%d %H:%M:%S'))
                currentTrace.config.setAttr("targetHW", self.getTarget().getName())
                currentTrace.config.setAttr("targetSW", "unknown")
                currentTrace.config.setAttr("scopeName", self.getScope().getName())
                currentTrace.config.setAttr("scopeSampleRate", 0)
                currentTrace.config.setAttr("notes", "Aux: " + ', '.join(w.getName() for w in self._auxList))
                currentTrace.setTraceHint(setSize)

                if waveBuffer is not None:
                    currentTrace.setTraceBuffer(waveBuffer)

                if self._auxList is not None:
                    for aux in self._auxList:
                        aux.setPrefix(baseprefix)

                ac = AcquisitionController(self.getScope(), self.getTarget(), currentTrace, self._auxList, self.getAcqPattern())
                ac.setMaxtraces(setSize)
                ac.signals.newTextResponse.connect(self.signals.newTextResponse.emit)
                ac.signals.traceDone.connect(self.signals.traceDone.emit)
                ac.signals.traceDone.connect(lambda: progressBar.updateStatus(i*setSize + ac.currentTrace, (i, ac.currentTrace)))
                ac.signals.traceDone.connect(lambda: ac.abortCapture(progressBar.wasAborted()))

                self.signals.campaignStart.emit(baseprefix)
                ac.doReadings(tracesDestination=self.project().traceManager())
                self.signals.campaignDone.emit()
                tcnt += setSize

                waveBuffer = currentTrace.traces   # Re-use the wave buffer for later segments to avoid memory realocation
                writerlist.append(currentTrace)

                if progressBar.wasAborted():
                    break

        return writerlist

    def project(self):
        return self._project

    def setProject(self, proj):
        self._project = proj
        self.signals.newProject.emit()

    def newProject(self):
        self.setProject(ProjectFormat())
        self.project().setProgramName(self.__name__)
        self.project().setProgramVersion(self.__version__)
        self.project().addParamTree(self)
        # self.project().addParamTree(self.getScope())
        # self.project().addParamTree(self.getTarget())

    def openProject(self, fname):
        self.newProject()
        self.project().load(fname)

    def saveProject(self, fname):
        self.project().setFilename(fname)
        self.project().save()

    def getTraceFormat(self):
        return self._traceFormat

    def setTraceFormat(self, format):
        self._traceFormat = format
        if self.getTraceFormat():
            self.getTraceFormat().paramListUpdated.connect(self.reloadTraceFormatParamList)
        self.reloadTraceFormatParamList()

    def getAttack(self):
        return self._attack

    def setAttack(self, attack):
        """Set the attack module, reloading GUI and connecting appropriate signals"""
        self._attack = attack
        if self.getAttack():
            self.getAttack().paramListUpdated.connect(self.reloadAttackParamList)
            self.getAttack().setTraceLimits(self.project().traceManager().numTraces(), self.project().traceManager().numPoints())
        self.reloadAttackParamList()

    def doAttack(self, mod, progressBar = None):
        """Called when the 'Do Attack' button is pressed, or can be called via API to cause attack to run"""
        if not progressBar: progressBar = ProgressBar()

        with progressBar:
            mod.initProject()
            mod.initPreprocessing()
            mod.initAnalysis()
            mod.initReporting(self.results)
            mod.doAnalysis(progressBar)
            mod.doneAnalysis()
            mod.doneReporting()

    def _setParameter_children(self, top, path, value, echo):
        """Descends down a given path, looking for value to set"""
        if top.name() == path[0]:
            if len(path) > 1:
                for c in top.children():
                    self._setParameter_children(c, path[1:], value, echo)
            else:
                #Check if this is a dictionary/list
                if "values" in top.opts:
                    try:
                        if isinstance(top.opts["values"], dict):
                            value = top.opts["values"][value]
                    except TypeError:
                        pass
                    except KeyError:
                        print "Parameter value %s is invalid. Check if it is spelled correctly or if the module was loaded properly" % value

                if echo == False:
                    top.opts["echooff"] = True

                if top.opts["type"] == "action":
                    top.activate()
                else:
                    top.setValue(value)

                raise ValueError()

    def setParameter(self, parameter, echo=False):
        """Sets a parameter based on a list, used for scripting in combination with showScriptParameter"""
        path = parameter[:-1]
        value = parameter[-1]

        try:
            for t in self.paramTrees:
                for i in range(0, t.invisibleRootItem().childCount()):
                    self._setParameter_children(t.invisibleRootItem().child(i).param, path, value, echo)

            print "Parameter not found: %s" % str(parameter)
        except ValueError:
            #A little klunky: we use exceptions to tell us the system DID work as intended
            pass
        except IndexError:
            raise IndexError("IndexError Setting Parameter %s\n%s" % (str(parameter), traceback.format_exc()))

        self.paramListUpdated.emit()

    def guiActions(self, mainWindow):
        self.encryptionStatusMonitor = EncryptionStatusMonitor(mainWindow)
        self.signals.newTextResponse.connect(self.encryptionStatusMonitor.newData)
        return [['Encryption Status Monitor','', self.encryptionStatusMonitor.show]]


    @staticmethod
    def getInstance():
        return CWCoreAPI.instance

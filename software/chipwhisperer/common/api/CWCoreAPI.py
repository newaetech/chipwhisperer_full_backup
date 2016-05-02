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
import copy

from chipwhisperer.common.utils.parameters import Parameterized, CWParameterTree
from chipwhisperer.common.api.ProjectFormat import ProjectFormat
from chipwhisperer.common.utils import util, pluginmanager
from chipwhisperer.common.ui.ProgressBar import *
from chipwhisperer.capture.api.AcquisitionController import AcquisitionController
from chipwhisperer.capture.ui.EncryptionStatusMonitor import EncryptionStatusMonitor
from chipwhisperer.common.utils.tracesource import TraceSource, LiveTraceSource


class CWCoreAPI(Parameterized):
    __name__ = "ChipWhisperer"
    __organization__ = "NewAE Technology Inc."
    __version__ = "V3.0"
    name = 'Generic Settings'
    instance = None

    def __init__(self):
        super(CWCoreAPI, self).__init__()
        self.sigNewProject = util.Signal()
        self.sigNewScopeData = util.Signal()
        self.sigConnectStatus = util.Signal()
        self.sigAttackChanged = util.Signal()
        self.sigNewInputData = util.Signal()
        self.sigNewTextResponse = util.Signal()
        self.sigTraceDone = util.Signal()
        self.sigCampaignStart = util.Signal()
        self.sigCampaignDone = util.Signal()
        self.sigTracesChanged = util.Signal()

        CWCoreAPI.instance = self

        LiveTraceSource().registerAs("Scope (Live)")

        self.valid_scopes = pluginmanager.getPluginsInDictFromPackage("chipwhisperer.capture.scopes", True, True)
        self.valid_targets =  pluginmanager.getPluginsInDictFromPackage("chipwhisperer.capture.targets", True, True)
        self.valid_traces = pluginmanager.getPluginsInDictFromPackage("chipwhisperer.common.traces", True, True)
        self.valid_aux = pluginmanager.getPluginsInDictFromPackage("chipwhisperer.capture.auxiliary", True, True)
        self.valid_acqPatterns =  pluginmanager.getPluginsInDictFromPackage("chipwhisperer.capture.acq_patterns", True, False, self)
        self.valid_attacks = pluginmanager.getPluginsInDictFromPackage("chipwhisperer.analyzer.attacks", True, False)
        self.resultWidgets = util.DictType()
        self.valid_preprocessingModules = pluginmanager.getPluginsInDictFromPackage("chipwhisperer.analyzer.preprocessing", False, True, self)

        self._project = self._scope = self._target = self._attack =  self._traceManager = self._acqPattern = None
        self._auxList = [None]  # TODO: implement it as a list in the whole class
        self._numTraces = 100
        self._numTraceSets = 1

        self.params.addChildren([
            {'name':'Scope Module', 'key':'scopeMod', 'type':'list', 'values':self.valid_scopes, 'value':self._scope, 'set':self.setScope, 'get':self.getScope},
            {'name':'Target Module', 'key':'targetMod', 'type':'list', 'values':self.valid_targets, 'value':self._target, 'set':self.setTarget, 'get':self.getTarget},
            {'name':'Trace Format', 'type':'list', 'values':self.valid_traces, 'value':self._traceManager, 'set':self.setTraceFormat},
            {'name':'Auxiliary Module', 'type':'list', 'values':self.valid_aux, 'value':self.getAuxList()[0], 'set':self.setAux},
            {'name':'Acquisition Settings', 'type':'group', 'children':[
                    {'name':'Number of Traces', 'type':'int', 'limits':(1, 1E9), 'value':self.numTraces(), 'set':self.setNumTraces, 'get':self.numTraces, 'linked':['Traces per Set']},
                    {'name':'Number of Sets', 'type':'int', 'limits':(1, 1E6), 'value':self.numTraceSets(), 'set':self.setNumTraceSets, 'get':self.numTraceSets, 'linked':['Traces per Set'], 'tip': 'Break api into N set, '
                     'which may cause data to be saved more frequently. The default capture driver requires that NTraces/NSets is small enough to avoid running out of system memory '
                     'as each segment is buffered into RAM before being written to disk.'}, #TODO: tip is not working
                    {'name':'Traces per Set', 'type':'int', 'value':self.tracesPerSet(), 'readonly':True, 'get':self.tracesPerSet},
                    {'name':'Key/Text Pattern', 'type':'list', 'values':self.valid_acqPatterns, 'value':self._acqPattern, 'set':self.setAcqPattern},
                                                                        ]},
                                 ])

        self.graphWidget = None

        self.generalParamTree = CWParameterTree("General Settings", [self])
        self.addActiveParams(lambda: self.lazy(self._acqPattern))
        self.resultsParamTree = CWParameterTree("Results", [v for v in self.resultWidgets.itervalues()])
        self.scopeParamTree = CWParameterTree("Scope Settings", [self.getScope()])
        self.targetParamTree = CWParameterTree("Target Settings", [self.getTarget()])
        self.traceParamTree = CWParameterTree("Trace Settings", [self.getTraceFormat()])
        self.auxParamTree = CWParameterTree("Aux Settings", self.getAuxList())
        self.attackParamTree = CWParameterTree("Attack Settings", [self.getAttack()])

        # Initialize default values
        #self.setScope(self.valid_scopes.get("ChipWhisperer/OpenADC", None))
        #self.setTarget(self.valid_targets.get("Simple Serial", None))
        self.setAttack(self.valid_traces.get("CPA", None))
        self.setAcqPattern(self.valid_acqPatterns.get("Basic", None))

        self.newProject()

    def getGraphWidget(self):
        return self.graphWidget

    def allGuiActions(self, mainWindow):
        ret = self.guiActions(mainWindow)
        if self.getScope(): ret.extend(self.getScope().guiActions(mainWindow))
        if self.getTarget(): ret.extend(self.getTarget().guiActions(mainWindow))
        if self.getTraceFormat(): ret.extend(self.getTraceFormat().guiActions(mainWindow))
        if self.getAuxList()[0]: ret.extend(self.getAuxList()[0].guiActions(mainWindow))
        if self.getAttack(): ret.extend(self.getAttack().guiActions(mainWindow))
        return ret

    def addResultWidgets(self, widgets):
        self.resultWidgets.update(widgets)
        self.resultsParamTree.extend([v for v in widgets.itervalues()])
        self.setupResultWidgets()

    def setupResultWidgets(self):
        # [v.setTraceSource(self.project().traceManager()) for v in self.resultWidgets.itervalues() if hasattr(v, "setTraceSource")]
        [v.setAnalysisSource(self.getAttack()) for v in self.resultWidgets.itervalues() if hasattr(v, "setAnalysisSource")]

    def getScope(self):
        return self._scope

    def setScope(self, driver):
        if self.getScope(): self.getScope().dis()
        self._scope = driver
        if self.getScope():
            self.getScope().dataUpdated.connect(self.sigNewScopeData.emit)
            self.getScope().connectStatus.connect(self.sigConnectStatus.emit)
            TraceSource.registeredObjects["Scope (Live)"].setScope(self.getScope())
        self.scopeParamTree.replace([self.getScope()])

    def getTarget(self):
        return self._target

    def setTarget(self, driver):
        if self.getTarget(): self.getTarget().dis()
        self._target = driver
        if self.getTarget():
            self.getTarget().newInputData.connect(self.sigNewInputData.emit)
            self.getTarget().connectStatus.connect(self.sigConnectStatus.emit)
        self.targetParamTree.replace([self.getTarget()])

    def getAuxList(self):
        return self._auxList

    def setAux(self, aux):
        self._auxList = [aux]
        self.auxParamTree.replace([self.getAuxList()])

    def getAcqPattern(self):
        return self._acqPattern

    def setAcqPattern(self, pat):
        self._acqPattern = pat
        self.paramListUpdated.emit()

    def getTraceFormat(self):
        return self._traceManager

    def setTraceFormat(self, format):
        self._traceManager = format
        self.traceParamTree.replace([self.getTraceFormat()])

    def getAttack(self):
        return self._attack

    def setAttack(self, attack):
        self._attack = attack
        if self.getAttack():
            self.getAttack().setTraceLimits(self.project().traceManager().numTraces(), self.project().traceManager().numPoints())
        self.attackParamTree.replace([self.getAttack()])
        self.sigAttackChanged.emit()
        self.setupResultWidgets()

    def project(self):
        return self._project

    def setProject(self, proj):
        self._project = proj
        self.sigNewProject.emit()

    def newProject(self):
        self.setProject(ProjectFormat())
        self.project().setProgramName(self.__name__)
        self.project().setProgramVersion(self.__version__)
        self.project().addParamTree(self)
        # self.project().addParamTree(self.getScope())
        # self.project().addParamTree(self.getTarget())
        self.project().traceManager().sigTracesChanged.connect(self.sigTracesChanged.emit)
        self.project().traceManager().registerAs("Trace Manager")
        self.setupResultWidgets()

    def openProject(self, fname):
        self.newProject()
        self.project().load(fname)

    def saveProject(self, fname):
        self.project().setFilename(fname)
        self.project().save()

    def connectScope(self):
        try:
            if self.getScope():
                self.getScope().con()
        except Warning:
            sys.excepthook(*sys.exc_info())
            return False
        return True

    def disconnectScope(self):
        self.getScope().dis()

    def connectTarget(self):
        try:
            if self.getTarget():
                self.getTarget().con(scope=self.getScope())
        except Warning:
            sys.excepthook(*sys.exc_info())
            return False
        return True

    def disconnectTarget(self):
        self.getTarget().dis()

    def doConDis(self):
        """DEPRECATED: It is here just for compatibility reasons"""
        print "Method doConDis() is deprecated... use connect() or disconnect() instead"
        return self.connect()

    def connect(self):
        return self.connectScope() and self.connectTarget()

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
            ac.sigNewTextResponse.connect(self.sigNewTextResponse.emit)
            return ac.doSingleReading()
        except Warning:
            sys.excepthook(*sys.exc_info())
            return False

    def captureM(self, progressBar = None):
        """Captures multiple traces and saves it in the Trace Manager"""
        if not progressBar: progressBar = ProgressBarText()

        with progressBar:
            progressBar.setStatusMask("Current Segment = %d Current Trace = %d")
            progressBar.setMaximum(self._numTraces - 1)

            waveBuffer = None
            tcnt = 0
            setSize = self.tracesPerSet()
            for i in range(0, self._numTraceSets):
                if progressBar.wasAborted(): break
                currentTrace = copy.copy(self._traceManager)
                currentTrace.clear()

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
                # currentTrace.config.setAttr("notes", "Aux: " + ', '.join(str(self._auxList)))
                currentTrace.setTraceHint(setSize)

                if waveBuffer:
                    currentTrace.setTraceBuffer(waveBuffer)

                if self._auxList:
                    for aux in self._auxList:
                        if aux:
                            aux.setPrefix(baseprefix)

                ac = AcquisitionController(self.getScope(), self.getTarget(), currentTrace, self._auxList, self.getAcqPattern())
                ac.setMaxtraces(setSize)
                ac.sigNewTextResponse.connect(self.sigNewTextResponse.emit)
                ac.sigTraceDone.connect(self.sigTraceDone.emit)
                ac.sigTraceDone.connect(lambda: progressBar.updateStatus(i*setSize + ac.currentTrace, (i, ac.currentTrace)))
                ac.sigTraceDone.connect(lambda: ac.abortCapture(progressBar.wasAborted()))

                self.sigCampaignStart.emit(baseprefix)
                ac.doReadings(tracesDestination=self.project().traceManager())
                self.sigCampaignDone.emit()
                tcnt += setSize

                waveBuffer = currentTrace.traces   # Re-use the wave buffer for later segments to avoid memory realocation

                if progressBar.wasAborted():
                    break

    def runScriptModule(self, mod, funcName="run"):
        try:
            classes = pluginmanager.getPluginClassesFromModules([mod])
            if len(classes) == 0:
                raise Warning("No UserScriptBase class found")
            for c in classes:
                self.runScriptClass(c, funcName)
        except Exception as e:
            sys.excepthook(Warning, "Could not execute Script Module %s: %s" % (str(mod), e.message), "")

    def runScriptClass(self, scriptClass, funcName="run"):
        try:
            m = scriptClass(self)
            if funcName is not None:
                eval('m.%s()' % funcName)
        except Exception as e:
                if funcName == 'TraceExplorerDialog_PartitionDisplay_findPOI':
                    return
                sys.excepthook(Warning, "Could not execute method %s in script class %s: %s" % (funcName, scriptClass.__name__, e.message), "")

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
            for t in CWParameterTree.getAllParameterTrees().itervalues():
                for i in range(0, t.invisibleRootItem().childCount()):
                    self._setParameter_children(t.invisibleRootItem().child(i).param, path, value, echo)

            print "Parameter not found: %s" % str(parameter)
        except ValueError:
            #A little klunky: we use exceptions to tell us the system DID work as intended
            pass
        except IndexError:
            raise IndexError("IndexError Setting Parameter %s\n%s" % (str(parameter), traceback.format_exc()))

        self.paramListUpdated.emit()

    def setupGuiActions(self, mainWindow):
        if not hasattr(self, 'encryptionStatusMonitor'):
            self.encryptionStatusMonitor = EncryptionStatusMonitor(mainWindow)
            self.sigNewTextResponse.connect(self.encryptionStatusMonitor.newData)
        return [['Encryption Status Monitor','', self.encryptionStatusMonitor.show]]


    @staticmethod
    def getInstance():
        return CWCoreAPI.instance

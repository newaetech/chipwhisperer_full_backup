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

import copy
import traceback
import sys
from chipwhisperer.capture.api.AcquisitionController import AcquisitionController
from chipwhisperer.common.api.ProjectFormat import ProjectFormat
from chipwhisperer.common.results.base import ResultsBase
from chipwhisperer.common.ui.ProgressBar import *
from chipwhisperer.common.utils import util, pluginmanager
from chipwhisperer.common.utils.parameter import Parameterized, Parameter, setupSetParam
from chipwhisperer.common.utils.tracesource import TraceSource


class CWCoreAPI(Parameterized):
    __name__ = "ChipWhisperer"
    __organization__ = "NewAE Technology Inc."
    __version__ = "V3.1"
    _name = 'Generic Settings'
    instance = None

    def __init__(self):
        CWCoreAPI.instance = self
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

        self.valid_scopes = pluginmanager.getPluginsInDictFromPackage("chipwhisperer.capture.scopes", True, True)
        self.valid_targets =  pluginmanager.getPluginsInDictFromPackage("chipwhisperer.capture.targets", True, True)
        self.valid_traces = pluginmanager.getPluginsInDictFromPackage("chipwhisperer.common.traces", True, True)
        self.valid_aux = pluginmanager.getPluginsInDictFromPackage("chipwhisperer.capture.auxiliary", True, True)
        self.valid_acqPatterns =  pluginmanager.getPluginsInDictFromPackage("chipwhisperer.capture.acq_patterns", True, False, self)
        self.valid_attacks = pluginmanager.getPluginsInDictFromPackage("chipwhisperer.analyzer.attacks", True, False)
        self.valid_preprocessingModules = pluginmanager.getPluginsInDictFromPackage("chipwhisperer.analyzer.preprocessing", False, True, self)

        # Initialize default values
        self._project = self._scope = self._target = self._attack =  self._traceFormat = self._acqPattern = None
        self._attack = self.valid_attacks.get("CPA", None)
        self._acqPattern = self.valid_acqPatterns["Basic"]
        self._auxList = [None]  # TODO: implement it as a list in the whole class
        self._numTraces = 50
        self._numTraceSets = 1

        self.params = Parameter(name='Generic Settings', type='group').register()
        self.params.addChildren([
            {'name':'Scope Module', 'key':'scopeMod', 'type':'list', 'values':self.valid_scopes, 'get':self.getScope, 'set':self.setScope},
            {'name':'Target Module', 'key':'targetMod', 'type':'list', 'values':self.valid_targets, 'get':self.getTarget, 'set':self.setTarget},
            {'name':'Trace Format', 'type':'list', 'values':self.valid_traces, 'get':self.getTraceFormat, 'set':self.setTraceFormat},
            {'name':'Auxiliary Module', 'type':'list', 'values':self.valid_aux, 'get':lambda: self.getAuxList()[0], 'set':self.setAux},
            {'name':'Acquisition Settings', 'type':'group', 'children':[
                    {'name':'Number of Traces', 'type':'int', 'limits':(1, 1E9), 'get':self.getNumTraces, 'set':self.setNumTraces, 'linked':['Traces per Set']},
                    {'name':'Number of Sets', 'type':'int', 'limits':(1, 1E6), 'get':self.getNumTraceSets, 'set':self.setNumTraceSets, 'linked':['Traces per Set'], 'tip': 'Break acquisition into N sets, '
                     'which may cause data to be saved more frequently. The default capture driver requires that NTraces/NSets is small enough to avoid running out of system memory '
                     'as each segment is buffered into RAM before being written to disk.'},
                    {'name':'Traces per Set', 'type':'int', 'readonly':True, 'get':self.tracesPerSet},
                    {'name':'Key/Text Pattern', 'type':'list', 'values':self.valid_acqPatterns, 'get':self.getAcqPattern, 'set':self.setAcqPattern},
            ]},
        ])
        self.scopeParam = Parameter(name="Scope Settings", type='group')
        self.params.getChild('Scope Module').stealDynamicParameters(self.scopeParam)

        self.targetParam = Parameter(name="Target Settings", type='group')
        self.params.getChild('Target Module').stealDynamicParameters(self.targetParam)

        self.traceParam = Parameter(name="Trace Settings", type='group')
        self.params.getChild('Trace Format').stealDynamicParameters(self.traceParam)

        self.auxParam = Parameter(name="Aux Settings", type='group')
        self.params.getChild('Auxiliary Module').stealDynamicParameters(self.auxParam)

        # self.attackParam = Parameter(name="Attack Settings", type='group')
        # self.params.getChild('Attack Module').getDynamicParameters(self.attackParam)

        self.newProject()

    def getResults(self, name):
        return ResultsBase.registeredObjects[name]

    def getScope(self):
        return self._scope

    @setupSetParam("Scope Module")
    def setScope(self, driver):
        if self.getScope():
            self.getScope().dis()
        self._scope = driver
        if self.getScope():
            self.getScope().dataUpdated.connect(self.sigNewScopeData.emit)
            self.getScope().connectStatus.connect(self.sigConnectStatus.emit)

    def getTarget(self):
        return self._target

    @setupSetParam("Target Module")
    def setTarget(self, driver):
        if self.getTarget(): self.getTarget().dis()
        self._target = driver
        if self.getTarget():
            self.getTarget().newInputData.connect(self.sigNewInputData.emit)
            self.getTarget().connectStatus.connect(self.sigConnectStatus.emit)

    def getAuxList(self):
        return self._auxList

    @setupSetParam("Auxiliary Module")
    def setAux(self, aux):
        self._auxList = [aux]

    def getAcqPattern(self):
        return self._acqPattern

    @setupSetParam("Key/Text Pattern")
    def setAcqPattern(self, pat):
        self._acqPattern = pat
        if self._acqPattern is not None:
            self._acqPattern.getParams().remove()
        self.getParams().append(self._acqPattern.getParams())

    def getNewTrace(self, format):
        if format is None:
            raise Warning("No trace format selected.")
        tmp = copy.copy(format)
        tmp.clear()
        starttime = datetime.now()
        prefix = starttime.strftime('%Y.%m.%d-%H.%M.%S') + "_"
        tmp.config.setConfigFilename(CWCoreAPI.getInstance().project().datadirectory + "traces/config_" + prefix + ".cfg")
        tmp.config.setAttr("prefix", prefix)
        tmp.config.setAttr("date", starttime.strftime('%Y-%m-%d %H:%M:%S'))
        return tmp

    def getTraceFormat(self):
        return self._traceFormat

    @setupSetParam("Trace Format")
    def setTraceFormat(self, format):
        self._traceFormat = format

    def getAttack(self):
        return self._attack

    def setAttack(self, attack):
        self._attack = attack
        if self.getAttack():
            self.getAttack().setTraceLimits(self.project().traceManager().numTraces(), self.project().traceManager().numPoints())
        self.sigAttackChanged.emit()

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
                # Sets the Plot Widget to the last added TraceSource
                try:
                    ResultsBase.registeredObjects["Trace Output Plot"].setTraceSource(
                        TraceSource.registeredObjects[next(reversed(TraceSource.registeredObjects))])
                except KeyError:
                    pass

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

    def getNumTraces(self):
        return self._numTraces

    @setupSetParam("Number of Traces")
    def setNumTraces(self, t):
        self._numTraces = t

    def getNumTraceSets(self):
        return self._numTraceSets

    @setupSetParam("Number of Sets")
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
                currentTrace = self.getNewTrace(self.getTraceFormat())

                # Load trace writer information
                prefix = currentTrace.config.attr("prefix")
                currentTrace.config.setAttr("targetHW", self.getTarget().getName() if self.getTarget() is not None else "None")
                currentTrace.config.setAttr("targetSW", "unknown")
                currentTrace.config.setAttr("scopeName", self.getScope().getName() if self.getScope() is not None else "None")
                currentTrace.config.setAttr("scopeSampleRate", 0)
                currentTrace.config.setAttr("notes", "AckPattern: " + str(self.getAcqPattern()) + "; Aux: " + ', '.join(item.getName() for item in self._auxList if item))
                currentTrace.setTraceHint(setSize)

                if waveBuffer is not None:
                    currentTrace.setTraceBuffer(waveBuffer)

                for aux in self._auxList:
                    if aux:
                        aux.setPrefix(prefix[:-1])

                ac = AcquisitionController(self.getScope(), self.getTarget(), currentTrace, self._auxList, self.getAcqPattern())
                ac.setMaxtraces(setSize)
                ac.sigNewTextResponse.connect(self.sigNewTextResponse.emit)
                ac.sigTraceDone.connect(self.sigTraceDone.emit)
                ac.sigTraceDone.connect(lambda: progressBar.updateStatus(i*setSize + ac.currentTrace, (i, ac.currentTrace)))
                ac.sigTraceDone.connect(lambda: ac.abortCapture(progressBar.wasAborted()))

                self.sigCampaignStart.emit(prefix[:-1])
                ac.doReadings(tracesDestination=self.project().traceManager())
                self.sigCampaignDone.emit()
                tcnt += setSize

                waveBuffer = currentTrace.traces   # Re-use the wave buffer for later segments to avoid memory realocation

                if progressBar.wasAborted():
                    break

            # Required in order to make the GC work properly :(
            currentTrace.unloadAllTraces()
            self._traceFormat.unloadAllTraces()

    def runScriptModule(self, mod, funcName="run"):
        try:
            classes = pluginmanager.getPluginClassesFromModules([mod])
            if len(classes) == 0:
                raise Warning("No UserScriptBase class found")
            for c in classes:
                self.runScriptClass(c, funcName)
        except Exception as e:
            sys.excepthook(Warning, "Could not execute Script Module %s: '%s'" %
                             (str(mod),
                              "".join(traceback.format_exception_only(sys.exc_info()[0], e.message)).rstrip("\n ")
                              ), sys.exc_info()[2])

    def runScriptClass(self, scriptClass, funcName="run"):
        try:
            m = scriptClass(self)
            if funcName is not None:
                eval('m.%s()' % funcName)
        except Exception as e:
                sys.excepthook(Warning, "Could not execute method %s in script class %s: '%s'" %
                               (funcName,
                                scriptClass.__name__,
                                "".join(traceback.format_exception_only(sys.exc_info()[0], e.message)).rstrip("\n ")
                                ), sys.exc_info()[2])

    def setParameter(self, pathAndValue):
        Parameter.setParameter(pathAndValue)

    @staticmethod
    def getInstance():
        return CWCoreAPI.instance

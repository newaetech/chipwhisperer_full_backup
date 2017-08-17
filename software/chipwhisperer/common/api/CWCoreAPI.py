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
import os
import traceback
import math
import sys
import logging
from chipwhisperer.capture.api.acquisition_controller import AcquisitionController
from chipwhisperer.capture.api.programmers import Programmer
from chipwhisperer.capture.api.aux_list import AuxList
from chipwhisperer.common.api.ProjectFormat import ProjectFormat
from chipwhisperer.common.results.base import ResultsBase
from chipwhisperer.common.ui.ProgressBar import *
from chipwhisperer.common.utils import util, pluginmanager
from chipwhisperer.common.utils.parameter import Parameterized, Parameter, setupSetParam
from chipwhisperer.common.utils.tracesource import TraceSource
from chipwhisperer.common.api.settings import Settings


class CWCoreAPI(Parameterized):
    """
    ChipWisperer API Class.
    Provides access to the most important parts of the tool.
    It has a singleton method called CWCoreAPI.getInstance() that returns a reference to the API instance.
    """

    __name__ = "ChipWhisperer"
    __organization__ = "NewAE Technology Inc."
    __version__ = "V3.5.4"
    _name = 'Generic Settings'
    instance = None

    def __init__(self):
        logging.basicConfig(format='%(levelname)s:%(message)s', level=logging.INFO)
        CWCoreAPI.instance = self
        self.sigNewProject = util.Signal()
        self.sigConnectStatus = util.Signal()
        self.sigAttackChanged = util.Signal()
        self.sigNewInputData = util.Signal()
        self.sigNewTextResponse = util.Signal()
        self.sigTraceDone = util.Signal()
        self.sigCampaignStart = util.Signal()
        self.sigCampaignDone = util.Signal()
        self.executingScripts = util.Observable(False)

        self.valid_scopes = pluginmanager.getPluginsInDictFromPackage("chipwhisperer.capture.scopes", True, True)
        self.valid_targets =  pluginmanager.getPluginsInDictFromPackage("chipwhisperer.capture.targets", True, True)
        self.valid_aux = pluginmanager.getPluginsInDictFromPackage("chipwhisperer.capture.auxiliary", True, True)
        self.valid_acqPatterns =  pluginmanager.getPluginsInDictFromPackage("chipwhisperer.capture.acq_patterns", True, False)
        self.valid_attacks = pluginmanager.getPluginsInDictFromPackage("chipwhisperer.analyzer.attacks", True, False)
        self.valid_preprocessingModules = pluginmanager.getPluginsInDictFromPackage("chipwhisperer.analyzer.preprocessing", False, True)

        self.settings = Settings()

        # Initialize default values
        self._project = self._scope = self._target = self._traceFormat = self._acqPattern = None
        self._attack = self.valid_attacks.get("CPA", None)
        self._acqPattern = self.valid_acqPatterns["Basic"]
        self._auxList = AuxList()
        self._numTraces = 50
        self._numTraceSets = 1

        self.params = Parameter(name='Generic Settings', type='group', addLoadSave=True).register()
        self.params.addChildren([
            {'name':'Scope Module', 'key':'scopeMod', 'type':'list', 'values':self.valid_scopes, 'get':self.getScope, 'set':self.setScope},
            {'name':'Target Module', 'key':'targetMod', 'type':'list', 'values':self.valid_targets, 'get':self.getTarget, 'set':self.setTarget},
            {'name':'Acquisition Settings', 'type':'group', 'children':[
                    {'name':'Number of Traces', 'type':'int', 'limits':(1, 1E9), 'get':self.getNumTraces, 'set':self.setNumTraces, 'linked':['Traces per Set']},
                    {'name':'Number of Sets', 'type':'int', 'limits':(1, 1E6), 'get':self.getNumTraceSets, 'set':self.setNumTraceSets, 'linked':['Traces per Set'], 'tip': 'Break acquisition into N sets, '
                     'which may cause data to be saved more frequently. The default capture driver requires that NTraces/NSets is small enough to avoid running out of system memory '
                     'as each segment is buffered into RAM before being written to disk.'},
                    {'name':'Traces per Set', 'type':'int', 'readonly':True, 'get':self.tracesPerSet},
                    {'name':'Key/Text Pattern', 'type':'list', 'values':self.valid_acqPatterns, 'get':self.getAcqPattern, 'set':self.setAcqPattern},
            ]},
        ])
        self.scopeParam = Parameter(name="Scope Settings", type='group', addLoadSave=True).register()
        self.params.getChild('Scope Module').stealDynamicParameters(self.scopeParam)

        self.targetParam = Parameter(name="Target Settings", type='group', addLoadSave=True).register()
        self.params.getChild('Target Module').stealDynamicParameters(self.targetParam)

        # Aux settings
        self.auxParam = self._auxList.getParams().register()

        # Note: Project settings are set up in setProject()

        self.newProject()

    def getResults(self, name):
        """Return the requested result widget. It should be registered."""
        return ResultsBase.registeredObjects[name]

    def getScope(self):
        """Return the current scope module object."""
        return self._scope

    @setupSetParam("Scope Module")
    def setScope(self, driver):
        """Set the current scope module object."""
        if self.getScope():
            # Don't do anything if we're not changing scopes
            if self.getScope() is driver:
                return
            else:
                self.getScope().dis()
        self._scope = driver
        if self.getScope():
            self.getScope().connectStatus.connect(self.sigConnectStatus.emit)
            self.scopeParam.append(self.getScope().params)
            try:
                ResultsBase.registeredObjects["Trace Output Plot"].setTraceSource(
                    TraceSource.registeredObjects[next(reversed(TraceSource.registeredObjects))])
            except KeyError:
                pass
            if self.getScope().getStatus():
                self.getScope().connectStatus.emit()

    def getTarget(self):
        """Return the current target module object."""
        return self._target

    @setupSetParam("Target Module")
    def setTarget(self, driver):
        """Set the current target module object."""
        if self.getTarget():
            # Don't do anything if we're not changing targets
            if self.getTarget() is driver:
                return
            self.getTarget().dis()
        self._target = driver
        if self.getTarget():
            self.targetParam.append(self.getTarget().params)
            self.getTarget().newInputData.connect(self.sigNewInputData.emit)
            self.getTarget().connectStatus.connect(self.sigConnectStatus.emit)
            if self.getTarget().getStatus():
                self.getTarget().connectStatus.emit()


    def getAuxList(self):
        return self._auxList

    def setAuxList(self, new_list):
        self._auxList = new_list

    def getAuxFunctions(self, only_enabled):
        """TODO: doc
        """
        return self._auxList.getDict(only_enabled)

    def getAcqPattern(self):
        """Return the selected acquisition pattern."""
        return self._acqPattern

    @setupSetParam(["Acquisition Settings", "Key/Text Pattern"])
    def setAcqPattern(self, pat):
        """Set the current acquisition pattern."""
        self._acqPattern = pat
        if self._acqPattern is not None:
            self._acqPattern.getParams().remove()
        self.getParams().append(self._acqPattern.getParams())

    def getNewTrace(self, format):
        """Return a new trace object for the specified format."""
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
        """Return the selected trace format."""
        if self._project is not None:
            return self._project.getTraceFormat()
        else:
            return None

    def setTraceFormat(self, format):
        """Set the current trace format for acquisition."""
        if self._project is not None:
            self._project.setTraceFormat(format)

    def project(self):
        """Return the current opened project"""
        return self._project

    def setProject(self, proj):
        """Set the current opened project"""
        self._project = proj
        self.params.append(proj.getParams())
        self.sigNewProject.emit()

    def newProject(self):
        """Create a new project"""
        self.setProject(ProjectFormat(self.__name__, self.__version__))

    def openProject(self, fname):
        """Open project file"""
        self.newProject()
        self.project().load(fname)
        try:
            ResultsBase.registeredObjects["Trace Output Plot"].setTraceSource(TraceSource.registeredObjects["Trace Management"])
        except KeyError:
            pass

    def saveProject(self, fname=None):
        """Save the current opened project to file"""
        if fname is not None:
            self.project().setFilename(fname)
        self.project().save()

    def connectScope(self):
        """Connect to the selected scope"""
        try:
            if self.getScope():
                self.getScope().con()
                try:
                    # Sets the Plot Widget input (if it exists) to the last added TraceSource
                    ResultsBase.registeredObjects["Trace Output Plot"].setTraceSource(
                        TraceSource.registeredObjects[next(reversed(TraceSource.registeredObjects))])
                except KeyError:
                    pass

        except Warning:
            sys.excepthook(*sys.exc_info())
            return False
        return True

    def disconnectScope(self):
        """Disconnect the current scope"""
        self.getScope().dis()

    def connectTarget(self):
        """Connect to the selected target"""
        try:
            if self.getTarget():
                self.getTarget().con(scope=self.getScope())
        except Warning:
            sys.excepthook(*sys.exc_info())
            return False
        return True

    def disconnectTarget(self):
        """Disconnect the current target"""
        self.getTarget().dis()

    def doConDis(self):
        """DEPRECATED: It is here just for compatibility reasons"""
        logging.warning('Method doConDis() is deprecated... use connect() or disconnect() instead')
        return self.connect()

    def connect(self):
        """Connect both: scope and target"""
        return self.connectScope() and self.connectTarget()

    def disconnect(self):
        """Disconnect both: scope and target"""
        self.disconnectScope()
        self.disconnectTarget()

    def getNumTraces(self):
        """Return the total number or traces for acquisition purposes"""
        return self._numTraces

    @setupSetParam(["Acquisition Settings", "Number of Traces"])
    def setNumTraces(self, n):
        """Set the total number or traces for acquisition purposes"""
        self._numTraces = n

    def getNumTraceSets(self):
        """Return the number of sets/segments"""
        return self._numTraceSets

    @setupSetParam(["Acquisition Settings", "Number of Sets"])
    def setNumTraceSets(self, s):
        """Set the number of sets/segments"""
        self._numTraceSets = s

    def tracesPerSet(self):
        """Return the number of traces in each set/segment"""
        return int(self._numTraces / self._numTraceSets)

    def capture1(self):
        """Capture one trace"""
        try:
            aux_dict = self.getAuxFunctions(True)
            ac = AcquisitionController(self.getScope(), self.getTarget(), writer=None, aux=aux_dict, keyTextPattern=self.getAcqPattern())
            ac.sigNewTextResponse.connect(self.sigNewTextResponse.emit)
            if self.getTarget():
                self.getTarget().init()
            return ac.doSingleReading()
        except Warning:
            sys.excepthook(*sys.exc_info())
            return False

    def captureM(self, progressBar=None, scope=None, target=None, project=None, aux_list=None, ktp=None, N=1, seg_size=None):
        """Capture multiple traces and save its result"""
        if not progressBar:
            progressBar = ProgressBarText()

        if seg_size is None:
            seg_size = 1000
        trace_mgr = project.traceManager() if project is not None else None
        trace_fmt = project.getTraceFormat() if project is not None else None
        aux_dict = aux_list.getDict(True) if aux_list is not None else None
        segments = int(math.ceil(N / float(seg_size)))

        with progressBar:
            progressBar.setStatusMask("Current Segment = %d Current Trace = %d", (0,0))
            progressBar.setMaximum(N)

            waveBuffer = None
            tcnt = 0
            for i in range(0, segments):
                if progressBar.wasAborted(): break

                this_seg_size = min(seg_size, N - i*seg_size)
                if trace_fmt is not None:
                    currentTrace = self.getNewTrace(trace_fmt)
                    # Load trace writer information
                    prefix = currentTrace.config.attr("prefix")[:-1]
                    currentTrace.config.setAttr("targetHW", target.getName() if target is not None else "None")
                    currentTrace.config.setAttr("targetSW", os.path.split(Programmer.lastFlashedFile)[1])
                    currentTrace.config.setAttr("scopeName", scope.getName() if scope is not None else "None")
                    notes_str = "AckPattern: " + str(ktp) + "; "
                    notes_str += "Aux: "
                    if aux_dict is not None:
                        for t in aux_dict.keys():
                            notes_str += "%s" % t + ", ".join([str(item) for item in aux_dict[t] if item])
                    currentTrace.config.setAttr("notes", notes_str)
                    currentTrace.setTraceHint(this_seg_size)

                    if waveBuffer is not None:
                        currentTrace.setTraceBuffer(waveBuffer)
                else:
                    currentTrace = None
                    prefix = datetime.now().strftime('%Y.%m.%d-%H.%M.%S')

                if self._aux_dict is not None:
                    for func in self._aux_dict['set_prefix']:
                        func(prefix)

                ac = AcquisitionController(scope, target, currentTrace, aux_dict, ktp)
                ac.setMaxtraces(this_seg_size)
                ac.sigNewTextResponse.connect(self.sigNewTextResponse.emit)
                ac.sigTraceDone.connect(self.sigTraceDone.emit)
                __pb = lambda: progressBar.updateStatus(i*seg_size + ac.currentTrace + 1, (i, ac.currentTrace))
                ac.sigTraceDone.connect(__pb)
                self.sigCampaignStart.emit(prefix)
                ac.doReadings(tracesDestination=trace_mgr, progressBar=progressBar)

                if currentTrace is not None:
                    project.saveAllSettings(os.path.dirname(currentTrace.config.configFilename()) + "/%s_settings.cwset" % prefix, onlyVisibles=True)
                    waveBuffer = currentTrace.traces  # Re-use the wave buffer to avoid memory reallocation
                self.sigCampaignDone.emit()
                tcnt += seg_size

                if progressBar.wasAborted():
                    break

            if currentTrace is not None:
                currentTrace.unloadAllTraces()  # Required in order to make the GC work properly :(
                trace_fmt.unloadAllTraces()
        return True

    def runScriptModule(self, mod, funcName="run"):
        """Execute the function in the Plugin classes of the specified module"""
        try:
            classes = pluginmanager.getPluginClassesFromModules([mod])
            if len(classes) == 0:
                raise Warning("No UserScriptBase class found")
            return [self.runScriptClass(c, funcName) for c in classes]
        except Exception as e:
            sys.excepthook(Warning, "Could not execute Script Module %s: '%s:%s'" %
                             (str(mod),
                              "".join(traceback.format_exception_only(sys.exc_info()[0], e.message)).rstrip("\n "),
                              str(e).rstrip("\n ")
                              ), sys.exc_info()[2])

    def runScriptClass(self, scriptClass, funcName="run"):
        """Execute the funcName function in the specified class."""
        try:
            self.executingScripts.setValue(True)
            m = scriptClass(self)
            if funcName is not None:
                return eval('m.%s()' % funcName)
        except Exception as e:
                sys.excepthook(Warning, "Could not execute method %s in script class %s: '%s:%s'" %
                               (funcName,
                                scriptClass.__name__,
                                "".join(traceback.format_exception_only(sys.exc_info()[0], e.message)).rstrip("\n "),
                                str(e).rstrip("\n ")
                                ), sys.exc_info()[2])
        finally:
            self.executingScripts.setValue(False)

    def getParameter(self, path):
        """Return the value of a registered parameter"""
        return Parameter.getParameter(path)

    def setParameter(self, pathAndValue):
        """Set the parameter value, given its path. It should be registered in Parameter.registeredParameters"""
        Parameter.setParameter(pathAndValue)

    @staticmethod
    def getInstance():
        """Implements the singleton pattern/anti-pattern. Returns a reference to the API instance."""
        return CWCoreAPI.instance

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

__author__ = "NewAE Technology Inc."

import traceback
import importlib
from datetime import *
from chipwhisperer.common.api.ProjectFormat import ProjectFormat
from chipwhisperer.common.utils import util
from chipwhisperer.capture.api.AcquisitionController import AcquisitionController, AcqKeyTextPattern_Basic, AcqKeyTextPattern_CRITTest

try:
    # OrderedDict is new in 2.7
    from collections import OrderedDict
    dicttype = OrderedDict
except ImportError:
    dicttype = dict
    
def _module_reorder(resp):
    #None is first, then alphabetical
    newresp = dicttype()
    if 'None' in resp:
        newresp['None'] = resp['None']
        del resp['None']
    newresp.update(sorted(resp.items(), key=lambda t: t[0]))   
    return newresp

class CWCoreAPI(object):
    __name__ = "ChipWhisperer"
    __organization__ = "NewAE"
    __version__ = "V3.0"
    instance = None

    class Signals(object):
        def __init__(self):
            self.parametersChanged = util.Signal()
            self.traceChanged = util.Signal()
            self.newProject = util.Signal()
            self.reloadAttackParamList = util.Signal()
            self.attackChanged = util.Signal()
            self.paramListUpdated = util.Signal()
            self.scopeChanged = util.Signal()
            self.targetChanged = util.Signal()
            self.auxChanged = util.Signal()
            self.acqPatternChanged = util.Signal()
            self.connectStatus = util.Signal()
            self.newInputData = util.Signal()
            self.newTextResponse = util.Signal()
            self.traceDone = util.Signal()
            self.campaignStart = util.Signal()
            self.campaignDone = util.Signal()
            self.abortCapture = util.Signal()

    def __init__(self, rootDir):
        self.rootDir = rootDir
        self.paramTrees = []
        self._traceManager = None
        self._project = None
        self._scope = None
        self._target = None
        self._traceClass = None
        self.da = None
        self.numTraces = 100
        self.numSegments = 1
        self.traceLimits = 0
        self.pointLimits = 0
        self.results = None
        self.signals = self.Signals()
        self.signals.abortCapture.connect(self.abortCapture)
        CWCoreAPI.instance = self

    def getRootDir(self):
        return self.rootDir

    def hasScope(self):
        return self._scope is not None

    def getScope(self):
        if not self.hasScope(): raise Exception("Scope Module not loaded")
        return self._scope

    def setScope(self, driver):
        self._scope = driver
        util.active_scope = self._scope
        self.signals.scopeChanged.emit()

    def hasTarget(self):
        return self._target is not None

    def getTarget(self):
        if not self.hasTarget(): raise Exception("Target Module not loaded")
        return self._target

    def setTarget(self, driver):
        self._target = driver
        self._target.paramListUpdated.connect(self.signals.paramListUpdated.emit)
        self._target.newInputData.connect(self.signals.newInputData.emit)
        self.signals.paramListUpdated.emit()
        self.signals.targetChanged.emit()

    def setAux(self, aux):
        self.auxList = [aux]
        self.signals.auxChanged.emit()

    def setAcqPattern(self, pat):
        self.acqPattern = pat
        self.acqPattern.setTarget(self.getTarget())
        self.signals.acqPatternChanged.emit()

    def connectScope(self):
        self.getScope().con()
        if hasattr(self.getScope(), "qtadc"):
            self.getTarget().setOpenADC(self.getScope().qtadc.ser)

    def connectTarget(self):
        self.getTarget().con()

    def doConDis(self):
        """DEPRECATED: Is here just for compatibility reasons"""
        print "Method doConDis() is deprecated... use connect() or disconnect() instead"
        self.connect()

    def connect(self):
        try:
            self.connectScope()
        finally:
            self.connectTarget()

    def disconnectScope(self):
        self.getScope().dis()

    def disconnectTarget(self):
        self.getTarget().dis()

    def disconnect(self):
        self.disconnectScope()
        self.disconnectTarget()

    def getNumTraces(self):
        return self.numTraces

    def setNumTraces(self, t):
        self.numTraces = t

    def getNumSegments(self):
        return self.numSegments

    def setNumSegments(self, s):
        self.numSegments = s

    def capture1(self):
        ac = AcquisitionController(self.getScope(), self.getTarget(), writer=None, auxList=self.auxList, keyTextPattern=self.acqPattern)
        ac.signals.newTextResponse.connect(self.signals.newTextResponse.emit)
        ac.doSingleReading()

    def abortCapture(self):
            print "Capture aborted"
            self._abortCapture = True

    def captureM(self):
        overallstarttime = datetime.now()
        writerlist = []
        tcnt = 0
        tracesPerRun = int(self.numTraces / self.numSegments)

        # This system re-uses one wave buffer a bunch of times. This is required since the memory will become
        # fragmented, even though you are just freeing & reallocated the same size buffer. It's slightly less
        # clear but it ensures you don't suddently have a capture interrupted with a memory error. This can
        # happen even if you have loads of memory free (e.g. are only using ~200MB for the buffer), well before
        # the 1GB limit that a 32-bit process would expect to give you trouble at.
        waveBuffer = None
        self._abortCapture = False

        for i in range(0, self.numSegments):
            if self._abortCapture:
                break
            currentTrace = self.getTraceClassInstance()

            # Load trace writer information
            starttime = datetime.now()
            baseprefix = starttime.strftime('%Y.%m.%d-%H.%M.%S')
            prefix = baseprefix + "_"
            currentTrace.config.setAttr("prefix", prefix)
            currentTrace.config.setConfigFilename(self.project().datadirectory + "traces/config_" + prefix + ".cfg")
            currentTrace.config.setAttr("date", starttime.strftime('%Y-%m-%d %H:%M:%S'))
            currentTrace.setTraceHint(tracesPerRun)

            if waveBuffer is not None:
                currentTrace.setTraceBuffer(waveBuffer)

            if self.auxList is not None:
                for aux in self.auxList:
                    aux.setPrefix(baseprefix)

            ac = AcquisitionController(self.getScope(), self.getTarget(), currentTrace, self.auxList, self.acqPattern)
            ac.setMaxtraces(tracesPerRun)
            ac.signals.newTextResponse.connect(self.signals.newTextResponse.emit)
            ac.signals.traceDone.connect(self.signals.traceDone.emit)
            self.signals.campaignStart.emit(baseprefix)
            ac.doReadings(addToList=self._traceManager)

            tcnt += tracesPerRun
            print "%d Captures Completed" % tcnt
            self.signals.campaignDone.emit()

            # Re-use the wave buffer for later segments
            if self.hasTraceClass():
                waveBuffer = currentTrace.traces
                writerlist.append(currentTrace)

        print "Capture delta time: %s" % (str(datetime.now() - overallstarttime))
        return writerlist

    def project(self):
        return self._project

    def setProject(self, proj):
        self._project = proj
        self.signals.newProject.emit()

    def newProject(self):
        self.setProject(ProjectFormat(self))
        self.project().setProgramName(self.__name__)
        self.project().setProgramVersion(self.__version__)
        self.project().addParamTree(self)
        # self.project().addParamTree(self.getScope())
        # self.project().addParamTree(self.getTarget())
        self.getTraceManager().newProject()
        self.project().setTraceManager(self.getTraceManager())

    def openProject(self, fname):
        self.setProject(ProjectFormat(self))
        self.project().setProgramName(self.__name__)
        self.project().setProgramVersion(self.__version__)
        self.getTraceManager().newProject()
        self.project().setTraceManager(self.getTraceManager())
        self.project().load(fname)
        self.attack.setProject(self.project())

    def saveProject(self, fname):
        self.project().setFilename(fname)
        self.project().save()

    def hasTraceClass(self):
        return self._traceClass is not None

    def getTraceClassInstance(self):
        if not self.hasTraceClass(): raise Exception("Trace format not defined")
        return self._traceClass(self._traceClass.getParams)

    def getTraceClass(self):
        return self._traceClass

    def setTraceClass(self, driver):
        self.signals.traceChanged.emit()
        self._traceClass = driver

    def getTraceManager(self):
        return self._traceManager

    def setTraceManager(self, manager):
        self._traceManager = manager

    def getAttack(self):
        return self.attack

    def setAttack(self, attack):
        """Set the attack module, reloading GUI and connecting appropriate signals"""

        self.attack = attack
        self.signals.reloadAttackParamList.emit()
        self.results.setAttack(self.attack)
        self.attack.paramListUpdated.connect(self.signals.reloadAttackParamList.emit)
        self.attack.setTraceLimits(self.traceLimits, self.pointLimits)

        # Sometimes required
        if hasattr(self, "traces") and self.traces:
            self.attack.setTraceManager(self.traces)

        self.attack.setProject(self.project())
        self.signals.attackChanged.emit()

    def doAttack(self, mod):
        """Called when the 'Do Attack' button is pressed, or can be called via API
        to cause attack to run"""

        print "Attacking..."
        # mod.initProject()

        # Setup trace sources etc, this calls the
        # .initPreprocessing itself
        # it also resets the setTraces in the passed 'mod',
        # which is REQUIRED for proper functioning!
        mod.initAnalysis()
        mod.initReporting(self.results)
        mod.doAnalysis()
        mod.doneAnalysis()
        mod.doneReporting()

        # print "Attack Started"
        # if self.results:
        #    self.results.setTraceManager(self.traces)
        #
        # if self.attack:
        #    self.attack.setTraceManager(self.traces)
        #    self.attack.doAttack()

        # print "Attack Done"

    def setTraceLimits(self, traces=None, points=None):
        """When traces is loaded, Tell everything default point/trace range"""

        #Set parameters for attack
        self.traceLimits = traces
        self.pointLimits = points
        self.attack.setTraceLimits(traces, points)

    def _setParameter_children(self, top, path, value, echo):
        """Descends down a given path, looking for value to set"""
        #print top.name()
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

            print "Parameter not found: %s"%str(parameter)
        except ValueError:
            #A little klunky: we use exceptions to tell us the system DID work as intended
            pass
        except IndexError:
            raise IndexError("IndexError Setting Parameter %s\n%s"%(str(parameter), traceback.format_exc()))

        self.signals.parametersChanged.emit()

    @staticmethod
    def getInstance():
        return CWCoreAPI.instance

    @staticmethod
    def getPreprocessingModules(dir, tracerManager, waveformWidget):
        resp = dicttype()
        for f in util.getPyFiles(dir):
            try:
                i = importlib.import_module('chipwhisperer.analyzer.preprocessing.' + f)
                mod = i.getClass()(tracerManager, waveformWidget)
                resp[mod.getName()] = mod
            except Exception as e:
                print "INFO: Could not import preprocessing module " + f + ": " + str(e)
        # print "Loaded preprocessing modules: " + resp.__str__()
        
        return resp

    @staticmethod
    def getTraceFormats(dir):
        resp = dicttype()
        for f in util.getPyFiles(dir):
            try:
                i = importlib.import_module('chipwhisperer.common.traces.' + f)
                if not hasattr(i, 'getClass'):
                    continue
                mod = i.getClass()
                resp[mod.getName()] = mod
            except Exception as e:
                print "INFO: Could not import trace format module " + f + ": " + str(e)
        # print "Loaded target modules: " + resp.__str__()
        return resp

    @staticmethod
    def getScopeModules(dir):
        resp = dicttype()
        for f in util.getPyFiles(dir):
            try:
                i = importlib.import_module('chipwhisperer.capture.scopes.' + f)
                mod = i.getInstance()
                resp[mod.getName()] = mod
            except Exception as e:
                print "INFO: Could not import scope module " + f + ": " + str(e)
        # print "Loaded scope modules: " + resp.__str__()
        
        return _module_reorder(resp)

    @staticmethod
    def getTargetModules(dir):
        resp = dicttype()
        for t in util.getPyFiles(dir):
            try:
                i = importlib.import_module('chipwhisperer.capture.targets.' + t)
                mod = i.getInstance()
                resp[mod.getName()] = mod
            except Exception as e:
                print "INFO: Could not import target module " + t + ": " + str(e)
        # print "Loaded target modules: " + resp.__str__()
        return _module_reorder(resp)

    @staticmethod
    def getAuxiliaryModules(dir):
        resp = dicttype()
        for f in util.getPyFiles(dir):
            try:
                i = importlib.import_module('chipwhisperer.capture.auxiliary.' + f)
                mod = i.getInstance()
                resp[mod.getName()] = mod
            except Exception as e:
                print "INFO: Could not import auxiliary module " + f + ": " + str(e)
        # print "Loaded scope modules: " + resp.__str__()
        return _module_reorder(resp)

    @staticmethod
    def getExampleScripts(dir):
        resp = []
        for f in util.getPyFiles(dir):
            try:
                m = importlib.import_module('chipwhisperer.capture.scripts.' + f)
                resp.append(m)
            except Exception as e:
                print "INFO: Could not import example script " + f + ": " + str(e)
        # print "Loaded scripts: " + resp.__str__()
        return resp

    @staticmethod
    def getAcqPatternModules():
        resp = dicttype()
        resp["Basic"] = AcqKeyTextPattern_Basic()
        if AcqKeyTextPattern_CRITTest:
            resp['CRI T-Test'] = AcqKeyTextPattern_CRITTest()
        # print "Loaded Patterns: " + resp.__str__()
        return _module_reorder(resp)
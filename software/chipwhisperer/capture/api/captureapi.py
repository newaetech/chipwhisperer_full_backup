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

import importlib
from datetime import *
from chipwhisperer.common.utils import util
from chipwhisperer.capture.api.AcquisitionController import AcquisitionController, AcqKeyTextPattern_Basic, AcqKeyTextPattern_CRITTest

class CWCaptureAPI(object):
    """This is the manager class"""

    class Signals():
        paramListUpdated = util.Signal()
        scopeChanged = util.Signal()
        targetChanged = util.Signal()
        traceChanged = util.Signal()
        connectStatus = util.Signal()
        newInputData = util.Signal()
        newTextResponse = util.Signal()
        traceDone = util.Signal()
        campaignStart = util.Signal()
        campaignDone = util.Signal()
        abortCapture = util.Signal()

    def __init__(self):
        self.signals = CWCaptureAPI.Signals()
        self.numTraces = 100
        self.numSegments = 1
        self._scope = None
        self._target = None
        self._traceClass = None

    def hasScope(self):
        return self._scope is not None

    def getScope(self):
        if not self.hasScope(): raise Exception("Scope Module not loaded")
        return self._scope

    def setScope(self, driver):
        self._scope = driver

        util.active_scope = self._scope
        self.signals.scopeChanged.emit()

    def setOpenADC(self, oadc):
        '''Declares OpenADC Instance in use. Only for openadc-integrated targets'''
        try:  # if hasattr(oadc, "scope.sc"):
            self.oadc = oadc.scope.sc
        except AttributeError:
            self.oadc = oadc
        if hasattr(self.getTarget(), "setOpenADC"):
            self.getTarget().setOpenADC(self.oadc)

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

    def connectScope(self):
        # try:
            self.getScope().con()
            if hasattr(self.getScope(), "qtadc"):
                self.getTarget().setOpenADC(self.getScope().qtadc.ser)
        # except Exception, e:
        #     util.appendAndForwardErrorMessage("Manager could not connect to Scope Module \"" + self.getScope().getName() + "\"", e)

    def connectTarget(self):
        # try:
         if hasattr(self.getTarget(), "setOpenADC") and hasattr(self, "oadc") and self.oadc:
             self.getTarget().setOpenADC(self.oadc)
         self.getTarget().con()
        # except Exception, e:
        #     util.appendAndForwardErrorMessage("Manager could not connect to Target Module \"" + self.getTarget().getName() + "\"", e)

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

    def setNumSegments(self, s):
        self.numSegments = s

    def getNumSegments(self):
        return self.numSegments

    def capture1(self, auxList, acqPattern):
        try:
            ac = AcquisitionController(self.getScope(), self.getTarget(), writer=None, auxList=auxList, keyTextPattern=acqPattern)
            ac.signals.newTextResponse.connect(self.signals.newTextResponse.emit)
            ac.doSingleReading()
        except Exception, e:
            util.appendAndForwardErrorMessage("Manager could execute capture1 method", e)

    def captureM(self, datadirectory, numTraces, numSegments, auxList, acqPattern):
        overallstarttime = datetime.now()
        writerlist = []
        tcnt = 0
        tracesPerRun = int(numTraces / numSegments)

        # This system re-uses one wave buffer a bunch of times. This is required since the memory will become
        # fragmented, even though you are just freeing & reallocated the same size buffer. It's slightly less
        # clear but it ensures you don't suddently have a capture interrupted with a memory error. This can
        # happen even if you have loads of memory free (e.g. are only using ~200MB for the buffer), well before
        # the 1GB limit that a 32-bit process would expect to give you trouble at.
        waveBuffer = None

        try:
            for i in range(0, numSegments):
                starttime = datetime.now()
                baseprefix = starttime.strftime('%Y.%m.%d-%H.%M.%S')
                prefix = baseprefix + "_"

                currentTrace = self.getTraceClassInstance()

                # Load trace writer information
                currentTrace.config.setAttr("prefix", prefix)
                currentTrace.config.setConfigFilename(datadirectory + "traces/config_" + prefix + ".cfg")
                currentTrace.config.setAttr("date", starttime.strftime('%Y-%m-%d %H:%M:%S'))
                currentTrace.setTraceHint(tracesPerRun)

                if waveBuffer is not None:
                    currentTrace.setTraceBuffer(waveBuffer)


                if auxList is not None:
                    for aux in auxList:
                        aux.setPrefix(baseprefix)

                ac = AcquisitionController(self.getScope(), self.getTarget(), currentTrace, auxList, acqPattern)
                ac.signals.newTextResponse.connect(self.signals.newTextResponse.emit)
                ac.signals.traceDone.connect(self.signals.traceDone.emit)
                self.signals.abortCapture.connect(ac.abortCapture)
                self.signals.campaignStart.emit(baseprefix)
                ac.setMaxtraces(tracesPerRun)

                ac.doReadings()
    #            ac.doReadings(addToList=self.manageTraces)

                tcnt += tracesPerRun
                print "%d Captures Completed" % tcnt
                self.signals.campaignDone.emit()

                # Re-use the wave buffer for later segments
                if self.hasTraceClass():
                    waveBuffer = currentTrace.traces
                    writerlist.append(currentTrace)
            print "Capture delta time: %s" % (str(datetime.now() - overallstarttime))
        except Exception, e:
            util.appendAndForwardErrorMessage("Manager could execute captureM method", e)

        return writerlist

    def validateSettings(self):
        # Validate settings from all modules before starting multi-capture
        ret = []
        try:
            ret.extend(self.getTarget().validateSettings())
        except AttributeError:
            ret.append(("info", "Target Module", "Target has no validateSettings()", "Internal Error", "73b08424-3865-4274-8fd7-dd213ede2c46"))
        except Exception as e:
            ret.append(("warn", "General Settings", e.message, "Specify Target Module", "2351e3b0-e5fe-11e3-ac10-0800200c9a66"))

        try:
            ret.extend(self.getScope().validateSettings())
        except AttributeError:
            ret.append(("info", "Scope Module", "Scope has no validateSettings()", "Internal Error", "d19be31d-ad1a-4533-80dc-9423dfa92753"))
        except Exception as e:
            ret.append(("warn", "General Settings", e.message, "Specify Scope Module", "325de1cf-0d47-4ed8-8e9f-77d8f9cf2d5f"))

        try:
            ret.extend(self.getTraceClass()().validateSettings())
        except AttributeError:
            ret.append(("info", "Writer Module", "Writer has no validateSettings()", "Internal Error", "d7b3a9a1-83f0-4b4d-92b9-3d7dcf6304ae"))
        except Exception as e:
            ret.append(("warn", "General Settings", e.message, "Specify Trace Writer Module", "57a3924d-3794-4ca6-9693-46a7b5243727"))

        return ret

    @staticmethod
    def getScopeModules(dir, showScriptParameter):
        resp = {}
        for f in util.getPyFiles(dir):
            try:
                i = importlib.import_module('chipwhisperer.capture.scopes.' + f)
                mod = i.getInstance(showScriptParameter)
                resp[mod.getName()] = mod
            except Exception as e:
                print "Warning: Could not import scope module " + f + ": " + str(e)
        # print "Loaded scope modules: " + resp.__str__()
        return resp

    @staticmethod
    def getTargetModules(dir, showScriptParameter):
        resp = {}
        for t in util.getPyFiles(dir):
            try:
                i = importlib.import_module('chipwhisperer.capture.targets.' + t)
                mod = i.getInstance(showScriptParameter)
                resp[mod.getName()] = mod
            except Exception as e:
                print "Warning: Could not import target module " + t + ": " + str(e)
        # print "Loaded target modules: " + resp.__str__()
        return resp

    @staticmethod
    def getAuxiliaryModules(dir, showScriptParameter):
        resp = {}
        for f in util.getPyFiles(dir):
            try:
                i = importlib.import_module('chipwhisperer.capture.auxiliary.' + f)
                mod = i.getInstance(showScriptParameter)
                resp[mod.getName()] = mod
            except Exception as e:
                print "Warning: Could not import auxiliary module " + f + ": " + str(e)
        # print "Loaded scope modules: " + resp.__str__()
        return resp

    @staticmethod
    def getTraceFormats(dir):
        resp = {}
        for f in util.getPyFiles(dir):
            try:
                i = importlib.import_module('chipwhisperer.common.traces.' + f)
                mod = i.getClass()
                resp[mod.getName()] = mod
            except Exception as e:
                print "Warning: Could not import trace format module " + f + ": " + str(e)
        # print "Loaded target modules: " + resp.__str__()
        return resp

    @staticmethod
    def getExampleScripts(dir):
        resp = []
        for f in util.getPyFiles(dir):
            try:
                m = importlib.import_module('chipwhisperer.capture.scripts.' + f)
                resp.append(m)
            except Exception as e:
                print "Warning: Could not import example script " + f + ": " + str(e)
        # print "Loaded scripts: " + resp.__str__()
        return resp

    @staticmethod
    def getAcqPatternModules(showScriptParameter):
        resp = {}
        resp["Basic"] = AcqKeyTextPattern_Basic(showScriptParameter=showScriptParameter)
        if AcqKeyTextPattern_CRITTest:
            resp['CRI T-Test'] = AcqKeyTextPattern_CRITTest(showScriptParameter=showScriptParameter)
        # print "Loaded Patterns: " + resp.__str__()
        return resp


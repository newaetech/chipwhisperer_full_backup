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
from chipwhisperer.common.api.CWCoreAPI import CWCoreAPI
from chipwhisperer.capture.api.AcquisitionController import AcquisitionController, AcqKeyTextPattern_Basic, AcqKeyTextPattern_CRITTest

class CWCaptureAPI(CWCoreAPI):
    """This is the manager class"""

    class Signals(CWCoreAPI.Signals):
        paramListUpdated = util.Signal()
        scopeChanged = util.Signal()
        targetChanged = util.Signal()
        auxChanged = util.Signal()
        acqPatternChanged = util.Signal()
        connectStatus = util.Signal()
        newInputData = util.Signal()
        newTextResponse = util.Signal()
        traceDone = util.Signal()
        campaignStart = util.Signal()
        campaignDone = util.Signal()
        abortCapture = util.Signal()

    def __init__(self):
        super(CWCaptureAPI, self).__init__()
        self._scope = None
        self._target = None
        self._traceClass = None
        self._traceManager = None
        self.signals.abortCapture.connect(self.abortCapture)
        self.numTraces = 100
        self.numSegments = 1

    def setupParameters(self, rootdir, showScriptParameter):
        valid_scopes = CWCaptureAPI.getScopeModules(rootdir + "/scopes", showScriptParameter)
        valid_targets =  CWCaptureAPI.getTargetModules(rootdir + "/targets", showScriptParameter)
        valid_traces = CWCaptureAPI.getTraceFormats(rootdir + "/../common/traces")
        valid_aux = CWCaptureAPI.getAuxiliaryModules(rootdir + "/auxiliary", showScriptParameter)
        valid_acqPatterns = CWCaptureAPI.getAcqPatternModules(showScriptParameter)

        self.setScope(valid_scopes["None"])
        self.setTarget(valid_targets["None"])
        self.setTraceClass(valid_traces["ChipWhisperer/Native"])
        self.auxList = [valid_aux["None"]]
        self.acqPattern = valid_acqPatterns['Basic']

        self.cwParams = [
                {'name':'Scope Module', 'type':'list', 'values':valid_scopes, 'value':self.getScope(), 'set':self.setScope},
                {'name':'Target Module', 'type':'list', 'values':valid_targets, 'value':self.getTarget(), 'set':self.setTarget},
                {'name':'Trace Format', 'type':'list', 'values':valid_traces, 'value':self.getTraceClass(), 'set':self.setTraceClass},
                {'name':'Auxiliary Module', 'type':'list', 'values':valid_aux, 'value':self.auxList[0], 'set':self.setAux},

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
                        # {'name':'Open Monitor', 'type':'action', 'action':self.esm.show},
                        {'name':'Key/Text Pattern', 'type':'list', 'values':valid_acqPatterns, 'value':self.acqPattern, 'set':self.setAcqPattern},
                    ]},
                ]

    def hasScope(self):
        return self._scope is not None

    def getScope(self):
        if not self.hasScope(): raise Exception("Scope Module not loaded")
        return self._scope

    def setScope(self, driver):
        self._scope = driver

        util.active_scope = self._scope
        self.signals.scopeChanged.emit()

    # def setOpenADC(self, oadc):
    #     '''Declares OpenADC Instance in use. Only for openadc-integrated targets'''
    #     try:  # if hasattr(oadc, "scope.sc"):
    #         self.oadc = oadc.scope.sc
    #     except AttributeError:
    #         self.oadc = oadc
    #     if hasattr(self.getTarget(), "setOpenADC"):
    #         self.getTarget().setOpenADC(self.oadc)

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
         if hasattr(self.getTarget(), "setOpenADC") and hasattr(self, "oadc") and self.oadc:
             self.getTarget().setOpenADC(self.oadc)
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
            ac.signals.newTextResponse.connect(self.signals.newTextResponse.emit)
            ac.signals.traceDone.connect(self.signals.traceDone.emit)
            self.signals.abortCapture.connect(ac.abortCapture)
            self.signals.campaignStart.emit(baseprefix)
            ac.setMaxtraces(tracesPerRun)

            ac.doReadings(addToList=self._traceManager)
            self.signals.abortCapture.disconnect(ac.abortCapture)

            if self._abortCapture:
                break

            tcnt += tracesPerRun
            print "%d Captures Completed" % tcnt
            self.signals.campaignDone.emit()

            # Re-use the wave buffer for later segments
            if self.hasTraceClass():
                waveBuffer = currentTrace.traces
                writerlist.append(currentTrace)
        print "Capture delta time: %s" % (str(datetime.now() - overallstarttime))
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

        tracesPerRun = int(self.numTraces / self.numSegments)
        if tracesPerRun > 10E3:
            ret.append(("warn", "General Settings", "Very Long Capture (%d traces)" % tracesPerRun, "Set 'Capture Segments' to '%d'" % (self.numTraces / 10E3), "1432bf95-9026-4d8c-b15d-9e49147840eb"))

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


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
import sys, os
from chipwhisperer.common.api.ProjectFormat import ProjectFormat
from chipwhisperer.common.utils import Util
from chipwhisperer.common.ui.ProgressBar import *
from chipwhisperer.capture.api.AcquisitionController import AcquisitionController


class CWCoreAPI(object):
    __name__ = "ChipWhisperer"
    __organization__ = "NewAE Technology Inc."
    __version__ = "V3.0"
    instance = None

    class Signals(object):
        def __init__(self):
            self.parametersChanged = Util.Signal()
            self.traceChanged = Util.Signal()
            self.newProject = Util.Signal()
            self.reloadAttackParamList = Util.Signal()
            self.attackChanged = Util.Signal()
            self.paramListUpdated = Util.Signal()
            self.scopeChanged = Util.Signal()
            self.targetChanged = Util.Signal()
            self.auxChanged = Util.Signal()
            self.acqPatternChanged = Util.Signal()
            self.connectStatus = Util.Signal()
            self.newInputData = Util.Signal()
            self.newTextResponse = Util.Signal()
            self.traceDone = Util.Signal()
            self.campaignStart = Util.Signal()
            self.campaignDone = Util.Signal()

    def __init__(self):
        self.paramTrees = []
        self._project = None
        self._scope = None
        self._target = None
        self._traceClass = None
        self._attack = None
        self._numTraces = 100
        self._numTraceSets = 1
        self.results = None
        self.signals = self.Signals()
        CWCoreAPI.instance = self

    def getRootDir(self):
        return Util.getRootDir()

    def hasScope(self):
        return self._scope is not None

    def getScope(self):
        if not self.hasScope(): raise Warning("Scope Module not loaded")
        return self._scope

    def setScope(self, driver):
        if self.hasScope(): self.getScope().dis()
        self._scope = driver
        self.signals.scopeChanged.emit()

    def hasTarget(self):
        return self._target is not None

    def getTarget(self):
        if not self.hasTarget(): raise Warning("Target Module not loaded")
        return self._target

    def setTarget(self, driver):
        if self.hasTarget(): self.getTarget().dis()
        self._target = driver
        self._target.paramListUpdated.connect(self.signals.paramListUpdated.emit)
        self._target.newInputData.connect(self.signals.newInputData.emit)
        self.signals.paramListUpdated.emit()
        self.signals.targetChanged.emit()

    def setAux(self, aux):
        self.auxList = [aux]
        self.signals.auxChanged.emit()

    def getAux(self):
        return self.auxList

    def setAcqPattern(self, pat):
        self.acqPattern = pat
        self.acqPattern.setTarget(self.getTarget())
        self.signals.acqPatternChanged.emit()

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
            ac = AcquisitionController(self.getScope(), self.getTarget(), writer=None, auxList=self.auxList, keyTextPattern=self.acqPattern)
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
                currentTrace = self.getTraceClassInstance()

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
                currentTrace.config.setAttr("notes", "Aux: " + ', '.join(w.getName() for w in self.auxList))
                currentTrace.setTraceHint(setSize)

                if waveBuffer is not None:
                    currentTrace.setTraceBuffer(waveBuffer)

                if self.auxList is not None:
                    for aux in self.auxList:
                        aux.setPrefix(baseprefix)

                ac = AcquisitionController(self.getScope(), self.getTarget(), currentTrace, self.auxList, self.acqPattern)
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

    def hasTraceClass(self):
        return self._traceClass is not None

    def getTraceClassInstance(self):
        if not self.hasTraceClass(): raise Warning("Trace format not defined")
        return self._traceClass(self._traceClass.getParams)

    def getTraceClass(self):
        return self._traceClass

    def setTraceClass(self, driver):
        self.signals.traceChanged.emit()
        self._traceClass = driver

    def getAttack(self):
        return self._attack

    def setAttack(self, attack): # Move to GUI??
        """Set the attack module, reloading GUI and connecting appropriate signals"""
        self._attack = attack
        self.signals.reloadAttackParamList.emit()
        self.getAttack().paramListUpdated.connect(self.signals.reloadAttackParamList.emit)
        self.getAttack().setTraceLimits(self.project().traceManager().numTraces(), self.project().traceManager().numPoints())
        self.signals.attackChanged.emit()

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

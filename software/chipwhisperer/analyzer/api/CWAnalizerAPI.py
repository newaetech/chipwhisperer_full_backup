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

__author__ = "Colin O'Flynn"

import importlib
from chipwhisperer.common.api.CWCoreAPI import CWCoreAPI
from chipwhisperer.common.utils import util

class CWAnalizerAPI(CWCoreAPI):

    class Signals(CWCoreAPI.Signals):
        def __init__(self):
            super(CWAnalizerAPI.Signals, self).__init__()
            self.reloadAttackParamList = util.Signal()
            self.attackChanged = util.Signal()

    def __init__(self):
        super(CWAnalizerAPI, self).__init__()
        self.da = None
        self.numTraces = 100
        self.traceLimits = 0
        self.pointLimits = 0
        self.results = None

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

    def openProject(self, fname):
        super(CWAnalizerAPI, self).openProject(fname)
        self.attack.setProject(self.project())

    @staticmethod
    def getPreprocessingModules(dir, tracerManager, waveformWidget):
        resp = {}
        for f in util.getPyFiles(dir):
            try:
                i = importlib.import_module('chipwhisperer.analyzer.preprocessing.' + f)
                mod = i.getClass()(tracerManager, waveformWidget)
                resp[mod.getName()] = mod
            except Exception as e:
                print "Warning: Could not import preprocessing module " + f + ": " + str(e)
        # print "Loaded preprocessing modules: " + resp.__str__()
        return resp
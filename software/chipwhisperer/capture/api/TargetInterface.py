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
from chipwhisperer.common.utils import util

class TargetInterface(object):
    """This is a standard target interface, which controls various supported lower-level hardware interfaces"""

    class Signals(object):
        paramListUpdated = util.Signal()
        targetUpdated = util.Signal()
        connectStatus = util.Signal()
        newInputData = util.Signal()

    signals = Signals()
    toplevel_param = {}
    _driver = None
    __connected = False

    def __init__(self, targetsDir, log = None, showScriptParameter = None):
        self.log = log
        self.addTargetModules(targetsDir, showScriptParameter);

    def addTargetModules(self, targetsDir, showScriptParameter):
        valid_targets = {}
        valid_targets["None"] = None
        for t in util.getPyFiles(targetsDir):
            try:
                i = importlib.import_module('chipwhisperer.capture.targets.' + t)
                name, mod = i.getTarget(self.log, showScriptParameter = showScriptParameter)
                valid_targets[name] = mod
            except Exception as e:
                print str(e)
        # print "Loaded target modules: " + valid_targets.__str__()
        self.toplevel_param = {'name':'Target Module', 'type':'list', 'values':valid_targets, 'value':valid_targets["None"], 'set':self.setDriver}

    def setOpenADC(self, oadc):
        '''Declares OpenADC Instance in use. Only for openadc-integrated targets'''
        try:  # if hasattr(oadc, "scope.sc"):
            self.oadc = oadc.scope.sc
        except AttributeError:
            self.oadc = oadc
        if hasattr(self._driver, "setOpenADC"):
            self._driver.setOpenADC(self.oadc)

    def driverIsLoaded(self):
        return self._driver is not None

    def setConnected(self, value):
        self.__connected = value
        self.signals.connectStatus.emit(self.isConnected())

    def isConnected(self):
        if self.driverIsLoaded():
            return self.__connected
        return False

    def con(self):
        if not self.driverIsLoaded(): raise Exception("Driver not loaded")

        self._driver.con()
        self.setConnected(True)

    def dis(self):
        if not self.driverIsLoaded(): raise Exception("Driver not loaded")

        self._driver.dis()
        self.setConnected(False)

    def getDriver(self):
        return self._driver

    def setDriver(self, driver):
        self._driver = driver
        self.setConnected(False)
        if hasattr(self._driver, "setOpenADC") and hasattr(self, "oadc") and self.oadc:
            self._driver.setOpenADC(self.oadc)

        if self.driverIsLoaded():
            self._driver.paramListUpdated.connect(self.signals.paramListUpdated.emit)
            self._driver.newInputData.connect(self.signals.newInputData.emit)
            self.signals.paramListUpdated.emit(self.getDriver().paramList())
            self.signals.targetUpdated.emit(True)
        else:
            self.signals.paramListUpdated.emit(None)
            self.signals.targetUpdated.emit(False)

    def paramList(self):
        if self.driverIsLoaded():
            return self._driver.paramList()
        return [None]

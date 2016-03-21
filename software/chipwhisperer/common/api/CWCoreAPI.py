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

__author__ = "Colin O'Flynn"

import traceback
import importlib
from chipwhisperer.common.api.ProjectFormat import ProjectFormat
from chipwhisperer.common.utils import util

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

    def __init__(self):
        self.paramTrees = []
        self._traceManager = None
        self._project = None
        self.signals = self.Signals()
        CWCoreAPI.instance = self

    @staticmethod
    def getInstance():
        return CWCoreAPI.instance

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

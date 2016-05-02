#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2016, NewAE Technology Inc
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

from pyqtgraph.parametertree import ParameterTree
from chipwhisperer.common.api.ExtendedParameter import ExtendedParameter, ConfigParameter
import chipwhisperer.common.ui.ParameterTypesCustom  # Do not remove!!
from chipwhisperer.common.utils import util


class CWParameterTree(ParameterTree):
    _helpWidget = None
    _allParameterTrees = util.DictType()
    paramTreeUpdated = util.Signal()

    def __init__(self, groupName="Defaul", parameterizedObjs=None):
        super(CWParameterTree, self).__init__()
        self.parameterizedObjs = []
        self.extend(parameterizedObjs)
        self._allParameterTrees[groupName] = self

    def replace(self, newParameterizedObjs):
        for parameterizedObj in self.parameterizedObjs:
            if parameterizedObj:
                    parameterizedObj.paramListUpdated.disconnect(self.reloadParams)
        self.parameterizedObjs = []
        self.extend(newParameterizedObjs)

    def extend(self, parameterizedObjs):
        if parameterizedObjs:
            for parameterizedObj in parameterizedObjs:
                if parameterizedObj:
                        parameterizedObj.paramListUpdated.connect(self.reloadParams)
                        self.parameterizedObjs.append(parameterizedObj)
        self.reloadParams()

    def reloadParams(self):
        activeParameters = []
        for parameterizedObj in self.parameterizedObjs:
            if parameterizedObj:
                activeParameters.extend(parameterizedObj.paramList())
        ExtendedParameter.reloadParams(activeParameters, self, help_window=self._helpWidget)
        self.paramTreeUpdated.emit()

    @classmethod
    def setHelpWidget(cls, widget):
        cls._helpWidget = widget

    @classmethod
    def getAllParameterTrees(cls):
        return cls._allParameterTrees

    @classmethod
    def getAllGuiActions(cls, mainWindow):
        ret = []
        for parameterTrees in cls.getAllParameterTrees().itervalues():
            if parameterTrees:
                for parameterizedObj in parameterTrees.parameterizedObjs:
                    if parameterizedObj:
                        ret.extend(parameterizedObj.guiActions(mainWindow))
        return ret


class Parameterized(object):
    name = "None"
    description = ""

    def __init__(self, parentParam=None, newName=None):
        if not hasattr(self, "params"):  # It was already called by other subclass (breaks diamond structure)
            self.newName = newName
            self.paramListUpdated = util.Signal()  # Called to refresh the Param List (i.e. new parameters were added)
            self.__activeParams = [lambda: self.lazy(self)]
            self.params = ConfigParameter.create_extended(self, name=self.getName(), type='group', children={})
            if self.description:
                self.params.addChildren([
                     {'name':'Description', 'type':'text', 'value':self.getDescription(), 'readonly':True},
                ])
        if parentParam:
            self.paramListUpdated.connect(parentParam.paramListUpdated.emit)

    def paramList(self):
        # Returns the current active parameters (including the child ones)
        ret = []
        for e in self.__activeParams:
            currentParams = e()
            if currentParams:
                if currentParams == self:
                    ret.append(currentParams.params)
                else:
                    ret.extend(currentParams.paramList())
        return ret

    def getName(self):
        return self.name if not self.newName else self.newName

    def getDescription(self):
        return self.description

    def setupActiveParams(self, params):
        # Use this method to setup the order of the parameterized objects to be shown
        self.__activeParams = params
        self.paramListUpdated.emit()

    def addActiveParams(self, param):
        # Use this method to setup the order of the parameterized objects to be shown
        self.__activeParams.append(param)
        self.paramListUpdated.emit()

    def guiActions(self, mainWindow):
        # Returns a list with all the gui actions in the active parameter tree.
        ret = []
        for e in self.__activeParams:
            currentParams = e()
            if currentParams:
                if currentParams == self:
                    ret.extend(currentParams.setupGuiActions(mainWindow))
                else:
                    ret.extend(currentParams.guiActions(mainWindow))
        return ret

    def setupGuiActions(self, mainWindow):
        """You should overload this. Copy/Paste into your class."""
        # self.window = Window(mainWindow, parameters)
        # return [['Name of the menu item','Description', self.window.show],...]
        return []

    @staticmethod
    def lazy(var):
        # Dummye method to cause late evaluation of the attributes when parameters are passed as argument
        # Ex.: self.setupActiveParams([lambda: lazy(self)])
        return var
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
from chipwhisperer.common.utils import util
from chipwhisperer.common.utils.parameter import Parameter


class CWParameterTree(object):
    _helpWidget = None
    _allParameterTrees = util.DictType()
    sigParamTreeUpdated = util.Signal()

    def __init__(self, groupName="Default", parameterizedObjs=None):
        self.parameterLists = []
        self.extend(parameterizedObjs)
        self._allParameterTrees[groupName] = self

    def getPyQtGraphParamTree(self):
        if not hasattr(self, "PyQtGraphParamTree"):
            self.PyQtGraphParamTree = ParameterTree()
        return self.PyQtGraphParamTree

    def replace(self, parameterLists):
        self.parameterLists = []
        self.extend(parameterLists)
        self.reloadParams()

    def extend(self, parameterLists):
        for parameterList in parameterLists:
            if parameterList:
                self.parameterLists.append(parameterList)
        self.reloadParams()

    def reloadParams(self):
        activeParameters = []
        for parameterList in self.parameterLists:
            if parameterList:
                activeParameters.extend(parameterList.paramList())
        ExtendedParameter.reloadParams(activeParameters, self.getPyQtGraphParamTree(), help_window=self._helpWidget)
        self.sigParamTreeUpdated.emit()


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


# class Parameterized(object):
#     """
#     Creates a new parameterTree group and adds its description
#     """
#     _name = "None"
#     _description = ""
#
#     def __init__(self, parentParam=None, name=None):
#         if not hasattr(self, "_instanceName") and name:  # These IFs avoids bugs in diamond class hierarchy
#             self._instanceName = name
#         if not hasattr(self, "__activeParams"):  # These IFs avoids bugs in diamond class hierarchy
#             self.__activeParams = []
#         if not hasattr(self, "paramListUpdated"):
#             self.paramListUpdated = util.Signal()  # Called to refresh the Param List (i.e. new parameters were added)
#         if parentParam:
#             self.paramListUpdated.connect(parentParam.paramListUpdated.emit)
#         if not hasattr(self, "params"):
#             self.params = ConfigParameter.create_extended(self, name=self.getName(), type='group', children={}, tip=self.getDescription())
#             if self._description:
#                 self.params.addChildren([{'name': '', 'type': 'label', 'value':self.getDescription(), 'readonly':True}])
#
#     def paramList(self):
#         # Returns the current active parameters (including the child ones)
#         ret = []
#         ret.append(self.params)
#         for e in self.__activeParams:
#             currentParams = e()
#             if currentParams:
#                 ret.extend(currentParams.paramList())
#         return ret
#
#     def getName(self):
#         # Returns the custom name (the class name can be overriden by the instance name)
#         return self.getClassName() if not hasattr(self, "_instanceName") else self._instanceName
#
#     @classmethod
#     def getClassName(cls):
#         return cls._name
#
#     @classmethod
#     def getDescription(cls):
#         return cls._description
#
#     def setupActiveParams(self, params):
#         # Use this method to setup the order of the parameterized objects to be shown
#         self.__activeParams = params
#         self.paramListUpdated.emit()
#
#     def addActiveParams(self, param):
#         # Use this method to setup the order of the parameterized objects to be shown
#         self.__activeParams.append(param)
#         self.paramListUpdated.emit()
#
#     def guiActions(self, mainWindow):
#         # Returns a list with all the gui actions in the active parameter tree.
#         ret = []
#         ret.extend(self.setupGuiActions(mainWindow))
#         for e in self.__activeParams:
#             currentParams = e()
#             if currentParams:
#                 ret.extend(currentParams.guiActions(mainWindow))
#         return ret
#
#     def getAllActiveParameters(self):
#         for e in self.__activeParams:
#             currentParameters = e()
#             if currentParameters:
#                 currentParameters.params.getAllParameters()
#
#     def setupGuiActions(self, mainWindow):
#         """You should overload this. Copy/Paste into your class."""
#         # self.window = Window(mainWindow, parameters)
#         # return [['Name of the menu item','Description', self.window.show],...]
#         return []
#
#     @staticmethod
#     def lazy(var):
#         # Dummye method to cause late evaluation of the attributes when parameters are passed as argument
#         # Ex.: self.setupActiveParams([lambda: lazy(self)])
#         return var

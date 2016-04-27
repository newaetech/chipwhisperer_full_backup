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

import inspect
import importlib
import traceback
import util
from chipwhisperer.common.api.config_parameter import ConfigParameter
import os.path
from pyqtgraph.parametertree import ParameterTree
from chipwhisperer.common.api.ExtendedParameter import ExtendedParameter

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

    def __init__(self, parentParam = None):
        self.paramListUpdated = util.Signal()  # Called to refresh the Param List (i.e. new parameters were added)
        self.__activeParams = [lambda: self.lazy(self)]
        if parentParam:
            self.paramListUpdated.connect(parentParam.paramListUpdated.emit)
        self.params = ConfigParameter.create_extended(self, name=self.name, type='group', children=self.setupParameters())

    def setupParameters(self):
        """You should overload this. Copy/Paste into your class."""
        # return [{'name':'Example Parameter', 'type':'int', 'value':5}]  # 'set':self.someFunction
        return []

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
        return self.name

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


loadedItems = []


class Plugin(Parameterized):
    description = "Some description"

    def getDescription(self):
        return self.description

try:
    from PySide.QtCore import *
    from PySide.QtGui import *

    class PluginStatusDialog(QTableWidget):
        def __init__(self, parent=None):
            super(PluginStatusDialog, self).__init__(len(loadedItems), 4, parent=parent)
            self.setWindowFlags(Qt.Window)
            self.setWindowTitle("Plugin Status")
            self.setHorizontalHeaderLabels(["Module", "Enabled", "Error Message", "Details (full stack)"])

            for indx, itm in enumerate(loadedItems):
                self.setItem(indx, 0, QTableWidgetItem(itm[0]))
                self.setItem(indx, 1, QTableWidgetItem(str(itm[1])))
                self.setItem(indx, 2, QTableWidgetItem(itm[2]))
                self.setItem(indx, 3, QTableWidgetItem(itm[3]))
            for y in range(0, len(loadedItems)):
                for x in (0,1,2,3):
                    self.item(y, x).setFlags(self.item(y, x).flags() ^ Qt.ItemIsEditable)

            self.resize(950, 400)
            self.horizontalHeader().setStretchLastSection(True)
            self.resizeColumnsToContents()
            self.resizeRowsToContents()
            self.show()
            self.raise_()

except ImportError:
    pass

def getPluginsInDictFromPackage(path, instantiate, addNone, *args, **kwargs):
    modules = importModulesInPackage(path)
    classes = getPluginClassesFromModules(modules)
    items = putInDict(classes, instantiate, *args, **kwargs)
    if addNone:
        items["None"] = None
    return module_reorder(items)


def importModulesInPackage(path):
    resp = []
    for package_name in util.getPyFiles(os.path.join(util.getRootDir(), (os.path.normpath(path).replace(".", "/")))):#   (os.path.normpath(path).replace(".", "/"))):
        full_package_name = '%s.%s' % (path, package_name)
        try:
            resp.append(importlib.import_module(full_package_name))
        except Exception as e:
            print "INFO: Could not import module: " + full_package_name + ": " + str(e)
            loadedItems.append([full_package_name, False, str(e), traceback.format_exc()])
    return resp


def getPluginClassesFromModules(modules):
    resp = []
    for module in modules:
        clsmembers = inspect.getmembers(module, lambda member: inspect.isclass(member) and member.__module__ == module.__name__)
        for clsName, clsMember in clsmembers:
            if issubclass(clsMember, Plugin) and (not clsName.startswith('_')):
                resp.append(clsMember)
    return resp


def putInDict(items, instantiate, *args, **kwargs):
    resp = util.DictType()
    for c in items:
        try:
            if instantiate:
                item = c(*args, **kwargs)
            else:
                item = c
            resp[item.name] = item
            loadedItems.append([str(c), True, "", ""])
        except Exception as e:
            print "INFO: Could not instantiate module " + str(c) + ": " + str(e)
            loadedItems.append([str(c), False, str(e), traceback.format_exc()])

    if len(resp) == 0:
        print "Warning: Dictionary contains zero modules"
    return resp

def module_reorder(resp):
    #None is first, then alphabetical
    newresp = util.DictType()
    if 'None' in resp:
        newresp['None'] = resp['None']
        del resp['None']
    newresp.update(sorted(resp.items(), key=lambda t: t[0]))
    return newresp

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
import Util
from chipwhisperer.common.api.config_parameter import ConfigParameter
import os.path


class Parameterized(object):
    name = "None"

    def __init__(self, parentParam = None):
        self.paramListUpdated = Util.Signal()  # Called to refresh the Param List (i.e. new parameters were added)
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


class Plugin(Parameterized):
    description = "Some description"

    def getDescription(self):
        return self.description


def getPluginsInDictFromPackage(path, instantiate, addNone, *args, **kwargs):
    modules = importModulesInPackage(path)
    classes = getPluginClassesFromModules(modules)
    items = Util.putInDict(classes, instantiate, *args, **kwargs)
    if addNone:
        items["None"] = None
    return module_reorder(items)


def importModulesInPackage(path):
    resp = []
    for package_name in Util.getPyFiles(os.path.join(Util.getRootDir(), (os.path.normpath(path).replace(".", "/")))):#   (os.path.normpath(path).replace(".", "/"))):
        full_package_name = '%s.%s' % (path, package_name)
        try:
            resp.append(importlib.import_module(full_package_name))
        except Exception as e:
            print "INFO: Could not import module: " + full_package_name + ": " + str(e)
    return resp


def getPluginClassesFromModules(modules):
    resp = []
    for module in modules:
        clsmembers = inspect.getmembers(module, lambda member: inspect.isclass(member) and member.__module__ == module.__name__)
        for clsName, clsMember in clsmembers:
            if issubclass(clsMember, Plugin) and (not clsName.startswith('_')):
                resp.append(clsMember)
            else:
                pass
                # print "INFO: Module " + module.__name__ + " has no top level method called getClass(). Ignoring it..."
    return resp


def module_reorder(resp):
    #None is first, then alphabetical
    newresp = Util.DictType()
    if 'None' in resp:
        newresp['None'] = resp['None']
        del resp['None']
    newresp.update(sorted(resp.items(), key=lambda t: t[0]))
    return newresp

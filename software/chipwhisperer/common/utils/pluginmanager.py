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

import importlib
import inspect
import os.path
import traceback

import imp

import util
from chipwhisperer.common.api.settings import Settings

loadedItems = []


class Plugin(object):
    # Just a Marker Interface so the plugins can be identified and load from the package
    pass


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
    normPath = (os.path.normpath(path).replace(".", "/"))
    packages = util.getPyFiles(os.path.join(util.getRootDir(), normPath))
    for package_name in packages:
        full_package_name = '%s.%s' % (path, package_name)
        try:
            resp.append(importlib.import_module(full_package_name))
        except Exception as e:
            print "INFO: Could not import module: " + full_package_name + ": " + str(e)
            loadedItems.append([full_package_name, False, str(e), traceback.format_exc()])

    files = util.getPyFiles(os.path.join(Settings().value("project-home-dir"), normPath), extension=True)
    for file in files:
        full_package_name = '%s.%s' % (path, os.path.splitext(file)[0])
        full_file_path = os.path.join(Settings().value("project-home-dir"), normPath)+"/"+file
        try:
            resp.append(imp.load_source(full_package_name, full_file_path))
        except Exception as e:
            print "INFO: Could not import user module: " + full_file_path + ": " + str(e)
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
                resp[item.getName()] = item
            else:
                item = c
                resp[item.getClassName()] = item
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

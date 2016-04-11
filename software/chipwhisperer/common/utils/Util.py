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

import ast
import os.path
import collections
import shutil
import sys
import importlib

try:
    # OrderedDict is new in 2.7
    from collections import OrderedDict
    DictType = OrderedDict
except ImportError:
    DictType = dict

def getModulesInDictFromPackage(path, instantiate, *args, **kwargs):
    modules = importModulesInPackage(path)
    classes = getMainClassesFromModules(modules)
    dictInstances = putInDict(classes, instantiate, *args, **kwargs)
    return module_reorder(dictInstances)


def getRootDir():
    path = os.path.join(os.path.dirname(__file__), "../../../")
    return os.path.normpath(path)


def importModulesInPackage(path):
    resp = []
    for package_name in getPyFiles(os.path.join(getRootDir(), (os.path.normpath(path).replace(".", "/")))):#   (os.path.normpath(path).replace(".", "/"))):
        full_package_name = '%s.%s' % (path, package_name)
        try:
            resp.append(importlib.import_module(full_package_name))
        except Exception as e:
            print "INFO: Could not import module: " + full_package_name + ": " + str(e)
    return resp


def getMainClassesFromModules(modules):
    resp = []
    for module in modules:
        if hasattr(module, "getClass"):
            resp.append(module.getClass())
        else:
            pass
            # print "INFO: Module " + module.__name__ + " has no top level method called getClass(). Ignoring it..."
    return resp


def putInDict(items, instantiate, *args, **kwargs):
    resp = DictType()
    for c in items:
        try:
            if instantiate:
                item = c(*args, **kwargs)
            else:
                item = c
            resp[item.name] = item
        except Exception as e:
            print "INFO: Could not instantiate module " + str(c) + ": " + str(e)
    return resp


def module_reorder(resp):
    #None is first, then alphabetical
    newresp = DictType()
    if 'None' in resp:
        newresp['None'] = resp['None']
        del resp['None']
    newresp.update(sorted(resp.items(), key=lambda t: t[0]))
    return newresp


def copyFile(source, destination, keepOriginals = True):
    if keepOriginals:
        shutil.copy2(source, destination)
    else:
        shutil.move(source, destination)


def strippedName(fullFileName):
    (filepath, filename) = os.path.split(fullFileName)
    (base, toplevel) = os.path.split(filepath)
    return toplevel + "/" + filename


def appendAndForwardErrorMessage(msg, e):
    raise type(e)(msg + "\n  -> " + str(e))


def list2hexstr(data, delim='', prefix=''):
    """
    Convert a list of integers to a hex string, with optional deliminators/prefix

    delim is inserted between each list item

    prefix is inserted infront of each item (including first item)
    """

    rstr = ["%02x" % t for t in data]
    rstr = (delim + prefix).join(rstr)
    rstr = prefix + rstr
    return rstr


def hexstr2list(data):
    """Convert a string with hex numbers into a list of numbers"""

    data = str(data)

    newdata = data.lower()
    newdata = newdata.replace("0x", "")
    newdata = newdata.replace(",", "")
    newdata = newdata.replace(" ", "")
    newdata = newdata.replace("[", "")
    newdata = newdata.replace("]", "")
    newdata = newdata.replace("(", "")
    newdata = newdata.replace(")", "")
    newdata = newdata.replace("{", "")
    newdata = newdata.replace("}", "")
    newdata = newdata.replace(":", "")
    newdata = newdata.replace("-", "")

    datalist = [int(newdata[i:(i + 2)], 16) for i in range(0, len(newdata), 2)]

    return datalist


def strListToList(strlist):
    """
    Convert string in form of '"[33, 42, 43]", "[24, 43, 4]"'
    into a normal list.
    """

    strlist = strlist.replace('"', '')
    strlist = strlist.replace("'", "")
    try:
        listeval = ast.literal_eval(strlist)
        return listeval
    except ValueError:
        raise ValueError("Failed to convert %s to list" % (strlist))


def convert_to_str(data):
    """
    Converts all dictionary elements to string type - similar to what ConfigObj will
    be doing when it saves and loads the data.
    """
    if isinstance(data, collections.Mapping):
        return dict(map(convert_to_str, data.iteritems()))
    elif isinstance(data, collections.Iterable) and not isinstance(data, basestring):
        return type(data)(map(convert_to_str, data))
    else:
        return str(data)


def hexStrToByteArray(hexStr):
    ba = bytearray()
    for s in hexStr.split():
        ba.append(int(s, 16))
    return ba


def getPyFiles(dir):
    scriptList = []
    if os.path.isdir(dir):
        for fn in os.listdir(dir):
            fnfull = dir + '/' + fn
            if os.path.isfile(fnfull) and fnfull.lower().endswith('.py') and (not fnfull.endswith('__init__.py')) and (not fn.startswith('_')):
                scriptList.append(os.path.splitext(fn)[0])
    return scriptList


class Signal(object):
    def __init__(self):
        self.observers = []

    def connect(self, observer):
        if observer not in self.observers:
            self.observers.append(observer)

    def disconnect(self, observer):
        self.observers.remove(observer)

    def emit(self, *arg):
        for observer in self.observers:
            try:
                observer(*arg)
            except Exception, e:
                etype, value, trace = sys.exc_info()
                value = "Exceptions should not escape from observers.\nReceived %s(\"%s\") from %s." % \
                        (type(e).__name__, e, observer)
                sys.excepthook(etype, value, trace)

    def disconnectAll(self):
        self.observers = []


class Observable(Signal):
    def __init__(self, value):
        super(Observable, self).__init__()
        self.data = value

    def setValue(self, value):
        if value != self.data:
            self.data = value
            self.emit()

    def value(self):
        return self.data

# def delete_keys_from_dict(dict_del, lst_keys):
#     for k in lst_keys:
#         try:
#             del dict_del[k]
#         except KeyError:
#             pass
#     for v in dict_del.values():
#         if isinstance(v, dict):
#             delete_keys_from_dict(v, lst_keys)
#
# def delete_objects_from_dict(d):
#     #todel = []
#     for key, value in d.iteritems():
#         if isinstance(value, (dict, list, tuple)):
#             delete_objects_from_dict(value)
#
#         elif inspect.ismethod(value) or isinstance(value, QObject):
#             #todel.append(key)
#             #print key
#             d[key] = value.__class__.__name__

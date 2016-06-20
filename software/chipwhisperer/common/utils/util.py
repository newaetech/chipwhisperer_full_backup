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
import collections
import os.path
import shutil
import sys
import weakref
import functools

try:
    # OrderedDict is new in 2.7
    from collections import OrderedDict
    DictType = OrderedDict
except ImportError:
    DictType = dict


def getRootDir():
    path = os.path.join(os.path.dirname(__file__), "../../../")
    return os.path.normpath(path)


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


def getPyFiles(dir, extension=False):
    scriptList = []
    if os.path.isdir(dir):
        for fn in os.listdir(dir):
            fnfull = dir + '/' + fn
            if os.path.isfile(fnfull) and fnfull.lower().endswith('.py') and (not fnfull.endswith('__init__.py')) and (not fn.startswith('_')):
                if extension:
                    scriptList.append(fn)
                else:
                    scriptList.append(os.path.splitext(fn)[0])
    return scriptList

def _make_id(target):
    if hasattr(target, '__func__'):
        return (id(target.__self__), id(target.__func__))
    return id(target)


class Signal(object):
    def __init__(self):
        self.callbacks = {}  #observing object ID -> weak ref, methodNames

    def connect(self, observer):
        if not callable(observer):
            raise TypeError('Expected method, got %s' % observer.__class__)

        if hasattr(observer, '__self__') and hasattr(observer, '__func__'):
            obj = observer.__self__
        else:
            obj = observer
        ID = _make_id(obj)
        if ID in self.callbacks:
            s = self.callbacks[ID][1]
        else:
            wr = weakref.ref(obj, self.disconnect)
            s = set()
            self.callbacks[ID] = (wr, s)
        try:
            s.add(observer.__name__)
        except AttributeError:
            s.add('<partial>')

    def disconnect(self, observer):
        try:
            if hasattr(observer, '__self__') and hasattr(observer, '__func__'):
                obj = observer.__self__
            else:
                obj = observer
            ID = _make_id(obj)
            if ID in self.callbacks:
               self.callbacks[ID][1].discard(observer.__name__)
        except ValueError:
            pass

    def disconnectAll(self):
        self.callbacks = {}  #observing object ID -> weak ref, methodNames

    def emit(self, *arg, **kw):
        callbacks = self.callbacks.keys()
        for ID in callbacks:
            wr, methodNames = self.callbacks[ID]
            obj = wr()
            if obj is not None:
                for methodName in methodNames:
                    if methodName == '<lambda>':
                        obj(*arg, **kw)
                    elif  methodName == '<partial>':
                        obj(*arg, **kw)
                    else:
                        getattr(obj, methodName)(*arg, **kw)


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


_uiupdateFunction = None

def setUIupdateFunction(func):
    global _uiupdateFunction
    _uiupdateFunction= func

def updateUI():
    if _uiupdateFunction:
        _uiupdateFunction()
#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2014, NewAE Technology Inc
# All rights reserved.
#
# Author: Colin O'Flynn
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


# class Settings(object):
#     def __init__(self):
#         self.dictionary = {}
#
#     def value(self, key):
#         if(self.dictionary.has_key(key)):
#             return self.dictionary[key]
#         else:
#             return ""
#
#     def setValue(self, key, value):
#         self.dictionary[key] = value
#
#     def clear(self):
#         self.dictionary.clear()
#
#
# globalSettings = Settings()

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
            if os.path.isfile(fnfull) and fnfull.lower().endswith('.py') and (not fnfull.endswith('__init__.py')):
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
            observer(*arg)

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

    # value = property(fget=setValue, fset=value)

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

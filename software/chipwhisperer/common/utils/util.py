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
    return scriptList;

class Signal:
    callback = None

    def emit(self, value):
        if self.callback is not None:
            self.callback(value)

    def connect(self, callback):
        self.callback = callback
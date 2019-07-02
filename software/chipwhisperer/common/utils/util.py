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
import weakref
import numpy as np
from functools import wraps
import warnings

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
        return dict(list(map(convert_to_str, iter(list(data.items())))))
    elif isinstance(data, collections.Iterable) and not isinstance(data, str):
        return type(data)(list(map(convert_to_str, data)))
    else:
        return str(data)


def hexStrToByteArray(hexStr):
    ba = bytearray(hexstr2list(hexStr))
    return ba


def binarylist2bytearray(bitlist, nrBits=8):
    ret = []
    pos = 0
    while pos <= len(bitlist) - nrBits:
        out = 0
        for bit in range(nrBits):
            out = (out << 1) | bitlist[pos + bit]
        ret.append(out)
        pos += nrBits
    return ret


def bytearray2binarylist(bytes, nrBits=8):
    init = np.array([], dtype=bool)
    for byte in bytes:
        init = np.concatenate((init, np.unpackbits(np.uint8(byte))[8 - nrBits:]), axis=0)
    return init


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
        return (id(target.__self__))
    return id(target)


class Signal(object):
    class Cleanup(object):
        def __init__(self, key, d):
            self.key = key
            self.d = d

        def __call__(self, wr):
            del self.d[self.key]

    def __init__(self):
        self.callbacks = {}  #observing object ID -> weak ref, methodNames

    def connect(self, observer):
        if not callable(observer):
            raise TypeError('Expected a method, got %s' % observer.__class__)

        ID = _make_id(observer)
        if ID in self.callbacks:
            s = self.callbacks[ID][1]
        else:
            try:
                target = weakref.ref(observer.__self__, Signal.Cleanup(ID, self.callbacks))
            except AttributeError:
                target = None
            s = set()
            self.callbacks[ID] = (target, s)

        if hasattr(observer, "__func__"):
            method = observer.__func__
        else:
            method = observer
        s.add(method)

    def disconnect(self, observer):
        ID = _make_id(observer)
        if ID in self.callbacks:
            if hasattr(observer, "__func__"):
                method = observer.__func__
            else:
                method = observer
            self.callbacks[ID][1].discard(method)
            if len(self.callbacks[ID][1]) == 0:
                del self.callbacks[ID]
        else:
            pass

    def disconnectAll(self):
        self.callbacks = {}  # observing object ID -> weak ref, methods

    def emit(self, *args, **kwargs):
        callbacks = list(self.callbacks.keys())
        for ID in callbacks:
            try:
                target, methods = self.callbacks[ID]
            except KeyError:
                continue
            for method in methods.copy():
                if target is None:  # Lambda or partial
                    method(*args, **kwargs)
                else:
                    targetObj = target()
                    if targetObj is not None:
                        method(targetObj, *args, **kwargs)


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


_consoleBreakRequested = False
class ConsoleBreakException(BaseException):
    """Custom exception class. Raised when pressing ctrl-C in console.

    This inherits from BaseException so that the generic "Save project?" window
    doesn't catch it.
    """
    pass

def requestConsoleBreak():
    global _consoleBreakRequested
    _consoleBreakRequested = True

_uiupdateFunction = None

def setUIupdateFunction(func):
    global _uiupdateFunction
    _uiupdateFunction= func

def updateUI():
    if _uiupdateFunction:
        _uiupdateFunction()

    # If an event handler has asked for a console break, raise an exception now
    global _consoleBreakRequested
    if _consoleBreakRequested:
        _consoleBreakRequested = False
        raise ConsoleBreakException()

class WeakMethod(object):
    """A callable object. Takes one argument to init: 'object.method'.
    Once created, call this object -- MyWeakMethod() --
    and pass args/kwargs as you normally would.
    """
    def __init__(self, object_dot_method, callback=None):
        try:
            if callback is None:
                self.target = weakref.ref(object_dot_method.__self__)
            else:
                self.target = weakref.ref(object_dot_method.__self__, callback)
            self.method = object_dot_method.__func__
        except AttributeError:
            self.target = None
            self.method = object_dot_method

    def __call__(self, *args, **kwargs):
        """Call the method with args and kwargs as needed."""
        if self.is_dead():
            raise TypeError('Method called on dead object')
        if self.target is None:  # Lambda or partial
            return self.method(*args, **kwargs)
        else:
            return self.method(self.target(), *args, **kwargs)

    def is_dead(self):
        '''Returns True if the referenced callable was a bound method and
        the instance no longer exists. Otherwise, return False.
        '''
        return self.target is not None and self.target() is None


class Command:
    """Converts a method call with arguments to be ignored in a simple call with no/fixed arguments (replaces lambda)"""
    def __init__(self, callback, *args, **kwargs):
        self.callback = WeakMethod(callback)
        self.args = args
        self.kwargs = kwargs

    def __call__(self, *args, **kwargs):
        return self.callback(*self.args, **self.kwargs)

if __name__ == '__main__':
    class test(object):
        def m(self):
            print("here")

        def __del__(self):
            print("deleted")

    x = test()
    y = x.m
    x = None
    y()

class DisableNewAttr(object):
    """Provides an ability to disable setting new attributes in a class, useful to prevent typos.

    Usage:
    1. Make a class that inherits this class:
    >>> class MyClass(DisableNewAttr):
    >>>     # Your class definition here

    2. After setting up all attributes that your object needs, call disable_newattr():
    >>>     def __init__(self):
    >>>         self.my_attr = 123
    >>>         self.disable_newattr()

    3. Subclasses raise an AttributeError when trying to make a new attribute:
    >>> obj = MyClass()
    >>> #obj.my_new_attr = 456   <-- Raises AttributeError
    """

    def __init__(self):
        self.disable_newattr()

    def disable_newattr(self):
        self._new_attributes_disabled = True

    def enable_newattr(self):
        self._new_attributes_disabled = False

    def __setattr__(self, name, value):
        if hasattr(self, '_new_attributes_disabled') and self._new_attributes_disabled and not hasattr(self, name):  # would this create a new attribute?
            raise AttributeError("Attempt to set unknown attribute in %s"%self.__class__, name)
        super(DisableNewAttr, self).__setattr__(name, value)


def dict_to_str(input_dict, indent=""):
    """Turn a dictionary of attributes/status values into a pretty-printed
    string for use on the command line. Recursively pretty-prints dictionaries.

    This function is most useful with OrderedDicts as it keeps the same
    printing order.
    """

    # Find minimum width that fits all names
    min_width = 0
    for n in input_dict:
        min_width = max(min_width, len(str(n)))

    # Build string
    ret = ""
    for n in input_dict:
        if type(input_dict[n]) in (dict, OrderedDict):
            ret += indent + str(n) + ' = '
            ret += '\n' + dict_to_str(input_dict[n], indent+"    ")
        else:
            ret += indent + str(n).ljust(min_width) + ' = '
            ret += str(input_dict[n]) + '\n'

    return ret


class cw_bytearray(bytearray):
    """Overwrites the __repr__ and __str__ methods of the builtin bytearray class
    so it prints without trying to turn everything into ascii characters

    It should be usable like the builtin bytearray class in all other regards
    """

    def __repr__(self):
        return "CWbytearray({})".format([hex(c)[-2:] for c in self])

    def __str__(self):
        return self.__repr__()


class NoneTypeScope(object):
    """Raises an intelligible error related to scope disconnect when any attribute is accessed
    """

    def __getattr__(self, item):
        raise AttributeError('Scope has not been connected')


class NoneTypeTarget(object):
    """Raises an intelligble error related to target disconnect when any attribute is accessed
    """
    def __getattr__(self, item):
        raise AttributeError('Target has not been connected')


def camel_case_deprecated(func):
    """Wrapper function to deprecate camel case functions.

    This is not a decorator, do not use it that way. This way of deprecating
    allows the changing of the function definition name and then using this
    wrapper to define the camel case function, including a usage warning. To
    use first refactor the camelCase function definition to snake case, then
    use the wrapper on the snake_case function and assign it to the camelCase
    function name. It is best shown with an example:
    Before:
    .. code::
        def fooBar():
            pass

    After:
    .. code::
        def foo_bar():
            pass

        fooBar = camel_case_deprecated(foo_bar)

    Advantages of this method include being able to change the camelCase function
    to snake_case right away and keeping backwards compatibility, as well as
    supporting arbitrary amount of arguments and keyword arguments and keeping
    docstrings in tact.

    Args:
        func: The now snake_case function

    Returns: The wrapped snake_case function which now raises a warning during
        usage.
    """

    def underscore_to_camelcase(value):
        # .. function author:: Dave Webb: Stack overflow

        def camelcase():
            yield str.lower
            while True:
                yield str.capitalize

        c = camelcase()
        return ''.join(next(c)(x) if x else '_' for x in value.split('_'))

    cc_func = underscore_to_camelcase(func.__name__)

    @wraps(func)
    def wrapper(*args, **kwargs):
        warnings.warn('{} function is deprecated use {} instead.'.format(cc_func, func.__name__))
        return func(*args, *kwargs)

    wrapper.__name__ = underscore_to_camelcase(func.__name__)
    wrapper.__doc__ = 'Deprecated: Use {} instead.'.format(func.__name__)
    return wrapper


def get_cw_type(sn=None):
    """ Gets the scope type of the connected ChipWhisperer

    If multiple connected, sn must be specified
    """
    from chipwhisperer.hardware.naeusb.naeusb import NAEUSB
    from chipwhisperer.capture import scopes

    cwusb = NAEUSB()
    possible_sn = cwusb.get_possible_devices(idProduct=None)
    name = ""

    if (len(possible_sn) > 1):
        if sn is None:
            serial_numbers = []
            for d in possible_sn:
                serial_numbers.append("sn = {} ({})".format(str(d['sn']), str(d['product'])))
            raise Warning("Multiple chipwhisperers connected, but device and/or serial number not specified.\nDevices:\n{}".format(serial_numbers))
        else:
            for d in possible_sn:
                if d['sn'] == sn:
                    name = d['product']
    else:
        name = possible_sn[0]['product']
    if name == "ChipWhisperer Lite":
        return scopes.OpenADC
    elif name == "ChipWhisperer Nano":
        return scopes.CWNano

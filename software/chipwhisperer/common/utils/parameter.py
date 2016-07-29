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
# =================================================
import logging
import sys
import copy
import weakref

from pyqtgraph.parametertree import Parameter as pyqtgraphParameter
from chipwhisperer.common.utils import util
import functools
import chipwhisperer.common.ui.ParameterTypesCustom  # Do not remove!!!


class Parameterized(object):
    """
    Abstract class that implements basic functionality required by parameterized objects.
    All parameterized objects should have _name overriden in the class. The objects can also override this attribute
     with different names in the case of having two or more instances. The _description is optional.
    """
    _name = "None"
    _description = ""

    def getParams(self):
        """Return a parameter for the object. Creates it in the first time it is executed."""
        if not hasattr(self, "params"):
            self.params = Parameter(name=self.getName(), type='group')
            if self._description != "":
                self.params.addChildren(
                    [{'name': '', 'type': 'label', 'value': self.getDescription(), 'readonly': True}])
        return self.params

    def findParam(self, nameOrPath):
        """Return the paramenter child with the given name/path."""
        return self.getParams().getChild(nameOrPath)

    def getName(self):
        """Return the final name (instance name)."""
        return self._name

    @classmethod
    def getClassName(cls):
        """Return the original name (class name)."""
        return cls._name

    @classmethod
    def getDescription(cls):
        """Return the description of the parameterized class."""
        return cls._description

    def deleteParams(self):
        """Delete its parameters. Helps the GC in doing its work."""
        if hasattr(self, "params"):
            self.getParams().delete()
            del self.params


class Parameter(object):
    """
    Basic unity of data (like an attribute of a class) that can accessed in a more uniform way (like using search) and
    displayed in the screen with advanced widgets if QT+PyQtGraph is supported.
    Each parameter has a name, a type, a value, and several other properties that modify the behavior. The value can
    be saved internally to the Parameter or externally, using get/set methods. An action function is called every time
    the value of the parameter changes.

    Supported types:
    "group"               - A label with children
    "list"                - A value to be selected between a set of allowed values
    "label"               - Used in the description of Parameterized objects
    "str", "text"         - A smaller and a larger textbox
    "bool"                - A checkbox
    "action"              - A button
    "int", "float"        - A value to be selected between a min and max limit
    "range", "rangegraph" - An interval and an interval that can be selected using a graph widget (graphwidget option)
    "file"                - A string type with a file dialog button
    "filelist"            - Complex widged with a file list and mangement buttons to add/remove/edit/copy/set active.
     "color"              - Opens a color seletion dialog
     "menu"               - Hiden widget that inserts a new menu option

    Supported Attributes:
    "name", "type"        - Mandatory attributes
    "key"                 - A nickname to make search easier
    "values"              - Allowed set of values (list or dictionary type)
    "value"               - Value saved internally
    "set", "get"          - Value save externally
    "limits"              - Range of allowed values
    "step"                - Increment in SpinBoxes
    "linked"              - Refreshes (calls set(get)) the values in the given list with other parameters
    "default"             - Set the initial value (skips initialization when using set/get)
    "tip"                 - Description of the parameter
    "action"              - Calls the provided method with the current paramenter as argument when the value is changed
    "visible"             - Show/hides the parameter
    "children"            - Insert other parameters to be accessed within the current parameter hierarchy
    "readonly"            - Prevents the user of changing its value (it can be forced though)
    "help"                - Text displayed when clicking the help button
    "graphwidget"         - Reference to the graph widget when using parameters with type "rangegraph"
    'siPrefix', 'suffix'  - Adds prefix and sets the suffix text
    "psync"               - Disable reverse synchronization when calling the set method directly (no decorator needed)
    "addLoadSave"         - Adds load / save settings button to a group parameter
    ...

    Examples:
        self.getParams().addChildren([
            {'name':'Redraw after Each', 'type':'bool', 'value':False},
            {'name':'Trace Range', 'key':'tracerng', 'type':'range', 'limits':(0, 0), 'value':(0, 0)},
            {'name':'Point Range', 'key':'pointrng', 'type':'rangegraph', 'limits':(0, 0), 'value':(0, 0), 'graphwidget':self},
            {'name':'Redraw', 'type':'action', 'action':lambda _: self.plotInputTrace()},
            {'name':'Y-Offset', 'key':'yoffset', 'type':'float', 'step':1E-3, 'siPrefix': True, 'suffix': 'V'},
            {'name':'Update Mode', 'key':'updateMode', 'type':'list', 'values':{'Entire Table (Slow)':'all',
             'PGE Only (faster)':'pge'}, 'get':self.getUpdateMode, 'set':self.setUpdateMode},
            {'name':'Trace color', 'type':'color', 'value':"0F0", 'action':lambda p: self.setTraceColor(p.getValue())},
            {'name':'Draw Type', 'type':'list', 'key':'drawtype', 'values':['Fastest', 'Normal', 'Detailed'], 'value':'Normal',
                                 'help':"Draw Type Help"},
            {'name':'CW Firmware Preferences','tip':'Configure ChipWhisperer FW Paths', 'type':"menu",
             "action":lambda _:self.getFwLoaderConfigGUI.show()},
            #  "default" means set() is NOT called during init, otherwise set WILL be called to set the default value (read from get()). Can be useful when don't want to read hardware at start-up
            {'name':'SAD Threshold', 'type':'int', 'limits':(0, 100000), 'default':0, 'set':self.setThreshold, 'get':self.getThreshold}
            {'name':'Acquisition Settings', 'type':'group', 'children':[
                    {'name':'Number of Traces', 'type':'int', 'limits':(1, 1E9), 'get':self.getNumTraces, 'set':self.setNumTraces, 'linked':['Traces per Set']},
                    {'name':'Number of Sets', 'type':'int', 'limits':(1, 1E6), 'get':self.getNumTraceSets, 'set':self.setNumTraceSets, 'linked':['Traces per Set'], 'tip': 'Break acquisition into N sets, '
                     'which may cause data to be saved more frequently. The default capture driver requires that NTraces/NSets is small enough to avoid running out of system memory '
                     'as each segment is buffered into RAM before being written to disk.'},
                    {'name':'Traces per Set', 'type':'int', 'readonly':True, 'get':self.tracesPerSet},
            ]}
        ])
    """

    sigParametersChanged = util.Signal()
    registeredParameters = {}
    scriptingOutput = sys.stdout
    supportedTypes = ["group", "list", "label", "str", 'text', "bool", "action", "int", "float", "rangegraph", "file",
                      'filelist', "range", "color", "menu"]
    suppertedAttributes = {"name", "key", "type", "values", "value", "set", "get", "limits", "step", "linked",
                           "default", "tip", "action", "visible", "children", "readonly", "graphwidget"}
    usePyQtGraph = False

    def __init__(self, parent=None, ignoreChildren=False, **opts):
        self.sigValueChanged = util.Signal()
        self.sigLimitsChanged = util.Signal()
        self.sigOptionsChanged = util.Signal()
        self.sigChildAdded = util.Signal()
        self.sigChildRemoved = util.Signal()

        self.previousValue = None
        self.parent = parent
        self.invalid = False
        self.opts = {"visible": True}
        self.opts.update(opts)

        if 'name' not in self.opts or not isinstance(self.opts['name'], basestring):
            raise Exception("Parameter must have a name.")

        name = self.opts["name"]
        if 'type' not in self.opts or not isinstance(self.opts['type'], basestring) or self.opts[
            'type'] not in Parameter.supportedTypes:
            raise Exception("Parameter \"%s\" must have a valid string type." % name)

        # for opt in opts:
        #     if opt not in Parameter.suppertedAttributes:
        #         raise Exception("Parameter \"%s\" has unknown attribute type: %s." % (name, opt))

        if self.opts['type'] != 'group':
            if (('set' in self.opts) or ('get' in self.opts)) and ('value' in self.opts):
                raise Exception(
                    "Use set/get or value, not both simultaneously in parameter \"%s\". If an action is needed, use the action option." % name)

            if not (
                                'set' in self.opts or 'get' in self.opts or 'value' in self.opts or 'action' in self.opts or 'linked' in self.opts):
                raise Exception(
                    "Useless parameter \"%s\" because no set/get/value/action/linked option is defined." % name)

            if 'set' in self.opts and (not ('get' in self.opts)):
                raise Exception("Option set and get should be used together in parameter \"%s\"." % name)

            if 'get' in self.opts and (not 'set' in self.opts) and ('readonly' in self.opts) and self.opts[
                'readonly'] == False:
                raise Exception("Parameters \"%s\" has get and no set. Should be marked as readonly." % name)

            if self.opts.get("type", None) == "list":
                self.opts['limits'] = opts['values']

            if 'set' in self.opts:
                self.sigValueChanged.connect(self.opts['set'])
                self.opts['set'] = None

            if 'get' in self.opts:
                self.opts['get'] = util.WeakMethod(self.opts['get'])

            if 'action' in self.opts:
                self.opts['action'] = util.WeakMethod(self.opts['action'])

            if "default" not in self.opts:
                self.opts["default"] = self.getValue()
                self.setValue(self.opts["default"], init=True)
        else:
            if 'addLoadSave' in self.opts and self.opts["addLoadSave"]:
                self.opts["addLoadSave"] = (self.load, self.save)

        self.childs = []
        self.ignoredChildren = self.opts.pop("children", [])
        if Parameter.usePyQtGraph and self.opts["type"] != "menu":
            self.setupPyQtGraphParameter()
        self.keys = {}
        if ignoreChildren is False:
            self.addChildren(self.ignoredChildren)
            self.ignoredChildren = []

    def getName(self):
        return self.opts["name"]

    def getType(self):
        return self.opts["type"]

    def getTip(self):
        return self.opts["tip"]

    def getAction(self):
        return self.opts["action"]

    def getOpts(self):
        return self.opts

    def getValue(self, default=None):
        """Return the internal or external value of the parameter"""
        val = self.opts.get("get", None)
        if val is None:
            return self.opts.get("value", default)
        else:
            return val()

    def getKeyFromValue(self, value):
        """Return the key used to set list type parameters"""
        if self.opts["type"] == "list":
            limits = self.opts["limits"]
            if isinstance(limits, dict):
                try:
                    return limits.keys()[limits.values().index(value)]
                except ValueError:
                    ValueError(
                        "Error: Value " + value + " is not valid in Parameter \"" + self.getName() + "\". Options are: " + str(
                            limits))
        return value

    def getValueKey(self):
        """Return the key used to set list type parameters"""
        return self.getKeyFromValue(self.getValue())

    def addChildren(self, children):
        """Add a list of children to the current paramenter"""
        addedChildren = []
        for child in children:
            addedChildren.append(Parameter(self, ignoreChildren=True, **child))
            self.append(addedChildren[-1])
        for child in addedChildren:  # Prevent children being added out of order
            child.addChildren(child.ignoredChildren)
            child.ignoredChildren = []

    def append(self, child):
        """Add one child"""
        if child is None:
            return

        if child.getName() in self.keys:
            self.keys[child.getName()].remove()

        self.keys[child.getName()] = child
        self.childs.append(child)

        if 'key' in child.getOpts():
            self.keys[child.getOpts()["key"]] = child
        child.setParent(self)
        if child.opts["type"] != "menu":
            self.sigChildAdded.emit(child)
        self.sigParametersChanged.emit()

    def setValue(self, value, blockSignal=None, blockAction=False, init=False, ignoreReadonly=False, echo=True,
                 addToList=False):
        """
        Set the parameter value. External values are updated using signals.

        Arguments:
        ignoreReadonly - force readonly parameter to be updated.
        blockSignal    - used to prevent already changed valued of being set again, causing infinite loops.
        blockAction    - prevents action callback of being called.
        init           - used internally to initialize the parameter.
        echo           - enables/disables broadcasting the changes.
        addToList      - add given value to list of valid values if not already present
        """
        if not ignoreReadonly and not init and self.readonly():
            raise ValueError("Parameter \"%s\" is currently set to read only." % self.getName())
        limits = self.opts.get("limits", None)
        type = self.opts["type"]
        if type == "group":
            return

        if addToList:
            newlimits = copy.copy(limits)
            newlimits[value.getName()] = value
            self.setLimits(newlimits)

        elif limits is not None and not self.invalid:
            if (type == "list" and
                    ((isinstance(limits, dict) and value not in limits.values()) or \
                             (not isinstance(limits, dict) and value not in limits))
                ) or \
                    (type == "bool" and value not in [True, False]) or \
                    ((type == "int" or type == "float") and (value < limits[0] or value > limits[1])) or \
                    (type == "rangegraph" and (value[1] - value[0] != -1) and (
                                    value[0] < limits[0] or value[0] > limits[1] or value[1] < limits[0] or value[1] >
                        limits[1])):
                if isinstance(limits, dict) and value in limits.keys():
                    value = limits[value]
                else:
                    #raise ValueError("Value %s out of limits (%s) in parameter \"%s\"" % (str(value), str(limits), self.getName()))
                    logging.error("Value %s out of limits (%s) in parameter \"%s\"" % (str(value), str(limits), self.getName()))

        try:
            if blockSignal is not None:
                self.sigValueChanged.disconnect(blockSignal)

            if "value" in self.opts:
                self.opts["value"] = value
            if 'psync' in self.opts and self.opts['psync'] == False:
                self.sigValueChanged.emit(value)
            else:
                self.sigValueChanged.emit(value, blockSignal=self.setValue)
        finally:
            if blockSignal is not None:
                self.sigValueChanged.connect(blockSignal)

        if self.previousValue is not None and self.previousValue() is not None:
            self.previousValue().getParams().hide()

        if isinstance(value, Parameterized):
            value.getParams().show()
            self.previousValue = weakref.ref(value)

        if not init:
            if not blockAction:
                self.callAction()
            if isinstance(limits, dict):
                for k, v in limits.iteritems():
                    if v == value:
                        value = k

            if echo and not self.opts.get("echooff", False) and not self.readonly():
                path = self.getPath()
                if path is not None:
                    print >> Parameter.scriptingOutput, str(path + [value]) + ","

    def callLinked(self):
        for name in self.opts.get("linked", []):
            linked = self.parent.getChild(name)
            if linked is not None:
                linked.sigValueChanged.emit(linked.getValue(), blockSignal=None)

    def callAction(self):
        act = self.opts.get("action", None)
        if act is not None:
            act(self)
            path = self.getPath()
            if path is not None:
                print >> Parameter.scriptingOutput, (str(path + [None]) + ",")
        self.callLinked()

    def setDefault(self, default):
        """Set the default value"""
        self.opts['default'] = default
        self.sigOptionsChanged.emit(default=default)

    def setLimits(self, limits):
        """Change the limits. Invalid interval limits are hidden."""
        self.opts['limits'] = limits
        type = self.opts["type"]
        if (type == "int" or type == "float" or type == "rangegraph" or type == "range") and limits[0] > limits[1]:
            self.invalid = True
            self.sigOptionsChanged.emit(visible=False)
        else:
            self.invalid = False
            self.sigOptionsChanged.emit(visible=self.isVisible())
            self.sigLimitsChanged.emit(limits.keys() if isinstance(limits, dict) else limits)

    def readonly(self):
        return self.opts.get('readonly', False)

    def setReadonly(self, readonly=True):
        self.opts["readonly"] = readonly
        self.sigOptionsChanged.emit(readonly=readonly)

    def hide(self):
        self.show(False)

    def show(self, show=True):
        self.opts['visible'] = show
        if not self.invalid:
            self.sigOptionsChanged.emit(visible=show)
        self.sigParametersChanged.emit()

    def isVisible(self):
        return self.opts['visible']

    def remove(self):
        """Detaches the parameter from its parent"""
        if self.parent is None:
            return
        self.parent.removeChild(self)

    def delete(self):
        """Deletes itself (makes the GC job easier removing cicles). WARNING: Can't be called again!!!"""
        self.remove()
        for c in self.childs:
            c.delete()
        self.sigValueChanged.disconnectAll()
        self.sigLimitsChanged.disconnectAll()
        self.sigOptionsChanged.disconnectAll()
        self.sigChildAdded.disconnectAll()
        self.sigChildRemoved.disconnectAll()
        if Parameter.usePyQtGraph:
            # if hasattr(self._PyQtGraphParameter, "sigActivated"):
            #     self._PyQtGraphParameter.sigActivated.disconnect()
            # self._PyQtGraphParameter.sigValueChanged.disconnect()
            self._PyQtGraphParameter = None
        self.deregister()
        self.previousValue = None
        self.opts.clear()

    def clearChildren(self):
        """Remove all children."""
        for ch in self.childs:
            self.removeChild(ch)

    def removeChild(self, child):
        """Remove the specified child."""
        if child is None:
            return

        self.keys.pop(child.getOpts()["name"])
        try:
            self.keys.pop(child.getOpts()["key"])
        except KeyError:
            pass

        if child.opts["type"] != "menu":
            self.sigChildRemoved.emit(child)
        self.childs.remove(child)
        child.parent = None
        self.sigParametersChanged.emit()

    def getChild(self, nameOrPath):
        """Return the paramenter child with the given name/path."""
        if isinstance(nameOrPath, list) or isinstance(nameOrPath, tuple):
            item = self.keys.get(nameOrPath[0], None)

            if len(nameOrPath) == 1 or item is None:
                return item
            else:
                return item.getChild(nameOrPath[1:])
        else:
            try:
                return self.keys[nameOrPath]
            except:
                raise KeyError("Could not find parameter with key %s. Options are: %s" % (nameOrPath, str(self.keys)))

    def getPyQtGraphParameter(self):
        """Return the PyQtGraph Parameter if it exists."""
        if hasattr(self, "_PyQtGraphParameter"):
            return self._PyQtGraphParameter
        return None

    def setupPyQtGraphParameter(self):
        """Creates a PyQtGraph Parameter and keeps it synchronized"""
        opts = {}
        opts.update(self.getOpts())
        if "default" in self.opts:
            opts["value"] = opts["default"]
        if "limits" in self.opts and isinstance(self.opts["limits"], dict):
            opts['limits'] = opts['limits'].keys()
            opts["value"] = self.getKeyFromValue(opts["default"])
            del opts['values']
        self._PyQtGraphParameter = pyqtgraphParameter.create(**opts)
        if hasattr(self._PyQtGraphParameter, "sigActivated"):
            self._PyQtGraphParameter.sigActivated.connect(self.callAction)
        self.sigChildRemoved.connect(lambda c: self._PyQtGraphParameter.removeChild(c.getPyQtGraphParameter()))
        self.sigChildAdded.connect(lambda c: self._PyQtGraphParameter.addChild(c.getPyQtGraphParameter()))
        sigValueUpdatedAdapter = lambda _, v: self.setValue(v, sigSetValueAdapter)
        self._PyQtGraphParameter.sigValueChanged.connect(sigValueUpdatedAdapter)
        sigSetValueAdapter = lambda v, blockSignal=None: self._PyQtGraphParameter.setValue(self.getKeyFromValue(v),
                                                                                      sigValueUpdatedAdapter)
        self.sigValueChanged.connect(sigSetValueAdapter)
        self.sigLimitsChanged.connect(self._PyQtGraphParameter.setLimits)
        self.sigOptionsChanged.connect(self._PyQtGraphParameter.setOpts)

    def setParent(self, parent):
        self.parent = parent

    def getRoot(self):
        """Return the root Parameter."""
        return self if self.parent is None else self.parent.getRoot()

    def getPath(self):
        """Return the path to the root."""
        if self in Parameter.registeredParameters.values():
            path = []
        elif self.parent is None:
            return None
        else:
            path = self.parent.getPath()
        if path is not None:
            path.append(self.opts["name"])
        return path

    def stealDynamicParameters(self, parent):
        """In list type parameters, append each of the Parameterized objects to the parent argument."""
        if self.opts.get("type", None) == "list" and isinstance(self.opts["values"], dict):
            for value in self.opts["values"].itervalues():
                if isinstance(value, Parameterized):
                    parent.append(value.getParams())
                    value.getParams().show(self.getValue() == value)

    def refreshAllParameters(self):
        """
        Refreshes (calls set(get()) ) all the parameters in the hierarchy.
        In list type parameters, append each of the Parameterized objects to the parent, root or child hierarchy.
        """
        if self.opts.get("type", None) == "list" and isinstance(self.opts["values"], dict):
            for value in self.opts["values"].itervalues():
                if isinstance(value, Parameterized):
                    mode = self.opts.get("childmode", None)
                    if mode == "parent":
                        self.parent.append(value.getParams())
                        value.getParams().hide()
                    elif mode == "root":  # Warning, this path is relative... can't call init more than once and sub-modules can't repeat name
                        self.getRoot().append(value.getParams())
                        value.getParams().hide()
                    elif mode == "child":
                        self.append(value.getParams())

        self.setValue(self.getValue(), init=True)
        for child in self.childs:
            child.refreshAllParameters()

    def init(self):
        self.refreshAllParameters()

    def _getAllParameters(self, type=None):
        ret = []
        if self.isVisible():
            if self.opts.get("type", None) == type or type == None:
                ret.append(self)
            for child in self.childs:
                ret.extend(child._getAllParameters(type))
        return ret

    @classmethod
    def getAllParameters(cls, type=None):
        """Return a list with all parameters with a given type in the hierarchy."""
        ret = []
        for p in cls.registeredParameters.itervalues():
            parameters = p._getAllParameters(type)
            [ret.append(param) for param in parameters if param not in ret]
        return ret

    def register(self):
        """Makes it accessible from the root when calling setParameter()"""
        Parameter.registeredParameters[self.getName()] = self
        return self

    def deregister(self):
        """Deregister a registered parameter. Ignores if it is already deregistered."""
        Parameter.registeredParameters.pop(self.getName(), None)

    def toString(self, level, onlyVisibles=False):
        lastLevel = level
        ret = ""
        if self.getType() != "group" and self.getType() != "action" and self.getType() != "menu" and self.getType() != "label":
            if (onlyVisibles and self.isVisible()) or not (onlyVisibles or self.readonly()):
                ret += self.getName() + " = " + str(self.getValueKey()) + "\n"

        if not onlyVisibles or self.isVisible():
            for child in self.childs:
                if lastLevel != level+1:
                    ret += ("[" * (level + 1)) + self.getName() + ("]" * (level + 1)) + "\n"
                    lastLevel = level+1
                try:
                    txt, lastLevel = child.toString(lastLevel, onlyVisibles)
                    ret += txt
                except:
                    logging.info('Info: Could not read parameter %s. Ignoring it...' % str(child.getPath()))
                    pass
        return ret, lastLevel

    def __str__(self):
        txt, _ = self.toString(0)
        return txt

    def save(self, fname, onlyVisibles=False):
        f = open(fname, 'w')
        txt, _ = self.toString(0, onlyVisibles=False)
        f.write(txt)

    @classmethod
    def saveRegistered(cls, fname, onlyVisibles=False):
        f = open(fname, 'w')
        for p in cls.registeredParameters.itervalues():
            if 'addLoadSave' in p.opts and p.opts['addLoadSave'] is not False:
                txt, _ = p.toString(0, onlyVisibles)
                f.write(txt)

    def load(self, fname):
        f = open(fname, 'r')
        path = []
        foundCorrectSection = False
        for lineNum, line in enumerate(f):
            level = 0
            for p in range(0, len(line)):
                if line[p] == "[":
                    level += 1
                else:
                    break
            if level != 0:
                path = path[0:level - 1]
                if level >= len(path):
                    if level > len(path) + 1:
                        raise Warning(
                            'Error reading file %s, line %d. Group hierarchy missing.' % (fname, lineNum))
                    path.append(line[level:-(level + 1)])
            else:
                if path[0] != self.getName():
                    continue
                foundCorrectSection = True
                separator = line.find("=")
                value = line[separator + 1:-1].strip()
                param = path[1:] + [line[0:separator].strip()]
                child = self.getChild(param)
                if child is None:
                    logging.warning('Parameter "%s" in line %d not found. Ignoring it.' % (str(param), lineNum))
                    continue

                if child.getType() == "int":
                    value = int(value)
                elif child.getType() == "float":
                    value = float(value)
                elif child.getType() == "menu" or child.getType() == "label" or child.getType() == "color":
                    continue
                elif child.getType() == "str" or child.getType() == "text" or child.getType() == "action" or \
                                child.getType() == "file" or child.getType() == "filelist":
                    pass
                elif child.getType() == "list" and isinstance(child.opts["limits"], dict):
                    value = child.opts["limits"][value]
                elif value != "":
                    try:
                        value = eval(value)
                    except:
                        pass
                if not child.readonly():
                    child.setValue(value)
                else:
                    if str(child.getValue()) != str(value):
                        logging.info('Parameter %s in line %d is readonly and value being set is different than the current one.' % (child.getName(), lineNum))
        if not foundCorrectSection:
            raise Warning('Could not found section "%s" in file: %s' % (self.getName(), fname))

    @classmethod
    def findParameter(cls, path):
        """
        Find a registered parameter based on a list (used for scripting).
        """
        child = cls.registeredParameters.get(path[0], None)
        if child is None:
            raise KeyError("Parameter not found: %s" % str(path))
        if len(path) > 1:
            return child.getChild(path[1:])
        else:
            return child

    @classmethod
    def getParameter(cls, path):
        """Return the value of a registered parameter"""
        return cls.findParameter(path).getValueKey()

    @classmethod
    def setParameter(cls, parameter, echo=False, blockSignal=False):
        """
        Sets a parameter based on a list (used for scripting).
        The first elements are the path and the last is the value.
        """
        path = parameter[:-1]
        value = parameter[-1]

        child = cls.findParameter(path)
        if isinstance(child.getOpts().get("values", None), dict):
            try:
                value = child.getOpts()["values"][value]
            except KeyError:
                raise ValueError("Invalid value '%s' for parameter '%s'.\nValid values: %s" %
                                 (value, str(parameter), child.getOpts()["values"].keys()))
        child.setValue(value, echo=echo)


def setupSetParam(parameter):
    """
    Decorator that should be used in the set methods specified in the parameter declaration.
    It synchronizes the parameter value when the set method is called directly.
    The blockSignal argument can be used to avoid this behavior when, for instance, you can't do that because the
    parameter wasn't created yet
    """

    def func_decorator(func):
        @functools.wraps(func)
        def func_wrapper(*args, **kargs):
            blockSignal = kargs.get("blockSignal", None)
            if "blockSignal" in kargs:
                del kargs["blockSignal"]
            if blockSignal is None:
                if parameter != "":
                    tmp = args[0].findParam(parameter)
                    tmp.setValue(args[1], blockSignal=tmp.opts["set"], **kargs)
            # todo - use inspect to remove things from kargs that are handled by setvalue above
            if "addToList" in kargs:
                del kargs["addToList"]
            func(*args, **kargs)

        return func_wrapper
        func_wrapper.__wrapped__ = func

    return func_decorator


if __name__ == '__main__':
    from pyqtgraph.Qt import QtGui
    from pyqtgraph.parametertree import ParameterTree

    app = QtGui.QApplication([])


    class submodule(Parameterized):
        def __init__(self, d):
            super(submodule, self).__init__()
            moreparams = [
                {'name': 'Value', 'type': 'list', 'values': [2, 3, 4], 'value': 3, 'action': self.action,
                 'linked': ['Linked Value (1/Value)']},
                {'name': 'Linked Value (1/Value)', 'type': 'float', 'default': 1 / 3, 'get': self.getLinked}
            ]
            self.params = Parameter(name='Sub-module %d' % d, type='group', children=moreparams)

        def action(self, e):
            print "action %s" % e.getValue()

        def getLinked(self):
            return 1.0 / float(self.params.getChild('Value').getValue())


    class module(Parameterized):
        def __init__(self, d):
            super(module, self).__init__()
            values = {'sm1': submodule(1), 'sm2': submodule(2), 'sm3': submodule(3)}
            self.sm = values['sm2']
            moreparams = [
                {'name': 'SubModule', 'type': 'list',
                 'values': values, 'set': self.setSubmodule, 'get': self.getSubmodule,
                 'childmode': "child",
                 },
            ]
            self.params = Parameter(name='Module %d' % d, type='group', children=moreparams)

        def getSubmodule(self):
            return self.sm

        @setupSetParam("SubModule")
        def setSubmodule(self, sm):
            self.sm = sm


    class maintest(Parameterized):
        def __init__(self):
            super(maintest, self).__init__()
            self.values = {'module 1': module(1), 'module 2': module(2), 'module 3': module(3)}
            self.module = self.values['module 2']
            p = [
                {'name': 'Module', 'type': 'list',
                 'values': self.values,
                 'set': self.setmodule,
                 'get': self.getmodule,
                 'childmode': "root",
                 'help': '%namehdr%Boatload of text is possible here. Can use markup too with external help window.'},
                {'name': 'Rocks to Skips', 'type': 'int', 'value': 0, 'help': 'Another help example', 'helpwnd': None,
                 'linked': ['1/Rocks to Skips']},
                {'name': '1/Rocks to Skips', 'type': 'float', 'help': 'Another help example', 'helpwnd': None,
                 'get': self.getLinked}
            ]
            self.params = Parameter(name='Root', type='group', children=p).register()
            self.params.init()

            self.t = ParameterTree()
            self.t.addParameters(self.params.getPyQtGraphParameter())

            self.test = Parameter(name='Test', type='group')
            self.test.addChildren([
                {'name': 'baud', 'type': 'int', 'key': 'baud', 'limits': (500, 2000000), 'value': 38400}
            ])

            self.params.append(self.test)
            self.test.remove()
            self.test.getChild("baud").setValue(500)
            self.params.append(self.test)

            self.t2 = ParameterTree()
            self.params2 = Parameter(name='Root', type='group')
            self.params.getChild("Module").stealDynamicParameters(self.params2)
            self.t2.addParameters(self.params2._PyQtGraphParameter)
            self.params.save("abcde.txt")
            self.params.load("abcde.txt")

        def printhelp(self, msg, obj):
            print msg

        def getmodule(self):
            return self.module

        @setupSetParam("Module")
        def setmodule(self, module):
            print "Changing Module"
            self.module = module

        def getLinked(self):
            try:
                return 1.0 / float(self.params.getChild('Rocks to Skips').getValue())
            except:
                return 0


    m = maintest()

    t = m.t
    t.show()
    t.setWindowTitle('pyqtgraph example: Parameter Tree')
    t.resize(400, 800)
    t2 = m.t2
    t2.show()
    t2.setWindowTitle('pyqtgraph example: Parameter Tree')
    t2.resize(400, 800)
    # Parameter.setParameter(['Root', 'Module', 'module 3'])
    # m.setmodule(m.values['module 1'])

    QtGui.QApplication.instance().exec_()
    # pg.systemInfo()
    # t2 = ParameterTree()
    # t2.show()
    # root = pyqtgraphParameter.create(name='Root', type='group')
    # t2.addParameters(root)
    # par = pyqtgraphParameter.create(name='Par1', type='group')
    # par.addChildren([
    #     {'name':'test', 'type':'int', 'limits':(500, 2000000), 'value':38400}
    # ])
    # root.addChild(par)
    # root.removeChild(par)
    # root.addChild(par)
    #
    # QtGui.QApplication.instance().exec_()

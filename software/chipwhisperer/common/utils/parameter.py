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

import sys
from pyqtgraph.parametertree import Parameter as pyqtgraphParameter
from chipwhisperer.common.utils import util
import chipwhisperer.common.ui.ParameterTypesCustom  # Do not remove!!!


class Parameterized(object):
    _name = "None"
    _description = ""

    def getParams(self):
        assert not hasattr(self, "param") and self.getParams is not None
        if not hasattr(self, "params"):
            self.params = Parameter(name=self.getName(), type='group')
        return self.params

    def findParam(self, name):
        return self.getParams().getChild(name)

    def getName(self):
        return self._name

    @classmethod
    def getClassName(cls):
        return cls._name

    @classmethod
    def getDescription(cls):
        return cls._description


class Parameter(object):
    registeredParameters = {}
    scriptingOutput = sys.stdout
    supportedTypes = ["group", "list", "str", 'text', "bool", "action", "int", "float", "rangegraph", "graphwidget", "file", 'filelist', "range", "color"]
    # attributes = {"name":0, "key":1, "type":2, "values":3, "value":4,
    #               "set":5, "get":6, "limits":7, "step":8, "linked":9, "default":10}

    def __init__(self, parent=None, setupPyQtGraph=True, **opts):
        self.sigValueChanged = util.Signal()
        self.sigLimitsChanged = util.Signal()
        self.sigOptionsChanged = util.Signal()
        self.sigChildAdded = util.Signal()
        self.sigChildRemoved = util.Signal()

        self.previousValue = None
        self.parent = parent
        self.opts = {}
        self.opts.update(opts)

        if 'name' not in self.opts or not isinstance(self.opts['name'], basestring):
            raise Exception("Parameter must have a name.")

        name = self.opts["name"]
        if 'type' not in self.opts or not isinstance(self.opts['type'], basestring) or self.opts['type'] not in Parameter.supportedTypes:
            raise Exception("Parameter \"%s\" must have a valid string type." % name)

        if self.opts['type'] != 'group':
            if (('set' in self.opts) or ('get' in self.opts)) and ('value' in self.opts):
                raise Exception("Use set/get or value, not both simultaneously in parameter \"%s\". If an action is needed, use the action option." % name)

            if not ('set' in self.opts or 'get' in self.opts or 'value' in self.opts or 'action' in self.opts or 'linked' in self.opts):
                raise Exception("Useless parameter \"%s\" because no set/get/value/action/linked option is defined." % name)

            if 'set' in self.opts and (not ('get' in self.opts)) :
                raise Exception("Option set and get should be used together in parameter \"%s\"." % name)

            if 'get' in self.opts and (not 'set' in self.opts) and ('readonly' in self.opts) and self.opts['readonly'] == False :
                raise Exception("Parameters \"%s\" has get and no set. Should be marked as readonly." % name)

            if self.opts.get("type", None) == "list":
                self.opts['limits'] = opts['values']

            if 'set' in self.opts:
                self.sigValueChanged.connect(self.opts['set'])

            if "default" not in self.opts:
                self.opts["default"] = self.getValue()
                self.setValue(self.getValue(), init=True)

        self.children = util.DictType()
        tmp = self.opts.pop("children", [])
        self.setupPyQtGraph = setupPyQtGraph
        if self.setupPyQtGraph:
            self.setupPyQtGraphParameter()
        self.keys = {}
        self.addChildren(tmp)

    def getName(self):
        return self.opts["name"]

    def addChildren(self, children):
        for child in children:
            self.append(Parameter(self, **child))

    def append(self, child):
        if child is None:
            return
        if child.getName() in self.children:
            self.children[child.getName()].remove()

        self.keys[child.getName()] = child
        if 'key' in child.getOpts():
            self.keys[child.getOpts()["key"]] = child
        child.setParent(self)
        self.children[child.getName()] = child
        self.sigChildAdded.emit(child)

    def getOpts(self):
        return self.opts

    def getValue(self, default=None):
        val = self.opts.get("get", None)
        if val is None:
            return self.opts.get("value", default)
        else:
            return val()

    def setValue(self, value,  blockSignal=None, init=False, echo=True):
        if not init and self.readonly():
            raise ValueError("Parameter \"%s\" is currently set to read only." % self.getName())
        limits = self.opts.get("limits", None)
        type = self.opts["type"]
        if type == "group":
            return
        if limits is not None:
            if      (type == "list" and
                        ((isinstance(limits, dict) and value not in limits.values()) or\
                        (not isinstance(limits, dict) and value not in limits))
                    ) or\
                    (type == "bool" and value not in [True, False]) or\
                    ((type == "int" or type =="float") and (value < limits[0] or value > limits[1])) or\
                    (type =="rangegraph" and (value[1] - value[0] != -1) and (value[0] < limits[0] or value[0] > limits[1] or value[1] < limits[0] or value[1] > limits[1])):
                raise ValueError("Value %s out of limits in parameter \"%s\"" % (str(value), self.getName()))

        try:
            if blockSignal is not None:
                self.sigValueChanged.disconnect(blockSignal)

            if "value" in self.opts:
                self.opts["value"] = value
            self.sigValueChanged.emit(value, blockSignal=self.setValue)

        finally:
            if blockSignal is not None:
                self.sigValueChanged.connect(blockSignal)

        if self.previousValue is not None and isinstance(self.previousValue, Parameterized):
            self.previousValue.getParams().hide()

        if isinstance(value, Parameterized):
            value.getParams().show()

        self.previousValue = value

        if not init:
            self.callAction()
            if isinstance(limits, dict):
                for k,v in limits.iteritems():
                    if v == value:
                        value = k

            if echo and not self.opts.get("echooff", False):
                print >> Parameter.scriptingOutput, (str(self.getPath() + [value]))

    def callLinked(self):
        for name in self.opts.get("linked", []):
            linked = self.parent.getChild(name)
            if linked is not None:
                linked.sigValueChanged.emit(linked.getValue(), blockSignal=None)

    def callAction(self):
        act = self.opts.get("action", None)
        if act is not None:
            act(self)
            print >> Parameter.scriptingOutput, (str(self.getPath()))
        self.callLinked()

    def setDefault(self, default):
        self.opts['default'] = default
        self.sigOptionsChanged.emit(default=default)

    def setLimits(self, limits):
        self.opts['limits'] = limits
        if self.opts['type'] =="rangegraph" and limits[0] > limits[1]:
            self.hide()
        else:
            self.show()
            self.sigLimitsChanged.emit(limits)

    def readonly(self):
        return self.opts.get('readonly', False)

    def setReadonly(self, readonly=True):
        self.opts["readonly"] = readonly

    def hide(self):
        self.show(False)

    def show(self, s=True):
        self.opts['visible'] = s
        self.sigOptionsChanged.emit(visible=s)

    def isVisible(self):
        return self.opts['visible']

    def remove(self):
        if self.parent is None:
            return
        self.parent.removeChild(self)

    def clearChildren(self):
        for ch in self.children.itervalues():
            self.removeChild(ch)

    def removeChild(self, child):
        if child is None:
            return

        self.keys.pop(child.getOpts()["name"])
        try:
            self.keys.pop(child.getOpts()["key"])
        except KeyError:
            pass

        self.sigChildRemoved.emit(child)
        del self.children[child.getName()]
        child.parent = None

    def getChild(self, path):
        if isinstance(path, list) or isinstance(path, tuple):
            item = self.keys.get(path[0], None)

            if len(path) == 1 or item is None:
                return item
            else:
                return item.getChild(path[1:])
        else:
            try:
                return self.keys[path]
            except:
                raise KeyError("Could not find parameter with key %s. Options are: %s" % (path, str(self.keys)))

    def getPyQtGraphParameter(self):
        if hasattr(self,"_PyQtGraphParameter"):
            return self._PyQtGraphParameter
        return False

    def setupPyQtGraphParameter(self):
        opts = {}
        opts.update(self.getOpts())
        if "default" in self.opts:
            opts["value"] = opts["default"]
        self._PyQtGraphParameter = pyqtgraphParameter.create(**opts)
        if hasattr(self._PyQtGraphParameter, "sigActivated"):
            self._PyQtGraphParameter.sigActivated.connect(self.callAction)
        self.sigChildRemoved.connect(lambda c: self._PyQtGraphParameter.removeChild(c.getPyQtGraphParameter()))
        self.sigChildAdded.connect(lambda c: self._PyQtGraphParameter.addChild(c.getPyQtGraphParameter()))
        updateParamValue = lambda _, v: self.setValue(v, guiCallback)
        guiCallback = lambda v, blockSignal: self._PyQtGraphParameter.setValue(v, updateParamValue)
        self._PyQtGraphParameter.sigValueChanged.connect(updateParamValue)
        self.sigValueChanged.connect(guiCallback)
        self.sigLimitsChanged.connect(lambda v: self._PyQtGraphParameter.setLimits(v))
        self.sigOptionsChanged.connect(self._PyQtGraphParameter.setOpts)

    def setParent(self, parent):
        self.parent = parent

    def getRoot(self):
        return self if self.parent is None else self.parent.getRoot()

    def getPath(self):
        if self.parent is None:
            path = []
        else:
            path = self.parent.getPath()
        path.append(self.opts["name"])
        return path

    def stealDynamicParameters(self, parent):
         if self.opts.get("type", None) == "list" and isinstance(self.opts["values"], dict):
            for value in self.opts["values"].itervalues():
                if isinstance(value, Parameterized):
                        parent.append(value.getParams())
                        value.getParams().show(self.getValue()==value)


    def refreshAllParameters(self):
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
        for child in self.children.itervalues():
            child.refreshAllParameters()

    def init(self):
        self.refreshAllParameters()

    def register(self):
        self.registeredParameters[self.getName()] = self
        return self

    # def deregister(self):
    #     self.registeredParameters.remove(self)
    #     return self

    @classmethod
    def setParameter(cls, parameter, echo=False, blockSignal=False):
        """Sets a parameter based on a list, used for scripting in combination with showScriptParameter"""
        path = parameter[:-1]
        value = parameter[-1]

        child = None
        p = cls.registeredParameters.get(path[0], None)
        if p is not None:
            child = p.getChild(path[1:])
            if child is not None:

                if isinstance(child.getOpts().get("values", None), dict):
                    value = child.getOpts()["values"][value]
                child.setValue(value, echo=echo)

        if child is None:
            raise KeyError("Parameter not found: %s" % str(parameter))

def setupSetParam(parameter):
    def func_decorator(func):
        def func_wrapper(*args, **kargs):
            blockSignal = kargs.get("blockSignal", None)
            if blockSignal is None:
                if parameter!="":
                    tmp = args[0].findParam(parameter)
                    tmp.setValue(args[1], tmp.opts["set"])
            if "blockSignal" in kargs:
                del kargs["blockSignal"]
            func(*args, **kargs)
        return func_wrapper
    return func_decorator

if __name__ == '__main__':
    from pyqtgraph.Qt import QtGui
    from pyqtgraph.parametertree import ParameterTree

    app = QtGui.QApplication([])

    class submodule(Parameterized):
        def __init__(self, d):
            super(submodule, self).__init__()
            moreparams = [
                {'name': 'Value', 'type': 'list', 'values': [2, 3, 4], 'value':3, 'action': self.action, 'linked':['Linked Value (1/Value)']},
                {'name': 'Linked Value (1/Value)', 'type': 'float', 'default':1/3, 'get': self.getLinked}
            ]
            self.params = Parameter(name='Sub-module %d' % d, type='group', children=moreparams)

        def action(self, e):
            print "action %s" % e.getValue()

        def getLinked(self):
            return 1.0/float(self.params.getChild('Value').getValue())


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
                # {'name':"something", 'type':"placeholder"}
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
                    {'name': 'Rocks to Skips', 'type': 'int', 'value': 0, 'help': 'Another help example', 'helpwnd': None, 'linked':['1/Rocks to Skips']},
                    {'name': '1/Rocks to Skips', 'type': 'float', 'help': 'Another help example', 'helpwnd': None, 'get': self.getLinked}
            ]
            self.params = Parameter(name='Root', type='group', children=p).register()
            self.params.init()

            self.t = ParameterTree()
            self.t.addParameters(self.params.getPyQtGraphParameter())

            self.test = Parameter(name='Test', type='group')
            self.test.addChildren([
                {'name':'baud', 'type':'int', 'key':'baud', 'limits':(500, 2000000), 'value':38400}
            ])

            self.params.append(self.test)
            self.test.remove()
            self.test.getChild("baud").setValue(500)
            self.params.append(self.test)

            self.t2 = ParameterTree()
            self.params2 = Parameter(name='Root', type='group')
            self.params.getChild("Module").stealDynamicParameters(self.params2)
            self.t2.addParameters(self.params2._PyQtGraphParameter)

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
                return 1.0/float(self.params.getChild('Rocks to Skips').getValue())
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

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

from pyqtgraph.parametertree import ParameterTree
from chipwhisperer.common.api.ExtendedParameter import ExtendedParameter, ConfigParameter
import chipwhisperer.common.ui.ParameterTypesCustom  # Do not remove!!
from chipwhisperer.common.utils import util


class CWParameterTree(object):
    _helpWidget = None
    _allParameterTrees = util.DictType()
    sigParamTreeUpdated = util.Signal()

    def __init__(self, groupName="Default", parameterizedObjs=None):
        self.parameterLists = []
        self.extend(parameterizedObjs)
        self._allParameterTrees[groupName] = self

    def getPyQtGraphParamTree(self):
        if not hasattr(self, "parameterTree"):
            self.parameterTree = ParameterTree()
        return self.parameterTree

    def replace(self, parameterLists):
        self.parameterLists = []
        self.extend(parameterLists)
        self.reloadParams()

    def extend(self, parameterLists):
        for parameterList in parameterLists:
            if parameterList:
                self.parameterLists.append(parameterList)
        self.reloadParams()

    def reloadParams(self):
        activeParameters = []
        for parameterList in self.parameterLists:
            if parameterList:
                activeParameters.extend(parameterList.paramList())
        ExtendedParameter.reloadParams(activeParameters, self.getPyQtGraphParamTree(), help_window=self._helpWidget)
        self.sigParamTreeUpdated.emit()

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
    """
    Creates a new parameterTree group and adds its description
    """
    _name = "None"
    _description = ""

    def __init__(self, parentParam=None, name=None):
        if not hasattr(self, "_instanceName") and name:  # These IFs avoids bugs in diamond class hierarchy
            self._instanceName = name
        if not hasattr(self, "__activeParams"):  # These IFs avoids bugs in diamond class hierarchy
            self.__activeParams = []
        if not hasattr(self, "paramListUpdated"):
            self.paramListUpdated = util.Signal()  # Called to refresh the Param List (i.e. new parameters were added)
        if parentParam:
            self.paramListUpdated.connect(parentParam.paramListUpdated.emit)
        if not hasattr(self, "params"):
            self.params = ConfigParameter.create_extended(self, name=self.getName(), type='group', children={}, tip=self.getDescription())
            if self._description:
                self.params.addChildren([{'name':'', 'type':'label', 'value':self.getDescription(), 'readonly':True}])

    def paramList(self):
        # Returns the current active parameters (including the child ones)
        ret = []
        ret.append(self.params)
        for e in self.__activeParams:
            currentParams = e()
            if currentParams:
                ret.extend(currentParams.paramList())
        return ret

    def getName(self):
        # Returns the custom name (the class name can be overriden by the instance name)
        return self.getClassName() if not hasattr(self, "_instanceName") else self._instanceName

    @classmethod
    def getClassName(cls):
        return cls._name

    @classmethod
    def getDescription(cls):
        return cls._description

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
        ret.extend(self.setupGuiActions(mainWindow))
        for e in self.__activeParams:
            currentParams = e()
            if currentParams:
                ret.extend(currentParams.guiActions(mainWindow))
        return ret

    def getAllActiveParameters(self):
        for e in self.__activeParams:
            currentParameters = e()
            if currentParameters:
                currentParameters.params.getAllParameters()

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


v = 0
def actSomething(value):
    print "action called with value = " + str(value)
    global act
    act = value

def setSomething(value):
    global v
    v = value
    print "set to " + str(v)

def getSomething():
    global v
    print "get " + str(v)
    return v


class ParameterItem(object):

    def __init__(self, opts):
        self.sigValueChanged = util.Signal()
        self.opts = opts

    def getValue(self, default=None):
        val = self.opts.get("get", None)
        if val is None:
            return self.opts.get("value", default)
        else:
            return val()

    def setValue(self, value):
        if self.readonly():
            raise ValueError("Parameter is currently set to read only")

        limits = self.opts.get("limits", None)
        if limits is not None:
            type = self.opts.get("type", None)
            if (type == "list" and value not in limits) or\
                    (type == "bool" and value not in [True, False]) or\
                    ((type == "int" or type =="float") and (value < limits[0] or value > limits[1])) or\
                    (type =="rangegraph" and (value[0] < limits[0] or value[0] > limits[1] or value[1] < limits[0] or value[1] > limits[1])):
                raise ValueError("Value out of limits")

        if self.getValue() != value:
            self.sigValueChanged.emit(self, value)

        ref = self.opts.get("set", None)
        if ref is None:
            self.opts["value"] = value
        else:
            ref(value)
        self.callAction()

    def callAction(self):
        act = self.opts.get("action", None)
        if act is not None:
            act(self.opts)

    def setLimits(self, limits):
        self.opts['limits'] = limits
        if type =="rangegraph" and limits[0] > limits[1]:
            self.setReadonly(True)

    def readonly(self):
        return self.opts.get('readonly', False)

    def setReadonly(self, readonly=True):
        self.opts["readonly"] = readonly

    def hide(self):
        self.show(False)

    def show(self, s=True):
        self.opts['visible'] = s
        # self.sigOptionsChanged.emit(self, {'visible': s})


class ParameterGroup(object):
    # attributes = {"name":0, "key":1, "type":2, "values":3, "value":4,
    #               "set":5, "get":6, "limits":7, "step":8, "linked":9, "default":10}

    def __init__(self, opts=None):
        self.opts = opts
        self.parameters = []
        self.lut = {}
        if opts is not None:
            children = opts.get("children", None)
            if children is not None:
                self.addFromDict(children)

    def addFromDict(self, parameters):
        for item in parameters:
            if item["type"] == "group":
                newParameter = ParameterGroup(item)
                item["children"]=newParameter.opts
            else:
                newParameter = ParameterItem(item)
            self.append(newParameter)

    def append(self, parameter):
        if 'key' in parameter.opts:
            self.lut[parameter.opts["key"]] = parameter
        if 'name' in parameter.opts:
            self.lut[parameter.opts["name"]] = parameter
        self.parameters.append(parameter)

    def extend(self, parameters):
        for parameter in parameters:
            self.append(parameter)

    def remove(self, name):
        item = self.lut[name]
        self.parameters.remove(item)
        self.lut.pop(item.opts["name"])
        try:
            self.lut.pop(item.opts["key"])
        except KeyError:
            pass

    def findParam(self, name):
        return self.lut.get(name, None)

    def findParamPath(self, path):
        item = self.lut.get(path[0], None)

        if len(path) == 1:
            return item

        if item is not None:
            if item.opts["type"] == "group":
                return item.findParamPath(path[1:])
            else:
                raise KeyError("Parameter item %s is not a group in path: %s" % (item.opts["name"], path))
        return item

    def getParamTree(self):
        ret = []
        for item in self.parameters:
            ret.append(item.opts)
        return ret

def main():
    p2 = ParameterGroup({'name': 'group2', 'type':'group', 'children':[
            {'name': 'blah', 'type':'int', 'value':5}
         ]})
    p = ParameterGroup()
    p.addFromDict([
        {'name': 'name1', 'key': 'n1', 'type': 'list', 'values': {"a": 1, "b": 2},
         'set': setSomething, 'get': getSomething},
        {'name': 'name2', 'key': 'n2', 'type': 'list', 'values': {"a": 1, "b": 2}, 'value': 1},
        {'name': 'action1', 'key': 'act1', 'type': 'list', 'values': {"a": 1, "b": 2}, 'value': 1, 'action':actSomething},
        {'name': 'action2', 'key': 'act2', 'type': 'list', 'values': {"a": 1, "b": 2}, 'action':actSomething},
        {'name': 'name3', 'key': 'n3', 'type': 'int', 'limits': [10,20],'value': 11},
        {'name': 'name4', 'key': 'n4', 'type': 'bool', 'value': True},
        {'name': 'group1', 'type':'group', 'children':[
            {'name': 'Target IO1', 'key':'gpio1mode', 'type':'list', 'values':{'Serial TXD':0, 'Serial RXD':1, 'USI-Out':2}, 'value':2}
         ]},
        {'name': 'name5', 'type': 'graphwidget', 'limits': [10,20], 'value': [10,20]},
    ])

    assert p.findParamPath(["group1", 'Target IO1']).getValue()==2
    try:
        p.findParamPath(["group1", 'Target IO1', "Target IO1"]).getValue()==2
        assert False
    except KeyError:
        pass

    try:
        p.findParamPath(["group1", 'adsf']).getValue()==2
        assert False
    except:
        pass

    global act
    act = 0

    assert p.findParam("name1").getValue() == 0
    assert p.findParam("n1").getValue() == 0
    assert p.findParam("act2").getValue("blah") == "blah"
    p.findParam("n1").setValue(10)
    assert p.findParam("name1").getValue() == 10
    assert p.findParam("name1").getValue() == p.findParam("n1").getValue()
    p.findParam("name2").setValue(20)
    assert p.findParam("n2").getValue() == 20
    p.findParam("action1").setValue(30)
    assert p.findParam("act1").getValue(None) == 30
    assert p.findParam("act1").getValue(None) == act["value"]
    p.findParam("act2").callAction()
    assert p.findParam("act2").opts["name"] == act["name"]
    try:
        p.findParam("name3").setValue(9)
        assert False
    except:
        pass
    try:
        p.findParam("n3").setValue(21)
        assert False
    except:
        pass

    p.findParam("name3").setValue(10)
    assert p.findParam("n3").getValue() == 10

    try:
        p.findParam("name4").setValue(9)
        assert False
    except:
        pass
    try:
        p.findParam("n4").setValue("a")
        assert False
    except:
        pass

    p.findParam("name4").setValue(False)
    assert p.findParam("n4").getValue() == False

    try:
        p.findParam("name5").setValue([9,11])
        assert False
    except:
        pass
    try:
        p.findParam("name5").setValue([20,21])
        assert False
    except:
        pass

    p.findParam("name5").setValue([10,20])
    p.findParam("name5").setValue([10,20])


    p.remove("name5")
    assert p.findParam("name5") == None
    p.append(p2)
    p.findParamPath(["group2","blah"]).setValue(10)

    list = p.getParamTree()
    print list

if __name__ == '__main__':
    main()
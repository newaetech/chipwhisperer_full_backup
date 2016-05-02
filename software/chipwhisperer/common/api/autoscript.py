#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2014, NewAE Technology Inc
# All rights reserved.
#
# Authors: Colin O'Flynn
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

from chipwhisperer.common.utils import util, timer


class SmartStatements(object):
    """
    SmartStatements is a simple class which holds a list of python commands
    to be run in a list. Adding a new command with addFunctionCall() will
    automatically replace a previous call in-place with the new arguments.
    """

    def __init__(self):
        self._statements = list()
        self._selfreplacement = "self."
        
    def addVariableAssignment(self, varname, values, loc=None):
        statementIndex = -1
        for i, s in enumerate(self._statements):
            if s["objname"] == varname:
                statementIndex = i

        mstr = "%s = %s" % (varname, values)

        if statementIndex == -1:
            d = {"objname":varname,
                 "type":"variable",
                 "values":values,
                 "command":mstr}
            if loc is not None:
                self._statements.insert(loc, d)
            else:
                self._statements.append(d)
        else:
            self._statements[statementIndex]["command"] = mstr
            self._statements[statementIndex]["values"] = values
        
    def addFunctionCall(self, methodname, arguments, varassignment=None, obj="self", loc=None):
        """
        Will add a python statement of the format 'methodname(arguments)'
        to the list of Python commands. If you already have a statement
        calling methodname() this will be replaced in-place.
        """
        statementIndex = -1
        for i,s in enumerate(self._statements):
            if s["objname"] == methodname:
                statementIndex = i

        if len(obj) > 0: obj += "."

        mstr = "%s%s(%s)" % (obj, methodname, arguments)

        if varassignment:
            mstr = varassignment + " = " + mstr

        if statementIndex == -1:
            d = {"objname":methodname,
                 "type":"function",
                 "args":arguments,
                 "varassignment":varassignment,
                 "obj":obj,
                 "command":mstr}
            if loc is not None:
                self._statements.insert(loc, d)
            else:
                self._statements.append(d)
        else:
            self._statements[statementIndex]["command"] = mstr
            self._statements[statementIndex]["args"] = arguments
            self._statements[statementIndex]["varassignment"] = varassignment
            self._statements[statementIndex]["obj"] = obj
            
    def delFunctionCall(self, methodname):
        """Remove a function call or silently fail"""
        statementIndex = -1
        for i, s in enumerate(self._statements):
            if s["objname"] == methodname:
                statementIndex = i

        if statementIndex >= 0:
            del self._statements[statementIndex]

    def statements(self, replaceSelf=True):
        cmdlist = list()
        for s in self._statements:
            if replaceSelf:
                s = s["command"].replace("self.", self._selfreplacement)
                s = s.rstrip('.')
            cmdlist.append(s)
        return cmdlist
    
    def selfReplacement(self):
        return self._selfreplacement

    def addSelfReplacement(self, newstr, force=False):
        if force or newstr not in self._selfreplacement:
            # print newstr not in self._selfreplacement
            # print "%s: %s" % (self._selfreplacement, newstr)
             
            sp = self._selfreplacement.split(".", 1)
            self._selfreplacement = sp[0] + "." + newstr + sp[1]


class AutoScript(object):
    """Base functions for getting/setting stuff to make main script file"""

    def __init__(self):
        self.scriptsUpdated = util.Signal()
        self.runScriptFunction = util.Signal()
        self.autoScriptInit()

    def autoScriptInit(self):
        self.clearStatements()
        self.updateDelayTimer = timer.Timer()
        self.updateDelayTimer.timeout.connect(self.scriptsUpdated.emit)
        self.updateDelayTimer.setSingleShot(True)
        self.updateDelayTimer.setInterval(500)

    def clearStatements(self):
        self.importStatements = []
        self._smartstatements = {"init":SmartStatements(),
                           "go":SmartStatements(),
                           "done":SmartStatements()}

    def importsAppend(self, statement):
        if statement not in self.importStatements:
            self.importStatements.append(statement)
        self.updateDelayTimer.start()

    def getImportStatements(self):
        return self.importStatements

    def addGroup(self, key):
        if key not in self._smartstatements:
            self._smartstatements[key] = SmartStatements()

    def addFunction(self, key, funcstr, argstr, varassignment=None, obj='self', loc=None):
        self._smartstatements[key].addFunctionCall(funcstr, argstr, varassignment=varassignment, obj=obj, loc=loc)
        self.updateDelayTimer.start()

    def mergeGroups(self, key, otherscript, prefix=""):
        for k in otherscript._smartstatements[key]._statements:
            if key not in self._smartstatements:
                self.addGroup(key)
            if k["type"] == "function":
                if len(prefix) > 0 and k["obj"] != "UserScript.":
                    obj = k["obj"] + prefix
                else:
                    obj = k["obj"].rstrip('.')
                # print k["args"]
                self.addFunction(key, k["objname"], k["args"], k["varassignment"], obj)
            elif k["type"] == "variable":
                self.addVariable(key, k["objname"], k["values"])
            else:
                raise ValueError("Invalid type: %s" % k["type"])
        self.updateDelayTimer.start()

    def delFunction(self, key, funcstr):
        self._smartstatements[key].delFunctionCall(funcstr)
        self.updateDelayTimer.start()

    def addVariable(self, key, varname, values, loc=None):
        self._smartstatements[key].addVariableAssignment(varname, values, loc=None)

    def getStatements(self, key):
        return self._smartstatements[key].statements()

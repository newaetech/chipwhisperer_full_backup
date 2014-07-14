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

from PySide.QtCore import *
from PySide.QtGui import *

class SmartStatements(object):
    """
    SmartStatements is a simple class which holds a list of python commands
    to be run in a list. Adding a new command with addFunctionCall() will
    automatically replace a previous call in-place with the new arguments.
    """

    def __init__(self):
        self._statements = list()
        
    def addFunctionCall(self, methodname, arguments):
        """
        Will add a python statement of the format 'methodname(arguments)'
        to the list of Python commands. If you already have a statement
        calling methodname() this will be replaced in-place.
        """
        statementIndex = -1
        for i,s in enumerate(self._statements):
            if s["method"] == methodname:
                statementIndex = i

        mstr = "%s(%s)" % (methodname, arguments)

        if statementIndex == -1:
            self._statements.append({"method":methodname, "command":mstr})
        else:
            self._statements[statementIndex]["command"] = mstr
            
    def statements(self):
        cmdlist = list()
        for s in self._statements:
            cmdlist.append(s["command"])
        return cmdlist

class AutoScript(QObject):
    """Base functions for getting/setting stuff to make main script file"""

    ScriptsUpdated = Signal()

    def __init__(self, parent=None, console=None):
        super(AutoScript, self).__init__(parent)

        self.importStatements = []
        self.initStatements = SmartStatements()
        self.goStatements = SmartStatements()
        self.doneStatements = SmartStatements()
        
    def importsAppend(self, statement):
        if statement not in self.importStatements:
            self.importStatements.append(statement)
        self.ScriptsUpdated.emit()

    def getImportStatements(self):
        return self.importStatements

    def addInitFunction(self, funcstr, argstr):
        self.initStatements.addFunctionCall(funcstr, argstr)
        self.ScriptsUpdated.emit()

    def getInitStatements(self):
        return self.initStatements.statements()

    def addGoFunction(self, funcstr, argstr):
        self.goStatements.addFunctionCall(funcstr, argstr)
        self.ScriptsUpdated.emit()

    def getDoStatements(self):
        return self.goStatements.statements()

    def addDoneFunction(self, funcstr, argstr):
        self.doneStatements.addFunctionCall(funcstr, argstr)
        self.ScriptsUpdated.emit()

    def getDoneStatements(self):
        return self.doneStatements.statements()

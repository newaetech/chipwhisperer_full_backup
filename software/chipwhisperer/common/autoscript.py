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
        
    def addFunctionCall(self, methodname, arguments, loc=None):
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
            d = {"method":methodname, "command":mstr}
            if loc:
                self._statements.insert(loc, d)
            else:
                self._statements.append(d)
        else:
            self._statements[statementIndex]["command"] = mstr
            
    def delFunctionCall(self, methodname):
        """Remove a function call or silently fail"""
        statementIndex = -1
        for i, s in enumerate(self._statements):
            if s["method"] == methodname:
                statementIndex = i

        if statementIndex >= 0:
            del self._statements[statementIndex]


    def statements(self):
        cmdlist = list()
        for s in self._statements:
            cmdlist.append(s["command"])
        return cmdlist

class AutoScript(QObject):
    """Base functions for getting/setting stuff to make main script file"""

    scriptsUpdated = Signal()

    def __init__(self, parent=None, console=None):
        super(AutoScript, self).__init__(parent)
        self.clearStatements()
        
        self.updateDelayTimer = QTimer(self)
        self.updateDelayTimer.timeout.connect(self.scriptsUpdated.emit)
        self.updateDelayTimer.setSingleShot(True)
        self.updateDelayTimer.setInterval(250)

    def clearStatements(self):
        self.importStatements = []
        self.initStatements = SmartStatements()
        self.goStatements = SmartStatements()
        self.doneStatements = SmartStatements()

    def importsAppend(self, statement):
        if statement not in self.importStatements:
            self.importStatements.append(statement)
        self.updateDelayTimer.start()

    def getImportStatements(self):
        return self.importStatements

    def addInitFunction(self, funcstr, argstr, loc=None):
        self.initStatements.addFunctionCall(funcstr, argstr, loc)
        self.updateDelayTimer.start()

    def delInitFunction(self, funcstr):
        self.initStatements.delFunctionCall(funcstr)
        self.updateDelayTimer.start()

    def getInitStatements(self):
        return self.initStatements.statements()

    def addGoFunction(self, funcstr, argstr):
        self.goStatements.addFunctionCall(funcstr, argstr)
        self.updateDelayTimer.start()

    def getDoStatements(self):
        return self.goStatements.statements()

    def addDoneFunction(self, funcstr, argstr):
        self.doneStatements.addFunctionCall(funcstr, argstr)
        self.updateDelayTimer.start()

    def getDoneStatements(self):
        return self.doneStatements.statements()

class AutoScriptBase(object):

    def __init__(self, parent=None, console=None, showScriptParameter=None):
        super(AutoScriptBase, self).__init__()
        self.parent = parent
        self.console = console
        self.showScriptParameter = showScriptParameter
        self._project = None

    def initProject(self):
        pass

    def initPreprocessing(self):
        pass

    def initAnalysis(self):
        pass

    def initReporting(self):
        pass

    def doAnalysis(self):
        pass

    def doneAnalysis(self):
        pass

    def doneReporting(self):
        pass

    def setTraceManager(self, tmanager):
        self._tmanager = tmanager

    def traceManager(self):
        return self._tmanager

    def setProject(self, project):
        self._project = project

    def project(self):
        return self._project

#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2013-2014, NewAE Technology Inc
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

import sys

from chipwhisperer.common.utils import util

def getInstance(*args):
    return ScopeTemplate(*args)

class ScopeTemplate(object):
    def __init__(self):
        self.connectStatus = util.Observable(False)
        self.dataUpdated = util.Signal()
        self.paramListUpdated = util.Signal()

    def dcmTimeout(self):
        pass

    def setAutorefreshDCM(self, enabled):
        pass

    def setCurrentScope(self, scope, update=True):
        pass

    def getStatus(self):
        return self.connectStatus.value()

    def con(self):
        self.connectStatus.setValue(True)
        raise NotImplementedError("Scope \"" + self.getName() + "\" does not implement method " + self.__class__.__name__ + ".con()")

    def dis(self):
        self.connectStatus.setValue(False)

    def doDataUpdated(self,  l, offset=0):
        pass

    def arm(self):
        if self.connectStatus.value() is False:
            raise Exception("Scope \"" + self.getName() + "\" is not connected. Connect it first...")
        raise NotImplementedError("Scope \"" + self.getName() + "\" does not implement method " + self.__class__.__name__ + ".arm()")

    def capture(self, update=True, NumberPoints=None, waitingCallback=None):
        pass

    def paramList(self):
        return []

    def guiActions(self, mainWindow):
        return []

    def validateSettings(self):
        return [("warn", "Scope Module", "You can't use module \"" + self.getName() + "\"", "Specify other module", "57a3924d-3794-4ca6-9693-46a7b5243727")]

    def getName(self):
        return "None"


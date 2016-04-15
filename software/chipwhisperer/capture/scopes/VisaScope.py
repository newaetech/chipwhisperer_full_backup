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

from _base import ScopeTemplate
from chipwhisperer.capture.scopes.visascope_interface.mso54831D import VisaScopeInterface_MSO54831D
from chipwhisperer.common.utils import plugin


class VisaScopeInterface(ScopeTemplate):
    name =  "VISA Scope"

    def __init__(self):
        super(VisaScopeInterface, self).__init__()
        self.scopetype = None
        self.setCurrentScope(self.findParam('type').value(type))

    def setupParameters(self):
        self.setupChildParamsOrder([lambda: self.scopetype])

        scopes = plugin.getPluginsInDictFromPackage("chipwhisperer.capture.scopes.visascope_interface", True)
        self.connectChildParamsSignals(scopes)
        for scope in scopes.itervalues():
            scope.dataUpdated.connect(self.passUpdated)

        return [{'name':'Scope Type', 'key':'type', 'type':'list', 'values':scopes, 'value':scopes[VisaScopeInterface_MSO54831D.name], 'set':self.setCurrentScope},
                       {'name':'Connect String', 'key':'connStr', 'type':'str', 'value':''},
                       {'name':'Example Strings', 'type':'list', 'values':['', 'TCPIP0::192.168.2.100::inst0::INSTR'], 'value':'', 'set':self.exampleString},
                      ]

    def exampleString(self, newstr):
        self.findParam('connStr').setValue(newstr)

    def passUpdated(self, lst, offset):
        self.datapoints = lst
        self.offset = offset
        self.dataUpdated.emit(lst, offset)

    def setCurrentScope(self, scope, update=True):
        self.scopetype = scope
        if update:
            self.paramListUpdated.emit()

    def con(self):
        if self.scopetype is not None:
            self.scopetype.con(self.findParam('connStr').value())
            self.connectStatus.setValue(True)

    def dis(self):
        if self.scopetype is not None:
            self.scopetype.dis()
            self.connectStatus.setValue(False)

    def arm(self):
        try:
            self.scopetype.arm()
        except Exception:
            self.dis()
            raise

    def capture(self, update=True, NumberPoints=None, waitingCallback=None):
        """Raises IOError if unknown failure, returns 'False' if successful, 'True' if timeout"""
        return self.scopetype.capture(update, NumberPoints, waitingCallback)

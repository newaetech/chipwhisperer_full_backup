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

from .base import ScopeTemplate
from chipwhisperer.capture.scopes.visascope_interface.mso54831D import VisaScopeInterface_MSO54831D
from chipwhisperer.common.utils import pluginmanager
from chipwhisperer.common.utils.parameter import setupSetParam
from chipwhisperer.common.utils.pluginmanager import Plugin


class VisaScopeInterface(ScopeTemplate, Plugin):
    _name = "VISA Scope"

    def __init__(self):
        ScopeTemplate.__init__(self)

        scopes = pluginmanager.getPluginsInDictFromPackage("chipwhisperer.capture.scopes.visascope_interface", True, False)

        self.params.addChildren([
            {'name':'Scope Type', 'key':'type', 'type':'list', 'values':scopes, 'value':scopes[VisaScopeInterface_MSO54831D._name], 'set':self.setCurrentScope, 'childmode':'parent'},
            {'name':'Connect String', 'key':'connStr', 'type':'str', 'value':''},
            {'name':'Example Strings', 'type':'list', 'values':['', 'TCPIP0::192.168.2.100::inst0::INSTR'], 'value':'', 'set':self.exampleString},
        ])
        self.params.init()
        self.scopetype = None
        self.setCurrentScope(self.findParam('type').getValue(type))

    @setupSetParam("Example Strings")
    def exampleString(self, newstr):
        self.findParam('connStr').setValue(newstr)

    @setupSetParam("Scope Type")
    def setCurrentScope(self, scope, update=True):
        self.scopetype = scope
        if scope is not None:
            self.scopetype.dataUpdated.connect(self.newDataReceived)

    def _con(self):
        if self.scopetype is not None:
            self.scopetype.con(self.findParam('connStr').getValue())
            return True
        return False

    def _dis(self):
        if self.scopetype is not None:
            self.scopetype.dis()

    def arm(self):
        try:
            self.scopetype.arm()
        except Exception:
            self.dis()
            raise

    def capture(self):
        """Raises IOError if unknown failure, returns 'False' if successful, 'True' if timeout"""
        return self.scopetype.capture()

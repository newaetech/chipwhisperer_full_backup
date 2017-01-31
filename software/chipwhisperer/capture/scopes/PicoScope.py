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

from chipwhisperer.common.utils import pluginmanager
from .base import ScopeTemplate
from chipwhisperer.common.utils.pluginmanager import Plugin


class PicoScopeInterface(ScopeTemplate, Plugin):
    _name = "PicoScope"

    def __init__(self):
        super(PicoScopeInterface, self).__init__()

        scopes = pluginmanager.getPluginsInDictFromPackage("chipwhisperer.capture.scopes.picoscope_interface", True, False)

        self.getParams().addChildren([
            {'name':'Scope Type', 'key':'type', 'type':'list', 'values':scopes, 'value':scopes["PS5000a"], 'action':self.setCurrentScope_act}
        ])
        self.scopetype = None
        self.advancedSettings = None
        self.setCurrentScope(self.findParam('type').getValue())

    def setCurrentScope_act(self, param):
        self.setCurrentScope(param.getValue())

    def setCurrentScope(self, scope):
        self.scopetype = scope
        if scope is not None:
            self.scopetype.dataUpdated.connect(self.newDataReceived)
            self.params.append(self.scopetype.getParams())

    def _con(self):
        if self.scopetype is not None:
            self.scopetype.con()
            return True
        return False

    def _dis(self):
        if self.scopetype is not None:
            self.scopetype.dis()  
        return True

    def arm(self):
        try:
            self.scopetype.arm()
        except:
            self.dis()
            raise

    def capture(self):
        """Raises IOError if unknown failure, returns 'False' if successful, 'True' if timeout"""
        return self.scopetype.capture()

#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2013-2014, NewAE Technology Inc
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

from chipwhisperer.common.utils import Util
from chipwhisperer.common.api.config_parameter import ConfigParameter


# ALWAYS USE THE getClass() METHOD IN ORDER TO INDICATE THE MAIN CLASS WITHIN THE MODULE
def getClass():
    """"Returns the Main Class in this Module"""
    return PluginTemplate


class PluginTemplate(object):
    name = "None"
    description = "Some description"

    def __init__(self):
        self.paramListUpdated = Util.Signal()
        self.updateUI = Util.Signal()
        self.params = ConfigParameter.create_extended(self, name=self.name, type='group', children=self.setupParameters())

    def setupParameters(self):
        """You should overload this. Copy/Paste into your class."""
        ssParams = [{'name':'Example Parameter', 'type':'int', 'value':5}]  # 'set':self.someFunction
        return []

    def paramList(self):
        return [self.params]

    def getName(self):
        return self.name

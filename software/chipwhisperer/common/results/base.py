#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2013-2016, NewAE Technology Inc
# All rights reserved.
#
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

from chipwhisperer.common.utils import util
from chipwhisperer.common.utils.parameter import Parameterized, Parameter


class ResultsBase(Parameterized):
    """
    Base class for the output widgets:
    To be uses in conjunct with chipwhisperer.common.utils.analysissource/tracesource
    """
    registeredObjects = util.DictType()
    sigRegisteredObjectsChanged = util.Signal()
    __classParameter = None
    __classes = None

    def getWidget(self):
        return None

    # @classmethod  #TODO: Finish this method
    # def deregister(self, name):
    #     if self.registeredObjects.pop(name, None):
    #         self.sigRegisteredObjectsChanged.emit(None)

    @classmethod
    def getClasses(cls):
        if not cls.__classes:
            pass
        return cls.__classes

    @classmethod
    def getClassParameter(cls):
        if not cls.__classParameter:
            cls.__classParameter = Parameter(name="Results", type='group').register()
        return cls.__classParameter

    @classmethod
    def createNew(cls, className, instanceName = None):
        # To do it manually using the python console:
        # from chipwhisperer.common.results.base import ResultsBase
        # ResultsBase.createNew("Trace Output Plot", "Channel 2")

        if instanceName in cls.registeredObjects:
            raise Warning("Result widget \"%s\" already exists, choose a different name" % instanceName)

        obj = cls.getClasses()[className](instanceName)
        cls.registeredObjects[obj.getName()] = obj
        cls.sigRegisteredObjectsChanged.emit(obj)
        cls.getClassParameter().append(obj.getParams())
        return obj

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
    connectStatus = util.Signal()
    dataUpdated = util.Signal()
    paramListUpdated = util.Signal()

    def __init__(self, showScriptParameter=None):
        pass

    def dcmTimeout(self):
        pass

    def setAutorefreshDCM(self, enabled):
        pass

    def emitParamListUpdated(self):
        pass

    def setCurrentScope(self, scope, update=True):
        pass

    def con(self):
        raise NotImplementedError("Connect method not implemented")

    def dis(self):
        raise NotImplementedError("Disconnect method not implemented")

    def doDataUpdated(self,  l, offset=0):
        pass

    def arm(self):
        pass

    def capture(self, update=True, NumberPoints=None, waitingCallback=None):
        pass

    def paramList(self):
        return []

    def guiActions(self):
        return []

    def getName(self):
        return "None"


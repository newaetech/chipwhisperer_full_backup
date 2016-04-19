#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2013-2016, NewAE Technology Inc
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
#=================================================

from chipwhisperer.common.utils import pluginmanager


class ProtocolTemplate(pluginmanager.Parameterized):
    name='Smartcard Protocol'

    def __init__(self):
        self.hw = None

    # def setupParameters(self):
    #     """You should overload this. Copy/Paste into your class."""
    #     return [{'name':'Example Parameter', 'type':'int', 'value':5, 'set':self.setSomething}]

    def setSomething(self):
        pass

    def setReaderHardware(self, hw):
        self.hw = hw

    def init(self):
        pass

    def reinit(self):
        pass

    def loadEncryptionKey(self, key):
        pass

    def loadInput(self, inputtext):
        self.input = inputtext

    def isDone(self):
        return True

    def readOutput(self):
        return ""

    def go(self):
        pass

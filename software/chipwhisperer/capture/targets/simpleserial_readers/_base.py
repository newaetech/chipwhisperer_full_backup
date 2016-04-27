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

from chipwhisperer.common.utils import pluginmanager, util


class SimpleSerialTemplate(pluginmanager.Plugin):
    name='Simple Serial Reader'

    def __init__(self, parentParam):
        super(SimpleSerialTemplate, self).__init__(parentParam)
        self.connectStatus = util.Observable(False)

    def selectionChanged(self):
        pass

    def write(self, string):
        pass

    def flush(self):
        pass

    def close(self):
        pass

    def con(self, scope = None):
        """Connect to target"""
        self.connectStatus.setValue(True)

    def dis(self):
        """Disconnect from target"""
        self.close()
        self.connectStatus.setValue(False)

    def flushInput(self):
        pass

    def read(self, num=0, timeout=250):
        return None
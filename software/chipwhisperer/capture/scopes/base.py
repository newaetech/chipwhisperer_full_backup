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
import logging

from chipwhisperer.common.utils import util
from chipwhisperer.common.utils.tracesource import TraceSource


class ScopeTemplate(object):
    _name = "None"

    def __init__(self):
        self.connectStatus = False

    def _getNAEUSB(self):
        raise Warning("Can't find low level USB interface for scope " + self.getName())

    def dcmTimeout(self):
        pass

    def setAutorefreshDCM(self, enabled):
        pass

    def setCurrentScope(self, scope):
        pass

    def newDataReceived(self, channelNum, data=None, offset=0, sampleRate=0):
        pass

    def getStatus(self):
        return self.connectStatus

    def con(self, sn=None, legacy=False):
        if self._con(sn, legacy):
            self.connectStatus = True

    def _con(self, sn=None, legacy=False):
        raise NotImplementedError("Scope \"" + self.getName() + "\" does not implement method " + self.__class__.__name__ + ".con()")

    def dis(self):
        if self._dis():
            pass
        self.connectStatus = False

    def _dis(self):
        raise NotImplementedError("Scope \"" + self.getName() + "\" does not implement method " + self.__class__.__name__ + ".dis()")

    def arm(self):
        """Prepare the scope for capturing"""
        # NOTE - if reimplementing this, should always check for connection first
        # if self.connectStatus.value() is False:
        #     raise Exception("Scope \"" + self.getName() + "\" is not connected. Connect it first...")
        # raise NotImplementedError("Scope \"" + self.getName() + "\" does not implement method " + self.__class__.__name__ + ".arm()")
        pass

    def capture(self):
        """Capture one trace and returns True if timeout has happened."""

        # NOTE: If you have a waiting loop (waiting for arm), call the function util.updateUI() inside that loop to keep
        #       the UI responsive:
        #
        # while self.done() == False:
        #     time.sleep(0.05)
        pass

    def get_name(self):
        return self._name

    getName = util.camel_case_deprecated(get_name)

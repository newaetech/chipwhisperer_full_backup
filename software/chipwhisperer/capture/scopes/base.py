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
from chipwhisperer.common.utils.parameter import Parameterized


class ScopeTemplate(Parameterized):
    _name = "None"

    def __init__(self):
        self.connectStatus = util.Observable(False)
        self.getParams().register()
        self.channels = [Channel(self.getName() + " - Channel " + str(n)) for n in range(1,2)]

    def dcmTimeout(self):
        pass

    def setAutorefreshDCM(self, enabled):
        pass

    def setCurrentScope(self, scope):
        pass

    def newDataReceived(self, channelNum, data=None, offset=0, sampleRate=0):
        self.channels[channelNum].newScopeData(data, offset, sampleRate)

    def getStatus(self):
        return self.connectStatus.value()

    def con(self):
        for channel in self.channels:
            channel.register()
        if self._con():
            self.connectStatus.setValue(True)

    def _con(self):
        raise Warning("Scope \"" + self.getName() + "\" does not implement method " + self.__class__.__name__ + ".con()")

    def dis(self):
        if self._dis():
            for channel in self.channels:
                channel.deregister()
        self.connectStatus.setValue(False)

    def _dis(self):
        raise Warning("Scope \"" + self.getName() + "\" does not implement method " + self.__class__.__name__ + ".dis()")

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
        #     util.updateUI()
        pass


class Channel(TraceSource):
    """Save the traces emited by the scopes and notify the TraceSourceObservers."""

    def __init__(self, name="Unknown Channel"):
        TraceSource.__init__(self, name)
        self._lastData = []
        self._lastOffset = 0
        self._sampleRate = 0

    def newScopeData(self, data=None, offset=0, sampleRate=0):
        """Capture the received trace and emit a signal to inform the observers"""
        self._lastData = data
        self._lastOffset = offset
        self._sampleRate = sampleRate
        if len(data) > 0:
            self.sigTracesChanged.emit()
        else:
            logging.warning('Captured trace in "%s" has len=0' % self.name)

    def getTrace(self, n=0):
        if n != 0:
            raise ValueError("Live trace source has no buffer, so it only supports trace 0.")
        return self._lastData

    def numPoints(self):
        return len(self._lastData)

    def numTraces(self):
        return 1

    def offset(self):
        return self._lastOffset

    def getSampleRate(self):
        return self._sampleRate
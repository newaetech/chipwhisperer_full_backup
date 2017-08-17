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
import logging
import time
from _base import AuxiliaryTemplate
from chipwhisperer.common.utils.timer import nonBlockingSleep
from chipwhisperer.common.api.CWCoreAPI import CWCoreAPI


class GPIOToggle(object):
    """The GPIOToggle class switches one GPIO pin's state from low to high (or
    vice versa) for a short amount of time.
    
    Arguments to __init__():
    - pin: The pin to toggle. Can be one of the strings "tio1"-"tio4", "nrst",
        "pdid", "pdic"
    - default_state: The standby state of the pin. Can be True (high, toggle
        low) or False (low, toggle high)
    - active_ms: Length of toggle in ms
    - delay_ms: Length of delay after returning to default state in ms
    """
    def __init__(self, pin, default_state, active_ms, delay_ms):
        self._pin = pin
        self._default_state = default_state
        self._active_ms = active_ms
        self._delay_ms = delay_ms

    def setPin(self, scope, pin, state):
        """Call like self.setPin(scope, "tio1", True)"""
        setattr(scope.io, pin, state)
        
    def togglePin(self, scope):
        self.setPin(scope, self._pin, not self._default_state)
        nonBlockingSleep(self._active_ms)
        self.setPin(scope, self._pin, self._default_state)
        nonBlockingSleep(self._delay_ms)

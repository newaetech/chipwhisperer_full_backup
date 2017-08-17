#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2015, NewAE Technology Inc.
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

from chipwhisperer.capture.auxiliary._base import AuxiliaryTemplate
from chipwhisperer.common.api.CWCoreAPI import CWCoreAPI
from chipwhisperer.common.utils.timer import nonBlockingDelay

class ResetCW1173(object):
    """Reset an AVR/XMEGA via the PDI/ISP interface.
    
    This class must be initialized with 2 values:
    - XMEGA/AVR: True if reseting XMEGA; False for AVR
    - Delay: How long to wait between device reset and scope arm
    
    Then, there are three self-explanatory functions that can be registered:
    - reset(): no delay. Useful for testing
    - resetThenDelay(): Useful for before_arm
    - delayThenReset(): Useful for after_arm
    
    Note that resetThenDelay() before arming is more consistent (resetting the 
    device can accidentally cause a trigger event) but delayThenReset() after 
    arming allows for a shorter delay between the reset and the scope
    measurement. 
    """
    
    def __init__(self, xmega, delay_ms):
        self._xmega = xmega
        self._delay_ms = delay_ms
        
    def reset(self, scope):
        if self._xmega:
            scope.scopetype.dev.getCwliteXMEGA().readSignature()
        else:
            scope.scopetype.dev.getCwliteAVR().readSignature()
        
    def resetThenDelay(scope, target, project):
        self.reset(scope)
        nonBlockingDelay(self._delay_ms)
        
    def delayThenReset(scope, target, project):
        nonBlockingDelay(self._delay_ms)
        self.reset(scope)

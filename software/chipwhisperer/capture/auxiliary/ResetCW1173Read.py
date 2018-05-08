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

from chipwhisperer.common.utils.timer import nonBlockingDelay
import time
import warnings
import logging

class ResetCW1173(object):
    """Reset an AVR/XMEGA via the PDI/ISP interface.
    
    This class must be initialized with 2 values:
    - Delay: How long to wait between device reset and scope arm
    - pin: (string) to identify pin to toggle
        XMEGA: use 'pdic', AVR and STM32Fx use 'nrst'
    - xmega: !DEPRECATED, use pin instead!(boolean)  true for xmega false for everything else
    
    Then, there are three self-explanatory functions that can be registered:
    - reset(): no delay. Useful for testing
    - resetThenDelay(): Useful for before_arm
    - delayThenReset(): Useful for after_arm
    
    Note that resetThenDelay() before arming is more consistent (resetting the 
    device can accidentally cause a trigger event) but delayThenReset() after 
    arming allows for a shorter delay between the reset and the scope
    measurement. 
    """
    
    def __init__(self, delay_ms, pin=None, xmega=None):
        self._delay_ms = delay_ms
        self._pin = pin
        self._xmega = xmega
        
    def reset(self, scope):

        # To keep backwards compatability
        if self._xmega is not None and self._pin is None:
            warnings.warn('xmega argument has been deprecated, use pin instead')
            logging.warning('xmega argument has been deprecated, use pin instead')
            if self._xmega:
                self._pin = 'pdic'
            else:
                self._pin = 'nrst'
        elif self._xmega is not None and self._pin is not None:
            warnings.warn('xmega argument is deprecated, do not use it and the new pin argument')
            logging.warning('xmega argument is deprecated, do not use it and the new pin argument')

        if self._pin == 'nrst':
            scope.io.nrst = 'low'
            time.sleep(0.01)
            scope.io.nrst = 'high'
        elif self._pin == 'pdic':
            scope.io.pdic = 'low'
            time.sleep(0.01)
            scope.io.pdic = 'high'
        else:
            ValueError('currently only nrst and pdic are toggleable')

    def resetThenDelay(self, scope, target, project):
        self.reset(scope)
        nonBlockingDelay(self._delay_ms)
        
    def delayThenReset(self, scope, target, project):
        nonBlockingDelay(self._delay_ms)
        self.reset(scope)

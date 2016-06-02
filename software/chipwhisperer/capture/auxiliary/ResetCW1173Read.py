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

import time
from chipwhisperer.capture.auxiliary._base import AuxiliaryTemplate
from chipwhisperer.common.api.CWCoreAPI import CWCoreAPI
from chipwhisperer.common.utils import util, timer


class ResetCW1173Read(AuxiliaryTemplate):
    '''
    This auxiliary module can do two things:
      1. Reset the target before arming the scope:
             ^ reset     ^ arm       ^ trigger
             |<-- d_1 -->|<-- d_2 -->|
             |           |           |
      -----------------------------------------------> (time)
      
      2. Reset the target after arming the scope:
                         ^ arm       ^ reset ^ trigger
              <-- d_1 -->|<-- d_2 -->|       |
                         |           |       |
      -----------------------------------------------> (time)
      
    Method 1 is more consistent (resetting the device can accidentally cause
    a trigger event) but method 2 allows for a shorter delay between the reset
    and the scope measurement. 
    
    In the ChipWhisperer software, method 1 is called "pre-arm reset" and 
    method 2 is called "post-arm reset".
    '''
    _name = "Reset AVR/XMEGA via CW-Lite"

    def __init__(self, parentParam=None):
        AuxiliaryTemplate.__init__(self, parentParam)
        self.params.addChildren([
            {'name':'Interface', 'type':'list', 'key':'target', 'values':['xmega (PDI)', 'avr (ISP)'], 'value':'xmega (PDI)'},
            {'name':'Delay (Pre-Arm)' , 'type':'int',  'key':'predelay',  'limits':(0, 10E3), 'value':0, 'suffix':' ms'},
			{'name':'Delay (Post-Arm)', 'type':'int',  'key':'postdelay', 'limits':(0, 10E3), 'value':0, 'suffix':' ms'},
            {'name':'Reset Timing'  , 'type':'list', 'key':'resettiming', 'values':['Pre-Arm', 'Post-Arm'], 'value':'Pre-Arm'},
            {'name':'Test Reset', 'type':'action', 'action':self.testReset}
        ])

    def captureInit(self):
        pass

    def captureComplete(self):
        pass

    def traceArm(self):
        # Before we arm the scope, possibly reset the device and wait for a bit
        resettiming = self.findParam('resettiming').value()
        if resettiming == 'Pre-Arm':
            self.resetDevice()

        dly = self.findParam('predelay').value()
        if dly > 0:
            self.nonblockingSleep(dly / 1000.0)
			
    def traceArmPost(self):
        # After we arm the scope, wait for a bit, then possibly reset the target            
        dly = self.findParam('postdelay').value()
        if dly > 0:
            self.nonblockingSleep(dly / 1000.0)
            
        resettiming = self.findParam('resettiming').value()
        if resettiming == 'Post-Arm':
            self.resetDevice()
			
    def traceDone(self):
        pass

    def resetDevice(self):
        # Reset the target by reading its signature
        target = self.findParam('target').value()
        if target == 'xmega (PDI)':
            CWCoreAPI.getInstance().getScope().scopetype.cwliteXMEGA.readSignature()
        else:
            CWCoreAPI.getInstance().getScope().scopetype.cwliteAVR.readSignature()
        
    def testReset(self):
        self.resetDevice()

    def nonblockingSleep_done(self):
        self._sleeping = False

    def nonblockingSleep(self, stime):
        """Sleep for given number of seconds (~50mS resolution), but don't block GUI while we do it"""
        timer.Timer.singleShot(stime * 1000, self.nonblockingSleep_done)
        self._sleeping = True
        while(self._sleeping):
            time.sleep(0.01)
            util.updateUI()

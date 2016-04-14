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
from chipwhisperer.capture.auxiliary.template import AuxiliaryTemplate
from chipwhisperer.common.api.config_parameter import ConfigParameter
from chipwhisperer.common.api.CWCoreAPI import CWCoreAPI
from chipwhisperer.common.utils import timer


def getClass():
    return ResetCW1173Read


class ResetCW1173Read(AuxiliaryTemplate):
    name = "Reset AVR/XMEGA via CW-Lite"

    def setupParameters(self):
        ssParams = [{'name':'Interface', 'type':'list', 'key':'target', 'values':['xmega (PDI)', 'avr (ISP)'], 'value':'xmega (PDI)'},
                    {'name':'Post-Reset Delay', 'type':'int', 'key':'toggledelay', 'limits':(0, 10E3), 'value':0, 'suffix':'mS'},
                    {'name':'Test Reset', 'type':'action', 'action':self.testReset}
                    ]
        return ssParams

    def captureInit(self):
        pass

    def captureComplete(self):
        pass

    def traceArmPost(self):
        target = self.findParam('target').value()
        if target == 'xmega (PDI)':
            CWCoreAPI.getInstance().getScope().scopetype.cwliteXMEGA.readSignature()
        else:
            CWCoreAPI.getInstance().getScope().scopetype.cwliteAVR.readSignature()

        dly = self.findParam('toggledelay').value()
        if dly > 0:
            self.nonblockingSleep(dly / 1000.0)

    def traceDone(self):
        pass

    def testReset(self):
        self.traceArmPost()

    def nonblockingSleep_done(self):
        self._sleeping = False

    def nonblockingSleep(self, stime):
        """Sleep for given number of seconds (~50mS resolution), but don't block GUI while we do it"""
        timer.Timer.singleShot(stime * 1000, self.nonblockingSleep_done)
        self._sleeping = True
        while(self._sleeping):
            time.sleep(0.01)
            self.updateUI.emit()

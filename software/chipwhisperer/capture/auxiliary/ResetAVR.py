#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2014, Colin O'Flynn <coflynn@newae.com>
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
from subprocess import call
from _base import AuxiliaryTemplate


class ResetAVR(AuxiliaryTemplate):
    _name = 'Reset AVR via STK500'

    def __init__(self, parentParam=None):
        AuxiliaryTemplate.__init__(self, parentParam)
        self.params.addChildren([
            {'name':'STK500.exe Path', 'type':'str', 'key':'stk500path', 'value':r'C:\Program Files (x86)\Atmel\AVR Tools\STK500\Stk500.exe'},
            {'name':'AVR Part', 'type':'list', 'key':'part', 'values':['atmega328p'], 'value':'atmega328p'},
            {'name':'Test Reset', 'type':'action', 'action':self.testReset}
        ])

    def captureInit(self):
        pass

    def captureComplete(self):
        pass

    def traceArm(self):
        # If using STK500
        stk500 = self.findParam('stk500path').value()
        ret = call([stk500, "-d%s" % self.findParam('part').value(), "-s", "-cUSB"])

        if int(ret) != 0:
            raise IOError("Error Calling Stk500.exe")

        time.sleep(1)

        # If using AVRDude
        # call(["avrdude"])

    def traceDone(self):
        pass

    def testReset(self):
        self.traceArm()

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
#
#
#
# This example captures data using the ChipWhisperer Rev2 api hardware. The target is a SimpleSerial board attached
# to the ChipWhisperer.
#
# Data is saved into both a project file and a MATLAB array
#

import sys
from chipwhisperer.common.api.CWCoreAPI import CWCoreAPI  # Import the ChipWhisperer API
import chipwhisperer.capture.ui.CWCaptureGUI as cwc       # Import the ChipWhispererCapture GUI
from chipwhisperer.common.scripts._base import UserScriptBase


class UserScript(UserScriptBase):
    name = "SASEBO-W: AES-128 SASEBO-W Smart Card OS"
    description = "SASEBO-W Loaded with ChipWhisperer using Provided AVR Smart Card"

    def __init__(self, api):
        super(UserScript, self).__init__(api)

    def run(self):
        #User commands here
        self.api.setParameter(['Generic Settings', 'Scope Module', 'ChipWhisperer/OpenADC'])
        self.api.setParameter(['ChipWhisperer/OpenADC', 'Connection', 'FTDI (SASEBO-W/SAKURA-G)'])
        self.api.setParameter(['FTDI (SASEBO-W/SAKURA-G)', 'Refresh Device List', None])
        self.api.setParameter(['Generic Settings', 'Target Module', 'Smart Card'])
        self.api.setParameter(['Smart Card', 'Reader Hardware', 'ChipWhisperer-SCARD'])
        self.api.setParameter(['Smart Card', 'SmartCard Protocol', 'SASEBO-W SmartCard OS'])
        self.api.setParameter(['Generic Settings', 'Trace Format', 'ChipWhisperer/Native'])

        self.api.connect()

        #Example of using a list to set parameters. Slightly easier to copy/paste in this format
        lstexample = [['OpenADC', 'Clock Setup', 'ADC Clock', 'Source', 'EXTCLK x1 via DCM'],
                      ['OpenADC', 'Trigger Setup', 'Total Samples', 3000],
                      ['OpenADC', 'Trigger Setup', 'Offset', 0],
                      ['OpenADC', 'Gain Setting', 'Setting', 50],
                      ['OpenADC', 'Trigger Setup', 'Mode', 'rising edge'],
                      #Final step: make DCMs relock in case they are lost
                      ['OpenADC', 'Clock Setup', 'ADC Clock', 'Reset ADC DCM', None],
                      ['OpenADC', 'Clock Setup', 'ADC Clock', 'Reset ADC DCM', None],
                      ]
        
        #Download all hardware setup parameters
        for cmd in lstexample: self.api.setParameter(cmd)
        
        #Let's only do a few traces
        self.api.setParameter(['Generic Settings', 'Acquisition Settings', 'Number of Traces', 1000])
                      
        #Throw away first few
        self.api.capture1()
        self.api.capture1()

        #Start api process
        #writer = self.api.captureM()
        #self.api.proj.setFilename("../capturev2/test_live.cwp")
        #self.api.saveProject()


if __name__ == '__main__':
    api = CWCoreAPI()                               # Instantiate the API
    app = cwc.makeApplication()                     # Make the GUI application (optional)
    #app.setApplicationName("Capture V2 Scripted")  # If you DO NOT want to overwrite settings from the GUI
    gui = cwc.CWCaptureGUI(api)                     # Pass the API as parameter to the GUI front-end (optional)
    gui.show()
    usercommands = UserScript(api)                  # Pass the API as parameter to the User Script
    usercommands.run()                              # Run the User Script

    app.exec_()
    sys.exit()

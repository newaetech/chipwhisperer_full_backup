#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2016, NewAE Technology Inc
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


import sys
from chipwhisperer.common.api.CWCoreAPI import CWCoreAPI  # Import the ChipWhisperer API
import chipwhisperer.capture.ui.CWCaptureGUI as cwc       # Import the ChipWhispererCapture GUI
from chipwhisperer.common.scripts.base import UserScriptBase


class UserScript(UserScriptBase):
    _name = "CW305 Artix Target w/ ChipWhisperer-Lite"
    _description = "Artix FPGA Target using CW305 Target Board"

    def __init__(self, api):
        super(UserScript, self).__init__(api)

    def run(self):
        #User commands here
        self.api.setParameter(['Generic Settings', 'Scope Module', 'ChipWhisperer/OpenADC'])
        self.api.setParameter(['Generic Settings', 'Target Module', 'ChipWhisperer CW305 (Artix-7)'])
        self.api.setParameter(['Generic Settings', 'Trace Format', 'ChipWhisperer/Native'])
        self.api.setParameter(['ChipWhisperer/OpenADC', 'Connection', 'ChipWhisperer-Lite'])

        #Load FW (must be configured in GUI first)

        self.api.connect()
        
        #Example of using a list to set parameters. Slightly easier to copy/paste in this format
        lstexample = [['CW Extra Settings', 'Trigger Pins', 'Target IO4 (Trigger Line)', True],
                     # ['OpenADC', 'Clock Setup', 'CLKGEN Settings', 'Desired Frequency', 5000000.0],
                     # ['CW Extra Settings', 'Target HS IO-Out', 'CLKGEN'],
                      ['OpenADC', 'Clock Setup', 'ADC Clock', 'Source', 'EXTCLK x4 via DCM'], #CLKGEN
                      ['OpenADC', 'Trigger Setup', 'Total Samples', 250],
                      ['OpenADC', 'Gain Setting', 'Setting', 30],
                      ['OpenADC', 'Gain Setting', 'Mode', 'high'],
                      ['OpenADC', 'Trigger Setup', 'Mode', 'rising edge'],
                      #Final step: make DCMs relock in case they are lost
                      ['OpenADC', 'Clock Setup', 'ADC Clock', 'Reset ADC DCM', None],
                      ['OpenADC', 'Clock Setup', 'ADC Clock', 'Phase Adjust', 255],
                      ]
        
        #Download all hardware setup parameters
        for cmd in lstexample: self.api.setParameter(cmd)
        
        #Let's only do a few traces
        self.api.setParameter(['Generic Settings', 'Acquisition Settings', 'Number of Traces', 5000])
                      
        #Throw away first few
        self.api.capture1()
        self.api.capture1()


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

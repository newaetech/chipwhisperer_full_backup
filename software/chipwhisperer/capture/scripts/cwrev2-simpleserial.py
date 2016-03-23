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

#Setup path
import sys

#Import the ChipWhispererCapture module
import chipwhisperer.capture.ui.CWCaptureGUI as cwc

#Check for PySide
try:
    from PySide.QtCore import *
    from PySide.QtGui import *
except ImportError:
    print "ERROR: PySide is required for this program"
    sys.exit()

import thread

import scipy.io as sio

exitWhenDone=False

def name():
    return "ChipWhisperer-Rev2: SimpleSerial Target"

def tip():
    return "SimpleSerial with CW Target Board"

def pe():
    QCoreApplication.processEvents()

class userScript(QObject):

    def __init__(self, capture):
        super(userScript, self).__init__()
        self.capture = capture
                

    def run(self):
        cap = self.capture
        
        #User commands here
        print "***** Starting User Script *****"
       
        cap.setParameter(['Generic Settings', 'Scope Module', 'ChipWhisperer/OpenADC'])
        cap.setParameter(['Generic Settings', 'Target Module', 'Simple Serial'])
        cap.setParameter(['Generic Settings', 'Trace Format', 'ChipWhisperer/Native'])
        cap.setParameter(['OpenADC Interface', 'Connection', 'ChipWhisperer Rev2'])
        cap.setParameter(['Target Connection', 'Connection', 'ChipWhisperer'])

        #Load FW (must be configured in GUI first)
        # cap.FWLoaderGo()
                
        #NOTE: You MUST add this call to pe() to process events. This is done automatically
        #for setParameter() calls, but everything else REQUIRES this
        pe()

        cap.doConDis()
        
        pe()
        pe()
        pe()
        pe()
        
        #Example of using a list to set parameters. Slightly easier to copy/paste in this format
        lstexample = [['CW Extra', 'CW Extra Settings', 'Trigger Pins', 'Front Panel A', False],
                      ['CW Extra', 'CW Extra Settings', 'Trigger Pins', 'Target IO4 (Trigger Line)', True],
                      ['CW Extra', 'CW Extra Settings', 'Clock Source', 'Target IO-IN'],
                      ['OpenADC', 'Clock Setup', 'ADC Clock', 'Source', 'EXTCLK x4 via DCM'],
                      ['OpenADC', 'Trigger Setup', 'Total Samples', 3000],
                      ['OpenADC', 'Trigger Setup', 'Offset', 1500],
                      ['OpenADC', 'Gain Setting', 'Setting', 45],
                      ['OpenADC', 'Trigger Setup', 'Mode', 'rising edge'],
                      #Final step: make DCMs relock in case they are lost
                      ['OpenADC', 'Clock Setup', 'ADC Clock', 'Reset ADC DCM', None],
                      ]
        
        #Download all hardware setup parameters
        for cmd in lstexample: cap.setParameter(cmd)
        
        #Let's only do a few traces
        cap.setParameter(['Generic Settings', 'Acquisition Settings', 'Number of Traces', 50])
                      
        #Throw away first few
        cap.capture1()
        pe()
        cap.capture1()
        pe()
        
        #Start capture process
        #writer = cap.captureM()
        #
        #pe()
        #
        #cap.proj.setFilename("../capturev2/test_live.cwp")
        #
        #pe()
        #
        #cap.saveProject()
        
        pe()

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
    
    #NB: Must call processEvents since we aren't using proper event loop
    pe()
    #Call user-specific commands 
    usercommands = userScript(capture)
    
    usercommands.run()
    
    app.exec_()
    
    sys.exit()

    

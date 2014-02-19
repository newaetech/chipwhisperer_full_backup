#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2013, Colin O'Flynn <coflynn@newae.com>
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
import time

#Check for PySide
try:
    from PySide.QtCore import *
    from PySide.QtGui import *
except ImportError:
    print "ERROR: PySide is required for this program"
    sys.exit()

#Import the ChipWhispererCapture module
import chipwhisperer.capture.ChipWhispererCapture as cwc
import chipwhisperer.capture.scopes.ChipWhispererExtra as cwe


exitWhenDone=False

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
        #cap.setParameter(['Generic Settings', 'Target Module', 'SmartCard DPAContestv4'])
        cap.setParameter(['Generic Settings', 'Target Module', 'Smart Card'])
        cap.setParameter(['Target Connection', 'Reader Hardware', 'ChipWhisperer-Connected'])
        #cap.setParameter(['OpenADC Interface', 'connection', 'FTDI (SASEBO-W)'])
        #cap.setParameter(['OpenADC-FTDI', 'Refresh Device List', None])

        #NOTE: You MUST add this call to pe() to process events. This is done automatically
        #for setParameter() calls, but everything else REQUIRES this
        pe()

        cap.doConDis()
        pe()
                      
        print "Loading CW-Extra Module"
        usi = cwe.CWUniversalSerial()
        usi.con(cap.scope.qtadc.sc)
        
        usi.setBaud(9600)
        usi.setParity("even")        
        usi.setStopbits(2)
        usi.write([0x80,0x04, 0x04, 0x00, 0x10, None, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01])
        
        time.sleep(0.05)
        
        usi.read(16, startonly=True)
        usi.write([0x80,0xC0, 0x00, 0x00, 0x10])
        p = bytearray(usi.read(16, waitonly=True))
        for t in p:
            print "%2x "%t,

        
        
        #End commands here
        print "***** Ending User Script *****"
        

#Make the application
app = cwc.makeApplication()

#If you DO NOT want to overwrite/use settings from the GUI version including
#the recent files list, uncomment the following:
#app.setApplicationName("Capture V2 Scripted")

#Get main module
capture = cwc.ChipWhispererCapture()

#Show window - even if not used
capture.show()

#NB: Must call processEvents since we aren't using proper event loop
pe()
#Call user-specific commands 
usercommands = userScript(capture)

usercommands.run()

app.exec_()

sys.exit()

    

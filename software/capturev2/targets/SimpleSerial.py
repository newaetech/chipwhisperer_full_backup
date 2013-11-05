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
import sys
import serial

from PySide.QtCore import *
from PySide.QtGui import *

from TargetTemplate import TargetTemplate

import time

try:
    from pyqtgraph.parametertree import Parameter, ParameterTree, ParameterItem, registerParameterType
except ImportError:
    print "ERROR: PyQtGraph is required for this program"
    sys.exit()
    
sys.path.append('../common')
sys.path.append('../../openadc/controlsw/python/common')
from ExtendedParameter import ExtendedParameter
import scan    

class SimpleSerial_serial(TargetTemplate):
    def setupParameters(self):
        ssParams = [{'name':'Baud', 'key':'baud', 'type':'list', 'values':{'38400':38400}, 'value':38400},
                    {'name':'Port', 'key':'port', 'type':'list', 'values':['Hit Refresh'], 'value':'Hit Refresh'},
                    {'name':'Refresh', 'type':'action', 'action':self.updateSerial}
                    ]      
        self.params = Parameter.create(name='Serial Port Settings', type='group', children=ssParams)
        
        ExtendedParameter.setupExtended(self.params, self)      
        self.ser = None
            
    def paramTreeChanged(self, param, changes):
        if self.showScriptParameter is not None:
            self.showScriptParameter(param, changes, self.params)

    def paramList(self):
        return [self.params]

    def updateSerial(self):
        serialnames = scan.scan()
        self.findParam('port').setLimits(serialnames)
        if len(serialnames) > 0:
            self.findParam('port').setValue(serialnames[0])

    def debugInfo(self, lastTx=None, lastRx=None, logInfo=None):
        pass

    def write(self, string):
        return self.ser.write(string)

    def read(self, num=0, timeout=100):
        return self.ser.read(num)

    def flush(self):
        self.ser.flushInput()
            
    def flushInput(self):
        self.ser.flushInput()
            
    def close(self):
        if self.ser is not None:
            self.ser.close()
            self.ser = None
            
    def con(self):
        if self.ser == None:
            
            # Open serial port if not already
            self.ser = serial.Serial()
            self.ser.port     = self.findParam('port').value()
            self.ser.baudrate = self.findParam('baud').value()
            self.ser.timeout  = 2     # 2 second timeout
            self.ser.open()

class SimpleSerial_ChipWhisperer(TargetTemplate):    
    CODE_READ       = 0x80
    CODE_WRITE      = 0xC0

    ADDR_DATA       = 33
    ADDR_LEN        = 34
    ADDR_BAUD       = 35

    def setupParameters(self):
        ssParams = [{'name':'baud', 'type':'list', 'values':['38400'], 'value':'38400', 'get':self.baud, 'set':self.setBaud}]        
        self.params = Parameter.create(name='Serial Port Settings', type='group', children=ssParams)
        ExtendedParameter.setupExtended(self.params, self)      
            

    def paramTreeChanged(self, param, changes):
        if self.showScriptParameter is not None:
            self.showScriptParameter(param, changes, self.params)

    def setBaud(self, baud):
        return
    
    def baud(self):
        return 38400

    def paramList(self):
        return [self.params]
    
    def setOpenADC(self, oa):
        self.oa = oa

    def debugInfo(self, lastTx=None, lastRx=None, logInfo=None):
        #if self.debugLog is not None:
        #    self.debugLog(lastTx, lastRx)
        pass

    def write(self, string):
        for s in string:
            d = bytearray(s)
            #print "%x"%d[0]
            self.oa.sendMessage(self.CODE_WRITE, self.ADDR_DATA, d, Validate=False)
            
        self.debugInfo(s)
            
    def inWaiting(self):
        resp = self.oa.sendMessage(self.CODE_READ, self.ADDR_LEN, Validate=False, maxResp=2)
        resp = resp[1]
        #print "%d waiting"%resp       
        return resp 

    def read(self, num=0, timeout=100):
        waiting = self.inWaiting()
        data = bytearray()

        #TODO: why is this needed? Some garbage at front...
        num = num + 1

        while (len(data) < num) and (timeout > 1):
            if waiting > 0:
                resp = self.oa.sendMessage(self.CODE_READ, self.ADDR_DATA, Validate=False, maxResp=1)
                if resp:
                    data.append(resp[0])
            else:
                time.sleep(0.01)
                timeout = timeout - 1
            waiting = self.inWaiting()
            
        if timeout <= 0:
            self.log("CW Serial timed out")

        #TODO: fix removing garbage at front
        result = data[1:(len(data)+1)]        
        result = result.decode("utf-8")
        self.debugInfo(lastRx=result)
        return result

    def flush(self):
        waiting = self.inWaiting()
        while waiting > 0:
            self.oa.sendMessage(self.CODE_READ, self.ADDR_DATA, Validate=False, maxResp=1)
            waiting = self.inWaiting()  
            
    def flushInput(self):
        self.flush()
            
    def close(self):
        pass
    
    def con(self):
        pass
       
class SimpleSerial(TargetTemplate):   
    paramListUpdated = Signal(list)     

    def setupParameters(self):
        ssParams = [{'name':'connection', 'type':'list', 'key':'con', 'values':{"System Serial Port":SimpleSerial_serial(self.showScriptParameter), "ChipWhisperer":SimpleSerial_ChipWhisperer(self.showScriptParameter)}, 'value':"System Serial Port", 'set':self.setConnection}]        
        self.params = Parameter.create(name='Target Connection', type='group', children=ssParams)
        ExtendedParameter.setupExtended(self.params, self)
        self.ser = None   
        
        self.setConnection(self.findParam('con').value())
      
    def setOpenADC(self, oadc):
        try:
            self.ser.setOpenADC(oadc)
        except:
            pass

    def setConnection(self, con):
        self.ser = con        
        self.paramListUpdated.emit(self.paramList)
        
    def paramList(self):
        p = [self.params]
        if self.ser is not None:
            for a in self.ser.paramList(): p.append(a)
        return p
    
    def con(self):        
        self.ser.con()
        # 'x' flushes everything & sets system back to idle
        self.ser.write("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx")
        self.ser.flush()

    def dis(self):
        self.close()
    
    def close(self):
        if self.ser != None:
            self.ser.close()
            self.ser = None
        return
        
    def init(self):
        return
      
    def setModeEncrypt(self):
        return

    def setModeDecrypt(self):
        return

    def loadEncryptionKey(self, key):
        if key != None:            
            cmd = "k"
            for b in key:
                cmd = cmd + "%2x"%b
            cmd = cmd + "\n"
            self.ser.flushInput()
            self.ser.write(cmd)            
            #self.ser.read(1)
      
    def loadInput(self, inputtext):
        self.input = inputtext

    def isDone(self):
        return True

    def readOutput(self):        
        response = self.ser.read(33)

        if len(response) < 33:
            self.log("WARNING: Response too short (len=%d): %s"%(len(response), response))
            return None

        if response[0] != 'r':
            self.log("Sync Error: %s"%response)
            return None

        data = bytearray(16)

        for i in range(0,16):
            data[i] = int(response[(i*2+1):(i*2+3)], 16)

        return data

    def go(self):
        self.ser.flushInput()
        cmd = "p"
        for b in self.input:
            cmd = cmd + "%2x"%b
        cmd = cmd + "\n"
        self.ser.flushInput()
        self.ser.write(cmd)
        #self.ser.read(1)       
        
class SimpleSerialWidget(SimpleSerial, QWidget):
    def __init__(self):
        super(SimpleSerialWidget, self).__init__()

        layout = QVBoxLayout()
        self.setLayout(layout)

        #Serial Settings (not changable right now)
        gbSerial = QGroupBox("Serial Settings")
        gbSerialLayout = QVBoxLayout()
        gbSerial.setLayout(gbSerialLayout)
        
        #Protocol Setup
        cbProtocol = QGroupBox("Protocol Information")
        cbProtocolLayout = QVBoxLayout()
        cbProtocol.setLayout(cbProtocolLayout)
        
        cbProtocolLayout.addWidget(QLabel("Set Key:       kXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\\n"))
        cbProtocolLayout.addWidget(QLabel( "Do Encryption: pXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\\n"))
        cbProtocolLayout.addWidget(QLabel( "Response:      rXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\\n"))
        
        layout.addWidget(cbProtocol)
        
        #Debug Stuff
        cbDebug = QGroupBox("Debug Information")
        cbDebugLayout = QGridLayout()
        cbDebug.setLayout(cbDebugLayout)
        
        self.txDebugASCII = QLabel()
        self.txDebugHEX = QLabel()
        self.rxDebugASCII = QLabel()
        self.rxDebugHEX = QLabel()
        
        cbDebugLayout.addWidget(QLabel("Last TX(ASCII)"), 1, 1)
        cbDebugLayout.addWidget(self.txDebugASCII, 1,  2)
        cbDebugLayout.addWidget(self.txDebugHEX, 2,  2)
        cbDebugLayout.addWidget(QLabel("Last RX"), 3, 1)
        cbDebugLayout.addWidget(self.rxDebugASCII, 3,  2)
        cbDebugLayout.addWidget(self.rxDebugHEX, 4,  2)
        
        layout.addWidget(cbDebug)
        
    def setDebugInfo(self,  lastSent=None,  lastResponse=None):
        if lastSent:
            self.txDebugASCII.setText(lastSent)
            string = ""
            for s in lastSent:                
                string = string + "%02x "%ord(s)
            self.txDebugHEX.setText(string)
        if lastResponse:
            self.rxDebugASCII.setText(lastResponse)
            string = ""
            for s in lastResponse:                
                string = string + "%02x "%ord(s)
            self.rxDebugHEX.setText(string)     

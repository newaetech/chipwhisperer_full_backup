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

import time

try:
    from pyqtgraph.parametertree import Parameter, ParameterTree, ParameterItem, registerParameterType
except ImportError:
    print "ERROR: PyQtGraph is required for this program"
    sys.exit()
    
sys.path.append('../common')
sys.path.append('../../openadc/controlsw/python/common')
from ExtendedParameter import ExtendedParameter

import ChipWhispererTargets

class ReaderTemplate(QObject):
    paramListUpdated = Signal(list)
    
    def __init__(self, console, showScriptParameter):
        """Pass None/None if you don't have/want console/showScriptParameter"""
        super(ReaderTemplate, self).__init__()        
        self.console = console
        self.showScriptParameter = showScriptParameter                
        self.setupParameters()
        
    def log(self, message):
        self.console.append(message)
                
    def setupParameters(self):
        """You should overload this. Copy/Paste into your class."""
        ssParams = [{'name':'Example Parameter', 'type':'int', 'value':5, 'set':self.setSomething}]        
        self.params = Parameter.create(name='Smartcard Reader', type='group', children=ssParams)
        ExtendedParameter.setupExtended(self.params, self)      
            
    def paramList(self):
        p = [self.params]
        #if self.ser is not None:
        #    for a in self.ser.paramList(): p.append(a)
        return p        
        
    def setSomething(self):
        """Here you would send value to the reader hardware"""
        pass 

    def sendAPDU(self):
        """Send APDU to SmartCard, get Response"""
        pass
    
    def con(self, oa=None):
        """Connect to reader. oa parameter is OpenADC/ChipWhisperer hardware, only used to integrated readers"""
        pass
    
    def flush(self):
        """Discard all input buffers"""
        pass
    
    def reset(self):
        """Reset card & save the ATR"""
        pass
    
    def getATR(self):
        """Get the ATR from the SmartCard. Reads a saved value, user reset() to actually reset card."""
        pass

class ReaderChipWhisperer(ReaderTemplate):
    
    def __init__(self, console=None, showScriptParameter=None):
        super(ReaderChipWhisperer, self).__init__(console, showScriptParameter)
        self.usi = ChipWhispererTargets.CWUniversalSerial()
        

    def setupParameters(self):
        ssParams = [{'name':'Baud', 'type':'int', 'value':9600, 'set':self.setBaud}                                                            
                    ]        
        self.params = Parameter.create(name='Target Connection', type='group', children=ssParams)
        ExtendedParameter.setupExtended(self.params, self)       

    def setBaud(self, brate):
        """Set baud rate of reader"""
        
        #For SmartCard we assume Stop-bits = 1, guard-bits=1, start-bits=1, parity=even
        
        self.usi.setBaud(brate)
        
    
    def sendAPDU(self, cla, ins, p1, p2, txdata=None, rxdatalen=0):
        """Send APDU to SmartCard, get Response"""        
    
        #SmartCard header
        data = [cla,ins, p1, p2]
        
        if txdata is not None:
            txdatalen = len(txdata)
            data.append(txdatalen)
        else:
            txdatalen = 0
            
        if (txdata is None) & (rxdatalen == 0):
            data.append(0)
        
        #Append gap during which SCard sends ACK
        data.append(None)
        
        #Append payload
        if txdata is not None:
            for b in txdata: data.append(b)
            
        if rxdatalen != 0:
            data.append(rxdatalen)
        
        #Append requested response size
    
        #Additional processing time
        proctime = 8
    
        #Start RX to catch response
        self.usi.read(2+rxdatalen+txdatalen+proctime, startonly=True)
        self.usi.write(data)
        p = bytearray(self.usi.read(2+rxdatalen+txdatalen+proctime, waitonly=True))
        
        if p[0] != ins:
            self.log("ACK Error: %x != %x"%(ins, p[0]))
            
        #for t in p:
        #    print "%x "%t,
            
        status = (p[-2] << 8) | p[-1]
        
        if rxdatalen > 0:
            payload = p[1:-2]
            return (status, payload)
        
        return status
    
    def con(self, oa=None):
        """Connect to reader. oa parameter is OpenADC/ChipWhisperer hardware, only used to integrated readers"""
        self.usi.con(oa)
        self.usi.setIdle(1)
        self.setBaud(9600)
        self.usi.setParity("even")        
        self.usi.setStopbits(2)
    
    def flush(self):
        """Discard all input buffers"""
        pass
    
    def reset(self):
        """Reset card & save the ATR"""
        pass
    
    def getATR(self):
        """Get the ATR from the SmartCard. Reads a saved value, user reset() to actually reset card."""
        pass
    
from smartcard.CardType import AnyCardType
from smartcard.CardRequest import CardRequest
from smartcard.util import toHexString

class ReaderPCSC(ReaderTemplate):
    def __init__(self, console=None, showScriptParameter=None):
        super(ReaderPCSC, self).__init__(console, showScriptParameter)
                

    def setupParameters(self):
        ssParams = [{'name':'Keep-Alive Interval (off=0)', 'type':'int', 'value':2, 'set':self.setKeepalive}                                                            
                    ]        
        self.params = Parameter.create(name='Target Connection', type='group', children=ssParams)
        ExtendedParameter.setupExtended(self.params, self)    
        
        
        self.timeoutTimer = QTimer()  
        self.timeoutTimer.timeout.connect(self.timeoutFired)
        self.timeoutTimer.setInterval(2000)

    def setKeepalive(self, kinterval):
        self.timeoutTimer.setInterval(kinterval*1000)
    
    def timeoutFired(self):
        self.scserv.connection.connect()       
    
    def sendAPDU(self, cla, ins, p1, p2, txdata=None, rxdatalen=0):
        """Send APDU to SmartCard, get Response"""       

        data = [cla,ins, p1, p2]
        
        if txdata is not None:
            txdatalen = len(txdata)
            data.append(txdatalen)
        else:
            txdatalen = 0
            
        if (txdata is None) & (rxdatalen == 0):
            data.append(0)
                
        #Append payload
        if txdata is not None:
            for b in txdata: data.append(b)
            
        if rxdatalen != 0:
            data.append(rxdatalen)
        
        response, sw1, sw2 = self.scserv.connection.transmit( data )
            
        status = (sw1 << 8) | sw2;
        
        if rxdatalen > 0:
            return (status, response)
        
        return status
    
    def con(self, oa=None):
        """Connect to reader. oa parameter is OpenADC/ChipWhisperer hardware, only used to integrated readers"""
        try:
            self.sccard = AnyCardType()
            self.screq = CardRequest(timeout=1, cardType=self.sccard)
            self.scserv = self.screq.waitforcard()

            #observer = ConsoleCardConnectionObserver()
            #self.scserv.connection.addObserver( observer )           
            
            if not self.timeoutTimer.isActive():
                self.timeoutTimer.start()

        except CardRequestTimeoutException:
            return False

        return True        
    
    def flush(self):
        """Discard all input buffers"""
        pass
    
    def reset(self):
        """Reset card & save the ATR"""
        pass
    
    def getATR(self):
        """Get the ATR from the SmartCard. Reads a saved value, user reset() to actually reset card."""
        pass
    
    def dis(self):
        self.scserv.connection.disconnect()
        self.timeoutTimer.stop()
    
class ProtocolTemplate(QObject):
    paramListUpdated = Signal(list)
    
    def __init__(self, console=None, showScriptParameter=None):
        super(ProtocolTemplate, self).__init__()
        
        self.console = console
        self.showScriptParameter = showScriptParameter
        self.hw = None
        self.setupParameters()
        
    def log(self, message):
        self.console.append(message)
                
    def setupParameters(self):
        """You should overload this. Copy/Paste into your class."""
        ssParams = [{'name':'Example Parameter', 'type':'int', 'value':5, 'set':self.setSomething}]        
        self.params = Parameter.create(name='Smartcard Reader', type='group', children=ssParams)
        ExtendedParameter.setupExtended(self.params, self)      
            
    def paramList(self):
        p = [self.params]
        #if self.ser is not None:
        #    for a in self.ser.paramList(): p.append(a)
        return p    

    def setReaderHardware(self, hw):
        self.hw = hw
    
    def init(self):
        pass
    
    def loadEncryptionKey(self, key):
        pass
      
    def loadInput(self, inputtext):
        self.input = inputtext

    def isDone(self):
        return True

    def readOutput(self):        
        return ""

    def go(self):
        pass    
    
class ProtocolSASEBOWCardOS(ProtocolTemplate):

    def setupParameters(self):
        """No parameters"""
        #ssParams = []        
        #self.params = Parameter.create(name='Smartcard Reader', type='group', children=ssParams)
        #ExtendedParameter.setupExtended(self.params, self)
        self.params = None
        
    def loadEncryptionKey(self, key):
        if len(key) != 16:
            raise ValueError("Encryption key != 16 bytes??")
        status = self.hw.sendAPDU(0x80, 0x12, 0x00, 0x00, key)
        #print status
        
    def loadInput(self, inputtext):
        self.inputtext = inputtext
        
    def go(self):
        if len(self.inputtext) != 16:
            raise ValueError("Plaintext != 16 bytes??")
        status = self.hw.sendAPDU(0x80, 0x04, 0x04, 0x00, self.inputtext)
        #print status
    
    def readOutput(self):
        (resp, pay) = self.hw.sendAPDU(0x80, 0xC0, 0x00, 0x00, rxdatalen=16)
        #print resp
        return pay    
               
class SmartCard(QObject):   
    paramListUpdated = Signal(list) 
     
    def __init__(self, console=None, showScriptParameter=None):
        super(SmartCard, self).__init__()
        
        self.console = console
        self.showScriptParameter = showScriptParameter
        self.oa=None
        self.driver = None
        ssParams = [{'name':'Reader Hardware', 'type':'list', 'values':{"ChipWhisperer-Connected":ReaderChipWhisperer(), "PC/SC Reader":ReaderPCSC(), "Cheapskate-Serial":None}, 'value':None, 'set':self.setConnection},
                    #"BasicCard v7.5 (INCOMPLETE"):None, 
                    #"Custom (INCOMPLETE)":None, "DPAContestv4 (INCOMPLETE)":None
                    {'name':'SmartCard Protocol', 'type':'list', 'values':{"SASEBO-W SmartCard OS":ProtocolSASEBOWCardOS(), "ChipWhisperer-Dumb":None}, 'value':None, 'set':self.setProtocol}                    
                    ]        
        self.params = Parameter.create(name='Target Connection', type='group', children=ssParams)
        ExtendedParameter.setupExtended(self.params, self)
        

    def __del__(self):
        self.close()
        
    def log(self, msg):
        if self.console is not None:
            self.console.append(msg)
        else:
            print msg
            
    def setOpenADC(self, oadc):
        try:
            self.oa=oadc
            self.driver.setOpenADC(oadc)
        except:
            pass

    def setConnection(self, con):
        self.driver = con        
        self.paramListUpdated.emit(self.paramList)
        
    def setProtocol(self, con):
        self.protocol = con
        self.paramListUpdated.emit(self.paramList)
        self.protocol.setReaderHardware(self.driver)
        
    def paramList(self):
        p = [self.params]
        if self.driver is not None:
            for a in self.driver.paramList(): p.append(a)
        return p
    
    def con(self):       
        self.driver.con(self.oa)
        self.driver.flush()
        self.protocol.setReaderHardware(self.driver)

    def dis(self):
        self.close()
    
    def close(self):
        if self.driver != None:
            self.driver.close()
            self.driver = None
        return
        
    def init(self):
        self.protocol.init()
      
    def loadEncryptionKey(self, key):
        self.protocol.loadEncryptionKey(key)
      
    def loadInput(self, inputtext):
        self.protocol.loadInput(inputtext)

    def isDone(self):
        return self.protocol.isDone()

    def readOutput(self):        
        return self.protocol.readOutput()

    def go(self):
        self.protocol.go()

    def setModeEncrypt(self):
        pass

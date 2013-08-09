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
    
class SmartCard_ChipWhisperer(QObject):
    paramListUpdated = Signal(list) 
    
    CODE_READ       = 0x80
    CODE_WRITE      = 0xC0

    ADDR_STATUS     = 30
    ADDR_HDRADDR     = 31
    ADDR_PLDADDR     = 32
    
    FLAG_ACKOK      = 0x10
    FLAG_BUSY       = 0x08
    FLAG_PASSTHRU   = 0x04
    FLAG_PRESENT    = 0x02
    FLAG_RESET      = 0x01

    
    def __init__(self, showScriptParameter=None):
        super(SmartCard_ChipWhisperer, self).__init__()
        ssParams = [{'name':'baud', 'type':'list', 'values':['9600'], 'value':'9600', 'get':self.baud, 'set':self.setBaud}]
        self.params = Parameter.create(name='SmartCard Settings', type='group', children=ssParams)
        ExtendedParameter.setupExtended(self.params, self)
        self.showScriptParameter = showScriptParameter
        self.debugLog = None   

    def paramTreeChanged(self, param, changes):
        if self.showScriptParameter is not None:
            self.showScriptParameter(param, changes, self.params)

    def setBaud(self, baud):
        return
    
    def baud(self):
        return 9600

    def paramList(self):
        return [self.params]
    
    def setOpenADC(self, oa):
        self.oa = oa

    def scGetStatus(self):
        sets = self.oa.sendMessage(self.CODE_READ, self.ADDR_STATUS, Validate=False)
        if sets:
            return sets[0]
        else:
            return 0

    def reset(self):
        #Flush
        #self.readAll()

        #Toggle Reset
        cmd = bytearray(1)
        #Reset active, pass-through on
        cmd[0] = self.FLAG_PASSTHRU | self.FLAG_RESET;        
        self.oa.sendMessage(self.CODE_WRITE, self.ADDR_STATUS, cmd, Validate=False)
        time.sleep(0.1)

        self.oa.flushInput()
        
        #Reset inactive, pass-through on
        cmd[0] = self.FLAG_PASSTHRU;
        self.oa.sendMessage(self.CODE_WRITE, self.ADDR_STATUS, cmd, Validate=False)

        #Wait for card to settle
        time.sleep(0.2)            

        #Get ATR
        data = self.readAll()
        stratr = "ATR = "
        for p in data:
            stratr = stratr + "%2X "%p

        #Disable pass-through
        cmd[0] = 0x00;
        self.oa.sendMessage(self.CODE_WRITE, self.ADDR_STATUS, cmd, Validate=False)            

        print stratr
        self.stratr = stratr
        return stratr  
      
    def readAll(self):
        resp = []
        while True:
            try:
                #For some reason just .read(50) or similar doesn't work,
                #need to call with individual 1-byte reads
                b = self.oa.serial.read(1)[0]
                #print "%2x "%b,
                resp.append(b)
            except:
                break


        return resp

    def readPayload(self):
        resp = self.oa.sendMessage(self.CODE_READ, self.ADDR_PLDADDR, Validate=False, maxResp=18)
        return bytearray(resp)

    def waitDone(self):
        resp = bytearray(1)
        resp[0] = self.FLAG_BUSY
        timeout = 0
        while((resp[0] & self.FLAG_BUSY) == self.FLAG_BUSY):
            resp = self.oa.sendMessage(self.CODE_READ, self.ADDR_STATUS, Validate=False)

            timeout = timeout + 1
            #time.sleep(0.01)
            
            if timeout > 100:
                return False

        if (resp[0] & self.FLAG_ACKOK):            
            return True
        else:
            print "No ACK from SCard?"
            return False

    def disconnect(self):
        self.oa = None
            
    def close(self):
        self.oa = None
        
    def init(self):
        self.reset()

    def getATR(self):
        return self.stratr

    def APDUHeader(self, cla, ins, p1, p2, cmd_datalen, rsp_datalen):
        header = bytearray()
        header.append(cla)
        header.append(ins)
        header.append(p1)
        header.append(p2)
        header.append(cmd_datalen)
        header.append(rsp_datalen)   
        self.oa.sendMessage(self.CODE_WRITE, self.ADDR_HDRADDR, header, Validate=False)       
        return True

    def APDUPayloadGo(self, payload=None):
        if payload:
            if len(payload) > 16:
                print "WARNING: APDU Payload must be < 16 bytes"
            payload = bytearray(payload)
            payload = payload + bytearray(range(16-len(payload)))
        else:
            payload = bytearray(range(16));

        #Padding
        payload += bytearray([0, 0])        
        self.oa.sendMessage(self.CODE_WRITE, self.ADDR_PLDADDR, payload, Validate=False)

    def APDUSend(self, cla, ins, p1, p2, data):
        self.APDUHeader(cla, ins, p1, p2, len(data), 0)
        self.APDUPayloadGo(data)
        
        self.waitDone()
        resp = self.readPayload()

        if len(resp) != 18:
            print "SASEBOW: USB Data Error, wrong Response Size"
            return 0

        status = resp[16:18]
        status = (status[0] << 8) | status[1]        
        return status

    def APDURecv(self, cla, ins, p1, p2, datalen):
        self.APDUHeader(cla, ins, p1, p2, 0, datalen)
        self.APDUPayloadGo(None)

        self.waitDone()
        resp = self.readPayload()

        if len(resp) != 18:
            print "SASEBOW: USB Data Error, wrong Response Size"
            return 0

        return bytearray(resp)
        #status = resp[16:18]
        #status = (status[0] << 8) | status[1]

        #resp = bytearray(resp)
        #return resp[0:datalen]       
 
    def APDUSendReceive(self, cla, ins, p1, p2, data):
        self.APDUHeader(cla, ins, p1, p2, len(data), 0)
        self.APDUPayloadGo(data)
        
        self.waitDone()
        resp = self.readPayload()

        if len(resp) != 18:
            print "SASEBOW: USB Data Error, wrong Response Size"
            return 0

        #for p in resp:
        #    print "%2x "%p,

        status = resp[16:18]
        status = (status[0] << 8) | status[1]        
        return status
        
    def flush(self):
        self.oa.flushInput()
       
class SmartCardDPAv4(QObject):   
    paramListUpdated = Signal(list) 
     
    def __init__(self, console=None, showScriptParameter=None):
        super(SmartCardDPAv4, self).__init__()
        
        self.console = console
        
        self.ser = None
        ssParams = [{'name':'connection', 'type':'list', 'values':{"None":None, "ChipWhisperer":SmartCard_ChipWhisperer(showScriptParameter)}, 'value':"ChipWhisperer", 'set':self.setConnection}]        
        self.params = Parameter.create(name='Target Connection', type='group', children=ssParams)
        ExtendedParameter.setupExtended(self.params, self)
        self.showScriptParameter = showScriptParameter

    def __del__(self):
        self.close()
    
    def paramTreeChanged(self, param, changes):
        if self.showScriptParameter is not None:
            self.showScriptParameter(param, changes, self.params)
    
    def log(self, msg):
        if self.console is not None:
            self.console.append(msg)
        else:
            print msg
            
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
        self.log("WARNING: Key change code unimplemented")
      
    def loadInput(self, inputtext):
        self.input = inputtext

    def isDone(self):
        return True

    def readOutput(self):        
        return

    def go(self):
        (data, resp) = self.ser.APDUSendReceive(0x80, 0xC0, 0x00, 0x00, self.input)
        print data
        print resp     


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
import time

from PySide.QtCore import *
from PySide.QtGui import *

try:
    from pyqtgraph.parametertree import Parameter, ParameterTree, ParameterItem, registerParameterType
except ImportError:
    print "ERROR: PyQtGraph is required for this program"
    sys.exit()
    
sys.path.append('../common')
sys.path.append('../../openadc/controlsw/python/common')
from ExtendedParameter import ExtendedParameter

from TargetTemplate import TargetTemplate

               
class SakuraG(TargetTemplate):   
    CODE_READ       = 0x80
    CODE_WRITE      = 0xC0
        
    ADDR_STATUS = 49
    ADDR_FIFO = 50
    
    FLAG_RESET = 0x01
    FLAG_WRFULL = 0x02
    FLAG_RDEMPTY = 0x04
     
    def setupParameters(self): 
        ssParams = [{'name':'Reset FPGA', 'type':'action', 'action':self.reset}]   
        self.params = Parameter.create(name='Target Connection', type='group', children=ssParams)
        ExtendedParameter.setupExtended(self.params, self)      
        self.oa = None
        self.fixedStart = True
                    
    def setOpenADC(self, oadc):
        self.oa = oadc
        
    def setProtocol(self, con):
        self.protocol = con
        self.paramListUpdated.emit(self.paramList)
        self.protocol.setReaderHardware(self.driver)
        
    def reset(self):
        self.oa.sendMessage(self.CODE_WRITE, self.ADDR_STATUS, [self.FLAG_RESET], Validate=False)
        time.sleep(0.05)
        self.oa.sendMessage(self.CODE_WRITE, self.ADDR_STATUS, [0x00], Validate=False)
        
    def con(self):   
        if self.oa is None:
            print "No OpenADC?"
            return
                
        #Reset AES Core
        self.oa.sendMessage(self.CODE_WRITE, self.ADDR_STATUS, [self.FLAG_RESET], Validate=False)
        self.oa.sendMessage(self.CODE_WRITE, self.ADDR_STATUS, [0x00], Validate=False)
                
        #Init
        self.init()
        
        return True

    def disconnect(self):
        return

    def flush(self):
        while (self.readStatus() & self.FLAG_RDEMPTY) != self.FLAG_RDEMPTY:
            self.oa.sendMessage(self.CODE_READ, self.ADDR_FIFO, Validate=False)

    def readStatus(self):
        b = self.oa.sendMessage(self.CODE_READ, self.ADDR_STATUS, Validate=False)
        return b[0]

    def writeMsg(self, msg):
                
        for b in msg:
            #while (self.readStatus() & self.FLAG_WRFULL) == self.FLAG_WRFULL:
            #    pass
                    
            self.oa.sendMessage(self.CODE_WRITE, self.ADDR_FIFO, [b], Validate=False)

    def readMsg(self, nbytes):
        
        msg = bytearray()
        
        for i in range(0, nbytes):
            if self.readStatus() & self.FLAG_RDEMPTY:
                pass
            
            b = self.oa.sendMessage(self.CODE_READ, self.ADDR_FIFO, Validate=False)
            msg.append(b[0])
            
        return msg

    def write(self, address, MSB, LSB):
        msg = bytearray(5)

        msg[0] = 0x01;
        msg[1] = (address >> 8) & 0xFF; #MSB
        msg[2] = address & 0xFF; #LSB
        msg[3] = MSB;
        msg[4] = LSB;

        #msg = bytearray(strmsg)
        #print "Write: %x %x %x %x %x"%(msg[0],msg[1],msg[2],msg[3],msg[4])

        self.writeMsg(msg)

    def read(self, address):
        self.flush()
        msg = bytearray(3)
        msg[0] = 0x00;
        msg[1] = (address >> 8) & 0xFF; #MSB
        msg[2] = address & 0xFF; #LSB
        self.writeMsg(msg)
        #print "Write: %x %x %x"%(msg[0],msg[1],msg[2]),
        
        msg = self.readMsg(2)

        #print " Read: %x %x"%(msg[0],msg[1])

        #Order = MSB, LSB
        return msg

    def read128(self, address):
        self.flush()
        msg = bytearray(3*8)
        for i in range(0, 8):
            msg[i*3] = 0x00;
            msg[i*3+1] = (address >> 8) & 0xFF;
            msg[i*3+2] = (address & 0xFF) + (i*2);
        self.writeMsg(msg)
        msg = self.readMsg(16)        
        return bytearray(msg)

    def close(self):
        #self.sasebo.close()
        pass

    def init(self):
        #Select AES
        self.write(0x0004, 0x00, 0x01)
        self.write(0x0006, 0x00, 0x00)

        #Reset AES module
        self.write(0x0002, 0x00, 0x04)
        self.write(0x0002, 0x00, 0x00)

        #Select AES output
        self.write(0x0008, 0x00, 0x01)
        self.write(0x000A, 0x00, 0x00)

    def setModeEncrypt(self):
        self.write(0x000C, 0x00, 0x00)

    def setModeDecrypt(self):
        self.write(0x000C, 0x00, 0x01)

    def checkEncryptionKey(self, key):
        #SAKURA-G AES Example has fixed first 9 bytes
        if self.fixedStart:
            for i in range(0,9):
                key[i] = i                
        return key 

    def loadEncryptionKey(self, key):
        """Encryption key is bytearray"""

        if key:                
            self.write(0x0100, key[0], key[1])
            self.write(0x0102, key[2], key[3])
            self.write(0x0104, key[4], key[5])
            self.write(0x0106, key[6], key[7])
            self.write(0x0108, key[8], key[9])
            self.write(0x010A, key[10], key[11])
            self.write(0x010C, key[12], key[13])
            self.write(0x010E, key[14], key[15])

        #Generate key schedule
        self.write(0x0002, 0x00, 0x02)

        #Wait for done
        while self.isDone() == False:
            continue

    def loadInput(self, inputtext):
        self.write(0x0140, inputtext[0], inputtext[1])
        self.write(0x0142, inputtext[2], inputtext[3])
        self.write(0x0144, inputtext[4], inputtext[5])
        self.write(0x0146, inputtext[6], inputtext[7])
        self.write(0x0148, inputtext[8], inputtext[9])
        self.write(0x014A, inputtext[10], inputtext[11])
        self.write(0x014C, inputtext[12], inputtext[13])
        self.write(0x014E, inputtext[14], inputtext[15])

    def isDone(self):
        result = self.read(0x0002)

        if result[0] == 0x00 and result[1] == 0x00:
            return True
        else:
            return False

    def readOutput(self):        
        return self.read128(0x0180)

    def setMode(self, mode):
        if mode == "encryption":
            self.write(0x000C, 0x00, 0x00)
        elif mode == "decryption":
            self.write(0x000C, 0x00, 0x01)
        else:
            print "Wrong mode!!!!"

    def go(self):
        self.write(0x0002, 0x00, 0x01)
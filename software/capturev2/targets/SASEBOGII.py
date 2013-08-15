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

import ftd2xx as ft

try:
    from pyqtgraph.parametertree import Parameter, ParameterTree, ParameterItem, registerParameterType
except ImportError:
    print "ERROR: PyQtGraph is required for this program"
    sys.exit()
    
sys.path.append('../common')
sys.path.append('../../openadc/controlsw/python/common')
from ExtendedParameter import ExtendedParameter

import ChipWhispererTargets

               
class SaseboGII(QObject):   
    paramListUpdated = Signal(list) 
     
    def __init__(self, console=None, showScriptParameter=None):
        super(SaseboGII, self).__init__()
        
        self.console = console
        self.showScriptParameter = showScriptParameter
        ssParams = []        
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
        pass
        
    def setProtocol(self, con):
        self.protocol = con
        self.paramListUpdated.emit(self.paramList)
        self.protocol.setReaderHardware(self.driver)
        
    def paramList(self):
        p = [self.params]
        return p
    
    def dis(self):
        self.close()

    def con(self):   
        try:
            self.sasebo = ft.openEx("FTSZ1IONB")
        except ft.ftd2xx.DeviceError, e:
            self.sasebo = None
            return False

        self.sasebo.setTimeouts(1000, 1000)
        return True

    def disconnect(self):
        return

    def flush(self):
        num = self.sasebo.getQueueStatus()
        if num > 0:
            self.sasebo.read(num)

    def write(self, address, MSB, LSB):
        msg = bytearray(5)

        msg[0] = 0x01;
        msg[1] = (address >> 8) & 0xFF; #MSB
        msg[2] = address & 0xFF; #LSB
        msg[3] = MSB;
        msg[4] = LSB;

        strmsg = str(msg);

        #msg = bytearray(strmsg)
        #print "Write: %x %x %x %x %x"%(msg[0],msg[1],msg[2],msg[3],msg[4])

        self.sasebo.write(strmsg)

    def read(self, address):
        self.flush()
        msg = bytearray(3)
        msg[0] = 0x00;
        msg[1] = (address >> 8) & 0xFF; #MSB
        msg[2] = address & 0xFF; #LSB
        self.sasebo.write(str(msg))
        #print "Write: %x %x %x"%(msg[0],msg[1],msg[2]),
        msg = self.sasebo.read(2)
        msg = bytearray(msg)

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
        self.sasebo.write(str(msg))
        msg = self.sasebo.read(16)        
        return bytearray(msg)

    def close(self):
        self.sasebo.close()

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

        #I don't know what this is but we need it? Not documented?
        self.write(0x0002, 0x00, 0x02)

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
#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2013-2014, NewAE Technology Inc
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

from _base import TargetTemplate
import unicodedata

try:
    import ftd2xx as ft
except OSError:  # also catches WindowsError
    raise ImportError


class SaseboGIIDPAContest(object):
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
        
        self.write(0x0002, 0x00, 0x02)
        while self.isDone() == False:
            continue

    def setModeEncrypt(self):
        self.write(0x000C, 0x00, 0x00)

    def setModeDecrypt(self):
        self.write(0x000C, 0x00, 0x01)

    def checkEncryptionKey(self, key):          
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

        #Generate key
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
            raise ValueError

    def go(self):
        self.write(0x0002, 0x00, 0x01)
    
    
class SaseboGIIAESRev1(object):
    def init(self):
        #Select AES
        self.write(0x0004, 0x00, 0x01)

        #Reset AES module
        self.write(0x0002, 0x00, 0x04)
        self.write(0x0002, 0x00, 0x00)

        #Select AES output
        self.write(0x0008, 0x00, 0x01)

    def setModeEncrypt(self):
        self.write(0x000C, 0x00, 0x00)

    def setModeDecrypt(self):
        self.write(0x000C, 0x00, 0x01)

    def checkEncryptionKey(self, key):          
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

        #Generate key
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
            raise ValueError

    def go(self):
        self.write(0x0002, 0x00, 0x01)
    
               
class SaseboGII(TargetTemplate):
    _name = "SASEBO GII"

    def __init__(self):
        TargetTemplate.__init__(self)

        self.getParams().addChildren([
        {'name': 'USB Serial #:', 'key': 'serno', 'type': 'list', 'values': ['Press Refresh'], 'value': 'Press Refresh'},
        {'name': 'Enumerate Attached Devices', 'key': 'pushsno', 'type': 'action', 'action': self.refreshSerial},
        ])

        self.sasebo = None

    def refreshSerial(self, _=None):
        serialnames = ft.listDevices()
        if serialnames == None:
            serialnames = [" No Connected Devices "]

        for i,s in enumerate(serialnames):
            if isinstance(s, unicode):
                serialnames[i] = unicodedata.normalize('NFC', s)

        self.findParam('serno').setLimits(serialnames)
        if len(serialnames) > 1:
            i = 1
        else:
            i = 0
        self.findParam('serno').setValue(serialnames[i])

    def _con(self, scope=None):
        self._sn = self.findParam('serno').getValue()
        try:
            self.sasebo = ft.openEx(self._sn)
        except ft.ftd2xx.DeviceError:
            self.sasebo = None
            raise Warning("Failed to connect to FTDI device (serial number: '%s'). Check 'Target' tab to ensure correct serial-number selected." % self._sn)
        
        self.sasebo.setTimeouts(1000, 1000)

        #Init
        self.init()
        return True

    def disconnect(self):
        return

    def flush(self):
        num = self.sasebo.getQueueStatus()
        if num > 0:
            self.sasebo.read(num)

    def write(self, address, MSB, LSB):
        if self.connectStatus.value()==False:
            raise Exception("Can't write to the target while disconnected. Connect to it first.")

        msg = bytearray(5)

        msg[0] = 0x01
        msg[1] = (address >> 8) & 0xFF #MSB
        msg[2] = address & 0xFF #LSB
        msg[3] = MSB
        msg[4] = LSB

        strmsg = str(msg)

        #msg = bytearray(strmsg)
        #print "Write: %x %x %x %x %x"%(msg[0],msg[1],msg[2],msg[3],msg[4])

        try:
            self.sasebo.write(strmsg)
        except Exception:
            self.dis()
            raise

    def read(self, address):
        self.flush()
        msg = bytearray(3)
        msg[0] = 0x00
        msg[1] = (address >> 8) & 0xFF #MSB
        msg[2] = address & 0xFF #LSB
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
            msg[i*3] = 0x00
            msg[i*3+1] = (address >> 8) & 0xFF
            msg[i*3+2] = (address & 0xFF) + (i*2)
        self.sasebo.write(str(msg))
        msg = self.sasebo.read(16)        
        return bytearray(msg)

    def close(self):
        if self.sasebo:
            self.sasebo.close()
        
    def reinit(self):
        self.init()

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
        
        #Generate key
        self.write(0x0002, 0x00, 0x02)
        while self.isDone() == False:
            continue

    def setModeEncrypt(self):
        self.write(0x000C, 0x00, 0x00)

    def setModeDecrypt(self):
        self.write(0x000C, 0x00, 0x01)

    def checkEncryptionKey(self, key):          
        return key 
    
    def timeoutWait(self, timeout=100):
        tm = 0
        while self.isDone() == False:
            tm += 1
            if tm > timeout:
                raise IOError("Timeout")

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

        #Generate key
        self.write(0x0002, 0x00, 0x02)

        self.timeoutWait()

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
            raise ValueError

    def go(self):
        self.write(0x0002, 0x00, 0x01)

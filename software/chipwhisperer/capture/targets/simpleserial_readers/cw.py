#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2013-2016, NewAE Technology Inc
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

from ._base import SimpleSerialTemplate
import time


class SimpleSerial_ChipWhisperer(SimpleSerialTemplate):
    _name = "ChipWhisperer"
    CODE_READ       = 0x80
    CODE_WRITE      = 0xC0
    ADDR_DATA       = 33
    ADDR_LEN        = 34
    ADDR_BAUD       = 35

    def __init__(self, parentParam=None):
        SimpleSerialTemplate.__init__(self, parentParam)
        self._regVer = 0
        self.params.addChildren([
            {'name':'TX Baud', 'key':'txbaud', 'type':'int', 'limits':(0, 1E6), 'value':38400, 'get':self.txBaud, 'set':self.setTxBaud},
            {'name':'RX Baud', 'key':'rxbaud', 'type':'int', 'limits':(0, 1E6), 'value':38400, 'get':self.rxBaud, 'set':self.setRxBaud},
            {'name':'Stop-Bits', 'key':'stopbits', 'type':'list', 'values':{'1':1, '2':2}, 'value':0, 'get':self.stopBits,
                            'set':self.setStopBits, 'readonly':True},
            {'name':'Parity', 'key':'parity', 'type':'list', 'values':{'None':'n', 'Even':'e'}, 'value':0, 'get':self.parity,
                            'set':self.setParity, 'readonly':True},
        ])

    def systemClk(self):
        return 30E6

    def setTxBaud(self, baud):
        breg = (baud * 4096 + self.systemClk() / 32) / (self.systemClk() / 16)
        breg = int(round(breg))
        self.setTxBaudReg(breg)

    def setRxBaud(self, baud):
        breg = (baud * 8 * 512 + self.systemClk() / 255) / (self.systemClk() / 128)
        breg = int(round(breg))
        self.setRxBaudReg(breg)

    def txBaud(self):
        breg = self.txBaudReg()
        baud = ((breg * (self.systemClk() / 16)) - (self.systemClk() / 32)) / 4096
        return baud

    def rxBaud(self):
        breg = self.rxBaudReg()
        baud = ((breg * (self.systemClk() / 128)) - (self.systemClk() / 255)) / 512
        baud = baud / 8
        return baud

    def checkVersion(self):
        """Check for newer version of register set - this MUST be called before any
           calls to setxxBaud, as otherwise these bits get blasted away
        """
        data = self.oa.sendMessage(self.CODE_READ, self.ADDR_BAUD, maxResp=4)

        if data is None:
            raise IOError("ChipWhisperer-SER Module not found, check you are using updated FPGA Bitstream")

        if(data[3] & 0b11000000) == 0b11000000:
            self._regVer = 1
            self.findParam('stopbits').setReadonly(False)
            self.findParam('parity').setReadonly(False)

    def setTxBaudReg(self, breg):
        data = self.oa.sendMessage(self.CODE_READ, self.ADDR_BAUD, maxResp=4)
        data[2] = breg & 0xff
        data[3] = (breg >> 8) & 0xff
        self.oa.sendMessage(self.CODE_WRITE, self.ADDR_BAUD, data)

    def setRxBaudReg(self, breg):
        data = self.oa.sendMessage(self.CODE_READ, self.ADDR_BAUD, maxResp=4)
        data[0] = breg & 0xff
        highbyte = (breg >> 8) & 0x3F
        data[1] = (data[1] & 0xC0) | highbyte
        self.oa.sendMessage(self.CODE_WRITE, self.ADDR_BAUD, data)

    def txBaudReg(self):
        data = self.oa.sendMessage(self.CODE_READ, self.ADDR_BAUD, maxResp=4)
        breg = data[2] | ((data[3] & 0x3F) << 8)
        return breg

    def rxBaudReg(self):
        data = self.oa.sendMessage(self.CODE_READ, self.ADDR_BAUD, maxResp=4)
        breg = data[0] | ((data[1] & 0x3F) << 8)
        return breg

    def stopBits(self):
        data = self.oa.sendMessage(self.CODE_READ, self.ADDR_BAUD, maxResp=4)
        if data[1] & (1 << 6):
            return 2
        else:
            return 1
        self.oa.sendMessage(self.CODE_WRITE, self.ADDR_BAUD, data)

    def setStopBits(self, stopbits):
        data = self.oa.sendMessage(self.CODE_READ, self.ADDR_BAUD, maxResp=4)
        if stopbits == 1:
            data[1] = data[1] & ~(1 << 6)
        else:
            data[1] = data[1] | (1 << 6)
        self.oa.sendMessage(self.CODE_WRITE, self.ADDR_BAUD, data)

    def parity(self):
        data = self.oa.sendMessage(self.CODE_READ, self.ADDR_BAUD, maxResp=4)
        if data[1] & (1 << 7):
            return 'e'
        else:
            return 'n'

    def setParity(self, par):
        data = self.oa.sendMessage(self.CODE_READ, self.ADDR_BAUD, maxResp=4)
        if par == 'e':
            data[1] = data[1] | (1 << 7)
        else:
            data[1] = data[1] & ~(1 << 7)
        self.oa.sendMessage(self.CODE_WRITE, self.ADDR_BAUD, data)

    def debugInfo(self, lastTx=None, lastRx=None, logInfo=None):
        #if self.debugLog is not None:
        #    self.debugLog(lastTx, lastRx)
        pass

    def write(self, string):
        for s in string:
            if isinstance(string, basestring):
                d = bytearray(s, 'latin-1')
            else:
                d = [s]
            #print "%x"%d[0]
            self.oa.sendMessage(self.CODE_WRITE, self.ADDR_DATA, d, Validate=False)

            self.debugInfo(s)

    def inWaiting(self):
        resp = self.oa.sendMessage(self.CODE_READ, self.ADDR_LEN, Validate=False, maxResp=2)
        resp = resp[1]
        # print "%d waiting"%resp
        return resp

    def read(self, num=0, timeout=100):
        waiting = self.inWaiting()
        data = bytearray()

        #TODO: why is this needed? Some garbage at front...
        # num = num + 1

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
            print("CW Serial timed out")

        #TODO: fix removing garbage at front
        # result = data[1:(len(data)+1)]
        result = data
        result = result.decode('latin-1')
        self.debugInfo(lastRx=result)
        return result

    def flush(self):
        waiting = self.inWaiting()
        while waiting > 0:
            self.oa.sendMessage(self.CODE_READ, self.ADDR_DATA, Validate=False, maxResp=1)
            waiting = self.inWaiting()

    def flushInput(self):
        self.flush()

    def con(self, scope = None):
        if not scope or not hasattr(scope, "qtadc"): Warning("You need a scope with OpenADC connected to use this Target")

        self.oa = scope.qtadc.ser
        scope.connectStatus.connect(self.dis())
        # Check first!
        self.checkVersion()
        self.params.getAllParameters()
        self.connectStatus.setValue(True)
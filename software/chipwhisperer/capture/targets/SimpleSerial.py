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

import time
from TargetTemplate import TargetTemplate
from chipwhisperer.common.utils import Util
from chipwhisperer.common.utils.Scan import scan
from chipwhisperer.common.api.config_parameter import ConfigParameter
from chipwhisperer.hardware.naeusb.serial import USART as CWL_USART


def getClass():
    return SimpleSerial


try:
    import serial

    class SimpleSerial_serial(TargetTemplate):
        name = "System Serial Port"

        def setupParameters(self):
            ssParams = [{'name':'Baud', 'key':'baud', 'type':'list', 'values':{'38400':38400, '19200':19200}, 'value':38400},
                        {'name':'Port', 'key':'port', 'type':'list', 'values':['Hit Refresh'], 'value':'Hit Refresh'},
                        {'name':'Refresh', 'type':'action', 'action':self.updateSerial}
                        ]
            self.params = ConfigParameter.create_extended(self, name='Serial Port Settings', type='group', children=ssParams)
            self.ser = None
    
        def paramList(self):
            return [self.params]
    
        def updateSerial(self):
            serialnames = scan.scan()
            self.findParam('port').setLimits(serialnames)
            if len(serialnames) > 0:
                self.findParam('port').setValue(serialnames[0])

        def selectionChanged(self):
            self.updateSerial()
    
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
    
        def con(self, scope = None):
            if self.ser == None:
    
                # Open serial port if not already
                self.ser = serial.Serial()
                self.ser.port     = self.findParam('port').value()
                self.ser.baudrate = self.findParam('baud').value()
                self.ser.timeout  = 2     # 2 second timeout
                self.ser.open()
                
except ImportError:
    SimpleSerial_serial = None


class SimpleSerial_ChipWhispererLite(TargetTemplate):
    name = "ChipWhisperer-Lite"

    def setupParameters(self):
        ssParams = [{'name':'baud', 'type':'int', 'key':'baud', 'value':38400, 'limits':(500, 2000000), 'get':self.baud, 'set':self.setBaud}]

        self.params = ConfigParameter.create_extended(self, name='Serial Port Settings', type='group', children=ssParams)
        self.cwlite_usart = None

    def setBaud(self, baud):
        if self.cwlite_usart:
            self.cwlite_usart.init(baud)
        else:
            print "Baud rate not set, need to connect first"
    
    def baud(self):
        return 38400
        
    def write(self, string):
        self.cwlite_usart.write(string)

    def inWaiting(self):
        return self.cwlite_usart.inWaiting()

    def read(self, num=0, timeout=250):
        data = bytearray(self.cwlite_usart.read(num, timeout=timeout))

        result = data.decode('latin-1')
        return result

    def flush(self):
        waiting = self.inWaiting()
        while waiting > 0:
            self.cwlite_usart.read(waiting)
            waiting = self.inWaiting()

    def flushInput(self):
        self.flush()

    def close(self):
        pass

    def con(self, scope = None):
        if scope is None or not hasattr(scope, "qtadc"): Warning("You need a scope with OpenADC connected to use this Target")

        scope.connectStatus.connect(self.dis)
        self.params.getAllParameters()
        self.cwlite_usart = CWL_USART(scope.qtadc.ser)
        self.cwlite_usart.init(baud=self.findParam('baud').value())
        self.connectStatus.setValue(True)
        
    def selectionChanged(self):
        pass


class SimpleSerial_ChipWhisperer(TargetTemplate):
    name = "ChipWhisperer"
    CODE_READ       = 0x80
    CODE_WRITE      = 0xC0
    ADDR_DATA       = 33
    ADDR_LEN        = 34
    ADDR_BAUD       = 35

    def setupParameters(self):
        ssParams = [{'name':'TX Baud', 'key':'txbaud', 'type':'int', 'limits':(0, 1E6), 'value':38400, 'get':self.txBaud, 'set':self.setTxBaud},
                    {'name':'RX Baud', 'key':'rxbaud', 'type':'int', 'limits':(0, 1E6), 'value':38400, 'get':self.rxBaud, 'set':self.setRxBaud},
                    {'name':'Stop-Bits', 'key':'stopbits', 'type':'list', 'values':{'1':1, '2':2}, 'value':0, 'get':self.stopBits,
                                    'set':self.setStopBits, 'readonly':True},
                    {'name':'Parity', 'key':'parity', 'type':'list', 'values':{'None':'n', 'Even':'e'}, 'value':0, 'get':self.parity,
                                    'set':self.setParity, 'readonly':True},
                    ]
        self._regVer = 0
        return ssParams

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
        
    def selectionChanged(self):
        pass


class SimpleSerial(TargetTemplate):
    name = "Simple Serial"

    def setupParameters(self):
        ser_cons = Util.putInDict([SimpleSerial_serial, SimpleSerial_ChipWhisperer, SimpleSerial_ChipWhispererLite], True)
        defSer = ser_cons[SimpleSerial_ChipWhispererLite.name]

        ssParams = [{'name':'Connection', 'type':'list', 'key':'con', 'values':ser_cons,'value':defSer, 'set':self.setConnection},
                    {'name':'Key Length', 'type':'list', 'values':[128, 256], 'value':128, 'set':self.setKeyLen},
                 #   {'name':'Plaintext Command', 'key':'ptcmd', 'type':'list', 'values':['p', 'h'], 'value':'p'},
                    {'name':'Init Command', 'key':'cmdinit', 'type':'str', 'value':''},
                    {'name':'Load Key Command', 'key':'cmdkey', 'type':'str', 'value':'k$KEY$\\n'},
                    {'name':'Load Input Command', 'key':'cmdinput', 'type':'str', 'value':''},
                    {'name':'Go Command','key':'cmdgo', 'type':'str', 'value':'p$TEXT$\\n'},
                    {'name':'Output Format', 'key':'cmdout', 'type':'str', 'value':'r$RESPONSE$\\n'},
                    #{'name':'Data Format', 'key':'datafmt', 'type':'list', 'values':{'DEADBEEF':'',
                    #                                                                 'DE AD BE EF':' ',
                    #                                                                 'DE:AD:BE:EF':':',
                    #                                                                 'DE-AD-BE-EF':'-'}, 'value':''},
                    ]
        self.ser = None
        self.keylength = 16
        self.input = ""
        self.setConnection(self.findParam('con').value())
        return ssParams

    def setKeyLen(self, klen):
        """ Set key length in BITS """
        self.keylength = klen / 8

    def keyLen(self):
        """ Return key length in BYTES """
        return self.keylength

    def setConnection(self, con):
        self.ser = con
        self.connectStatus = self.ser.connectStatus
        self.paramListUpdated.emit()
        self.ser.selectionChanged()

    def paramList(self):
        p = [self.params]
        if self.ser is not None:
            for a in self.ser.paramList(): p.append(a)
        return p

    def con(self, scope = None):
        if not scope or not hasattr(scope, "qtadc"): Warning("You need a scope with OpenADC connected to use this Target")

        self.ser.con(scope)
        # 'x' flushes everything & sets system back to idle
        self.ser.write("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx")
        self.ser.flush()
        self.connectStatus.setValue(True)

    def close(self):
        if self.ser != None:
            self.ser.close()
            # self.ser = None

    def init(self):
        self.runCommand(self.findParam('cmdinit').value())

    def setModeEncrypt(self):
        pass

    def setModeDecrypt(self):
        pass

    def convertVarToString(self, var):
        if isinstance(var, str):
            return var

        sep = ""
        s = sep.join(["%02x"%b for b in var])
        return s

    def runCommand(self, cmdstr, flushInputBefore=True):
        if self.connectStatus.value()==False:
            raise Warning("Can't write to the target while disconected. Connect to it first.")

        if cmdstr is None or len(cmdstr) == 0:
            return

        varList = [("$KEY$",self.key, "Hex Encryption Key"),
                   ("$TEXT$",self.input, "Input Plaintext")]

        newstr = cmdstr

        #Find variables to insert
        for v in varList:
            newstr = newstr.replace(v[0], self.convertVarToString(v[1]))

        #This is dumb
        newstr = newstr.replace("\\n", "\n")
        newstr = newstr.replace("\\r", "\r")

        #print newstr
        try:
            if flushInputBefore:
                self.ser.flushInput()
            self.ser.write(newstr)
        except Exception:
            self.dis()
            raise

    def loadEncryptionKey(self, key):
        self.key = key
        if self.key:
            self.runCommand(self.findParam('cmdkey').value())

    def loadInput(self, inputtext):
        self.input = inputtext
        self.runCommand(self.findParam('cmdinput').value())

    def isDone(self):
        return True

    def readOutput(self):
        dataLen= 32

        fmt = self.findParam('cmdout').value()
        #This is dumb
        fmt = fmt.replace("\\n", "\n")
        fmt = fmt.replace("\\r", "\r")

        if len(fmt) == 0:
            return None

        if fmt.startswith("$GLITCH$"):

            try:
                databytes = int(fmt.replace("$GLITCH$",""))
            except ValueError:
                databytes = 64


            self.newInputData.emit(self.ser.read(databytes))
            return None

        dataLen += len(fmt.replace("$RESPONSE$", ""))
        expected = fmt.split("$RESPONSE$")

        #Read data from serial port
        response = self.ser.read(dataLen)

        if len(response) < dataLen:
            print("WARNING: Response too short (len=%d): %s"%(len(response), response))
            return None

        #Go through...skipping expected if applicable
        #Check expected first

        #Is a beginning part
        if len(expected[0]) > 0:
            if response[0:len(expected[0])] != expected[0]:
                print("Sync Error: %s"%response)
                print("Hex Version: %s" % (" ".join(["%02x" % ord(t) for t in response])))
                return None

        startindx = len(expected[0])

        #Is middle part?
        data = bytearray(16)
        if len(expected) == 2:
            for i in range(0,16):
                data[i] = int(response[(i * 2 + startindx):(i * 2 + startindx + 2)], 16)

            startindx += 32

        #Is end part?
        if len(expected[1]) > 0:
            if response[startindx:startindx+len(expected[1])] != expected[1]:
                print("Sync Error: %s"%response)
                return None

        return data

    def go(self):
        self.runCommand(self.findParam('cmdgo').value())

    def checkEncryptionKey(self, kin):
        blen = self.keyLen()

        if len(kin) < blen:
            print "note: Padding key..."
            newkey = bytearray(kin)
            newkey += bytearray([0]*(blen - len(kin)))
            return newkey
        elif len(kin) > blen:
            print "note: Truncating key..."
            return kin[0:blen]

        return kin

    def validateSettings(self):
        return []


class SimpleSerialWidget(SimpleSerial):
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

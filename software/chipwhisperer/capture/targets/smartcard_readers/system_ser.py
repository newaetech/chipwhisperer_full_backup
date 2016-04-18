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

from _base import ReaderTemplate
import serial
import time
from chipwhisperer.common.utils import Scan


class ReaderSystemSER(ReaderTemplate):
    name = "System Serial (SASEBO-W)"

    def __init__(self):
        super(ReaderSystemSER, self).__init__()

    def setupParameters(self):
        self.ser = None
        return [
                    {'name':'Port', 'key':'port', 'type':'list', 'values':['Hit Refresh'], 'value':'Hit Refresh'},
                    {'name':'Refresh', 'type':'action', 'action':self.updateSerial},
                    {'name':'Get ATR (Reset Card)', 'type':'action', 'action':self.reset},
                    {'name':'ATR', 'key':'atr', 'type':'str'}
                ]

    def updateSerial(self):
        serialnames = Scan.scan()
        self.findParam('port').setLimits(serialnames)
        if len(serialnames) > 0:
            self.findParam('port').setValue(serialnames[0])

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

    def sendAPDU(self, cla, ins, p1, p2, txdata=None, rxdatalen=0):
        """Send APDU to SmartCard, get Response"""

        #SmartCard header
        data = [cla,ins, p1, p2]

        #Flush input
        self.ser.flush()

        if txdata is not None:
            txdatalen = len(txdata)
            data.append(txdatalen)
        else:
            txdatalen = 0
            data.append(rxdatalen)

        #Send data
        self.ser.write(data)

        #Wait for ACK - don't fail yet as some scards
        #don't send ACK properly it seems (mainly DPA Contest v4)
        ack = self.ser.read(1)

        #Send payload
        if txdata and len(txdata) > 0:
            self.ser.write(txdata)

        #Send length request if we had data
        if txdatalen > 0 and rxdatalen > 0:
            self.ser.write([rxdatalen])
            self.ser.read(1)

        #Read Payload data
        rxdata = []
        while len(rxdata) < rxdatalen:
            char = self.ser.read(1)
            if len(char) == 0:
                break
            rxdata.extend(char)

        #Convert
        rxdata = [ord(t) for t in rxdata]

        #Read two bytes of status
        stat = self.ser.read(2)

        # Uncomment to print data stream on RX pin
        #print "APDU: ",
        #for t in rxdata: print "%02x " % t,
        #print ""

        if len(ack) < 1:
            print "ACK Error: not received?"
        elif ord(ack[0]) != ins:
            print "ACK Error: %x != %x"%(ins, ord(ack[0]))

        if len(stat) < 2:
            raise IOError("Status too small: %d, %s" % (len(stat), " ".join(["%02x"%ord(t) for t in stat])))

        status = (ord(stat[0]) << 8) | ord(stat[1])

        if rxdatalen > 0:
            payload = rxdata
            return (status, payload)

        return status

    def con(self, scope = None):
        """Connect to reader. oa parameter is OpenADC/ChipWhisperer hardware, only used to integrated readers"""
        if self.ser == None:
            # Open serial port if not already
            self.ser = serial.Serial()
            self.ser.port = self.findParam('port').value()
            self.ser.baudrate = 9600
            self.ser.stopbits = serial.STOPBITS_TWO
            self.ser.parity = serial.PARITY_EVEN
            self.ser.timeout = 2     # 2 second timeout
            self.ser.dtr = False
            self.ser.rts = True
            self.ser.open()
        self.reset()

    def reset(self):
        """Reset card & save the ATR"""

        self.atr = [0]
        self.findParam('atr').setValue("--atr not read--")

        #Toggle GPIO1
        self.ser.setRTS(True)
        #Flush serial port
        self.ser.flush()
        time.sleep(0.2)
        self.ser.setRTS(False)
        time.sleep(0.5)

        #Read ATR
        atr = []
        rxdata = [0,0,0]
        while len(rxdata) > 0:
            self.ser.timeout = 0.2
            rxdata = self.ser.read(1)
            atr.extend(rxdata)

        self.ser.timeout = 2.0

        self.atr = [ord(t) for t in atr]
        stratr = " ".join(["%02x"%ord(t) for t in atr])
        print "ATR: %s"%stratr
        self.findParam('atr').setValue(stratr)

    def getATR(self):
        """Get the ATR from the SmartCard. Reads a saved value, user reset() to actually reset card."""
        return self.atr

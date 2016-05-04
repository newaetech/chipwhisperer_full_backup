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
from ..simpleserial_readers.cw import SimpleSerial_ChipWhisperer
import time


class ReaderChipWhispererSER(ReaderTemplate):
    _name = "CWCR2-SER"

    def __init__(self, parentParam=None):
        ReaderTemplate.__init__(self, parentParam)

        self.ser = SimpleSerial_ChipWhisperer(self)
        self.setupActiveParams([lambda: self.lazy(self), lambda: self.lazy(self.ser)])

        self.params.addChildren([
            {'name':'Reset Pin', 'type':'list', 'values':['GPIO1']},
            {'name':'Get ATR (Reset Card)', 'type':'action', 'action':self.reset},
            {'name':'ATR', 'key':'atr', 'type':'str'}
        ])

    def waitEcho(self, data):
        rxdata = []
        while len(rxdata) < len(data):
            char = self.ser.read(1)
            if len(char) == 0:
                print "SCARD TIMEOUT: Failed to echo data?"
                print "  SENT: %s"%" ".join(["%02x"%t for t in data])
                print "  RECEIVED: %s"%" ".join(["%02x"%ord(t) for t in rxdata])
                raise IOError("SmartCard Line Stuck?")
            rxdata.extend(char)

        #print "  SENT: %s"%" ".join(["%02x"%t for t in data])
        #print "  RECEIVED: %s"%" ".join(["%02x"%ord(t) for t in rxdata])

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
        self.waitEcho(data)

        #Wait for ACK - don't fail yet as some scards
        #don't send ACK properly it seems (mainly DPA Contest v4)
        ack = self.ser.read(1)

        #Send payload
        if txdata and len(txdata) > 0:
            self.ser.write(txdata)
            self.waitEcho(txdata)

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
        self.ser.con(scope)

        #Set defaults
        self.ser.findParam('parity').setValue('e')

        #NB: This should be '2' I thought, but the turn-around time on the SASEBO-W card is too fast, so
        #    this needs to be reduced to '1' to avoid a collision.
        self.ser.findParam('stopbits').setValue(1)

        self.ser.findParam('rxbaud').setValue(9600)
        self.ser.findParam('txbaud').setValue(9600)

        #Setup GPIO Pins
        if hasattr(scope, 'advancedSettings') and scope.advancedSettings:
            self.cwe = scope.advancedSettings
            self.cwe.findParam('gpio1mode').setValue(self.cwe.cwEXTRA.IOROUTE_GPIOE)
            self.cwe.findParam('gpio2mode').setValue(self.cwe.cwEXTRA.IOROUTE_HIGHZ)
            self.cwe.findParam('gpio3mode').setValue(self.cwe.cwEXTRA.IOROUTE_STXRX)
            self.cwe.findParam('gpiostate1').setValue(True)
        else:
            self.cwe = None

        self.reset()

    def flush(self):
        """Discard all input buffers"""
        self.ser.flush()

    def reset(self):
        """Reset card & save the ATR"""

        self.atr = [0]
        self.ser.findParam('atr').setValue("--atr not read--")

        if self.cwe is None:
            return

        #Toggle GPIO1
        self.cwe.cwEXTRA.setGPIOState(False, 0)
        #Flush serial port
        self.ser.flush()
        time.sleep(0.01)
        self.cwe.cwEXTRA.setGPIOState(True, 0)

        #Read ATR
        atr = []
        rxdata = [0,0,0]
        while len(rxdata) > 0:
            rxdata = self.ser.read(1, 200)
            atr.extend(rxdata)

        self.atr = [ord(t) for t in atr]

        stratr = " ".join(["%02x"%ord(t) for t in atr])
        print "ATR: %s"%stratr
        self.ser.findParam('atr').setValue(stratr)

    def getATR(self):
        """Get the ATR from the SmartCard. Reads a saved value, user reset() to actually reset card."""
        return self.atr

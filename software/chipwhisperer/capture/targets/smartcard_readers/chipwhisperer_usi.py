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
import chipwhisperer.capture.scopes.cwhardware.ChipWhispererTargets as ChipWhispererTargets


class ReaderChipWhispererUSI(ReaderTemplate):
    _name = "CWCR2-USI (obsolete)"

    def __init__(self):
        ReaderTemplate.__init__(self)
        self.getParams().addChildren([
            {'name':'Baud', 'type':'int', 'value':9600, 'action':lambda p: self.setBaud(p.getValue())}
        ])
        self.usi = ChipWhispererTargets.CWUniversalSerial()

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
        temprx = bytearray(self.usi.read(2 + rxdatalen + txdatalen + proctime, waitonly=True))

        # Uncomment to print data stream on RX pin
        print "APDU: ",
        for t in temprx: print "%02x " % t,
        print ""

        # We've actually read entire sent + received data in
        # Strip header
        del temprx[0:5]

        # Strip transmitted data
        if txdatalen > 0:
            del temprx[1:(txdatalen + 1)]

        # Strip receive data length, present only if TX + RX happened
        if rxdatalen > 0 and txdatalen > 0:
            del temprx[1]

        p = temprx

        if p[0] != ins:
            print("ACK Error: %x != %x"%(ins, p[0]))

        #for t in p:
        #    print "%x "%t,

        if len(p) < 2:
            raise IOError("Payload too small: %d, %s" % (len(p), str(p)))

        status = (p[-2] << 8) | p[-1]

        if rxdatalen > 0:
            payload = p[1:-2]
            return (status, payload)

        return status

    def con(self, scope=None):
        """Connect to reader. scope parameter is OpenADC/ChipWhisperer hardware, only used to integrated readers"""
        self.usi.con(scope.qtadc.ser)
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

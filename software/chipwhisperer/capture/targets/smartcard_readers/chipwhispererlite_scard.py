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


class ReaderChipWhispererLiteSCard(ReaderTemplate):
    name = "CW1173/1180-SCARD"
    REQ_DATA = 0x1C
    REQ_CFG = 0x1D
    REQ_AUX = 0x1E

    REQ_CFG_ATR = 0x01
    REQ_CFG_PROTOCOL = 0x02
    REQ_CFG_TXRX = 0x05

    def __init__(self):
        super(ReaderChipWhispererLiteSCard, self).__init__()

    def setupParameters(self):
        self.protocol = 0
        return [
                  {'name':'Get ATR (Reset Card)', 'type':'action', 'action':self.reset},
                  {'name':'ATR', 'key':'atr', 'type':'str'},
                ]

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

        if self.protocol:
            #T-1 Mode needs header added
            data.insert(0, len(data))
            data.insert(0, 0)
            data.insert(0, 0)

        #Load APDU into buffer & send
        #TODO: Break APDU into multiple transmissions for larger blocks
        offset = 0
        self.usbcon.sendCtrl(self.REQ_DATA, value=offset, data=data)
        self.usbcon.sendCtrl(self.REQ_CFG, self.REQ_CFG_TXRX)

        #Read response
        if self.protocol:
            contRead = True
            offset = 0
            resplen = 64
            response = []
            while contRead:
                data = self.usbcon.readCtrl(self.REQ_DATA, value = ((offset << 8) | resplen), dlen=resplen)
                response.extend(data)

                if len(data) == resplen:
                    offset += resplen
                else:
                    contRead = False

            #Simple reader passes everything back to us
            print response

            if ((response[1] & 0x7F) == 0x81) or((response[1] & 0x7F) == 0x82):
                   raise IOError("Error on response:, T-1 PCB = %02x"%response[1])

            #Get length
            resplen = response[2]
            #Cut off first three bytes & CRC bytes
            response = response[3:(3+resplen)]
            print response
            #Get status
            status = (response[0] << 8) | response[1]

            #Return status seperately from "payload" if any
            if len(response) > 2:
                return (status, response[2:])
            else:
                return status
        else:
            #TODO: break into multiple as well for larger blocks?
            offset = 0
            resplen = rxdatalen+2
            response = self.usbcon.readCtrl(self.REQ_DATA, value = ((offset << 8) | resplen), dlen=resplen)
            sw1 = response[0]
            sw2 = response[1]

            status = (sw1 << 8) | sw2;

            if rxdatalen > 0:
                response = response[2:]
                return (status, response)

            return status

    def con(self, scope):
        """Connect to reader. oa parameter is OpenADC/ChipWhisperer hardware, only used to integrated readers"""
        self.usbcon = scope.qtadc.ser
        self.reset()

    def flush(self):
        """Discard all input buffers"""
        pass

    def reset(self):
        """Reset card & save the ATR"""

        self.usbcon.sendCtrl(self.REQ_CFG, self.REQ_CFG_ATR)
        self.atr = self.usbcon.readCtrl(self.REQ_CFG, self.REQ_CFG_ATR, 55)
        stratr = " ".join(["%02x"%t for t in self.atr])
        print "ATR: %s"%stratr
        self.findParam('atr').setValue(stratr)

        self.protocol = self.usbcon.readCtrl(self.REQ_CFG, self.REQ_CFG_PROTOCOL, 1)[0]

        print "SmartCard Protocol = T-%d"%self.protocol

    def getATR(self):
        """Get the ATR from the SmartCard. Reads a saved value, user reset() to actually reset card."""
        return self.atr

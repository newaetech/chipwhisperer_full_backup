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
import logging

from _base import ReaderTemplate
from chipwhisperer.common.utils import timer
from chipwhisperer.common.utils.parameter import setupSetParam

try:
    from smartcard.CardType import AnyCardType
    from smartcard.CardRequest import CardRequest
    from smartcard.CardConnection import CardConnection
    from smartcard.util import toHexString
except ImportError:
    raise ImportError("Smartcard libraries are missing")


class ReaderPCSC(ReaderTemplate):
    _name = "PC/SC Reader"

    def __init__(self):
        ReaderTemplate.__init__(self)

        self.timeoutTimer = timer.Timer()
        self.timeoutTimer.timeout.connect(self.timeoutFired)
        self.timeoutTimer.setInterval(2000)
        self.getParams().addChildren([
            {'name':'Keep-Alive Interval (off=0)', 'type':'int', 'value':2, 'set':self.setKeepalive}
        ])

    @setupSetParam("Keep-Alive Interval (off=0)")
    def setKeepalive(self, kinterval):
        self.timeoutTimer.setInterval(kinterval*1000)

    def timeoutFired(self):
        self.scserv.connection.connect()

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

        # Append payload
        if txdata is not None:
            for b in txdata: data.append(b)

        if rxdatalen != 0:
            data.append(rxdatalen)

        response, sw1, sw2 = self.scserv.connection.transmit(data , CardConnection.T0_protocol)

        status = (sw1 << 8) | sw2

        if rxdatalen > 0:
            return (status, response)

        return status

    def con(self, scope=None):
        """Connect to reader. scope parameter is OpenADC/ChipWhisperer hardware, only used to integrated readers"""
        try:
            self.sccard = AnyCardType()
            self.screq = CardRequest(timeout=1, cardType=self.sccard)
            self.scserv = self.screq.waitforcard()

            if not self.timeoutTimer.isActive():
                self.timeoutTimer.start()

            logging.info('SCARD: Connected...')
        except Exception:
            raise Exception("SCARD: Failed to connect...")

    def flush(self):
        """Discard all input buffers"""
        pass

    def reset(self):
        """Reset card & save the ATR"""
        pass

    def getATR(self):
        """Get the ATR from the SmartCard. Reads a saved value, user reset() to actually reset card."""
        pass

    def dis(self):
        self.scserv.connection.disconnect()
        self.timeoutTimer.stop()

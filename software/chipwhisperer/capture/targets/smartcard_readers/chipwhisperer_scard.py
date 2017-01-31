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

from ._base import ReaderTemplate
import chipwhisperer.capture.scopes.cwhardware.ChipWhispererTargets as ChipWhispererTargets


class ReaderChipWhispererSCard(ReaderTemplate):
    _name = "CWCR2-SCARD (obsolete)"

    def __init__(self):
        ReaderTemplate.__init__(self)

        self.params.addChildren([
            {'name':'Card Present', 'key':'statusStr', 'type':'bool', 'value':False, 'readonly':True},
            {'name':'Update Status', 'type':'action', 'action':self.statusUpdate},
            {'name':'Answer To Reset (ATR)', 'key':'atr', 'type':'str', 'value':'', 'readonly':True},
            {'name':'Reset Card', 'type':'action', 'action':self.reset},
        ])

        self.scard = ChipWhispererTargets.CWSCardIntegrated()

    def reset(self):
        atr = self.scard.reset()
        self.findParam('atr').setValue(atr)

    def getATR(self):
        return self.scard.stratr

    def statusUpdate(self):
        self.findParam('statusStr').setValue(self.scard.isPresent())

    def con(self, scope=None):
        self.scard.con(scope.qtadc.ser)
        self.reset()

    def sendAPDU(self, cla, ins, p1, p2, txdata=None, rxdatalen=0):
        """Send APDU to SmartCard, get Response"""

        if txdata is None:
            resp = self.scard.APDURecv(cla, ins, p1, p2, rxdatalen)
            status = resp[16:18]
            status = (status[0] << 8) | status[1]
            payload = resp[0:rxdatalen]

            logging.info('%4x' % status)

            return (status, payload)

        else:
            return self.scard.APDUSend(cla, ins, p1, p2, txdata)

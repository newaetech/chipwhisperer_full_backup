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

from ._base import ProtocolTemplate


class ProtocolJCardTest(ProtocolTemplate):
    _name = "JCARD"

    def loadEncryptionKey(self, key):
        pass

    def loadInput(self, inputtext):
        pass

    def go(self):

        # self.hw.scserv.connection.disconnect()
        # self.hw.con()

        status = self.hw.sendAPDU(0x00, 0xA4, 0x04, 0x00, [0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0x01, 0x01])
        if status != 0x9000:
            raise IOError("Invalid Status: %x" % status)

        (resp, pay) = self.hw.sendAPDU(0x80, 0x10, 0x01, 0x02, [0xAE], rxdatalen=6)
        if  resp != 0x9000:
            raise IOError("Invalid Status: %x" % status)

        counter1 = (pay[5] << 8) | pay[6]
        counter2 = (pay[7] << 8) | pay[8]
        counter3 = (pay[9] << 8) | pay[10]

        self.resp = (counter1, counter2, counter3)
        logging.debug(str(self.resp))

    def readOutput(self):
        return self.resp

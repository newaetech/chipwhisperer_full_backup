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

from chipwhisperer.common.utils.pluginmanager import Plugin


class ReaderTemplate(Plugin):
    name='Smartcard Reader'

    def __init__(self, parentParam=None):
        Plugin.__init__(self, parentParam)

        # self.params.addChildren([
        #     {'name':'Example Parameter', 'type':'int', 'value':5, 'set':self.setSomething}
        # ])

    def setSomething(self):
        """Here you would send value to the reader hardware"""
        pass

    def sendAPDU(self, cla, ins, p1, p2, txdata=None, rxdatalen=0):
        """Send APDU to SmartCard, get Response"""
        pass

    def con(self, scope=None):
        """Connect to reader."""
        pass

    def close(self):
        """Close connection."""
        pass

    def flush(self):
        """Discard all input buffers"""
        pass

    def reset(self):
        """Reset card & save the ATR"""
        pass

    def getATR(self):
        """Get the ATR from the SmartCard. Reads a saved value, user reset() to actually reset card."""
        pass

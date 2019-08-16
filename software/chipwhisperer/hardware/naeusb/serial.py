# -*- coding: utf-8 -*-
#
# Copyright (c) 2014-2018, NewAE Technology Inc
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
#==========================================================================


import time
import os
from .naeusb import packuint32

class USART(object):
    """
    USART Class communicates with NewAE USB Interface to read/write data over control endpoint.
    """

    CMD_USART0_DATA = 0x1A
    CMD_USART0_CONFIG = 0x1B

    USART_CMD_INIT = 0x0010
    USART_CMD_ENABLE = 0x0011
    USART_CMD_DISABLE = 0x0012
    USART_CMD_NUMWAIT = 0x0014

    def __init__(self, usb, timeout=200):
        """
        Set the USB communications instance.
        """

        self._usb = usb
        self.timeout = timeout

        self._baud = 38400
        self._stopbits = 1
        self._parity = "none"

    def init(self, baud=115200, stopbits=1, parity="none"):
        """
        Open the serial port, set baud rate, parity, etc.
        """

        self._baud = baud
        self._stopbits = stopbits
        self._parity = parity

        if stopbits == 1:
            stopbits = 0
        elif stopbits == 1.5:
            stopbits = 1
        elif stopbits == 2:
            stopbits = 2
        else:
            raise ValueError("Invalid stop-bit spec: %s" % str(stopbits))

        if parity == "none":
            parity = 0
        elif parity == "odd":
            parity = 1
        elif parity == "even":
            parity = 2
        elif parity == "mark":
            parity = 3
        elif parity == "space":
            parity = 4
        else:
            raise ValueError("Invalid parity spec: %s" % str(parity))

        cmdbuf = packuint32(baud)
        cmdbuf.append(stopbits)
        cmdbuf.append(parity)
        cmdbuf.append(8)  # Data bits

        self._usartTxCmd(self.USART_CMD_INIT, cmdbuf)
        self._usartTxCmd(self.USART_CMD_ENABLE)
        print("Serial baud rate = {}".format(baud))

    def write(self, data, slow=False):
        """
        Send data to serial port.
        """
        # print "%d: %s" % (len(data), str(data))

        try:
            data = bytearray(data)
        except TypeError:
            try:
                data = bytearray(data, 'latin-1')
            except TypeError:
                #Second type-error happens if input was already list?
                pass

        datasent = 0

        while datasent < len(data):
            datatosend = len(data) - datasent
            datatosend = min(datatosend, 58)
            self._usb.sendCtrl(self.CMD_USART0_DATA, 0, data[datasent:(datasent + datatosend)])
            datasent += datatosend

    def flush(self):
        """
        Flush all input buffers
        """
        inwait = self.inWaiting()
        while(inwait):
            self.read(inwait)
            inwait = self.inWaiting()

    def inWaiting(self):
        """
        Get number of bytes waiting to be read.
        """
        # print "Checking Waiting..."
        data = self._usartRxCmd(self.USART_CMD_NUMWAIT, dlen=4)
        # print data
        return data[0]

    def read(self, dlen=0, timeout=0):
        """
        Read data from input buffer, if 'dlen' is 0 everything present is read. If timeout is non-zero
        system will block for a while until data is present in buffer.
        """

        waiting = self.inWaiting()

        if dlen == 0:
            dlen = waiting

        if timeout == 0:
            timeout = self.timeout


        resp = []

        while dlen and (timeout * 10) > 0:
            if waiting > 0:
                newdata = self._usb.readCtrl(self.CMD_USART0_DATA, 0, min(waiting, dlen))
                resp.extend(newdata)
                dlen -= len(newdata)
            waiting = self.inWaiting()
            timeout -= 1

        return resp


    def _usartTxCmd(self, cmd, data=[]):
        """
        Send a command to the USART interface (internal function).
        """

        # windex selects interface
        self._usb.sendCtrl(self.CMD_USART0_CONFIG, cmd, data)

    def _usartRxCmd(self, cmd, dlen=1):
        """
        Read the result of some command (internal function).
        """
        # windex selects interface, set to 0
        return self._usb.readCtrl(self.CMD_USART0_CONFIG, cmd, dlen)

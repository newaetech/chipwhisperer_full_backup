# -*- coding: utf-8 -*-
#
# Copyright (c) 2014-2021, NewAE Technology Inc
# All rights reserved.
#
# Find this and more at newae.com - this file is part of the chipwhisperer
# project, http://www.chipwhisperer.com . ChipWhisperer is a registered
# trademark of NewAE Technology Inc in the US & Europe.
#
#    This file is part of chipwhisperer.
#
#    Licensed under the Apache License, Version 2.0 (the "License");
#    you may not use this file except in compliance with the License.
#    You may obtain a copy of the License at
#
#       http://www.apache.org/licenses/LICENSE-2.0
#
#    Unless required by applicable law or agreed to in writing, software
#    distributed under the License is distributed on an "AS IS" BASIS,
#    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#    See the License for the specific language governing permissions and
#    limitations under the License.
#==========================================================================


import time
import os
from .naeusb import packuint32
from ...common.utils.util import fw_ver_required
from ...logging import *
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
    USART_CMD_NUMWAIT_TX = 0x0018

    def __init__(self, usb, timeout=200, usart_num=0):
        """
        Set the USB communications instance.
        """
        self._max_read = 256

        self._usb = usb
        self.timeout = timeout

        self._baud = 38400
        self._stopbits = 1
        self._parity = "none"
        
        # could warn users about this,
        # but this obj gets created a lot,
        # and don't want to spam them
        self.tx_buf_in_wait = False
        self.fw_read = None
        self._usart_num = usart_num

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
        target_logger.info("Serial baud rate = {}".format(baud))

        try:
            self.tx_buf_in_wait = self._usb.check_feature("TX_IN_WAITING")
            # self.tx_buf_in_wait = False
            # a = self.fw_version
            # if a["major"] > 0 or a["minor"] >= 20:
            #     self.tx_buf_in_wait = True
        except OSError:
            pass

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

            # need to make sure we don't write too fast
            # and overrun the internal buffer...
            # Can probably elimiate some USB communication
            # to make this faster, but okay for now...
            if self.tx_buf_in_wait:
                datatosend = min(datatosend, 128-self.in_waiting_tx())
            self._usb.sendCtrl(self.CMD_USART0_DATA, (self._usart_num << 8), data[datasent:(datasent + datatosend)])
            datasent += datatosend

        # if self.fw_version_str >= '0.20':
        #     i = 1000
        #     while self.in_waiting_tx() > 0:
        #         # print(self.in_waiting_tx())
        #         i -= 1
        #         if i > 0:
        #             time.sleep(0.001)
        #         else:
        #             target_logger.warning("Write timed out!")
        #             raise OSError("Write timed out")


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

    # @fw_ver_required(0, 20)
    def in_waiting_tx(self):
        """
        Get number of bytes in tx buffer
        """
        if self._usb.check_feature("TX_IN_WAITING"):
            data = self._usartRxCmd(self.USART_CMD_NUMWAIT_TX, dlen=4)
            return data[0]

    def read(self, dlen=0, timeout=0):
        """
        Read data from input buffer, if 'dlen' is 0 everything present is read. If timeout is non-zero
        system will block for a while until data is present in buffer.
        """
        resp = []

        if timeout == 0:
            timeout = self.timeout

        waiting = self.inWaiting()

        if dlen == 0:
            dlen = waiting

        # * 10 does nothing
        while dlen and (timeout) > 0:
            if waiting > 0:
                newdata = self._usb.readCtrl(self.CMD_USART0_DATA, (self._usart_num << 8), min(min(waiting, dlen), self._max_read))
                resp.extend(newdata)
                dlen -= len(newdata)
            waiting = self.inWaiting()
            timeout -= 1
            # time.sleep(0.001)
            if (timeout % 10) == 0:
                time.sleep(0.01)

        return resp


    def _usartTxCmd(self, cmd, data=[]):
        """
        Send a command to the USART interface (internal function).
        """

        # windex selects interface
        self._usb.sendCtrl(self.CMD_USART0_CONFIG, (self._usart_num << 8) | cmd, data)

    def _usartRxCmd(self, cmd, dlen=1):
        """
        Read the result of some command (internal function).
        """
        # windex selects interface, set to 0
        return self._usb.readCtrl(self.CMD_USART0_CONFIG, cmd | (self._usart_num << 8), dlen)

    @property
    def fw_version(self):
        if not self.fw_read:
            self.fw_read = self._usb.readFwVersion()
        return {"major": self.fw_read[0], "minor": self.fw_read[1], "debug": self.fw_read[2]}

    @property
    def fw_version_str(self):
        if not self.fw_read:
            self.fw_read = self._usb.readFwVersion()
        return "{}.{}.{}".format(self.fw_read[0], self.fw_read[1], self.fw_read[2])

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

from _base import SimpleSerialTemplate
from chipwhisperer.hardware.naeusb.serial import USART as CWL_USART
from chipwhisperer.common.utils.parameter import setupSetParam


class SimpleSerial_ChipWhispererLite(SimpleSerialTemplate):
    _name = 'NewAE USB (CWLite/CW1200)'

    def __init__(self):
        SimpleSerialTemplate.__init__(self)
        self._baud = 38400
        self.cwlite_usart = None
        self.params.addChildren([
            {'name':'baud', 'type':'int', 'key':'baud', 'limits':(500, 2000000), 'get':self.baud, 'set':self.setBaud, 'default':38400}
        ])

    @setupSetParam("baud")
    def setBaud(self, baud):
        self._baud = baud
        if self.cwlite_usart:
            self.cwlite_usart.init(baud)
        else:
            logging.error('Baud rate not set, need to connect first')

    def baud(self):
        return self._baud

    def write(self, string):
        # Write to hardware
        self.cwlite_usart.write(string)

        # Update terminal buffer
        for c in string:
            self.terminal_queue.append(['out', c])
            if self.terminal_count < self.max_queue_size:
                self.terminal_count += 1
            else:
                self.terminal_queue.popleft()

    def read(self, num=0, timeout=250):
        # Try to read from queue
        ret = ''
        while num > 0 and self.target_count > 0:
            ret += self.target_queue.popleft()
            self.target_count -= 1
            num -= 1

        if num == 0:
            return ret

        # If we didn't get enough data, try to read more from the hardware
        data = str(bytearray(self.cwlite_usart.read(num, timeout=timeout)))
        for c in data:
            self.terminal_queue.append(['in', c])
            if self.terminal_count < self.max_queue_size:
                self.terminal_count += 1
            else:
                self.terminal_queue.popleft()
        ret += data
        return ret

    def hardware_inWaiting(self):
        bwait = self.cwlite_usart.inWaiting()
        if bwait == 127:
            logging.warning('SAM3U Serial buffers OVERRUN - data loss has occurred.')
        return bwait

    def inWaiting(self):
        bbuf = self.target_count
        if bbuf == self.max_queue_size:
            logging.warning('Python SimpleSerial reader buffer OVERRUN - data loss has occurred.')
        return self.hardware_inWaiting() + bbuf

    def flush(self):
        waiting = self.hardware_inWaiting()
        while waiting > 0:
            self.cwlite_usart.read(waiting)
            waiting = self.hardware_inWaiting()
        self.target_queue.clear()
        self.target_count = 0

    def flushInput(self):
        self.flush()

    def terminal_write(self, string):
        # Write to hardware
        self.cwlite_usart.write(string)

    def terminal_read(self, num=0, timeout=250):
        # Try to read from queue
        ret = []
        while num > 0 and self.terminal_count > 0:
            ret.append(self.terminal_queue.popleft())
            self.terminal_count -= 1
            num -= 1

        if num == 0:
            return ret

        # If we didn't get enough data, try to read more from the hardware
        data = self.cwlite_usart.read(num, timeout=timeout)
        data = bytearray(data)
        data = str(data)
        for c in data:
            self.target_queue.append(str(c))
            if self.target_count < self.max_queue_size:
                self.target_count += 1
            else:
                self.target_count.popleft()
            ret.append(['in', c])
        return ret

    def terminal_flush(self):
        self.terminal_queue.clear()
        self.terminal_count = 0

    def terminal_inWaiting(self):
        bbuf = self.terminal_count
        if bbuf == self.max_queue_size:
            logging.warning('Python SimpleSerial reader buffer OVERRUN - data loss has occurred.')
        return self.hardware_inWaiting() + bbuf

    def close(self):
        pass

    def con(self, scope = None):
        if scope is None or not hasattr(scope, "qtadc"): Warning("You need a scope with OpenADC connected to use this Target")

        scope.connectStatus.connect(self.dis)
        self.cwlite_usart = CWL_USART(scope.qtadc.ser)
        self.cwlite_usart.init(baud=self.findParam('baud').getValue())
        self.params.refreshAllParameters()
        self.connectStatus.setValue(True)


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

from chipwhisperer.common.utils import util
from chipwhisperer.common.utils.pluginmanager import Plugin
from chipwhisperer.common.utils.parameter import Parameterized, Parameter
import collections

class SimpleSerialTemplate(Parameterized, Plugin):

    """
    SimpleSerial serial reader base class.

    All SimpleSerial readers have two data buffers:
    1. A buffer for received data to go to the SimpleSerial target module. This buffer contains bytes.
    2. A buffer for sent and received data to go to the serial terminal. This buffer contains ['in'/'out', char] pairs.
    These two structures are queues with a fixed maximum size: when they overflow, the oldest data must be removed
    from the queues.
    """

    _name= 'Simple Serial Reader'

    def __init__(self):
        self.connectStatus = util.Observable(False)
        self.getParams()

        self.target_queue = collections.deque()
        self.target_count = 0

        self.terminal_queue = collections.deque()
        self.terminal_count = 0

        self.max_queue_size = 100

    def selectionChanged(self):
        pass

    def close(self):
        pass

    def con(self, scope=None):
        """Connect to target"""
        self.connectStatus.setValue(True)

    def dis(self):
        """Disconnect from target"""
        self.close()
        self.connectStatus.setValue(False)

    def flushInput(self):
        pass

    def write(self, string):
        """
        Write a string to the device.

        This involves 2 actions:
        1. Write the string to the hardware;
        2. Insert ['out', b] pairs into the terminal queue

        This should be called by a target module (like SimpleSerial)

        Args:
            string: The string to be written to the device
        Returns:
            None
        """
        pass

    def read(self, num=0, timeout=250):
        """
        Attempt to read a string from the device.

        This involves three steps:
        1. Remove existing data from the target buffer
        2. If needed, request more data from the hardware
        3. If any, add received data to terminal queue in ['in', b] pairs

        This is the interface for target modules - it places the received bytes in a terminal buffer.

        Args:
            num: The number of bytes to be read. If 0, read all data available.
            timeout: How long to wait before returning, in ms. If 0, block until data received.

        Returns:
            String of received data (possibly shorter than num characters)
        """
        return None

    def flush(self):
        """
        Remove any waiting data from the target buffer.

        Returns:
            None
        """
        pass

    def inWaiting(self):
        """
        Check the number of bytes to be read (for target modules).

        This is the total amount of data between the target buffer and the hardware's buffer.

        Returns:
            int: the number of bytes waiting to be read
        """
        pass

    def terminal_write(self, string):
        """
        Write a string to the device.

        No other processing is done here. This is intended for the ChipWhisperer terminal - it doesn't interact with
        the data buffers.

        Args:
            string: The string to be written

        Returns:
            None

        """
        pass

    def terminal_read(self, num=0, timeout=250):
        """
        Attempt to read a bytestring from the device.

        This involves three steps:
        1. Remove existing data from the terminal buffer
        2. If needed, request more data from the hardware
        3. If any, add received data to target queue
        This is the interface for the ChipWhisperer terminal - it places the received bytes in a target buffer.

        Args:
            num: The number of bytes to be read. If 0, read all data available.
            timeout: How long to wait before returning, in ms. If 0, block until data received.

        Returns:
            A list of ['in'/'out', char] pairs
        """
        pass

    def terminal_inWaiting(self):
        """
        Check the number of bytes to be read (for terminal).

        This is the total amount of data between the terminal buffer and the hardware's buffer.

        Returns:
            int: the number of bytes waiting to be read
        """
        pass
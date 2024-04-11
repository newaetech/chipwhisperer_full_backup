#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2013-2021, NewAE Technology Inc
# All rights reserved.
#
# Find this and more at newae.com - this file is part of the chipwhisperer
# project, http://www.assembla.com/spaces/chipwhisperer
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
#=================================================

import collections
import logging
import time

class SimpleSerialTemplate:

    """
    SimpleSerial serial reader base class.

    All SimpleSerial readers have two data buffers:
    1. A buffer for received data to go to the SimpleSerial target module. This buffer contains bytes.
    2. A buffer for sent and received data to go to the serial terminal. This buffer contains ['in'/'out', char] pairs.
    These two structures are queues with a fixed maximum size: when they overflow, the oldest data must be removed
    from the queues.

    Note that child classes should only need to implement the following:
    - hw_read()
    - hw_write()
    - hw_in_waiting()
    """

    _name= 'Simple Serial Reader'

    def __init__(self):
        self.connectStatus = False

        self.target_queue = collections.deque()
        self.target_count = 0

        self.terminal_queue = collections.deque()
        self.terminal_count = 0

        self.max_queue_size = 384

    def selectionChanged(self):
        pass

    def close(self):
        pass

    def con(self, scope=None):
        """Connect to target"""
        self.connectStatus = True

    def dis(self):
        """Disconnect from target"""
        self.close()
        self.connectStatus = False

    def flushInput(self):
        self.flush()

    def write(self, string, timeout=0):
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

        # Write to hardware

        self.hardware_write(string)
        if timeout is None:
            timeout = 10000000000
        else:
            while timeout > 0:
                start = time.perf_counter()
                if self.inWaitingTX() <= 0:
                    return
                end = time.perf_counter()
                timeout -= end - start

        # Update terminal buffer
        for c in string:
            self.terminal_queue.append(['out', c])
            if self.terminal_count < self.max_queue_size:
                self.terminal_count += 1
            else:
                self.terminal_queue.popleft()

    def read(self, num=0, timeout=250):
        """
        Attempt to read a string from the device.

        This involves three steps:
        1. Remove existing data from the target buffer
        2. If needed, request more data from the hardware
        3. If any, add received data to terminal queue in ['in', b] pairs

        This is the interface for target modules - it places the received bytes in a terminal buffer.

        Args:
            num: The number of bytes to be read. If 0, read no data.
            timeout: How long to wait before returning, in ms. If 0, block until data received.

        Returns:
            String of received data (possibly shorter than num characters)
        """

        # Try to read from queue
        ret = ''
        while num > 0 and self.target_count > 0:
            ret += self.target_queue.popleft()
            self.target_count -= 1
            num -= 1

        if num == 0:
            if isinstance(ret, str):
                return ret
            return ret.decode('latin-1')

        # If we didn't get enough data, try to read more from the hardware
        data = bytearray(self.hardware_read(num, timeout=timeout)).decode('latin-1')
        for c in data:
            self.terminal_queue.append(['in', c])
            if self.terminal_count < self.max_queue_size:
                self.terminal_count += 1
            else:
                self.terminal_queue.popleft()
        ret += data
        return ret

    def flush(self):
        """
        Remove any waiting data from the target buffer.

        Returns:
            None
        """
        waiting = self.hardware_inWaiting()
        while waiting > 0:
            self.hardware_read(waiting)
            waiting = self.hardware_inWaiting()
        self.target_queue.clear()
        self.target_count = 0

    def inWaiting(self):
        """
        Check the number of bytes to be read (for target modules).

        This is the total amount of data between the target buffer and the hardware's buffer.

        Returns:
            int: the number of bytes waiting to be read
        """
        bbuf = self.target_count
        if bbuf == self.max_queue_size:
            logging.warning('Python SimpleSerial reader buffer OVERRUN - data loss has occurred.')
        return self.hardware_inWaiting() + bbuf

    def inWaitingTX(self):
        return self.hardware_inWaitingTX()

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
        # Write to hardware
        self.hardware_write(string)

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

        # Try to read from queue
        ret = []
        while num > 0 and self.terminal_count > 0:
            ret.append(self.terminal_queue.popleft())
            self.terminal_count -= 1
            num -= 1

        if num == 0:
            return ret

        # If we didn't get enough data, try to read more from the hardware
        data = self.hardware_read(num, timeout=timeout)
        data = bytearray(data)
        data = str(data)
        for c in data:
            self.target_queue.append(str(c))
            if self.target_count < self.max_queue_size:
                self.target_count += 1
            else:
                self.target_queue.popleft()
            ret.append(['in', c])
        return ret

    def terminal_flush(self):
        """
        Remove all data waiting for the terminal. Don't remove data currently waiting in the hardware.

        Returns:
            None
        """

        self.terminal_queue.clear()
        self.terminal_count = 0

    def terminal_inWaiting(self):
        """
        Check the number of bytes to be read (for terminal).

        This is the total amount of data between the terminal buffer and the hardware's buffer.

        Returns:
            int: the number of bytes waiting to be read
        """

        bbuf = self.terminal_count
        if bbuf == self.max_queue_size:
            logging.warning('Python SimpleSerial reader buffer OVERRUN - data loss has occurred.')
        return self.hardware_inWaiting() + bbuf

    def hardware_inWaiting(self):
        """
        Check how many bytes are in waiting on the device's hardware buffer.

        This function needs to be implemented in child classes.

        Returns:
            int: number of bytes waiting to be read
        """
        raise NotImplementedError

    def hardware_inWaitingTX(self):
        """
        Check how many bytes are in waiting on the device's hardware buffer.

        This function needs to be implemented in child classes.

        Returns:
            int: number of bytes waiting to be read
        """
        raise NotImplementedError

    def hardware_write(self, string):
        """
        Write a string to the target.

        This function needs to be implemented in child classes.

        Returns:
            None
        """
        raise NotImplementedError

    def hardware_read(self, num, timeout=250):
        """
        Read a number of bytes from the hardware.

        This function needs to be implemented in child classes.

        Args:
            num: The number of bytes to be read. If 0, read all data available.
            timeout: How long to wait before returning, in ms. If 0, block until data received.

        Returns:
            String of received data (possibly shorter than num characters)
        """
        raise NotImplementedError

#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2013-2021, NewAE Technology Inc
# All rights reserved.
#
# Find this and more at newae.com - this file is part of the chipwhisperer
# project, https://github.com/newaetech/chipwhisperer
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
import binascii
from ._base import TargetTemplate
from .simpleserial_readers.cwlite import SimpleSerial_ChipWhispererLite
from ...common.utils import util
from collections import OrderedDict
from ...common.utils.util import camel_case_deprecated, dict_to_str
import time

from chipwhisperer.logging import *

class SimpleSerial(TargetTemplate, util.DisableNewAttr):
    """SimpleSerial target object.

    This class contains the public API for a target that uses serial
    communication.

    The easiest way to connect to the target is::

        import chipwhisperer as cw
        scope = cw.scope()
        target = cw.target(scope)

    The target is automatically connected to if the default configuration
    adequate.

    A `noflush=True` kwarg may be used to suppress an initial protocol-specific
    flush of the UART. The caller is then responsible for invoking flush() manually
    to flush underlying buffers.

    For more help use the help() function with one of the submodules
    (target.baud, target.write, target.read, ...).

      * :attr:`target.baud <.SimpleSerial.baud>`
      * :meth:`target.write <.SimpleSerial.write>`
      * :meth:`target.read <.SimpleSerial.read>`
      * :meth:`target.in_waiting <.SimpleSerial.in_waiting>`
      * :meth:`target.in_waiting_tx <.SimpleSerial.in_waiting_tx>`
      * :meth:`target.simpleserial_wait_ack <.SimpleSerial.simpleserial_wait_ack>`
      * :meth:`target.simpleserial_write <.SimpleSerial.simpleserial_write>`
      * :meth:`target.simpleserial_read <.SimpleSerial.simpleserial_read>`
      * :meth:`target.simpleserial_read_witherrors <.SimpleSerial.simpleserial_read_witherrors>`
      * :meth:`target.set_key <.SimpleSerial.set_key>`
      * :meth:`target.close <.SimpleSerial.close>`
      * :meth:`target.con <.SimpleSerial.con>`
      * :meth:`target.get_simpleserial_commands <.SimpleSerial.get_simpleserial_commands>`

    .. warning::
        The CWLite, CW1200, and CWNano have a 128 byte read buffer and a 128
        byte send buffer. If the read buffer overflows, a warning message
        will be printed. Prior to firmware 0.20, the send buffer can silently
        overflow. In ChipWhisperer 5.4, this is upgraded to a 200 byte read/send buffer.
    """
    _name = "Simple Serial"

    def __init__(self):
        TargetTemplate.__init__(self)

        self.ser = SimpleSerial_ChipWhispererLite()

        self._protver = 'auto'
        self.protformat = 'hex'
        self.last_key = bytearray(16)
        self._output_len = 16

        self._proto_ver = "auto"
        self._proto_timeoutms = 20
        self._simpleserial_last_read = ""
        self._simpleserial_last_sent = ""
        self.disable_newattr()

    def __repr__(self):
        ret = "SimpleSerial Settings ="
        for line in dict_to_str(self._dict_repr()).split("\n"):
            ret += "\n\t" + line
        return ret

    def __str__(self):
        return self.__repr__()

    def _dict_repr(self):
        rtn = OrderedDict()
        rtn['output_len'] = self.output_len

        rtn['baud']     = self.baud
        rtn['simpleserial_last_read'] = self.simpleserial_last_read
        rtn['simpleserial_last_sent'] = self.simpleserial_last_sent
        rtn['xonxoff'] = self.xonxoff
        rtn['currently_xoff'] = self.currently_xoff
        #rtn['protver'] = self.protver
        return rtn

    @property
    def simpleserial_last_read(self):
        """The last raw string read by a simpleserial_read* command"""
        return self._simpleserial_last_read

    @property
    def simpleserial_last_sent(self):
        """The last raw string written via simpleserial_write"""
        return self._simpleserial_last_sent

    @property
    def output_len(self):
        """The length of the output expected from the crypto algorithm (in bytes):
        
        :meta private:
        
        """
        return self._output_len

    @output_len.setter
    def output_len(self, length):
        self._output_len = length

    @property
    def baud(self):
        """The current baud rate of the serial connection.

        :Getter: Return the current baud rate.

        :Setter: Set a new baud rate. Valid baud rates are any integer in the
            range [500, 2000000].

        Raises:
            AttributeError: Target doesn't allow baud to be changed.
        """
        if hasattr(self.ser, 'baud') and callable(self.ser.baud):
            return self.ser.baud()
        else:
            raise AttributeError("Can't access baud rate")

    @baud.setter
    def baud(self, new_baud):
        if hasattr(self.ser, 'baud') and callable(self.ser.baud):
            self.ser.setBaud(new_baud)
        else:
            raise AttributeError("Can't access baud rate")

    @property
    def protver(self):
        """Get the protocol version used for the target

        :meta private:
        """
        return self._proto_ver

    @protver.setter
    def protver(self, value):
        """Set the protocol version used for the target ('1.1', '1.0', or 'auto')

        """
        self._proto_ver = value


    def setConnection(self, con):
        """I don't think this does anything
        
        :meta private:

        """
        self.ser = con
        self.ser.connectStatus = self.connectStatus
        self.ser.selectionChanged()

    def _con(self, scope = None, **kwargs):
        if not scope or not hasattr(scope, "qtadc"): Warning("You need a scope with OpenADC connected to use this Target")

        self.ser.con(scope)

        # Check to see if the caller wants to be responsible for flushing the
        # UART on connect. For real world targets, we may just want to quietly
        # open serial port without sending "xxx..." at a potentially incorrect
        # baud rate.
        if kwargs.get('noflush', False) == False:
            # 'x' flushes everything & sets system back to idle
            self.ser.write("xxxxxxxxxxxxxxxxxxxxxxxx")
            self.ser.flush()

    def dis(self):
        self.close()

    def close(self):
        if self.ser != None:
            self.ser.close()

    def init(self):
        self.ser.flush()

    def is_done(self):
        """Always returns True"""
        return True

    def get_simpleserial_commands(self, timeout=250, ack=True):
        """Gets available simpleserial commands for target

        Args:
            timeout (int, optional): Value to use for timeouts during initial
                read of expected data in ms. If 0, block until all expected
                data is returned. Defaults to 250.
            ack (bool, optional): Wait for ack after sending key. Defaults to
                True.

        Returns:
            List of dics with fields 'cmd' command_byte, 'len' command_length, 'flags' command_flags
        """
        self.flush()
        self.simpleserial_write('y', bytearray())
        num_commands = self.simpleserial_read('r', 1, timeout=timeout, ack=ack)
        self.simpleserial_write('w', bytearray())

        cmd_packet = self.simpleserial_read('r', num_commands[0]*3, timeout=timeout, ack=ack)
        command_list = []
        for i in range(num_commands[0]):
            command_list.append({"cmd": bytes([cmd_packet[3*i]]), "len": cmd_packet[1+3*i], "flags": cmd_packet[2+3*i]})

        return command_list




    def write(self, data, timeout=0):
        """ Writes data to the target over serial.

        Args:
            data (str): Data to write over serial.
            timeout (float or None): Wait <timeout> seconds for write buffer to clear.
                If None, block for a long time. If 0, return immediately. Defaults to 0.

        Raises:
            Warning: Target not connected

        .. versionadded:: 5.1
            Added target.write()
        """
        if type(data) is list:
            data = bytearray(data)
        if not self.connectStatus:
            raise Warning("Target not connected")

        try:
            self.ser.write(data, timeout)
                    
        except Exception as e:
            self.dis()
            raise e

    def read(self, num_char = 0, timeout = 250):
        """ Reads data from the target over serial.

        Args:
            num_char (int, optional): Number of byte to read. If 0, read all
                data available. Defaults to 0.
            timeout (int, optional): How long in ms to wait before returning.
                If 0, block for a long time. Defaults to 250.

        Returns:
            String of received data.

        .. versionadded:: 5.1
            Added target.read()
        """
        if not self.connectStatus:
            raise Warning("Target not connected")
        if timeout == 0:
            timeout = 10000000000
        try:
            if num_char == 0:
                num_char = self.ser.inWaiting()
            return self.ser.read(num_char, timeout)
        except Exception as e:
            self.dis()
            raise e

    def simpleserial_wait_ack(self, timeout=500):
        """Waits for an ack from the target for timeout ms

        Args:
            timeout (int, optional): Time to wait for an ack in ms. If 0, block
                until we get an ack. Defaults to 500.

        Returns:
            The return code from the ChipWhisperer command or None if the target
            failed to ack

        Raises:
            Warning: Target not connected.

        .. versionadded:: 5.1
            Added target.simpleserial_wait_ack

        .. versionadded:: 5.2
            Defined return value
        """

        data = self.read(4, timeout = timeout)
        if len(data) < 4:
            target_logger.error("Target did not ack")
            return None
        if data[0] != 'z':
            target_logger.error("Ack error: {}".format(data))
            return None
        ret = None
        try:
            ret = int(data[1:3], 16)
        except ValueError:
            target_logger.error("Ack error, couldn't decode return {}".format(data))
            return None
        return ret

    def simpleserial_write(self, cmd, num, end='\n'):
        """ Writes a simpleserial command to the target over serial.

        Writes 'cmd' + ascii(num) + 'end' over serial. Flushes the read and
        write buffers before writing.

        Args:
            cmd (str): String to start the simpleserial command with. For
                'p'.
            num (bytearray): Number to write as part of command. For example,
                the 16 byte plaintext for the 'p' command. Converted to hex-ascii
                before being sent. If set to 'none' is omitted.
            end (str, optional): String to end the simpleserial command with.
                Defaults to '\\n'.

        Example:
            Sending a 'p' command::

                key, pt = ktp.new_pair()
                target.simpleserial_write('p', pt)

        Raises:
            Warning: Write attempted while disconnected or error during write.

        .. versionadded:: 5.1
            Added target.simpleserial_write()
        """
        if type(num) is list:
            num = bytearray(num)
        self.ser.flush()
        if cmd:
            cmd += binascii.hexlify(num).decode()
        cmd += end
        target_logger.debug("Sending: {}".format(cmd))
        self.write(cmd)
        self._simpleserial_last_sent = cmd

    def simpleserial_read(self, cmd, pay_len, end='\n', timeout=250, ack=True):
        r""" Reads a simpleserial command from the target over serial.

        Reads a command starting with <start> with an ASCII encoded bytearray
        payload of length exp_len*2 (i.e. exp_len=16 for an AES128 key) and
        ending with <end>. Converts the payload to a bytearray. Will ignore
        non-ASCII bytes in the payload, but warn the user of them.

        Args:
            cmd (str): Expected start of the command. Will warn the user if
                the received command does not start with this string.
            pay_len (int): Expected length of the returned bytearray in number
                of bytes. Note that SimpleSerial commands send data as ASCII;
                this is the length of the data that was encoded.
            end (str, optional): Expected end of the command. Will warn the
                user if the received command does not end with this string.
                Defaults to '\n'
            timeout (int, optional): Value to use for timeouts during reads in
                ms. If 0, block until all expected data is returned. Defaults
                to 250.
            ack (bool, optional): Expect an ack at the end for SimpleSerial
                >= 1.1. Defaults to True.

        Returns:
            The received payload as a bytearray or None if the read failed.

        Example:
            Reading ciphertext back from the target after a 'p' command::

                ct = target.simpleserial_read('r', 16)

        Raises:
            Warning: Device did not ack or error during read.

        .. versionadded:: 5.1
            Added target.simpleserial_read()
        """
        cmd_len = len(cmd)
        ascii_len = pay_len * 2
        recv_len = cmd_len + ascii_len + len(end)
        response = self.read(recv_len, timeout=timeout)
        self._simpleserial_last_read = response

        payload = bytearray(pay_len)
        if cmd_len > 0:
            if response[0:cmd_len] != cmd:
                target_logger.warning("Unexpected start to command: {}".format(
                    response[0:cmd_len]
                ))
                return None
        idx = cmd_len
        for i in range(0, pay_len):
            try:
                payload[i] = int(response[idx:(idx + 2)], 16)
            except ValueError as e:
                target_logger.warning("ValueError: {}".format(e))
            idx += 2

        if len(end) > 0:
            if response[(idx):(idx + len(end))] != end:
                target_logger.warning("Unexpected end to command: {}".format(
                    response[(idx):(idx+len(end))]))
                return None

        if ack:
            if self.simpleserial_wait_ack(timeout) is None:
                raise Warning("Device failed to ack")

        return payload

    def simpleserial_read_witherrors(self, cmd, pay_len, end="\n", timeout=250, glitch_timeout=8000, ack=True):
        r""" Reads a simpleserial command from the target over serial, but returns invalid responses.

        Reads a command starting with <start> with an ASCII encoded bytearray
        payload of length exp_len*2 (i.e. exp_len=16 for an AES128 key) and
        ending with <end>. Converts the payload to a bytearray, returns a dictionary
        showing if processing was successful along with decoded and raw values.
        This function is designed to be used with glitching where you may have
        invalid responses.

        Args:
            cmd (str): Expected start of the command. Will warn the user if
                the received command does not start with this string.
            pay_len (int): Expected length of the returned bytearray in number
                of bytes. Note that SimpleSerial commands send data as ASCII;
                this is the length of the data that was encoded.
            end (str, optional): Expected end of the command. Will warn the
                user if the received command does not end with this string.
                Defaults to '\n'
            timeout (int, optional): Value to use for timeouts during initial
                read of expected data in ms. If 0, block until all expected
                data is returned. Defaults to 250.
            glitch_timeout (int, optional): Value to wait for additional data
                if expected data isn't returned. Useful to have a longer
                timeout for a reset or other unexpected event.
            ack (bool, optional): Expect an ack at the end for SimpleSerial
                >= 1.1. Defaults to True.

        Returns:
            A dictionary with these elements:
                valid (bool): Did response look valid?
                payload: Bytearray of decoded data (only if valid is 'True', otherwise None)
                full_response: Raw output of serial port.
                rv: If 'ack' in command, includes return value

        Example:
            Reading the output of one of the glitch tests when no error:
                resp = target.simpleserial_read_witherrors('r', 4)
                print(resp)
                >{'valid': True, 'payload': CWbytearray(b'c4 09 00 00'), 'full_response': 'rC4090000\n', 'rv': 0}

            Reading the output of one of the glitch tests when an error happened:
                resp = target.simpleserial_read_witherrors('r', 4)
                print(resp)
                >{'valid': False, 'payload': None, 'full_response': '\x00\x00\x00\x00\x00\x00\x00rRESET   \n', 'rv': None}

        Raises:
            Warning: Device did not ack or error during read.

        .. versionadded:: 5.2
            Added target.simpleserial_read_witherrors()
        """

        cmd_len = len(cmd)
        ascii_len = pay_len * 2
        recv_len = cmd_len + ascii_len + len(end)
        response = self.read(recv_len, timeout=timeout)

        payload = bytearray(pay_len)
        valid = False
        rv = None

        if len(response) != recv_len or response[0:cmd_len] != cmd:
            # Switch to robust mode - likely a glitch happened. Get all response first...
            response += self.read(1000, timeout=glitch_timeout)
            payload = None
        else:
            valid = True
            idx = cmd_len
            for i in range(0, pay_len):
                try:
                    payload[i] = int(response[idx:(idx + 2)], 16)
                except ValueError as e:
                    payload = None
                    valid = False
                    break
                    #target_logger.warning("ValueError: {}".format(e))

                idx += 2

            if valid and (len(end) > 0):
                if response[(idx):(idx + len(end))] != end:
                    target_logger.warning("Unexpected end to command: {}".format(
                        response[(idx):(idx + len(end))]))
                    payload = None

            if ack and valid:
                rv = self.simpleserial_wait_ack(timeout)

        # return payload, valid, response
        self._simpleserial_last_read = response
        return {'valid': valid, 'payload': payload, 'full_response': response, 'rv': rv}

    def set_key(self, key, ack=True, timeout=250, always_send=False):
        """Checks if key is different than the last one sent. If so, send it.

        Uses simpleserial_write('k')

        Args:
            key (bytearray): key to send
            ack (bool, optional): Wait for ack after sending key. Defaults to
                True.
            timeout (int, optional): How long in ms to wait for the ack.
                Defaults to 250.

        Raises:
            Warning: Device did not ack or error during read.

        .. versionadded:: 5.1
            Added target.set_key()
        """
        if (self.last_key != key) or always_send:
            self.last_key = key
            self.simpleserial_write('k', key)
            if ack:
                if self.simpleserial_wait_ack(timeout) is None:
                    raise Warning("Device failed to ack")
        else:
            target_logger.debug("Key unchanged, skipping send")

    def in_waiting(self):
        """Returns the number of characters available from the serial buffer.

        Returns:
            The number of characters available via a target.read() call.

        .. versionadded:: 5.1
            Added target.in_waiting()
        """
        return self.ser.inWaiting()

    inWaiting = camel_case_deprecated(in_waiting)

    def flush(self):
        """Removes all data from the serial buffer.

        .. versionadded:: 5.1
            Added target.flush()
        """
        self.ser.flush()

    def in_waiting_tx(self):
        """Returns the number of characters waiting to be sent by the ChipWhisperer.

        Requires firmware version >= 0.2 for the CWLite/Nano and firmware version and
        firmware version >= 1.2 for the CWPro.

        Used internally to avoid overflowing the TX buffer, since CW version 5.3

        Returns:
            The number of characters waiting to be sent to the target

        .. versionadded:: 5.3.1
            Added public method for in_waiting_tx().
        """
        return self.ser.inWaitingTX()

    @property
    def xonxoff(self):
        # NOTE: Firmware version checked in lower serial driver
        return self.ser.xonxoff
    
    @xonxoff.setter
    def xonxoff(self, enable):
        self.ser.xonxoff = enable

    @property
    def currently_xoff(self):
        return self.ser.currently_xoff
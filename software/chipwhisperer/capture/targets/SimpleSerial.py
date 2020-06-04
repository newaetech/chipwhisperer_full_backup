#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2013-2020, NewAE Technology Inc
# All rights reserved.
#
# Find this and more at newae.com - this file is part of the chipwhisperer
# project, https://github.com/newaetech/chipwhisperer
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

import random
from usb import USBError

import binascii
from ._base import TargetTemplate
from .simpleserial_readers.cwlite import SimpleSerial_ChipWhispererLite
from chipwhisperer.common.utils import util
from collections import OrderedDict
from chipwhisperer.common.utils.util import camel_case_deprecated, dict_to_str


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

    For more help use the help() function with one of the submodules
    (target.baud, target.write, target.read, ...).

      * :attr:`target.baud <.SimpleSerial.baud>`
      * :meth:`target.write <.SimpleSerial.write>`
      * :meth:`target.read <.SimpleSerial.read>`
      * :meth:`target.simpleserial_wait_ack <.SimpleSerial.simpleserial_wait_ack>`
      * :meth:`target.simpleserial_write <.SimpleSerial.simpleserial_write>`
      * :meth:`target.simpleserial_read <.SimpleSerial.simpleserial_read>`
      * :meth:`target.simpleserial_read_witherrors <.SimpleSerial.simpleserial_read_witherrors>`
      * :meth:`target.set_key <.SimpleSerial.set_key>`
      * :meth:`target.close <.SimpleSerial.close>`
      * :meth:`target.con <.SimpleSerial.con>`

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
        dict = OrderedDict()
        dict['output_len'] = self.output_len

        dict['baud']     = self.baud
        dict['simpleserial_last_read'] = self.simpleserial_last_read
        dict['simpleserial_last_sent'] = self.simpleserial_last_sent
        #dict['protver'] = self.protver
        return dict

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
        """The length of the output expected from the crypto algorithm (in bytes)"""
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
        """
        return self._proto_ver

    @protver.setter
    def protver(self, value):
        """Set the protocol version used for the target ('1.1', '1.0', or 'auto')
        """
        self._proto_ver = value


    def setConnection(self, con):
        """I don't think this does anything"""
        self.ser = con
        self.ser.connectStatus = self.connectStatus
        self.ser.selectionChanged()

    def _con(self, scope = None):
        if not scope or not hasattr(scope, "qtadc"): Warning("You need a scope with OpenADC connected to use this Target")

        self.ser.con(scope)
        # 'x' flushes everything & sets system back to idle
        self.ser.write("xxxxxxxxxxxxxxxxxxxxxxxx")
        self.ser.flush()


    def close(self):
        if self.ser != None:
            self.ser.close()

    def init(self):
        self.ser.flush()

    def is_done(self):
        """Always returns True"""
        return True



    def write(self, data):
        """ Writes data to the target over serial.

        Args:
            data (str): Data to write over serial.

        Raises:
            Warning: Target not connected

        .. versionadded:: 5.1
            Added target.write()
        """
        if not self.connectStatus:
            raise Warning("Target not connected")

        try:
            self.ser.write(data)
        except USBError:
            self.dis()
            raise Warning("Error in target. It may have been disconnected")
        except Exception as e:
            self.dis()
            raise e

    def read(self, num_char = 0, timeout = 250):
        """ Reads data from the target over serial.

        Args:
            num_char (int, optional): Number of byte to read. If 0, read all
                data available. Defaults to 0.
            timeout (int, optional): How long in ms to wait before returning.
                If 0, block until data received. Defaults to 250.

        Returns:
            String of received data.

        .. versionadded:: 5.1
            Added target.read()
        """
        if not self.connectStatus:
            raise Warning("Target not connected")
        try:
            if num_char == 0:
                num_char = self.ser.inWaiting()
            return self.ser.read(num_char, timeout)
        except USBError:
            self.dis()
            raise Warning("Error in target. It may have been disconnected")
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
            logging.error("Target did not ack")
            return None
        if data[0] != 'z':
            logging.error("Ack error: {}".format(data))
            return None
        return int(data[1:3], 16)

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
        self.ser.flush()
        if cmd:
            cmd += binascii.hexlify(num).decode()
        cmd += end
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
                logging.warning("Unexpected start to command: {}".format(
                    response[0:cmd_len]
                ))
                return None
        idx = cmd_len
        for i in range(0, pay_len):
            try:
                payload[i] = int(response[idx:(idx + 2)], 16)
            except ValueError as e:
                logging.warning("ValueError: {}".format(e))
            idx += 2

        if len(end) > 0:
            if response[(idx):(idx + len(end))] != end:
                logging.warning("Unexpected end to command: {}".format(
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
                    logging.warning("ValueError: {}".format(e))

                idx += 2

            if len(end) > 0:
                if response[(idx):(idx + len(end))] != end:
                    logging.warning("Unexpected end to command: {}".format(
                        response[(idx):(idx + len(end))]))
                    payload = None

            if ack:
                rv = self.simpleserial_wait_ack(timeout)

        # return payload, valid, response
        self._simpleserial_last_read = response
        return {'valid': valid, 'payload': payload, 'full_response': response, 'rv': rv}

    def set_key(self, key, ack=True, timeout=250):
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
        if self.last_key != key:
            self.last_key = key
            self.simpleserial_write('k', key)
            if ack:
                if self.simpleserial_wait_ack(timeout) is None:
                    raise Warning("Device failed to ack")

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




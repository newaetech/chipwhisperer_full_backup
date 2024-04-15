#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2020-2021, NewAE Technology Inc
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

import time


from ._base import TargetTemplate
from .simpleserial_readers.cwlite import SimpleSerial_ChipWhispererLite

from ...logging import *
from ...common.utils import util
class SimpleSerial2_Err:
    OK = 0
    ERR_CMD = 1
    ERR_CRC = 2
    ERR_TIMEOUT = 3
    ERR_LEN = 4
    ERR_FRAME_BYTE = 5

bytearray = util.CWByteArray # type: ignore


class SimpleSerial2(TargetTemplate):
    """Target object for new SimpleSerial V2 protocol.

    .. versionadded:: 5.4
        Added SimpleSerial V2 (not on by default)

    Currently, the easiest way to use it is::

        scope = cw.scope()
        target = cw.target(scope, cw.targets.SimpleSerial2, flush_on_err=False)

    For more help, use the help() function with one of the submodules
    (target.baud, target.write, target.read, ...).

      * :attr:`target.baud <.SimpleSerial2.baud>`
      * :meth:`target.write <.SimpleSerial2.write>`
      * :meth:`target.read <.SimpleSerial2.read>`
      * :meth:`target.in_waiting <.SimpleSerial2.in_waiting>`
      * :meth:`target.in_waiting_tx <.SimpleSerial2.in_waiting_tx>`
      * :meth:`target.send_cmd <.SimpleSerial2.send_cmd>`
      * :meth:`target.read_cmd <.SimpleSerial2.read_cmd>`
      * :meth:`target.simpleserial_wait_ack <.SimpleSerial2.simpleserial_wait_ack>`
      * :meth:`target.simpleserial_write <.SimpleSerial2.simpleserial_write>`
      * :meth:`target.simpleserial_read <.SimpleSerial2.simpleserial_read>`
      * :meth:`target.simpleserial_read_witherrors <.SimpleSerial2.simpleserial_read_witherrors>`
      * :meth:`target.set_key <.SimpleSerial2.set_key>`
      * :meth:`target.close <.SimpleSerial2.close>`
      * :meth:`target.con <.SimpleSerial2.con>`
      * :meth:`target.get_simpleserial_commands <.SimpleSerial2.get_simpleserial_commands>`

    The protocol is as follows:

    [cmd, subcmd, data_len, data_0, ..., data_n, crc (poly=0xA6)]

    The frame is then consistant overhead byte stuffed (COBS) to remove
    all 0x00 bytes. An 0x00 byte is then appended to the end of the frame.

    Uses 230400bps by default.

    Return packets have the form:

    [cmd, data_len, data_0, ..., data_n, crc (poly=0xA6)]

    All commands sent to the target will be responded to with an
    ack/error packet.

    Allows us to send more information in the same number of bytes. Also
    should be more robust and a bit easier to work with:

    1. It's easy to reset communications by sending frame bytes (0x00)
    2. We have many ways of checking the validity of a packet:
        -Frame byte where it shouldn't be
        -No frame byte at end of message
        -CRC
    3. No need to specify length of return message
    """
    _frame_byte = 0x00
    def __init__(self):
        TargetTemplate.__init__(self)
        self.ser = SimpleSerial_ChipWhispererLite()
        self.ser._baud = 230400
        self._protver = 'auto'
        self.protformat = 'hex'
        self.last_key = bytearray(16)
        self._output_len = 16

    def close(self):
        self.ser.close()

    def dis(self):
        self.close()

    @staticmethod
    def strerror(e):
        """Get string error message based on integer error e
        """
        if e == SimpleSerial2_Err.OK:
            return "No error"
        if e == SimpleSerial2_Err.ERR_CMD:
            return "Invalid comand"
        if e == SimpleSerial2_Err.ERR_CRC:
            return "Bad CRC"
        if e == SimpleSerial2_Err.ERR_TIMEOUT:
            return "Read timed out"
        if e == SimpleSerial2_Err.ERR_LEN:
            return "Invalid frame length"
        if e == SimpleSerial2_Err.ERR_FRAME_BYTE:
            return "Frame byte in expected spot"
        return "Unknown error"

    @staticmethod
    def _calc_crc(buf):
        """Calculate CRC (0xA6) for buf
        """
        crc = 0x00
        try:
            for b in buf:
                crc ^= b
                for _ in range(8):
                    if crc & 0x80:
                        crc = (crc << 1) ^ 0x4D
                        crc &= 0xFF
                    else:
                        crc <<= 1
                        crc &= 0xFF
        except:
            target_logger.error("crc error: {}. Try rebuilding firmware if you only get this error.".format(buf))
        return crc


    def _stuff_data(self, buf):
        """Apply COBS to buf
        """
        l = len(buf)
        ptr = 0
        last = 0
        for i in range(1, l):
            if (buf[i] == self._frame_byte):
                buf[last] = i - last
                last = i
                target_logger.debug("Stuffing byte {}".format(i))
        return buf

    def _unstuff_data(self, buf):
        """Removes COBS from buf

        Can currently get into an infinite loop, don't know why
        """
        if len(buf) == 0:
            return 0x00
        n = buf[0]
        buf[0] = 0
        l = len(buf) - 1
        sentinel = 0
        while n < l:
            target_logger.debug("Unstuff position {}".format(n))
            tmp = buf[n]
            buf[n] = self._frame_byte
            n += tmp
            if (n == 0) and (tmp == 0):
                target_logger.error("Infinite loop in unstuff data")
                target_logger.error(buf)
                return
            sentinel += 1
            if sentinel > len(buf):
                target_logger.error(f"{buf}, {n}, {tmp}")
                return
        if n > l:
            return n
        return 0x00

    def con(self, scope=None, flush_on_err=True, **kwargs):
        self.ser.con(scope)
        self._flush_on_err = flush_on_err
        self.reset_comms()
        #self.baud = 230400
        self.flush()

    def simpleserial_write(self, cmd, data, end='\n'):
        """Mimic SimpleSerial v1 behaviour with new firmware

        Internal/firmware details may change

        Special cases for 'p' and 'k' to use sub command
        to select what to do with simpleserial-aes

        Args:
            cmd (str): If 'p', SSV2 cmd=0x01, scmd=0x01. If 'k', SSV2 cmd=0x01, scmd=0x02.
                        Otherwise, SSV2 cmd=cmd, scmd=0x00.
            data (bytearray): Number to write as part of command. For example,
                the 16 byte plaintext for the 'p' command. If an empty list is given,
                [0x00] is sent.
            end (str, optional): Unused

        Example:
            Sending a 'p' command::

                key, pt = ktp.new_pair()
                target.simpleserial_write('p', pt)
        """
        if cmd == 'p':
            self.send_cmd(0x01, 0x01, data)
        elif cmd == 'k':
            self.send_cmd(0x01, 0x02, data)
        else:
            self.send_cmd(cmd, 0x00, data)

    def simpleserial_read(self, cmd=None, pay_len=None, end='\n', timeout=250, ack=True):
        """Reads back response from target and ack packet.

        Args:
            cmd (str, optional): Expected start of the command. Will warn the user if
                the received command does not start with this string. Defaults to None
            pay_len (int, optional): Expected length of the returned bytearray in number
                of bytes. If None, uses the length field of the packet to
                determine how much data to read
            end (str, optional): Unused
            timeout (int, optional): Value to use for timeouts during initial
                read of expected data in ms. If 0, block until all expected
                data is returned. Defaults to 250.
            ack (bool, optional): Expect an ack packet at the end for SimpleSerial
                2. Defaults to True.
        """
        rtn = self.read_cmd(cmd, pay_len, timeout)
        if not rtn:
            return None
        else:
            rtn = rtn[3:-2]
        try:
            if ack:
                self.simpleserial_wait_ack(timeout)
        except:
            pass
        return bytearray(rtn)

    def is_done(self):
        """Required on other platforms
        """
        return True

    def flush_on_error(self):
        """Internally used to reset comms and flush serial buffer when we get an error.
        """
        if self._flush_on_err:
            self.reset_comms()
            time.sleep(0.15)
            self.flush()

    def simpleserial_wait_ack(self, timeout=500):
        """Waits for an ack/error packet from the target for timeout ms

        Args:
            timeout (int, optional): Time to wait for an ack in ms. If 0, block
                until we get an ack. Defaults to 500.

        Returns:
            The return code from the ChipWhisperer command or None if the target
            failed to ack
        """
        rtn = self.read_cmd('e')
        if not rtn:
            target_logger.error("Device did not ack")
            return
        if rtn[3] != 0x00:
            target_logger.error(f"Device reported error {hex(rtn[3])}")
            self.flush_on_error()
            target_logger.error(bytearray(rtn))
        return rtn[3:-2]


    # very ugly since we're decoding stuff as we read it back
    # need to decode bytearray to give raw serial back
    # TODO: Improve this
    def simpleserial_read_witherrors(self, cmd=None, pay_len=None, end='\n',\
                                    timeout=250, glitch_timeout=1000, ack=True):
        r""" Reads a simpleserial command from the target over serial, but returns invalid responses.

        Reads a command starting with <start> with a COBS encoded bytearray
        payload of length pay_len (i.e. pay_len=16 for an AES128 key) and
        ending with 0x00. Does normal read_cmd() stuff (decoding, etc).
        If an error is found (timeout, frame issues, etc), a single
        read of 1000 character with glitch_timeout as a timeout is done, and the raw
        response is returned.

        The packet will be valid if:

            * All requested reads return the requested characters
            * No frame bytes except the terminator are read
            * The packet doesn't end with a frame byte
            * If an ack packet isn't received

        Args:
            cmd (str, optional): Expected start of the command. Will warn the user if
                the received command does not start with this string. Defaults to None
            pay_len (int, optional): Expected length of the returned bytearray in number
                of bytes. If None, uses the length field of the packet to
                determine how much data to read
            end (str, optional): Unused
            timeout (int, optional): Value to use for timeouts during initial
                read of expected data in ms. If 0, block until all expected
                data is returned. Defaults to 250.
            glitch_timeout (int, optional): Value to wait for additional data
                if expected data isn't returned. Useful to have a longer
                timeout for a reset or other unexpected event. Defaults to 1000
            ack (bool, optional): Expect an ack packet at the end for SimpleSerial
                >= 2. Defaults to True.

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
        """

        if isinstance(cmd, str):
            cmd = ord(cmd[0])
        if pay_len is None:
            recv_len = 3
        else:
            recv_len = 5 + pay_len #cmd, len, data, crc
        response = self.read(recv_len, timeout=timeout)


        if response is None or len(response) < recv_len:
            # got nothing or less than requested back
            response += self.read(1000, timeout=glitch_timeout)
            return {'valid': False, 'payload': None, 'full_response': response, 'rv': None}

        response = bytearray(response.encode('latin-1'))
        if self._frame_byte in response and len(response) == 3:
            # invalid response
            response = response.decode('latin-1')
            response += self.read(1000, timeout=glitch_timeout)
            return {'valid': False, 'payload': None, 'full_response': response, 'rv': None}


        next_frame = self._unstuff_data(response)
        if cmd and response[1] != cmd:
            target_logger.warning(f"Unexpected start to command {response[1]}")

        l = response[2]

        if not pay_len:
            # user didn't specify, do second read based on sent length
            x = self.read(l+2, timeout=timeout)
            if x is None:
                target_logger.warning("Read timed out")
                response = response.decode('latin-1')
                response += self.read(1000, timeout=glitch_timeout)
                return {'valid': False, 'payload': None, 'full_response': response, 'rv': None}
            if len(x) != (l + 2):
                target_logger.warning(f"Didn't get all data {len(x)}, {l+2}")
                target_logger.warning(bytearray(x.encode('latin-1')))
                target_logger.warning(response)
                response = response.decode('latin-1')
                response += self.read(1000, timeout=glitch_timeout)
                return {'valid': False, 'payload': None, 'full_response': response, 'rv': None}

            response.extend(bytearray(x.encode('latin-1')))
            pay_len = len(response) - 5

            # need to do second unstuff since we read stuff after last one
            if self._frame_byte in response[3:-1]:
                #target_logger.warning(f"Unexpected frame byte in {response}")
                response = response.decode('latin-1')
                response += self.read(1000, timeout=glitch_timeout)
                return {'valid': False, 'payload': None, 'full_response': response, 'rv': None}
            resp_cpy = response[next_frame:]
            self._unstuff_data(resp_cpy)
            response[next_frame:] = resp_cpy[:]
        if pay_len and l != pay_len:
            target_logger.warning(f"Unexpected length {l}, {pay_len}")
            response = response.decode('latin-1')
            response += self.read(1000, timeout=glitch_timeout)
            return {'valid': False, 'payload': None, 'full_response': response, 'rv': None}

        crc = self._calc_crc(response[1:-2]) #calc crc for all bytes except last (crc)
        if crc != response[-2]:
            target_logger.warning(f"Invalid CRC. Expected {crc} got {response[-2]}")
            response = response.decode('latin-1')
            response += self.read(1000, timeout=glitch_timeout)
            return {'valid': False, 'payload': None, 'full_response': response, 'rv': None}

        if response[-1] != self._frame_byte:
            target_logger.warning(f"Did not receive end of frame, got {response[-1]}")
            response = response.decode('latin-1')
            response += self.read(1000, timeout=glitch_timeout)
            return {'valid': False, 'payload': None, 'full_response': response, 'rv': None}

        try:
            rv = self.simpleserial_wait_ack()
            if rv is None:
                response = response.decode('latin-1')
                response += self.read(1000, timeout=glitch_timeout)
                return {'valid': False, 'payload': None, 'full_response': response, 'rv': None}
        except:
            response = response.decode('latin-1')
            response += self.read(1000, timeout=glitch_timeout)
            return {'valid': False, 'payload': None, 'full_response': response, 'rv': None}

        return {'valid': True, 'payload': bytearray(response[3:-2]), 'full_response': response, 'rv': rv}

    def get_simpleserial_commands(self, timeout=250, flush_on_err=None, ack=True):
        """Gets available simpleserial commands for target

        Args:
            timeout (int, optional): Value to use for timeouts during initial
                read of expected data in ms. If 0, block until all expected
                data is returned. Defaults to 250.
            flush_on_err (bool/None, optional): If True, reset/flush the serial lines.
                If False, don't. If None, determine via whether or not flush_on_err
                was True or False when passed to con()
            ack (bool, optional): Wait for ack after sending key. Defaults to
                True.

        Returns:
            List of dics with fields 'cmd' command_byte, 'len' 0x00, 'flags' 0x00
        """
        self.flush()
        self.simpleserial_write('w', [])
        cmd_packet = self.read_cmd('r', None, timeout, flush_on_err)
        if ack:
            self.read_cmd('e')

        num_commands = cmd_packet[2]
        command_list = []
        for i in range(num_commands):
            command_list.append({"cmd": bytes([cmd_packet[3+i]]), "len": 0x00, "flags": 0x00})

        return command_list


    def read_cmd(self, cmd=None, pay_len=None, timeout=250, flush_on_err=None):
        """Read and decode simpleserial-v2 command

        Args:
            cmd (str, optional): Expected start of the command. Will warn the user if
                the received command does not start with this string. Defaults to None
            pay_len (int, optional): Expected length of the returned bytearray in number
                of bytes. If None, uses the length field of the packet to
                determine how much data to read
            timeout (int, optional): Value to use for timeouts during initial
                read of expected data in ms. If 0, block until all expected
                data is returned. Defaults to 250.
            flush_on_err (bool/None, optional): If True, reset/flush the serial lines.
                If False, don't. If None, determine via whether or not flush_on_err
                was True or False when passed to con()
        """
        tmp = None
        if not flush_on_err is None:
            tmp = self._flush_on_err
            self._flush_on_err = flush_on_err
        if isinstance(cmd, str):
            cmd = ord(cmd[0])
        if pay_len is None:
            recv_len = 3
        else:
            recv_len = 5 + pay_len #cmd, len, data, crc
        response = self.read(recv_len, timeout=timeout)
        target_logger.debug("1st read: {}".format(bytearray(response.encode())))

        if response is None or len(response) < recv_len:
            self.flush_on_error()
            target_logger.warning("Read timed out: " + response)
            return None

        response = bytearray(response.encode('latin-1'))
        if (self._frame_byte in response and len(response) == 3) or \
            (self._frame_byte in response[:-1] and len(response) != 3):
            target_logger.warning(f"Unexpected frame byte in {response}")
            self.flush_on_error()
            return None
        next_frame = self._unstuff_data(response)
        target_logger.debug("Unstuffed first read: {}".format(next_frame))
        if cmd and response[1] != cmd:
            target_logger.warning(f"Unexpected start to command {response[1]}")

        l = response[2]

        if not pay_len:
            # user didn't specify, do second read based on sent length
            target_logger.debug("Length not specified, reading {} bytes (plus CRC and frame byte) based on packet".format(l))
            x = self.read(l+2, timeout=timeout)
            target_logger.debug("2nd read: {}".format(bytearray(x.encode())))
            if x is None:
                target_logger.warning("Read timed out")
                self.flush_on_error()
                return None
            if len(x) != (l + 2):
                target_logger.warning(f"Didn't get all data {len(x)}, {l+2}")
                target_logger.warning(bytearray(x.encode('latin-1')))
                target_logger.warning(response)
            response.extend(bytearray(x.encode('latin-1')))
            pay_len = len(response) - 5

            # need to do second unstuff since we read stuff after last one
            if self._frame_byte in response[3:-1]:
                target_logger.warning(f"Unexpected frame byte in {response}")
                self.flush_on_error()
            resp_cpy = response[next_frame:]
            target_logger.debug("Unstuffing {}".format(resp_cpy))
            self._unstuff_data(resp_cpy)
            response[next_frame:] = resp_cpy[:]
        if pay_len and l != pay_len:
            target_logger.warning(f"Unexpected length {l}, {pay_len}")
            self.flush_on_error()
            return None

        crc = self._calc_crc(response[1:-2]) #calc crc for all bytes except last (crc)
        if crc != response[-2]:
            target_logger.warning(f"Invalid CRC. Expected {crc} got {response[-2]}")

        if response[-1] != self._frame_byte:
            target_logger.warning(f"Did not receive end of frame, got {response[-1]}")

        target_logger.debug("Correct CRC {}".format(crc))

        if not flush_on_err is None:
            self._flush_on_err = tmp

        target_logger.info("Received: {}".format(response))

        return response

    def read(self, num_char = 0, timeout = 250):
        """ Reads data from the target over serial.

        Args:
            num_char (int, optional): Number of byte to read. If 0, read all
                data available. Defaults to 0.
            timeout (int, optional): How long in ms to wait before returning.
                If 0, block until data received. Defaults to 250.

        Returns:
            String of received data.
        """
        if num_char == 0:
            num_char = self.ser.inWaiting()
        if timeout == 0:
            timeout = 1000000000000
        return self.ser.read(num_char, timeout)

    def send_cmd(self, cmd, scmd, data):
        """Send a SSV2 command to the target.

        Does all the CRC/Byte stuffing for you

        Args:
            cmd (char or int): The command to use
            scmd (int): The subcommand to use
            data (bytearray): The data to send
        """
        if type(data) is list:
            data = bytearray(data)
        if isinstance(cmd, str):
            cmd = ord(cmd[0])
        buf = [0x00, cmd, scmd, len(data)]
        buf.extend(data)
        crc = self._calc_crc(buf[1:])
        buf.append(crc)
        buf.append(0x00)
        buf = self._stuff_data(buf)
        self.write(buf)
        target_logger.debug("Sending: {}".format(bytearray(buf)))
        self._unstuff_data(buf)
        target_logger.debug("Unstuffed data: {}".format(bytearray(buf)))

    def reset_comms(self):
        """ Try to reset communication with the target and put it in
        a state to read commands

        Sends 10 0x00 bytes, sleeps for 0.05 seconds, then flushes the serial buffer
        """
        self.write([0x00]*2) # make sure target not processing a command
        time.sleep(0.05)
        while self.in_waiting() > 0:
            self.flush()
            time.sleep(0.05)

    def write(self, data, timeout=0):
        """ Writes data to the target over serial.

        Args:
            data (str): Data to write over serial.
            timeout (float or None): Wait <timeout> seconds for write buffer to clear.
                If None, block for a long time. If 0, return immediately. Defaults to 0.

        Raises:
            Warning: Target not connected
        """
        if type(data) is list:
            data = bytearray(data)
        self.ser.write(data, timeout)

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
        """
        if (self.last_key != key) or always_send:
            self.reset_comms()
            self.last_key = key
            self.simpleserial_write('k', key)
            if ack:
                if self.simpleserial_wait_ack(timeout) is None:
                    self.reset_comms()

    def in_waiting(self):
        """Returns the number of characters available from the serial buffer.

        Returns:
            The number of characters available via a target.read() call.
        """
        return self.ser.inWaiting()


    def flush(self):
        """Removes all data from the serial buffer.
        """
        self.ser.flush()

    def in_waiting_tx(self):
        """Returns the number of characters waiting to be sent by the ChipWhisperer.

        Requires firmware version >= 0.2 for the CWLite/Nano and firmware version and
        firmware version >= 1.2 for the CWPro.

        Used internally to avoid overflowing the TX buffer, since CW version 5.3

        Returns:
            The number of characters waiting to be sent to the target
        """
        return self.ser.inWaitingTX()

    @property
    def xonxoff(self):
        # TODO: check version to make sure fw has this
        return self.ser.xonxoff
    
    @xonxoff.setter
    def xonxoff(self, enable):
        self.ser.xonxoff = enable

    @property
    def currently_xoff(self):
        return self.ser.currently_xoff

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

class SimpleSerial2_CDC(SimpleSerial2):
    """Target Option for Using SSV2 with a CDC Port

    .. versionadded:: 5.5
        Added CDC SSV2

    Currently, the easiest way to use it is::

        scope = cw.scope()
        target = cw.target(scope, cw.targets.SimpleSerial2_CDC)

    Upon connection, this target object will using USB info from
    the scope object to figure out which serial port to use. You
    can also specify the serial port manually using the dev_path
    parameter. ::

        target = cw.target(scope, cw.targets.SimpleSerial2_CDC, dev_path='COM5')

    Other than that, usage is mostly the same as regular simpleserial
    V2, except the read timeout is always fixed to 250ms.

    It does offer better performance than the regular SSV2 object
    if reading serial data back from the target.
    """
    def __init__(self):
        super().__init__()
        self.ser = None

    def close(self):
        self.ser.close()

    def con(self, scope, dev_path=None, interface=None, flush_on_err=True):
        import serial # type: ignore
        self._flush_on_err = flush_on_err
        if dev_path is None:
            ports = scope.get_serial_ports()
            if len(ports) == 0:
                raise OSError("No port associated with scope found, please specify via dev_path")
            final_port = None
            if len(ports) > 1:
                for port in ports:
                    if port['interface'] == interface:
                        final_port = port
                        break
                if final_port is None:
                    raise ValueError("Invalid interface {}, found {}".format(interface, ports))
                dev_path = final_port['port']
            else:
                dev_path = ports[0]['port']
        self.dev_path = dev_path
        self.ser = serial.Serial(dev_path, baudrate=230400, timeout=0.25)


    def write(self, data):
        #data = bytearray(data)
        self.ser.write(data)

    def read(self, num_char = 0, timeout = 250):
        self.ser.timeout = timeout/1000
        if num_char == 0:
            num_char = self.ser.in_waiting
        return self.ser.read(num_char).decode('latin-1')

    def in_waiting(self):
        return self.ser.in_waiting

    def flush(self):
        self.ser.reset_input_buffer()

    def in_waiting_tx(self):
        return self.ser.out_waiting

    @property
    def baud(self):
        """The current baud rate of the serial connection.

        :Getter: Return the current baud rate.

        :Setter: Set a new baud rate. Valid baud rates are any integer in the
            range [500, 2000000].

        Raises:
            AttributeError: Target doesn't allow baud to be changed.
        """
        return self.ser.baud

    @baud.setter
    def baud(self, new_baud):
        self.ser.baud = new_baud

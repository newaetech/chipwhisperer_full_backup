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
from chipwhisperer.common.utils.util import camel_case_deprecated


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
      * :meth:`target.set_key <.SimpleSerial.set_key>`
      * :meth:`target.close <.SimpleSerial.close>`
      * :meth:`target.con <.SimpleSerial.con>`

    """
    _name = "Simple Serial"

    def __init__(self):
        TargetTemplate.__init__(self)

        self.ser = SimpleSerial_ChipWhispererLite()

        self.keylength = 16
        self.textlength = 16
        self.outputlength = 16
        self.input = ""
        self.key = ""
        self._protver = 'auto'
        self._read_timeout = 500
        self.masklength = 18
        self._fixedMask = True
        self.initmask = '1F 70 D6 3C 23 EB 1A B8 6A D5 E2 0D 5F D9 58 A3 CA 9D'
        self._mask = util.hexStrToByteArray(self.initmask)
        self.protformat = 'hex'
        self.last_key = bytearray(16)

        # Preset lists are in the form
        # {'Dropdown Name':['Init Command', 'Load Key Command', 'Load Input Command', 'Go Command', 'Output Format']}
        # If a command is None, it's left unchanged and the text field is editable;
        # Otherwise, it's loaded with the value and set to readonly
        self.presets = {
            'Custom':[None, None, None, None, None],
            'SimpleSerial Encryption':['','k$KEY$\\n', '', 'p$TEXT$\\n', 'r$RESPONSE$\\n'],
            'SimpleSerial Authentication':['','k$KEY$\\n', 't$EXPECTED$\\n', 'p$TEXT$\\n', 'r$RESPONSE$\\n'],
            'Glitching':[None, None, None, None, '$GLITCH$\\n'],
        }
        self._preset = 'Custom'

        self._linkedmaskgroup = (('maskgroup', 'cmdmask'), ('maskgroup', 'initmask'), ('maskgroup', 'masktype'),
                                 ('maskgroup', 'masklen'), ('maskgroup', 'newmask'))



        self._proto_ver = "auto"
        self._proto_timeoutms = 20
        self._init_cmd = ''
        self._key_cmd = 'k$KEY$\n'
        self._input_cmd = ''
        self._go_cmd = 'p$TEXT$\n'
        self._output_cmd = 'r$RESPONSE$\n'

        self._mask_enabled = False
        self._mask_cmd = 'm$MASK$\n'

        self.outstanding_ack = False

        self.setConnection(self.ser)
        self.disable_newattr()

    def getInitialMask(self):
        return " ".join(["%02X" % b for b in self._mask])

    def setInitialMask(self, initialMask, binaryMask=False):
        if initialMask:
            if binaryMask:
                maskStr = ''
                for s in initialMask:
                    maskStr += '%02x' % s
                self._mask = bytearray(initialMask)
            else:
                maskStr = initialMask
                self._mask = util.hexStrToByteArray(initialMask)
            self.initmask = maskStr

    @property
    def fixed_mask(self):
        if self.getMaskEnabled():
            return self.getInitialMask()
        return ''

    @fixed_mask.setter
    def fixed_mask(self, m):
        self.setInitialMask(m)

    def _dict_repr(self):
        dict = OrderedDict()
        dict['key_len'] = self.key_len
        dict['input_len'] = self.input_len
        dict['output_len'] = self.output_len
        dict['mask_len'] = self.mask_len
        dict['read_timeout'] = self.read_timeout

        dict['init_cmd']    = bytes(self.init_cmd.encode("UTF-8"))
        dict['key_cmd']  = bytes(self.key_cmd.encode("UTF-8"))
        dict['input_cmd']   = bytes(self.input_cmd.encode("UTF-8"))
        dict['go_cmd']   = bytes(self.go_cmd.encode("UTF-8"))
        dict['output_cmd'] = bytes(self.output_cmd.encode("UTF-8"))
        dict['mask_cmd'] = bytes(self.mask_cmd.encode("UTF-8"))

        dict['mask_enabled'] = self.mask_enabled
        if hasattr(self, 'getMaskEnabled'):
            dict['mask_enabled'] = self.mask_enabled
        
        if hasattr(self, 'getMaskType'):
            dict['mask_type'] = self.mask_type
            if dict['mask_type'] == 'fixed':
                dict['fixed_mask'] = self.fixed_mask

        dict['baud']     = self.baud
        dict['protver'] = self.protver
        return dict

    @property
    def key_len(self):
        """The length of the key (in bytes)"""
        return self.keyLen()

    @key_len.setter
    def key_len(self, length):
        self.setKeyLen(length)

    @property
    def input_len(self):
        """The length of the input to the crypto algorithm (in bytes)"""
        return self.textLen()

    @input_len.setter
    def input_len(self, length):
        self.setTextLen(length)

    @property
    def output_len(self):
        """The length of the output expected from the crypto algorithm (in bytes)"""
        return self.textLen()

    @output_len.setter
    def output_len(self, length):
        return self.setOutputLen(length)


    @property
    def read_timeout(self):
        """Timeout in mS on how long to wait for target to respond."""
        return self._read_timeout

    @read_timeout.setter
    def read_timeout(self, timeout):
        self._read_timeout = timeout

    @property
    def mask_len(self):
        """The length of the mask to send (in bytes)"""
        return self.maskLen()

    @mask_len.setter
    def mask_len(self, length):
        return self.setMaskLen(length)

    @property
    def init_cmd(self):
        """The command sent to the target before starting a capture.

        This value is a string that is sent to the target via the serial port.
        It can contain 4 special strings that are replaced during each capture:
        - "$KEY$": The encryption key
        - "$TEXT$": The text input
        - "$EXPECTED$": The expected result of the target's operation
        - "$MASK$": The mask used for masked-AES implementation
        These strings are replaced with ASCII values
        ex: k$KEY$ -> k0011223344556677

        Getter: Return the current init command

        Setter: Set a new init command
        """
        return self._init_cmd

    @init_cmd.setter
    def init_cmd(self, cmd):
        self._init_cmd = cmd

    @property
    def key_cmd(self):
        """The command used to send the key to the target.

        See init_cmd for details about special strings.

        Getter: Return the current key command

        Setter: Set a new key command
        """
        return self._key_cmd

    @key_cmd.setter
    def key_cmd(self, cmd):
        self._key_cmd = cmd

    @property
    def input_cmd(self):
        """The command used to send the text input to the target.

        See init_cmd for details about special strings.

        Getter: Return the current text input command

        Setter: Set a new text input command
        """
        return self._input_cmd

    @input_cmd.setter
    def input_cmd(self, cmd):
        self._input_cmd = cmd

    @property
    def go_cmd(self):
        """The command used to tell the target to start the operation.

        See init_cmd for details about special strings.

        Getter: Return the current text input command

        Setter: Set a new text input command
        """
        return self._go_cmd

    @go_cmd.setter
    def go_cmd(self, cmd):
        self._go_cmd = cmd

    @property
    def output_cmd(self):
        """The expected format of the output string.

        The output received from the target is compared to this string after
        capturing a trace. If the format doesn't match, an error is logged.

        This format string can contain two special strings:
          * "$RESPONSE$": If the format contains $RESPONSE$, then this part of
            the received text is converted to the output text (ciphertext or
            similar). The length of this response string is given in outputLen()
            and set by setOutputLen().
          * "$GLITCH$": If the format starts with $GLITCH$, then all output is
            redirected to the glitch explorer.

        Getter: Return the current output format

        Setter: Set a new output format
        """
        return self._output_cmd

    @output_cmd.setter
    def output_cmd(self, cmd):
        self._output_cmd = cmd

    @property
    def mask_cmd(self):
        """The command used to set a mask for the masked-AES implementation.
        This command might be ignored by unsupported targets.

        See init_cmd for details about special strings.

        Getter: Return the current mask command

        Setter: Set a new mask command
        """
        return self._mask_cmd

    @mask_cmd.setter
    def mask_cmd(self, cmd):
        self._mask_cmd = cmd

    @property
    def mask_type(self):
        """mask_type is either 'fixed' or 'random'."""
        return "fixed" if self.getMaskType() else "random"

    @mask_type.setter
    def mask_type(self, masktype):
        if masktype == 'fixed' or masktype == True:
            self._fixedMask = True
        elif masktype == 'random' or masktype == False:
            self._fixedMask = False
        else:
            raise ValueError('Invalid value for mask_type. Should be "fixed" or "random"')

    @property
    def mask_enabled(self):
        return self._mask_enabled

    @mask_enabled.setter
    def mask_enabled(self, enable):
        self._mask_enabled = enable

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

    def setKeyLen(self, klen):
        """ Set key length in bytes """
        self.keylength = klen

    def keyLen(self):
        """ Return key length in bytes """
        return self.keylength

    def setMaskLen(self, mlen):
        self.masklength = mlen

    def maskLen(self):
        return self.masklength

    def setTextLen(self, tlen):
        """ Set plaintext length. tlen given in bytes """
        self.textlength = tlen

    def textLen(self):
        """ Return plaintext length in bytes """
        return self.textlength

    def setOutputLen(self, tlen):
        """ Set plaintext length in bytes """
        self.outputlength = tlen

    def outputLen(self):
        """ Return output length in bytes """
        return self.outputlength

    def setProtFormat(self, protformat):
        """ Set the protocol format used 'bin' or 'hex' """
        self.protformat = protformat

    def protFormat(self):
        """ Return the protocol format used 'bin' or 'hex' """
        return self.protformat

    def getConnection(self):
        return self.ser

    def setConnection(self, con):
        self.ser = con
        self.ser.connectStatus = self.connectStatus
        self.ser.selectionChanged()

    def _con(self, scope = None):
        if not scope or not hasattr(scope, "qtadc"): Warning("You need a scope with OpenADC connected to use this Target")
        self.outstanding_ack = False

        self.ser.con(scope)
        # 'x' flushes everything & sets system back to idle
        self.ser.write("xxxxxxxxxxxxxxxxxxxxxxxx")
        self.ser.flush()


    def close(self):
        if self.ser != None:
            self.ser.close()

    def getVersion(self):
        self.ser.flush()
        self.ser.write("v\n")
        data = self.ser.read(4, timeout=self._proto_timeoutms)

        if len(data) > 1 and data[0] == 'z':
            logging.info("SimpleSerial: protocol V1.1 detected")
            return '1.1'
        else:
            logging.info("SimpleSerial: protocol V1.0 detected")
            return '1.0'


    def init(self):
        self.ser.flush()
        ver = self.protver
        if ver == 'auto':
            self._protver = self.getVersion()
        else:
            self._protver = ver
        self.outstanding_ack = False

        self.runCommand(self._init_cmd)
        # If we use a fix mask, set it once at init
        if self._mask_enabled and self._mask_cmd and self.getMaskType():
            self._mask = self.checkMask(self._mask)
            self.runCommand(self._mask_cmd)

    def newRandMask(self, _=None):
        new_mask = [random.randint(0, 255) for _ in range(self.maskLen())]
        self._mask = bytearray(new_mask)

    def reinit(self):
        if self._mask_enabled and self._mask_cmd:
            # Only set a mask if it's random. Fixed mask is set by init()
            if not self.getMaskType():  # Random
                self.newRandMask()
                self._mask = self.checkMask(self._mask)
                self.runCommand(self._mask_cmd)

    def setModeEncrypt(self):
        pass

    def setModeDecrypt(self):
        pass

    def convertVarToString(self, var):
        if isinstance(var, str):
            return var

        sep = ""
        s = sep.join(["%02x"%b for b in var])
        return s

    def runCommand(self, cmdstr, flushInputBefore=True):
        if self.connectStatus==False:
            raise Warning("Can't write to the target while disconected. Connect to it first.")

        if cmdstr is None or len(cmdstr) == 0:
            return

        # Protocol version 1.1 waits for ACK - if we have outstanding ACK, wait now
        if self._protver == '1.1':
            if self.outstanding_ack:
                # TODO - Should be user-defined maybe
                data = self.ser.read(4, timeout=500)
                if len(data) > 1:
                    if data[0] != 'z':
                        logging.error("SimpleSerial: ACK ERROR, read %02x" % ord(data[0]))
                else:
                    logging.error("SimpleSerial: ACK ERROR, did not see anything - TIMEOUT possible!")
                self.outstanding_ack = False

        varList = [("$KEY$",self.key, "Hex Encryption Key"),
                   ("$TEXT$",self.input, "Input Plaintext"),
                   ("$MASK$",self._mask, "Mask"),
                   ("$EXPECTED$", None, "Expected Ciphertext")]

        newstr = cmdstr

        #Find variables to insert
        for v in varList:
            if v[1] is not None:
                newstr = newstr.replace(v[0], self.convertVarToString(v[1]))

        #This is dumb
        newstr = newstr.replace("\\n", "\n")
        newstr = newstr.replace("\\r", "\r")

        #print newstr
        try:
            if flushInputBefore:
                self.ser.flushInput()
            if self.protformat == "bin":
                newstr = binascii.unhexlify(newstr)
            self.ser.write(newstr)
        except USBError:
            self.dis()
            raise Warning("Error in the target. It may have been disconnected.")
        except Exception as e:
            self.dis()
            raise e
        if self._protver == '1.1':
            self.outstanding_ack = True

    def loadEncryptionKey(self, key):
        """ Updates encryption key on target.

        The key is updated in this object and sent to the target over serial.
        """
        self.key = key
        if self.key:
            self.runCommand(self._key_cmd)

    def loadInput(self, inputtext):
        """ Sends plaintext to target

        Also updates the internal plaintext
        """
        self.input = inputtext
        self.runCommand(self._input_cmd)

    def loadMask(self, mask):
        self.mask = mask
        self.runCommand(self._mask_cmd)

    def is_done(self):
        return True

    def readOutput(self):
        dataLen= self.outputlength*2

        fmt = self._output_cmd
        #This is dumb
        fmt = fmt.replace("\\n", "\n")
        fmt = fmt.replace("\\r", "\r")

        if len(fmt) == 0:
            return None

        if fmt.startswith("$GLITCH$"):

            try:
                databytes = int(fmt.replace("$GLITCH$",""))
            except ValueError:
                databytes = 64

            self.newInputData.emit(self.ser.read(databytes,timeout=self.read_timeout))
            return None

        dataLen += len(fmt.replace("$RESPONSE$", ""))
        expected = fmt.split("$RESPONSE$")

        #Read data from serial port
        response = self.ser.read(dataLen, timeout=self.read_timeout)

        # If the protocol format is bin convert is back to hex for handling by CW
        if self.protformat == "bin":
            response = binascii.hexlify(response.encode('latin1'))

        if len(response) < dataLen:
            logging.warning('Response length from target shorter than expected (%d<%d): "%s".' % (len(response), dataLen, response))
            return None

        #Go through...skipping expected if applicable
        #Check expected first

        #Is a beginning part
        if len(expected[0]) > 0:
            if response[0:len(expected[0])] != expected[0]:
                logging.warning("Response start doesn't match what was expected:")
                logging.warning("Got {}, Expected {} + data".format(response, expected[0]))
                logging.warning("Hex Version: %s" % (" ".join(["%02x" % ord(t) for t in response])))

                return None

        startindx = len(expected[0])

        #Is middle part?
        data = bytearray(self.outputlength)
        if len(expected) == 2:
            for i in range(0,self.outputlength):
                # when glitched, the target might send us corrupted data...
                try:
                    data[i] = int(response[(i * 2 + startindx):(i * 2 + startindx + 2)], 16)
                except ValueError as e:
                    logging.warning('ValueError: %s' % str(e))

            startindx += self.outputlength*2

        #Is end part?
        if len(expected[1]) > 0:
            if response[startindx:startindx+len(expected[1])] != expected[1]:
                logging.warning("Unexpected end to response:")
                logging.warning("Got: {}, Expected {}".format(response, expected[1]))
                return None

        return data

    def go(self):
        self.runCommand(self._go_cmd)

    def checkEncryptionKey(self, kin):
        blen = self.keyLen()

        if len(kin) < blen:
            logging.warning('Padding key...')
            newkey = bytearray(kin)
            newkey += bytearray([0]*(blen - len(kin)))
            return newkey
        elif len(kin) > blen:
            logging.warning('Truncating key...')
            return kin[0:blen]

        return kin

    def checkPlaintext(self, text):
        blen = self.textLen()

        if len(text) < blen:
            logging.warning('Padding plaintext...')
            newtext = bytearray(text)
            newtext += bytearray([0] * (blen - len(text)))
            return newtext
        elif len(text) > blen:
            logging.warning('Truncating plaintext...')
            return text[0:blen]
        return text

    def checkMask(self, mask):
        blen = self.maskLen()

        if len(mask) < blen:
            logging.warning('Padding mask...')
            newmask = bytearray(mask)
            newmask += bytearray([0] * (blen - len(mask)))
            return newmask
        elif len(mask) > blen:
            logging.warning('Truncating mask...')
            return mask[0:blen]
        return mask

    def getExpected(self):
        """Based on key & text get expected if known, otherwise returns None"""
        return None
        if self.textLen() == 16:
            return TargetTemplate.getExpected(self)
        else:
            return None

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
            A dictionary.

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




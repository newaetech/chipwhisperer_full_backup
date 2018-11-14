#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2013-2014, NewAE Technology Inc
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

import random
from usb import USBError

import binascii
from ._base import TargetTemplate
from .simpleserial_readers.cwlite import SimpleSerial_ChipWhispererLite
from chipwhisperer.common.utils import util
from collections import OrderedDict


class SimpleSerial(TargetTemplate, util.DisableNewAttr):
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

        dict['init_cmd']    = self.init_cmd
        dict['key_cmd']  = self.key_cmd
        dict['input_cmd']   = self.input_cmd
        dict['go_cmd']   = self.go_cmd
        dict['output_cmd'] = self.output_cmd
        dict['mask_cmd'] = self.mask_cmd

        dict['mask_enabled'] = self.mask_enabled
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
        - "$RESPONSE$": If the format contains $RESPONSE$, then this part of
          the received text is converted to the output text (ciphertext or
          similar). The length of this response string is given in outputLen()
          and set by setOutputLen().
        - "$GLITCH$": If the format starts with $GLITCH$, then all output is
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

        This property is only compatible with the ChipWhisperer-Lite serial
        connection - using it with a different connection raises an
        AttributeError.

        Getter: Return the current baud rate.

        Setter: Set a new baud rate. Valid baud rates are any integer in the
                range [500, 2000000].
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

        self.ser.connectStatus.setValue(False)
        self.ser.connectStatus.connect(self.connectStatus.emit)
        self.ser.selectionChanged()

    def _con(self, scope = None):
        if not scope or not hasattr(scope, "qtadc"): Warning("You need a scope with OpenADC connected to use this Target")
        scope.scope_disconnected_signal.connect(self.dis)
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
        if self.connectStatus.value()==False:
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
                   ("$EXPECTED$", self.getExpected(), "Expected Ciphertext")]

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
        self.key = key
        if self.key:
            self.runCommand(self._key_cmd)

    def loadInput(self, inputtext):
        self.input = inputtext
        self.runCommand(self._input_cmd)

    def loadMask(self, mask):
        self.mask = mask
        self.runCommand(self._mask_cmd)

    def isDone(self):
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
            response = binascii.hexlify(response)

        if len(response) < dataLen:
            logging.warning('Response length from target shorter than expected (%d<%d): "%s".' % (len(response), dataLen, response))
            return None

        #Go through...skipping expected if applicable
        #Check expected first

        #Is a beginning part
        if len(expected[0]) > 0:
            if response[0:len(expected[0])] != expected[0]:
                logging.warning("Sync Error: %s"%response)
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
                logging.warning("Sync Error: %s"%response)
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
        if self.textLen() == 16:
            return TargetTemplate.getExpected(self)
        else:
            return None

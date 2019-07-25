#!/usr/bin/python
# HIGHLEVEL_CLASSLOAD_FAIL_FUNC_DEBUG
# -*- coding: utf-8 -*-
#
# Copyright (c) 2014, NewAE Technology Inc
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

import hid
from ._base import TargetTemplate


class HIDSPI(object):
    CMDSPI = 0x01
    CMDBOOT = 0xFE

    def findCWSPI(self, VID=0x03EB, PID=0xBAED):
        logging.info('Detecting HID device...')
        self.hdev = hid.device(VID, PID)
        logging.info('Manufacturer: %s' % self.hdev.get_manufacturer_string())
        logging.info('Product: %s' % self.hdev.get_product_string())

    def sendHID(self, cmd, data=[]):
        # Report is 64 bytes. -2 for our bytes
        padding = 64 - 2 - len(data)

        # Generate the message to pass to the stack
        hidmsg = []

        # Report ID is first byte (only 1 report here)
        hidmsg.append(0x00)

        # Command
        hidmsg.append(cmd)

        # Length of Payload (if applicable)
        hidmsg.append(len(data))

        # Payload padded to correct size
        hidmsg = hidmsg + data
        hidmsg = hidmsg + [0] * padding

        # Pass onward
        if self.hdev == None:
            return
        elif self.hdev != 2:
        # else:
            # print "HID Write: ",
            # for i in hidmsg:
            #    print "0x%02x"%i,
            # print ""
            if self.hdev.write(hidmsg) < 0:
                raise IOError("Error calling hdev.write()")

    def sendSPI(self, data):
        self.sendHID(self.CMDSPI, data)
        data = self.hdev.read(65)
        if data[0] & 0x80:
            plen = data[0] & 0x7F
            results = data[1:(1 + plen)]
        else:
            raise IOError("Invalid input HID Report")

        # print results
        return results

    def jumpBootloader(self):
        self.sendHID(self.CMDBOOT)


class ChipWhispererSPI(TargetTemplate):
    _name = "ChipWhisperer SPI"

    def __init__(self):
        TargetTemplate.__init__(self)
        self.hdev = HIDSPI()
        self.keylength = 16
        self.params.addChildren([
            {'name':'Jump to Bootloader', 'type':'action', 'action':self.hdev.jumpBootloader}
        ])

    def setKeyLen(self, klen):
        """ Set key length in BITS """
        self.keylength = klen / 8

    def keyLen(self):
        """ Return key length in BYTES """
        return self.keylength

    def _con(self, scope=None):
        self.hdev.findCWSPI()

    def init(self):
        return

    def setModeEncrypt(self):
        return

    def setModeDecrypt(self):
        return

    def loadEncryptionKey(self, key):
        self.key = key

    def loadInput(self, inputtext):
        self.input = inputtext

    def isDone(self):
        return True

    def readOutput(self):
        output = self.hdev.sendSPI([0] * 32)

        if output[:16] != output[16:]:
            raise IOError("Error, response not repeated: %s" % str(output))

        return output[:16]

    def go(self):
        self.hdev.sendSPI(list(self.input + self.key))

    def checkEncryptionKey(self, kin):
        blen = self.keyLen()

        if len(kin) < blen:
            logging.warning('Padding key...')
            newkey = bytearray(kin)
            newkey += bytearray([0] * (blen - len(kin)))
            return newkey
        elif len(kin) > blen:
            logging.warning('Truncating key...')
            return kin[0:blen]

        return kin

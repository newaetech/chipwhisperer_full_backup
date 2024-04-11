#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2014, NewAE Technology Inc
# All rights reserved.
#
# Authors: Colin O'Flynn
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

from chipwhisperer.logging import *
from ....common.utils.util import dict_to_str, DisableNewAttr
from .._OpenADCInterface import OpenADCInterface
from copy import copy

CODE_READ       = 0x80
CODE_WRITE      = 0xC0
CODE_USART      = 0x01

class ChipWhispererDecodeTrigger(DisableNewAttr):
    """
    Communicates and drives the Digital Pattern Match module inside the FPGA.

    Basic Usage for triggering on 'r'::

        #assuming setup scope:
        scope.trigger.triggers = 'tio1'
        scope.trigger.module = 'DECODEIO'
        scope.decode_IO.rx_baud = 38400
        scope.decode_IO.decode_type = 'USART'
        scope.decode_IO.trigger_pattern = ['r']
    """
    _name = 'I/O Decoder Trigger Module'
    def __init__(self, oa : OpenADCInterface):
        super().__init__()
        self.oa = oa
        self.pattern = None

        #init baud to avoid weird baud being reported (-91.4)
        breg = (38400 * 8 * 512 + self.systemClk() / 255) / (self.systemClk() / 128)
        breg = int(round(breg))
        self.setRxBaudReg(breg)

    def __str__(self):
        return self.__repr__()

    def __repr__(self):
        return dict_to_str(self._dict_repr())

    def _dict_repr(self):
        rtn = {}
        rtn['trigger_pattern'] = self.trigger_pattern
        rtn['rx_baud'] = self.rx_baud
        rtn['decode_type'] = self.decode_type
        return rtn

    @property
    def trigger_pattern(self):
        r""" The pattern to trigger off of.

        :Getter: Get the currently set trigger pattern

        :Setter: Sets the trigger pattern. Expects a list of ASCII
            characters/ints. ints in this list are equivalent to their
            corresponding ASCII representation. For example, [0x10] is the same
            as ['\\n'].

        Warns the user if they try to set an incorrect pattern.

        Example to trigger off of :code:`'r\n'`::

            scope.decode_IO.trigger_pattern = ['r', 0x10]

        """
        return self.pattern

    @trigger_pattern.setter
    def trigger_pattern(self, tp):
        self.set_triggerpattern(tp)

    def get_triggerpattern(self):
        """ Get the trigger pattern, where 'XX' is used for don't-care bytes """

        bm = self.get_trig_bitmap()
        rdata = self.get_trig_rawdata()

        #Get length from bitmap
        bm_length = 0
        for i in range(7, 0, -1):
            #If there is a 1 here, stop
            if (1<<i) & bm:
                bm_length = i+1
                break

        trigger_pattern = [0] * bm_length

        for i in range(0, bm_length):
            if (1<<i) & bm:
                trigger_pattern[i] = rdata[i]
            else:
                trigger_pattern[i] = "XX"

        return str(trigger_pattern)

    def set_triggerpattern(self, tp):
        """
        Set the trigger pattern - expects to be passed a string which will be evaluated, or a list directly

        tp should be a string which evaluates to a list, like "['r', 0x00, 0x12, 'a']
        """


        if isinstance(tp, str):
            # If we can't evaluate the string, give up now
            try:
                tl = eval(tp)
            except Exception as e:
                scope_logger.error("IO Decode Trigger: could not evaluate string %s, err = %s"%(tp, e))
                return
        else:
            tl = tp

        # If we didn't get a list or it's too long, we can't use this trigger pattern
        if type(tl) is not list:
            scope_logger.error("Trigger pattern must be a list")
            return
        if len(tl) > 8:
            scope_logger.error("Trigger pattern is of length %d, too long (max is 8)"%len(tl))
            return

        #Reverse order
        self.pattern = copy(tl)
        tl = tl[::-1]

        # Bitmap: 0s indicate "don't care" bytes in tdata
        bm = 0
        tdata = [0]*8
        for i in range(0, len(tl)):
            tli = tl[i]
            # "XX" and "xx" are don't care signals
            if tli in ("XX", "xx"):
                pass
            # Other strings need to be length 1
            elif (isinstance(tli, str)) and (len(tli) != 1):
                scope_logger.error("Trigger pattern with list of strings must have length = 1 for each string")
                return
            else:
                if isinstance(tl[i], str):
                    tdata[i] = ord(tl[i])
                else:
                    tdata[i] = tl[i]
                bm = bm | (1<<i)

        self.set_trig_bitmap(bm)
        self.set_trig_rawdata(tdata)

    @property
    def decode_type(self):
        """ Type of data to trigger off of. Only 'USART' for now.

        :Getter: Gets the current decode_type

        :Setter: Sets the decode_type

        Raises:
            ValueError: User attempted to set decode_type to something other than
                'USART'
        """
        dtype = self.get_decodetype()
        if dtype == CODE_USART:
            return "USART"
        else:
            return "unknown"

    @decode_type.setter
    def decode_type(self, typ):
        if typ == "USART":
            self.set_decodetype(CODE_USART)
        else:
            raise ValueError("Unknown decode type {}. Must be 'USART'".format(typ))

    def set_decodetype(self, decode_type):
        data = self.oa.sendMessage(CODE_READ, "IODECODETRIG_CFG_ADDR", Validate=False, maxResp=8)
        data[0] = (data[0] & 0xF0) | decode_type
        self.oa.sendMessage(CODE_WRITE, "IODECODETRIG_CFG_ADDR", data)

    def get_decodetype(self):
        data = self.oa.sendMessage(CODE_READ, "IODECODETRIG_CFG_ADDR", Validate=False, maxResp=8)
        return data[0] & 0x0F

    def get_trig_bitmap(self):
        data = self.oa.sendMessage(CODE_READ, "IODECODETRIG_CFG_ADDR", Validate=False, maxResp=8)
        return data[2]

    def set_trig_bitmap(self, bm):
        data = self.oa.sendMessage(CODE_READ, "IODECODETRIG_CFG_ADDR", Validate=False, maxResp=8)
        data[2] = bm
        self.oa.sendMessage(CODE_WRITE, "IODECODETRIG_CFG_ADDR", data)

    def get_trig_rawdata(self):
        data = self.oa.sendMessage(CODE_READ, "IODECODETRIG_DATA_ADDR", Validate=False, maxResp=8)
        return data

    def set_trig_rawdata(self, data):
        self.oa.sendMessage(CODE_WRITE, "IODECODETRIG_DATA_ADDR", data)

    @property
    def rx_baud(self):
        """ The baud rate of the serial data to trigger off of

        The baud rate can be between 0 and 1E6

        :Getter: Gets the set baud rate

        :Setter: Sets the baud rate

        Raises:
            ValueError: User attempted to set baud rate out of bounds
        """
        return self.get_rxbaud()

    @rx_baud.setter
    def rx_baud(self, baud):
        if baud > 1E6 or baud < 0:
            raise ValueError("Baud rate {} out of bounds of (0, 100000)".format(baud))
        self.set_rxbaud(baud)


    def set_rxbaud(self, baud):
        breg = (baud * 8 * 512 + self.systemClk() / 255) / (self.systemClk() / 128)
        breg = int(round(breg))
        self.setRxBaudReg(breg)

    def get_rxbaud(self):
        breg = self.rxBaudReg()
        tmp =((breg * (self.systemClk() / 128)) - (self.systemClk() / 255)) / 512
        baud = tmp / 8
        return baud



    def setRxBaudReg(self, breg):
        data = self.oa.sendMessage(CODE_READ, "IODECODETRIG_CFG_ADDR", Validate=False, maxResp=8)
        data[3] = breg & 0xff
        data[4] = (breg >> 8) & 0xff
        self.oa.sendMessage(CODE_WRITE, "IODECODETRIG_CFG_ADDR", data)

    def rxBaudReg(self):
        data = self.oa.sendMessage(CODE_READ, "IODECODETRIG_CFG_ADDR", Validate=False, maxResp=8)
        breg = data[3] | (data[4] << 8)
        return breg

    def systemClk(self):
        return self.oa.hwInfo.sysFrequency()

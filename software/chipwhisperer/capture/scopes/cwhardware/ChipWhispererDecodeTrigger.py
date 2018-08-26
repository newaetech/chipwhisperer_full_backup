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

from chipwhisperer.common.utils.parameter import Parameter, Parameterized, setupSetParam
import logging

CODE_READ       = 0x80
CODE_WRITE      = 0xC0

ADDR_DECODECFG = 57
ADDR_DECODEDATA = 58


class ChipWhispererDecodeTrigger(Parameterized):
    """
    Communicates and drives with the Digital Pattern Match module inside the FPGA. 
    """
    _name = 'I/O Decoder Trigger Module'
    def __init__(self, oa):
        self.oa = oa

        self.getParams().addChildren([

            {'name': 'Decode Type', 'type': 'list', 'values': {'USART': 1, 'SPI': 2, 'Unknown':0}, 'get':self.get_decodetype, 'set':self.set_decodetype},
            #{'name':'Data Line Source', 'type':'list', 'values':{'Ext-Trigger Mux':0, 'MOSI':1, 'MOSI':2, 'PDID':3}, 'get':self.get_dataline_src, 'set':self.set_dataline_src},
            {'name':'Trigger Data', 'key':'trigpatt', 'type':'str', 'get':self.get_triggerpattern, 'set':self.set_triggerpattern},
            {'name':'Baud', 'key':'baud', 'type':'int', 'limits':(0, 1E6), 'get':self.get_rxbaud, 'set':self.set_rxbaud},
            #{'name': 'Stop-Bits', 'key': 'stopbits', 'type': 'list', 'values': {'1': 1, '2': 2}, 'default': 1, 'get': self.stopBits, 'set': self.setStopBits, 'readonly': True},
            #{'name': 'Parity', 'key': 'parity', 'type': 'list', 'values': {'None': 'n', 'Even': 'e'}, 'default': 'n', 'get': self.parity, 'set': self.setParity, 'readonly': True},
        ])


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

    @setupSetParam("Trigger Data")
    def set_triggerpattern(self, tp):
        """
        Set the trigger pattern - expects to be passed a string which will be evaluated, or a list directly

        tp should be a string which evaluates to a list, like "['r', 0x00, 0x12, 'a']
        """


        if isinstance(tp, str):
            # If we can't evaluate the string, give up now
            try:
                tl = eval(tp)
            except Exception:
                logging.error("IO Decode Trigger: could not evaluate string %s"%tp)
                return
        else:
            tl = tp

        # If we didn't get a list or it's too long, we can't use this trigger pattern
        if type(tl) is not list:
            logging.error("Trigger pattern must be a list")
            return
        if len(tl) > 8:
            logging.error("Trigger pattern is of length %d, too long (max is 8)"%len(tl))
            return

        #Reverse order
        tl = tl[::-1]

        # Bitmap: 0s indicate "don't care" bytes in tdata
        bm = 0
        tdata = [0]*8
        for i in range(0, len(tl)):
            tli = tl[i]
            # "XX" and "xx" are don't care signals
            if tli == "XX" or tli == "xx":
                pass
            # Other strings need to be length 1
            elif (isinstance(tli, str)) and (len(tli) != 1):
                logging.error("Trigger pattern with list of strings must have length = 1 for each string")
                return
            else:
                tdata[i] = tl[i]
                bm = bm | (1<<i)

        self.set_trig_bitmap(bm)
        self.set_trig_rawdata(tdata)

    @setupSetParam("Decode Type")
    def set_decodetype(self, type):
        data = self.oa.sendMessage(CODE_READ, ADDR_DECODECFG, Validate=False, maxResp=8)
        data[0] = (data[0] & 0xF0) | type
        self.oa.sendMessage(CODE_WRITE, ADDR_DECODECFG, data)

    def get_decodetype(self):
        data = self.oa.sendMessage(CODE_READ, ADDR_DECODECFG, Validate=False, maxResp=8)
        return data[0] & 0x0F

    def get_trig_bitmap(self):
        data = self.oa.sendMessage(CODE_READ, ADDR_DECODECFG, Validate=False, maxResp=8)
        return data[2]

    def set_trig_bitmap(self, bm):
        data = self.oa.sendMessage(CODE_READ, ADDR_DECODECFG, Validate=False, maxResp=8)
        data[2] = bm
        self.oa.sendMessage(CODE_WRITE, ADDR_DECODECFG, data)

    def get_trig_rawdata(self):
        data = self.oa.sendMessage(CODE_READ, ADDR_DECODEDATA, Validate=False, maxResp=8)
        return data

    def set_trig_rawdata(self, data):
        self.oa.sendMessage(CODE_WRITE, ADDR_DECODEDATA, data)

    @setupSetParam("Baud")
    def set_rxbaud(self, baud):
        breg = (baud * 8 * 512 + self.systemClk() / 255) / (self.systemClk() / 128)
        breg = int(round(breg))
        self.setRxBaudReg(breg)

    def get_rxbaud(self):
        breg = self.rxBaudReg()
        baud = ((breg * (self.systemClk() / 128)) - (self.systemClk() / 255)) / 512
        baud = baud / 8
        return baud

    def setRxBaudReg(self, breg):
        data = self.oa.sendMessage(CODE_READ, ADDR_DECODECFG, Validate=False, maxResp=8)
        data[3] = breg & 0xff
        data[4] = (breg >> 8) & 0xff
        self.oa.sendMessage(CODE_WRITE, ADDR_DECODECFG, data)

    def rxBaudReg(self):
        data = self.oa.sendMessage(CODE_READ, ADDR_DECODECFG, Validate=False, maxResp=8)
        breg = data[3] | (data[4] << 8)
        return breg

    def systemClk(self):
        return self.oa.hwInfo.sysFrequency()
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

            {'name': 'Decode Type', 'type': 'list', 'values': {'SPI': 0}, 'value':0},
            #{'name':'Data Line Source', 'type':'list', 'values':{'Ext-Trigger Mux':0, 'MOSI':1, 'MOSI':2, 'PDID':3}, 'get':self.get_dataline_src, 'set':self.set_dataline_src},
            {'name':'Trigger Data', 'key':'trigpatt', 'type':'str', 'get':self.get_triggerpattern, 'set':self.set_triggerpattern},
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
        """ Set the trigger pattern - expects to be passed a string which will be evaluated, or a list directly """
        if isinstance(tp, basestring):
            tl = eval(tp)
        else:
            tl = tp

        if len(tl) > 8:
            raise ValueError("Trigger Pattern is of length %d, too long (max is 8)"%len(tl))

        bm = 0
        tdata = [0]*8
        for i in range(0, len(tl)):
            if tl[i] == "XX" or tl[i] == "xx":
                pass
            else:
                tdata[i] = tl[i]
                bm = bm | (1<<i)

        self.set_trig_bitmap(bm)
        self.set_trig_rawdata(tdata)

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
        data = data[::-1]
        self.oa.sendMessage(CODE_WRITE, ADDR_DECODEDATA, data)


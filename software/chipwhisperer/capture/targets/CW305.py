#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2015-2016, NewAE Technology Inc
# All rights reserved.
#
# Find this and more at newae.com - this file is part of the chipwhisperer
# project, http://www.chipwhisperer.com . ChipWhisperer is a registered
# trademark of NewAE Technology Inc in the US & Europe.
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
import time
from datetime import datetime
from functools import partial
import os.path
from _base import TargetTemplate
from chipwhisperer.hardware.naeusb.naeusb import NAEUSB
from chipwhisperer.hardware.naeusb.pll_cdce906 import PLLCDCE906
from chipwhisperer.hardware.naeusb.fpga import FPGA
from chipwhisperer.common.utils.parameter import setupSetParam


class CW305_USB(object):
    REQ_SYSCFG = 0x22
    REQ_VCCINT = 0x31
    SYSCFG_CLKOFF = 0x04
    SYSCFG_CLKON = 0x05
    SYSCFG_TOGGLE = 0x06
    VCCINT_XORKEY = 0xAE


class CW305(TargetTemplate):
    _name = "ChipWhisperer CW305 (Artix-7)"

    def __init__(self):
        TargetTemplate.__init__(self)
        self._naeusb = NAEUSB()
        self.pll = PLLCDCE906(self._naeusb, ref_freq = 12.0E6, parent=self)
        self.fpga = FPGA(self._naeusb)

        self.hw = None
        # self._fpgabs = QSettings().value("cw305-bitstream", '')
        self.oa = None

        self._woffset = 0x400

        self.params.addChildren([
            {'name':'PLL Settings', 'key':'pll', 'type':'group', 'children':[
                {'name':'Enabled', 'key':'pllenabled', 'type':'bool', 'default':False, 'set':self.pll.pll_enable_set, 'get':self.pll.pll_enable_get, 'psync':False},
                {'name':'CLK-SMA (X6)', 'key':'pll0', 'type':'group', 'children':[
                    {'name':'CLK-SMA Enabled', 'key':'pll0enabled', 'type':'bool', 'default':False, 'set':partial(self.pll.pll_outenable_set, outnum=0), 'get':partial(self.pll.pll_outenable_get, outnum=0), 'psync':False},
                    {'name':'CLK-SMA Source', 'key':'pll0source', 'type':'list', 'values':['PLL0', 'PLL1', 'PLL2'], 'default':'PLL0', 'set':partial(self.pll.pll_outsource_set, outnum=0), 'get':partial(self.pll.pll_outsource_get, outnum=0), 'psync':False},
                    {'name':'CLK-SMA Slew Rate', 'key':'pll0slew', 'type':'list', 'values':['+3nS', '+2nS', '+1nS', '+0nS'], 'default':'+0nS', 'set':partial(self.pll.pll_outslew_set, outnum=0), 'get':partial(self.pll.pll_outslew_get, outnum=0), 'psync':False},
                    {'name':'PLL0 Frequency', 'key':'pll0freq', 'type':'float', 'limits':(0.625E6, 167E6), 'default':0, 'step':1E6,
                        'siPrefix':True, 'suffix':'Hz', 'set':partial(self.pll.pll_outfreq_set, outnum=0), 'get':partial(self.pll.pll_outfreq_get, outnum=0), 'psync':False},
                ]},
                {'name':'CLK-N13 (FGPA Pin N13)', 'key':'pll1', 'type':'group', 'children':[
                    {'name':'CLK-N13 Enabled', 'key':'pll1enabled', 'type':'bool', 'default':False, 'set':partial(self.pll.pll_outenable_set, outnum=1), 'get':partial(self.pll.pll_outenable_get, outnum=1), 'psync':False},
                    {'name':'CLK-N13 Source', 'key':'pll1source', 'type':'list', 'values':['PLL1'], 'value':'PLL1'},
                    {'name':'CLK-N13 Slew Rate', 'key':'pll1slew', 'type':'list', 'values':['+3nS', '+2nS', '+1nS', '+0nS'], 'default':'+0nS', 'set':partial(self.pll.pll_outslew_set, outnum=1), 'get':partial(self.pll.pll_outslew_get, outnum=1), 'psync':False},
                    {'name':'PLL1 Frequency', 'key':'pll1freq', 'type':'float', 'limits':(0.625E6, 167E6), 'default':0, 'step':1E6,
                        'siPrefix':True, 'suffix':'Hz', 'set':partial(self.pll.pll_outfreq_set, outnum=1), 'get':partial(self.pll.pll_outfreq_get, outnum=1), 'psync':False},
                ]},
                {'name':'CLK-E12 (FGPA Pin E12)', 'key':'pll2', 'type':'group', 'children':[
                    {'name':'CLK-E12 Enabled', 'key':'pll2enabled', 'type':'bool', 'default':False, 'set':partial(self.pll.pll_outenable_set, outnum=2), 'get':partial(self.pll.pll_outenable_get, outnum=2), 'psync':False},
                    {'name':'CLK-E12 Source', 'key':'pll2source', 'type':'list', 'values':['PLL2'], 'value':'PLL2'},
                    {'name':'CLK-E12 Slew Rate', 'key':'pll2slew', 'type':'list', 'values':['+0nS', '+1nS', '+2nS', '+3nS'], 'default':'+0nS', 'set':partial(self.pll.pll_outslew_set, outnum=2), 'get':partial(self.pll.pll_outslew_get, outnum=2), 'psync':False},
                    {'name':'PLL2 Frequency', 'key':'pll2freq', 'type':'float', 'limits':(0.625E6, 167E6), 'default':0, 'step':1E6,
                        'siPrefix':True, 'suffix':'Hz', 'set':partial(self.pll.pll_outfreq_set, outnum=2), 'get':partial(self.pll.pll_outfreq_get, outnum=2), 'psync':False},
                ]},
                {'name':'Save as Default (stored in EEPROM)', 'type':'action', 'action':lambda _ : self.pll.pll_writedefaults()},
            ]},
            {'name':'Disable CLKUSB For Capture', 'key':'clkusbautooff', 'type':'bool', 'value':True},
            {'name':'Time CLKUSB Disabled for', 'key':'clksleeptime', 'type':'int', 'range':(1, 50000), 'value':50, 'suffix':'mS'},
            {'name':'CLKUSB Manual Setting', 'key':'clkusboff', 'type':'bool', 'value':True, 'action':self.usb_clk_setenabled_action},
            {'name':'Send Trigger', 'type':'action', 'action':self.usb_trigger_toggle},
            {'name':'VCC-INT', 'key':'vccint', 'type':'float', 'default':1.00, 'range':(0.6, 1.10), 'suffix':' V', 'decimals':3, 'set':self.vccint_set, 'get':self.vccint_get},
            {'name':'FPGA Bitstream', 'type':'group', 'children':[
                    {'name':'Bitstream File', 'key':'fpgabsfile', 'type':'file', 'value':"", "filter":'*.bit'},
                    {'name':'Program FPGA', 'type':'action', 'action':self.gui_programfpga},
            ]},
        ])

    def fpga_write(self, addr, data):
        """ Write to specified address """

        if addr < self._woffset:
            raise IOError("Write to read-only location: 0x%04x"%addr)

        return self._naeusb.cmdWriteMem(addr, data)
        
    def fpga_read(self, addr, readlen):
        """ Read from address """

        if addr > self._woffset:
            logging.info('Read from write address, confirm this is not an error')

        data = self._naeusb.cmdReadMem(addr, readlen)
        return data

    def usb_clk_setenabled_action(self, p):
        self.usb_clk_setenabled(p.getValue())

    def usb_clk_setenabled(self, status):
        """ Turn on or off the Data Clock to the FPGA """
        if status:
            self._naeusb.sendCtrl(CW305_USB.REQ_SYSCFG, CW305_USB.SYSCFG_CLKON)
        else:
            self._naeusb.sendCtrl(CW305_USB.REQ_SYSCFG, CW305_USB.SYSCFG_CLKOFF)

    def usb_trigger_toggle(self, _=None):
        """ Toggle the trigger line high then low """
        self._naeusb.sendCtrl(CW305_USB.REQ_SYSCFG, CW305_USB.SYSCFG_TOGGLE)
        
    @setupSetParam("VCC-INT")
    def vccint_set(self, vccint=1.0):
        """ Set the VCC-INT for the FPGA """

        # print "vccint = " + str(vccint)

        if (vccint < 0.6) or (vccint > 1.15):
            raise ValueError("VCC-Int out of range 0.6V-1.1V")
        
        # Convert to mV
        vccint = int(vccint * 1000)
        vccsetting = [vccint & 0xff, (vccint >> 8) & 0xff, 0]

        # calculate checksum
        vccsetting[2] = vccsetting[0] ^ vccsetting[1] ^ CW305_USB.VCCINT_XORKEY

        self._naeusb.sendCtrl(CW305_USB.REQ_VCCINT, 0, vccsetting)

        resp = self._naeusb.readCtrl(CW305_USB.REQ_VCCINT, dlen=3)
        if resp[0] != 2:
            raise IOError("VCC-INT Write Error, response = %d" % resp[0])

    def vccint_get(self):
        """ Get the last set value for VCC-INT """

        resp = self._naeusb.readCtrl(CW305_USB.REQ_VCCINT, dlen=3)
        return float(resp[1] | (resp[2] << 8)) / 1000.0

    def gui_programfpga(self, _=None):
        bsfile = self.params.getChild(['FPGA Bitstream',"fpgabsfile"]).getValue()
        if not os.path.isfile(bsfile):
            raise Warning("FPGA Bitstream not configured or %s not a file." % str(bsfile))
        starttime = datetime.now()
        self.fpga.FPGAProgram(open(bsfile, "rb"))
        stoptime = datetime.now()
        logging.info('FPGA Config time: %s' % str(stoptime - starttime))

    def _con(self, scope=None, bsfile=None, force=False):
        """Connect to CW305 board, download bitstream"""

        self._naeusb.con(idProduct=[0xC305])
        if self.fpga.isFPGAProgrammed() == False or force:
            if bsfile is None:
                bsfile = self.params.getChild(['FPGA Bitstream',"fpgabsfile"]).getValue()
            if not os.path.isfile(bsfile):
                print("FPGA Bitstream not configured or '%s' not a file." % str(bsfile))
            else:
                from datetime import datetime
                starttime = datetime.now()
                self.fpga.FPGAProgram(open(bsfile, "rb"))
                stoptime = datetime.now()
                logging.info('FPGA Config time: %s' % str(stoptime - starttime))
        self.usb_clk_setenabled(True)
        self.fpga_write(0x100+self._woffset, [0])
        self.params.refreshAllParameters()
        self.pll.cdce906init()

    def checkEncryptionKey(self, key):
        """Validate encryption key"""
        return key 

    def loadEncryptionKey(self, key):
        """Write encryption key to FPGA"""
        self.key = key
        key = key[::-1]
        self.fpga_write(0x100+self._woffset, key)

    def loadInput(self, inputtext):
        """Write input to FPGA"""
        self.input = inputtext
        text = inputtext[::-1]
        self.fpga_write(0x200+self._woffset, text)

    def isDone(self):
        """Check if FPGA is done"""
        result = self.fpga_read(0x50, 1)[0]

        if result == 0x00:
            return False
        else:
            # Clear trigger
            self.fpga_write(0x40+self._woffset, [0])
            # LED Off
            self.fpga_write(0x10+self._woffset, [0])
            return True
        
    def readOutput(self):
        """"Read output from FPGA"""
        data = self.fpga_read(0x200, 16)
        data = data[::-1]
        return data

    def go(self):
        """Disable USB clock (if requested), perform encryption, re-enable clock"""
        if self.findParam('clkusbautooff').getValue():
            self.usb_clk_setenabled(False)
            
        #LED On
        self.fpga_write(0x10+self._woffset, [0x01])

        time.sleep(0.01)
        self.usb_trigger_toggle()
        # self.FPGAWrite(0x100, [1])
        # self.FPGAWrite(0x100, [0])

        if self.findParam('clkusbautooff').getValue():
            time.sleep(self.findParam('clksleeptime').getValue() / 1000.0)
            self.usb_clk_setenabled(True)

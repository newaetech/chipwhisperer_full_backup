#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2013-2014, NewAE Technology Inc
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

import sys
from functools import partial
import time

from PySide.QtCore import *
from PySide.QtGui import *

try:
    from pyqtgraph.parametertree import Parameter
except ImportError:
    print "ERROR: PyQtGraph is required for this program"
    sys.exit()

from openadc.ExtendedParameter import ExtendedParameter
import ChipWhispererGlitch

CODE_READ = 0x80
CODE_WRITE = 0xC0

ADDR_DATA = 33
ADDR_LEN = 34
ADDR_BAUD = 35

ADDR_EXTCLK = 38
ADDR_TRIGSRC = 39
ADDR_TRIGMOD = 40

ADDR_I2CSTATUS = 47
ADDR_I2CDATA = 48

ADDR_IOROUTE = 55

class ChipWhispererExtra(QObject):
    paramListUpdated = Signal(list)

    def __init__(self, showScriptParameter=None, cwtype="cwrev2"):
        #self.cwADV = CWAdvTrigger()

        if cwtype == "cwrev2":
            self.cwEXTRA = CWExtraSettings()
        elif cwtype == "cwlite":
            self.cwEXTRA = CWExtraSettings(hasFPAFPB=False, hasGlitchOut=True, hasPLL=False)
        else:
            raise ValueError("Unknown ChipWhisperer: %s" % cwtype)

        self.params = Parameter.create(name='CW Extra', type='group', children=self.cwEXTRA.param)
        ExtendedParameter.setupExtended(self.params, self)
        self.showScriptParameter = showScriptParameter

        self.enableGlitch = True

        if self.enableGlitch:
            self.glitch = ChipWhispererGlitch.ChipWhispererGlitch(showScriptParameter, cwtype=cwtype)


    def paramTreeChanged(self, param, changes):
        if self.showScriptParameter is not None:
            self.showScriptParameter(param, changes, self.params)

    def setOpenADC(self, oa):
        #self.cwADV.setOpenADC(oa)
        if self.enableGlitch:
            self.glitch.setOpenADC(oa.sc)
        self.cwEXTRA.con(oa.sc)
        self.params.getAllParameters()

    def paramList(self):
        p = []
        p.append(self.params)
        if self.enableGlitch:
            p.append(self.glitch.params)
        return p

    def armPreScope(self):
        if self.enableGlitch:
            self.glitch.armPreScope()

    def armPostScope(self):
        if self.enableGlitch:
            self.glitch.armPostScope()

    #def testPattern(self):
    #    desired_freq = 38400 * 3
    #    clk = 30E6
    #    clkdivider = (clk / (2 * desired_freq)) + 1
    #    self.cwADV.setIOPattern(strToPattern("\n"), clkdiv=clkdivider)




class CWExtraSettings(object):
    PIN_FPA = 0x01
    PIN_FPB = 0x02
    PIN_RTIO1 = 0x04
    PIN_RTIO2 = 0x08
    PIN_RTIO3 = 0x10
    PIN_RTIO4 = 0x20
    MODE_OR = 0x00
    MODE_AND = 0x01

    MODULE_BASIC = 0x00
    MODULE_ADVPATTERN = 0x01
    MODULE_SADPATTERN = 0x02

    CLOCK_FPA = 0x00
    CLOCK_FPB = 0x01
    CLOCK_PLL = 0x02
    CLOCK_RTIOIN = 0x03
    CLOCK_RTIOOUT = 0x04

    IOROUTE_HIGHZ = 0
    IOROUTE_STX = 0b00000001
    IOROUTE_SRX = 0b00000010
    IOROUTE_USIO = 0b00000100
    IOROUTE_USII = 0b00001000
    IOROUTE_USINOUT = 0b00011000
    IOROUTE_STXRX = 0b00100010
    IOROUTE_GPIO = 0b01000000
    IOROUTE_GPIOE = 0b10000000

    def __init__(self, hasFPAFPB=True, hasGlitchOut=False, hasPLL=True):
        super(CWExtraSettings, self).__init__()
        self.oa = None

        self.name = "CW Extra Settings"
        self.param = [{'name': 'CW Extra Settings', 'type': 'group', 'children': []}]
        
        # Generate list of input pins present on the hardware
        if hasFPAFPB:
            tpins = [
                    {'name': 'Front Panel A', 'type':'bool', 'value':True, 'get':partial(self.getPin, pin=self.PIN_FPA), 'set':partial(self.setPin, pin=self.PIN_FPA)},
                    {'name': 'Front Panel B', 'type':'bool', 'value':True, 'get':partial(self.getPin, pin=self.PIN_FPB), 'set':partial(self.setPin, pin=self.PIN_FPB)}
                    ]
        else:
            tpins = []

        tpins.extend([{'name': 'Target IO1 (Serial TXD)', 'type':'bool', 'value':True, 'get':partial(self.getPin, pin=self.PIN_RTIO1), 'set':partial(self.setPin, pin=self.PIN_RTIO1)},
                    {'name': 'Target IO2 (Serial RXD)', 'type':'bool', 'value':True, 'get':partial(self.getPin, pin=self.PIN_RTIO2), 'set':partial(self.setPin, pin=self.PIN_RTIO2)},
                    {'name': 'Target IO3 (SmartCard Serial)', 'type':'bool', 'value':True, 'get':partial(self.getPin, pin=self.PIN_RTIO3), 'set':partial(self.setPin, pin=self.PIN_RTIO3)},
                    {'name': 'Target IO4 (Trigger Line)', 'type':'bool', 'value':True, 'get':partial(self.getPin, pin=self.PIN_RTIO4), 'set':partial(self.setPin, pin=self.PIN_RTIO4)},
                    {'name': 'Collection Mode', 'type':'list', 'values':{"OR":self.MODE_OR, "AND":self.MODE_AND}, 'value':"OR", 'get':self.getPinMode, 'set':self.setPinMode }
                    ])

        # Add trigger pins & modules
        self.param[0]["children"].extend([
                {'name': 'Trigger Pins', 'type':'group', 'children':tpins},
                {'name': 'Trigger Module', 'type':'list', 'values':{"Basic (Edge/Level)":self.MODULE_BASIC, "Digital Pattern Matching":self.MODULE_ADVPATTERN, "SAD Match":self.MODULE_SADPATTERN},
                 'value':self.MODULE_BASIC, 'set':self.setModule, 'get':self.getModule}])

        
        # Generate list of clock sources present in the hardware
        if hasFPAFPB:
            self.param[0]["children"].append(
                {'name': 'Trigger Out on FPA', 'type':'bool', 'value':False, 'set':self.setTrigOut})
            clksrc = {'Front Panel A':self.CLOCK_FPA, 'Front Panel B':self.CLOCK_FPB}
        else:
            clksrc = {}

        if hasPLL:
            clksrc["PLL Input"] = self.CLOCK_PLL

        clksrc["Target IO-IN"] = self.CLOCK_RTIOIN
        # clksrc["Fake"] = 0
        
        #Added July 6/2015, Release 0.11RC1
        #WORKAROUND: Initial CW-Lite FPGA firmware didn't default to CLKIN routed properly, and needed
        #            this to be set, as you can't do it through the GUI. This will be fixed in later firmwares.
        if hasFPAFPB==False and hasPLL==False:
            self.forceclkin = True
        else:
            self.forceclkin = False


        self.param[0]["children"].extend([
                {'name':'Clock Source', 'type':'list', 'values':clksrc, 'set':self.setClockSource, 'get':self.clockSource},
                {'name':'Target HS IO-Out', 'type':'list', 'values':{'Disabled':0, 'CLKGEN':2, 'Glitch Module':3}, 'value':0, 'set':self.setTargetCLKOut, 'get':self.targetClkOut},
                ])

        if hasGlitchOut:
            self.param[0]["children"].extend([
                {'name':'HS-Glitch Out Enable (High Power)', 'type':'bool', 'value':False, 'set':partial(self.setTargetGlitchOut, 'A'), 'get':partial(self.targetGlitchOut, 'A')},
                {'name':'HS-Glitch Out Enable (Low Power)', 'type':'bool', 'value':False, 'set':partial(self.setTargetGlitchOut, 'B'), 'get':partial(self.targetGlitchOut, 'B')}])

        self.param[0]["children"].extend([
                {'name':'Target IOn Pins', 'type':'group', 'children':[
                    {'name': 'Target IO1', 'key':'gpio1mode', 'type':'list', 'values':{'Serial TXD':self.IOROUTE_STX, 'Serial RXD':self.IOROUTE_SRX, 'USI-Out':self.IOROUTE_USIO,
                                                                    'USI-In':self.IOROUTE_USII, 'GPIO':self.IOROUTE_GPIOE, 'High-Z':self.IOROUTE_HIGHZ},
                                           'value':self.IOROUTE_STX, 'set':partial(self.setTargetIOMode, IONumber=0), 'get':partial(self.getTargetIOMode, IONumber=0)},
                    {'name': 'Target IO2', 'key':'gpio2mode', 'type':'list', 'values':{'Serial TXD':self.IOROUTE_STX, 'Serial RXD':self.IOROUTE_SRX, 'USI-Out':self.IOROUTE_USIO,
                                                                    'USI-In':self.IOROUTE_USII, 'GPIO':self.IOROUTE_GPIOE, 'High-Z':self.IOROUTE_HIGHZ},
                                           'value':self.IOROUTE_SRX, 'set':partial(self.setTargetIOMode, IONumber=1), 'get':partial(self.getTargetIOMode, IONumber=1)},
                    {'name': 'Target IO3', 'key':'gpio3mode', 'type':'list', 'values':{'Serial TXD':self.IOROUTE_STX, 'Serial RXD':self.IOROUTE_SRX, 'Serial-TX/RX':self.IOROUTE_STXRX,
                                                                    'USI-Out':self.IOROUTE_USIO, 'USI-In':self.IOROUTE_USII, 'USI-IN/OUT':self.IOROUTE_USINOUT,
                                                                    'GPIO':self.IOROUTE_GPIOE, 'High-Z':self.IOROUTE_HIGHZ},
                                           'value':self.IOROUTE_HIGHZ, 'set':partial(self.setTargetIOMode, IONumber=2), 'get':partial(self.getTargetIOMode, IONumber=2)},
                    {'name': 'Target IO4', 'key':'gpio4mode', 'type':'list', 'values':{'Serial TXD':self.IOROUTE_STX, 'GPIO':self.IOROUTE_GPIOE, 'High-Z':self.IOROUTE_HIGHZ},
                                           'value':self.IOROUTE_HIGHZ, 'set':partial(self.setTargetIOMode, IONumber=3), 'get':partial(self.getTargetIOMode, IONumber=3)},

                 ]},

                {'name':'Target IOn GPIO Mode', 'type':'group', 'children':[
                    {'name':'Target IO1: GPIO', 'key':'gpiostate1', 'type':'list', 'values':{'Low':False, 'High':True, 'Disabled':None}, 'value':None,
                                           'get':partial(self.getGPIOState, IONumber=0), 'set':partial(self.setGPIOState, IONumber=0)},
                    {'name':'Target IO2: GPIO', 'key':'gpiostate2', 'type':'list', 'values':{'Low':False, 'High':True, 'Disabled':None}, 'value':None,
                                           'get':partial(self.getGPIOState, IONumber=1), 'set':partial(self.setGPIOState, IONumber=1)},
                    {'name':'Target IO3: GPIO', 'key':'gpiostate3', 'type':'list', 'values':{'Low':False, 'High':True, 'Disabled':None}, 'value':None,
                                           'get':partial(self.getGPIOState, IONumber=2), 'set':partial(self.setGPIOState, IONumber=2)},
                    {'name':'Target IO4: GPIO', 'key':'gpiostate4', 'type':'list', 'values':{'Low':False, 'High':True, 'Disabled':None}, 'value':None,
                                           'get':partial(self.getGPIOState, IONumber=3), 'set':partial(self.setGPIOState, IONumber=3)},
                ]},
        ])
        
        #Catch for CW-Lite Specific Stuff
        if hasFPAFPB==False and hasPLL==False:
            self.param[0]["children"].extend([
                {'name':'Target Power State', 'type':'bool', 'value':'True', 'set':self.setTargetPowerState, 'get':self.getTargetPowerState}
            ])
        

    def con(self, oa):
        self.oa = oa
        
        # TEMPORARY PATCH: REMOVE ONCE FPGA FIXED
        #Over-ride default for CW-Lite
        if self.forceclkin:
            self.setClockSource(self.CLOCK_RTIOIN)

    def setGPIOState(self, state, IONumber):
        if state is not None:
            data = self.oa.sendMessage(CODE_READ, ADDR_IOROUTE, Validate=False, maxResp=8)
            if data[IONumber] & self.IOROUTE_GPIOE == 0:
                raise IOError("TargetIO %d is not in GPIO mode" % IONumber)

            if state:
                data[IONumber] |= self.IOROUTE_GPIO
            else:
                data[IONumber] &= ~(self.IOROUTE_GPIO)

            self.oa.sendMessage(CODE_WRITE, ADDR_IOROUTE, data)

    def getGPIOState(self, IONumber):
        data = self.oa.sendMessage(CODE_READ, ADDR_IOROUTE, Validate=False, maxResp=8)
        if data[IONumber] & self.IOROUTE_GPIOE == 0:
            return None

        if data[IONumber] == self.IOROUTE_GPIO:
            return True
        else:
            return False

    def setTargetIOMode(self, setting, IONumber):
        data = self.oa.sendMessage(CODE_READ, ADDR_IOROUTE, Validate=False, maxResp=8)
        data[IONumber] = setting
        self.oa.sendMessage(CODE_WRITE, ADDR_IOROUTE, data)

    def getTargetIOMode(self, IONumber):
        data = self.oa.sendMessage(CODE_READ, ADDR_IOROUTE, Validate=False, maxResp=8)
        return data[IONumber]

    def setClockSource(self, source):
        data = self.oa.sendMessage(CODE_READ, ADDR_EXTCLK, Validate=False, maxResp=1)
        data[0] = (data[0] & ~0x07) | source
        self.oa.sendMessage(CODE_WRITE, ADDR_EXTCLK, data)

    def clockSource(self):
        resp = self.oa.sendMessage(CODE_READ, ADDR_EXTCLK, Validate=False, maxResp=1)
        return resp[0] & 0x07

    def setTargetCLKOut(self, clkout):
        data = self.oa.sendMessage(CODE_READ, ADDR_EXTCLK, Validate=False, maxResp=1)
        data[0] = (data[0] & ~(3<<5)) | (clkout << 5)
        self.oa.sendMessage(CODE_WRITE, ADDR_EXTCLK, data)

    def targetClkOut(self):
        resp = self.oa.sendMessage(CODE_READ, ADDR_EXTCLK, Validate=False, maxResp=1)
        return ((resp[0] & (3<<5)) >> 5)

    def setTargetGlitchOut(self, out='A', enabled=False):
        data = self.oa.sendMessage(CODE_READ, ADDR_IOROUTE, Validate=False, maxResp=8)

        if out == 'A':
            bn = 0
        elif out == 'B':
            bn = 1
        else:
            raise ValueError("Invalid glitch output: %s" % str(out))

        if enabled:
            data[4] = data[4] | (1 << bn)
        else:
            data[4] = data[4] & ~(1 << bn)
        self.oa.sendMessage(CODE_WRITE, ADDR_IOROUTE, data)

    def targetGlitchOut(self, out='A'):
        resp = self.oa.sendMessage(CODE_READ, ADDR_IOROUTE, Validate=False, maxResp=8)
        if out == 'A':
            bn = 0
        elif out == 'B':
            bn = 1
        else:
            raise ValueError("Invalid glitch output: %s" % str(out))
        return resp[4] & (1 << bn)

    def setAVRISPMode(self, enabled):
        data = self.oa.sendMessage(CODE_READ, ADDR_IOROUTE, Validate=False, maxResp=8)
        if enabled:
            data[5] |= 0x01
        else:
            data[5] &= ~(0x01)

        self.oa.sendMessage(CODE_WRITE, ADDR_IOROUTE, data)

    def setTargetPowerState(self, enabled):
        data = self.oa.sendMessage(CODE_READ, ADDR_IOROUTE, Validate=False, maxResp=8)
        if enabled:
            data[5] &= ~(0x02)
        else:
            data[5] |= (0x02)

        self.oa.sendMessage(CODE_WRITE, ADDR_IOROUTE, data)

    def getTargetPowerState(self):
        data = self.oa.sendMessage(CODE_READ, ADDR_IOROUTE, Validate=False, maxResp=8)
        if data[5] & 0x02:
            return False
        else:
            return True

    def setPin(self, enabled, pin):
        current = self.getPins()

        pincur = current[0] & ~(pin)
        if enabled:
            pincur = pincur | pin

        self.setPins(pincur, current[1])


    def getPin(self, pin):
        current = self.getPins()
        current = current[0] & pin
        if current == 0:
            return False
        else:
            return True

    def setPinMode(self, mode):
        current = self.getPins()
        self.setPins(current[0], mode)

    def getPinMode(self):
        current = self.getPins()
        return current[1]

    def setPins(self, pins, mode):
        d = bytearray()
        d.append((mode << 6) | pins)
        self.oa.sendMessage(CODE_WRITE, ADDR_TRIGSRC, d)

    def getPins(self):
        resp = self.oa.sendMessage(CODE_READ, ADDR_TRIGSRC, Validate=False, maxResp=1)
        pins = resp[0] & 0x3F
        mode = resp[0] >> 6
        return(pins, mode)

    def setModule(self, module):
        resp = self.oa.sendMessage(CODE_READ, ADDR_TRIGMOD, Validate=False, maxResp=1)
        resp[0] = resp[0] & 0xF8
        resp[0] = resp[0] | module
        self.oa.sendMessage(CODE_WRITE, ADDR_TRIGMOD, resp)

    def getModule(self):
        resp = self.oa.sendMessage(CODE_READ, ADDR_TRIGMOD, Validate=False, maxResp=1)
        return resp[0]

    def setTrigOut(self, enabled):
        resp = self.oa.sendMessage(CODE_READ, ADDR_TRIGMOD, Validate=False, maxResp=1)
        resp[0] = resp[0] & 0xE7
        if enabled:
            resp[0] = resp[0] | 0x08
        self.oa.sendMessage(CODE_WRITE, ADDR_TRIGMOD, resp)

class CWPLLDriver(object):
    def __init__(self):
        super(CWPLLDriver, self).__init__()
        self.oa = None

    def con(self, oa):
        self.oa = oa

    def isPresent(self):
        """Check for CDCE906 PLL Chip"""
        try:
            result = self.readByte(0x00)
        except IOError:
            return False
        if result & 0x0F != 0x01:
            return False
        return True

    def setupPLL(self, N, M, bypass=False, highspeed=True, num=1):
        """
        Setup PLL1.
         * For M & N:
            M =< N.
            VCOF = (Fin * N) / M
            VCOF must be in range 80-300MHz.

         * For highspeed:
           Set to 'True' if VCO freq in range 180-300 MHz. Set low if in range 80-200 MHz
        """

        if num != 1:
            raise ValueError("Only PLL1 Config Supported")

        self.writeByte(0x01, M & 0xFF)
        self.writeByte(0x02, N & 0xFF)

        b = self.readByte(0x03)
        b &= (1<<6)|(1<<5)
        if bypass:
            b |= 1<<7

        b |= (M >> 8)
        b |= ((N >> 8) & 0x0F) << 1

        self.writeByte(0x03, b)

        b = self.readByte(0x06)
        b &= ~(1<<7)
        if highspeed:
            b |= 1 << 7

        self.writeByte(0x06, b)

    def setupDivider(self, setting, clksrc, divnum=2):
        """
        setting = Divide VCOF from PLL by this value

        clksrc = 0 means PLL Bypass
        clksrc = 1 means PLL1
        clksrc = 2 means PLL2 w/ SCC etc... not supported

        divnum is divider number (0-5)
        """

        if divnum > 5:
            raise ValueError("Invalid Divider Number (0-5 allowed): %d"%divnum)

        divreg = 13 + divnum

        if (setting < 1) | (setting > 127):
            raise ValueError("Invalid Divider Setting (1-127 allowed): %d"%setting)

        self.writeByte(divreg, setting)

        if divnum == 0:
            divreg = 9
            divbits = 5
        elif divnum == 1:
            divreg = 10
            divbits = 5
        elif divnum == 2:
            divreg = 11
            divbits = 0
        elif divnum == 3:
            divreg = 11
            divbits = 3
        elif divnum == 4:
            divreg = 12
            divbits = 0
        else:
            divreg = 12
            divbits = 3

        bold = self.readByte(divreg)
        b = bold & ~(0x07<<divbits)
        b |= (clksrc & 0x07) << divbits

        if bold != b:
            self.writeByte(divreg, b)

    def setupOutput(self, outnum, inv=False, enabled=True, divsource=2, slewrate=3):
        """
        outnum is output number, 0-5
        inv = invert output?
        enable = enable output?
        divsource = divider source, 0-5
        """
        outreg = 19 + outnum

        data = 0

        if enabled:
            data |= 1<<3

        if inv:
            data |= 1<<6

        if divsource > 5:
            raise ValueError("Invalid Divider Source Number (0-5 allowed): %d"%divsource)

        data |= divsource
        data |= (slewrate & 0x03) << 4

        self.writeByte(outreg, data)


    def setupClockSource(self, diff=True, useIN0=False, useIN1=False):
        if diff == False:
            #Select which single-ended input to use
            if (useIN0 ^ useIN1) == False:
                raise ValueError("Only one of useIN0 or useIN1 can be True")

            bold = self.readByte(10)
            b = bold & ~(1<<4)
            if useIN1:
                b |= 1<<4

            if b != bold:
                self.writeByte(10, b)
                # print "%x, %x"%(b, self.readByte(10))

        bold = self.readByte(11)
        bnew = bold & ~((1<<6) | (1<<7))
        if diff:
            bnew |= 1<<7
        else:
            bnew |= 1<<6

        if bnew != bold:
            self.writeByte(11, bnew)

        print "%x, %x" % (bnew, self.readByte(11))



    def readByte(self, regaddr, slaveaddr=0x69):
        d = bytearray([0x00, 0x80 | 0x69, 0x80 |  regaddr])
        self.oa.sendMessage(CODE_WRITE, ADDR_I2CSTATUS, d, Validate=False)
        time.sleep(0.05)

        d = bytearray([0x04, 0x80 | 0x69, 0x80 |  regaddr])
        self.oa.sendMessage(CODE_WRITE, ADDR_I2CSTATUS, d, Validate=False)
        time.sleep(0.05)

        d = bytearray([0x00, 0x80 | 0x69, 0x80 |  regaddr])
        self.oa.sendMessage(CODE_WRITE, ADDR_I2CSTATUS, d, Validate=False)
        time.sleep(0.05)

        stat = self.oa.sendMessage(CODE_READ, ADDR_I2CSTATUS, Validate=False, maxResp=3)
        if stat[0] & 0x01:
            raise IOError("No ACK from Slave in I2C")

        stat = self.oa.sendMessage(CODE_READ, ADDR_I2CDATA, Validate=False, maxResp=1)
        return stat[0]

    def writeByte(self, regaddr, data, slaveaddr=0x69):
        d = bytearray([data])
        self.oa.sendMessage(CODE_WRITE, ADDR_I2CDATA, d, Validate=False)

        d = bytearray([0x00, 0x69, 0x80 |  regaddr])
        self.oa.sendMessage(CODE_WRITE, ADDR_I2CSTATUS, d, Validate=False)
        time.sleep(0.05)

        d = bytearray([0x04, 0x69, 0x80 |  regaddr])
        self.oa.sendMessage(CODE_WRITE, ADDR_I2CSTATUS, d, Validate=False)
        time.sleep(0.05)

        d = bytearray([0x00, 0x69, 0x80 |  regaddr])
        self.oa.sendMessage(CODE_WRITE, ADDR_I2CSTATUS, d, Validate=False)
        time.sleep(0.05)

        stat = self.oa.sendMessage(CODE_READ, ADDR_I2CSTATUS, Validate=False, maxResp=3)
        if stat[0] & 0x01:
            raise IOError("No ACK from Slave in I2C")








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
import logging
import time
from functools import partial
import ChipWhispererGlitch
from chipwhisperer.common.utils.parameter import Parameterized, Parameter, setupSetParam
from chipwhisperer.common.utils import util

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


class GPIOMuxSettings(util.DisableNewAttr):
    def __init__(self, cwextra):
        self.cwe = cwextra

        self.TIO_VALID = [
            {'Serial TXD': self.cwe.IOROUTE_STX, 'Serial RXD': self.cwe.IOROUTE_SRX, 'USI-Out': self.cwe.IOROUTE_USIO,
             'USI-In': self.cwe.IOROUTE_USII, 'GPIO': self.cwe.IOROUTE_GPIOE, 'High-Z': self.cwe.IOROUTE_HIGHZ},

            {'Serial TXD': self.cwe.IOROUTE_STX, 'Serial RXD': self.cwe.IOROUTE_SRX, 'USI-Out': self.cwe.IOROUTE_USIO,
             'USI-In': self.cwe.IOROUTE_USII, 'GPIO': self.cwe.IOROUTE_GPIOE, 'High-Z': self.cwe.IOROUTE_HIGHZ},

            {'Serial TXD': self.cwe.IOROUTE_STX, 'Serial RXD': self.cwe.IOROUTE_SRX,
             'Serial-TX/RX': self.cwe.IOROUTE_STXRX,
             'USI-Out': self.cwe.IOROUTE_USIO, 'USI-In': self.cwe.IOROUTE_USII, 'USI-IN/OUT': self.cwe.IOROUTE_USINOUT,
             'GPIO': self.cwe.IOROUTE_GPIOE, 'High-Z': self.cwe.IOROUTE_HIGHZ},

            {'Serial TXD': self.cwe.IOROUTE_STX, 'GPIO': self.cwe.IOROUTE_GPIOE, 'High-Z': self.cwe.IOROUTE_HIGHZ}
        ]


        self.HS2_VALID = {'disabled': 0, 'clkout': 2, 'glitchout': 3}

        self.disable_newattr()

    @property
    def tio1(self):
        return self.tio(1)

    @tio1.setter
    def tio1(self, state):
        self.tio_set(1, state)

    @property
    def tio2(self):
        return self.tio(2)

    @tio2.setter
    def tio2(self, state):
        self.tio_set(2, state)

    @property
    def tio3(self):
        return self.tio(3)

    @tio3.setter
    def tio3(self, state):
        self.tio_set(3, state)

    @property
    def tio4(self):
        return self.tio(4)

    @tio4.setter
    def tio4(self, state):
        self.tio_set(4, state)

    def tio(self, pinnum):
        if pinnum < 1 or pinnum > 4:
            raise ValueError("Invalid PIN: %d. Valid range = 1-4." % pinnum, pinnum)

        pnum = pinnum - 1
        mode = self.cwe.getTargetIOMode(pnum)

        # Find string
        if mode != self.cwe.IOROUTE_GPIOE:
            for s, bmask in self.TIO_VALID[pnum].iteritems():
                if mode == bmask:
                    return s
            raise IOError("Invalid IO Mode returned by FPGA", mode)
        else:
            self.cwe.getGPIOState(pnum)

    def tio_set(self, pinnum, mode):

        if mode is None:
            mode = "High-Z"

        if pinnum < 1 or pinnum > 4:
            raise ValueError("Invalid PIN: %d. Valid range = 1-4." % pinnum, pinnum)

        valid_modes = self.TIO_VALID[pinnum-1].keys()

        if isinstance(mode, int):
            gpio_mode = True
            gpio_setting = mode
            mode = "GPIO"
        else:
            gpio_mode = False
            gpio_setting = False

        try:
            iomode = self.TIO_VALID[pinnum - 1][mode]
        except KeyError:
            raise ValueError("Invalid IO-Mode for GPIO%d: %s. Valid modes: %s" % (pinnum, mode, valid_modes))

        self.cwe.setTargetIOMode(iomode, pinnum - 1)

        if gpio_mode:
            self.cwe.setGPIOState(gpio_setting, pinnum - 1)

    def sck(self):
        pass

    def mosi(self):
        pass

    def miso(self):
        pass

    def cs(self):
        pass

    def pdic(self):
        pass

    def pdid(self):
        pass

    def nrst(self):
        pass

    @property
    def hs2(self):
        """Gets the HS2 (High-Speed Output) pin function. Returns a string of 'clkout', 'glitchout', or None """
        mode = self.cwe.targetClkOut()
        for k, v in self.HS2_VALID.iteritems():
            if mode == v:
                if k == 'disabled':
                    return None
                else:
                    return k

        raise IOError("Hardware returned known HS2 mode: %02x"%mode)

    @hs2.setter
    def hs2(self, mode):
        """Sets the HS2 (High-Speed Output) pin on the ChipWhisperer function ('clkout' or 'glitchout').

        Args:
            mode: The output mode of the string. Must be one of ['clkout', 'glitchout', None]
                  'clkout': The output from the internal CLKGEN module.
                  'glitchout': The output from the GLITCH module. You must configure this module as appropriate.
                  None: Disable the output, normally drives it low.
        """

        if mode is None:
            mode = 'disabled'

        if mode not in self.HS2_VALID:
            raise ValueError("Unknown mode for HS2 pin: '%s'. Valid modes: [%s]"%(mode, self.HS2_VALID.keys()), mode)

        self.cwe.setTargetCLKOut(self.HS2_VALID[mode])

    def reset_target(self, initial_state=1, reset_state=0, reset_delay=0.01, postreset_delay=0.01):
        pass

    @property
    def extclk_src(self):
        raise NotImplemented()

    @extclk_src.setter
    def extclk_src(self, src):
        raise NotImplemented()


class Crowbar(util.DisableNewAttr):

    @property
    def highpower(self):
        pass

    @highpower.setter
    def highpower(self, enabled):
        pass

    @property
    def lowpower(self):
        pass

    @lowpower.setter
    def lowpower(self, enabled):
        pass


class TriggerMux(util.DisableNewAttr):

    def __init__(self, cwextra):
        self.cwe = cwextra

        self.supported_tpins = {
            'tio1':self.cwe.PIN_RTIO1,
            'tio2': self.cwe.PIN_RTIO2,
            'tio3': self.cwe.PIN_RTIO3,
            'tio4': self.cwe.PIN_RTIO4,
        }

        if self.cwe.hasAux:
            self.supported_tpins['sma'] = self.cwe.PIN_FPA

        self.disable_newattr()

    @property
    def triggers(self):
        #Get pin logic + combo mode
        pins, mode = self.cwe.getPins()

        tstring = []
        if mode == self.cwe.MODE_OR: modes = "OR"
        elif mode ==  self.cwe.MODE_AND: modes = "AND"
        elif mode == self.cwe.MODE_NAND: modes = "NAND"
        else: raise IOError("Unknown mode reported by hardware: %02x", mode)

        if pins & self.cwe.PIN_RTIO1:
            tstring.append("tio1")
            tstring.append(modes)

        if pins & self.cwe.PIN_RTIO2:
            tstring.append("tio2")
            tstring.append(modes)

        if pins & self.cwe.PIN_RTIO3:
            tstring.append("tio3")
            tstring.append(modes)

        if pins & self.cwe.PIN_RTIO4:
            tstring.append("tio4")
            tstring.append(modes)

        if pins & self.cwe.PIN_FPA:
            tstring.append("sma")
            tstring.append(modes)

        #Remove last useless combination mode
        if len(tstring) > 1:
            tstring = tstring[0:-1]

        #Return a string indicating trigger mode
        return " ".join(tstring)

    @triggers.setter
    def triggers(self, s):

        s = s.lower()

        #Split up string
        triggers = s.split()

        #Check there is only one type of combination mode
        triggerset = set(triggers)
        numcombined = int('and' in triggerset) + int('or' in triggerset) + int('nand' in triggerset)
        if numcombined > 1:
           raise ValueError("Combining multiple triggers requires same logic between each combination", s)

        if numcombined == 0 and len(triggers) > 1:
            raise ValueError("Detected more than 1 trigger pin specified, but no combination logic.", s)

        enablelogic = 0

        #Figure out enabled triggers
        for t in self.supported_tpins.keys():
            if t in triggers:
                if triggers.count(t) != 1:
                    raise ValueError("Pin '%s' appears %d times, only 1 apperance supported" % (t, triggers.count(t)), s)
                enablelogic |= self.supported_tpins[t]

        #Find mode
        if ('or' in triggerset) or (len(triggerset) == 1):
            mode = self.cwe.MODE_OR
            modes = "or"
        elif 'and' in triggerset:
            mode = self.cwe.MODE_AND
            modes = "and"
        elif 'nand' in triggerset:
            mode = self.cwe.MODE_NAND
            modes = "nand"

        #Check mode operations in correct order, no unknown things
        expect_tpin = True
        for t in triggers:
            if expect_tpin:
                if t not in self.supported_tpins.keys():
                    raise ValueError("Error processing string at expected pin '%s'. Valid pins: %s"%(t, self.supported_tpins.keys()), s)
            else:
                if t != modes:
                    raise ValueError("Unexpected combination mode '%s'. Expected %s."%(t, modes), s)
            expect_tpin ^= True

        #Finally set this thing, guess we're looking HOT
        self.cwe.setPins(enablelogic, mode)

    def test(self):
        #Self-test for development
        self.triggers("tio1 OR tio2 AND tio3")
        self.triggers("tio1 OR tio2")
        self.triggers("tio1")
        self.triggers("tio4 AND tio2 AND tio1")
        self.triggers("tio1 NAND tio3")
        self.triggers("tio1 NAND tio2 NAND")
        self.triggers("tio1 AND tio1")


class GlitchGenerator(util.DisableNewAttr):
    pass


class SADTrigger(util.DisableNewAttr):
    pass


class DataTrigger(util.DisableNewAttr):
    pass


class ChipWhispererExtra(Parameterized):
    _name = 'CW Extra'

    def __init__(self, cwtype, scope, oa):
        #self.cwADV = CWAdvTrigger()

        self.cwEXTRA = CWExtraSettings(oa, cwtype)
        self.enableGlitch = True
        if self.enableGlitch:
            self.glitch = ChipWhispererGlitch.ChipWhispererGlitch(cwtype, scope, oa)

        self.getParams().append(self.cwEXTRA.getParams())
        self.getParams().append(self.glitch.getParams())

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


class CWExtraSettings(Parameterized):
    PIN_FPA = 0x01
    PIN_FPB = 0x02
    PIN_RTIO1 = 0x04
    PIN_RTIO2 = 0x08
    PIN_RTIO3 = 0x10
    PIN_RTIO4 = 0x20
    MODE_OR = 0x00
    MODE_AND = 0x01
    MODE_NAND = 0x02

    MODULE_BASIC = 0x00
    MODULE_ADVPATTERN = 0x01
    MODULE_SADPATTERN = 0x02
    MODULE_DECODEIO = 0x03

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

    _name = "CW Extra Settings"

    def __init__(self, oa, cwtype):

        if cwtype == "cwrev2":
            hasFPAFPB = True
            hasGlitchOut = False
            hasPLL = True
            hasAux=False
        elif cwtype == "cwlite":
            hasFPAFPB=False
            hasGlitchOut=True
            hasPLL=False
            hasAux=False
        elif cwtype == "cw1200":
            hasFPAFPB=False
            hasGlitchOut=True
            hasPLL=False
            hasAux=True
        else:
            raise ValueError("Unknown ChipWhisperer: %s" % cwtype)

        self.oa = oa
        self.hasFPAFPB = hasFPAFPB
        self.hasGlitchOut = hasGlitchOut
        self.hasPLL = hasPLL
        self.hasAux = hasAux

        ret = []
        # Generate list of input pins present on the hardware
        if self.hasFPAFPB:
            tpins = [
                {'name': 'Front Panel A', 'type':'bool', 'get':partial(self.getPin, pin=self.PIN_FPA), 'set':partial(self.setPin, pin=self.PIN_FPA)},
                {'name': 'Front Panel B', 'type':'bool', 'get':partial(self.getPin, pin=self.PIN_FPB), 'set':partial(self.setPin, pin=self.PIN_FPB)}
            ]
        else:
            tpins = []

        if self.hasAux:
            tpins.append({'name': 'Aux SMA', 'type':'bool', 'get':partial(self.getPin, pin=self.PIN_FPA), 'set':partial(self.setPin, pin=self.PIN_FPA)})

        tpins.extend([
            {'name': 'Target IO1 (Serial TXD)', 'type':'bool', 'get':partial(self.getPin, pin=self.PIN_RTIO1), 'set':partial(self.setPin, pin=self.PIN_RTIO1)},
            {'name': 'Target IO2 (Serial RXD)', 'type':'bool', 'get':partial(self.getPin, pin=self.PIN_RTIO2), 'set':partial(self.setPin, pin=self.PIN_RTIO2)},
            {'name': 'Target IO3 (SmartCard Serial)', 'type':'bool', 'get':partial(self.getPin, pin=self.PIN_RTIO3), 'set':partial(self.setPin, pin=self.PIN_RTIO3)},
            {'name': 'Target IO4 (Trigger Line)', 'type':'bool', 'get':partial(self.getPin, pin=self.PIN_RTIO4), 'set':partial(self.setPin, pin=self.PIN_RTIO4)},
            {'name': 'Collection Mode', 'type':'list', 'values':{"OR":self.MODE_OR, "AND":self.MODE_AND, "NAND":self.MODE_NAND}, 'get':self.getPinMode, 'set':self.setPinMode }
        ])

        # Add trigger pins & modules

        trigger_modules = {"Basic (Edge/Level)": self.MODULE_BASIC}

        if cwtype == "cwlite":
            pass
        elif cwtype == "cw1200":
            trigger_modules["SAD Match"] = self.MODULE_SADPATTERN
            trigger_modules["Digital IO Decode"] = self.MODULE_DECODEIO
        elif cwtype == "cwrev2":
            trigger_modules["SAD Match"] = self.MODULE_SADPATTERN
            trigger_modules["Digital Pattern Matching"] = self.MODULE_ADVPATTERN
        else:
            raise ValueError("Unknown ChipWhisperer %s"%cwtype)

        ret.extend([
            {'name': 'Trigger Pins', 'type':'group', 'children':tpins},
            {'name': 'Trigger Module', 'type':'list', 'values':trigger_modules,
             'set':self.setTriggerModule, 'get':self.getTriggerModule}
        ])

        # Generate list of clock sources present in the hardware
        if self.hasFPAFPB:
            ret.append({'name': 'Trigger Out on FPA', 'type':'bool', 'set':self.setTrigOut, 'get':self.getTrigOut})
            clksrc = {'Front Panel A':self.CLOCK_FPA, 'Front Panel B':self.CLOCK_FPB}
        else:
            clksrc = {}

        if self.hasAux:
            ret.append({'name': 'Trigger Out on Aux', 'type':'bool', 'set':self.setTrigOutAux, 'get':self.getTrigOut})
        else:
            clksrc = {}

        if self.hasPLL:
            clksrc["PLL Input"] = self.CLOCK_PLL

        clksrc["Target IO-IN"] = self.CLOCK_RTIOIN

        #Added July 6/2015, Release 0.11RC1
        #WORKAROUND: Initial CW-Lite FPGA firmware didn't default to CLKIN routed properly, and needed
        #            this to be set, as you can't do it through the GUI. This will be fixed in later firmwares.
        if cwtype == "cwlite":
            self.forceclkin = True
        else:
            self.forceclkin = False
        # TEMPORARY PATCH: REMOVE ONCE FPGA FIXED
        #Over-ride default for CW-Lite
        if self.forceclkin:
            self.setClockSource(self.CLOCK_RTIOIN, blockSignal=True)

        ret.extend([
            {'name':'Clock Source', 'type':'list', 'values':clksrc, 'set':self.setClockSource, 'get':self.clockSource},
            {'name':'Target HS IO-Out', 'type':'list', 'values':{'Disabled':0, 'CLKGEN':2, 'Glitch Module':3}, 'set':self.setTargetCLKOut, 'get':self.targetClkOut},
        ])

        if self.hasGlitchOut:
            ret.extend([
                {'name':'HS-Glitch Out Enable (High Power)', 'type':'bool', 'set':partial(self.setTargetGlitchOut, 'A'), 'get':partial(self.targetGlitchOut, 'A')},
                {'name':'HS-Glitch Out Enable (Low Power)', 'type':'bool', 'set':partial(self.setTargetGlitchOut, 'B'), 'get':partial(self.targetGlitchOut, 'B')}
            ])

        ret.extend([
            {'name':'Target IOn Pins', 'type':'group', 'children':[
                {'name': 'Target IO1', 'key':'gpio1mode', 'type':'list', 'values':{'Serial TXD':self.IOROUTE_STX, 'Serial RXD':self.IOROUTE_SRX, 'USI-Out':self.IOROUTE_USIO,
                                                                'USI-In':self.IOROUTE_USII, 'GPIO':self.IOROUTE_GPIOE, 'High-Z':self.IOROUTE_HIGHZ},
                                       'set':partial(self.setTargetIOMode, IONumber=0), 'get':partial(self.getTargetIOMode, IONumber=0)},
                {'name': 'Target IO2', 'key':'gpio2mode', 'type':'list', 'values':{'Serial TXD':self.IOROUTE_STX, 'Serial RXD':self.IOROUTE_SRX, 'USI-Out':self.IOROUTE_USIO,
                                                                'USI-In':self.IOROUTE_USII, 'GPIO':self.IOROUTE_GPIOE, 'High-Z':self.IOROUTE_HIGHZ},
                                       'set':partial(self.setTargetIOMode, IONumber=1), 'get':partial(self.getTargetIOMode, IONumber=1)},
                {'name': 'Target IO3', 'key':'gpio3mode', 'type':'list', 'values':{'Serial TXD':self.IOROUTE_STX, 'Serial RXD':self.IOROUTE_SRX, 'Serial-TX/RX':self.IOROUTE_STXRX,
                                                                'USI-Out':self.IOROUTE_USIO, 'USI-In':self.IOROUTE_USII, 'USI-IN/OUT':self.IOROUTE_USINOUT,
                                                                'GPIO':self.IOROUTE_GPIOE, 'High-Z':self.IOROUTE_HIGHZ},
                                       'set':partial(self.setTargetIOMode, IONumber=2), 'get':partial(self.getTargetIOMode, IONumber=2)},
                {'name': 'Target IO4', 'key':'gpio4mode', 'type':'list', 'values':{'Serial TXD':self.IOROUTE_STX, 'GPIO':self.IOROUTE_GPIOE, 'High-Z':self.IOROUTE_HIGHZ},
                                       'set':partial(self.setTargetIOMode, IONumber=3), 'get':partial(self.getTargetIOMode, IONumber=3)},
            ]},

            {'name':'Target IOn GPIO Mode', 'type':'group', 'children':[
                {'name':'Target IO1: GPIO', 'key':'gpiostate1', 'type':'list', 'values':{'Low':False, 'High':True, 'Disabled':None},
                                       'get':partial(self.getGPIOState, IONumber=0), 'set':partial(self.setGPIOState, IONumber=0)},
                {'name':'Target IO2: GPIO', 'key':'gpiostate2', 'type':'list', 'values':{'Low':False, 'High':True, 'Disabled':None},
                                       'get':partial(self.getGPIOState, IONumber=1), 'set':partial(self.setGPIOState, IONumber=1)},
                {'name':'Target IO3: GPIO', 'key':'gpiostate3', 'type':'list', 'values':{'Low':False, 'High':True, 'Disabled':None},
                                       'get':partial(self.getGPIOState, IONumber=2), 'set':partial(self.setGPIOState, IONumber=2)},
                {'name':'Target IO4: GPIO', 'key':'gpiostate4', 'type':'list', 'values':{'Low':False, 'High':True, 'Disabled':None},
                                       'get':partial(self.getGPIOState, IONumber=3), 'set':partial(self.setGPIOState, IONumber=3)},
                {'name':'nRST: GPIO', 'key':'gpiostatenrst', 'type':'list', 'values':{'Low':False, 'High':True, 'Default':None},
                                       'get':partial(self.getGPIOState, IONumber=100), 'set':partial(self.setGPIOState, IONumber=100)},
                {'name': 'PDID: GPIO', 'key': 'gpiostatepdid', 'type': 'list', 'values': {'Low': False, 'High': True, 'Default': None},
                                       'get': partial(self.getGPIOState, IONumber=101), 'set': partial(self.setGPIOState, IONumber=101)},
                {'name': 'PDIC: GPIO', 'key': 'gpiostatepdic', 'type': 'list', 'values': {'Low': False, 'High': True, 'Default': None},
                                       'get': partial(self.getGPIOState, IONumber=102), 'set': partial(self.setGPIOState, IONumber=102)},
            ]},
        ])

        #Catch for CW-Lite Specific Stuff
        if self.hasFPAFPB==False and self.hasPLL==False:
            ret.extend([
                {'name':'Target Power State', 'type':'bool', 'set':self.setTargetPowerState, 'get':self.getTargetPowerState}
            ])

        self.params = Parameter(name=self.getName(), type='group' , children=ret).register()

        #Add special single-class items used as higher-level API
        self.gpiomux = GPIOMuxSettings(self)
        self.triggermux = TriggerMux(self)

    @setupSetParam("")
    def setGPIOState(self, state, IONumber):

        # Special GPIO nRST, PDID, PDIC
        if IONumber >= 100:
            if IONumber == 100: # nRST IO Number
                bitnum = 0
            elif IONumber == 101: # PDID IO Number
                bitnum = 2
            elif IONumber == 102: # PDIC IO Number
                bitnum = 4
            else:
                raise ValueError("Invalid special IO Number: %d"%IONumber)

            data = self.oa.sendMessage(CODE_READ, ADDR_IOROUTE, Validate=False, maxResp=8)

            if state is None:
                #Disable GPIO mode
                data[6] &= ~(1<<bitnum)
            else:
                #Enable GPIO mode
                data[6] |= (1<<bitnum)

                #Set pin high/low
                if state:
                    data[6] |= (1<<(bitnum+1))
                else:
                    data[6] &= ~(1<<(bitnum + 1))

            self.oa.sendMessage(CODE_WRITE, ADDR_IOROUTE, data)

        #Regular GPIO1-4
        elif state is not None:
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

        #Catch special modes
        if IONumber >= 100:
            if IONumber == 100: # nRST IO Number
                bitnum = 0
            elif IONumber == 101: # PDID IO Number
                bitnum = 2
            elif IONumber == 102: # PDIC IO Number
                bitnum = 4
            else:
                raise ValueError("Invalid special IO Number: %d"%IONumber)

            if (data[6] & (1<<bitnum)) == 0:
                return None
            else:
                return (data[6] & (1<<(bitnum+1))) != 0

        if data[IONumber] & self.IOROUTE_GPIOE == 0:
            return None

        return data[IONumber] == self.IOROUTE_GPIO

    @setupSetParam("")
    def setTargetIOMode(self, setting, IONumber):
        data = self.oa.sendMessage(CODE_READ, ADDR_IOROUTE, Validate=False, maxResp=8)
        data[IONumber] = setting
        self.oa.sendMessage(CODE_WRITE, ADDR_IOROUTE, data)

    def getTargetIOMode(self, IONumber):
        data = self.oa.sendMessage(CODE_READ, ADDR_IOROUTE, Validate=False, maxResp=8)
        return data[IONumber]

    @setupSetParam("Clock Source")
    def setClockSource(self, source):
        data = self.oa.sendMessage(CODE_READ, ADDR_EXTCLK, Validate=False, maxResp=1)
        data[0] = (data[0] & ~0x07) | source
        self.oa.sendMessage(CODE_WRITE, ADDR_EXTCLK, data)

    def clockSource(self):
        resp = self.oa.sendMessage(CODE_READ, ADDR_EXTCLK, Validate=False, maxResp=1)
        return resp[0] & 0x07

    @setupSetParam("Target HS IO-Out")
    def setTargetCLKOut(self, clkout):
        data = self.oa.sendMessage(CODE_READ, ADDR_EXTCLK, Validate=False, maxResp=1)
        data[0] = (data[0] & ~(3<<5)) | (clkout << 5)
        self.oa.sendMessage(CODE_WRITE, ADDR_EXTCLK, data)

    def targetClkOut(self):
        resp = self.oa.sendMessage(CODE_READ, ADDR_EXTCLK, Validate=False, maxResp=1)
        return ((resp[0] & (3<<5)) >> 5)

    @setupSetParam("")
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

    @setupSetParam("Target Power State")
    def setTargetPowerState(self, enabled):
        data = self.oa.sendMessage(CODE_READ, ADDR_IOROUTE, Validate=False, maxResp=8)
        if enabled:
            data[5] &= ~(0x02)
        else:
            data[5] |= (0x02)

        self.oa.sendMessage(CODE_WRITE, ADDR_IOROUTE, data)

    def setTargetPowerSlew(self, fastmode):
        data = self.oa.sendMessage(CODE_READ, ADDR_IOROUTE, Validate=False, maxResp=8)
        if fastmode:
            data[5] |= (0x04)
        else:
            data[5] &= ~(0x04)

        self.oa.sendMessage(CODE_WRITE, ADDR_IOROUTE, data)

    def getTargetPowerState(self):
        data = self.oa.sendMessage(CODE_READ, ADDR_IOROUTE, Validate=False, maxResp=8)
        if data[5] & 0x02:
            return False
        else:
            return True

    @setupSetParam("")
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

    @setupSetParam("Collection Mode")
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

    @setupSetParam("Trigger Module")
    def setTriggerModule(self, module):

        #When using special modes, force rising edge & stop user from easily changing
        if module != self.MODULE_BASIC:
            Parameter.findParameter(['OpenADC', 'Trigger Setup', 'Mode']).setValue("rising edge", ignoreReadonly=True)
            Parameter.findParameter(['OpenADC', 'Trigger Setup', 'Mode']).setReadonly(True)
        else:
            Parameter.findParameter(['OpenADC', 'Trigger Setup', 'Mode']).setReadonly(False)

        resp = self.oa.sendMessage(CODE_READ, ADDR_TRIGMOD, Validate=False, maxResp=1)
        resp[0] &= 0xF8
        resp[0] |= module
        self.oa.sendMessage(CODE_WRITE, ADDR_TRIGMOD, resp)

    def getTriggerModule(self):
        resp = self.oa.sendMessage(CODE_READ, ADDR_TRIGMOD, Validate=False, maxResp=1)
        return resp[0]

    @setupSetParam("Trigger Out on Aux")
    def setTrigOutAux(self, enabled):
        resp = self.oa.sendMessage(CODE_READ, ADDR_TRIGMOD, Validate=False, maxResp=1)
        resp[0] &= 0xE7
        if enabled:
            resp[0] |= 0x08
        self.oa.sendMessage(CODE_WRITE, ADDR_TRIGMOD, resp)

    @setupSetParam("Trigger Out on FPA")
    def setTrigOut(self, enabled):
        resp = self.oa.sendMessage(CODE_READ, ADDR_TRIGMOD, Validate=False, maxResp=1)
        resp[0] &= 0xE7
        if enabled:
            resp[0] |= 0x08
        self.oa.sendMessage(CODE_WRITE, ADDR_TRIGMOD, resp)

    def getTrigOut(self):
        resp = self.oa.sendMessage(CODE_READ, ADDR_TRIGMOD, Validate=False, maxResp=1)
        return resp[0] & 0x08


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
        b &= (1 << 6)|(1 << 5)
        if bypass:
            b |= 1 << 7

        b |= (M >> 8)
        b |= ((N >> 8) & 0x0F) << 1

        self.writeByte(0x03, b)

        b = self.readByte(0x06)
        b &= ~(1 << 7)
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
            data |= 1 << 3

        if inv:
            data |= 1 << 6

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

        logging.debug('%x, %x' % (bnew, self.readByte(11)))

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

        d = bytearray([0x00, 0x69, 0x80 | regaddr])
        self.oa.sendMessage(CODE_WRITE, ADDR_I2CSTATUS, d, Validate=False)
        time.sleep(0.05)

        d = bytearray([0x04, 0x69, 0x80 | regaddr])
        self.oa.sendMessage(CODE_WRITE, ADDR_I2CSTATUS, d, Validate=False)
        time.sleep(0.05)

        d = bytearray([0x00, 0x69, 0x80 | regaddr])
        self.oa.sendMessage(CODE_WRITE, ADDR_I2CSTATUS, d, Validate=False)
        time.sleep(0.05)

        stat = self.oa.sendMessage(CODE_READ, ADDR_I2CSTATUS, Validate=False, maxResp=3)
        if stat[0] & 0x01:
            raise IOError("No ACK from Slave in I2C")

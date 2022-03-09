#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2013-2021, NewAE Technology Inc
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
import time
from collections import OrderedDict
from . import ChipWhispererGlitch
from ....common.utils import util

from ....logging import *

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
ADDR_IOREAD = 59

# API aliases for the TIO settings
_tio_alias = {
    'serial_tx': 'Serial TXD',
    'serial_rx': 'Serial RXD',
    'serial_tx_rx': 'Serial-TX/RX',
    'gpio_low': 'GPIO',
    'gpio_high': 'GPIO',
    'gpio_disabled': 'GPIO',
    'high_z': 'High-Z'
}

# More aliases for GPIO
_gpio_alias = {
    'gpio_low': 'low',
    'gpio_high': 'high',
    'gpio_disabled': 'disabled',
}

# Reverse alias lookup
_tio_api_alias = {_tio_alias[n]: n for n in _tio_alias}
_gpio_api_alias = {_gpio_alias[n]: n for n in _gpio_alias}

class GPIOSettings(util.DisableNewAttr):

    def __init__(self, cwextra):
        super().__init__()
        self.cwe = cwextra

        # This stuff actually matters, used with _tio_alias above
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

        self.HS2_VALID = {'disabled': 0, 'clkgen': 2, 'glitch': 3}

        self.disable_newattr()


    def read_tio_states(self):
        bitmask = self.cwe.readTIOPins()
        return tuple(((bitmask >> i) & 0x01) for i in range(4))

    def _dict_repr(self):
        rtn = OrderedDict()
        rtn['tio1'] = self.tio1
        rtn['tio2'] = self.tio2
        rtn['tio3'] = self.tio3
        rtn['tio4'] = self.tio4

        rtn['pdid'] = self.pdid
        rtn['pdic'] = self.pdic
        rtn['nrst'] = self.nrst

        rtn['glitch_hp'] = self.glitch_hp
        rtn['glitch_lp'] = self.glitch_lp

        rtn['extclk_src'] = self.extclk_src
        rtn['hs2'] = self.hs2

        rtn['target_pwr'] = self.target_pwr

        rtn['tio_states'] = self.tio_states

        rtn['cdc_settings'] = self.cdc_settings

        return rtn

    @property
    def tio_states(self):
        """
        Reads the logic level of the TIO pins (1-4) and
        returns them as a tuple of the logic levels.

        .. warning:: ChipWhisperer firmware before release 5.2.1 does not support
            reading the TIO pins!

        :getter: Read TIO states

        Returns:
            A tuple of 1's and 0's representing the logic levels
            of each TIO pin

        .. versionadded:: 5.3
            Add documented interface for the old method of reading TIO pins
        """
        return self.read_tio_states()

    def __repr__(self):
        return util.dict_to_str(self._dict_repr())

    def __str__(self):
        return self.__repr__()

    def _tioApiToInternal(self, tio_setting):
        """Convert an API TIO string to a (TIO, GPIO) parameter tuple

        Ex:
         * "serial_tx" -> ("Serial TXD", None)
         * "gpio_high" -> ("GPIO", "High")
        """

        # Accept None in place of "high-z"
        if tio_setting is None:
            return ("High-Z", None)
        # Accept True/False in place of "gpio_low"/"gpio_high"
        if isinstance(tio_setting, int):
            if tio_setting:
                gpio_mode = "high"
            else:
                gpio_mode = "low"
            return ("GPIO", gpio_mode)

        if tio_setting not in _tio_alias:
            raise ValueError("Can't find TIO setting %s; valid values: %s" % (tio_setting, _tio_alias), tio_setting)
        tio_param = _tio_alias[tio_setting]

        if tio_param == "GPIO":
            gpio_param = _gpio_alias[tio_setting]
            return (tio_param, gpio_param)
        else:
            return (tio_param, None)

    def _tioInternalToApi(self, tio_setting, gpio_setting):
        """Convert TIO and GPIO parameter settings to an API string.

        Ex:
         * ("Serial TXD", None) -> "serial_tx"
         * ("GPIO", "High") -> "gpio_high"
        """
        try:
            if tio_setting == "GPIO":
                return _gpio_api_alias[gpio_setting]
            else:
                return _tio_api_alias[tio_setting]
        except KeyError:
            return "?"

    @property
    def cdc_settings(self):
        """Check or set whether USART settings can be changed via the USB CDC connection

        i.e. whether you can change USART settings (baud rate, 8n1) via a serial client like PuTTY

        :getter: An array of length four for four possible CDC serial ports (though only one is used)

        :setter: Can set either via an integer (which sets both ports) or an array of length 4 (which sets each port)

        Returns None if using firmware before the CDC port was added
        """
        rawver = self.cwe.oa.serial.readFwVersion()
        ver = '{}.{}'.format(rawver[0], rawver[1])
        if ver < '0.30':
            return None
        return self.cwe.oa.serial.get_cdc_settings()

    @cdc_settings.setter
    def cdc_settings(self, port):
        rawver = self.cwe.oa.serial.readFwVersion()
        ver = '{}.{}'.format(rawver[0], rawver[1])
        if ver < '0.30':
            return None
        return self.cwe.oa.serial.set_cdc_settings(port)

    @property
    def tio1(self):
        """The function of the Target IO1 pin.

        TIO1 can be used for the following functions:
         * "serial_rx": UART input
         * "serial_tx": UART output
         * "high_z" / None: High impedance input
         * "gpio_low" / False: Driven output: logic 0
         * "gpio_high" / True: Driven output: logic 1
         * "gpio_disabled": Driven output: no effect

        Default value is "serial_rx".

        :Getter:  Return one of the above strings

        :Setter: Set the Target IO1 mode.

        Raises:
           ValueError: if new value is not one of the above modes

        """
        return self._tioInternalToApi(self._getTio(0), self._getGpio(0))

    @tio1.setter
    def tio1(self, state):
        (tio, gpio) = self._tioApiToInternal(state)
        self._setTio(0, tio)
        self._setGpio(0, gpio)

    @property
    def tio2(self):
        """The function of the Target IO2 pin.

        TIO2 can be used for the following functions:
         * "serial_rx": UART input
         * "serial_tx": UART output
         * "high_z" / None: High impedance input
         * "gpio_low" / False: Driven output: logic 0
         * "gpio_high" / True: Driven output: logic 1
         * "gpio_disabled": Driven output: no effect

        Default value is "serial_tx".

        :Getter:  Return one of the above strings

        :Setter: Set the Target IO2 mode.

        Raises:
           ValueError: if new value is not one of the above modes
        """
        return self._tioInternalToApi(self._getTio(1), self._getGpio(1))

    @tio2.setter
    def tio2(self, state):
        (tio, gpio) = self._tioApiToInternal(state)
        self._setTio(1, tio)
        self._setGpio(1, gpio)

    @property
    def tio3(self):
        """The function of the Target IO3 pin.

        TIO3 can be used for the following functions:
         * "serial_rx": UART input
         * "serial_tx": UART output
         * "serial_tx_rx": UART 1-wire I/O (for smartcards)
         * "high_z" / None: High impedance input
         * "gpio_low" / False: Driven output: logic 0
         * "gpio_high" / True: Driven output: logic 1
         * "gpio_disabled": Driven output: no effect

        Default value is "high_z".

        :Getter:  Return one of the above strings

        :Setter: Set the Target IO3 mode.

        Raises:
           ValueError: if new value is not one of the above modes
        """
        return self._tioInternalToApi(self._getTio(2), self._getGpio(2))

    @tio3.setter
    def tio3(self, state):
        (tio, gpio) = self._tioApiToInternal(state)
        self._setTio(2, tio)
        self._setGpio(2, gpio)

    @property
    def tio4(self):
        """The function of the Target IO4 pin.

        TIO4 can be used for the following functions:
         * "serial_tx": UART output
         * "high_z" / None: High impedance input
         * "gpio_low" / False: Driven output: logic 0
         * "gpio_high" / True: Driven output: logic 1
         * "gpio_disabled": Driven output: no effect

        Default value is "high_z". Typically, this pin is used as a trigger
        input.

        :Getter:  Return one of the above strings

        :Setter: Set the Target IO4 mode

        Raises:
           ValueError: if new value is not one of the above modes
        """
        return self._tioInternalToApi(self._getTio(3), self._getGpio(3))

    @tio4.setter
    def tio4(self, state):
        (tio, gpio) = self._tioApiToInternal(state)
        self._setTio(3, tio)
        self._setGpio(3, gpio)

    def _getTio(self, pinnum):
        if pinnum < 0 or pinnum >= 4:
            raise ValueError("Invalid PIN: %d. Valid range = 0-3." % pinnum, pinnum)

        mode = self.cwe.getTargetIOMode(pinnum)
        # Don't include GPIO state in mode check
        mode &= ~self.cwe.IOROUTE_GPIO

        # Find string
        for s, bmask in self.TIO_VALID[pinnum].items():
            if mode == bmask:
                return s

        raise IOError("Invalid IO Mode returned by FPGA", mode)

    def _setTio(self, pinnum, mode):
        if mode is None:
            mode = "High-Z"

        if pinnum < 0 or pinnum >= 4:
            raise ValueError("Invalid PIN: %d. Valid range = 0-3." % pinnum, pinnum)

        valid_modes = list(self.TIO_VALID[pinnum].keys())

        try:
            iomode = self.TIO_VALID[pinnum][mode]
        except KeyError as e:
            raise ValueError("Invalid IO-Mode for GPIO%d: %s. Valid modes: %s" % (pinnum+1, mode, valid_modes)) from e

        self.cwe.setTargetIOMode(iomode, pinnum)

    @property
    def pdic(self):
        """The state of the PDIC pin output pin.

        This is a GPIO pin. The following values are allowed:
         * "high" / True: logic 1
         * "low" / False: logic 0
         * "disabled" / "default" / "high_z" / None: undriven

        :Getter:  Return one of "high", "low", or "high_z"

        :Setter: Set the pin's state

        Raises:
        ValueError: if new state not listed above
        """
        return self._getGpio(102)

    @pdic.setter
    def pdic(self, state):
        self._setGpio(102, state)

    @property
    def pdid(self):
        """The state of the PDID pin.

        See pdic for more information."""
        return self._getGpio(101)

    @pdid.setter
    def pdid(self, state):
        self._setGpio(101, state)

    @property
    def nrst(self):
        """The state of the NRST pin.

        See pdic for more information."""
        return self._getGpio(100)

    @nrst.setter
    def nrst(self, state):
        self._setGpio(100, state)

    def _getGpio(self, pinnum):
        """GPIO state getter for GPIO settings on 1-4 and for special pins"""
        state = self.cwe.getGPIOState(pinnum)
        if state is None:
            return "high_z"
        elif state:
            return "high"
        else:
            return "low"

    def _setGpio(self, pinnum, level):
        """GPIO state setter for all GPIO pins"""
        if level == "high" or level == True:
            new_state = True
        elif level == "low" or level == False:
            new_state = False
        elif level in ("disabled", "default", "high_z", None):
            new_state = None
        else:
            raise ValueError("Can't set GPIO %d to level %s (expected 'high'/True, 'low'/False, or 'disabled'/'default'/'high_z'/None)" % (pinnum, level), level)

        self.cwe.setGPIOState(new_state, pinnum)

    @property
    def extclk_src(self):
        """The clock signal being used as input for EXTCLK.

        Currently, this can only be HS1, which is the clock from the target.
        As such, this value is read-only.
        """
        return "hs1"

    @property
    def hs2(self):
        """The clock signal routed to the HS2 high speed output pin.

        Allowed clock signals are:
         * "clkgen": The output from the CLKGEN module
         * "glitch": The output from the glitch module
         * "disabled" / None: No clock; output driven low

        :Getter:  Return one of "clkgen", "glitch", or "disabled"

        :Setter: Set the clock to be output on HS2.

        Raises:
        ValueError: if new value not listed above
        """
        mode = self.cwe.targetClkOut()
        for k, v in self.HS2_VALID.items():
            if mode == v:
                if k == 'disabled':
                    return None
                else:
                    return k

        raise IOError("Hardware returned unknown HS2 mode: %02x" % mode)

    @hs2.setter
    def hs2(self, mode):
        if mode is None:
            mode = 'disabled'

        if mode not in self.HS2_VALID:
            raise ValueError(
                "Unknown mode for HS2 pin: '%s'. Valid modes: [%s]" % (mode, list(self.HS2_VALID.keys())), mode)

        self.cwe.setTargetCLKOut(self.HS2_VALID[mode])

    @property
    def target_pwr(self):
        """Whether the target board is powered by the ChipWhisperer.

        If True, the ChipWhisperer is currently supplying power to the target
        board; if False, it is not. This setting can be used to cycle power to
        the target or to help program it.

        If the target board is powered through an external supply, this setting
        may have no effect.

        :Getter:  Return the current power state of the target (True or False)

        :Setter: Turn the target power on or off.
        """
        return self.cwe.getTargetPowerState()

    @target_pwr.setter
    def target_pwr(self, power):
        self.cwe.setTargetPowerState(power)

    @property
    def glitch_hp(self):
        """Whether the high-power crowbar MOSFET is enabled.

        The glitch output is an SMA-connected output line that is normally
        connected to a target's power rails. If this setting is enabled, a
        high-powered MOSFET shorts the power-rail to ground when the glitch
        module's output is active.

        .. warning:: Use with caution - ensure that the glitch module is properly
            configured before enabling this setting, as it is possible to
            permanently damage hardware with this output.

        :Getter:  Return True if enabled or False if disabled

        :Setter: Turn the high-power MOSFET on or off
        """
        return self.cwe.targetGlitchOut('A')

    @glitch_hp.setter
    def glitch_hp(self, active):
        self.cwe.setTargetGlitchOut('A', active)

    @property
    def glitch_lp(self):
        """Whether the low-power crowbar MOSFET is enabled.

        This is the low-power version of glitch_hp - see that documentation
        for more details.

        .. warning:: Use with caution - ensure that the glitch module is properly
            configured before enabling this setting, as it is possible to
            permanently damage hardware with this output.
        """
        return self.cwe.targetGlitchOut('B')

    @glitch_lp.setter
    def glitch_lp(self, active):
        self.cwe.setTargetGlitchOut('B', active)


    def reset_target(self, initial_state=1, reset_state=0, reset_delay=0.01, postreset_delay=0.01):
        raise NotImplementedError()

    # .. todo:: implement SCK/MOSI/MISO/CS?

    def sck(self):
        raise NotImplementedError()

    def mosi(self):
        raise NotImplementedError()

    def miso(self):
        raise NotImplementedError()

    def cs(self):
        raise NotImplementedError()

class TriggerSettings(util.DisableNewAttr):
    def __init__(self, cwextra):
        super().__init__()
        self.cwe = cwextra

        self.supported_tpins = {
            'tio1':self.cwe.PIN_RTIO1,
            'tio2': self.cwe.PIN_RTIO2,
            'tio3': self.cwe.PIN_RTIO3,
            'tio4': self.cwe.PIN_RTIO4,
            'nrst': self.cwe.PIN_TNRST,
        }

        self.last_module = "basic"
        if self.cwe.hasAux:
            self.supported_tpins['sma'] = self.cwe.PIN_FPA

        self.disable_newattr()

    def _dict_repr(self):
        rtn = OrderedDict()
        rtn['triggers'] = self.triggers
        rtn['module'] = self.module

        return rtn

    def __repr__(self):
        return util.dict_to_str(self._dict_repr())

    def __str__(self):
        return self.__repr__()

    @property
    def triggers(self):
        """The logical input into the trigger module.

        The trigger module uses some combination of the scope's I/O pins to
        produce a single value, which it uses for edge/level detection or UART
        triggers. This trigger output can combine 5 pins using one of 3
        different boolean operations.

        Pins:
         * tio1-4: Target I/O pins 1-4. Note that these pins can be in any mode.
         * nRST: Target I/O pin nRST. Note that these pins can be in any mode.
         * sma: An auxiliary SMA input, if available (only on CW1200)

        Boolean operations:
         * OR: True if any inputs are True; False if none are
         * AND: True if all inputs are True; False if any are not
         * NAND: False if all inputs are True; True if any are not

        Note that only one boolean operation can be used over all input pins.

        Examples of acceptable trigger inputs:
         * "tio1"
         * "tio3 OR tio4"
         * "tio1 NAND tio2 NAND sma"
         * "nrst"

        Examples of unallowed trigger inputs:
         * "tio1 tio2"
         * "tio1 AND tio2 OR tio3"
         * "tio1 OR tio1"
         * "tio1 XOR tio2"
         * "serial-tx"

        :Getter:  Return a string describing the trigger mode (see examples)

        :Setter: Set the trigger mode using a string like the ones above

        Raises:
           ValueError: if string cannot be converted to a legal mode
        """
        #Get pin logic + combo mode
        pins, mode = self.cwe.getPins()

        tstring = []
        if mode == self.cwe.MODE_OR: modes = "OR"
        elif mode ==  self.cwe.MODE_AND: modes = "AND"
        elif mode == self.cwe.MODE_NAND: modes = "NAND"
        else: raise IOError("Unknown mode reported by hardware: %02x" % mode)

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

        if pins & self.cwe.PIN_TNRST:
            tstring.append("nrst")
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
        for t in list(self.supported_tpins.keys()):
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
                if t not in list(self.supported_tpins.keys()):
                    raise ValueError("Error processing string at expected pin '%s'. Valid pins: %s"%(t, list(self.supported_tpins.keys())), s)
            else:
                if t != modes:
                    raise ValueError("Unexpected combination mode '%s'. Expected %s."%(t, modes), s)
            expect_tpin ^= True

        #Finally set this thing, guess we're looking HOT
        self.cwe.setPins(enablelogic, mode)

    @property
    def module(self):
        """The trigger module in use.

        The trigger modules available depend on the hardware. On the CWLite,
        only the basic trigger module can be used; on the CW1200, the serial
        data and SAD triggers are available too.

        Available trigger modules:
         * 'basic': Trigger on a logic level or edge

        :Getter: Returns 'basic'
        """
        return "basic"

class ProTrigger(TriggerSettings):
    def _dict_repr(self):
        rtn = super()._dict_repr()
        rtn['module'] = self.module
        rtn['aux_out'] = self.aux_out
        return rtn

    @property
    def module(self):
        """The trigger module in use.

        The trigger modules available depend on the hardware. On the CWLite,
        only the basic trigger module can be used; on the CW1200, the serial
        data and SAD triggers are available too.

        Available trigger modules:
         * 'basic': Trigger on a logic level or edge
         * 'SAD':   Trigger from SAD module (CWPro only)
         * 'DECODEIO': Trigger from decode_IO module (CWPro only)
         * 'trace': Trigger from TraceWhisperer  (CW-Husky only)

        :Getter: Return the active trigger module

        :Setter: Sets the active trigger module

        Raises:
            ValueError: module isn't one of the available strings
        """
        return self.last_module

    @module.setter
    def module(self, mode):
        if mode == "basic":
            module = self.cwe.MODULE_BASIC
        elif mode == "SAD":
            module = self.cwe.MODULE_SADPATTERN
        elif mode == "DECODEIO":
            module = self.cwe.MODULE_DECODEIO
        else:
            raise ValueError("Invalid mode {}. Must be 'basic', 'SAD', or 'DECODEIO'")

        resp = self.cwe.oa.sendMessage(CODE_READ, ADDR_TRIGMOD,
                                       Validate=False, maxResp=1)
        resp[0] &= 0xF8
        resp[0] |= module
        resp = self.cwe.oa.sendMessage(CODE_WRITE, ADDR_TRIGMOD,
                                       resp)
        self.last_module = mode

    @property
    def aux_out(self):
        """Controls AUX out on the CWPro

        CWPro only

        :Getter: Returns True if yes, False if no

        :Setter: Set True to enable aux_out, False to disable
        """
        resp = self.cwe.oa.sendMessage(CODE_READ, ADDR_TRIGMOD, Validate=False, maxResp=1)
        return bool(resp[0] & 0x08)

    @aux_out.setter
    def aux_out(self, enabled):
        resp = self.cwe.oa.sendMessage(CODE_READ, ADDR_TRIGMOD, Validate=False, maxResp=1)
        resp[0] &= 0xE7
        if enabled:
            resp[0] |= 0x08
        self.cwe.oa.sendMessage(CODE_WRITE, ADDR_TRIGMOD, resp)


class HuskyTrigger(TriggerSettings):
    def _dict_repr(self):
        rtn = super()._dict_repr()
        rtn['module'] = self.module
        return rtn

    @property
    def module(self):
        """The trigger module in use.

        The trigger modules available depend on the hardware. On the CWLite,
        only the basic trigger module can be used; on the CW1200, the serial
        data and SAD triggers are available too.

        Available trigger modules:
         * 'basic': Trigger on a logic level or edge
         * 'SAD':   Trigger from SAD module (CWPro only)
         * 'DECODEIO': Trigger from decode_IO module (CWPro only)
         * 'trace': Trigger from TraceWhisperer  (CW-Husky only)

        :Getter: Return the active trigger module

        :Setter: Sets the active trigger module

        Raises:
            ValueError: module isn't one of the available strings
        """
        return self.last_module

    @module.setter
    def module(self, mode):
        if mode == "basic":
            module = self.cwe.MODULE_BASIC
        elif mode == "SAD":
            module = self.cwe.MODULE_SADPATTERN # TODO
        elif mode == "DECODEIO":
            module = self.cwe.MODULE_DECODEIO # TODO
        elif mode == "trace":
            module = self.cwe.MODULE_TRACE
        else:
            raise ValueError("Invalid mode {}. Must be 'basic', 'SAD', 'DECODEIO', or 'trace'")

        resp = self.cwe.oa.sendMessage(CODE_READ, ADDR_TRIGMOD,
                                       Validate=False, maxResp=1)
        resp[0] &= 0xF8
        resp[0] |= module
        resp = self.cwe.oa.sendMessage(CODE_WRITE, ADDR_TRIGMOD,
                                       resp)
        self.last_module = mode


class SADTrigger(util.DisableNewAttr):
    pass


class DataTrigger(util.DisableNewAttr):
    pass


class ChipWhispererExtra(util.DisableNewAttr):
    _name = 'CW Extra'

    def __init__(self, cwtype, scope, oa):
        super().__init__()
        #self.cwADV = CWAdvTrigger()

        self.cwEXTRA = CWExtraSettings(oa, cwtype)
        #if cwtype == "cwhusky":
        self.enableGlitch = True
        if self.enableGlitch:
            self.glitch = ChipWhispererGlitch.ChipWhispererGlitch(cwtype, scope, oa)

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


class CWExtraSettings:
    PIN_FPA = 0x01
    PIN_TNRST = 0x02
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
    MODULE_TRACE = 0x04

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
        elif cwtype == "cwhusky":
            hasFPAFPB=False
            hasGlitchOut=False # TODO-temporary
            hasPLL=False
            hasAux=False
        else:
            raise ValueError("Unknown ChipWhisperer: %s" % cwtype)

        self.oa = oa
        self.hasFPAFPB = hasFPAFPB
        self.hasGlitchOut = hasGlitchOut
        self.hasPLL = hasPLL
        self.hasAux = hasAux


        #Add special single-class items used as higher-level API
        self.gpiomux = GPIOSettings(self)
        self.triggermux = TriggerSettings(self)
        self.protrigger = ProTrigger(self)
        self.huskytrigger = HuskyTrigger(self)


    def _setGPIOState(self, state, IONumber):
        # Special GPIO nRST, PDID, PDIC
        if IONumber >= 100:
            if IONumber == 100:  # nRST IO Number
                bitnum = 0
            elif IONumber == 101:  # PDID IO Number
                bitnum = 2
            elif IONumber == 102:  # PDIC IO Number
                bitnum = 4
            else:
                raise ValueError("Invalid special IO Number: %d" % IONumber)

            data = self.oa.sendMessage(CODE_READ, ADDR_IOROUTE, Validate=False, maxResp=8)

            if state is None:
                # Disable GPIO mode
                data[6] &= ~(1 << bitnum)
            else:
                # Enable GPIO mode
                data[6] |= (1 << bitnum)

                # Set pin high/low
                if state:
                    data[6] |= (1 << (bitnum + 1))
                else:
                    data[6] &= ~(1 << (bitnum + 1))

            self.oa.sendMessage(CODE_WRITE, ADDR_IOROUTE, data)

        # Regular GPIO1-4
        elif state is not None:
            data = self.oa.sendMessage(CODE_READ, ADDR_IOROUTE, Validate=False, maxResp=8)

            if data[IONumber] & self.IOROUTE_GPIOE == 0:
                raise IOError("TargetIO %d is not in GPIO mode" % IONumber)

            if state:
                data[IONumber] |= self.IOROUTE_GPIO
            else:
                data[IONumber] &= ~(self.IOROUTE_GPIO)

            self.oa.sendMessage(CODE_WRITE, ADDR_IOROUTE, data)

    def setGPIOState1(self, state):
        self._setGPIOState(state, 0)

    def setGPIOState2(self, state):
        self._setGPIOState(state, 1)

    def setGPIOState3(self, state):
        self._setGPIOState(state, 2)

    def setGPIOState4(self, state):
        self._setGPIOState(state, 3)

    def setGPIOStatenrst(self, state):
        self._setGPIOState(state, 100)

    def setGPIOStatepdid(self, state):
        self._setGPIOState(state, 101)

    def setGPIOStatepdic(self, state):
        self._setGPIOState(state, 102)

    def setGPIOState(self, state, IONumber):
        if IONumber == 0:
            self.setGPIOState1(state)
        elif IONumber == 1:
            self.setGPIOState2(state)
        elif IONumber == 2:
            self.setGPIOState3(state)
        elif IONumber == 3:
            self.setGPIOState4(state)
        elif IONumber == 100:
            self.setGPIOStatenrst(state)
        elif IONumber == 101:
            self.setGPIOStatepdid(state)
        elif IONumber == 102:
            self.setGPIOStatepdic(state)
        else:
            raise ValueError("Invalid GPIO State")

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

        return data[IONumber] & self.IOROUTE_GPIO

    def readTIOPins(self):
        """Read signal level of all 4 Target IOn pins synchronously.

        In most cases this is useful for low-speed digital input, hence the
        GPIO state of the Target IOn pin(s) used for digital input should be
        configured as 'High-Z'.

        Returns a bit mask where set bits indicate which of the 4 target IOn
        pins is read as high. Counting starts at bit 0, for example, bit0
        refers to tio1.
        """

        data = self.oa.sendMessage(CODE_READ, ADDR_IOREAD, Validate=False, maxResp=1)
        return data[0]

    def readTIOPin(self, tio):
        """Read signal level of a Target IOn pin.

        Returns True if the signal level of the Target IOn pin is high,
        otherwise False is returned.
        """
        if tio < 1 or tio > 4:
            raise ValueError("Invalid Target IO. Currently only tio1 to tio4 are supported.")
        tios = self.readTIOPins()
        return (tios & (1<<(tio-1))) > 0

    def setTargetIOMode1(self, setting):
        self._setTargetIOMode(setting, 0)

    def setTargetIOMode2(self, setting):
        self._setTargetIOMode(setting, 1)

    def setTargetIOMode3(self, setting):
        self._setTargetIOMode(setting, 2)

    def setTargetIOMode4(self, setting):
        self._setTargetIOMode(setting, 3)

    def _setTargetIOMode(self, setting, IONumber):
        #Sends actual IO mode to FPGA
        data = self.oa.sendMessage(CODE_READ, ADDR_IOROUTE, Validate=False, maxResp=8)
        data[IONumber] = setting
        self.oa.sendMessage(CODE_WRITE, ADDR_IOROUTE, data)

    def setTargetIOMode(self, setting, IONumber):
        #To keep parameters syncronized, we need ot call individual set functions
        if IONumber == 0:
            self.setTargetIOMode1(setting)
        elif IONumber == 1:
            self.setTargetIOMode2(setting)
        elif IONumber == 2:
            self.setTargetIOMode3(setting)
        elif IONumber == 3:
            self.setTargetIOMode4(setting)
        else:
            raise ValueError("Invalid IO Number, valid range is 0,1,2,3", IONumber)


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
        return bool(resp[4] & (1 << bn))

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

    def setTriggerModule(self, module):

        #When using special modes, force rising edge & stop user from easily changing
        resp = self.oa.sendMessage(CODE_READ, ADDR_TRIGMOD, Validate=False, maxResp=1)
        resp[0] &= 0xF8
        resp[0] |= module
        self.oa.sendMessage(CODE_WRITE, ADDR_TRIGMOD, resp)

    def getTriggerModule(self):
        resp = self.oa.sendMessage(CODE_READ, ADDR_TRIGMOD, Validate=False, maxResp=1)
        return resp[0]

    def setTrigOutAux(self, enabled):
        resp = self.oa.sendMessage(CODE_READ, ADDR_TRIGMOD, Validate=False, maxResp=1)
        resp[0] &= 0xE7
        if enabled:
            resp[0] |= 0x08
        self.oa.sendMessage(CODE_WRITE, ADDR_TRIGMOD, resp)

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

        scope_logger.debug('%x, %x' % (bnew, self.readByte(11)))

    def readByte(self, regaddr, slaveaddr=0x69):
        d = bytearray([0x00, 0x80 | 0x69, 0x80 |  regaddr])
        self.oa.sendMessage(CODE_WRITE, ADDR_I2CSTATUS, d, Validate=False)
        time.sleep(0.001)

        d = bytearray([0x04, 0x80 | 0x69, 0x80 |  regaddr])
        self.oa.sendMessage(CODE_WRITE, ADDR_I2CSTATUS, d, Validate=False)
        time.sleep(0.001)

        d = bytearray([0x00, 0x80 | 0x69, 0x80 |  regaddr])
        self.oa.sendMessage(CODE_WRITE, ADDR_I2CSTATUS, d, Validate=False)
        time.sleep(0.001)

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
        time.sleep(0.005)

        d = bytearray([0x04, 0x69, 0x80 | regaddr])
        self.oa.sendMessage(CODE_WRITE, ADDR_I2CSTATUS, d, Validate=False)
        time.sleep(0.005)

        d = bytearray([0x00, 0x69, 0x80 | regaddr])
        self.oa.sendMessage(CODE_WRITE, ADDR_I2CSTATUS, d, Validate=False)
        time.sleep(0.005)

        stat = self.oa.sendMessage(CODE_READ, ADDR_I2CSTATUS, Validate=False, maxResp=3)
        if stat[0] & 0x01:
            raise IOError("No ACK from Slave in I2C")

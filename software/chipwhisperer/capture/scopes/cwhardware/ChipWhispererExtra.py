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
from . import ChipWhispererGlitch
from ....common.utils import util

from ....logging import *

CODE_READ = 0x80
CODE_WRITE = 0xC0

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

    PIN_USERIO0 = 0x0100
    PIN_USERIO1 = 0x0200
    PIN_USERIO2 = 0x0400
    PIN_USERIO3 = 0x0800
    PIN_USERIO4 = 0x1000
    PIN_USERIO5 = 0x2000
    PIN_USERIO6 = 0x4000
    PIN_USERIO7 = 0x8000

    CLOCK_FPA = 0x00
    CLOCK_FPB = 0x01
    CLOCK_PLL = 0x02
    CLOCK_RTIOIN = 0x03
    CLOCK_RTIOOUT = 0x04

    GLITCH_OUT_DISABLE = 0
    GLITCH_OUT_HP = 1 << 0
    GLITCH_OUT_LP = 1 << 1
    GLITCH_OUT_BOTH = GLITCH_OUT_HP | GLITCH_OUT_LP
    GLITCH_OUT_CLR = ~GLITCH_OUT_BOTH

    GPIO_PIN_TIO1 = 0
    GPIO_PIN_TIO2 = 1
    GPIO_PIN_TIO3 = 2
    GPIO_PIN_TIO4 = 3
    GPIO_PIN_NRST = 4
    GPIO_PIN_PDID = 5
    GPIO_PIN_PDIC = 6

    # NOTE: PIN_READ_TIO* and GPIO_PIN_TIO* are 1:1
    PIN_READ_TIO1 = 0
    PIN_READ_TIO2 = 1
    PIN_READ_TIO3 = 2
    PIN_READ_TIO4 = 3
    PIN_READ_MOSI = 4
    PIN_READ_MISO = 5
    PIN_READ_PDIC = 6
    PIN_READ_PDID = 7
    PIN_READ_NRST = 8
    PIN_READ_SCK = 9

    TIO_MODE_HIGHZ = 0
    TIO_MODE_STX = 0b00000001
    TIO_MODE_SRX = 0b00000010
    TIO_MODE_USIO = 0b00000100
    TIO_MODE_USII = 0b00001000
    TIO_MODE_USINOUT = 0b00011000
    TIO_MODE_STXRX = 0b00100010
    _TIO_MODE_GPIO_MODE = 0b10000000
    _TIO_MODE_GPIO_STATE = 0b01000000
    TIO_MODE_GPIO_LOW = _TIO_MODE_GPIO_MODE
    TIO_MODE_GPIO_HIGH = _TIO_MODE_GPIO_MODE | _TIO_MODE_GPIO_STATE

    TIO_MODE_VALID = (
        ( # GPIO_PIN_TIO1
            TIO_MODE_HIGHZ,
            TIO_MODE_STX,
            TIO_MODE_SRX,
            TIO_MODE_USIO,
            TIO_MODE_USII,
            TIO_MODE_GPIO_LOW,
            TIO_MODE_GPIO_HIGH,
        ), ( # GPIO_PIN_TIO2
            TIO_MODE_HIGHZ,
            TIO_MODE_STX,
            TIO_MODE_SRX,
            TIO_MODE_USIO,
            TIO_MODE_USII,
            TIO_MODE_GPIO_LOW,
            TIO_MODE_GPIO_HIGH,
        ), ( # GPIO_PIN_TIO3
            TIO_MODE_HIGHZ,
            TIO_MODE_STX,
            TIO_MODE_SRX,
            TIO_MODE_STXRX,
            TIO_MODE_USIO,
            TIO_MODE_USII,
            TIO_MODE_USINOUT,
            TIO_MODE_GPIO_LOW,
            TIO_MODE_GPIO_HIGH,
        ), ( # GPIO_PIN_TIO4
            TIO_MODE_HIGHZ,
            TIO_MODE_STX,
            TIO_MODE_GPIO_LOW,
            TIO_MODE_GPIO_HIGH,
        )
    )

    XIO_MODE_HIGHZ = 0
    _XIO_MODE_ENABLE = 1 << 0
    _XIO_MODE_STATE = 1 << 1
    XIO_MODE_LOW = _XIO_MODE_ENABLE
    XIO_MODE_HIGH = _XIO_MODE_ENABLE | _XIO_MODE_STATE
    _XIO_MODE_MASK = XIO_MODE_HIGH

    _name = "CW Extra Settings"

    def __init__(self, oa, cwtype):

        if cwtype == "cwrev2":
            hasFPAFPB = True
            hasGlitchOut = False
            hasPLL = True
            hasAux=False
            hasUserio=False
        elif cwtype == "cwlite":
            hasFPAFPB=False
            hasGlitchOut=True
            hasPLL=False
            hasAux=False
            hasUserio=False
        elif cwtype == "cw1200":
            hasFPAFPB=False
            hasGlitchOut=True
            hasPLL=False
            hasAux=True
            hasUserio=False
        elif cwtype in ["cwhusky", "cwhusky-plus"]:
            hasFPAFPB=False
            hasGlitchOut=True
            hasPLL=False
            hasAux=True
            hasUserio=True
        else:
            raise ValueError("Unknown ChipWhisperer: %s" % cwtype)

        self.oa = oa
        self.hasFPAFPB = hasFPAFPB
        self.hasGlitchOut = hasGlitchOut
        self.hasPLL = hasPLL
        self.hasAux = hasAux
        self.hasUserio = hasUserio


        #Add special single-class items used as higher-level API
        self.gpiomux = GPIOSettings(self)
        self.triggermux = TriggerSettings(self)
        self.protrigger = ProTrigger(self)
        self.huskytrigger = HuskyTrigger(self)

        if cwtype in ["cwhusky", "cwhusky-plus"]:
            self.gpiomux._is_husky = True
            self.triggermux._is_husky = True
            self._addr_trigsrc_size = 2
            self._is_husky = True
        else:
            self._addr_trigsrc_size = 1
            self._is_husky = False

### IOROUTE Helpers

    def read_ioroute(self):
        """Reads the IOROUTE device state.

        Return:
            bytearray representing the IOROUTE state.
        """
        return self.oa.msg_read("CW_IOROUTE_ADDR", max_resp=8)

    def write_ioroute(self, data):
        """Writes a specified state to IOROUTE.
        """
        self.oa.msg_write("CW_IOROUTE_ADDR", data)

    def extr_ioroute_mask(self, i, mask):
        return self.oa.msg_extr_mask("CW_IOROUTE_ADDR", i, mask, max_resp=8)

    def test_ioroute_mask(self, i, mask):
        """Evaluates a non-zero flag for a specified byte in the IOROUTE state.

        Return:
            False if the mask evaluates to 0, else True.
        """
        return self.oa.msg_test_mask("CW_IOROUTE_ADDR", i, mask, max_resp=8)

    def set_ioroute_mask(self, i, mask):
        """OR's a mask into a specified byte in the IOROUTE state.

        Return:
            The updated IOROUTE state.
        """
        return self.oa.msg_set_mask("CW_IOROUTE_ADDR", i, mask, max_resp=8)

    def clr_ioroute_mask(self, i, mask):
        """NAND's a mask into a specified byte in the IOROUTE state.

        Return:
            The updated IOROUTE state.
        """
        return self.oa.msg_clr_mask("CW_IOROUTE_ADDR", i, mask, max_resp=8)

    def upd_ioroute_mask(self, i, mask, set):
        """Conditionally OR's or NAND's a mask into a specified byte in the IOROUTE state.

        Return:
            The updated IOROUTE state.
        """
        return self.oa.msg_upd_mask("CW_IOROUTE_ADDR", i, mask, set, max_resp=8)

    def ins_ioroute_mask(self, i, mask, value):
        """Injects a value into a specified byte in the IOROUTE state.

        Return:
            The updated IOROUTE state.
        """
        return self.oa.msg_ins_mask("CW_IOROUTE_ADDR", i, mask, value, max_resp=8)

    def get_ioroute_value(self, i):
        """Gets a specified byte from the IOROUTE state.

        Return:
            The specified IOROUTE byte.
        """
        return self.oa.msg_get_value("CW_IOROUTE_ADDR", i, max_resp=8)

    def set_ioroute_value(self, i, value):
        """Assigns a specified byte in the IOROUTE state.

        Return:
            The updated IOROUTE state.
        """
        return self.oa.msg_set_value("CW_IOROUTE_ADDR", i, value, max_resp=8)

### GPIO Settings

    def assert_tio_num(self, io_num):
        """Raises an exception if the TIO pin is invalid.
        """
        if (io_num < self.GPIO_PIN_TIO1) or (io_num > self.GPIO_PIN_TIO4):
            raise ValueError('Invalid Target IO ID: %d' % io_num)

    def assert_xio_num(self, io_num):
        """Raises an exception if the CTL pin is invalid.
        """
        if (io_num < self.GPIO_PIN_NRST) or (io_num > self.GPIO_PIN_PDIC):
            raise ValueError('Invalid Extra IO assignment ID: %d' % io_num)

    def get_tio_pin_mode(self, io_num):
        """Gets the TIO mode of the specified pin.

        Return:
            A value representing the TIO_MODE_* masks.
        """
        self.assert_tio_num(io_num)
        return self.get_ioroute_value(io_num)

    def set_tio_pin_mode(self, io_num, mode):
        """Sets the mode for the specified TIO pin.

        Return:
            The updated IOROUTE state.
        """
        #Sends actual IO mode to FPGA
        self.assert_tio_num(io_num)
        if not mode in self.TIO_MODE_VALID[io_num]:
            raise ValueError('Invalid mode (0x%x) for TargetIO[%d]' % (mode, io_num))
        return self.set_ioroute_value(io_num, mode)

    def get_xio_index(self, io_num):
        """Gets a bitmask for a valid Extra IO pin for the IROUTE state.

        Return:
            A bitmask representing the CTL IO pin.
        """
        self.assert_xio_num(io_num)
        return (io_num - self.GPIO_PIN_NRST) << 1

    def get_xio_gpio_mode(self, io_num):
        """Gets the current state of a Extra IO pin.

        Return:
            None if not driven, True if driven high, else False if driven low.
        """
        io_num = self.get_xio_index(io_num)
        d6 = self.get_ioroute_value(6) >> io_num
        if not (d6 & self._XIO_MODE_ENABLE):
            return self.XIO_MODE_HIGHZ
        return d6 & self._XIO_MODE_MASK

    def set_xio_gpio_mode(self, io_num, state):
        """Sets the mode for the specified Extra IO pin.

        Return:
            The updated IOROUTE state.
        """
        io_num = self.get_xio_index(io_num)
        if not (state & self._XIO_MODE_ENABLE):
            """NOTE: To set HIGHZ mode, must only clear ENABLE.  Clearing both (unconditionally
            setting mode to 0) seems to cause issues in some situations."""
            return self.clr_ioroute_mask(6, self._XIO_MODE_ENABLE << io_num)
        state <<= io_num
        io_num = self._XIO_MODE_MASK << io_num
        return self.ins_ioroute_mask(6, io_num, state)

### IO Pin Reader API
    def read_ioread(self, for_extra):
        if for_extra:
            if not self._is_husky:
                raise RuntimeError('Extra pins only supported with Husky!')
            for_extra = 2
        else:
            for_extra = 1
        return self.oa.msg_read("CW_IOREAD_ADDR", max_resp=for_extra)

    def read_tio_pins(self):
        """Read signal level of all 4 Target IOn pins synchronously.

        In most cases this is useful for low-speed digital input, hence the
        GPIO state of the Target IOn pin(s) used for digital input should be
        configured as 'High-Z'.

        Returns a bit mask where set bits indicate which of the 4 target IOn
        pins is read as high. Counting starts at bit 0, for example, bit0
        refers to tio1.
        """
        return self.read_ioread(False)[0]

    def read_tio_states(self):
        """Gets all the current Target IOn state values.

        Returns:
            A tuple of the pin states in integer form.
        """
        bitmask = self.read_tio_pins()
        return (
            bitmask & 1,
            (bitmask >> 1) & 1,
            (bitmask >> 2) & 1,
            (bitmask >> 3) & 1
        )

    def read_tio_pin(self, io_num):
        """Read signal level of a Target IOn pin.

        Returns True if the signal level of the Target IOn pin is high,
        otherwise False is returned.
        """
        self.assert_tio_num(io_num)
        pins = self.read_tio_pins()
        return (pins >> io_num) & 1
    
    def read_xio_pins(self):
        data = self.read_ioread(True)
        if len(data) < 2:
            return data[0]
        return util.unpack_u16(data, 0)

    def read_xio_pin(self, io_num):
        if (io_num < self.PIN_READ_MOSI) or (io_num > self.PIN_READ_SCK):
            raise ValueError('Invalid Extra IO pin read ID: %d' % io_num)
        pins = self.read_xio_pins()
        return (pins >> io_num) & 1

### Clock API

    def setClockSource(self, source):
        data = self.oa.sendMessage(CODE_READ, "CW_EXTCLK_ADDR", Validate=False, maxResp=1)
        data[0] = (data[0] & ~0x07) | source
        self.oa.sendMessage(CODE_WRITE, "CW_EXTCLK_ADDR", data)

    def clockSource(self):
        resp = self.oa.sendMessage(CODE_READ, "CW_EXTCLK_ADDR", Validate=False, maxResp=1)
        return resp[0] & 0x07

    def setTargetCLKOut(self, clkout):
        data = self.oa.sendMessage(CODE_READ, "CW_EXTCLK_ADDR", Validate=False, maxResp=1)
        data[0] = (data[0] & ~(3<<5)) | (clkout << 5)
        self.oa.sendMessage(CODE_WRITE, "CW_EXTCLK_ADDR", data)

    def targetClkOut(self):
        resp = self.oa.sendMessage(CODE_READ, "CW_EXTCLK_ADDR", Validate=False, maxResp=1)
        return ((resp[0] & (3<<5)) >> 5)

### VCC Glitch Control

    def vglitch_get_mode(self):
        """Gets a GLITCH_OUT_* bitmask indicating which VCC glitch MOSFET's are enabled.

        Return:
            A bitmask of GLITCH_OUT_HP/LP set if they are enabled.
        """
        return self.extr_ioroute_mask(4, self.GLITCH_OUT_BOTH)

    def vglitch_set_mode(self, mask):
        """Sets the VCC glitch mosfet enabled states.

        Return:
            The updated IOROUTE state.
        """
        return self.ins_ioroute_mask(4, self.GLITCH_OUT_BOTH, mask)

    def vglitch_get_hp(self):
        """Gets the enabled state for the HP glitch mosfet.

        Return:
            True if the HP glitch mosfet is enabled, else False.
        """
        return self.test_ioroute_mask(4, self.GLITCH_OUT_HP)

    def vglitch_get_lp(self):
        """Gets the enabled state for the LP glitch mosfet.

        Return:
            True if the LP glitch mosfet is enabled, else False.
        """
        return self.test_ioroute_mask(4, self.GLITCH_OUT_LP)

    def vglitch_enable(self, mask, enabled):
        """Conditionally enables or disables a GLITCH_OUT_* mask.

        Return:
            The updated IOROUTE state.
        """
        return self.upd_ioroute_mask(4, mask & self.GLITCH_OUT_BOTH, enabled)

    def vglitch_clear(self):
        """Disables both of the VCC glitch mosfets.

        Return:
            The updated IOROUTE state.
        """
        return self.clr_ioroute_mask(4, self.GLITCH_OUT_BOTH)

    def vglitch_reset(self, delay):
        """Disables and reenables the VCC glitch mosfets that were previously enabled.

        TODO: Would be nice to have this as firmware API call.

        Return:
            The updated IOROUTE state.
        """
        data = self.read_ioroute()
        d4 = data[4]

        data[4] = d4 & self.GLITCH_OUT_CLR
        self.write_ioroute(data)

        time.sleep(delay)

        data[4] = d4
        self.write_ioroute(data)
        return data

### Power Settings

    def setAVRISPMode(self, enabled):
        data = self.oa.sendMessage(CODE_READ, "CW_IOROUTE_ADDR", Validate=False, maxResp=8)
        if enabled:
            data[5] |= 0x01
        else:
            data[5] &= ~(0x01)

        self.oa.sendMessage(CODE_WRITE, "CW_IOROUTE_ADDR", data)

    def setTargetPowerState(self, enabled):
        data = self.oa.sendMessage(CODE_READ, "CW_IOROUTE_ADDR", Validate=False, maxResp=8)
        if enabled:
            data[5] &= ~(0x02)
        else:
            data[5] |= (0x02)

        self.oa.sendMessage(CODE_WRITE, "CW_IOROUTE_ADDR", data)

    def setTargetPowerSlew(self, fastmode):
        data = self.oa.sendMessage(CODE_READ, "CW_IOROUTE_ADDR", Validate=False, maxResp=8)
        if fastmode:
            data[5] |= (0x04)
        else:
            data[5] &= ~(0x04)

        self.oa.sendMessage(CODE_WRITE, "CW_IOROUTE_ADDR", data)

    def setHuskySoftPowerOnParameters(self, pwm_cycles1, pwm_cycles2, pwm_period, pwm_off_time1, pwm_off_time2):
        """Sets the soft power-on PWM parameters.

        Args:
            pwm_cycles1 (8-bit int): this plus pwm_cycles2 is the number of PWM on/off cycles before power is fully on
            pwm_cycles2 (8-bit int): this plus pwm_cycles1 is the number of PWM on/off cycles before power is fully on
            pwm_period (16-bit int): number of cycles in PWM period
            pwm_off_time1 (16-bit int): number of cycles in PWM period where power is off, for the first pwm_cycles1
        """
        if not self._is_husky:
            raise ValueError("For Husky only")
        raw = [pwm_cycles1, pwm_cycles2]
        raw.extend(list(int.to_bytes(pwm_period, length=2, byteorder='little')))
        raw.extend(list(int.to_bytes(pwm_off_time1, length=2, byteorder='little')))
        raw.extend(list(int.to_bytes(pwm_off_time2, length=2, byteorder='little')))
        self.oa.sendMessage(CODE_WRITE, "SOFTPOWER_CONTROL", raw)

    def getHuskySoftPowerOnParameters(self):
        """Get the soft power-on PWM parameters as (pwm_cycles1, pwm_cycles2, pwm_period, pwm_off_time1, pwm_off_time2)
        """
        if not self._is_husky:
            raise ValueError("For Husky only")
        raw = self.oa.sendMessage(CODE_READ, "SOFTPOWER_CONTROL", Validate=False, maxResp=8)
        pwm_cycles1 = raw[0]
        pwm_cycles2 = raw[1]
        pwm_period = int.from_bytes(raw[2:4], byteorder='little')
        pwm_off_time1 = int.from_bytes(raw[4:6], byteorder='little')
        pwm_off_time2 = int.from_bytes(raw[6:8], byteorder='little')
        return (pwm_cycles1, pwm_cycles2, pwm_period, pwm_off_time1, pwm_off_time2)

    def getTargetPowerState(self):
        data = self.oa.sendMessage(CODE_READ, "CW_IOROUTE_ADDR", Validate=False, maxResp=8)
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
        d = list(int.to_bytes((mode << 6) | pins, length=self._addr_trigsrc_size, byteorder='little'))
        self.oa.sendMessage(CODE_WRITE, "CW_TRIGSRC_ADDR", d, maxResp=self._addr_trigsrc_size)

    def getPins(self):
        resp = self.oa.sendMessage(CODE_READ, "CW_TRIGSRC_ADDR", Validate=False, maxResp=self._addr_trigsrc_size)
        pins, mode = self.raw2pins(resp)
        return(pins, mode)

    def raw2pins(self, raw):
        pins = raw[0] & 0x3F
        if self._addr_trigsrc_size == 2:
            pins += (raw[1] << 8)
        mode = raw[0] >> 6
        return(pins, mode)


    def setTriggerModule(self, module):
        #When using special modes, force rising edge & stop user from easily changing
        resp = self.oa.sendMessage(CODE_READ, "CW_TRIGMOD_ADDR", Validate=False, maxResp=1)
        resp[0] &= 0xF8
        resp[0] |= module
        self.oa.sendMessage(CODE_WRITE, "CW_TRIGMOD_ADDR", resp)

    def getTriggerModule(self):
        resp = self.oa.sendMessage(CODE_READ, "CW_TRIGMOD_ADDR", Validate=False, maxResp=1)
        return resp[0]

    def setTrigOutAux(self, enabled):
        resp = self.oa.sendMessage(CODE_READ, "CW_TRIGMOD_ADDR", Validate=False, maxResp=1)
        resp[0] &= 0xE7
        if enabled:
            resp[0] |= 0x08
        self.oa.sendMessage(CODE_WRITE, "CW_TRIGMOD_ADDR", resp)

    def setTrigOut(self, enabled):
        resp = self.oa.sendMessage(CODE_READ, "CW_TRIGMOD_ADDR", Validate=False, maxResp=1)
        resp[0] &= 0xE7
        if enabled:
            resp[0] |= 0x08
        self.oa.sendMessage(CODE_WRITE, "CW_TRIGMOD_ADDR", resp)

    def getTrigOut(self):
        resp = self.oa.sendMessage(CODE_READ, "CW_TRIGMOD_ADDR", Validate=False, maxResp=1)
        return resp[0] & 0x08

class GPIOSettings(util.DisableNewAttr):
    VCC_GLITCHT_DISABLED = CWExtraSettings.GLITCH_OUT_DISABLE
    VCC_GLITCHT_HP = CWExtraSettings.GLITCH_OUT_HP
    VCC_GLITCHT_LP = CWExtraSettings.GLITCH_OUT_LP
    VCC_GLITCHT_BOTH = CWExtraSettings.GLITCH_OUT_BOTH

    # NOTE: Technically, VCC_GLITCHT is a bit field, but in this situation, it can also be used as a 4 value enum.
    VCC_GLITCHT_TRANSLATE = util.EnumTranslationDirect.alloc_instance(
        (
            'disabled',
            'hp',
            'lp',
            'both',
        ), {
            None: VCC_GLITCHT_DISABLED,
        }
    )

    GPIO_PIN_TIO1 = CWExtraSettings.GPIO_PIN_TIO1
    GPIO_PIN_TIO2 = CWExtraSettings.GPIO_PIN_TIO2
    GPIO_PIN_TIO3 = CWExtraSettings.GPIO_PIN_TIO3
    GPIO_PIN_TIO4 = CWExtraSettings.GPIO_PIN_TIO4
    GPIO_PIN_NRST = CWExtraSettings.GPIO_PIN_NRST
    GPIO_PIN_PDID = CWExtraSettings.GPIO_PIN_PDID
    GPIO_PIN_PDIC = CWExtraSettings.GPIO_PIN_PDIC

    # NOTE: GPIO LOW/HIGH *MUST* be 0/1 respectively since bool is subclass of int
    GPIO_MODE_LOW = 0
    GPIO_MODE_HIGH = 1
    GPIO_MODE_HIGHZ = 2
    GPIO_MODE_SERIAL_TX = 3
    GPIO_MODE_SERIAL_RX = 4
    GPIO_MODE_SERIAL_IO = 5

    """NOTE: There doesn't seem to be a "true" disabled setting.  That was just used in previous
    logic to ignore writing GPIO state.  Does it make sense to set disabled to HIGHZ instead of
    GPIO LOW?
    """
    TIO_MODE_TRANSLATE = util.EnumTranslationAPI.alloc_instance(
        (
            CWExtraSettings.TIO_MODE_GPIO_LOW,
            CWExtraSettings.TIO_MODE_GPIO_HIGH,
            CWExtraSettings.TIO_MODE_HIGHZ,
            CWExtraSettings.TIO_MODE_STX,
            CWExtraSettings.TIO_MODE_SRX,
            CWExtraSettings.TIO_MODE_STXRX,
        ), (
            'gpio_low',
            'gpio_high',
            'high_z',
            'serial_tx',
            'serial_rx',
            'serial_tx_rx',
        ), {
            'gpio_disabled': GPIO_MODE_LOW,
            None: GPIO_MODE_HIGHZ,
            False: GPIO_MODE_LOW,
            True: GPIO_MODE_HIGH,
        }
    )

    XIO_MODE_TRANSLATE = util.EnumTranslationAPI.alloc_instance(
        (
            CWExtraSettings.XIO_MODE_LOW,
            CWExtraSettings.XIO_MODE_HIGH,
            CWExtraSettings.XIO_MODE_HIGHZ,
        ), (
            'low',
            'high',
            'high_z',
        ), {
            'disabled': GPIO_MODE_HIGHZ,
            'default': GPIO_MODE_HIGHZ,
            None: GPIO_MODE_HIGHZ,
            False: GPIO_MODE_LOW,
            True: GPIO_MODE_HIGH,
        }
    )

    def __init__(self, cwextra):
        super().__init__()
        self.cwe = cwextra

        self.HS2_VALID = {'disabled': 0, 'clkgen': 2, 'glitch': 3}
        self._is_husky = False

        self.disable_newattr()

    def __repr__(self):
        return util.dict_to_str(self._dict_repr())

    def __str__(self):
        return self.__repr__()

    def _dict_repr(self):
        rtn = {}
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

        if self._is_husky:
            rtn['aux_io_mcx'] = self.aux_io_mcx
            rtn['glitch_trig_mcx'] = self.glitch_trig_mcx

        return rtn

    @property
    def tio_state_mask(self):
        return self.cwe.read_tio_pins()

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

    def read_tio_states(self):
        return self.cwe.read_tio_states()

    @property
    def tio1_mode(self):
        return self._get_tio_mode(self.GPIO_PIN_TIO1)

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

        :Getter:  Return one of the above strings. This shows how ChipWhisperer is 
                driving this pin; it does not show its actual logic level. Use
                scope.io.tio_states to see the actual logic level.

        :Setter: Set the Target IO1 mode.

        Raises:
           ValueError: if new value is not one of the above modes

        """
        return self._get_tio_str(self.GPIO_PIN_TIO1)

    @tio1.setter
    def tio1(self, mode):
        self._set_tio_mode(self.GPIO_PIN_TIO1, mode)

    @property
    def tio1_state(self):
        return self._read_tio_pin(self.cwe.PIN_READ_TIO1)

    @property
    def tio2_mode(self):
        return self._get_tio_mode(self.GPIO_PIN_TIO2)

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

        :Getter:  Return one of the above strings. This shows how ChipWhisperer is 
                driving this pin; it does not show its actual logic level. Use
                scope.io.tio_states to see the actual logic level.

        :Setter: Set the Target IO2 mode.

        Raises:
           ValueError: if new value is not one of the above modes
        """
        return self._get_tio_str(self.GPIO_PIN_TIO2)

    @tio2.setter
    def tio2(self, mode):
        self._set_tio_mode(self.GPIO_PIN_TIO2, mode)

    @property
    def tio2_state(self):
        return self._read_tio_pin(self.cwe.PIN_READ_TIO2)

    @property
    def tio3_mode(self):
        return self._get_tio_mode(self.GPIO_PIN_TIO3)

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

        :Getter:  Return one of the above strings. This shows how ChipWhisperer is 
                driving this pin; it does not show its actual logic level. Use
                scope.io.tio_states to see the actual logic level.

        :Setter: Set the Target IO3 mode.

        Raises:
           ValueError: if new value is not one of the above modes
        """
        return self._get_tio_str(self.GPIO_PIN_TIO3)

    @tio3.setter
    def tio3(self, mode):
        self._set_tio_mode(self.GPIO_PIN_TIO3, mode)

    @property
    def tio3_state(self):
        return self._read_tio_pin(self.cwe.PIN_READ_TIO3)

    @property
    def tio4_mode(self):
        return self._get_tio_mode(self.GPIO_PIN_TIO4)

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

        :Getter:  Return one of the above strings. This shows how ChipWhisperer is 
                driving this pin; it does not show its actual logic level. Use
                scope.io.tio_states to see the actual logic level.

        :Setter: Set the Target IO4 mode

        Raises:
           ValueError: if new value is not one of the above modes
        """
        return self._get_tio_str(self.GPIO_PIN_TIO4)

    @tio4.setter
    def tio4(self, mode):
        self._set_tio_mode(self.GPIO_PIN_TIO4, mode)

    @property
    def tio4_state(self):
        return self._read_tio_pin(self.cwe.PIN_READ_TIO4)

    def _is_valid_tio_mode(self, mode):
        return self.TIO_MODE_TRANSLATE.is_valid_api(mode)

    def _get_tio_mode(self, tio_num):
        mode = self.cwe.get_tio_pin_mode(tio_num)
        value = self.TIO_MODE_TRANSLATE.try_hw_to_api(mode)
        if not self._is_valid_tio_mode(value):
            raise IOError('Invalid TIO mode returned by FPGA: 0x%x', mode)
        return value

    def _get_tio_str(self, tio_num):
        """Internal function to read the current mode of a TIO pin.

        Return:
            The API string that represents the TIO GPIO mode.
        """
        mode = self._get_tio_mode(tio_num)
        return self.TIO_MODE_TRANSLATE.api_to_str(mode)

    def _set_tio_mode(self, tio_num, mode):
        """Internal function to set the current mode of a TIO pin.

        Return:
            The updated IOROUTE state.
        """
        value = self.TIO_MODE_TRANSLATE.try_var_to_api(mode)
        if not self._is_valid_tio_mode(value):
            raise ValueError('%s is not a valid TIO mode!' % mode)
        value = self.TIO_MODE_TRANSLATE.api_to_hw(value)
        return self.cwe.set_tio_pin_mode(tio_num, value)

    def _read_tio_pin(self, tio_num):
        """Reads the current value of a specified TIO* pin.

        Return:
            True if the pin is high, else False if the pin is low.
        """
        return self.cwe.read_tio_pin(tio_num)

    @property
    def pdic_mode(self):
        return self._get_xio_mode(self.GPIO_PIN_PDIC)

    @property
    def pdic(self):
        """The function of the PDIC pin output pin.

        This is a GPIO pin. The following values are allowed:
         * "high" / True: logic 1
         * "low" / False: logic 0
         * "disabled" / "default" / "high_z" / None: undriven

        :Getter:  Return one of "high", "low", or "high_z". This shows how ChipWhisperer
                is driving this pin; it does not show its actual logic level.

        :Setter: Set the pin's state

        Raises:
        ValueError: if new state not listed above
        """
        return self._get_xio_str(self.GPIO_PIN_PDIC)

    @pdic.setter
    def pdic(self, mode):
        self._set_xio_mode(self.GPIO_PIN_PDIC, mode)

    @property
    def pdic_state(self):
        """ Reads the logic level of the PDIC pin. Supported by Husky only.
        """
        return self._read_xio_pin(self.cwe.PIN_READ_PDIC)

    @property
    def pdid_mode(self):
        return self._get_xio_mode(self.GPIO_PIN_PDID)

    @property
    def pdid(self):
        """The state of the PDID pin.

        See pdic for more information."""
        return self._get_xio_str(self.GPIO_PIN_PDID)

    @pdid.setter
    def pdid(self, mode):
        self._set_xio_mode(self.GPIO_PIN_PDID, mode)

    @property
    def pdid_state(self):
        """ Reads the logic level of the PDID pin. Supported by Husky only.
        """
        return self._read_xio_pin(self.cwe.PIN_READ_PDID)

    @property
    def nrst_mode(self):
        return self._get_xio_mode(self.GPIO_PIN_NRST)

    @property
    def nrst(self):
        """The state of the NRST pin.

        See pdic for more information.
        See also :class:`nrst_drive_poweroff` for behaviour with :class:`target_pwr`.

        """
        return self._get_xio_str(self.GPIO_PIN_NRST)

    @nrst.setter
    def nrst(self, mode):
        self._set_xio_mode(self.GPIO_PIN_NRST, mode)

    @property
    def nrst_state(self):
        """ Reads the logic level of the nRST pin. Supported by Husky only.
        """
        return self._read_xio_pin(self.cwe.PIN_READ_NRST)

    @property
    def nrst_drive_poweroff(self):
        """Control how the nRST pin is driven when :class:`target_pwr` is
        False.  If set, nRST is driven (or not) as per :class:`nrst` (i.e.
        :class:`target_pwr` has no effect on how nRST is driven). Otherwise,
        nRST is tristated. 

        .. NOTE:: our target boards pull up nRST to `target_pwr`; if 
            `nrst_drive_poweroff` is False and nRST is driven low, then setting
            `target_pwr` to False will cause nRST to pulse as it gets tristated
            and pulled up for a brief moment until the target power gets
            sufficiently low to return it to low.

        """
        return self.cwe.test_ioroute_mask(6, 2**6)

    @nrst_drive_poweroff.setter
    def nrst_drive_poweroff(self, mode):
        self.cwe.upd_ioroute_mask(6, 2**6, mode)

    @property
    def miso_state(self):
        """ Reads the logic level of the MISO pin. Supported by Husky only.
        """
        return self._read_xio_pin(self.cwe.PIN_READ_MISO)

    @property
    def mosi_state(self):
        """ Reads the logic level of the MOSI pin. Supported by Husky only.
        """
        return self._read_xio_pin(self.cwe.PIN_READ_MOSI)

    @property
    def sck_state(self):
        """ Reads the logic level of the SCK pin. Supported by Husky only.
        """
        return self._read_xio_pin(self.cwe.PIN_READ_SCK)

    def _is_valid_xio_mode(self, mode):
        return self.XIO_MODE_TRANSLATE.is_valid_api(mode)

    def _get_xio_mode(self, xio_pin):
        mode = self.cwe.get_xio_gpio_mode(xio_pin)
        value = self.XIO_MODE_TRANSLATE.try_hw_to_api(mode)
        if not self._is_valid_xio_mode(value):
            raise ValueError('HW API returned invalid state: %s' % mode)
        return value

    def _get_xio_str(self, xio_pin):
        """Gets the current IO mode for extra pins.

        Return:
            'high_z' if not driven, 'high' if driven high, else 'low' if driven low.
        """
        mode = self._get_xio_mode(xio_pin)
        return self.XIO_MODE_TRANSLATE.api_to_str(mode)

    def _set_xio_mode(self, xio_pin, mode):
        """Sets the current IO mode for extra pins.

        Return:
            The updated IOROUTE state.
        """
        value = self.XIO_MODE_TRANSLATE.try_var_to_api(mode)
        if not self._is_valid_xio_mode(value):
            raise ValueError('%s is not a valid mode' % mode)
        mode = self.XIO_MODE_TRANSLATE.api_to_hw(value)
        return self.cwe.set_xio_gpio_mode(xio_pin, mode)

    def _read_xio_pin(self, read_pin):
        return self.cwe.read_xio_pin(read_pin);

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
    def aux_io_mcx(self):
        """Set the function of the AUX I/O MCX on Husky.

        Options:

        * "high_z": input: to use as a trigger (scope.trigger.triggers = 'aux') or clock (scope.clock.clkgen_src = 'extclk_aux_io').
        * "hs2": output: provide the same clock that's on HS2.
        """
        if not self._is_husky:
            raise ValueError("For CW-Husky only.")
        data = self.cwe.oa.sendMessage(CODE_READ, "CW_AUX_IO", Validate=False, maxResp=1)[0]
        if data & 0x01:
            return "hs2"
        else:
            return "high_z"

    @aux_io_mcx.setter
    def aux_io_mcx(self, state):
        if not self._is_husky:
            raise ValueError("For CW-Husky only.")
        data = self.cwe.oa.sendMessage(CODE_READ, "CW_AUX_IO", Validate=False, maxResp=1)[0]
        if state == 'high_z':
            data &= 0xfe
        elif state == 'hs2':
            data |= 0x01
        else:
            raise ValueError("Options: high_z, hs2")
        return self.cwe.oa.sendMessage(CODE_WRITE, "CW_AUX_IO", [data])

    @property
    def glitch_trig_mcx(self):
        """Set the function of the Trigger/Glitch Out MCX on Husky.
        Options:

        * "glitch": glitch output (clock or voltage glitch signal, as defined by scope.glitch settings)
        * "trigger": internal trigger signal (as defined by scope.trigger)
        * "inverted [glitch | trigger]": inverted glitch or trigger signal
        """
        if not self._is_husky:
            raise ValueError("For CW-Husky only.")
        data = self.cwe.oa.sendMessage(CODE_READ, "CW_AUX_IO", Validate=False, maxResp=1)[0]
        if data & 0x04:
            setting = 'inverted '
        else:
            setting = ''
        if data & 0x02:
            setting += "glitch"
        else:
            setting += "trigger"
        return setting

    @glitch_trig_mcx.setter
    def glitch_trig_mcx(self, state):
        if not self._is_husky:
            raise ValueError("For CW-Husky only.")
        data = self.cwe.oa.sendMessage(CODE_READ, "CW_AUX_IO", Validate=False, maxResp=1)[0]
        if 'trigger' in state:
            data &= 0xfd
        elif 'glitch' in state:
            data |= 0x02
        else:
            raise ValueError("Options: glitch, trig")
        if 'inverted' in state:
            data |= 0x04
        else:
            data &= 0xfb
        self.cwe.oa.sendMessage(CODE_WRITE, "CW_AUX_IO", [data])

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

        .. NOTE:: Setting this value to False/0 also sets the following to high-z
            until power is reenabled: TIO1-4, HS2, PDID, PDIC, MOSI, SCK.
            nRST is also tristated if :class:`nrst_drive_poweroff` is False.

        :Getter:  Return the current power state of the target (True or False)

        :Setter: Turn the target power on or off.
        """
        return self.cwe.getTargetPowerState()

    @target_pwr.setter
    def target_pwr(self, power):
        self.cwe.setTargetPowerState(power)

    @property
    def vcc_glitcht(self):
        """Gets a bitmask indicating which VCC glitch MOSFET's are enabled, based on the
        VCC_GLITCHT_* consts.

        Return:
            A bitmask with the VCC_GLITCHT_HP/LP set if enabled.
        """
        return self.cwe.vglitch_get_mode()

    @vcc_glitcht.setter
    def vcc_glitcht(self, mode):
        glitcht = self.VCC_GLITCHT_TRANSLATE.try_var_to_api(mode)
        if not self.VCC_GLITCHT_TRANSLATE.is_valid_api(glitcht):
            raise ValueError('%s is an invalid glitch mode!' % mode)
        self.cwe.vglitch_set_mode(glitcht)

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
        return self.cwe.vglitch_get_hp()

    @glitch_hp.setter
    def glitch_hp(self, active):
        self.cwe.vglitch_enable(CWExtraSettings.GLITCH_OUT_HP, active)

    @property
    def glitch_lp(self):
        """Whether the low-power crowbar MOSFET is enabled.

        This is the low-power version of glitch_hp - see that documentation
        for more details.

        .. warning:: Use with caution - ensure that the glitch module is properly
            configured before enabling this setting, as it is possible to
            permanently damage hardware with this output.
        """
        return self.cwe.vglitch_get_lp()

    @glitch_lp.setter
    def glitch_lp(self, active):
        self.cwe.vglitch_enable(CWExtraSettings.GLITCH_OUT_LP, active)

    def vglitch_disable(self):
        """Disables both glitch mosfets.
        """
        self.cwe.vglitch_clear()

    def vglitch_reset(self, delay=0.005):
        """Disables and reenables the glitch mosfets that were previously enabled.
        """
        self.cwe.vglitch_reset(delay)

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
            'tio1': self.cwe.PIN_RTIO1,
            'tio2': self.cwe.PIN_RTIO2,
            'tio3': self.cwe.PIN_RTIO3,
            'tio4': self.cwe.PIN_RTIO4,
            'nrst': self.cwe.PIN_TNRST,
        }

        self.last_module = "basic"
        if self.cwe.hasAux:
            self.supported_tpins['sma'] = self.cwe.PIN_FPA
            self.supported_tpins['aux'] = self.cwe.PIN_FPA # alias for Husky since it's labeled 'Aux' on the sticker

        if self.cwe.hasUserio:
            self.supported_tpins['userio_d0'] = self.cwe.PIN_USERIO0
            self.supported_tpins['userio_d1'] = self.cwe.PIN_USERIO1
            self.supported_tpins['userio_d2'] = self.cwe.PIN_USERIO2
            self.supported_tpins['userio_d3'] = self.cwe.PIN_USERIO3
            self.supported_tpins['userio_d4'] = self.cwe.PIN_USERIO4
            self.supported_tpins['userio_d5'] = self.cwe.PIN_USERIO5
            self.supported_tpins['userio_d6'] = self.cwe.PIN_USERIO6
            self.supported_tpins['userio_d7'] = self.cwe.PIN_USERIO7


        self._is_husky = False

        self.disable_newattr()

    def _dict_repr(self):
        rtn = {}
        rtn['triggers'] = self.triggers
        rtn['module'] = self.module

        return rtn

    def __repr__(self):
        return util.dict_to_str(self._dict_repr())

    def __str__(self):
        return self.__repr__()

    def _trigger_value2string(self, pins, mode):
        """Takes the raw programmed "CW_TRIGSRC_ADDR" register value and
        converts its meaning into a human-readable string.

        Args:
            pins, mode: as returned by getPins()

        Returns:
            String describing the trigger input(s).
        """
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

        if pins & self.cwe.PIN_USERIO0:
            tstring.append("userio_d0")
            tstring.append(modes)

        if pins & self.cwe.PIN_USERIO1:
            tstring.append("userio_d1")
            tstring.append(modes)

        if pins & self.cwe.PIN_USERIO2:
            tstring.append("userio_d2")
            tstring.append(modes)

        if pins & self.cwe.PIN_USERIO3:
            tstring.append("userio_d3")
            tstring.append(modes)

        if pins & self.cwe.PIN_USERIO4:
            tstring.append("userio_d4")
            tstring.append(modes)

        if pins & self.cwe.PIN_USERIO5:
            tstring.append("userio_d5")
            tstring.append(modes)

        if pins & self.cwe.PIN_USERIO6:
            tstring.append("userio_d6")
            tstring.append(modes)

        if pins & self.cwe.PIN_USERIO7:
            tstring.append("userio_d7")
            tstring.append(modes)

        #Remove last useless combination mode
        if len(tstring) > 1:
            tstring = tstring[0:-1]

        #Return a string indicating trigger mode
        return " ".join(tstring)

    @property
    def triggers(self):
        """The logical input into the trigger module.

        The trigger module uses some combination of the scope's I/O pins to
        produce a single value, which it uses for edge/level detection or UART
        triggers. This trigger output can combine 5 pins using one of 3
        different boolean operations. N/A for 'trace' trigger module (see
        scope.trace.trace_mode for how to connect trace pins.)

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
        return self.getTriggers()

    @triggers.setter
    def triggers(self, s):
        self.setTriggers(s)

    def getTriggers(self):
        if self._is_husky and self.sequencer_enabled:
            message = self.getMultipleTriggers()
        else:
            pins, mode = self.cwe.getPins()
            message = self._trigger_value2string(pins, mode)
        return message

    def _trigger_string2value(self, s):
        """Takes human-readable trigger description and returns the
        inputs required by setPins().
        """

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

        return (enablelogic, mode)


    def setTriggers(self, s):
        if self._is_husky and self.sequencer_enabled:
            self.setMultipleTriggers(s)
        else:
            enablelogic, mode = self._trigger_string2value(s)
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
    MODULE = {'basic':          0x00,
              'SAD':            0x02,
              'DECODEIO':       0x03
              }

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

        :Getter: Return the active trigger module

        :Setter: Sets the active trigger module

        Raises:
            ValueError: module isn't one of the available strings
        """
        return self.last_module

    @module.setter
    def module(self, mode):
        if mode in self.MODULE.keys():
            module = self.MODULE[mode]
        else:
            msg = 'Invalid mode %s. Must be one of: ' % mode
            for key in self.MODULE.keys():
                msg = msg + key + ', '
            raise ValueError(msg)
        resp = self.cwe.oa.sendMessage(CODE_READ, "CW_TRIGMOD_ADDR",
                                       Validate=False, maxResp=1)
        resp[0] &= 0xF8
        resp[0] |= module
        resp = self.cwe.oa.sendMessage(CODE_WRITE, "CW_TRIGMOD_ADDR",
                                       resp)
        self.last_module = mode


    @property
    def aux_out(self):
        """Controls AUX out on the CWPro

        CWPro only

        :Getter: Returns True for 'trigger', 'glitch' for 'glitch', 'clock' for 'clock' or False for no output.

        :Setter: Set False or 0 to disable, True or :code:`'trigger'` for trig_out,
                :code:`'glitch'` for glitch out, or :code:`'clock'` for clock_out
        """
        # resp1 = self.cwe.oa.sendMessage(CODE_READ, "CW_EXTCLK_ADDR", Validate=False, maxResp=1)
        resp = self.cwe.oa.sendMessage(CODE_READ, "CW_TRIGMOD_ADDR", Validate=False, maxResp=1)
        resp2 = self.cwe.oa.sendMessage(CODE_READ, "CW_EXTCLK_ADDR", Validate=False, maxResp=1)


        if (resp[0] & 0x08):
            return True
        elif resp2[0] & 0x10:
            return "glitch"
        elif resp2[0] & 0x08:
            return "clock"
        else:
            return False

    @aux_out.setter
    def aux_out(self, enabled):
        if enabled is True:
            enabled = "trigger"
        
        resp = self.cwe.oa.sendMessage(CODE_READ, "CW_TRIGMOD_ADDR", Validate=False, maxResp=1)
        resp2 = self.cwe.oa.sendMessage(CODE_READ, "CW_EXTCLK_ADDR", Validate=False, maxResp=1)
        resp2[0] &= 0xE7
        resp[0] &= 0xE7
        if enabled == "trigger":
            resp[0] |= 0x08
        elif enabled == "glitch":
            resp2[0] |= 0x10
        elif enabled == "clock":
            resp2[0] |= 0x08
        self.cwe.oa.sendMessage(CODE_WRITE, "CW_TRIGMOD_ADDR", resp)
        self.cwe.oa.sendMessage(CODE_WRITE, "CW_EXTCLK_ADDR", resp2)


class SequenceTriggerList(list):
    """Class that behaves like a list, but can set individual elements using a getter/setter

    Useful so that we can do scope.trigger.<property>[<index>] = <value> with Husky sequenced triggers
    """
    def __setitem__(self, *args, **kwargs):
        oldval = self._getter()
        oldval[args[0]] = args[1]
        self._setter(oldval)
        pass

    def __repr__(self):
        oldrepr = super().__repr__()
        return f"SequenceTriggerList({oldrepr})"

    def __init__(self, *args, **kwargs):
        if "getter" not in kwargs:
            raise KeyError("SequenceTriggerList requires a getter")
        if "setter" not in kwargs:
            raise KeyError("SequenceTriggerList requires a setter")
        
        self._getter = kwargs.pop("getter")
        self._setter = kwargs.pop("setter")
        super().__init__(*args, **kwargs)


class HuskyTrigger(TriggerSettings):
    """Husky trigger object.
    Communicates with all the trigger modules inside CW-Husky.
    Usage depends on the active trigger module.
    """
    MODULE = {'basic':          0x00,
              'advpattern':     0x01,
              'SAD':            0x02,
              'UART':           0x03,
              'trace':          0x04,
              'ADC':            0x05,
              'edge_counter':   0x06
              }

    def __init__(self, cwextra):
        self._edges = 1
        self._window_bytes = 2
        super().__init__(cwextra)
        self._is_husky = True

    def _dict_repr(self):
        rtn = {}
        rtn['sequencer_enabled'] = self.sequencer_enabled
        if self.sequencer_enabled:
            rtn['max_sequenced_triggers'] = self.max_sequenced_triggers
            rtn['num_triggers'] = self.num_triggers
            rtn['sad_always_active'] = self.sad_always_active
        rtn['module'] = self.module
        if self.sequencer_enabled:
            rtn['triggers'] = self.triggers
            rtn['window_start'] = self.window_start
            rtn['window_end'] = self.window_end
            for i in range(self.num_triggers):
                seq_trig_rtn = {}
                seq_trig_rtn['module'] = self.module[i]
                if self.module[i] == 'ADC':
                    seq_trig_rtn['level'] = self.level
                if self.module[i] in ['basic', 'UART', 'edge_counter']:
                    seq_trig_rtn['triggers'] = self.triggers[i]
                if self.module[i] == 'edge_counter':
                    seq_trig_rtn['edges'] = self.edges
                if i > 0:
                    seq_trig_rtn['window_start'] = self.window_start[i-1]
                    seq_trig_rtn['window_end'] = self.window_end[i-1]
                rtn['sequence trigger #%d' % i] = seq_trig_rtn

        else:
            if self.module == 'ADC':
                rtn['level'] = self.level
            if self.module in ['basic', 'UART', 'edge_counter']:
                rtn['triggers'] = self.triggers
            if self.module == 'edge_counter':
                rtn['edges'] = self.edges
        return rtn

    def readMultipleTriggers(self):
        message = []
        raw = self.cwe.oa.sendMessage(CODE_READ, "CW_TRIGSRC_ADDR", Validate=False, maxResp=self.cwe._addr_trigsrc_size*self.max_sequenced_triggers)
        for i in range(0, len(raw), self.cwe._addr_trigsrc_size):
            pins, mode = self.cwe.raw2pins(raw[i:i+2])
            message.append(self._trigger_value2string(pins, mode))
        return message

    def getMultipleTriggers(self):
        triggers = self.readMultipleTriggers()
        if type(triggers) is str:
            return triggers
        else:
            return SequenceTriggerList(triggers, setter=self.setMultipleTriggers, getter=self.readMultipleTriggers)

    def setMultipleTriggers(self, triggers):
        msg = []
        for s in triggers:
            pins, mode = self._trigger_string2value(s)
            d = list(int.to_bytes((mode << 6) | pins, length=self.cwe._addr_trigsrc_size, byteorder='little'))
            msg.extend(d)
        self.cwe.oa.sendMessage(CODE_WRITE, "CW_TRIGSRC_ADDR", msg, maxResp=len(msg))

    @property
    def max_sequenced_triggers(self):
        """Maximum number of triggers in the trigger sequence.
        """
        return self.cwe.oa.sendMessage(CODE_READ, "SEQ_TRIGGERS_CONFIG", Validate=False, maxResp=2)[1]


    @property
    def num_triggers(self):
        """Number of triggers in the trigger sequence.
        """
        raw = self.cwe.oa.sendMessage(CODE_READ, "SEQ_TRIGGERS_CONFIG", Validate=False, maxResp=1)[0]
        return (raw & 0x0f) + 1

    @num_triggers.setter
    def num_triggers(self, num):
        if num < 2 or num > self.max_sequenced_triggers:
            raise ValueError('Minimum 2, maximum %d' % self.max_sequenced_triggers)
        else:
            raw = self.cwe.oa.sendMessage(CODE_READ, "SEQ_TRIGGERS_CONFIG", Validate=False, maxResp=1)[0]
            raw = (raw & 0xf0) + (num-1)
            self.cwe.oa.sendMessage(CODE_WRITE, "SEQ_TRIGGERS_CONFIG", [raw])

    @property
    def sad_always_active(self):
        """Make the SAD trigger module always active. Its associated trigger
        window will still be used: only SAD triggers inside its window will be
        recognized as part of the trigger sequence, but setting this allows the
        SAD module to fire outside of its window, which can be helpful in
        tuning SAD parameters.
        """
        raw = self.cwe.oa.sendMessage(CODE_READ, "SEQ_TRIGGERS_CONFIG", Validate=False, maxResp=1)[0]
        if raw & 0x40:
            return True
        else:
            return False


    @sad_always_active.setter
    def sad_always_active(self, enable):
        raw = self.cwe.oa.sendMessage(CODE_READ, "SEQ_TRIGGERS_CONFIG", Validate=False, maxResp=1)[0]
        if enable:
            raw = (raw & 0b10111111) | 0x40
        else:
            raw = raw & 0b10111111
        self.cwe.oa.sendMessage(CODE_WRITE, "SEQ_TRIGGERS_CONFIG", [raw])


    @property
    def module(self):
        """The trigger module in use.

        The trigger modules available depend on the hardware. On the CWLite,
        only the basic trigger module can be used; on the CW1200, the serial
        data and SAD triggers are available too.

        Available trigger modules:
         * 'basic':        Trigger on a logic level or edge
         * 'ADC':          Trigger on ADC sample exceeding a threshold
         * 'SAD':          Trigger from SAD module
         * 'UART':         Trigger from UART module
         * 'edge_counter': Trigger after a number of rising/falling edges
         * 'trace':        Trigger from TraceWhisperer

        :Getter: Return the active trigger module

        :Setter: Sets the active trigger module

        Raises:
            ValueError: module isn't one of the available strings
        """
        return self.getModule()


    @module.setter
    def module(self, mode):
        self.setModule(mode)


    def readModule(self):
        resp = self.cwe.oa.sendMessage(CODE_READ, "CW_TRIGMOD_ADDR", Validate=False, maxResp=self.max_sequenced_triggers)
        modules = []
        for r in resp:
            module = None
            for key,value in self.MODULE.items():
                if value == r:
                    module = key
                    break
            if not module:
                raise ValueError('Internal error: unknown trigger module ID %d' % r)
            modules.append(module)
        if self.sequencer_enabled:
            return modules
        else:
            return modules[0]

    def getModule(self):
        modules = self.readModule()
        if type(modules) is str:
            return modules
        else:
            return SequenceTriggerList(modules, setter=self.setModule, getter=self.readModule)


    def setModule(self, modes):
        if type(modes) != list:
            modes = [modes]
        # first things first, check that it's legal: only the 'basic' module can be used multiple times, and the
        # trace/UART module can only be used once (trace or UART but not both)
        if self.sequencer_enabled:
            if len(modes) < self.num_triggers:
                new_modes = self.module
                for i,m in enumerate(modes):
                    new_modes[i] = m
            else:
                new_modes = modes
            modes_used = {}
            for m in new_modes:
                if m in ['trace', 'UART']:
                    m = 'trace/UART'
                if m in modes_used.keys():
                    modes_used[m] += 1
                else:
                    modes_used[m] = 1
            for m in modes_used.keys():
                if m != 'basic' and modes_used[m] > 1:
                    raise ValueError('Module %s can only be used once in a trigger sequence.' % m)

        modules = []
        for i, mode in enumerate(modes):
            if mode in self.MODULE.keys():
                module = self.MODULE[mode]
                # for UART and edge, the trigger line must be specified separately to the FPGA:
                if mode in ['UART', 'edge_counter']:
                    raw = self.cwe.oa.sendMessage(CODE_READ, "SEQ_TRIGGERS_UART_EDGE_CHOOSER", Validate=False, maxResp=1)[0]
                    if i > 0xf:
                        raise ValueError('Internal error, too many triggers!')
                    if mode == 'UART':
                        raw = raw & 0x0f | (i << 4)
                    else:
                        raw = raw & 0xf0 | i
                    self.cwe.oa.sendMessage(CODE_WRITE, "SEQ_TRIGGERS_UART_EDGE_CHOOSER", [raw])
            else:
                msg = 'Invalid mode %s. Must be one of: ' % mode
                for key in self.MODULE.keys():
                    msg = msg + key + ', '
                raise ValueError(msg)
            modules.append(module)
        self.cwe.oa.sendMessage(CODE_WRITE, "CW_TRIGMOD_ADDR", modules)

    @property
    def sequencer_enabled(self):
        """Enable the trigger sequencer.
        """
        raw = self.cwe.oa.sendMessage(CODE_READ, "SEQ_TRIGGERS_CONFIG", Validate=False, maxResp=1)[0]
        if raw & 0x80:
            return True
        else:
            return False


    @sequencer_enabled.setter
    def sequencer_enabled(self, enable):
        # MSB is enable bit; LSB is number of triggers-1;
        raw = self.cwe.oa.sendMessage(CODE_READ, "SEQ_TRIGGERS_CONFIG", Validate=False, maxResp=1)[0]
        if enable:
            raw = (raw & 0x7f) | 0x80
        else:
            raw = raw & 0x7f
        self.cwe.oa.sendMessage(CODE_WRITE, "SEQ_TRIGGERS_CONFIG", [raw])


    @property
    def window_start(self):
        """Minimum number of clock cycles (of the ADC sampling clock) that must follow trigger #0
        before trigger #1 is allowed to complete the sequence. 0 = no limit.
        Args:
            start: 16-bit integer
        """
        return self.get_window_start()

    @window_start.setter
    def window_start(self, start):
        self.set_window_start(start)

    def get_window_start(self):
        starts = self.read_multiple_window_start()
        return SequenceTriggerList(starts, setter=self.set_multiple_window_start, getter=self.read_multiple_window_start)

    def read_multiple_window_start(self):
        raw = self.cwe.oa.sendMessage(CODE_READ, "SEQ_TRIGGERS_MINMAX", Validate=False, maxResp=self._window_bytes*(self.max_sequenced_triggers-1)*2)
        starts = []
        for i in range(self.max_sequenced_triggers-1):
            raw_index = i * self._window_bytes
            starts.append(int.from_bytes(raw[raw_index:raw_index+self._window_bytes], byteorder='little'))
        return starts

    def set_window_start(self, start):
        if self.max_sequenced_triggers == 2 or type(start) is int:
            if start >= 2**(8*self._window_bytes):
                raise ValueError('too big')
            self._check_windows(start, self.window_end)
            raw = list(int.to_bytes(start, length=self._window_bytes, byteorder='little'))
            self.cwe.oa.sendMessage(CODE_WRITE, "SEQ_TRIGGERS_MINMAX", raw)
        else:
            self.set_multiple_window_start(start)

    def set_multiple_window_start(self, starts):
        if len(starts) > self.max_sequenced_triggers-1:
            raise ValueError('Too many settings: can only specify %d windows' % (self.max_sequenced_triggers-1))
        self._check_windows(starts, self.window_end)
        raw = self.cwe.oa.sendMessage(CODE_READ, "SEQ_TRIGGERS_MINMAX", Validate=False, maxResp=self._window_bytes*(self.max_sequenced_triggers-1)*2)
        for i,start in enumerate(starts):
            if start >= 2**(8*self._window_bytes):
                raise ValueError('too big')
            raw_index = i * self._window_bytes
            raw[raw_index:raw_index+self._window_bytes] = list(int.to_bytes(start, length=self._window_bytes, byteorder='little'))
        self.cwe.oa.sendMessage(CODE_WRITE, "SEQ_TRIGGERS_MINMAX", raw)


    @property
    def window_end(self):
        """Maximum number of clock cycles (of the ADC sampling clock) that can follow trigger #0
        before trigger #1 is allowed to complete the sequence. 0 = no limit.
        Args:
            end: 16-bit integer
        """
        return self.get_window_end()


    @window_end.setter
    def window_end(self, end):
        self.set_window_end(end)

    def get_window_end(self):
        ends = self.read_multiple_window_end()
        return SequenceTriggerList(ends, setter=self.set_multiple_window_end, getter=self.read_multiple_window_end)

    def read_multiple_window_end(self):
        raw = self.cwe.oa.sendMessage(CODE_READ, "SEQ_TRIGGERS_MINMAX", Validate=False, maxResp=self._window_bytes*(self.max_sequenced_triggers-1)*2)
        ends = []
        for i in range(self.max_sequenced_triggers-1):
            raw_index = (self.max_sequenced_triggers -1 + i) * self._window_bytes
            ends.append(int.from_bytes(raw[raw_index:raw_index+self._window_bytes], byteorder='little'))
        return ends

    def set_window_end(self, end):
        if self.max_sequenced_triggers == 2 or type(end) is int:
            if end >= 2**(8*self._window_bytes):
                raise ValueError('too big')
            self._check_windows(self.window_start, end)
            raw = self.cwe.oa.sendMessage(CODE_READ, "SEQ_TRIGGERS_MINMAX", Validate=False, maxResp=2*self._window_bytes)
            raw[2:4] = list(int.to_bytes(end, length=2, byteorder='little'))
            self.cwe.oa.sendMessage(CODE_WRITE, "SEQ_TRIGGERS_MINMAX", raw)
        else:
            self.set_multiple_window_end(end)

    def set_multiple_window_end(self, ends):
        if len(ends) > self.max_sequenced_triggers-1:
            raise ValueError('Too many settings: can only specify %d windows' % (self.max_sequenced_triggers-1))
        self._check_windows(self.window_start, ends)
        raw = self.cwe.oa.sendMessage(CODE_READ, "SEQ_TRIGGERS_MINMAX", Validate=False, maxResp=self._window_bytes*(self.max_sequenced_triggers-1)*2)
        for i,end in enumerate(ends):
            if end >= 2**(8*self._window_bytes):
                raise ValueError('too big')
            raw_index = (self.max_sequenced_triggers - 1 + i) * self._window_bytes
            raw[raw_index:raw_index+self._window_bytes] = list(int.to_bytes(end, length=self._window_bytes, byteorder='little'))
        self.cwe.oa.sendMessage(CODE_WRITE, "SEQ_TRIGGERS_MINMAX", raw)


    def _check_windows(self, starts, ends):
        if type(starts) is int:
            starts = [starts]
        if type(ends) is int:
            ends = [ends]
        for i,(start,end) in enumerate(zip(starts, ends)):
            if start > 0 and end > 0 and start > end:
                scope_logger.warning('window %d is such that this trigger will never fire! (start=%d, end=%d)' % (i, start, end))


    @property
    def level(self):
        """For triggering on ADC sample exceeding a treshold,
        when scope.trigger.module = 'ADC'.

        Sets the trigger threshold, in the range [-0.5, 0.5].

        If positive, triggers when the ADC sample exceeds this setting;
        if negative, triggers when the ADC sample is below this setting.

        Only a single trigger is issued (i.e. multiple samples exceeding
        the threshold do not each generate a trigger; cannot be used in
        conjunction with segmented capture).
        """
        offset = self.cwe.oa.offset
        raw = int.from_bytes(self.cwe.oa.sendMessage(CODE_READ, "ADC_TRIGGER_LEVEL", Validate=False, maxResp=2), byteorder='little')
        return raw / 2**12 - offset

    @level.setter
    def level(self, val):
        if not (-0.5 <= val <= 0.5):
            raise ValueError("Out of range: [-0.5, 0.5]")
        offset = self.cwe.oa.offset
        val = int((val + offset) * 2**12)
        self.cwe.oa.sendMessage(CODE_WRITE, "ADC_TRIGGER_LEVEL", list(int.to_bytes(val, length=2, byteorder='little')))

    @property
    def edges(self):
        """For triggering on edge counts, when :code:`scope.trigger.module = 'edge_counter'`.

        Sets the number of rising+falling edges on :code:`scope.trigger.triggers` that
        need to be seen for a trigger to be issued.

        Edges are sampled by the ADC sampling clock (:code:`scope.clock.adc_freq`), so
        ensure that scope.trigger.triggers does not change faster than what can
        be seen by that clock.

        Args:
            val (int): number of edges, non-zero 16-bit integer.
        """
        return self._edges

    @edges.setter
    def edges(self, val):
        if val < 1 or val > 2**16:
            raise ValueError("Out of range: [1, 2**16]")
        self._edges = val
        self.cwe.oa.sendMessage(CODE_WRITE, "EDGE_TRIGGER_COUNT", list(int.to_bytes(val-1, length=2, byteorder='little')))

    @property
    def edges_seen(self):
        """Returns the number of edges seen. 
        
        Under normal operation this should
        be the same as :code:`scope.trigger.edges`. When trigger generation failed, Can
        be useful to understand why. Resets upon :code:`scope.arm()`.
        """
        return int.from_bytes(self.cwe.oa.sendMessage(CODE_READ, "EDGE_TRIGGER_COUNT", Validate=False, maxResp=2), byteorder='little')

    def get_trigger_times(self):
        """Retrieve the timestamped trigger times.

        When multiple triggers occur, the number of ADC clock cycles between
        successive triggers is recorded. Up to 1024 triggers can be
        timestamped. The counter for each timestamp is 32-bits wide; overflows
        are noted. Recorded triggers are automatically reset when the scope is
        armed.
        
        """
        if self._trigger_times_empty():
            return None
        else:
            if self._trigger_times_overflow():
                scope_logger.warning('Trigger times FIFO overflowed (too many triggers occured).')
                self.cwe.oa.sendMessage(CODE_WRITE, "NUM_TRIGGERS_STAT", [1]) # clears the overflow flag
            times = []
            while not self._trigger_times_empty():
                self.cwe.oa.sendMessage(CODE_WRITE, "NUM_TRIGGERS_DATA", [1])
                raw = int.from_bytes(self.cwe.oa.sendMessage(CODE_READ, "NUM_TRIGGERS_DATA", Validate=False, maxResp=4), byteorder='little')
                if raw == 2**32-1 and len(times): # don't care about overflow on first entry
                    scope_logger.error('Trigger times counter overflowed (more than 2**32 cycles between successive triggers).')
                else:
                    times.append(raw)
            if self._trigger_times_underflow():
                scope_logger.error('Internal error: trigger times FIFO underflowed.')
            return times[1:]


    def _trigger_times_empty(self):
        if self.cwe.oa.sendMessage(CODE_READ, "NUM_TRIGGERS_STAT", Validate=False, maxResp=1)[0] & 1:
            return True
        else:
            return False

    def _trigger_times_overflow(self):
        if self.cwe.oa.sendMessage(CODE_READ, "NUM_TRIGGERS_STAT", Validate=False, maxResp=1)[0] & 2:
            return True
        else:
            return False

    def _trigger_times_underflow(self):
        if self.cwe.oa.sendMessage(CODE_READ, "NUM_TRIGGERS_STAT", Validate=False, maxResp=1)[0] & 4:
            return True
        else:
            return False


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

class CWPLLDriver(object):
    ADDR_I2CSTATUS = 47
    ADDR_I2CDATA = 48

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

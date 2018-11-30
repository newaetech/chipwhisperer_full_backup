#!/usr/bin/python
# HIGHLEVEL_CLASSLOAD_FAIL_FUNC_WARN
# -*- coding: utf-8 -*-
#
# Copyright (c) 2013-2018, NewAE Technology Inc
# All rights reserved.
#
# Authors: Colin O'Flynn
#
# Find this and more at newae.com - this file is part of the chipwhisperer
# project, http://www.github.com/newaetech/chipwhisperer
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
import numpy as np
from usb import USBError
from base import ScopeTemplate
from chipwhisperer.capture.scopes.openadc_interface.naeusbchip import OpenADCInterface_NAEUSBChip
from chipwhisperer.common.utils import util, timer, pluginmanager
from chipwhisperer.common.utils.parameter import Parameter, setupSetParam
from chipwhisperer.common.utils.pluginmanager import Plugin
from chipwhisperer.common.utils.util import dict_to_str
from collections import OrderedDict

from chipwhisperer.capture.ui.programmers_dialog import XMEGAProgrammerDialog, AVRProgrammerDialog, STM32FProgrammerDialog
from chipwhisperer.common.utils.parameter import Parameterized
from chipwhisperer.hardware.naeusb.serial import USART
from chipwhisperer.hardware.naeusb.naeusb import NAEUSB, packuint32, unpackuint32
from chipwhisperer.hardware.naeusb.programmer_avr import AVRISP
from chipwhisperer.hardware.naeusb.programmer_xmega import XMEGAPDI
from chipwhisperer.hardware.naeusb.programmer_stm32fserial import STM32FSerial
from chipwhisperer.common.api.CWCoreAPI import CWCoreAPI
import time
import datetime



class ADCSettings(util.DisableNewAttr):
    USB_ADCLK_SET = 0x28
    USB_SAMPLES = 0x2A


    def __init__(self, usb):
        self.usb = usb
        self.disable_newattr()

    def _dict_repr(self):
        dict = OrderedDict()
        dict['clk_src'] = self.clk_src
        dict['clk_freq'] = self.clk_freq
        dict['samples'] = self.samples
        return dict

    def __repr__(self):
        return util.dict_to_str(self._dict_repr())

    def __str__(self):
        return self.__repr__()

    @property
    def samples(self):
        """Number of samples to store"""

        resp = self.usb.readCtrl(self.USB_SAMPLES, 0, 4)
        return unpackuint32(resp)

    @samples.setter
    def samples(self, numsamples):

        nresp = packuint32(numsamples)
        self.usb.sendCtrl(self.USB_SAMPLES, 0, nresp)


    @property
    def clk_src(self):
        """ADC Clock source: 'int' or 'ext' """

        resp = self.usb.readCtrl(self.USB_ADCLK_SET, 0, 5)
        if resp[3] == 0:
            return 'int'
        else:
            return 'ext'


    @clk_src.setter
    def clk_src(self, src):

        if src == "int":
            src = 0
        elif src == "ext":
            src = 1
        else:
            raise ValueError("Invalid source: %s"%str(src))

        resp = self.usb.readCtrl(self.USB_ADCLK_SET, 0, 5)

        resp[3] = src
        resp[4] = 1

        self.usb.sendCtrl(self.USB_ADCLK_SET, 0, 5)

    @property
    def clk_freq(self):
        """"Set the frequency for CLKOUT. Will be rounded to nearest possible values, check results to see
        programmed value. Set to 'None' for disabling (High-Z) output."""

        resp = self.usb.readCtrl(self.USB_ADCLK_SET, 0, 5)
        if resp[0] == 0:
            return 0
        else:
            return 240E6/resp[0]

    @clk_freq.setter
    def clk_freq(self, freqset):

        resp = self.usb.readCtrl(self.USB_ADCLK_SET, 0, 5)

        if freqset is None:
            best_div = 0
        else:
            #Get as close as possible - 240 MHz clock
            #Divider options: 1,2,4,8,16,32,64
            div_options = [1,2,4,8,16,32,64]

            freqsrc = 240E6

            err_list = [100E6]*len(div_options)

            for i, div in enumerate(div_options):
                err_list[i] = abs((freqsrc / div) - freqset)

            best_div = div_options[np.argmin(err_list)]

        resp[0] = best_div
        resp[4] = 1

        self.usb.sendCtrl(self.USB_ADCLK_SET, 0, resp)

        if best_div == 0:
            return 0
        else:
            return 240E6 / best_div

class GlitchSettings(util.DisableNewAttr):
    USB_GLITCH_SETTINGS = 0x2C
    USB_GLITCH_GO = 0x2D

    def __init__(self, usb):
        self.usb = usb
        self.disable_newattr()


    def _dict_repr(self):
        dict = OrderedDict()
        dict['repeat'] = self.repeat
        dict['ext_offset'] = self.ext_offset
        return dict

    def __repr__(self):
        return util.dict_to_str(self._dict_repr())

    def __str__(self):
        return self.__repr__()

    @property
    def ext_offset(self):
        """Offset from rising edge of trigger & when glitch gets inserted, approx = 8.3 ns * ext_offset"""
        resp = self.usb.readCtrl(self.USB_GLITCH_SETTINGS, 0, 8)[0:4]
        return unpackuint32(resp)

    @ext_offset.setter
    def ext_offset(self, offset):

        resp = self.usb.readCtrl(self.USB_GLITCH_SETTINGS, 0, 8)
        nresp = packuint32(offset)
        nresp.extend(resp[4:8])
        self.usb.sendCtrl(self.USB_GLITCH_SETTINGS, 0, nresp)

    @property
    def repeat(self):
        """Width of glitch in cycles (approx = 8.3 ns * width)"""

        resp = self.usb.readCtrl(self.USB_GLITCH_SETTINGS, 0, 8)[4:8]
        return unpackuint32(resp)

    @repeat.setter
    def repeat(self, width):
        resp = self.usb.readCtrl(self.USB_GLITCH_SETTINGS, 0, 8)
        nresp = resp[0:4]
        nresp.extend(packuint32(width))
        self.usb.sendCtrl(self.USB_GLITCH_SETTINGS, 0, nresp)

    def manualTrigger(self):
        """Manually trigger the glitch"""
        self.insert()

    def insert(self):
        self.usb.sendCtrl(self.USB_GLITCH_GO)


class GPIOSettings(util.DisableNewAttr):


    USB_MODE_OUT = 0x01
    USB_MODE_IN = 0x02
    USB_MODE_LOW = 0x03
    USB_MODE_HIGH = 0x04
    USB_MODE_PERA = 0x05
    USB_MODE_PERB = 0x06

    USB_GPIO_READDIR = 0x24
    USB_GPIO_SET   = 0x25
    USB_GPIO_READ  = 0x26

    USB_CLKOUT_SET = 0x27

    USB_GPIO1_MASK = (1<<0)
    USB_GPIO2_MASK = (1<<1)
    USB_GPIO3_MASK = (1<<2)
    USB_GPIO4_MASK = (1<<3)
    USB_PDIC_MASK = (1<<4)
    USB_PDID_MASK = (1<<5)
    USB_nRST_MASK = (1<<6)

    def __init__(self, usb):
        self.usb = usb
        self.disable_newattr()


    def _dict_repr(self):
        dict = OrderedDict()
        dict['tio1'] = self.tio1
        dict['tio2'] = self.tio2
        dict['tio3'] = self.tio3
        dict['tio4'] = self.tio4

        dict['pdid'] = self.pdid
        dict['pdic'] = self.pdic
        dict['nrst'] = self.nrst

        #dict['glitch_lp'] = self.glitch_lp

        dict['clkout'] = self.clkout

        return dict

    def __repr__(self):
        return util.dict_to_str(self._dict_repr())

    def __str__(self):
        return self.__repr__()

    @property
    def tio1(self):
        """The function of the Target IO1 pin.

        TIO1 can be used for the following functions:
        - "serial_rx": UART input
        - "high_z" / None: High impedance input
        - "gpio_low" / False: Driven output: logic 0
        - "gpio_high" / True: Driven output: logic 1
        - "gpio_disabled": Driven output: no effect
        Default value is "serial_rx".

        Getter: Return one of the above strings

        Setter: Set the Target IO1 mode.
            Raises: ValueError if new value is not one of the above modes
        """
        return None

    @tio1.setter
    def tio1(self, state):
        state = self._gpio_name_to_state(state)
        if state == "serial_rx":
            state = "pera"

        self.gpio_generic_set(self.USB_GPIO1_MASK, state)

    @property
    def tio2(self):
        """The function of the Target IO2 pin.

        TIO2 can be used for the following functions:
        - "serial_tx": UART output
        - "high_z" / None: High impedance input
        - "gpio_low" / False: Driven output: logic 0
        - "gpio_high" / True: Driven output: logic 1
        - "gpio_disabled": Driven output: no effect
        Default value is "serial_tx".

        Getter: Return one of the above strings

        Setter: Set the Target IO2 mode.
            Raises: ValueError if new value is not one of the above modes
        """
        return None

    @tio2.setter
    def tio2(self, state):
        state = self._gpio_name_to_state(state)
        if state == "serial_tx":
            state = "pera"

        self.gpio_generic_set(self.USB_GPIO2_MASK, state)

    @property
    def tio3(self):
        """The function of the Target IO3 pin.

        TIO3 can be used for the following functions:
        - "high_z" / None: High impedance input
        - "gpio_low" / False: Driven output: logic 0
        - "gpio_high" / True: Driven output: logic 1
        - "gpio_disabled": Driven output: no effect
        Default value is "high_z".

        Getter: Return one of the above strings

        Setter: Set the Target IO3 mode.
            Raises: ValueError if new value is not one of the above modes
        """
        return None

    @tio3.setter
    def tio3(self, state):
        state = self._gpio_name_to_state(state)
        self.gpio_generic_set(self.USB_GPIO3_MASK, state)

    @property
    def tio4(self):
        """The function of the Target IO4 pin.

        TIO4 can be used for the following functions:
        - "high_z" / None: High impedance input
        - "gpio_low" / False: Driven output: logic 0
        - "gpio_high" / True: Driven output: logic 1
        - "gpio_disabled": Driven output: no effect
        Default value is "high_z". Typically, this pin is used as a trigger
        input.

        Getter: Return one of the above strings

        Setter: Set the Target IO4 mode
            Raises: ValueError if new value is not one of the above modes
        """
        return None

    @tio4.setter
    def tio4(self, state):
        state = self._gpio_name_to_state(state)
        self.gpio_generic_set(self.USB_GPIO4_MASK, state)

    @property
    def pdic(self):
        """The state of the PDIC pin output pin.

        This is a GPIO pin. The following values are allowed:
        - "high" / True: logic 1
        - "low" / False: logic 0
        - "disabled" / "default" / "high_z" / None: undriven

        Getter: Return one of "high", "low", or "high_z"

        Setter: Set the pin's state
            Raises: ValueError if new state not listed above
        """
        return False

    @pdic.setter
    def pdic(self, state):
        state = self._gpio_name_to_state(state)
        self.gpio_generic_set(self.USB_PDIC_MASK, state)

    @property
    def pdid(self):
        """The state of the PDID pin.

        See pdic for more information."""
        return True

    @pdid.setter
    def pdid(self, state):
        state = self._gpio_name_to_state(state)
        self.gpio_generic_set(self.USB_PDID_MASK, state)

    @property
    def nrst(self):
        """The state of the NRST pin.

        See pdic for more information."""
        return True

    @nrst.setter
    def nrst(self, state):
        state = self._gpio_name_to_state(state)
        self.gpio_generic_set(self.USB_nRST_MASK, state)


    @property
    def clkout(self):
        """"Set the frequency for CLKOUT. Will be rounded to nearest possible values, check results to see
        programmed value. Set to 'None' for disabling (High-Z) output."""

        resp = self.usb.readCtrl(self.USB_CLKOUT_SET, 0, 3)
        if resp[0] == 0:
            return 0
        else:
            return 240E6/resp[0]

    @clkout.setter
    def clkout(self, freqset):

        if freqset is None:
            best_div = 0
        else:
            #Get as close as possible - 240 MHz clock
            #Divider options: 1,2,4,8,16,32,64
            div_options = [1,2,4,8,16,32,64]

            freqsrc = 240E6

            err_list = [100E6]*len(div_options)

            for i, div in enumerate(div_options):
                err_list[i] = abs((freqsrc / div) - freqset)

            best_div = div_options[np.argmin(err_list)]

        self.usb.sendCtrl(self.USB_CLKOUT_SET, 0, [best_div, 0, 0])

        if best_div == 0:
            return 0
        else:
            return 240E6 / best_div


    def gpio_generic_set(self, gpiomask, state, setdir=True):
        if state is None:
            self.usb.sendCtrl(self.USB_GPIO_SET, self.USB_MODE_IN, [gpiomask, 0])

        elif isinstance(state, int):
            #If binary/int, check true/false
            if state:
                if setdir:
                    self.usb.sendCtrl(self.USB_GPIO_SET, self.USB_MODE_OUT, [gpiomask, 0])
                self.usb.sendCtrl(self.USB_GPIO_SET, self.USB_MODE_HIGH, [gpiomask, 0])
            else:
                if setdir:
                    self.usb.sendCtrl(self.USB_GPIO_SET, self.USB_MODE_OUT, [gpiomask, 0])
                self.usb.sendCtrl(self.USB_GPIO_SET, self.USB_MODE_LOW, [gpiomask, 0])

        elif state == "pera":
            self.usb.sendCtrl(self.USB_GPIO_SET, self.USB_MODE_PERA, [gpiomask, 0])

        elif state == "perb":
            self.usb.sendCtrl(self.USB_GPIO_SET, self.USB_MODE_PERB, [gpiomask, 0])

        else:
            raise ValueError("Unknown pin state: %s"%state)

    def _gpio_state_to_name(self, state):
        if state is None:
            return "high_z"
        elif state:
            return "high"
        else:
            return "low"

    def _gpio_name_to_state(self, state):
        if (state == "gpio_low") or (state == "low"):
            state = False
        elif (state == "gpio_high") or (state == "high"):
            state = True
        elif (state == "high_z") or (state == "default") or (state == "disabled"):
            state = None

        return state


class CWNano(ScopeTemplate, Plugin, util.DisableNewAttr):
    """CWNano scope object.

    This class contains the public API for the CWNano hardware. It includes
    specific settings for each of these devices.

    To connect to one of these devices, the easiest method is

    This code will automatically detect an attached ChipWhisperer device and
    connect to it.

    For more help about scope settings, try help() on each of the ChipWhisperer
    scope submodules:
        scope.adc
        scope.io
        scope.glitch
    """

    _name = "ChipWhisperer Nano"

    REQ_ARM = 0x29
    REQ_SAMPLES = 0x2A

    def __init__(self):
        ScopeTemplate.__init__(self)
        self._is_connected = False

        self.params.init()

        self._cwusb = NAEUSB()
        self.ser = self._cwusb
        self.scopetype = self
        self.dev = self

        self.xmega = XMEGAPDI(self._cwusb)
        self.avr = AVRISP(self._cwusb)
        self.usart = USART(self._cwusb)
        self.serialstm32f = STM32FSerial(cwserial=self.usart)
        self.serialstm32f.scope = self
        self.io = GPIOSettings(self._cwusb)
        self.adc = ADCSettings(self._cwusb)
        self.glitch = GlitchSettings(self._cwusb)
        self._timeout = 2

        self._lasttrace = None

        self.getParams().addChildren([
            {'name':"CW-Lite XMEGA Programmer", 'tip':"Open XMEGA Programmer (ChipWhisperer-Lite Only)", 'type':"menu", "action":lambda _:self.getCwliteXMEGA().show()},
            {'name':"CW-Lite AVR Programmer", 'tip':"Open AVR Programmer (ChipWhisperer-Lite Only)", 'type':"menu", "action":lambda _:self.getCwliteAVR().show()},
            {'name':'Serial STM32F Programmer', 'tip':"Open STM32F Programmer (Serial/ChipWhisperer)", 'type':"menu", "action":lambda _:self.getSerialSTM32F().show()}
        ])

        self.disable_newattr()

    def getCurrentScope(self):
        return self

    def _con(self):
        self._cwusb.con(idProduct=[0xACE0])
        self.disable_newattr()
        self._is_connected = True
        return True

    def _dis(self):
        self.enable_newattr()
        self._is_connected = False
        return True

    def arm(self):
        """Arm the ADC, the trigger will be GPIO4 rising edge (fixed trigger)."""
        if self.connectStatus.value() is False:
            raise Warning("Scope \"" + self.getName() + "\" is not connected. Connect it first...")

        self._cwusb.sendCtrl(self.REQ_ARM, 1)


    def capture(self):
        """Raises IOError if unknown failure, returns 'True' if timeout, 'False' if no timeout"""

        starttime = datetime.datetime.now()
        while self._cwusb.readCtrl(self.REQ_ARM, dlen=1)[0] == 0:
            # Wait for a moment before re-running the loop
            time.sleep(0.05)
            diff = datetime.datetime.now() - starttime

            # If we've timed out, don't wait any longer for a trigger
            if (diff.total_seconds() > self._timeout):
                logging.warning('Timeout in cwnano capture()')
                return True

        self._lasttrace = self._cwusb.cmdReadMem(0, self.adc.samples)

        self._lasttrace = np.array(self._lasttrace) / 256.0 - 0.5

        self.newDataReceived(0, self._lasttrace, 0, self.adc.clk_freq)

        return False


    def getLastTrace(self):
        """Return the last trace captured with this scope.
        """
        return self._lasttrace


    def _dict_repr(self):
        dict = OrderedDict()
        dict['io']    = self.io._dict_repr()
        dict['adc']   = self.adc._dict_repr()
        dict['glitch'] = self.glitch._dict_repr()
        return dict

    def __repr__(self):
        # Add some extra information about ChipWhisperer type here
        if self._is_connected:
            ret = "ChipWhisperer Nano Device\n"
            return ret + dict_to_str(self._dict_repr())
        else:
            ret = "ChipWhisperer Nano device (disconnected)"
            return ret

    def __str__(self):
        return self.__repr__()

    def get_possible_devices(self, idProduct):
        return self._cwusb.get_possible_devices(idProduct=idProduct)

    def usbdev(self):
        return self._cwusb

    def getCwliteXMEGA(self):
        if not hasattr(self, 'cwliteXMEGA'):
            self.enable_newattr()
            self.cwliteXMEGA = XMEGAProgrammerDialog()
            self.disable_newattr()
        return self.cwliteXMEGA

    def getCwliteAVR(self):
        if not hasattr(self, 'cwliteAVR'):
            self.enable_newattr()
            self.cwliteAVR = AVRProgrammerDialog()
            self.disable_newattr()
        return self.cwliteAVR

    def getSerialSTM32F(self):
        if not hasattr(self, 'serialSTM32F'):
            self.enable_newattr()
            self.serialSTM32F = STM32FProgrammerDialog()
            self.disable_newattr()
        return self.serialSTM32F

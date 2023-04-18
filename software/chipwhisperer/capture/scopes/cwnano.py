#!/usr/bin/python
# HIGHLEVEL_CLASSLOAD_FAIL_FUNC_WARN
# -*- coding: utf-8 -*-
#
# Copyright (c) 2013-2022, NewAE Technology Inc
# All rights reserved.
#
# Find this and more at newae.com - this file is part of the chipwhisperer
# project, http://www.github.com/newaetech/chipwhisperer
#
#=================================================
import logging

from ...logging import *

import numpy as np
from ...capture.scopes.openadc_interface.naeusbchip import OpenADCInterface_NAEUSBChip
from ...common.utils import util
from ...common.utils.util import dict_to_str
from collections import OrderedDict
from ...hardware.naeusb.serial import USART
from .cwhardware.ChipWhispererSAM3Update import SAMFWLoader

from ...hardware.naeusb.serial import USART
from ...hardware.naeusb.naeusb import NAEUSB, packuint32, unpackuint32
from ...hardware.naeusb.programmer_avr import AVRISP
from ...hardware.naeusb.programmer_xmega import XMEGAPDI
from ...hardware.naeusb.programmer_stm32fserial import STM32FSerial
from ...common.utils.util import camel_case_deprecated, DelayedKeyboardInterrupt
from ..api.cwcommon import ChipWhispererCommonInterface, ChipWhispererSAMErrors
import time
import datetime



class ADCSettings(util.DisableNewAttr):
    USB_ADCLK_SET = 0x28
    USB_SAMPLES = 0x2A


    def __init__(self, usb):
        super().__init__()
        self.usb = usb
        self.disable_newattr()

    def _dict_repr(self):
        rtn = OrderedDict()
        rtn['clk_src'] = self.clk_src
        rtn['clk_freq'] = self.clk_freq
        rtn['samples'] = self.samples
        return rtn

    def __repr__(self):
        return util.dict_to_str(self._dict_repr())

    def __str__(self):
        return self.__repr__()



    @property
    def samples(self):
        """Number of samples to store."""

        resp = self.usb.readCtrl(self.USB_SAMPLES, 0, 4)
        return unpackuint32(resp)

    @samples.setter
    def samples(self, numsamples):

        nresp = packuint32(numsamples)
        self.usb.sendCtrl(self.USB_SAMPLES, 0, nresp)


    @property
    def clk_src(self):
        """ADC Clock source.

        :Getter: Returns 'int' or 'ext' based on the clock source.

        :Setter: (str) Set the ADC clock source to either internal or external:
            ('int' or 'ext')
        """

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

        self.usb.sendCtrl(self.USB_ADCLK_SET, 0, resp)

    @property
    def clk_freq(self):
        """Set the frequency for CLKOUT. Will be rounded to nearest possible values, check results to see
        programmed value. Set to 'None' for disabling (High-Z) output.

        :Getter: Returns the actual frequency for CLKOUT

        :Setter: Sets CLKOUT to the nearest possible value.
        """

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
        rtn = OrderedDict()
        rtn['repeat'] = self.repeat
        rtn['ext_offset'] = self.ext_offset
        return rtn

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
        rtn = OrderedDict()
        rtn['tio1'] = self.tio1
        rtn['tio2'] = self.tio2
        rtn['tio3'] = self.tio3
        rtn['tio4'] = self.tio4

        rtn['pdid'] = self.pdid
        rtn['pdic'] = self.pdic
        rtn['nrst'] = self.nrst

        #rtn['glitch_lp'] = self.glitch_lp

        rtn['clkout'] = self.clkout

        rtn['cdc_settings'] = self.cdc_settings

        return rtn

    def __repr__(self):
        return util.dict_to_str(self._dict_repr())

    def __str__(self):
        return self.__repr__()

    @property
    def tio1(self):
        """The function of the Target IO1 pin.

        TIO1 can be used for the following functions:
          * "serial_rx": UART input

        Default value is "serial_rx".

        :Getter: Return None

        :Setter: Set the Target IO1 mode.
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
          * "serial_tx": UART output

        Default value is "serial_tx".

        :Getter: Return None

        :Setter: Set the Target IO2 mode.
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
          * "high_z" / None: High impedance input
          * "gpio_low" / False: Driven output: logic 0
          * "gpio_high" / True: Driven output: logic 1
          * "gpio_disabled": Driven output: no effect

        Default value is "high_z".

        :Getter: Return one of the above strings

        :Setter: Set the Target IO3 mode.
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
          * "high_z" / None: High impedance input

        Default value is "high_z". Typically, this pin is used as a trigger
        input.

        :Getter: Return None

        :Setter: Set the Target IO4 mode
            Raises: ValueError if new value is not one of the above modes
        """
        return "high_z"

    @tio4.setter
    def tio4(self, state):
        state = self._gpio_name_to_state(state)
        self.gpio_generic_set(self.USB_GPIO4_MASK, state)

    @property
    def pdic(self):
        """The state of the PDIC pin output pin.

        This is a GPIO pin. The following values are allowed:
          * "high" / True: logic 1
          * "low" / False: logic 0
          * "disabled" / "default" / "high_z" / None: undriven

        :Getter: Return one of "high", "low", or "high_z"

        :Setter: Set the pin's state
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

    @property
    def cdc_settings(self):
        """Check or set whether USART settings can be changed via the USB CDC connection

        i.e. whether you can change USART settings (baud rate, 8n1) via a serial client like PuTTY

        :getter: An array of length two for two possible CDC serial ports (though only one is used)

        :setter: Can set either via an integer (which sets both ports) or an array of length 2 (which sets each port)

        Returns None if using firmware before the CDC port was added
        """
        rawver = self.usb.readFwVersion()
        ver = '{}.{}'.format(rawver[0], rawver[1])
        if ver < '0.30':
            return None
        return self.usb.get_cdc_settings()

    @cdc_settings.setter
    def cdc_settings(self, port):
        rawver = self.usb.readFwVersion()
        ver = '{}.{}'.format(rawver[0], rawver[1])
        if ver < '0.30':
            return None
        return self.usb.set_cdc_settings(port)

class CWNano(util.DisableNewAttr, ChipWhispererCommonInterface):
    """CWNano scope object.

    This class contains the public API for the CWNano hardware. It includes
    specific settings for each of these devices.

    To connect to one of these devices, the easiest method is::

        import chipwhisperer as cw
        scope = cw.scope(type=scopes.CWNano)

    Some sane default settings can be set using::

        scope.default_setup()

    For more help about scope settings, try help() on each of the ChipWhisperer
    scope submodules (scope.adc, scope.io, scope.glitch):

      * :attr:`scope.adc <chipwhisperer.capture.scopes.cwnano.ADCSettings>`
      * :attr:`scope.io <chipwhisperer.capture.scopes.cwnano.GPIOSettings>`
      * :attr:`scope.glitch <chipwhisperer.capture.scopes.cwnano.GlitchSettings>`
      * :meth:`scope.default_setup <.CWNano.default_setup>`
      * :meth:`scope.con <.CWNano.con>`
      * :meth:`scope.dis <.CWNano.dis>`
      * :meth:`scope.get_last_trace <.CWNano.get_last_trace>`
      * :meth:`scope.arm <.CWNano.arm>`
      * :meth:`scope.capture <.CWNano.capture>`
      * :meth:`scope.get_serial_ports <chipwhisperer.capture.api.cwcommon.ChipWhispererCommonInterface.get_serial_ports>`

    Inherits from :class:`chipwhisperer.capture.api.cwcommon.ChipWhispererCommonInterface`
    """

    _name = "ChipWhisperer Nano"

    REQ_ARM = 0x29
    REQ_SAMPLES = 0x2A

    def __init__(self):
        self._is_connected = False


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
        self.errors = ChipWhispererSAMErrors(self._getNAEUSB())
        self._timeout = 2

        self._lasttrace = None

        self.connectStatus = False
        self._lasttrace_int = None
        self.disable_newattr()

    def _getFWPy(self):
        from ...hardware.firmware.cwnano import fwver
        return fwver

    def default_setup(self):
        """ Sets up sane capture defaults for this scope

          * 7.5MHz ADC clock
          * 7.5MHz output clock
          * 5000 capture samples
          * tio1 = serial rx
          * tio2 = serial tx
          * glitch module off

        .. versionadded:: 5.1
            Added default setup for CWNano
        """
        self.adc.clk_freq = 7.5E6
        self.io.clkout = 7.5E6
        self.adc.samples = 5000
        self.io.tio1 = "serial_rx"
        self.io.tio2 = "serial_tx"
        self.glitch.repeat = 0

    def glitch_disable(self):
        self.glitch.repeat = 0
        self.glitch.ext_offset = 0

    def vglitch_setup(self, glitcht=None, default_setup=True):
        """Sets up sane defaults for voltage glitch

        repeat = 1
        ext_offset = 0
        """
        if default_setup:
            self.default_setup()

        self.glitch.repeat = 1
        self.glitch.ext_offset = 0

    def getCurrentScope(self):
        return self

    def _get_usart(self) -> USART:
        return self.usart

    def _getNAEUSB(self):
        return self._cwusb

    def _getCWType(self):
        return 'cwnano'

    def reset_clock_phase(self):
        """Resets the target and adc clocks, resetting their phase

        .. warning:: causes an interruption in the target clock. You may need to reset the target.
        """
        if self.check_feature("NANO_CLOCK_RESET"):
            tfreq = self.io.clkout
            afreq = self.adc.clk_freq
            self._getNAEUSB().sendCtrl(0x22, 0xF0)
            self.io.clkout = tfreq
            self.adc.clk_freq = afreq

    def con(self, sn=None, **kwargs):
        """Connects to attached chipwhisperer hardware (Nano)

        Args:
            sn (str): The serial number of the attached device. Does not need to
                be specified unless there are multiple devices attached.

        Returns:
            True if connection is successful, False otherwise.
        """
        self._read_only_attrs = []
        try:
            # possible_sn = self._cwusb.get_possible_devices(idProduct=[0xACE0])
            # serial_numbers = []
            # if len(possible_sn) > 1:
            #     if sn is None:
            #         for d in possible_sn:
            #             serial_numbers.append("sn = {} ({})".format(str(d['sn']), str(d['product'])))
            #         raise Warning("Multiple ChipWhisperers detected. Please specify device from the following list using cw.scope(sn=<SN>): \n{}".format(serial_numbers))
            # else:
            #     sn = None
            if "idProduct" in kwargs:
                del kwargs['idProduct']
            found_id = self._cwusb.con(idProduct=[0xACE0], serial_number=sn, **kwargs)
        except (IOError, ValueError) as e:
            raise Warning("Could not connect to cwnano. It may have been disconnected,\
is in an error state, or is being used by another tool.") from e
        module_list = [x for x in self.__dict__ if isinstance(self.__dict__[x], util.DisableNewAttr)]
        self.add_read_only(module_list)
        self.disable_newattr()
        self._is_connected = True
        self.connectStatus=True
        return True

    def dis(self):
        """Disconnects the current scope object.

        Returns:
            True if the disconnection was successful, False otherwise.
        """
        self._read_only_attrs = []
        self.usbdev().close()
        self._is_connected = False
        return True

    def arm(self):
        """Arm the ADC, the trigger will be GPIO4 rising edge (fixed trigger)."""
        with DelayedKeyboardInterrupt():
            if self.connectStatus is False:
                raise Warning("Scope \"" + 'CWNano' + "\" is not connected. Connect it first...")

            self._cwusb.sendCtrl(self.REQ_ARM, 1)


    def capture(self, poll_done=False):
        """Raises IOError if unknown failure, returns 'True' if timeout, 'False' if no timeout"""

        with DelayedKeyboardInterrupt():
            starttime = datetime.datetime.now()
            while self._cwusb.readCtrl(self.REQ_ARM, dlen=1)[0] == 0:
                # Wait for a moment before re-running the loop
                time.sleep(0.001)
                diff = datetime.datetime.now() - starttime

                # If we've timed out, don't wait any longer for a trigger
                if (diff.total_seconds() > self._timeout):
                    scope_logger.warning('Timeout in cwnano capture()')
                    return True

            self._lasttrace = self._cwusb.cmdReadMem(0, self.adc.samples)

            # can just keep rerunning this until it works I think
            i = 0
            while len(self._lasttrace) < self.adc.samples:
                scope_logger.debug("couldn't read ADC data from Nano, retrying...")

                self._lasttrace = self._cwusb.cmdReadMem(0, self.adc.samples)
                i+= 1
                if i > 20:
                    scope_logger.warning("Couldn't read trace data back from Nano")
                    return True
            self._lasttrace_int = np.array(self._lasttrace)
            self._lasttrace = np.array(self._lasttrace) / 256.0 - 0.5

            #self.newDataReceived(0, self._lasttrace, 0, self.adc.clk_freq)

            return False


    def get_last_trace(self, as_int=False):
        """Return the last trace captured with this scope.

        Can return traces as floating point values (:code:`as_int=False`)
        or as integers.

        Floating point values are scaled and shifted to be between -0.5 and 0.5.

        Integer values are raw readings from the ChipWhisperer ADC. The ChipWhisperer-Lite
        has a 10-bit ADC, the Nano has an 8-bit ADC, and the Husky can read either
        8-bits or 12-bits of ADC data.

        Args:
            as_int (bool): If False, return trace as a float. Otherwise, return as an int.

        Returns:
           Numpy array of the last capture trace.

        .. versionchanged:: 5.6.1
            Added as_int parameter
        """
        if as_int:
            return self._lasttrace_int
        return self._lasttrace

    getLastTrace = camel_case_deprecated(get_last_trace)


    def _dict_repr(self):
        rtn = OrderedDict()
        rtn['fw_version'] = self.fw_version
        rtn['io']    = self.io._dict_repr()
        rtn['adc']   = self.adc._dict_repr()
        rtn['glitch'] = self.glitch._dict_repr()
        rtn['errors'] = self.errors._dict_repr()
        return rtn

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

    def __enter__(self):
        return self

    def __exit__(self, type, value, traceback):
        self.dis()

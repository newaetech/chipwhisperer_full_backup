#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2021, NewAE Technology Inc
# All rights reserved.
#
# Authors: Jean-Pierre Thibault
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
from ....common.utils import util
from ...api.cwcommon import ChipWhispererSAMErrors
from .. import _OpenADCInterface as OAI

from ....logging import *
import numpy as np
import time

CODE_READ = 0x80
CODE_WRITE = 0xC0

class XilinxDRP(util.DisableNewAttr):
    ''' Read/write methods for DRP port in Xilinx primitives such as MMCM and XADC.
        Husky only.
        Talks to something like reg_mmcm_drp.v.
    '''
    _name = 'Xilinx DRP Access'
    def __init__(self, oaiface : OAI.OpenADCInterface, data_address, address_address, reset_address = None):
        super().__init__()
        self.oa = oaiface
        self.data = data_address
        self.addr = address_address
        self.reset_address = reset_address
        self.disable_newattr()

    def write(self, addr, data):
        """Write DRP register. UG480 for register definitions.

        Args:
            addr (int): 6-bit address
            data (int): 16-bit write data
        """
        self.oa.sendMessage(CODE_WRITE, self.data, [data  & 0xff, data >> 8])
        self.oa.sendMessage(CODE_WRITE, self.addr, [addr + 0x80])

    def read(self, addr):
        """Read DRP register. UG480 for register definitions.

        Args:
            addr (int): 6-bit address

        Returns:
            A 16-bit integer.
        """
        self.oa.sendMessage(CODE_WRITE, self.addr, [addr])
        raw = self.oa.sendMessage(CODE_READ, self.data, maxResp=2)
        return int.from_bytes(raw, byteorder='little')

    def reset(self):
        """Pulse reset to associated IP block (intended for MMCM blocks, which
        need to be reset when their M/D parameters are updated).
        """
        if self.reset_address is None:
            raise ValueError("Reset not defined for this DRP interface")
        self.oa.sendMessage(CODE_WRITE, self.reset_address, [1])
        self.oa.sendMessage(CODE_WRITE, self.reset_address, [0])



class XilinxMMCMDRP(util.DisableNewAttr):
    ''' Methods for dynamically programming Xilinx MMCM via its DRP.
        Husky only.
    '''
    _name = 'Xilinx MMCM DRP'
    def __init__(self, drp):
        super().__init__()
        self.drp = drp
        self.disable_newattr()

    def set_mul(self, mul):
        muldiv2 = int(mul/2)
        lo = muldiv2
        if mul%2:
            hi = lo+1
        else:
            hi = lo
        if hi >= 2**6:
            raise ValueError("Internal error: calculated hi/lo value exceeding range")
        raw = lo + (hi<<6) + 0x1000
        self.drp.write(0x14, raw)
        self.drp.reset()


    def set_main_div(self, div):
        if not isinstance(div, int):
            raise ValueError("Only integers are supported")
        # Set main divider:
        if div == 1:
            raw = 0x1000
        else:
            div2 = int(div/2)
            lo = div2
            if div % 2:
                hi = lo+1
            else:
                hi = lo
            raw = lo + (hi<<6)
        self.drp.write(0x16, raw)
        self.drp.reset()


    def set_sec_div(self, div, clock=0):
        if not isinstance(div, int):
            raise ValueError("Only integers are supported")
        if clock > 5:
            raise ValueError("Clock must be in range [0,5]")
        addr = 0x08 + clock*2
        # Set secondary divider:
        if div == 1:
            self.drp.write(addr+1, 0x0040)
        else:
            div2 = int(div/2)
            lo = div2
            if div % 2:
                hi = lo+1
            else:
                hi = lo
            raw = lo + (hi<<6) + 0x1000
            self.drp.write(addr, raw)
        self.drp.reset()
        if self.get_sec_div(clock=clock) != div:
            scope_logger.error("""
                Failed to update the MMCM secondary divider. A hard reboot of
                Husky is likely necessary.  This likely occurred because you
                either:
                (1) programmed invalid MMCM parameters, or
                (2) the MMCM had valid parameters for a particular input clock
                frequency, but then you changed that clock frequency and this
                made the MMCM parameters invalid (this is the likely scenario). 

                To avoid this in the future, once the MMCM is operating (i.e.
                scope.LA.clkgen_enabled = True), if you wish to change
                the input clock frequency, first disable the MMCM, then update
                the clock, then update the MMCM, and then re-enable the MMCM.

                For example:
                # set up initial valid settings:
                scope.clock.clkgen_freq = 5e6
                scope.LA.oversampling_factor = 40
                scope.LA.clkgen_enabled = True
                # update to new settings:
                scope.LA.clkgen_enabled = False
                scope.clock.clkgen_freq = 100e6
                scope.LA.oversampling_factor = 3
                scope.LA.clkgen_enabled = True
                """)


    def get_mul(self):
        # 1. read CLKFBOUT2 to ensure fractional mode is disabled:
        raw = list(int.to_bytes(self.drp.read(0x11), length=2, byteorder='little'))
        if raw[1] & 0x08:
            print('WARNING: fractional mode is enabled. This is unexpected. Reported multiplier value will be incorrect.')
        # 2. check "NO COUNT" bit:
        if raw[0] & 0x04:
            mul = 1
        else:
            # 3. read CLKFBOUT:
            raw = list(int.to_bytes(self.drp.read(0x14), length=2, byteorder='little'))
            # extract high time and low time
            lo = (raw[0] & 0x3f)
            hi = (raw[0]>>6) + ((raw[1] & 0x0f)<<2)
            mul = lo + hi
            #if lo != hi: print('WARNING: high and low times unequal (%d, %d) ! Duty cycle is not 50/50. This is unexpected. % (hi, lo))
        return mul


    def get_main_div(self):
        raw = list(int.to_bytes(self.drp.read(0x16), length=2, byteorder='little'))
        if raw[1] & 0x10:
            maindiv = 1
        else:
            # extract high time and low time
            lo = (raw[0] & 0x3f)
            hi = (raw[0]>>6) + ((raw[1] & 0x0f)<<2)
            maindiv = lo + hi
        return maindiv


    def get_sec_div(self, clock=0):
        if clock > 5:
            raise ValueError("Clock must be in range [0,5]")
        addr = 0x08 + clock*2
        #  read CLKOUT2 to ensure fractional mode is disabled and check NO_COUNT bit for CLKOUT divider:
        raw = list(int.to_bytes(self.drp.read(addr+1), length=2, byteorder='little'))
        if raw[1] & 0x08:
            logging.error('CLKGEN fractional mode is enabled. This is unexpected.')
        if raw[0] & 0x40:
            secdiv = 1
        else:
            # read CLKOUT divider:
            raw = list(int.to_bytes(self.drp.read(addr), length=2, byteorder='little'))
            # extract high time and low time
            lo = (raw[0] & 0x3f)
            hi = (raw[0]>>6) + ((raw[1] & 0x0f)<<2)
            secdiv = lo + hi
        return secdiv


class LEDSettings(util.DisableNewAttr):
    ''' Set source of Husky front-panel LEDs
    '''
    _name = 'Husky LEDs Setting'

    def __init__(self, oaiface : OAI.OpenADCInterface):
        super().__init__()
        self.oa = oaiface
        self.disable_newattr()

    def _dict_repr(self):
        rtn = {}
        rtn['setting'] = self.setting
        return rtn

    def __repr__(self):
        return util.dict_to_str(self._dict_repr())

    def __str__(self):
        return self.__repr__()

    @property
    def setting(self):
        """Front-panel LED sources.

        #. default: green=armed, blue=capture, top red=PLL lock fail, bottom red=glitch
        #. green: USB clock heartbeat, blue=CLKGEN clock heartbeat
        #. green: ADC sampling clock heartbeat, blue=PLL reference clock heartbeat
        #. green: PLL clock heartbeat, blue=external clock change detected

        In all cases, blinking red lights indicate a temperature, voltage, or
        sampling error (see scope.XADC.status and scope.adc.errors for details),
        whlie blinking green and blue lights indicate that a frequency change
        was detected on the external clock input (and that scope.clock should
        be updated to account for this).

        """
        raw = self.oa.sendMessage(CODE_READ, "LED_SELECT", maxResp=1)[0]
        if raw == 0:   return "0 (default, as labelled)"
        elif raw == 1: return "1 (USB and CLKGEN clock heartbeats)"
        elif raw == 2: return "2 (ADC clock heartbeats)"
        elif raw == 3: return "3 (PLL clock heartbeat)"
        else: return ValueError

    @setting.setter
    def setting(self, val):
        if val < 0 or val > 3:
            raise ValueError
        self.oa.sendMessage(CODE_WRITE, "LED_SELECT", [val])


class HuskyErrors(ChipWhispererSAMErrors):
    ''' Gather all the Husky error sources in one place.
        Use scope.errors.clear() to clear them.
    '''
    _name = 'Husky Errors'

    def __init__(self, oaiface : OAI.OpenADCInterface, XADC, adc, clock, trace):
        super().__init__(oaiface.serial) # naeusb comms
        self.enable_newattr()
        self.oa = oaiface
        self.XADC = XADC
        self.adc = adc
        self.clock = clock
        self.trace = trace
        self.disable_newattr()

    def _dict_repr(self):
        rtn = super()._dict_repr()
        rtn['XADC errors'] = self.XADC.errors()
        rtn['ADC errors'] = self.adc.errors
        rtn['extclk error'] = self.clock.extclk_error
        rtn['trace errors'] = self.trace.errors
        return rtn

    def __repr__(self):
        return util.dict_to_str(self._dict_repr())

    def __str__(self):
        return self.__repr__()

    def clear(self):
        super().clear()
        self.XADC.status = 0
        self.adc.errors = 0
        self.clock.extclk_error = 0
        self.trace.errors = 0


class USERIOSettings(util.DisableNewAttr):
    ''' Control Husky's USERIO (20-pin front connector) interface.
    '''
    _name = 'USERIO Control'

    fpga_mode_definitions = [None]*16 # type: list

    # fpga_mode = 0:
    fpga_mode_definitions[0] = ['streaming debug',
                                 ['stream_segment_available',
                                  'slow_fifo_wr_slow',
                                  'slow_fifo_rd_slow',
                                  'reg_read_slow',
                                  'fast_fifo_read',
                                  'fifo_error_flag',
                                  'glitchclk',
                                  'glitch_enable',
                                  'unused']]

    # fpga_mode = 1:
    fpga_mode_definitions[1] = ['trigger_unit.v debug', 
                                  ['cmd_arm_usb',
                                   'trigger',
                                   'armed',
                                   'capture_active_o',
                                   'capture_done_i',
                                   'int_reset_capture',
                                   'arm_o',
                                   'armed_and_ready',
                                   'adc_capture_done']]

    # fpga_mode = 2:
    fpga_mode_definitions[2] = ['fifo_top_husky.v debug', 
                                 ['state[0]',
                                  'state[1]',
                                  'state[2]',
                                  'capture_go',
                                  'arming',
                                  'armed_and_ready',
                                  'fifo_rst',
                                  'adc_capture_stop',
                                  'unused']]

    # fpga_mode = 3:
    fpga_mode_definitions[3] = ['glitch debug', 
                                 ['cmd_arm_usb',
                                  'trigger_capture',
                                  'glitch_enable',
                                  'glitchclk',
                                  'glitch_mmcm2_clk_out',
                                  'glitch_mmcm1_clk_out',
                                  'xadc_error_flag',
                                  'unused',
                                  'unused']]

    # fpga_mode = 4:
    fpga_mode_definitions[4] = ['clockglitch debug1', 
                                 ['exttrigger',
                                  'exttrigger_resync',
                                  'trigger_resync_idle',
                                  'glitch_trigger',
                                  'glitch_done_count[0]',
                                  'glitch_done_count[1]',
                                  'clockglitch_count[0]',
                                  'clockglitch_count[1]',
                                  'unused']]

    # fpga_mode = 5:
    fpga_mode_definitions[5] = ['clockglitch debug2', 
                                 ['glitch_done_count[1]',
                                  'glitch_done_count[0]',
                                  'glitch_enable',
                                  'glitchclk',
                                  'glitch_trigger',
                                  'glitch_mmcm1_clk_out',
                                  'sourceclk',
                                  'exttrigger',
                                  'unused']]

    # fpga_mode = 6:
    fpga_mode_definitions[6] = ['usb debug1', 
                                 ['USB_Data[4]',
                                  'USB_Data[5]',
                                  'USB_Data[6]',
                                  'USB_Data[7]',
                                  'clk_usb_buf',
                                  'USB_CEn',
                                  'USB_WRn',
                                  'USB_RDn',
                                  'unused']]

    # fpga_mode = 7:
    fpga_mode_definitions[7] = ['usb debug2', 
                                 ['USB_Data[0]',
                                  'USB_Data[1]',
                                  'USB_Data[2]',
                                  'USB_Data[3]',
                                  'clk_usb_buf',
                                  'USB_CEn',
                                  'USB_WRn',
                                  'USB_RDn',
                                  'unused']]

    # fpga_mode = 8:
    fpga_mode_definitions[8] = ['usb debug3', 
                                 ['USB_Data[0]',
                                  'USB_Data[1]',
                                  'USB_Data[2]',
                                  'USB_Data[3]',
                                  'USB_Data[4]',
                                  'USB_Data[5]',
                                  'USB_Data[6]',
                                  'reg_write',
                                  'unused']]

    # fpga_mode = 9:
    fpga_mode_definitions[9] = ['edge trigger debug', 
                                 ['trigger_in',
                                  'trigger_in_r[0]',
                                  'trigger_in_r[1]',
                                  'triggered',
                                  'armed_and_ready',
                                  'running',
                                  'edge_counter[0]',
                                  'edge_counter[1]',
                                  'unused']]

    # fpga_mode = 10:
    fpga_mode_definitions[10] = ['clockglitch debug3 (trigger_resync)', 
                                 ['exttrig',
                                  'exttrigger_resync',
                                  'async_trigger',
                                  'state[0]',
                                  'state[1]',
                                  'done',
                                  'oneshot',
                                  'glitch_condition',
                                  'unused']]

    # fpga_mode = 11:
    fpga_mode_definitions[11] = ['triggers',
                                 ['cmd_arm_usb',
                                  'trigger_edge_counter',
                                  'trigger_adc',
                                  'trace_trig_out',
                                  'trigger_sad',
                                  'uart_trigger_line',
                                  'target_io4',
                                  'edge_trigger_line',
                                  'unused']]

    # fpga_mode = 12:
    fpga_mode_definitions[12] = ['trigger_unit.v debug2',
                                 ['arm_i',
                                  'adc_capture_done',
                                  'trigger',
                                  'capture_active_o',
                                  'int_reset_capture',
                                  'capture_go_start',
                                  'capture_go_o',
                                  'adc_delay_cnt == 0',
                                  'ununsed']]

    # fpga_mode = 13:
    fpga_mode_definitions[13] = ['sequencer debug (2 triggers)',
                                 ['trigger 0',
                                  'trigger 1',
                                  'too early',
                                  'too late',
                                  'sequence trigger output',
                                  'trigger 0 window',
                                  'trigger 1 window',
                                  'armed_and_ready',
                                  'unused']]

    # fpga_mode = 14:
    fpga_mode_definitions[14] = ['sequencer/SAD debug',
                                 ['unused',
                                  'unused',
                                  'unused',
                                  'trigger[0]',
                                  'trigger[1]',
                                  'trigger 0 window',
                                  'trigger 1 window',
                                  'too late',
                                  'unused']]

    # fpga_mode = 15:
    fpga_mode_definitions[15] = ['sequencer/SAD debug (4 triggers)',
                                 ['trigger 0',
                                  'trigger 1',
                                  'trigger 2',
                                  'trigger 3',
                                  'trigger 0 window',
                                  'trigger 1 window',
                                  'trigger 2 window',
                                  'trigger 3 window',
                                  'unused']]

    def __init__(self, oaiface : OAI.OpenADCInterface, trace):
        super().__init__()
        self._last_mode = None
        self.oa = oaiface
        self._trace = trace
        self.disable_newattr()

    def _dict_repr(self):
        rtn = {}
        rtn['mode'] = self.mode
        if self.mode in ['fpga_debug', 'swo_trace_plus_debug']:
            rtn['fpga_mode'] = self.fpga_mode
        rtn['direction'] = self.direction
        rtn['drive_data'] = self.drive_data
        rtn['status'] = self.status
        pins_rtn = {}
        trace_pins = ['TMS', 'TCK', 'TDO/SWO', 'unused', 'TRACEDATA[0]', 'TRACEDATA[1]', 'TRACEDATA[2]', 'TRACEDATA[3]', 'TRACECLOCK']
        for i in range(9):
            if self.mode == 'trace':
                info = trace_pins[i]
            elif self.mode == 'swo_trace_plus_debug':
                if i < 3:
                    info = trace_pins[i]
                else:
                    info = self.fpga_mode_definitions[self.fpga_mode][1][i]
            elif self.direction & 2**i:
                info = 'Husky-driven, '
                if self.mode == 'normal':
                    info += 'value = %d' % ((self.status >> i) & 0x01)
                else:
                    info = self.fpga_mode_definitions[self.fpga_mode][1][i]
            else:
                info = 'Target-driven, value = %d' % ((self.status >> i) & 0x01)
            if i < 8:
                pins_rtn['pin D%d' % i] = info
            else:
                pins_rtn['pin CK'] = info
        rtn['Individual pins'] = pins_rtn
        return rtn

    def __repr__(self):
        return util.dict_to_str(self._dict_repr())

    def __str__(self):
        return self.__repr__()

    @property
    def mode(self):
        """Set mode for USERIO pins:
            "normal": as defined by scope.userio.direction.
            "trace": for target trace capture.
            "target_debug_jtag": for target debugging with ChipWhisperer using MPSSE in JTAG mode
            "target_debug_swd": for target debugging with ChipWhisperer using MPSSE in SWD mode
            "fpga_debug": for FPGA debug (print the scope.userio object to obtain current signal definition, which is determined by scope.userio.fpga_mode).
            "swo_trace_plus_debug": pins D0-D2 are used for SWO trace, D3-D7 for fpga_debug.
        """
        if self._last_mode:
            return self._last_mode
        else:
            debug = self.oa.sendMessage(CODE_READ, "USERIO_DEBUG_DRIVEN", maxResp=1)[0]
            if self._trace:
                trace = self._trace.enabled
            else:
                trace = None
            if trace:
                return "trace"
            elif debug == 1:
                return "fpga_debug"
            elif debug == 2:
                return "target_debug_jtag"
            elif debug == 6:
                return "target_debug_swd"
            else:
                return "normal"

    @mode.setter
    def mode(self, setting):
        if setting == 'normal':
            self.oa.sendMessage(CODE_WRITE, "USERIO_DEBUG_DRIVEN", [0])
            self._trace._set_enabled(0)
        elif setting == 'trace':
            self.oa.sendMessage(CODE_WRITE, "USERIO_DEBUG_DRIVEN", [0])
            self._trace._set_enabled(1)
            self._trace._set_userio_dir(3)  # restore default just in case
        elif setting == 'fpga_debug':
            self.oa.sendMessage(CODE_WRITE, "USERIO_DEBUG_DRIVEN", [1])
            self._trace._set_enabled(0)
        elif setting == 'swo_trace_plus_debug':
            self.oa.sendMessage(CODE_WRITE, "USERIO_DEBUG_DRIVEN", [1])
            self._trace._set_enabled(1)
            self._trace._set_userio_dir(0xff-4)
        elif setting == 'target_debug_jtag':
            self.oa.sendMessage(CODE_WRITE, "USERIO_DEBUG_DRIVEN", [2])
            self._trace._set_enabled(0)
        elif setting == 'target_debug_swd':
            self.oa.sendMessage(CODE_WRITE, "USERIO_DEBUG_DRIVEN", [6])
            self._trace._set_enabled(0)
        else:
            raise ValueError("Invalid mode; use normal/trace/fpga_debug/target_debug_jtag/target_debug_swd")
        self._last_mode = setting

    @property
    def fpga_mode(self):
        """When scope.userio.mode = 'fpga_debug', selects which FPGA signals
        are routed to the USERIO pins. Print the scope.userio object to obtain the signal definition
        corresponding to the current fpga_mode setting.
        """
        return self.oa.sendMessage(CODE_READ, "USERIO_DEBUG_SELECT", maxResp=1)[0]

    @fpga_mode.setter
    def fpga_mode(self, setting):
        if not setting in range(0, 16):
            raise ValueError("Must be integer in [0, 15]")
        else:
            self.oa.sendMessage(CODE_WRITE, "USERIO_DEBUG_SELECT", [setting])


    @property
    def direction(self):
        """Set the direction of the USERIO data pins (D0-D7) with an
        8-bit integer, where bit <x> determines the direction of D<x>;
        bit x = 0: D<x> is an input to Husky.
        bit x = 1: D<x> is driven by Husky.
        When scope.userio.mode is not "normal", then this setting is controlled
        by the FPGA and cannot be changed by the user.
        Use with care.
        """
        return self.oa.sendMessage(CODE_READ, "USERIO_CW_DRIVEN", maxResp=1)[0]

    @direction.setter
    def direction(self, setting):
        if not setting in range(0, 256):
            raise ValueError("Must be integer 0-255")
        else:
            self.oa.sendMessage(CODE_WRITE, "USERIO_CW_DRIVEN", [setting])

    @property
    def drive_data(self):
        """8-bit data to drive on the USERIO data bus.
        """
        return self.oa.sendMessage(CODE_READ, "USERIO_DRIVE_DATA", maxResp=1)[0]

    @drive_data.setter
    def drive_data(self, setting):
        if not setting in range(0, 256):
            raise ValueError("Must be integer 0-255")
        else:
            self.oa.sendMessage(CODE_WRITE, "USERIO_DRIVE_DATA", [setting])

    @property
    def status(self):
        """Returns current value of header pins. LSB=D0, MSB=CK.
        """
        raw = self.oa.sendMessage(CODE_READ, "USERIO_READ", maxResp=2)
        return int.from_bytes(raw, byteorder='little')



class XADCSettings(util.DisableNewAttr):
    ''' Husky FPGA XADC temperature and voltage monitoring.
    XADC alarms are sticky and shut down generated clocks and SAD logic; the
    error condition must be manually cleared (scope.XADC.status = 0) to re-enable
    shutdown logic.
    '''
    _name = 'Husky XADC Setting'

    def __init__(self, oaiface : OAI.OpenADCInterface):
        super().__init__()
        self.oa = oaiface
        self.drp = XilinxDRP(oaiface, "XADC_DRP_DATA", "XADC_DRP_ADDR")
        self.disable_newattr()

    def _dict_repr(self):
        rtn = {}
        rtn['status'] = self.status
        rtn['current temperature [C]'] = '%.1f' % self.temp
        rtn['maximum temperature [C]'] = '%.1f' % self.max_temp
        rtn['user temperature alarm trigger [C]'] = '%.1f' % self.temp_trigger
        rtn['user temperature reset trigger [C]'] = '%.1f' % self.temp_reset
        rtn['device temperature alarm trigger [C]'] = '%.1f' % self.ot_temp_trigger
        rtn['device temperature reset trigger [C]'] = '%.1f' % self.ot_temp_reset
        rtn['vccint'] = '%.3f' % self.vccint
        rtn['vccaux'] = '%.3f' % self.vccaux
        rtn['vccbram'] = '%.3f' % self.vccbram
        return rtn

    def __repr__(self):
        return util.dict_to_str(self._dict_repr())

    def __str__(self):
        return self.__repr__()

    @property
    def status(self):
        """Read XADC alarm status bits
        :Getter: Returns status string.

        :Setter: Clear the status error bits (they are sticky).
        """
        raw = self.oa.sendMessage(CODE_READ, "XADC_STAT", maxResp=1)[0]
        stat = ''
        if raw & 1:  stat += 'Over temperature alarm, '
        if raw & 2:  stat += 'User temperature alarm, '
        if raw & 4:  stat += 'VCCint alarm, '
        if raw & 8:  stat += 'VCCaux alarm, '
        if raw & 16: stat += 'VCCbram alarm, '
        if stat == '':
            stat = 'good'
        return stat

    @status.setter
    def status(self, clear):
        self.oa.sendMessage(CODE_WRITE, "XADC_STAT", [0x0])

    def errors(self):
        if self.status == 'good':
            return False
        else:
            return self.status


    @property
    def temp(self):
        """Returns the current FPGA temperature.
        """
        return self.get_temp(0)

    @property
    def max_temp(self):
        """Returns the highest observed FPGA temperature.
        """
        return self.get_temp(32)

    @property
    def temp_trigger(self):
        """FPGA user temperature trigger.
        If the FPGA temperature exceeds this value, an error is flagged, and
        all clock-generating modules are shut down until the temperature
        returns below temp_reset (since they are very power hungry).
        """
        return self.get_temp(0x50)

    @temp_trigger.setter
    def temp_trigger(self, temp):
        return self.set_temp(temp, 0x50)

    @property
    def temp_reset(self):
        """FPGA user temperature reset.
        When the FPGA temperature returns below this value, the error condition
        triggered by temp_trigger is cleared.
        """
        return self.get_temp(0x54)

    @temp_reset.setter
    def temp_reset(self, temp):
        return self.set_temp(temp, 0x54)

    @property
    def ot_temp_trigger(self):
        """FPGA over-temperature trigger.
        If the FPGA temperature exceeds this value, an error is flagged, and
        all clock-generating modules are shut down until the temperature
        returns below ot_temp_reset (since they are very power hungry).
        Read-only.
        """
        return self.get_temp(0x53)

    @property
    def ot_temp_reset(self):
        """FPGA over-temperature reset.
        When the FPGA temperature returns below this value, the error condition
        triggered by ot_temp_trigger is cleared.
        """
        return self.get_temp(0x57)

    def get_temp(self, addr=0):
        """Read XADC temperature.

        Args:
            addr (int): DRP address (0: current; 32: max; 36: min)

        Returns:
            Temperature in celcius (float).
        """
        raw = self.drp.read(addr)
        return (raw>>4) * 503.975/4096 - 273.15 # ref: UG480

    def set_temp(self, temp, addr=0):
        """Set XADC temperature thresholds.

        Args:
            addr (int): DRP address
            temp (float): temperature threshold [celcius]

        Returns:
            Temperature in celcius (float).
        """
        raw = (int((temp + 273.15)*4096/503.975) << 4) & 0xffff
        self.drp.write(addr, raw)


    @property
    def vccint(self):
        """Returns the current VCCint value.
        """
        return self.get_vcc('vccint')

    @property
    def vccaux(self):
        """Returns the current VCCaux value.
        """
        return self.get_vcc('vccaux')

    @property
    def vccbram(self):
        """Returns the current VCCbram value.
        """
        return self.get_vcc('vccbram')

    def get_vcc(self, rail='vccint', value='current'):
        """Read XADC vcc.

        Args:
            rail (string): 'vccint', 'vccaux', or 'vccbram'
            value (string): 'current', 'min', or 'max'

        Returns:
            voltage (float).
        """
        if rail == 'vccint':
            addr = 1
        elif rail == 'vccaux':
            addr = 2
        elif rail == 'vccbram':
            if value == 'current':
                addr = 6
            else:
                addr = 3
        else:
            raise ValueError("Invalid rail")
        if value == 'current':
            pass
        elif value == 'min':
            addr += 0x24
        elif value == 'max':
            addr += 0x20
        else:
            raise ValueError("Invalid measurement request")

        raw = self.drp.read(addr)
        return (raw>>4)/4096 * 3 # ref: UG480

    def _get_vcc_limit(self, rail='vccint', limit='upper'):
        """Get XADC vcc limit.

        Args:
            rail (string): 'vccint', 'vccaux', or 'vccbram'
            limit (string): 'upper', 'lower'

        Returns:
            voltage (float).
        """
        if rail == 'vccint':
            addr = 0x51
        elif rail == 'vccaux':
            addr = 0x52
        elif rail == 'vccbram':
            addr = 0x58
        else:
            raise ValueError("Invalid rail")
        if limit == 'lower':
            addr += 4
        elif limit == 'upper':
            pass
        else:
            raise ValueError("Invalid limit")
        raw = self.drp.read(addr)
        return (raw>>4)/4096 * 3 # ref: UG480

    def _set_vcc_limit(self, value, rail='vccint', limit='upper'):
        """Set XADC vcc limit.

        Args:
            value (float): voltage limit
            rail (string): 'vccint', 'vccaux', or 'vccbram'
            limit (string): 'upper', 'lower'
        """
        if rail == 'vccint':
            addr = 0x51
        elif rail == 'vccaux':
            addr = 0x52
        elif rail == 'vccbram':
            addr = 0x58
        else:
            raise ValueError("Invalid rail")
        if limit == 'lower':
            addr += 4
        elif limit == 'upper':
            pass
        else:
            raise ValueError("Invalid limit")
        raw = int(value /3*4096) << 4 # ref: UG480
        if raw > 2**16:
            raise ValueError("Out of range")
        self.drp.write(addr, raw)

    def _enable_vcc_alarms(self, enable):
        """Enable or disable XADC vcc alarms.
        """
        addr = 0x41
        val = self.drp.read(addr)
        # VCC alarms are disabled when bits 2, 3 and 8 are set, enabled when they are clear (ref: UG480 Table 3.5)
        mask_enable = 2**16-1 - 2**2 - 2**3 - 2**8
        mask_disable = 2**2 + 2**3 + 2**8
        if enable:
            val &= mask_enable
        else:
            val |= mask_disable
        self.drp.write(addr, val)

    def _user_reset(self):
        """Do a user reset of the XADC.
        This will not clear scope.XADC.errors (use scope.XADC.status = 0 for this), but it will
        clear stored min/max temperature and voltage measurements.
        """
        self.drp.write(0x03, 0xeeee) # (ref: UG480, "XADC JTAG Reset")


class LASettings(util.DisableNewAttr):
    ''' Husky logic analyzer settings. For accessing recorded glitch generation, IO, and USERIO signals.
    '''
    _name = 'Husky Logic Analyzer Setting'

    def __init__(self, oaiface : OAI.OpenADCInterface, mmcm, scope):
        # oaiface = OpenADCInterface
        super().__init__()
        self.oa = oaiface
        self._mmcm = mmcm
        self._scope = scope
        self._is_husky_plus = False
        self.disable_newattr()

    def _dict_repr(self):
        rtn = {}
        rtn['present'] = self.present
        rtn['enabled'] = self.enabled
        rtn['clkgen_enabled'] = self.clkgen_enabled
        rtn['locked'] = self.locked
        rtn['clk_source'] = self.clk_source
        rtn['trigger_source'] = self.trigger_source
        rtn['oversampling_factor'] = self.oversampling_factor
        rtn['sampling_clock_frequency'] = self.sampling_clock_frequency
        rtn['downsample'] = self.downsample
        rtn['capture_group'] = self.capture_group
        rtn['capture_depth'] = self.capture_depth
        if self.enabled:
            rtn['errors'] = self.errors
        return rtn

    def __repr__(self):
        return util.dict_to_str(self._dict_repr())

    def __str__(self):
        return self.__repr__()

    def read_capture(self, source, length=None):
        """Returns captured data for specified signal source.
        What you get depends on the capture group; see the capture_group documentation.

        Args:
           source (int): signal to read
           length (int): number of byte to read. If unspecified, returns the full capture size
                         (which is implementation-dependent and can be learned from capture_depth)

        Returns:
            Numpy array of binary values.

        Raises:
           ValueError: invalid source
        """
        scope_logger.error("Obsolete. Use read_capture_data() and extract() instead.")

    def reset_MMCM(self):
        """Reset the sampling clock's MMCM.
        """
        self._mmcm.drp.reset()


    def trigger_now(self):
        """Trigger the capture manually.
        """
        self.oa.sendMessage(CODE_WRITE, "LA_MANUAL_CAPTURE", [1], Validate=False)
        self.oa.sendMessage(CODE_WRITE, "LA_MANUAL_CAPTURE", [0], Validate=False)


    @staticmethod
    def _bytes_to_bits(bytelist):
        bitlist = []
        for x in bytelist:
            for bit in range(8):
                bitlist.append(x & 0x01)
                x = x >> 1
        return np.asarray(bitlist)

    @property
    def present(self):
        """ Return whether the logic analyzer functionality is present in this build (True or False).
        If it is not present, none of the functionality of this class is available.
        """
        raw = self.oa.sendMessage(CODE_READ, "COMPONENTS_EXIST", Validate=False, maxResp=1)[0]
        if raw & 1:
            return True
        else:
            return False

    @property
    def locked(self):
        """ Return whether the sampling clock MMCM is locked (True or False).
        """
        raw = self.oa.sendMessage(CODE_READ, "LA_ENABLED", Validate=False, maxResp=1)[0]
        if raw & 2:
            return True
        else:
            return False

    @property
    def max_capture_depth(self):
        """Maximum capture depth.
        """
        if self._is_husky_plus:
            return 65535
        else:
            return 16376

    @property
    def capture_depth(self):
        """Number of bits captured for each signal.

        Args:
            depth (int): capture <depth> samples of each signal. 16-bit value, in range [1, scope.LA.max_capture_depth]
        """
        raw = self.oa.sendMessage(CODE_READ, "LA_CAPTURE_DEPTH", Validate=False, maxResp=2)
        return int.from_bytes(raw, byteorder='little')

    @capture_depth.setter
    def capture_depth(self, depth):
        if depth > self.max_capture_depth:
            raise ValueError("Maximum capture depth is %s" % self.max_capture_depth)
        if depth % 2:
            depth -= 1
        self.oa.sendMessage(CODE_WRITE, "LA_CAPTURE_DEPTH", int.to_bytes(depth, length=2, byteorder='little'), Validate=False)


    @property
    def enabled(self):
        """Controls whether this block is active for recording data. The LA
        and trace components share the same FPGA storage, so they cannot be
        simultaneously enabled.
        """
        raw = self.oa.sendMessage(CODE_READ, "LA_ENABLED", Validate=False, maxResp=1)[0] & 0x01
        if raw:
            return True
        else:
            return False

    @enabled.setter
    def enabled(self, enable):
        raw = self.oa.sendMessage(CODE_READ, "LA_ENABLED", Validate=False, maxResp=1)[0]
        if enable:
            val = raw | 0x01
            # only one of Trace/LA can be enabled at once:
            if self._scope.trace.enabled and self._scope.trace.capture.mode != 'off':
                scope_logger.warning("Can't enable scope.LA and scope.trace simultaneously; turning off scope.trace.")
                self._scope.trace.enabled = False
            self.clkgen_enabled = True
        else:
            val = raw & 0xfe
        self.oa.sendMessage(CODE_WRITE, "LA_ENABLED", [val], Validate=False)
        self.reset_MMCM()

    @property
    def clkgen_enabled(self):
        """Controls whether the Xilinx MMCM used to generate the sampling clock
        is powered on or not.  7-series MMCMs are power hungry. In the Husky
        FPGA, MMCMs are estimated to consume close to half of the FPGA's power.
        If you run into temperature issues and don't require the logic analyzer
        or debug trace functionality, power down this MMCM.
        """
        raw = self.oa.sendMessage(CODE_READ, "LA_POWERDOWN", Validate=False, maxResp=1)[0]
        if raw == 1:
            return False
        elif raw == 0:
            return True
        else:
            raise ValueError("Unexpected: read %d" % raw)

    @clkgen_enabled.setter
    def clkgen_enabled(self, enable):
        if enable:
            val = [0]
        else:
            val = [1]
        self.oa.sendMessage(CODE_WRITE, "LA_POWERDOWN", val, Validate=False)
        self.reset_MMCM()


    @property
    def clk_source(self):
        """The clock signal that the logic analyzer is using to generate its sampling clock.

        There are three different sources:
         * "target": The clock from the target device (from HS1 or AUX, as per scope.clock.clkgen_src)
         * "usb": The 96 MHz internal USB clock.
         * "pll": Husky's on-board PLL clock.

        :Getter:
           Return the clock signal currently in use

        :Setter:
           Change the clock source

        Raises:
           ValueError: New value not one of "target", "usb" or "pll"
        """

        return self._getClkSource()

    @clk_source.setter
    def clk_source(self, enable):
        self._setClkSource(enable)
        self.reset_MMCM()

    def arm(self):
        self.oa.sendMessage(CODE_WRITE, "LA_ARM", [1], Validate=False)

    @property
    def errors(self):
        """Indicate whether internal FPGA errors have occurred.
           Write to clear.
        """
        # note this does (almost) the exact same thing as scope.trace.errors
        stat = ""
        fifo_stat = self.oa.sendMessage(CODE_READ, self._scope.trace.REG_SNIFF_FIFO_STAT, Validate=False, maxResp=1)[0]
        if (fifo_stat & 2) >> 1:
            stat += "FIFO underflow, "
        if (fifo_stat & 16) >> 4:
            stat += "FIFO overflow, "
        if stat:
            return stat
        else:
            return None

    @errors.setter
    def errors(self, val):
        self.oa.sendMessage(CODE_WRITE, self._scope.trace.REG_CLEAR_ERRORS, [1], Validate=False)

    def read_capture_data(self, check_empty=False):
        """Read captured data.

        Returns: List of captured entries. Each list element is itself a 3-element list,
        containing the 3 bytes that make up a capture entry. Use extract() to parse.

        """
        # note this does the exact same thing as scope.trace.read_capture_data()
        data = []
        # first check for FIFO to not be empty:
        assert self.fifo_empty() == False, 'FIFO is empty'

        # then check that no underflows or overflows occurred during capture:
        if  self.errors:
            scope_logger.warning("FIFO errors occured: %s" % self.errors)

        if check_empty:
            # check if FIFO is empty before reading every word... much slower! for development
            while not self.fifo_empty():
                data.append(self.oa.sendMessage(CODE_READ, self._scope.trace.REG_SNIFF_FIFO_RD, Validate=False, maxResp=4)[1:4])
        else:
            # 4 bytes of REG_SNIFF_FIFO_RD carries 2 samples of each signal, hence we read capture_depth*2 bytes:
            raw = self.oa.sendMessage(CODE_READ, self._scope.trace.REG_SNIFF_FIFO_RD, Validate=False, maxResp=self.capture_depth*2)
            for i in range(self.capture_depth//2):
                data.append(raw[i*4+1: i*4+4])

        if len(data): # maybe we only got empty reads
            if data[-1][2] & 2**self._scope.trace.FE_FIFO_STAT_UNDERFLOW:
                scope_logger.warning("Capture FIFO underflowed!")

        return data

    @staticmethod
    def extract(raw, index):
        """
        """
        bitlist = []
        if index == 0:
            byte_index = 0
            bit0_index = 0
            bit1_index = 1
        elif index == 1:
            byte_index = 0
            bit0_index = 2
            bit1_index = 3
        elif index == 2:
            byte_index = 0
            bit0_index = 4
            bit1_index = 5
        elif index == 3:
            byte_index = 0
            bit0_index = 6
            bit1_index = 7
        elif index == 4:
            byte_index = 1
            bit0_index = 0
            bit1_index = 1
        elif index == 5:
            byte_index = 1
            bit0_index = 2
            bit1_index = 3
        elif index == 6:
            byte_index = 1
            bit0_index = 4
            bit1_index = 5
        elif index == 7:
            byte_index = 1
            bit0_index = 6
            bit1_index = 7
        elif index == 8:
            byte_index = 2
            bit0_index = 0
            bit1_index = 1
        else:
            raise ValueError
        for i, x in enumerate(raw):
            bitlist.append((x[byte_index] >> bit1_index) & 0x01)
            bitlist.append((x[byte_index] >> bit0_index) & 0x01)
        return np.asarray(bitlist)

    def fifo_empty(self):
        """Returns True if the capture FIFO is empty, False otherwise.
        """
        # note this does the exact same thing as scope.trace.fifo_empty()
        if self.oa.sendMessage(CODE_READ, self._scope.trace.REG_SNIFF_FIFO_STAT, Validate=False, maxResp=1)[0] & 1:
            return True
        else:
            return False


    @property
    def trigger_source(self):
        """The trigger used by the logic analyzer to capture.

        There are several different sources:
         * "glitch": The internal glitch enable trigger, one cycle earlier than the
                     glitch output seen when scope.glitch.output = 'enable_only'. This
                     signal is in the MMCM1 clock glitch domain.
         * "capture": The internal ADC capture trigger.
         * "glitch_source": The internal manual glitch trigger in the source or target clock
                            domain (as per scope.glitch.clk_src), which accounts for 
                            scope.glitch.ext_offset but not scope.glitch.offset. Should
                            only be used with scope.glitch.trigger_src = 'manual'; may
                            not fire reliably with other settings.
         * "glitch_trigger": The internal glitch trigger in the MMCM1 clock domain.
         * "trigger_glitch": The trigger *for* the glitch module (aka scope.trigger.triggers).
         * "HS1": The HS1 input clock.
         * "rising_userio_d[0-7]": a rising edge (0->1) on a USERIO pin
         * "falling_userio_d[0-7]": a falling edge (1->0) on a USERIO pin
         * "rising_tio[0-3]": a rising edge (0->1) on a tio pin
         * "failling_tio[0-3]": a falling edge (0->1) on a tio pin

         In addition, capture can be triggered manually, irrespective of the trigger_source
         setting, by calling scope.LA.trigger_now()

        :Getter:
           Return the trigger source currently in use

        :Setter:
           Change the trigger source

        Raises:
           ValueError: New value not one of the options listed above.
        """

        return self._getTriggerSource()

    @trigger_source.setter
    def trigger_source(self, enable):
        self._setTriggerSource(enable)

    @property
    def oversampling_factor(self):
        """Multiplier for the sampling clock.
        Can be fractional, but an integer is probably what you want.
        Whether the desired oversampling factor can be achieved depends on the
        source clock frequency; a warning is issued if it can't.

        :Getter:
           Return the actual oversampling factor.

        :Setter:
           Set the desired oversampling factor.
        """
        return self._getOversamplingFactor()

    @oversampling_factor.setter
    def oversampling_factor(self, factor):
        self._setOversamplingFactor(factor)

    @property
    def _warning_frequency(self):
        """Convenience function to access scope.trace.clock._warning_frequency
        """
        return self._scope.trace.clock._warning_frequency

    @_warning_frequency.setter
    def _warning_frequency(self, freq):
        self._scope.trace.clock._warning_frequency = freq

    @property
    def sampling_clock_frequency(self):
        """Report the actual sampling clock frequency.
        """
        return self._scope.trace.clock.swo_clock_freq

    @property
    def source_clock_frequency(self):
        """Report the actual clock frequency of the input clock to the shared LA/trace MMCM.
        """
        raw = int.from_bytes(self.oa.sendMessage(CODE_READ, "LA_SOURCE_FREQ", Validate=False, maxResp=4), byteorder='little')
        freq = raw * 96e6 / float(pow(2,23))
        return freq

    @property
    def downsample(self):
        """Downsample setting.

        Args:
            downsample (int): capture every <downsample> samples. 16-bit value, in range [1, 2**16].
        """
        return self._getDownsample()

    @downsample.setter
    def downsample(self, factor):
        self._setDownsample(factor)


    @property
    def capture_group(self):
        """Sets which group of signals are captured.

        There are three groups. The signals captured for each group are as follows:
        'glitch' (group 0):

            #. glitch output
            #. source clock of glitch module
            #. glitch internal MMCM1 (offset) output
            #. glitch internal MMCM2 (width) output
            #. glitch go internal signal
            #. capture trigger
            #. glitch enable
            #. manual glitch trigger in source clock domain (e.g. signal 1 of this group)
            #. glitch trigger in MMCM1 clock domain

        'CW 20-pin' (group 1):

            #. IO1
            #. IO2
            #. IO3
            #. IO4
            #. HS1
            #. HS2
            #. AUX MCX
            #. TRIG MCX
            #. ADC sampling clock

        'USERIO 20-pin' (group 2):

            #. D0
            #. D1
            #. D2
            #. D3
            #. D4
            #. D5
            #. D6
            #. D7
            #. CK

        'trigger debug' (group 3)
        'internal trace 1' (group 4)
        'internal trace 2' (group 5)
        'glitch debug' (group 6)

        :Getter:
           Return the capture group currently in use.

        :Setter:
           Change the capture group.

        Raises:
           ValueError: invalid capture group.
        """
        return self._getCaptureGroup()

    @capture_group.setter
    def capture_group(self, group):
        self._setCaptureGroup(group)


    def _setClkSource(self, source):
        if source == 'target':
            val = [0]
        elif source == 'usb':
            val = [1]
        elif source == 'pll':
            val = [2]
        else:
            raise ValueError("Must be one of 'target', 'usb', or 'pll'")
        self.oa.sendMessage(CODE_WRITE, "LA_CLOCK_SOURCE", val, Validate=False)

    def _getClkSource(self):
        raw = self.oa.sendMessage(CODE_READ, "LA_CLOCK_SOURCE", Validate=False, maxResp=1)[0]
        if raw == 0:
            return 'target'
        elif raw == 1:
            return 'usb'
        elif raw == 2:
            return 'pll'
        else:
            raise ValueError("Unexpected: read %d" % raw)

    def _setTriggerSource(self, source):
        if source == 'glitch':
            val = [0]
        elif source == 'capture':
            val = [1]
        elif source == 'glitch_source':
            val = [2]
        elif source == 'HS1':
            val = [3]
        elif source == 'glitch_trigger':
            val = [4]
        elif source == 'trigger signal 0':
            val = [5]
        elif source == 'trigger signal 1':
            val = [6]
        elif source == 'trigger_glitch':
            val = [7]
        elif 'rising_userio_d' in source:
            val = [0x08 + int(source[-1])]
        elif 'falling_userio_d' in source:
            val = [0x18 + int(source[-1])]
        elif 'rising_tio' in source:
            val = [0x20 + int(source[-1]) - 1]
        elif 'falling_tio' in source:
            val = [0x28 + int(source[-1]) - 1]
        else:
            raise ValueError("Must be one of 'glitch', 'capture', 'glitch_source', 'HS1', '[rising|falling]_userio_d[0-7]', or [rising|falling]_tio[0-3]")
        self.oa.sendMessage(CODE_WRITE, "LA_TRIGGER_SOURCE", val, Validate=False)

    def _getTriggerSource(self):
        raw = self.oa.sendMessage(CODE_READ, "LA_TRIGGER_SOURCE", Validate=False, maxResp=1)[0]
        if raw == 0:
            return 'glitch'
        elif raw == 1:
            return 'capture'
        elif raw == 2:
            return 'glitch_source'
        elif raw == 3:
            return 'HS1'
        elif raw == 4:
            return 'glitch_trigger'
        elif raw == 5:
            return 'trigger signal 0'
        elif raw == 6:
            return 'trigger signal 1'
        elif raw == 7:
            return 'trigger_glitch'
        elif raw in range(0x8, 0x10):
            return 'rising_userio_d' + str(raw & 0x07)
        elif raw in range(0x10, 0x20):
            return 'falling_userio_d' + str(raw & 0x07)
        elif raw in range(0x20, 0x24):
            return 'rising_tio' + str((raw & 0x03) + 1)
        elif raw in range(0x28, 0x2c):
            return 'falling_tio' + str((raw & 0x03) + 1)
        else:
            raise ValueError("Unexpected: read %d" % raw)

    def _setOversamplingFactor(self, factor):
        self._scope.trace.clock.swo_clock_freq = self.source_clock_frequency * factor

    def _getOversamplingFactor(self):
        return self._mmcm.get_mul() // (self._mmcm.get_main_div() * self._mmcm.get_sec_div())

    def _setDownsample(self, factor):
        if factor < 1 or factor > 2**16:
            raise ValueError("Error: downsample value out of range.")
        factor -= 1
        self.oa.sendMessage(CODE_WRITE, "LA_DOWNSAMPLE", int.to_bytes(factor, length=2, byteorder='little'), Validate=False)

    def _getDownsample(self):
        return int.from_bytes(self.oa.sendMessage(CODE_READ, "LA_DOWNSAMPLE", Validate=False, maxResp=2), byteorder='little') + 1


    def _setCaptureGroup(self, group):
        if group == 'glitch':
            num = 0
        elif group == 'CW 20-pin':
            num = 1
        elif group == 'USERIO 20-pin':
            num = 2
        elif group == 'trigger debug':
            num = 3
        elif group == 'internal trace 1':
            num = 4
        elif group == 'internal trace 2':
            num = 5
        elif group == 'glitch debug':
            num = 6
        else:
            raise ValueError("invalid group name")
        self.oa.sendMessage(CODE_WRITE, "LA_CAPTURE_GROUP", [num], Validate=False)

    def _getCaptureGroup(self):
        num = self.oa.sendMessage(CODE_READ, "LA_CAPTURE_GROUP", Validate=False, maxResp=1)[0]
        if num == 0:
            group = 'glitch'
        elif num == 1:
            group = 'CW 20-pin'
        elif num == 2:
            group = 'USERIO 20-pin'
        elif num == 3:
            group = 'trigger debug'
        elif num == 4:
            group = 'internal trace 1'
        elif num == 5:
            group = 'internal trace 2'
        elif num == 6:
            group = 'glitch debug'
        else:
            raise ValueError("invalid group name")
        return group


class ADS4128Settings(util.DisableNewAttr):
    ''' Husky ADS4128 ADC settings. Mostly for testing, not needed in normal use.
    '''
    _name = 'Husky ADS4128 ADC Setting'

    def __init__(self, oaiface : OAI.OpenADCInterface):
        super().__init__()
        # oaiface = OpenADCInterface
        self.oa = oaiface
        self.adc_reset()
        self.set_defaults()
        self.disable_newattr()

    def _dict_repr(self):
        rtn = {}
        rtn['mode'] = self.mode
        rtn['low_speed'] = self.low_speed
        rtn['hi_perf'] = self.hi_perf
        return rtn

    def __repr__(self):
        return util.dict_to_str(self._dict_repr())

    def __str__(self):
        return self.__repr__()

    def adc_reset(self):
        """Resets the ADC.
        Note this is done by the FPGA - see reg_husky_adc.v
        """
        self.oa.sendMessage(CODE_WRITE, "CW_ADC_CTRL", [0x41])
        self.oa.sendMessage(CODE_WRITE, "CW_ADC_CTRL", [0xc1])
        self.oa.sendMessage(CODE_WRITE, "CW_ADC_CTRL", [0x41])

    def _adc_write(self, address, data):
        """Write ADC configuration register.
        Note this is done by the FPGA - see reg_husky_adc.v
        """
        self.oa.sendMessage(CODE_WRITE, "CW_ADC_CTRL", [0x41])
        for i in range(8):
            bit = (address >> (7-i)) & 1
            val = (bit << 4) + 1
            self.oa.sendMessage(CODE_WRITE, "CW_ADC_CTRL", [val])
            val = (bit << 4) + 0
            self.oa.sendMessage(CODE_WRITE, "CW_ADC_CTRL", [val])

        for i in range(8):
            bit = (data >> (7-i)) & 1
            val = (bit << 4) + 1
            self.oa.sendMessage(CODE_WRITE, "CW_ADC_CTRL", [val])
            val = (bit << 4) + 0
            self.oa.sendMessage(CODE_WRITE, "CW_ADC_CTRL", [val])
        self.oa.sendMessage(CODE_WRITE, "CW_ADC_CTRL", [val])

    def _adc_read(self, address):
        """Read ADC configuration register.
        Note this is done by the FPGA - see reg_husky_adc.v
        """
        # first, enable readout:
        self._adc_write(0x0, 0x1)
        self.oa.sendMessage(CODE_WRITE, "CW_ADC_CTRL", [0x41])
        for i in range(8):
            bit = (address >> (7-i)) & 1
            self.oa.sendMessage(CODE_WRITE, "CW_ADC_CTRL", [(bit<<4) + 1])
            self.oa.sendMessage(CODE_WRITE, "CW_ADC_CTRL", [(bit<<4) + 0])
        for i in range(8):
            self.oa.sendMessage(CODE_WRITE, "CW_ADC_CTRL", [0x01])
            self.oa.sendMessage(CODE_WRITE, "CW_ADC_CTRL", [0x02])
        self.oa.sendMessage(CODE_WRITE, "CW_ADC_CTRL", [0x41])
        data =  self.oa.sendMessage(CODE_READ, "CW_ADC_CTRL", maxResp=1)[0]
        # finished, disable readout:
        self._adc_write(0x0, 0x0)
        return data

    def set_defaults(self):
        self.set_normal_settings()
        self.set_low_speed(True)
        self.set_hi_perf(2)
        self._adc_write(0x3d, 0xc0) # set offset binary output

    def set_normal_settings(self):
        self._adc_write(0x42, 0x00) # enable low-latency mode
        self._adc_write(0x25, 0x00) # disable test patterns
        self._adc_write(0x3d, 0xc0) # set offset binary output
        self._mode_cached = "normal"

    def set_test_settings(self, mode):
        self._adc_write(0x42, 0x08) # disable low-latency mode
        self._adc_write(0x3d, 0xc0) # set offset binary output
        if mode == 'test ramp':
            self._adc_write(0x25, 0x04) # set test pattern to ramp
        elif mode == 'test alternating':
            self._adc_write(0x25, 0x03) # set test pattern to alternating 0x555 / 0xaaa
        else:
            raise ValueError
        self._mode_cached = mode

    def set_low_speed(self, val):
        if val:
            self._adc_write(0xdf, 0x30)
            self._low_speed_cached = True
        else:
            self._adc_write(0xdf, 0x00)
            self._low_speed_cached = False

    def set_hi_perf(self, val):
        if val == 0:
            self._adc_write(0x03, 0x00)
            self._adc_write(0x4a, 0x00)
        elif val == 1:
            self._adc_write(0x03, 0x03)
            self._adc_write(0x4a, 0x00)
        elif val == 2:
            self._adc_write(0x03, 0x03)
            self._adc_write(0x4a, 0x01)
        else:
            raise ValueError("Must be 0, 1 or 2")
        self._hi_perf_cached = val


    @property
    def mode(self):
        """The current mode of the ADC.

        :Getter: Return the current ADC operating mode ("normal" or "test ramp")

        :Setter: Set the operating mode.

        Raises:
            ValueError: if mode not one of "normal" or "test ramp"
        """
        return self._mode_cached

    @mode.setter
    def mode(self, val):
        return self.setMode(val)

    def setMode(self, mode):
        if mode == "normal":
            self.set_normal_settings()
            self.oa.sendMessage(CODE_WRITE, "NO_CLIP_ERRORS", [0])
        elif mode in ("test ramp", "test alternating"):
            self.set_test_settings(mode)
            self.oa.sendMessage(CODE_WRITE, "NO_CLIP_ERRORS", [1])
        else:
            raise ValueError("Invalid mode, only 'normal', 'test ramp' or 'test alternating' allowed")


    @property
    def low_speed(self):
        """Whether the ADC is set to "low speed" operation; recommended for sampling rates below 80 MS/s.

        :Getter: Return whether the ADC is set to low speed mode.

        :Setter: Set the low speed mode.
        """
        return self._low_speed_cached

    @low_speed.setter
    def low_speed(self, val):
        return self.set_low_speed(val)

    @property
    def hi_perf(self):
        """High performance mode setting.
        Valid values are 0 (high performance off), 1, and 2. See ADS4128 datasheet for more information.

        :Getter: Return the high performance mode setting.

        :Setter: Set the high performance mode setting.
        """
        return self._hi_perf_cached

    @hi_perf.setter
    def hi_perf(self, val):
        return self.set_hi_perf(val)

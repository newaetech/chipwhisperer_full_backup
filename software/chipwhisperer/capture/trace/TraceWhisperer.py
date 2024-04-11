# pylint skip-file
#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2020-2021, NewAE Technology Inc
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
import time
import datetime
import re
import math
import pkg_resources # type: ignore
import chipwhisperer as cw
from ...common.utils import util
from ...hardware.naeusb.naeusb import NAEUSB
from ...hardware.naeusb.fpga import FPGA

from ..scopes.cwhardware.ChipWhispererHuskyMisc import XilinxDRP, XilinxMMCMDRP
from ...logging import *
from collections import OrderedDict
import numpy as np

CODE_READ       = 0x80
CODE_WRITE      = 0xC0

class TraceWhisperer(util.DisableNewAttr):

    """ Trace interface object.

    This class contains the public API for the Arm Coresight trace sniffing
    hardware, which exists on several platforms:
    - CW-Husky
    - CW305, as DesignStartTrace
    - CW610 (PhyWhisperer)

    Connecting depends on the platform:

        (a) CW-Husky case: available as scope.trace, no additional steps needed.

        (b) CW305 (DesignStart) case:
    
            import chipwhisperer as cw
            from chipwhisperer.capture.trace.TraceWhisperer import TraceWhisperer
            scope = cw.scope()
            target = cw.target(scope, targets.CW305, bsfile=<valid FPGA bitstream file>)
            trace = TraceWhisperer(target, scope)

        (c) CW610 (PhyWhisperer) case:
        
            import chipwhisperer as cw
            from chipwhisperer.capture.trace.TraceWhisperer import TraceWhisperer
            scope = cw.scope()
            target = cw.target(scope)
            trace = TraceWhisperer(target)

    """

    _name = "TraceWhisperer"

    rule_length = [0]*8

    longsync = [255, 255, 255, 127]
    shortsync = [255, 127]

    def __init__(self, target, scope, husky=False, defines_files=None, bs='', force_bitfile=False, trace_reg_select=None, main_reg_select=None):
        """
        Args:
            target: SimpleSerial target object
            scope: CW scope object
            bs (string): FPGA bitfile (default is used if not specified)
            defines_files (list of 2 strings): path to defines_trace.v and defines_pw.v
            force_bitfile (bool): force loading of FPGA bitfile, even if FPGA is already programmed.
            trace_reg_select (int): mapping of FPGA register address space. Don't touch.
            main_reg_select (int): mapping of FPGA register address space. Don't touch.
        """
        super().__init__()
        self._trace_port_width = 4
        self._base_target_clock = 7.384e6
        self._base_baud = 38400
        self._usb_clock = 96e6
        self._uart_clock = self._usb_clock * 2
        self.expected_verilog_defines = 131
        self.swo_mode = False
        self._scope = scope

        if husky:
            self.platform = 'Husky'
            self._ss = target

        # Detect whether we exist on CW305 or CW610 based on the target we're given:
        elif target._name == 'Simple Serial':
            self.platform = 'CW610'
            self._ss = target
            self._naeusb = NAEUSB()
            self._naeusb.con(idProduct=[0xC610])
            # we're using the CW NAEUSB, which has no knowledge of PW firmware, so let's manually
            # check the FW version here:
            fw_latest = [1,1]
            if self._naeusb.readFwVersion()[0] < fw_latest[0]:
                tracewhisperer_logger.warning('Your PhyWhisperer firmware is outdated - latest is %d.%d' % (fw_latest[0], fw_latest[1]) +
                                     '. Suggested to update firmware, as you may experience errors.')

            self._fpga = FPGA(self._naeusb)
            if not self._fpga.isFPGAProgrammed() or force_bitfile:
                if not bs:
                    bs = pkg_resources.resource_filename('chipwhisperer', 'hardware/firmware/tracewhisperer_top.bit')
                self._fpga.FPGAProgram(open(bs, 'rb'), exceptOnDoneFailure=False)

        else:
            self.platform = 'CW305'
            self._ss = cw.target(scope)
            self._naeusb = target._naeusb

        self.slurp_defines(defines_files, trace_reg_select, main_reg_select)
        self.target_registers = ARM_debug_registers(self)
        self.clock = clock(self)
        self.capture = capture(self)

        if husky:
            self.tms_bit = 0
            self.tck_bit = 1
        elif self.board_rev == 3:
            self.tms_bit = 0
            self.tck_bit = 2
            tracewhisperer_logger.warning("Using FPGA bitfile built for rev-3 board, make sure this is what you intend!")
        elif self.board_rev == 4:
            self.tms_bit = 0
            self.tck_bit = 1

        self.pattern_size = self.fpga_read(self.REG_BUFFER_SIZE, 1)[0] # match pattern size, in bytes
        self.disable_newattr()
        self._set_defaults()

    def sendMessage(self, mode, address, payload=None, Validate=False, maxResp=None, readMask=None):
        """ This exists only so that borrowed classes from ChipWhisperer can work. For "native" trace work,
            use fpga_read() / fpga_write() directly instead.
        """
        if Validate or readMask:
            raise ValueError("Not implemented!")
        if mode == CODE_READ:
            return self.fpga_read(address, maxResp)
        elif mode == CODE_WRITE:
            return self.fpga_write(address, payload)


    def _dict_repr(self):
        rtn = OrderedDict()
        if self.platform == 'Husky':
            rtn['present']   = self.present
            rtn['enabled']   = self.enabled
        else:
            rtn['fpga_buildtime']   = self.fpga_buildtime
        rtn['errors']           = self.errors
        rtn['trace_synced']     = self.trace_synced
        rtn['trace_mode']       = self.trace_mode
        if self.trace_mode == 'SWO':
            rtn['swo_div']      = self.swo_div
        else:
            rtn['trace_width']  = self.trace_width
        if self.platform == 'CW610':
            rtn['leds']             = self.leds
        rtn['clock']            = self.clock._dict_repr()
        rtn['capture']          = self.capture._dict_repr()
        if self._ss:
            rtn['target_registers'] = self.target_registers._dict_repr()

        return rtn


    def __repr__(self):
        return util.dict_to_str(self._dict_repr())

    def __str__(self):
        return self.__repr__()


    def _set_defaults(self):
        """ Set some registers which for various reasons don't reset to what we want them to.
        """
        self.fpga_write(self.REG_CAPTURE_WHILE_TRIG, [1])
        # this is a register which, in Husky, is changed by scope.userio.mode = 'swo_trace_plus_debug'; 
        # that setting would prevent parallel trace from working, so let's make sure we have the proper
        # value whenever we connect:
        self.fpga_write(self.REG_TRACE_USERIO_DIR, [3]) 

    def _set_userio_dir(self, val):
        self.fpga_write(self.REG_TRACE_USERIO_DIR, [val])


    def reset_fpga(self):
        """ Reset FPGA registers to defaults, use liberally to clear incorrect states.
            On the CW305, this resets the full FPGA, including the target Arm core.
        """
        self.fpga_write(self.REG_RESET_REG, [1])
        self.fpga_write(self.REG_RESET_REG, [0])
        self._set_defaults()


    def slurp_defines(self, defines_files=None, trace_reg_select=None, main_reg_select=None):
        """ Parse Verilog defines file so we can access register and bit
        definitions by name and avoid 'magic numbers'.
        """
        self.verilog_define_matches = 0
        if not defines_files:
            defines_files = [pkg_resources.resource_filename('chipwhisperer', 'capture/trace/defines/defines_trace.v'),
                             pkg_resources.resource_filename('chipwhisperer', 'capture/trace/defines/defines_pw.v')]
        for i,defines_file in enumerate(defines_files):
            defines = open(defines_file, 'r')
            define_regex_base  =   re.compile(r'`define')
            define_regex_reg   =   re.compile(r'`define\s+?REG_')
            define_regex_radix =   re.compile(r'`define\s+?(\w+).+?\'([bdh])([0-9a-fA-F]+)')
            define_regex_noradix = re.compile(r'`define\s+?(\w+?)\s+?(\d+?)')
            for define in defines:
                if define_regex_base.search(define):
                    reg = define_regex_reg.search(define)
                    match = define_regex_radix.search(define)
                    if reg:
                        if i == 0:
                            if trace_reg_select:
                                select = trace_reg_select
                            else:
                                select = self.TRACE_REG_SELECT
                        else:
                            if main_reg_select:
                                select = main_reg_select
                            else:
                                select = self.MAIN_REG_SELECT
                        block_offset = select << 6
                    else:
                        block_offset = 0
                    if match:
                        self.verilog_define_matches += 1
                        if match.group(2) == 'b':
                            radix = 2
                        elif match.group(2) == 'h':
                            radix = 16
                        else:
                            radix = 10
                        setattr(self, match.group(1), int(match.group(3),radix) + block_offset)
                    else:
                        match = define_regex_noradix.search(define)
                        if match:
                            self.verilog_define_matches += 1
                            setattr(self, match.group(1), int(match.group(2),10) + block_offset)
                        else:
                            tracewhisperer_logger.warning("Couldn't parse line: %s", define)
            defines.close()
        # make sure everything is cool:
        if self.verilog_define_matches != self.expected_verilog_defines:
            tracewhisperer_logger.warning("Trouble parsing Verilog defines file (%s): didn't find the right number of defines; expected %d, got %d" % (defines_file, self.expected_verilog_defines, self.verilog_define_matches))


    @property
    def present(self):
        """ Return whether the trace functionality is present in this build (True or False).
        If it is not present, none of the functionality of this class is available.
        """
        if self.platform == 'Husky':
            raw = self.fpga_read("COMPONENTS_EXIST", 1)[0]
            if raw & 2:
                return True
            else:
                return False
        else:
            return True


    @property 
    def enabled(self):
        """Controls whether trace data collecting is enabled or not. Mostly affects configuration
        of the front 20-pin header.

        Args:
            enable (bool)
        """
        raw = self.fpga_read(self.REG_TRACE_EN, 1)[0]
        if raw == 1:
            return True
        else:
            return False

    @enabled.setter 
    def enabled(self, enable):
        # only one of Trace/LA can be enabled at once:
        if enable and self.platform == 'Husky':
            if self._scope.LA.enabled:
                scope_logger.warning("Can't enable scope.LA and scope.trace simultaneously; turning off scope.LA.")
                self._scope.LA.enabled = False
            self._scope.LA.clkgen_enabled = True
        if not enable:
            self.capture.use_husky_arm = False
        self._set_enabled(enable)

    def _set_enabled(self, enable):
        self.fpga_write(self.REG_TRACE_EN, [enable])

    @property 
    def target(self):
        """Set the target object. Not strictly necessary for TraceWhisperer operation; it is used for
        setting/getting the target debug registers, which is done with SimpleSerial communication with
        the target. If you don't require this, then you don't need to set this property.

        Args:
            target: SimpleSerial target object
        """
        return self._ss

    @target.setter 
    def target(self, target):
        self._ss = target


    @property 
    def trace_mode(self):
        """Set trace or SWO mode. SWO mode is only available on the Husky and CW610 platforms.

        For SWO mode, the following connections are needed, from the target to the Husky or CW610 front header:
            - TMS to D0
            - TCK to D1
            - TDO to D2
        For trace mode, the following connections are needed, from the target to the Husky or CW610 front header:
            - TRACEDATA[0] to D4
            - TRACEDATA[1] to D5
            - TRACEDATA[2] to D6
            - TRACEDATA[3] to D7
            - TRACECLOCK   to CK

        Args:
            mode (string): 'parallel' or 'swo'

        """
        if self.swo_mode:
            return "SWO"
        else:
            return "parallel"

    @trace_mode.setter 
    def trace_mode(self, mode):
        if mode in ['swo', 'SWO']:
            if self.platform == 'CW305':
                tracewhisperer_logger.error('CW305 does not support SWO mode')
            self.swo_mode = True
            self.fpga_write(self.REG_SWO_ENABLE, [1])
            if self._ss: # don't want to log an error if no target
                self.target_registers.TPI_SPPR = '00000002'
            if self.platform == 'Husky':
                assert self._scope.LA.present, 'Cannot use this operation mode without the LA component.'
                # disable LA, 
                self._scope.LA.enabled = False
        elif mode == "parallel":
            self.swo_mode = False
            self.fpga_write(self.REG_SWO_ENABLE, [0])
            if self._ss: # don't want to log an error if no target
                self.target_registers.TPI_SPPR = '00000000'
        else:
            raise ValueError('Invalid mode (swo/parallel)')



    def set_trace_mode(self, mode, swo_div=8, acpr=0):
        """Set trace or SWO mode. SWO mode is only available on CW610 platform.
        For SWO mode, we also adjust the target clock to match the SWO parameters.

        Args:
            mode (string): 'trace' or 'swo'
            swo_div (int): number of 96 MHz clock cycles per SWO bit (SWO mode only)
            acpr (int): value for TPI.ACPR register (SWO mode only)
        """
        tracewhisperer_logger.warning('Deprecated, use trace.trace_mode instead.')
        if mode == 'trace':
            self.swo_mode = False
            self.target_registers.TPI_SPPR = '00000000'
            self.fpga_write(self.REG_SWO_ENABLE, [0])
        elif mode in ['swo', 'SWO']:
            if self.platform == 'CW305':
                tracewhisperer_logger.error('CW305 does not support SWO mode')
            self.swo_mode = True
            self.target_registers.TPI_SPPR = '00000002'
            self.target_registers.TPI_ACPR = acpr
            self.fpga_write(self.REG_SWO_BITRATE_DIV, int.to_bytes([swo_div-1], length=2, byteorder='little')) # not a typo: hardware requires -1; doing this is easier than fixing the hardware

            self.fpga_write(self.REG_SWO_ENABLE, [1])
            # Next we set the target clock and update CW baud rate accordingly:
            new_target_clock = int(self._uart_clock / (swo_div * (acpr+1)))
            self._scope.clock.clkgen_freq = new_target_clock
            self._ss.baud = int(self._base_baud * (new_target_clock/self._base_target_clock))
            #self.swo_target_clock_ratio = self._usb_clock / new_target_clock
            tracewhisperer_logger.info("Ensure target is in SWD mode, e.g. using jtag_to_swd().")
        else:
            tracewhisperer_logger.error('Invalid mode %s: specify "trace" or "swo"', mode)

    @property
    def swo_div(self):
        """Set the number of swo_clock cycles per SWO bit.

        Args:
            div (int): number of cycles per SWO bit.
        """
        return int.from_bytes(self.fpga_read(self.REG_SWO_BITRATE_DIV, 2), byteorder='little') + 1 # not a typo: hardware requires -1; doing this is easier than fixing the hardware

    @swo_div.setter
    def swo_div(self, div):
        if div < 1:
            raise ValueError
        return self.fpga_write(self.REG_SWO_BITRATE_DIV, int.to_bytes(div-1, length=2, byteorder='little')) # not a typo: hardware requires -1; doing this is easier than fixing the hardware

    @property
    def uart_state(self):
        """ Return the hardware UART state. For debug.
        """
        raw = self.fpga_read(self.REG_UART, 1)[0]
        # state names from uart_core.v:
        if   raw == 0: state ='ERX_IDLE'
        elif raw == 1: state ='ERX_START'
        elif raw == 2: state ='ERX_BITS'
        elif raw == 3: state ='ERX_STOP'
        elif raw == 4: state ='ERX_SYN'
        else:
            raise ValueError("Got unknown UART state: %d" % raw)
        return state

    def _uart_reset(self):
        """ Reset the hardware UART FSM. Shouldn't be needed!
        """
        self.fpga_write(self.REG_UART, [1])


    @property
    def leds(self):
        """Set the meaning of the armed/capturing LEDs.

        Args:
            mode (str): "normal": as labeled (armed/capturing)
                        "hearbeat": armed = front-end clock heartbeat; capturing = trace clock heartbeat
        """
        if self.platform == 'CW305':
            return 'normal'
        else:
            raw = self.fpga_read(self.REG_LED_SELECT, 1)[0]
            if raw == 0:
                return 'normal'
            elif raw == 1:
                return 'hearbeat'
            else:
                raise ValueError

    @leds.setter
    def leds(self, mode):
        if mode == 'normal':
            val = 0
        elif mode == 'heartbeat':
            val = 1
        else:
            raise ValueError
        return self.fpga_write(self.REG_LED_SELECT, [val])


    @property
    def trace_width(self):
        """Set the parallel trace port width.

        Args:
            width (int): width of the trace port.
        """
        return self.fpga_read(self.REG_TRACE_WIDTH, 1)[0]

    @trace_width.setter
    def trace_width(self, width):
        if width not in [1,2,4]:
            raise ValueError("Unsupported trace width. Must be 1, 2 or 4.")
        if width != 4:
            tracewhisperer_logger.warning("Widths different than 4 may not work. Don't forget to set TPI_CSPSR properly.")
        return self.fpga_write(self.REG_TRACE_WIDTH, [width])

    def check_clocks(self):
        tracewhisperer_logger.warning('Deprecated, use trace.clock.swo_clock_locked instead.')
        return self.clock.swo_clock_locked

    def set_capture_mode(self, mode, counts=0):
        """Determine the duration of the trace capture.

        Args:
            mode (string): 'while_trig' or 'count_cycles' or 'count_writes'
            counts (int): number of cycles (mode == 'count_cycles') or writes (mode == 'count_writes') to capture for (0 = capture until full)
        """
        tracewhisperer_logger.warning('Deprecated; use trace.capture.capture_mode / capture_count instead.')
        if mode == 'while_trig':
            self.fpga_write(self.REG_CAPTURE_WHILE_TRIG, [1])
        elif mode == 'count_cycles':
            self.fpga_write(self.REG_CAPTURE_WHILE_TRIG, [0])
            self.fpga_write(self.REG_COUNT_WRITES, [0])
            self.fpga_write(self.REG_CAPTURE_LEN, int.to_bytes(counts, length=4, byteorder='little'))
        elif mode == 'count_writes':
            self.fpga_write(self.REG_CAPTURE_WHILE_TRIG, [0])
            self.fpga_write(self.REG_COUNT_WRITES, [1])
            self.fpga_write(self.REG_CAPTURE_LEN, int.to_bytes(counts, length=4, byteorder='little'))
        else:
            tracewhisperer_logger.error('Invalid mode %s')


    @property
    def board_rev(self):
        """Obtain board revision from the FPGA bitfile.
        """
        if self.platform == 'Husky':
            tracewhisperer_logger.warning("N/A for Husky")
            return None
        else:
            return self.fpga_read(self.REG_BOARD_REV, 1)[0]


    def jtag_to_swd(self):
        """Switch to SWD mode by driving the JTAG-to-SWD sequence on TMS/TCK.
        (reference: https://developer.arm.com/documentation/ka001179/1-0/)

        Args: none
        """
        if self.platform == 'Husky':
            reg_pwdriven = "USERIO_CW_DRIVEN"
            reg_data = "USERIO_DRIVE_DATA"
        else:
            reg_pwdriven = self.REG_USERIO_PWDRIVEN
            reg_data = self.REG_USERIO_DATA

        self.fpga_write(reg_pwdriven, [(1<<self.tms_bit) + (1<<self.tck_bit)])
        self.fpga_write(reg_data, [1<<self.tms_bit])
        self._line_reset(reg_data)
        self._send_tms_byte(reg_data, 0x9e)
        self._send_tms_byte(reg_data, 0xe7)
        self._line_reset(reg_data)
        self.fpga_write(reg_data, [1<<self.tms_bit])
        self.fpga_write(reg_pwdriven, [0])


    def _send_tms_byte(self, addr, data):
        """Bit-bang 8 bits of data on TMS/TCK (LSB first).

        Args:
            data (int): 8 bits data to send.
        """
        for i in range(8):
            bit = (data & 2**i) >> i
            self.fpga_write(addr, [bit<<self.tms_bit])
            self.fpga_write(addr, [(1<<self.tck_bit) + (bit<<self.tms_bit)])


    def _line_reset(self, addr, num_bytes=8):
        """Bit-bang a line reset on TMS/TCK.

        Args: none
        """
        for i in range(num_bytes): self._send_tms_byte(addr, 0xff)



    def simpleserial_write(self, cmd, data, printresult=False, wait=0.6):
        """Convenience function to send a simpleserial command to the simpleserial target,
        and optionally fetch and print the result.
        """
        if self._ss is None:
            tracewhisperer_logger.error("Target must be connected for this to work (e.g. scope.trace.target = target)")
        else:
            self._ss.simpleserial_write(cmd, data)
            if printresult:
                time.sleep(wait)
                print(self._ss.read().split('\n')[0])

    def _words2bytes(self, words):
        """Converts a list of self.data_bits-sized words into a list of bytes, for programming
        the REG_TRACE_PATTERNx and REG_TRACE_MASKx registers.
        """
        word_size = self.data_bits
        total_bits = self.pattern_size * 8
        bigword = 0
        for i,w in enumerate(words):
            bigword >>= word_size
            bigword += (w << (total_bits - word_size))
            #print('Added %s: %s' % (hex(w), hex(bigword)))
        result = []
        #while bigword:
        for _ in range(self.pattern_size):
            result.append(bigword & 0xff)
            bigword >>= 8
        return result

    def set_pattern_match(self, index, pattern, mask=None, enable_rule=True):
        """Sets pattern match and mask parameters.

        Args:
            index: match index [0-7]
            pattern: list of 8-bit integers, pattern match value. Maximum size given by self.pattern_size.
                If fewer than self.pattern_size bytes are given, the list is expanded to self.pattern_size
                by *prepending* the required number of zeros (see usage notes below for implications of
                this for short patterns).
            mask (list, optional): list of bytes, must have same size as 'pattern' if
                set. Defaults to [0]*(self.pattern_size*8-len(pattern) + [0xff]*len(pattern) if not set.
                See usage notes below for implications of this for short patterns.

        Usage notes:
            The pattern matching logic looks at the full match pattern and mask, including mask bytes which
            are set to 0. For example, pattern = [1,2,3,4,0,0,0,0], mask = [255,255,255,255,0,0,0,0] will
            trigger a match when 8 bytes ([1,2,3,4] followed by 4 don't care bytes) have been received 
            (this example assumes self.pattern_size = 8).  If only [1,2,3,4] is received (no other data 
            follows), no match will be triggered.
            If the message you which to trigger on is shorter than self.pattern_size, you must set the don't
            care bytes at the *start* of the pattern and mask; for example: pattern = [0,0,0,0,1,2,3,4],
            mask = [0,0,0,0,255,255,255,255] will trigger a match immediately after [1,2,3,4] is received
            (even if no valid data is received prior to this).
        """
        # Since this also gets used by generic UART, we can't assume that word size is 8 bits.
        # Translate pattern (and mask, if provided) to bytes:
        if len(pattern) > self.pattern_size:
            raise ValueError('pattern and mask cannot be more than 64 bytes.')
        pattern_converted = self._words2bytes(pattern)
        if mask:
            if len(pattern) != len(mask):
                raise ValueError('pattern and mask must be of same size.')
        else:
            mask = [2**self.data_bits-1]*len(pattern)

        mask_converted = self._words2bytes(mask)

        self.fpga_write(self.REG_TRACE_PATTERN0+index, pattern_converted)
        self.fpga_write(self.REG_TRACE_MASK0+index, mask_converted)
        # count trailing zeros in the mask, as these determine how much time elapses from
        # the start of receiving a trace packet, until the match is determined -- so that the
        # recorded timestamp can be rolled back to when the trace packet began
        trailing_zeros = 0
        for m in mask_converted[::-1]:
            if not m:
                trailing_zeros += 1
        self.rule_length[index] = 8-trailing_zeros
        if enable_rule:
            rawrules = self.fpga_read(self.REG_PATTERN_ENABLE, 1)[0]
            rawrules |= 2**index
            self.fpga_write(self.REG_PATTERN_ENABLE, [rawrules])


    def arm_trace(self, check_uart=True):
        """Arms trace sniffer for capture; also checks sync status.
        When used as part of Husky, it's possible for forego this and have the
        trace module be armed by the regular Husky arm, by setting
        :code:`scope.trace.capture.use_husky_arm` to True.

        Args:
            check_uart (bool): check that the hardware UART state machine is not stuck,
            and if it is, reset it. Should not be required unless trace is left enabled
            when not used. Trace clock needs to be active for this to work.
        """
        assert self.trace_synced, 'Not synchronized!'
        assert self.enabled, 'Not enabled!'
        if check_uart and self.swo_mode:
            if self.uart_state != 'ERX_IDLE':
                tracewhisperer_logger.warning("UART appears stuck, resetting it...")
                self._uart_reset()
                assert self.uart_state == 'ERX_IDLE', 'UART is still stuck!'
        self.fpga_write(self.REG_ARM, [1])

    @property
    def errors(self):
        """Indicate whether internal FPGA errors have occurred.
           Write to clear.

        Error types and their causes:
            * 'presample error': capture trigger occurs before the requested
            * 'SWO internal CDC error': data is coming in faster than it can be
                    collected; this may be caused by incorrect scope.trace.clock
                    settings.
            * 'FIFO underflow': host tried to read more samples than are
                    available.
            * 'FIFO overflow': exceeded sample storage capacity; shorten the
                    capture.
 
        """
        stat = ""
        if self.fpga_read(self.REG_STAT, 1)[0]:
            stat = "SWO internal CDC error, "
        fifo_stat = self.fpga_read(self.REG_SNIFF_FIFO_STAT, 1)[0]
        if (fifo_stat & 2) >> 1:
            stat += "FIFO underflow, "
        if (fifo_stat & 16) >> 4:
            stat += "FIFO overflow, "
        if stat:
            return stat
        else:
            return False

    @errors.setter
    def errors(self, val):
        self.fpga_write(self.REG_CLEAR_ERRORS, [1])


    @property
    def trace_synced(self):
        """Check whether:
        1. the chosen front-end clock is alive;
        2. for parallel trace mode, whether we are seeing valid sync frames;
        3. for SWO mode, whether UART settings are what they should be (Husky platform only)
        """
        if not self.clock.fe_clock_alive:
            tracewhisperer_logger.error('Front-end clock is not alive! Check scope.trace.clock settings.')
        if self.swo_mode and (self.platform == 'Husky') and (self.parity != 'none' or self.stop_bits != 1 or self.data_bits != 8):
            tracewhisperer_logger.warning('UART Rx is not set to 8-N-1; this will prevent correct trace operation. Run restore_uart() to resolve this.')
        if self.fpga_read(self.REG_SYNCHRONIZED, 1)[0] == 1:
            return True
        else:
            return False

    def restore_uart(self):
        """Convenience method to set the UART receiver settings to 8-N-1,
        as required by trace in SWO mode (8 bits data, no parity, 1 stop bit).
        """
        if self.platform == 'Husky':
            self.parity = 'none'
            self.stop_bits = 1 
            self.data_bits = 8
        else:
            tracewhisperer_logger.warning("Not supported on this platform.")


    def resync(self):
        """Force trace sniffer to resynchronize (using sync frames that are
        continously emitted on the parallel trace port). Failure could be from
        absence of a trace clock, or mis-sampling of trace data due to
        setup/hold violations (clock edge too close to data edge).
        """
        self.fpga_write(self.REG_TRACE_RESET_SYNC, [1])
        assert self.trace_synced, 'Not synchronized!'


    def is_done(self):
        """Calls SimpleSerial target's is_done().
        """
        if self._ss:
            return self._ss.is_done()
        else:
            tracewhisperer_logger.error("Target must be connected for this to work (e.g. scope.trace.target = target)")
            return False


    def fpga_write(self, addr, data):
        """Write to an FPGA register.

        Args:
            addr (int): Address to write to
            data (list): Data to write to addr
        """
        # on CW305, change word address to byte address (CW610 uses addressing differently)
        if self.platform == 'CW305':
            addr = addr << 7
        if self.platform == 'Husky':
            return self._scope.sc.sendMessage(CODE_WRITE, addr, data)
        else:
            return self._naeusb.cmdWriteMem(addr, data)


    def fpga_read(self, addr, readlen=4):
        """Read from an FPGA register.

        Args:
            addr (int): Address to read from
            readlen (int): Length of data to read

        Returns:
            Requested data as a list
        """
        # on CW305, change word address to byte address (CW610 uses addressing differently)
        if self.platform == 'CW305':
            addr = addr << 7
        if self.platform == 'Husky':
            data = self._scope.sc.sendMessage(CODE_READ, addr, maxResp=readlen)
        else:
            data = self._naeusb.cmdReadMem(addr, readlen)
        return data


    def check_fifo_errors(self, underflow=0, overflow=0):
        """Check whether an underflow or overflow occured on the capture FIFO.

        Args:
            underflow (int, optional): expected status, 0 or 1
            overflow (int, optional): expected status, 0 or 1
        """
        tracewhisperer_logger.warning('Deprecated; use trace.errors instead.')
        status = self.fpga_read(self.REG_SNIFF_FIFO_STAT, 1)[0]
        fifo_underflow = (status & 2) >> 1
        fifo_overflow = (status & 16) >> 4
        assert fifo_underflow == underflow
        assert fifo_overflow == overflow


    def fifo_empty(self):
        """Returns True if the capture FIFO is empty, False otherwise.
        """
        if self.fpga_read(self.REG_SNIFF_FIFO_STAT, 1)[0] & 1:
            return True
        else:
            return False


    @property
    def fpga_buildtime(self):
        """Returns date and time when FPGA bitfile was generated.
        """
        raw = self.fpga_read(addr=self.REG_BUILDTIME, readlen=4)
        # definitions: Xilinx XAPP1232
        day = raw[3] >> 3
        month = ((raw[3] & 0x7) << 1) + (raw[2] >> 7)
        year = ((raw[2] >> 1) & 0x3f) + 2000
        hour = ((raw[2] & 0x1) << 4) + (raw[1] >> 4)
        minute = ((raw[1] & 0xf) << 2) + (raw[0] >> 6)
        return "{}/{}/{}, {:02d}:{:02d}".format(month, day, year, hour, minute)

    def get_fpga_buildtime(self):
        tracewhisperer_logger.warning('Deprecated; use trace.fpga_buildtime instead.')
        return self.fpga_buildtime


    def get_fw_buildtime(self):
        """Returns date and time when target FW was compiled.
        """
        if self._ss is None:
            tracewhisperer_logger.error("Target must be connected for this to work (e.g. scope.trace.target = target)")
            return None
        else:
            self._ss.simpleserial_write('i', b'')
            time.sleep(0.1)
            return self._ss.read().split('\n')[0]


    def phywhisperer_name(self):
        """Returns project-specific 'name' string embedded in PhyWhisperer bitfile
        """
        nameb = self.fpga_read(self.REG_NAME, 8)
        names = ''
        for i in nameb:
            names += hex(i)[2:]
        return bytearray.fromhex(names).decode()


    def test_itm(self, port=1):
        """Print test string via ITM using specified port number.

        Args:
            port (int): ITM port number to use.

        """
        if self._ss is None:
            tracewhisperer_logger.error("Target must be connected for this to work (e.g. scope.trace.target = target)")
        else:
            self._ss.simpleserial_write('t', bytearray([port]))
            time.sleep(0.1)
            print(self._ss.read().split('\n')[0])


    def read_capture_data(self):
        """Read captured trace data.

        Returns: List of captured entries. Each list element is itself a 3-element list,
        containing the 3 bytes that make up a capture entry. Can be parsed by get_rule_match_times()
        or get_raw_trace_packets(). See defines_trace.v for definition of the FIFO
        data fields.

        """
        data = []
        starttime = time.time()

        # first check for FIFO to not be empty:
        assert self.fifo_empty() == False, 'FIFO is empty'

        # then check that no underflows or overflows occurred during capture:
        if  self.errors:
            tracewhisperer_logger.warning("FIFO errors occured: %s" % self.errors)

        while not self.fifo_empty():
            data.append(self.fpga_read(self.REG_SNIFF_FIFO_RD, 4)[1:4])

        if len(data): # maybe we only got empty reads
            if data[-1][2] & 2**self.FE_FIFO_STAT_UNDERFLOW:
                tracewhisperer_logger.warning("Capture FIFO underflowed!")

        return data


    def print_raw_data(self, rawdata):
        """Prints collected raw data in hexadecimal. Raw data includes data
        type, timestamp, and payload. See defines_trace.v for bitfield
        definitions.
        """
        for e in rawdata:
            entry = 0
            entry += (e[2] & 0x3) << 16
            entry += e[1] << 8
            entry += e[0]
            print('%05x' % entry)


    def get_rule_match_times(self, rawdata, rawtimes=False, verbose=False):
        """Split raw capture data into data events and times, stat events and times.

        Args:
            rawdata: raw capture data, list of lists, e.g. obtained from read_capture_data()
            rawtimes:
                True: return reported times (obtained at the *end* of the pattern match)
                False: roll back times to the *start* of the pattern match
            verbose: print timestamped rules
        Returns:
            list of [time, rule index] tuples
        """

        times = []
        timecounter = 0
        lasttime = 0
        lastadjust = 0
        for raw in rawdata:
            command = raw[2] & 0x3
            if command == self.FE_FIFO_CMD_DATA:
                timecounter += raw[0]
                data = raw[1]
                rule = int(math.log2(data))
                if rawtimes:
                    adjust = 0
                else:
                    adjust = self.rule_length[rule]*self._cycles_per_byte()
                timecounter = timecounter - adjust + lastadjust
                delta = timecounter - lasttime
                lasttime = timecounter
                lastadjust = adjust
                if verbose:
                    print("%8d rule # %d, delta = %d" % (timecounter, rule, delta))
                times.append([timecounter, rule])
            elif command == self.FE_FIFO_CMD_TIME:
                timecounter += raw[0] + (raw[1] << 8)
            elif command == self.FE_FIFO_CMD_STAT:
                tracewhisperer_logger.error("Unexpected STAT command, not supported by this method; maybe try get_raw_trace_packets() instead?")
            elif command == self.FE_FIFO_CMD_STRM:
                pass
        return times


    def _cycles_per_byte(self):
        """Returns number of clock cycles needed to send one byte of trace
        data over the trace or SWO port.
        """
        if self.swo_mode:
            return 8
        else:
            return 8/self._trace_port_width


    def get_raw_trace_packets(self, rawdata, removesyncs=True, verbose=False):
        """Split raw capture data into pseudo-frames, optionally suppressing
        sync frames (and using those sync frames as marker which is separating
        pseudo-frames). It's the best we can do without actually parsing the
        trace packets, which is best left to other tools!

        Args:
            rawdata: raw capture data, list of lists, e.g. obtained from read_capture_data()
            verbose: print timestamped packets
        Returns:
            list of pseudo-frames
        """

        pseudoframes = []
        pseudoframe = []
        timecounter = 0
        lasttime = 0
        for raw in rawdata:
            command = raw[2] & 0x3
            if command == self.FE_FIFO_CMD_STAT:
                timecounter += raw[0]
                data = raw[1]
                if not len(pseudoframe):
                    starttime = timecounter
                pseudoframe.append(data)

                if removesyncs:
                    if pseudoframe[-len(self.longsync):] == self.longsync:
                        pseudoframe = pseudoframe[:-len(self.longsync)]
                        sync_removed = True
                        #print('Removed long')
                    elif pseudoframe[-len(self.shortsync):] == self.shortsync:
                        pseudoframe = pseudoframe[:-len(self.shortsync)]
                        sync_removed = True
                        #print('Removed short')
                    else:
                        sync_removed = False
                else:
                    sync_removed = False

                if sync_removed and len(pseudoframe):
                    pseudoframes.append([starttime, pseudoframe])
                    if verbose:
                        print("Pseudoframe: ", end='')
                        for b in pseudoframe:
                            print('%02x ' % b, end='')
                        print();
                    pseudoframe = []

                delta = timecounter - lasttime
                lasttime = timecounter

            elif command == self.FE_FIFO_CMD_TIME:
                timecounter += raw[0] + (raw[1] << 8)
            elif command == self.FE_FIFO_CMD_DATA:
                raise ValueError("Unexpected DATA command, not supported by this method; maybe try get_rule_match_times() instead?")
            elif command == self.FE_FIFO_CMD_STRM:
                pass

        if not removesyncs:
            pseudoframes.append([starttime, pseudoframe])

        return pseudoframes



    def use_soft_trigger(self):
        """ Use target-generated trigger to initiate trace capture.
        """
        tracewhisperer_logger.warning('Deprecated; use trace.capture.trigger_source instead.')
        self.fpga_write(self.REG_SOFT_TRIG_ENABLE, [1])
        self.fpga_write(self.REG_SOFT_TRIG_PASSTHRU, [1])
        self.fpga_write(self.REG_PATTERN_TRIG_ENABLE, [0])


    def use_trace_trigger(self, rule=0):
        """ Use matching trace data to initiate trace capture.

        Args:
            rule (int): rule number to use
        """
        tracewhisperer_logger.warning('Deprecated; use trace.capture.trigger_source instead.')
        self.fpga_write(self.REG_SOFT_TRIG_ENABLE, [0])
        self.fpga_write(self.REG_SOFT_TRIG_PASSTHRU, [0])
        self.fpga_write(self.REG_PATTERN_TRIG_ENABLE, [2**rule])
        self.fpga_write(self.REG_TRIGGER_ENABLE, [1])
        # these can be customized but let's start you off with simple default values:
        self.fpga_write(self.REG_NUM_TRIGGERS, [1])
        self.fpga_write(self.REG_TRIGGER_WIDTH, [16])


    def set_isync_matches(self, addr0=0, addr1=0, match=None):
        """ Set exact PC address matching rules.

        Args:
            addr0 (int): Matching address 0 (DWT_COMP0)
            addr1 (int): Matching address 0 (DWT_COMP1)
            match:
                None: disable PC address match packets
                0: enable addr0 matching only
                1: enable addr1 matching only
                "both": enable both addr0 and addr1 matching
        """
        self.target_registers.DWT_COMP0 = addr0
        self.target_registers.DWT_COMP1 = addr1
        if match == None:
            self.target_registers.ETM_TEEVR = '00000000'
        elif match == 0:
            self.target_registers.ETM_TEEVR = '00000020'
        elif match == 1:
            self.target_registers.ETM_TEEVR = '00000021'
        elif match == 'both':
            self.target_registers.ETM_TEEVR = '000150a0'


    def set_periodic_pc_sampling(self, enable=1, cyctap=0, postinit=1, postreset=0):
        """ Set periodic PC sampling parameters. Enabling PC sampling through
        this method will start PC sampling *after* the target triggers, thereby
        ensuring that the resulting trace data can be parsed without trouble.
        Alternatively, you can set the DWT_CTRL register directly.

        Args:
            enable (int): enable or disable periodic PC sampling
            cyctap (int): DWT_CTRL.CYCTAP bit
            postinit (int): DWT_CTRL.POSTINIT bits
            postreset (int): DWT_CTRL.POSTRESET bits
        """
        if self._ss is None:
            tracewhisperer_logger.error("Target must be connected for this to work (e.g. scope.trace.target = target)")
        else:
            self.target_registers.cached_values[self.target_registers.regs['DWT_CTRL']] = None # this may change DTW_CTRL, so uncache it
            self.simpleserial_write('c', bytearray([enable, cyctap, postinit, postreset]), printresult=False)


    def write_raw_capture(self, raw, filename='raw.bin', presyncs=8):
        """Writes raw trace data to a file (which can be read by orbuculum).
        Prepends a number of sync frames to facilitate parsing.

        Args:
            raw (array): raw trace data as obtained from
                get_raw_trace_packets()
            filename (string): output file
            presyncs (int): number of long syncronization frames which are
                prepended to the collected trace data.
        """
        binout = open(filename, "wb")
        for i in range(presyncs):
            binout.write(bytes(self.longsync))
        for frame in raw:
            binout.write(bytes(frame[1]))
        binout.close()

    @property 
    def parity(self):
        """Parity setting for the UART receiver. Only available on the Husky
        platform.  For trace operation this should always be set to 'none'. The
        option to turn on parity is provided because on the Husky platform, the
        same UART receiver that is used for trace is also used as a receiver
        for triggering on generic UART traffic.

        Args:
            value: 'none' / 'even' / 'odd'
        """
        if self.platform == 'Husky':
            raw = self.fpga_read(self.REG_UART_PARITY_SETTING, 1)[0] & 0x03
            if raw == 0:
                return 'none'
            elif raw == 3:
                return 'odd'
            elif raw == 2:
                return 'even'
            else:
                raise ValueError('Unexpected register value %d' % raw)
        else:
            return 'none'

    @parity.setter 
    def parity(self, value):
        if self.platform == 'Husky':
            if value == 'none':
                raw = 0
            elif value == 'odd':
                raw = 3
            elif value == 'even':
                raw = 2
            else:
                raise ValueError('illegal setting: use "none"/"odd"/"even"')
            setting = self.fpga_read(self.REG_UART_PARITY_SETTING, 1)[0]
            # don't touch bit 2:
            setting &= 0x04
            setting |= raw
            self.fpga_write(self.REG_UART_PARITY_SETTING, [setting])
        else:
            tracewhisperer_logger.warning("Not supported on this platform.")

    @property 
    def accept_parity_errors(self):
        """Only available on the Husky platform. Has no effect on trace
        operation.  For UART-based triggering, control whether UART words with
        parity errors are accepted into the pattern match. If set, the pattern
        match logic treats UART words with parity errors as though they were
        never received.  Consider this example where uppercase letters
        represent the UART words received, ^ indicates a parity error, and the
        programmed pattern is [A,B,C,D}:
        1. [A,B,C^,D]: if accept_parity_errors is set, the pattern match will
        trigger; otherwise it will not.
        2. [A,B,C^,C,D]: if accept_parity_errors is set, the pattern match will
        *not* trigger; otherwise it *will* trigger.

        Args:
            value (bool)
        """
        if self.platform == 'Husky':
            raw = self.fpga_read(self.REG_UART_PARITY_SETTING, 1)[0] & 0x04
            if raw:
                return True
            else:
                return False
        else:
            return None

    @accept_parity_errors.setter 
    def accept_parity_errors(self, value):
        if self.platform == 'Husky':
            if value:
                raw = 4
            else:
                raw = 0
            setting = self.fpga_read(self.REG_UART_PARITY_SETTING, 1)[0]
            setting = setting & (raw + 3) # don't clear bits 1:0
            # don't touch bits 1:0:
            setting &= 0x03
            setting |= raw
            self.fpga_write(self.REG_UART_PARITY_SETTING, [setting])
        else:
            tracewhisperer_logger.warning("Not supported on this platform.")

    @property 
    def stop_bits(self):
        """Number of stop bits for the UART receiver. Only available on the
        Husky platform.  For trace operation this should always be set to 1.
        The option to set this to 2 is provided because on the Husky platform,
        the same UART receiver that is used for trace is also used as a
        receiver for triggering on generic UART traffic.

        Args:
            value (int): 1 or 2
        """
        if self.platform == 'Husky':
            return self.fpga_read(self.REG_UART_STOP_BITS, 1)[0]
        else:
            return 1

    @stop_bits.setter 
    def stop_bits(self, value):
        if self.platform == 'Husky':
            if value not in [1,2]:
                raise ValueError("Illegal setting: only 1 or 2 allowed")
            self.fpga_write(self.REG_UART_STOP_BITS, [value])
        else:
            tracewhisperer_logger.warning("Not supported on this platform.")

    @property 
    def data_bits(self):
        """Number of data bits per word for the UART receiver. Only available
        on the Husky platform.  For trace operation this should always be set
        to 8.  The option to set this to [5,9] is provided because on the Husky
        platform, the same UART receiver that is used for trace is also used as
        a receiver for triggering on generic UART traffic.

        Args:
            value (int): minimum 5, maximum 9
        """
        if self.platform == 'Husky':
            return self.fpga_read(self.REG_UART_DATA_BITS, 1)[0]
        else:
            return 8

    @data_bits.setter 
    def data_bits(self, value):
        if self.platform == 'Husky':
            if value not in range(5,10):
                raise ValueError("Illegal setting: must be in range(5,10)")
            self.fpga_write(self.REG_UART_DATA_BITS, [value])
        else:
            tracewhisperer_logger.warning("Not supported on this platform.")



class clock(util.DisableNewAttr):
    ''' Clock-related settings.
    '''
    _name = 'TraceWhisperer clock settings'

    def __init__(self, main):
        super().__init__()
        self.main = main
        if self.main.platform == 'Husky':
            self.swo_drp = XilinxDRP(main, "LA_DRP_DATA", "LA_DRP_ADDR", "LA_DRP_RESET")
        else:
            self.swo_drp = XilinxDRP(main, main.REG_TRIGGER_DRP_DATA, main.REG_TRIGGER_DRP_ADDR, main.REG_TRIGGER_DRP_RESET)
        self.traceclk_drp = XilinxDRP(main, main.REG_TRACECLK_DRP_DATA, main.REG_TRACECLK_DRP_ADDR, main.REG_TRACECLK_DRP_RESET)
        self.swo_mmcm = XilinxMMCMDRP(self.swo_drp)
        self.traceclk_mmcm = XilinxMMCMDRP(self.traceclk_drp)
        self._warning_frequency = 250e6
        self._trace_clock_shift_steps = 0
        self._trace_clock_vco = 0
        self._trace_clock_muldiv = 0
        self._timeout = 1
        self.disable_newattr()

    def _dict_repr(self):
        rtn = OrderedDict()
        rtn['fe_clock_alive']   = self.fe_clock_alive
        if self.main.platform == 'CW610' or self.main.platform == 'Husky':
            rtn['fe_clock_src']     = self.fe_clock_src
        if self.main.platform == 'Husky':
            rtn['clkgen_enabled'] = self.clkgen_enabled
        rtn['fe_freq']          = self.fe_freq
        rtn['swo_clock_locked']   = self.swo_clock_locked
        rtn['swo_clock_freq']     = self.swo_clock_freq
        if self.fe_clock_src == 'trace_clock':
            rtn['trace_clock_shift_enable'] = self.trace_clock_shift_enable
            rtn['trace_clock_shift_locked'] = self.trace_clock_shift_locked
            rtn['trace_clock_shift_steps'] = self.trace_clock_shift_steps
            rtn['trace_clock_shift_range'] = self.trace_clock_shift_range
        return rtn

    def __repr__(self):
        return util.dict_to_str(self._dict_repr())

    def __str__(self):
        return self.__repr__()

    @property
    def clkgen_enabled(self):
        """Controls whether the Xilinx MMCM used to generate the sampling clock
        is powered on or not.  7-series MMCMs are power hungry. In the Husky
        FPGA, MMCMs are estimated to consume close to half of the FPGA's power.
        If you run into temperature issues and don't require the logic analyzer
        or debug trace functionality, power down this MMCM.
        """
        return self.main._scope.LA.clkgen_enabled

    @clkgen_enabled.setter
    def clkgen_enabled(self, enable):
        self.main._scope.LA.clkgen_enabled = enable


    @property
    def fe_freq(self):
        """Measured clock frequency of the selected front-end clock, as set by trace.clock.fe_clock_src.
        """
        raw = int.from_bytes(self.main.fpga_read(self.main.REG_FE_FREQ, 4), byteorder='little')
        freq = raw * 96e6 / float(pow(2,23))
        return freq

    @property
    def swo_clock_freq(self):
        """Measured clock frequency of the SWO sampling clock, which is used for both (a) trigger generation
           and (b) SWO sampling.
        """
        raw = int.from_bytes(self.main.fpga_read(self.main.REG_TRIGGER_FREQ, 4), byteorder='little')
        freq = raw * 96e6 / float(pow(2,23))
        return freq

    @swo_clock_freq.setter
    def swo_clock_freq(self, freq, vcomin=600e6, vcomax=1200e6, threshold=0.01):
        """Calculate Multiply & Divide settings based on input frequency"""
        if self.main.enabled and not self.fe_clock_alive:
            tracewhisperer_logger.error("FE clock not present, cannot calculate proper M/D settings")
        if self.main.platform == 'Husky':
            input_freq = self.main._scope.LA.source_clock_frequency
        else:
            input_freq = self.fe_freq
        lowerror = 1e99
        best = (0,0,0)
        for maindiv in range(1,6):
            mmin = int(np.ceil(vcomin/input_freq*maindiv))
            mmax = int(np.ceil(vcomax/input_freq*maindiv))
            for mul in range(mmin,mmax+1):
                if mul/maindiv < vcomin/input_freq or mul/maindiv > vcomax/input_freq or mul >= 2**7:
                    continue
                for secdiv in range(1,127):
                    calcfreq = input_freq*mul/maindiv/secdiv
                    err = abs(freq - calcfreq)
                    if err < lowerror:
                        lowerror = err
                        best = (mul, maindiv, secdiv)
        if best == (0,0,0):
            tracewhisperer_logger.error("Couldn't find a legal div/mul combination")
        else:
            self.swo_mmcm.set_mul(best[0])
            self.swo_mmcm.set_main_div(best[1])
            self.swo_mmcm.set_sec_div(best[2])
            actual = input_freq*best[0]/best[1]/best[2]
            if abs(actual-freq)/freq*100 > threshold:
                scope_logger.warning("Coudln't achieve exact desired frequency (%f); setting to %f instead." % (freq, input_freq*best[0]/best[1]/best[2]))
            time.sleep(0.1)
            if freq > self._warning_frequency:
                scope_logger.warning("""
                    Clock frequency exceeds specification (250 MHz). 
                    This may or may not work, depending on temperature, voltage, and luck.
                    It may not work reliably.
                    You can adjust trace.clock._warning_frequency if you don't want
                    to see this message anymore.
                    """)
            # changing the frequency may result in SWO internal CDC error; clear it:
            self.main.errors = 0

    @property
    def fe_clock_alive(self):
        """Indicates whether the selected front-end clock appears to be alive.
        """
        read1 = int.from_bytes(self.main.fpga_read(self.main.REG_FE_CLOCK_COUNT, 4), byteorder='little')
        read2 = int.from_bytes(self.main.fpga_read(self.main.REG_FE_CLOCK_COUNT, 4), byteorder='little')
        if read1 == read2:
            return False
        else:
            return True

    @property 
    def swo_clock_locked(self):
        """Indicates whether the FPGA MMCM which generates the SWO sampling / trigger clock is locked.
        """
        if self.main.fpga_read(self.main.REG_MMCM_LOCKED, 1)[0] & 2:
            return True
        else:
            return False

    @property
    def fe_clock_src(self):
        """Choose which clock is used as the front-end clock.
        On the CW305 platform, "target_clock" is the only option.
        On Husky, "target_clock" refers to either the target-generated clock or
        Husky-generated clock, as per :code:`scope.clock.clkgen_src`.

        Args:
            src (str): "target_clock", "trace_clock" or "usb_clock"
        """
        if self.main.platform == 'CW305':
            return 'target_clock'
        else:
            raw = self.main.fpga_read(self.main.REG_FE_CLOCK_SEL, 1)[0]
            if raw == 0:
                return 'target_clock'
            elif raw == 1:
                return 'trace_clock'
            elif raw == 2:
                return 'usb_clock'
            else:
                raise ValueError("Unexpected value: %d" % raw)

    @fe_clock_src.setter
    def fe_clock_src(self, src):
        if src != 'target_clock' and self.main.platform == 'CW305':
            raise ValueError("Not supported on CW305. Use 'target_clock'")
        if src == 'target_clock':
            val = 0
        elif src == 'trace_clock':
            val = 1
        elif src == 'usb_clock':
            val = 2
        else:
            raise ValueError('Invalid source (target_clock/trace_clock/usb_clock)')
        self.main.fpga_write(self.main.REG_FE_CLOCK_SEL, [val])

    @property
    def trace_clock_shift_enable(self):
        """Turn on the MMCM for shifting the trace clock. When disabled, the
        raw input trace clock is used; when enabled, the MMCM-shifted trace
        clock is used.

        Args:
            enable (bool)
        """
        raw = self.main.fpga_read(self.main.REG_TRACECLK_SHIFT_EN, 1)[0]
        if raw:
            return True
        else:
            return False

    @trace_clock_shift_enable.setter
    def trace_clock_shift_enable(self, enable):
        if enable:
            val = 1
        else:
            val = 0
        self.main.fpga_write(self.main.REG_TRACECLK_SHIFT_EN, [val])

    @property
    def trace_clock_shift_steps(self):
        """The trace clock phase shift. There are
        `trace.clock.trace_clock_shift_range` steps in a full period of the
        trace clock (the trace clock frequency *must* be specified by calling
        `trace.clock.trace_clock_set_freq()` in order for this to be accurate).

        Negative values are allowed, but -x is equivalent to
        `trace.clock.trace_clock_shift_range - x`. The setting rolls over (+x
        is equivalent to `trace.clock.trace_clock_shift_range + x`). Run the
        trace_clock_alignment.ipynb notebook in the DesignStartTrace repository
        to visualize phase shift settings.

        Args:
            steps (signed 16-bit integer): number of phase shift steps.
        """
        return self._trace_clock_shift_steps

    @trace_clock_shift_steps.setter
    def trace_clock_shift_steps(self, steps):
        if not (self.trace_clock_shift_enable and self.trace_clock_shift_locked):
            raise ValueError("Can't change settings if not enabled and locked.")
        assert type(steps) == int
        LSB = steps & 0x00FF
        MSB = (steps & 0xFF00) >> 8
        self.main.fpga_write(self.main.REG_TRACECLK_PHASE, [LSB, MSB])
        # Large adjustments can take a while so it's important to check if done.
        starttime = datetime.datetime.now()
        done = False
        while not done:
            diff = datetime.datetime.now() - starttime
            if (diff.total_seconds() > self._timeout):
                scope_logger.warning('Timeout in phase adjustment. Increase self._timeout. This should not be necessary unless you make *huge* step jumps.')
                break
            raw = self.main.fpga_read(self.main.REG_TRACECLK_PHASE, 1)[0]
            done = raw & 0x01
        self._trace_clock_shift_steps = steps

    @property
    def trace_clock_shift_locked(self):
        """Indicates whether the MMCM (PLL) for the trace clock phase shift is
        locked.  If this is False, make sure that the trace clock is present
        (CK pin of the USERIO header), and that `trace_clock_set_freq()` has
        been called to specify the trace clock frequency.
        """
        raw = self.main.fpga_read(self.main.REG_TRACECLK_PHASE, 1)[0]
        if raw & 0x02:
            return True
        else:
            return False

    @property
    def trace_clock_shift_range(self):
        """Returns number of phase shift steps in one trace pll cycle.  This
        is simply 56 times the MMCM's multiplier, indepedent of the trace
        clock frequency.  (ref: Xilinx UG472 v1.14, "Dynamic Phase Shift
        Interface in the MMCM")
        The trace clock frequency must be provided via `trace_clock_set_freq()`
        in order for this to be accurate.  The number of phase shift steps
        depends on the clock frequency (fewer steps as the frequency increases)
        and on the PLL VCO frequency which can also be specified via
        `trace_clock_set_freq()` (more steps as the VCO increases).
        """
        return self._trace_clock_muldiv * 56

    def trace_clock_set_freq(self, freq, vco=600e6):
        """Use this to specify the trace clock frequency. This is important for
        (a) operating the phase shift MMCM in its allowed operating range, and
        (b) accurately determining the number of phase shift steps per clock
        period. The MMCM's VCO frequency can optionally be specified. Note that
        the trace clock frequency is usually half of the target's clock
        frequency.

        Args:
            freq (int): trace clock frequency. Minimum: 5 MHz.
            vco (int): VCO frequency. Allowed range [600e6, 1200e6]. Higher
                    values allow finer phase adjustments but consume more
                    power.  Default: 600 MHz.
        """
        if vco > 1200e6 or vco < 600e6:
            raise ValueError("Requested VCO out of range")

        # The following changes resets the phase shift setting, but just
        # resetting the internal (Python) phase setting doesn't work as one
        # would expect; resetting the actual FPGA MMCM phase is needed to get
        # consistent results.
        if self.trace_clock_shift_enable and self.trace_clock_shift_locked:
            self.trace_clock_shift_steps = 0

        muldiv = int(np.ceil(vco/freq))
        if freq * muldiv > 1200e6:
            muldiv -= 1
        self._trace_clock_vco = freq * muldiv

        tracewhisperer_logger.info("Setting vco {}, muldiv: {}".format(vco, muldiv))
        self.traceclk_mmcm.set_mul(muldiv)
        self.traceclk_mmcm.set_sec_div(muldiv)
        self.traceclk_mmcm.set_main_div(1)
        self._trace_clock_muldiv = muldiv


class capture(util.DisableNewAttr):
    ''' Capture-related settings.
    '''
    _name = 'TraceWhisperer capture settings'

    def __init__(self, main):
        super().__init__()
        self.main = main
        self.disable_newattr()
        if self.main.platform == 'Husky':
            self.use_husky_arm = False

    def _dict_repr(self):
        rtn = OrderedDict()
        rtn['trigger_source']           = self.trigger_source
        if self.main.platform == 'Husky':
            rtn['use_husky_arm']        = self.use_husky_arm
        rtn['raw']                      = self.raw
        rtn['rules_enabled']            = self.rules_enabled
        rtn['rules']                    = self.rules
        rtn['mode']                     = self.mode
        rtn['count']                    = self.count
        rtn['max_triggers']             = self.max_triggers
        rtn['triggers_generated']       = self.triggers_generated
        rtn['record_syncs']             = self.record_syncs
        rtn['matched_pattern_data']     = self.matched_pattern_data
        rtn['matched_pattern_counts']   = self.matched_pattern_counts
        return rtn

    def __repr__(self):
        return util.dict_to_str(self._dict_repr())

    def __str__(self):
        return self.__repr__()

    @property
    def raw(self):
        """ Set whether TraceWhisperer captures raw trace data or matching rule indices.

        Args:
            val (int or bool): if set, capture raw trace data. Otherwise, capture the index
                               of the matching rule number.
        """
        if self.main.fpga_read(self.main.REG_CAPTURE_RAW, 1)[0]:
            return True
        else:
            return False

    @raw.setter
    def raw(self, val):
        if val:
            self.main.fpga_write(self.main.REG_CAPTURE_RAW, [1])
        else:
            self.main.fpga_write(self.main.REG_CAPTURE_RAW, [0])

    @property
    def record_syncs(self):
        """ Set whether TraceWhisperer captures all trace sync frames or not.
            Note: since the TraceWhisperer FPGA does not parse the ETM frames,
            this is done as "best effort", and some sync frames will still be
            captured. The intent of this feature is to minimize the storage
            consumed by the sync frames, not eliminate it.

        Args:
            val (int or bool)
        """
        if self.main.fpga_read(self.main.REG_RECORD_SYNCS, 1)[0]:
            return True
        else:
            return False

    @record_syncs.setter
    def record_syncs(self, val):
        if val:
            self.main.fpga_write(self.main.REG_RECORD_SYNCS, [1])
        else:
            self.main.fpga_write(self.main.REG_RECORD_SYNCS, [0])

    @property
    def matched_pattern_data(self):
        """ Return the actual trace data seen for the last matched pattern.
        """
        return '%016x' % (int.from_bytes(self.main.fpga_read(self.main.REG_MATCHED_DATA, 8)[::-1], byteorder='little'))

    @property
    def matched_pattern_counts(self):
        """ Return a list with the number of times each pattern match rule was matched.
        """
        return list(self.main.fpga_read(self.main.REG_TRACE_COUNT, 8)[::-1])

    @property
    def max_triggers(self):
        """ Maximum number of triggers to generate. Intended for trace-based 
            triggering (i.e. :code:`scope.trigger.module = 'trace'`), where the trace
            event(s) which can generate a trigger can occur multiple times
            (e.g. the start of an AES round). Setting this to 'x' does not mean
            that 'x' triggers will be generated, it means that *up to* 'x'
            triggers can be generated. This parameter is needed so that the
            trace module knows when it is 'done'; it's also useful to
            coordinate with e.g.  segmented capture parameters
            (:code:`scope.adc.segments`).

        Args:
            number (int): number from 1 to 2**16-1.
        """
        return int.from_bytes(self.main.fpga_read(self.main.REG_NUM_TRIGGERS, 2), byteorder='little')

    @max_triggers.setter
    def max_triggers(self, number):
        if not 0 < number < 2**16:
            raise ValueError("Out of allowed range")
        else:
            self.main.fpga_write(self.main.REG_NUM_TRIGGERS, int.to_bytes(number, length=4, byteorder='little'))

    @property
    def triggers_generated(self):
        """ Number of triggers that were generated on the last capture cycle.
        """
        return self.main.fpga_read(self.main.REG_TRIGGERS_GENERATED, 1)[0]


    @property
    def trigger_source(self):
        """ Set whether firmware trigger or trace trigger is used to enable recording of trace data.
        To use the firmware trigger on CW610, it must be connected to the side connector 'PC' pin.

        Args:
            source (str or int): "firmware trigger": Use target-generated trigger to initiate trace capture.
                                 int: use matching trace data to initiate trace capture, with given rule number.
        """
        if self.main.fpga_read(self.main.REG_SOFT_TRIG_ENABLE, 1)[0]:
            return('firmware trigger')
        else:
            raw = self.main.fpga_read(self.main.REG_PATTERN_TRIG_ENABLE, 1)[0]
            if raw > 0:
                return('trace trigger, rule #%d' % int(math.log2(raw)))
            else:
                return('trace trigger (no rule set)')

    @trigger_source.setter
    def trigger_source(self, source):
        if source == 'firmware trigger':
            self.main.fpga_write(self.main.REG_SOFT_TRIG_ENABLE, [1])
            self.main.fpga_write(self.main.REG_SOFT_TRIG_PASSTHRU, [1])
            self.main.fpga_write(self.main.REG_PATTERN_TRIG_ENABLE, [0])
        else:
            if type(source) == int:
                self.main.fpga_write(self.main.REG_SOFT_TRIG_ENABLE, [0])
                self.main.fpga_write(self.main.REG_SOFT_TRIG_PASSTHRU, [0])
                self.main.fpga_write(self.main.REG_PATTERN_TRIG_ENABLE, [2**source])
                self.main.fpga_write(self.main.REG_TRIGGER_ENABLE, [1])
                # these can be customized but let's start you off with simple default values:
                self.main.fpga_write(self.main.REG_NUM_TRIGGERS, [1])
                self.main.fpga_write(self.main.REG_TRIGGER_WIDTH, [16])
            else:
                raise TypeError

    @property
    def mode(self):
        """Determine the duration of the trace capture.

        Args:
            mode (string): 'while_trig': capture while the trigger input is high
                           'count_cycles': capture for self.count clock cycles
                           'count_writes': capture self.count events
                           'off': capture disabled (e.g. for trigger generation only)
        """
        raw = self.main.fpga_read(self.main.REG_CAPTURE_OFF, 1)[0]
        if raw:
            return "off"
        else:
            raw = self.main.fpga_read(self.main.REG_CAPTURE_WHILE_TRIG, 1)[0]
            if raw:
                return "while_trig"
            else:
                raw = self.main.fpga_read(self.main.REG_COUNT_WRITES, 1)[0]
                if raw:
                    return "count_writes"
                else:
                    return "count_cycles"

    @mode.setter
    def mode(self, mode):
        if mode == 'off':
            self.main.fpga_write(self.main.REG_CAPTURE_OFF, [1])
        elif mode == 'while_trig':
            self.main.fpga_write(self.main.REG_CAPTURE_OFF, [0])
            self.main.fpga_write(self.main.REG_CAPTURE_WHILE_TRIG, [1])
        elif mode == 'count_cycles':
            self.main.fpga_write(self.main.REG_CAPTURE_OFF, [0])
            self.main.fpga_write(self.main.REG_CAPTURE_WHILE_TRIG, [0])
            self.main.fpga_write(self.main.REG_COUNT_WRITES, [0])
        elif mode == 'count_writes':
            self.main.fpga_write(self.main.REG_CAPTURE_OFF, [0])
            self.main.fpga_write(self.main.REG_CAPTURE_WHILE_TRIG, [0])
            self.main.fpga_write(self.main.REG_COUNT_WRITES, [1])
        else:
            tracewhisperer_logger.error('Invalid mode %s', mode)

    @property
    def count(self):
        """Control how long we capture trace events when self.mode != 'while_trig'.

        Args:
            counts (int): number of clock cycles (self.mode == 'count_cycles') or writes (self.mode == 'count_writes') to capture for; 0 means capture until storage is full
        """
        return int.from_bytes(self.main.fpga_read(self.main.REG_CAPTURE_LEN, 4), byteorder='little')

    @count.setter
    def count(self, counts):
        self.main.fpga_write(self.main.REG_CAPTURE_LEN, int.to_bytes(counts, length=4, byteorder='little'))

    @property
    def rules_enabled(self):
        """Set which matching rules are enabled.

        Args:
            rules (list of ints): turn on the specified rules; others are turned off.  example: [0, 5, 7]: turns on rules 0, 5 and 7.
        """
        raw = self.main.fpga_read(self.main.REG_PATTERN_ENABLE, 1)[0]
        rules = []
        for b in range(8):
            if raw & 2**b:
                rules.append(b)
        return rules

    @rules_enabled.setter
    def rules_enabled(self, rules):
        raw = 0
        for rule in rules:
            raw += 2**rule
        self.main.fpga_write(self.main.REG_PATTERN_ENABLE, [raw])

    @property
    def rules(self):
        """Use set_pattern_match() to set these.
        """
        rulez = []
        for e in self.rules_enabled:
            rulez.append({'rule':e, 'patt':self.main.fpga_read(self.main.REG_TRACE_PATTERN0+e, 8), 'mask':self.main.fpga_read(self.main.REG_TRACE_MASK0+e, 8)})
        return rulez

    @property
    def use_husky_arm(self):
        """ When used as part of Husky, it's possible for forego this and have
        the trace/UART module be armed by the regular Husky arm, by setting
        this to True.
        """
        val = self.main.fpga_read(self.main.REG_EXTERNAL_ARM, 1)[0]
        if self.main.platform != 'Husky':
            tracewhisperer_logger.error("Applicable to Husky only")
            return False
        elif val:
            return True
        else:
            return False

    @use_husky_arm.setter
    def use_husky_arm(self, val):
        if self.main.platform == 'Husky':
            self.main.fpga_write(self.main.REG_EXTERNAL_ARM, [val])


class ARM_debug_registers(util.DisableNewAttr):
    ''' Convenience methods to set or get ARM debug registers (requires firmware support).
    '''
    _name = 'ARM target debug registers for TraceWhisperer'

    # must be in sync with target firmware:
    regs = {
            'DWT_CTRL':     0x0,
            'DWT_COMP0':    0x1,
            'DWT_COMP1':    0x2,
            'ETM_CR':       0x3,
            'ETM_TESSEICR': 0x4,
            'ETM_TEEVR':    0x5,
            'ETM_TECR1':    0x6,
            'ETM_TRACEIDR': 0x7,
            'TPI_ACPR':     0x8,
            'TPI_SPPR':     0x9,
            'TPI_FFCR':     0xa,
            'TPI_CSPSR':    0xb,
            'ITM_TCR':      0xc
           }

    def __init__(self, main):
        super().__init__()
        self.main = main
        self.cached_values = [None] * len(self.regs)
        self.disable_newattr()

    def _dict_repr(self):
        if self.main._ss is None:
            tracewhisperer_logger.error("Target must be connected for this to work (e.g. scope.trace.target = target)")
            return OrderedDict()

        else:
            rtn = OrderedDict()
            rtn['DWT_CTRL']     = self.DWT_CTRL
            rtn['DWT_COMP0']    = self.DWT_COMP0
            rtn['DWT_COMP1']    = self.DWT_COMP1
            rtn['ETM_CR']       = self.ETM_CR
            rtn['ETM_TESSEICR'] = self.ETM_TESSEICR
            rtn['ETM_TEEVR']    = self.ETM_TEEVR
            rtn['ETM_TECR1']    = self.ETM_TECR1
            rtn['ETM_TRACEIDR'] = self.ETM_TRACEIDR
            rtn['TPI_ACPR']     = self.TPI_ACPR
            rtn['TPI_SPPR']     = self.TPI_SPPR
            rtn['TPI_FFCR']     = self.TPI_FFCR
            rtn['TPI_CSPSR']    = self.TPI_CSPSR
            rtn['ITM_TCR']      = self.ITM_TCR

            return rtn

    def __repr__(self):
        return util.dict_to_str(self._dict_repr())

    def __str__(self):
        return self.__repr__()

    @property 
    def DWT_CTRL(self):
        return self._get('DWT_CTRL')

    @DWT_CTRL.setter
    def DWT_CTRL(self, val): 
        self._set('DWT_CTRL', val)

    @property
    def DWT_COMP0(self):
        return self._get('DWT_COMP0')

    @DWT_COMP0.setter
    def DWT_COMP0(self, val): 
        self._set('DWT_COMP0', val)

    @property
    def DWT_COMP1(self): 
        return self._get('DWT_COMP1')

    @DWT_COMP1.setter
    def DWT_COMP1(self, val): 
        self._set('DWT_COMP1', val)

    @property
    def ETM_CR(self): 
        return self._get('ETM_CR')

    @ETM_CR.setter
    def ETM_CR(self, val): 
        self._set('ETM_CR', val)

    @property
    def ETM_TESSEICR(self): 
        return self._get('ETM_TESSEICR')

    @ETM_TESSEICR.setter
    def ETM_TESSEICR(self, val): 
        self._set('ETM_TESSEICR', val)

    @property
    def ETM_TEEVR(self): 
        return self._get('ETM_TEEVR')

    @ETM_TEEVR.setter
    def ETM_TEEVR(self, val): 
        self._set('ETM_TEEVR', val)

    @property
    def ETM_TECR1(self): 
        return self._get('ETM_TECR1')

    @ETM_TECR1.setter
    def ETM_TECR1(self, val): 
        self._set('ETM_TECR1', val)

    @property
    def ETM_TRACEIDR(self): 
        return self._get('ETM_TRACEIDR')

    @ETM_TRACEIDR.setter
    def ETM_TRACEIDR(self, val): 
        self._set('ETM_TRACEIDR', val)

    @property
    def TPI_ACPR(self): 
        return self._get('TPI_ACPR')

    @TPI_ACPR.setter
    def TPI_ACPR(self, val): 
        self._set('TPI_ACPR', val)

    @property
    def TPI_SPPR(self): 
        return self._get('TPI_SPPR')

    @TPI_SPPR.setter
    def TPI_SPPR(self, val): 
        self._set('TPI_SPPR', val)

    @property
    def TPI_FFCR(self): 
        return self._get('TPI_FFCR')

    @TPI_FFCR.setter
    def TPI_FFCR(self, val): 
        self._set('TPI_FFCR', val)

    @property
    def TPI_CSPSR(self): 
        return self._get('TPI_CSPSR')

    @TPI_CSPSR.setter
    def TPI_CSPSR(self, val): 
        self._set('TPI_CSPSR', val)

    @property
    def ITM_TCR(self): 
        return self._get('ITM_TCR')

    @ITM_TCR.setter
    def ITM_TCR(self, val): 
        self._set('ITM_TCR', val)

    def _set(self, reg, data, printresult=False):
        """Set a Cortex debug register

        Args:
            reg (string): Register to write. See self.regs for available registers.
            data (int or string): 32-bit integer or 8-character hex string, value to write to
                                  specified register (e.g. '1000F004')
        """
        if self.main._ss is None:
            tracewhisperer_logger.error("Target must be connected for this to work (e.g. scope.trace.target = target)")

        elif reg in self.regs:
            if type(data) == str:
                data_str = data
                data_int = int(data, 16)
            elif type(data) == int:
                data_str = '%08x' % data
                data_int = data
            else:
                raise TypeError

            self.cached_values[self.regs[reg]] = data_int
            data = '%02x' % self.regs[reg] + data_str
            self.main._ss.simpleserial_write('s', util.hexStrToByteArray(data))
            time.sleep(0.1)
            if printresult:
                print(self.main._ss.read().split('\n')[0])
        else:
            tracewhisperer_logger.error('Register %s does not exist.', reg)


    def _get(self, reg):
        """Reads a Cortex debug register

        Args:
            reg (string): Register to read. See self.regs for available registers.
        """
        if self.main._ss is None:
            tracewhisperer_logger.error("Target must be connected for this to work (e.g. scope.trace.target = target)")
            return None
        elif reg in self.regs:
            if self.cached_values[self.regs[reg]]:
                val = self.cached_values[self.regs[reg]]
            else:
                data = '%02x' % self.regs[reg] + '00000000'
                self.main._ss.simpleserial_write('g', util.hexStrToByteArray(data))
                time.sleep(0.1)
                try:
                    val = int(self.main._ss.read().split('\n')[0][1:], 16)
                    self.cached_values[self.regs[reg]] = val
                except Exception as e:
                    scope_logger.error("Can't read trace register. Perhaps target firmware doesn't support it? See simpleserial-trace for an example of what's required. " + str(e))
                    val = 0
            return '%08x' % val
        else:
            tracewhisperer_logger.error('Register %s does not exist.', reg)



class UARTTrigger(TraceWhisperer):
    ''' Husky UART trigger module settings.
    Basic usage for triggering on 'r'::

        #assuming setup scope:
        scope.trigger.triggers = 'tio1'
        scope.trigger.module = 'UART'
        scope.UARTTrigger.enabled = True
        scope.UARTTrigger.baud = 38400
        scope.UARTTrigger.set_pattern_match(0, 'r')
        scope.UARTTrigger.trigger_source = 0

    TraceWhisperer runs on the same hardware as this, so configuration changes
    in one affects the other and vice-versa.
    '''
    _name = 'UART Trigger Module'

    def __init__(self, scope, trace_reg_select, main_reg_select):
        self._baud = 0
        self._baud_margin = 0.005
        super().__init__(husky=True, target=None, scope=scope, trace_reg_select=trace_reg_select, main_reg_select=main_reg_select)
        self.disable_newattr()
        self.trigger_source = 0

    def _dict_repr(self):
        rtn = OrderedDict()
        rtn['enabled'] = self.enabled
        rtn['baud'] = self.baud
        rtn['data_bits'] = self.data_bits
        rtn['stop_bits'] = self.stop_bits
        rtn['parity'] = self.parity
        rtn['accept_parity_errors'] = self.accept_parity_errors
        rtn['sampling_clock'] = self.sampling_clock
        rtn['trigger_source'] = self.trigger_source
        rtn['rules_enabled'] = self.rules_enabled
        rtn['rules'] = self.rules
        #rtn['matched_pattern_data'] = self.matched_pattern_data
        rtn['matched_pattern_counts'] = self.matched_pattern_counts
        return rtn

    def __repr__(self):
        return util.dict_to_str(self._dict_repr())

    def __str__(self):
        return self.__repr__()

    @property 
    def enabled(self):
        """Controls whether trace data collecting is enabled or not. Mostly affects configuration
        of the front 20-pin header.

        Args:
            enable (bool)
        """
        return super().enabled

    @enabled.setter 
    def enabled(self, enable):
        if enable:
            # set useful defaults:
            self.trace_mode = 'swo'
            self.capture.mode = 'off'
            self.clock.fe_clock_src = 'target_clock'
            self.capture.record_syncs = True
            self.capture.use_husky_arm = True
        # accessing base class setter is awkward! all we want to do here is super().enabled = enable, but this is the way to do that:
        super(UARTTrigger, self.__class__).enabled.fset(self, enable)
        # enabling the UART trigger module may result in SWO internal CDC error; clear it:
        time.sleep(0.1)
        self.errors = 0

    @property 
    def rules_enabled(self):
        """Shortcut to self.capture.rules_enabled
        """
        return self.capture.rules_enabled

    @rules_enabled.setter 
    def rules_enabled(self, enable):
        self.capture.rules_enabled = enable

    @property 
    def rules(self):
        """Shortcut to self.capture.rules
        """
        return self.capture.rules

    @property
    def baud(self):
        """ Set the desired baud rate.
        """
        return self._baud

    @baud.setter
    def baud(self, req_baud):
        found = False
        freq_mul = 2 # TODO: should be able to use 1, but MMCM doesn't work correctly at that setting
        sample_freq = self.clock.fe_freq * freq_mul
        while True:
            div = int(sample_freq / req_baud)
            actual_baud = sample_freq / div
            if abs(req_baud-actual_baud)/req_baud < self._baud_margin:
                found = True
                break
            freq_mul += 1
            sample_freq = self.clock.fe_freq * freq_mul
            if sample_freq > 200e6:
                break
        if found:
            self.clock.swo_clock_freq = sample_freq
            if not self.clock.swo_clock_locked:
                tracewhisperer_logger.error("SWO clock not locked!")
            self.swo_div = div
            self._baud = actual_baud
            tracewhisperer_logger.info("Setting baudrate to %d (freq_mul=%d)" % (actual_baud, freq_mul))
        else:
            tracewhisperer_logger.error("Couldn't find parameters for requested baud rate. Try increasing scope.uart._baud_margin.")


    @property
    def sampling_clock(self):
        """ Shorthand for accessing the measured frequency of the sampling clock.
            Indirectly set via 'baud' setting.
        """
        return self.clock.swo_clock_freq

    def uart_data(self, rawdata, prepend_matched_pattern=True, return_ascii=True):
        """ Helper function to parse the captured UART data.

        Args:
            rawdata (list): raw capture data, list of lists, e.g. obtained from read_capture_data()
            prepend_matched_pattern (bool): 
            return_ascii (bool): return data as ASCII (otherwise hex)
        """
        datalist = []
        prepend = []
        if prepend_matched_pattern:
            for b in self.fpga_read(self.REG_MATCHED_DATA, 8):
                prepend.append([0, b, self.FE_FIFO_CMD_STAT])
        for raw in prepend+rawdata:
            command = raw[2] & 0x3
            if command == self.FE_FIFO_CMD_STAT:
                data = raw[1]
                if return_ascii:
                    datalist.append(chr(data))
                else:
                    datalist.append(data)
            elif command == self.FE_FIFO_CMD_DATA:
                print("Unexpected data command.")
        return datalist


    def matched_pattern_data(self, as_string=True):
        """ Return the actual trace data seen for the last matched pattern.

        Args:
            as_string (bool): convert each byte to its boolean string; otherwise,
            results are returned as a list of self.data_bits-sized words.
        """
        raw = self.fpga_read(self.REG_MATCHED_DATA, 8)
        if as_string:
            string = ''
            for b in raw:
                string += chr(b)
            return string
        else:
            word_size = self.data_bits
            bigword = 0
            for i,w in enumerate(raw):
                bigword += (w << (8*i))
            pattern = []
            while bigword:
                pattern.append((bigword & ((2**word_size-1) << 64-word_size)) >> (64-word_size))
                bigword <<= word_size
                bigword &= 2**64-1
            return pattern[::-1]

    @property
    def matched_pattern_counts(self):
        """ Return the actual trace data seen for the last matched pattern.
        """
        return self.capture.matched_pattern_counts


    @property
    def trigger_source(self):
        """ Set which pattern match rule is used to generate a trigger.

        Args:
            rule (int)
        """
        if self.fpga_read(self.REG_SOFT_TRIG_ENABLE, 1)[0]:
            return('firmware trigger')
        else:
            raw = self.fpga_read(self.REG_PATTERN_TRIG_ENABLE, 1)[0]
            if raw > 0:
                return('rule #%d' % int(math.log2(raw)))
            else:
                return('no rule set!')

    @trigger_source.setter
    def trigger_source(self, rule):
        if 0 <= rule < 8:
            self.fpga_write(self.REG_SOFT_TRIG_ENABLE, [0])
            self.fpga_write(self.REG_SOFT_TRIG_PASSTHRU, [0])
            self.fpga_write(self.REG_PATTERN_TRIG_ENABLE, [2**rule])
            self.fpga_write(self.REG_TRIGGER_ENABLE, [1])
        else:
            raise ValueError

    def set_pattern_match(self, index, pattern, mask=None, enable_rule=True):
        """Sets pattern match and mask parameters.  The pattern may be
        specified as a string (if the UART word size is 8 bits), or as a list
        of self.data_bits-sized integers (as is done for trace).  
        Refer to set_pattern_match() documentation in parent TraceWhisperer
        class for more details.

        Args:
            index: match index [0-7]
            pattern: string or list of self.data_bits-sized integers.
                Maximum size given by self.pattern_size.
            mask (list, optional): list of self.data_bits-sized integers, 
                must have same size as 'pattern' if set.

        """
        if type(pattern) is str:
            lpattern = []
            for c in pattern:
                lpattern.append(ord(c))
            pattern = lpattern
        super().set_pattern_match(index, pattern, mask, enable_rule)



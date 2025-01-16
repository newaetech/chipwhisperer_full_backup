# -*- coding: cp1252 -*-

# This file is part of the OpenADC Project. See www.newae.com for more details,
# or the codebase at http://www.assembla.com/spaces/openadc .
#
# Copyright (c) 2012-2013, Colin O'Flynn <coflynn@newae.com>. All rights reserved.
# This project is released under the Modified FreeBSD License. See LICENSE
# file which should have came with this code.
import logging
import sys
import time
import datetime
from ...common.utils import util
import array
import numpy as np
from collections import OrderedDict
import copy
import io
import re

from chipwhisperer.logging import *

from typing import Optional, Type, Union, List, Any

CODE_READ       = 0x80
CODE_WRITE      = 0xC0

SETTINGS_RESET     = 0x01
SETTINGS_GAIN_HIGH = 0x02
SETTINGS_GAIN_LOW  = 0x00
SETTINGS_TRIG_HIGH = 0x04
SETTINGS_TRIG_LOW  = 0x00
SETTINGS_ARM       = 0x08
SETTINGS_WAIT_YES  = 0x20
SETTINGS_WAIT_NO   = 0x00
SETTINGS_TRIG_NOW  = 0x40

STATUS_ARM_MASK    = 0x01
STATUS_FIFO_MASK   = 0x02
STATUS_EXT_MASK    = 0x04
STATUS_DCM_MASK    = 0x08
STATUS_DDRCAL_MASK = 0x10
STATUS_DDRERR_MASK = 0x20
STATUS_DDRMODE_MASK= 0x40
STATUS_OVERFLOW_MASK = 0x80

MSG_HDR_SIZE = 3

# sign extend b low bits in x
# from "Bit Twiddling Hacks"
def SIGNEXT(x, b):
    m = 1 << (b - 1)
    x = x & ((1 << b) - 1)
    return (x ^ m) - m

class OpenADCInterface(util.DisableNewAttr):

    def __init__(self, serial_instance, registers):
        super().__init__()
        self.serial = serial_instance
        self._registers = registers
        self.registers = {}
        self.hwInfo = None
        self.offset = 0.5
        self.ddrMode = False
        self.sysFreq = 0
        self._bits_per_sample = 10
        self._sbuf = []
        self._support_decimate = True
        self._nosampletimeout = 100
        self._timeout = 2
        self.presamples_desired = 0
        self.presamples_actual = 0
        self.presampleTempMargin = 24
        self._stream_mode = False
        self._stream_segment_size = 65536
        self._support_get_duration = True
        self._is_husky = False
        self._fast_fifo_read_enable = True
        self._fast_fifo_read_active = False
        self.hwMaxSamples = 0
        self.hwMaxSegmentSamples = 0
        self._stream_len = 0
        self._total_samples = 0
        self._int_data = None
        self._stream_rx_bytes = 0
        self._clear_caches()

        self._slurp_registers()

        self.settings()

        # Send clearing function if using streaming mode
        if hasattr(self.serial, "stream") and self.serial.stream == False:
            pass
        else:
            nullmessage = bytearray([0] * 20)
            self.serial.write(str(nullmessage))

        self.disable_newattr()

    def _clear_caches(self):
        self.cached_settings = None

    def _slurp_registers(self):
        """ Parse Verilog register defines file so we can access register address
        definitions by name.

        """
        self.verilog_define_matches = 0

        if type(self._registers) == io.BytesIO:
            registers = io.TextIOWrapper(self._registers)
        else:
            if not os.path.isfile(self._registers):
                scope_logger.error('Cannot find %s' % self._registers)
            registers = open(self._registers, 'r', encoding='utf-8')
        define_regex_base  =   re.compile(r'`define')
        define_regex_reg   =   re.compile(r'`define\s+?REG_')
        define_regex_radix =   re.compile(r'`define\s+?(\w+).+?\'([bdh])([0-9a-fA-F]+)')
        define_regex_noradix = re.compile(r'`define\s+?(\w+?)\s+?(\d+)')
        block_offset = 0
        for define in registers:
            if define_regex_base.search(define):
                reg = define_regex_reg.search(define)
                match = define_regex_radix.search(define)
                if match:
                    self.verilog_define_matches += 1
                    if match.group(2) == 'b':
                        radix = 2
                    elif match.group(2) == 'h':
                        radix = 16
                    else:
                        radix = 10
                    self.registers[match.group(1)] = int(match.group(3),radix) + block_offset
                    #setattr(self, match.group(1), int(match.group(3),radix) + block_offset)
                    scope_logger.debug('_slurp_registers: setting %s to %d' % (match.group(1), int(match.group(3),radix) + block_offset))
                else:
                    match = define_regex_noradix.search(define)
                    if match:
                        self.verilog_define_matches += 1
                        self.registers[match.group(1)] = int(match.group(2),10) + block_offset
                        #setattr(self, match.group(1), int(match.group(2),10) + block_offset)
                        scope_logger.debug('_slurp_registers: setting %s to %d' % (match.group(1), int(match.group(2),10) + block_offset))
                    else:
                        scope_logger.warning("Couldn't parse line: %s", define)
        registers.close()
        scope_logger.debug("Found %d Verilog register definitions." % self.verilog_define_matches)


    def setStreamMode(self, stream):
        self._stream_mode = stream
        self.updateStreamBuffer()

    def setStreamSegmentSize(self, size):
        self._stream_segment_size = size

    def setFastSMC(self, active):
        self.setFastFIFORead(active)
        self.serial.set_smc_speed(active)

    def setBitsPerSample(self, bits):
        self._bits_per_sample = bits

    def setTimeout(self, timeout):
        self._timeout = timeout

    def timeout(self):
        return self._timeout

    @property
    def _serial_not_stream(self):
        """Gets a value indicating if the serial instance is not in streaming mode.

        Return:
            False if a generic serial interface with no stream attribute or if explicitly in stream
        mode, else True if serial is explicitly not in stream mode.
        """
        return hasattr(self.serial, "stream") and self.serial.stream is False

    def _send_msg_hdr(self, mode, addr, dlen):
        """Sends a serial command header to the device.
        """
        buf = bytearray(MSG_HDR_SIZE)
        buf[0] = mode | addr
        util.pack_u16_into(buf, 1, dlen)
        self.serial.write(buf)

    def msg_read(self, address, max_resp=None):
        """Reads the response from the device after issuing a command to it.

        Return:
            A bytearray of the response received from the device.
        """
        address = self._address_str2int(address)
        if not max_resp:
            if address == self._address_str2int("ADCREAD_ADDR"):
                max_resp = 65000
            else:
                max_resp = 1

        self.flushInput()

        if self._serial_not_stream:
            if self._fast_fifo_read_active and address != self._address_str2int("ADCREAD_ADDR"):
                scope_logger.warning("Internal error: in fast read mode but not reading FIFO! (address=%0d)." % address)
                scope_logger.warning("This happens when attempting to access (read or write) some Husky FPGA setting after the")
                scope_logger.warning("ADC sample FIFO read has been set up; in stream mode, this is done when scope.arm() is called.")
                scope_logger.warning("If this happens to you as a user: access scope.* settings before calling scope.arm(), not after.")
                scope_logger.warning("If this happens to you as a developer, you can resolve this by caching the setting you need.")
                scope_logger.warning("The error trace below will show you what led to this:")
                raise ValueError

            data = self.serial.cmdReadMem(address, max_resp)
        else:
            self._send_msg_hdr(CODE_READ, address, 0)
            data = self.serial.read(max_resp)

            if len(data) < 1:
                self.flushInput()
                scope_logger.warning('Timeout in read: %d (address: 0x%02x)' % (len(data), address))
                return None

        return bytearray(data)

    def _address_str2int(self, address):
        if type(address) is str:
            if address in self.registers:
                return self.registers[address]
            else:
                raise KeyError('Unknown register %s' % address)
        else:
            return address


    def msg_write(self, address, payload):
        """Sends a command message to the device.
        """
        payload = util.get_bytes_memview(payload)
        if len(payload) < 1:
            scope_logger.warning('Invalid payload for mode')
            return

        address = self._address_str2int(address)
        if self._serial_not_stream:
            # Write output to memory
            self.serial.cmdWriteMem(address, payload)
        else:
            self._send_msg_hdr(CODE_WRITE, address, len(payload))
            self.serial.write(payload)

    def msg_validate(self, address, data, read_mask=None):
        """Validates that a previous write was successful by reading from the same address and
        comparing the payload to the provided expected data.
        """
        data = util.get_bytes_memview(data)
        pba = bytearray(len(data))
        address = self._address_str2int(address)
        check = self.msg_read(address, len(data))

        if read_mask:
            try:
                for i, m in enumerate(read_mask):
                    check[i] = check[i] & m
                    pba[i] = pba[i] & m
            except IndexError:
                pass

        if check == payload:
            return

        errmsg = "For address 0x%02x=%d" % (address, address)
        errmsg += "  Sent data: "
        for c in pba: errmsg += "%02x" % c
        errmsg += " Read data: "
        if check:
            for c in check: errmsg += "%02x" % c
        else:
            errmsg += "<Timeout>"

        scope_logger.error(errmsg)

    def sendMessage(self, mode, address, payload=None, Validate=False, maxResp=None, readMask=None):
        """Top level swiss-army knife method for sending a message.  Kept for back-compat

        TODO: Transition API calls to use simplified API and remove this function...
        """
        address = self._address_str2int(address)
        if mode != CODE_WRITE:
            return self.msg_read(address, maxResp)
        self.msg_write(address, payload)
        if Validate:
            self.msg_validate(address, payload, read_mask=readMask)

### Utility operations

    def msg_get_value(self, address, i, max_resp=None):
        """Gets a byte from the message response from the device.

        Return:
            The value of the specified byte.
        """
        address = self._address_str2int(address)
        return self.msg_read(address, max_resp=max_resp)[i]

    def msg_set_value(self, address, i, value, max_resp=None):
        """Reads a response from the device, updates a byte in the response, and writes the
        response back.

        Return:
            The updated data that was written to the device.
        """
        address = self._address_str2int(address)
        data = self.msg_read(address, max_resp=max_resp)
        data[i] = value
        self.msg_write(address, data)
        return data

    def msg_extr_mask(self, address, i, mask, max_resp=None):
        """Gets a byte from the message response and bitwise AND's a mask to it.

        Return:
            The masked value of the specified byte.
        """
        address = self._address_str2int(address)
        return self.msg_get_value(address, i, max_resp=max_resp) & mask

    def msg_ins_mask(self, address, i, mask, value, max_resp=None):
        """Reads a response from the device, bitwise NAND's a mask and bitwise OR's a value to a
        byte, and writes the response back to the device.

        Return:
            The updated data that was written back to the device.
        """
        address = self._address_str2int(address)
        data = self.msg_read(address, max_resp=max_resp)
        data[i] = (data[i] & ~mask) | (value & mask)
        self.msg_write(address, data)
        return data

    def msg_extr_field(self, address, i, bfield, max_resp=None):
        address = self._address_str2int(address)
        return bfield.extr_value(self.msg_get_value(address, i, max_resp=max_resp))

    def msg_ins_field(self, address, i, bfield, value, max_resp=None):
        address = self._address_str2int(address)
        data = self.msg_read(address, max_resp=max_resp)
        data[i] = bfield.ins_value(data[i], value)
        self.msg_write(address, data)
        return data

    def msg_test_mask(self, address, i, mask, max_resp=None):
        """Gets a masked byte from the message response and converts it to a bool.

        Return:
            False if the result was 0, else True.
        """
        address = self._address_str2int(address)
        return bool(self.msg_extr_mask(address, i, mask, max_resp=max_resp))

    def msg_match_mask(self, address, i, mask, max_resp=None):
        """Gets a masked byte from the message response and converts it to a bool.

        Return:
            False if the result was 0, else True.
        """
        address = self._address_str2int(address)
        return bool(self.msg_extr_mask(address, i, mask, max_resp=max_resp) == mask)

    def msg_set_mask(self, address, i, mask, max_resp=None):
        """Reads a response from the device, bitwise OR's a mask into a byte, and writes the
        response back to the device.

        Return:
            The updated data that was written back to the device.
        """
        address = self._address_str2int(address)
        data = self.msg_read(address, max_resp=max_resp)
        data[i] |= mask
        self.msg_write(address, data)
        return data

    def msg_clr_mask(self, address, i, mask, max_resp=None):
        """Reads a response from the device, bitwise NAND's a mask to a byte, and writes the
        response back to the device.

        Return:
            The updated data that was written back to the device.
        """
        address = self._address_str2int(address)
        data = self.msg_read(address, max_resp=max_resp)
        data[i] &= ~mask
        self.msg_write(address, data)
        return data

    def msg_upd_mask(self, address, i, mask, set, max_resp=None):
        """Reads a response from the device, conditionally bitwise OR's or NAND's a mask into a
        byte, and writes the response back to the device.

        Return:
            The updated data that was written back to the device.
        """
        address = self._address_str2int(address)
        if set:
            return self.msg_set_mask(address, i, mask, max_resp=max_resp)
        return self.msg_clr_mask(address, i, mask, max_resp=max_resp)

### Generic
    def fpga_write(self, address, data):
        """Helper function to write FPGA registers. Intended for development/debug, not for regular use.
        """
        address = self._address_str2int(address)
        return self.msg_write(address, data)

    def fpga_read(self, address, num_bytes):
        """Helper function to read FPGA registers. Intended for development/debug, not for regular use.
        """
        address = self._address_str2int(address)
        return self.msg_read(address, maxResp=num_bytes)

    def reset_fpga(self):
        """ Reset all FPGA resgiters to their defaults.
        """
        if not self._is_husky:
            raise ValueError("For CW-Husky only.")
        self.sendMessage(CODE_WRITE, "RESET", [1])
        self.sendMessage(CODE_WRITE, "RESET", [0])


    def setSettings(self, state, validate=False):
        cmd = bytearray(1)
        cmd[0] = state
        self.cached_settings = state
        self.sendMessage(CODE_WRITE, "SETTINGS_ADDR", cmd, Validate=validate)

    def settings(self, use_cached=False):
        if (not use_cached) or (not self.cached_settings):
            sets = self.sendMessage(CODE_READ, "SETTINGS_ADDR")
            if sets is None:
                self.cached_settings = 0
            else:
                self.cached_settings = sets[0]
        return self.cached_settings

    def _setReset(self, value):
        """Note: this is only intended to be called when connecting. If used outside of this, cached register values may be not reflect reality.
        """
        if value:
            self.setSettings(self.settings() | SETTINGS_RESET, validate=False)
            if self._is_husky:
                self.hwMaxSamples = self.numMaxSamples()
                self.hwMaxSegmentSamples = self.numMaxSegmentSamples()
            else:
                #Hack to adjust the hwMaxSamples since the number should be smaller than what is being returned
                self.hwMaxSamples = self.numSamples() - 45
            self.setNumSamples(self.hwMaxSamples)
        else:
            self.setSettings(self.settings() & ~SETTINGS_RESET)

    def triggerNow(self):
        initial = self.settings(True)
        self.setSettings(initial | SETTINGS_TRIG_NOW)
        time.sleep(0.05)
        self.setSettings(initial & ~SETTINGS_TRIG_NOW)

    def getStatus(self):
        result = self.sendMessage(CODE_READ, "STATUS_ADDR")

        if len(result) == 1:
            return result[0]
        else:
            return None

    def setNumSamples(self, samples, segments=1):
        self.sendMessage(CODE_WRITE, "SAMPLES_ADDR", list(int.to_bytes(samples, length=4, byteorder='little')))
        self.updateStreamBuffer(samples*segments)


    def updateStreamBuffer(self, samples=None):
        # yes this is a bit weird but it is so:
        if samples is not None:
            self._total_samples = samples
        if self._is_husky:
            if self._stream_mode:
                sbuf_len = int(self._total_samples * self._bits_per_sample / 8)
                if sbuf_len % 3:
                    # need to capture a multiple of 3 otherwise processHuskyData may fail
                    sbuf_len += 3 - sbuf_len % 3
                self._sbuf = array.array('B', [0]) * sbuf_len
                # For CW-Pro, _stream_len is the number of (10-bit) samples (which was previously set), whereas for Husky, to accomodate 8/12-bit samples, 
                # it's the total number of bytes, so we need to update _stream_len accordingly:
                self._stream_len = sbuf_len
        else:
            bufsizebytes = 0
            self._stream_len = self._total_samples
            if self._stream_mode:
                nae = self.serial
                #Save the number we will return
                bufsizebytes = nae.cmdReadStream_bufferSize(self._stream_len)

            #Generate the buffer to save buffer
            self._sbuf = array.array('B', [0]) * bufsizebytes


    def setDecimate(self, decsamples):
        cmd = bytearray(2)
        if decsamples <= 0 or decsamples >= 2**16 or not type(decsamples) is int:
            raise ValueError("Decsamples must be a positive 16-bit integer")
        decsamples -= 1
        self.sendMessage(CODE_WRITE, "DECIMATE_ADDR", list(int.to_bytes(decsamples, length=2, byteorder='little')))


    def decimate(self):
        if self._support_decimate:
            decnum = 0x00000000
            temp = self.sendMessage(CODE_READ, "DECIMATE_ADDR", maxResp=2)
            #If we don't support decimate just return 1 in the future to avoid
            if temp:
                decnum |= temp[0] << 0
                decnum |= temp[1] << 8
                decnum += 1
            else:
                self._support_decimate = False
                decnum = 1
        else:
            decnum = 1
        return decnum

    def set_clip_errors_disabled(self, disable):
        if not self._is_husky:
            raise ValueError("For CW-Husky only.")
        raw = self.sendMessage(CODE_READ, "NO_CLIP_ERRORS", maxResp=1)[0]
        if disable:
            raw |= 1 # set bit 0
        else:
            raw &= 2 # clear bit 0
        self.sendMessage(CODE_WRITE, "NO_CLIP_ERRORS", [raw])

    def clip_errors_disabled(self):
        if not self._is_husky:
            raise ValueError("For CW-Husky only.")
        if self.sendMessage(CODE_READ, "NO_CLIP_ERRORS", maxResp=1)[0] & 1:
            return True
        else:
            return False

    def set_lo_gain_errors_disabled(self, disable):
        if not self._is_husky:
            raise ValueError("For CW-Husky only.")
        raw = self.sendMessage(CODE_READ, "NO_CLIP_ERRORS", maxResp=1)[0]
        if disable:
            raw |= 2 # set bit 1
        else:
            raw &= 1 # clear bit 1
        self.sendMessage(CODE_WRITE, "NO_CLIP_ERRORS", [raw])

    def lo_gain_errors_disabled(self):
        if not self._is_husky:
            raise ValueError("For CW-Husky only.")
        if self.sendMessage(CODE_READ, "NO_CLIP_ERRORS", maxResp=1)[0] & 2:
            return True
        else:
            return False

    def numSamples(self):
        """Return the number of samples captured in one go. Returns max after resetting the hardware"""
        temp = self.sendMessage(CODE_READ, "SAMPLES_ADDR", maxResp=4)
        samples = int.from_bytes(temp, byteorder='little')
        return samples

    def numMaxSamples(self):
        """Return the maximum number of samples that can be captured in one go. Husky only."""
        if not self._is_husky:
            scope_logger.error("Supported by Husky only.")
        return int.from_bytes(self.sendMessage(CODE_READ, "MAX_SAMPLES_ADDR", maxResp=4), byteorder='little')

    def numMaxSegmentSamples(self):
        """Return the maximum number of samples that can be captured in one go when segmenting is used. Husky only."""
        if not self._is_husky:
            scope_logger.error("Supported by Husky only.")
        return int.from_bytes(self.sendMessage(CODE_READ, "MAX_SEGMENT_SAMPLES_ADDR", maxResp=4), byteorder='little')


    def getBytesInFifo(self):
        if self._is_husky:
            scope_logger.error("Shouldn't be calling getBytesInFifo on Husky: associated register doesn't exist.")
        else:
            temp = self.sendMessage(CODE_READ, "RETSAMPLES_ADDR", maxResp=4)
            samples = int.from_bytes(temp, byteorder='little')
            return samples

    def flushInput(self):
        try:
            self.serial.flushInput()
        except AttributeError:
            return

    def devicePresent(self):

        #Re-init these settings:
        self._support_decimate = True

        #Send "ping" message, wait for pong
        msgin = bytearray([])
        msgin.append(0xAC)

        self.flushInput()

        #Reset... will automatically clear by the time we are done
        self._setReset(True)
        self.flushInput()

        #Send ping
        self.sendMessage(CODE_WRITE, "ECHO_ADDR", msgin)

        #Pong?
        msgout = self.sendMessage(CODE_READ, "ECHO_ADDR")

        if (msgout != msgin):
            return False

        #Init stuff
        state = self.getStatus()

        if state & STATUS_DDRMODE_MASK:
            self.ddrMode = True
        else:
            self.ddrMode = False

        return True

    # def setDDRAddress(self, addr):
    #     cmd = bytearray(1)
    #     cmd[0] = ((addr >> 0) & 0xFF)
    #     self.sendMessage(CODE_WRITE, ADDR_DDR1, cmd)
    #     cmd[0] = ((addr >> 8) & 0xFF)
    #     self.sendMessage(CODE_WRITE, ADDR_DDR2, cmd)
    #     cmd[0] = ((addr >> 16) & 0xFF)
    #     self.sendMessage(CODE_WRITE, ADDR_DDR3, cmd)
    #     cmd[0] = ((addr >> 24) & 0xFF)
    #     self.sendMessage(CODE_WRITE, ADDR_DDR4, cmd)
    #
    # def getDDRAddress(self):
    #     addr = 0x00000000
    #     temp = self.sendMessage(CODE_READ, ADDR_DDR1)
    #     addr = addr | (temp[0] << 0)
    #     temp = self.sendMessage(CODE_READ, ADDR_DDR2)
    #     addr = addr | (temp[0] << 8)
    #     temp = self.sendMessage(CODE_READ, ADDR_DDR3)
    #     addr = addr | (temp[0] << 16)
    #     temp = self.sendMessage(CODE_READ, ADDR_DDR4)
    #     addr = addr | (temp[0] << 24)
    #     return addr

    def arm(self, enable=True):
        # keeps calling self.setting() - what if we cache it?
        if enable:
            #Must arm first
            self.setSettings(self.settings() | SETTINGS_ARM)
        else:
            self.setSettings(self.settings() & ~SETTINGS_ARM)

    def setFastFIFORead(self, active):
        if active:
            self.sendMessage(CODE_WRITE, "FAST_FIFO_READ_MODE", [1])
            self._fast_fifo_read_active = True
        else:
            self.sendMessage(CODE_WRITE, "FAST_FIFO_READ_MODE", [0])
            self._fast_fifo_read_active = False

    def startCaptureThread(self):
        # Then init the stream mode stuff
        if self._stream_mode:
            # Stream mode adds 500mS of extra timeout on USB traffic itself...
            scope_logger.debug("Stream on!")
            if self._is_husky and self._fast_fifo_read_enable:
                self.setFastSMC(1)
            self.serial.initStreamModeCapture(self._stream_len, self._sbuf, timeout_ms=int(self._timeout * 1000) + 500, \
                is_husky=self._is_husky, segment_size=self._stream_segment_size)


    def capture(self, offset=None, adc_freq=29.53E6, samples=24400, segments=1, segment_cycles=0, poll_done=False):
        timeout = False
        if self._stream_mode:

            # Wait for a trigger, letting the UI run when it can
            starttime = datetime.datetime.now()
            while self.serial.cmdReadStream_isDone(self._is_husky) == False:
                # Wait for a moment before re-running the loop
                time.sleep(0.05)
                diff = datetime.datetime.now() - starttime

                # If we've timed out, don't wait any longer for a trigger
                if (diff.total_seconds() > self._timeout):
                    scope_logger.warning('Timeout in OpenADC capture(), no trigger seen! Trigger forced, data is invalid')
                    timeout = True
                    self.triggerNow()
                    self.serial.streamModeCaptureStream.stop = True
                    break

            if self._is_husky and self._fast_fifo_read_enable:
                scope_logger.debug("DISABLING fast fifo read")
                self.setFastSMC(0)

            self._stream_rx_bytes, stream_timeout = self.serial.cmdReadStream(self._is_husky)
            timeout |= stream_timeout
            #Check the status now
            scope_logger.debug("Streaming done, results: rx_bytes = %d"%(self._stream_rx_bytes))
            self.arm(False)

            if self._is_husky and self.sendMessage(CODE_READ, "FIFO_STAT", maxResp=1)[0] & 0x0f:
                scope_logger.warning("FIFO error occured; see scope.adc.errors for details.")

            if stream_timeout:
                if self._stream_rx_bytes == 0: # == (self._stream_len - 3072):
                    scope_logger.warning("Streaming mode OVERFLOW occured as trigger too fast - Adjust offset upward (suggest = 200 000)")
                else:
                    scope_logger.warning("Streaming mode OVERFLOW occured during capture - ADC sample clock probably too fast for stream mode (keep ADC Freq < 10 MHz)")
                timeout = True
        else:
            status = self.getStatus()
            starttime = datetime.datetime.now()

            # Wait for a trigger
            while ((status & STATUS_ARM_MASK) == STATUS_ARM_MASK) | ((status & STATUS_FIFO_MASK) == 0):
                status = self.getStatus()

                diff = datetime.datetime.now() - starttime

                # If we've timed out, don't wait any longer for a trigger
                if (diff.total_seconds() > self._timeout):
                    scope_logger.warning('Timeout in OpenADC capture(), no trigger seen! Trigger forced, data is invalid. Status: %02x'%status)
                    timeout = True
                    self.triggerNow()

                    #Once in timeout mode we can't rely on STATUS_ARM_MASK anymore - just wait for FIFO to fill up
                    if (status & STATUS_FIFO_MASK) == 0:
                        break

        # give time for ADC to finish reading data
        if self._is_husky and poll_done:
            # poll Husky to find out when the capture is complete:
            starttime = datetime.datetime.now()
            while not self.sendMessage(CODE_READ, "CAPTURE_DONE", maxResp=1)[0]:
                diff = datetime.datetime.now() - starttime
                if (diff.total_seconds() > self._timeout):
                    scope_logger.warning('Timeout in OpenADC capture() waiting for scope "done" to go high.')
                    break
        else:
            # calculate how long the capture should take:
            if self._is_husky:
                # in the case of Husky, "samples" is the number of samples *per segment*:
                if segment_cycles:
                    # in the case of cycle-count-based segmenting, we can calculate the exact length of the capture in ADC samples:
                    samples = segment_cycles * segments
                else:
                    # in the case of trigger-based segmentings, this is the best we can do for the general case: we assume that one
                    # segment is <samples> long; if this doesn't work, either adjust the delay manually, or use poll_done=True
                    samples = samples * segments

            if offset == None:
                offset = 0
            if not timeout:
                time.sleep((offset+samples)/adc_freq)

        self.arm(False) # <------ ADC will stop reading after this
        return timeout

    def flush(self):
        # Flush output FIFO
        self.sendMessage(CODE_READ, "ADCREAD_ADDR", None, False, None)

    def readData(self, NumberPoints=None, progressDialog=None):
        scope_logger.debug("Reading data from OpenADC (NumberPoints=%d)..." % NumberPoints)
        if self._is_husky: 
            return self.readHuskyData(NumberPoints)
        elif self._stream_mode:
            # Process data
            bsize = self.serial.cmdReadStream_size_of_fpgablock()
            num_bytes = self.serial.cmdReadStream_bufferSize(self._stream_len)

            # Remove sync bytes from trace
            data = np.zeros(num_bytes, dtype=np.uint8)
            data[0] = self._sbuf[0]
            dbuf2_idx = 1
            for i in range(0, self._stream_rx_bytes, bsize):
                if self._sbuf[i] != 0xAC:
                    scope_logger.warning("Stream mode: Expected sync byte (AC) at location %d but got %x" % (i, self._sbuf[i]))
                    break
                s = i + 1
                data[dbuf2_idx: (dbuf2_idx + (bsize - 1))] = self._sbuf[s:(s + (bsize - 1))]

                # Write to next section
                dbuf2_idx += (bsize - 1)

            scope_logger.debug("Stream mode: read %d bytes"%len(data))

            # Turn raw bytes into samples
            datapoints = self.processData(data, 0.0)

            if datapoints is not None and len(datapoints):
                scope_logger.debug("Stream mode: done, %d samples processed"%len(datapoints))
            else:
                scope_logger.warning("Stream mode: done, no samples resulted from processing")
                datapoints = []

            if len(datapoints) > NumberPoints:
                datapoints = datapoints[0:NumberPoints]

            return datapoints

        else:
            datapoints = []

            if NumberPoints == None:
                NumberPoints = 0x1000

            if self.ddrMode:
                # We were passed number of samples to read. DDR interface
                # reads 3 points per 4 bytes, and reads in blocks of
                # 256 bytes (e.g.: 192 samples)
                NumberPackages = NumberPoints / 192

                # If user requests we send extra then scale back afterwards
                if (NumberPoints % 192) > 0:
                    NumberPackages = NumberPackages + 1

                start = 0
                self.setDDRAddress(0)


                BytesPerPackage = 257

                if progressDialog:
                    progressDialog.setMinimum(0)
                    progressDialog.setMaximum(NumberPackages)
            else:
                # FIFO takes 3 samples at a time... todo figure this out
                NumberPackages = 1

                # We get 3 samples in each word returned (word = 4 bytes)
                # So need to convert samples requested to words, rounding
                # up if we request an incomplete number
                nwords = NumberPoints / 3
                if NumberPoints % 3:
                    nwords = nwords + 1

                # Return 4x as many bytes as words, +1 for sync byte
                BytesPerPackage = nwords * 4 + 1

            for status in range(0, NumberPackages):
                # Address of DDR is auto-incremented following a read command
                # so no need to write new address

                # print "Address=%x"%self.getDDRAddress()

                # print "bytes = %d"%self.getBytesInFifo()

                bytesToRead = self.getBytesInFifo()
                #print(bytesToRead)

                # print bytesToRead

                if bytesToRead == 0:
                    bytesToRead = BytesPerPackage

                #If bytesToRead is huge, we only read what is needed
                #Bytes get packed 3 samples / 4 bytes
                #Add some extra in case needed
                hypBytes = (NumberPoints * 4)/3 + 256

                bytesToRead = min(hypBytes, bytesToRead)

                # +1 for sync byte
                data = self.sendMessage(CODE_READ, "ADCREAD_ADDR", None, False, bytesToRead + 1)  # BytesPerPackage)
                #print(data)

                # for p in data:
                #       print "%x "%p,

                if data is not None:
                    data = np.array(data)
                    datapoints = self.processData(data, 0.0)

                if progressDialog:
                    progressDialog.setValue(status)

                    if progressDialog.wasCanceled():
                        break

            # for point in datapoints:
            #       print "%3x"%(int((point+0.5)*1024))

            if datapoints is None:
                return []
            if len(datapoints) > NumberPoints:
                datapoints = datapoints[0:NumberPoints]

            if len(self._int_data) > NumberPoints:
                self._int_data = self._int_data[0:NumberPoints]

            # if len(datapoints) < NumberPoints:
            # print len(datapoints),
            # print NumberPoints

            return datapoints



    def readHuskyData(self, NumberPoints=None):
        if self._bits_per_sample == 12:
            bytesToRead = int(np.ceil(NumberPoints*1.5))
        else:
            bytesToRead = NumberPoints
        # Husky hardware always captures a multiple of 3 samples. We want to read all the captured samples.
        if bytesToRead % 3:
            bytesToRead += 3  - bytesToRead % 3
        scope_logger.debug("XXX reading with NumberPoints=%d, bytesToRead=%d" % (NumberPoints, bytesToRead))

        if self._stream_mode:
            data = self._sbuf
            scope_logger.debug('stream: got data len = %d' % len(data))
        else:
            if self._fast_fifo_read_enable:
                # switch FPGA and SAM3U into fast read timing mode
                self.setFastSMC(1)
            data = self.sendMessage(CODE_READ, "ADCREAD_ADDR", None, False, bytesToRead)
            # switch FPGA and SAM3U back to regular read timing mode
            if self._fast_fifo_read_enable:
                scope_logger.debug("DISABLING fast fifo read")
                self.setFastSMC(0)

        scope_logger.debug("XXX read %d bytes; NumberPoints=%d, bytesToRead=%d" % (len(data), NumberPoints, bytesToRead))
        if data is not None:
            data = np.array(data)
            datapoints = self.processHuskyData(NumberPoints, data)
        if datapoints is None:
            return []
        return datapoints


    def processHuskyData(self, NumberPoints, data):
        if self._bits_per_sample == 12:
            data = np.frombuffer(data, dtype=np.uint8)
            fst_uint8, mid_uint8, lst_uint8 = np.reshape(data, (data.shape[0] // 3, 3)).astype(np.uint16).T
            fst_uint12 = (fst_uint8 << 4) + (mid_uint8 >> 4)
            snd_uint12 = ((mid_uint8 % 16) << 8) + lst_uint8
            data = np.reshape(np.concatenate((fst_uint12[:, None], snd_uint12[:, None]), axis=1), 2 * fst_uint12.shape[0])

        self._int_data = data[:NumberPoints]
        fpData = data / 2**self._bits_per_sample - self.offset
        return fpData[:NumberPoints]


    def processData(self, data, pad=float('NaN'), debug=False):
        if data[0] != 0xAC:
            scope_logger.warning('Unexpected sync byte in processData(): 0x%x' % data[0])
            #print(data)
            return None

        self._int_data = None

        orig_data = copy.copy(data)
        if debug:
            fpData = []
            intData = []
            # Slow, verbose processing method
            # Useful for fixing issues in ADC read
            for i in range(1, len(data) - 3, 4):
                # Convert
                temppt = (data[i + 3] << 0) | (data[i + 2] << 8) | (data[i + 1] << 16) | (data[i + 0] << 24)

                # print("%2x "%data[i])

                # print "%x %x %x %x"%(data[i +0], data[i +1], data[i +2], data[i +3]);
                # print "%x"%temppt

                intpt1 = temppt & 0x3FF
                intpt2 = (temppt >> 10) & 0x3FF
                intpt3 = (temppt >> 20) & 0x3FF

                # print "%x %x %x" % (intpt1, intpt2, intpt3)

                if trigfound == False:
                    mergpt = temppt >> 30
                    if (mergpt != 3):
                        trigfound = True
                        trigsamp = trigsamp + mergpt
                        # print "Trigger found at %d"%trigsamp
                    else:
                        trigsamp += 3

                # input validation test: uncomment following and use
                # ramp input on FPGA
                ##if (intpt != lastpt + 1) and (lastpt != 0x3ff):
                ##    print "intpt: %x lstpt %x\n"%(intpt, lastpt)
                ##lastpt = intpt;

                # print "%x %x %x"%(intpt1, intpt2, intpt3)

                fpData.append(float(intpt1) / 1024.0 - self.offset)
                fpData.append(float(intpt2) / 1024.0 - self.offset)
                fpData.append(float(intpt3) / 1024.0 - self.offset)
                intData.extend([intpt1, intpt2, intpt3])
                
        else:
            # Fast, efficient NumPy implementation

            # Figure out how many bytes we're going to process
            # Cut off some bytes at the end: we need the length to be a multiple of 4, and we probably have extra data
            numbytes = len(data) - 1
            extralen = (len(data) - 1) % 4

            # Copy the data into a NumPy array. For long traces this is the longest part
            data = data[1:1+numbytes-extralen]

            # Split data into groups of 4 bytes and combine into words
            data = np.reshape(data, (-1, 4))
            data = np.left_shift(data, [24, 16, 8, 0])
            data = np.sum(data, 1)

            # Split words into samples and trigger bytes
            data = np.right_shift(np.reshape(data, (-1, 1)), [0, 10, 20, 30]) & 0x3FF
            fpData = np.reshape(data[:, [0, 1, 2]], (-1))
            trigger = data[:, 3] % 4
            self._int_data = np.array(fpData, dtype='int16')
            fpData = fpData / 1024.0 - self.offset
            scope_logger.debug("Trigger_data: {} len={}".format(trigger, len(trigger)))
            scope_logger.debug("Unprocessed data, fpData: {}, int_data: {}".format(len(fpData), len(self._int_data)))

            # Search for the trigger signal
            trigfound = False
            trigsamp = 0
            for t in trigger:
                if(t != 3):
                    trigfound = True
                    trigsamp = trigsamp + (t & 0x3)
                    scope_logger.debug("Trigger found at %d"%trigsamp)
                    break
                else:
                    trigsamp += 3

        #print len(fpData)

        if trigfound == False:
            scope_logger.warning('Trigger not found in ADC data. No data reported!')
            scope_logger.debug('Trigger not found typically caused by the actual \
            capture starting too late after the trigger event happens')
            scope_logger.debug('Data: {}'.format(orig_data))


        #Ensure that the trigger point matches the requested by padding/chopping
        diff = self.presamples_desired - trigsamp
        if diff > 0:
            #fpData = [pad]*diff + fpData
            fpData = np.append([pad]*diff, fpData)
            self._int_data = np.append([0x00]*diff, self._int_data)
            scope_logger.debug("Diff > 0, fpData: {}, int_data: {}".format(len(fpData), len(self._int_data)))
            scope_logger.warning('Pretrigger not met: Do not use downsampling and pretriggering at same time.')
            scope_logger.debug('Pretrigger not met: can attempt to increase presampleTempMargin(in the code).')
        else:
            scope_logger.debug("Diff <= 0, fpData: {}, int_data: {}".format(len(fpData), len(self._int_data)))
            fpData = fpData[-diff:]
            self._int_data = self._int_data[-diff:]

        scope_logger.debug("Processed data, fpData: {}, int_data: {}".format(len(fpData), len(self._int_data)))

        return fpData

class HWInformation(util.DisableNewAttr):
    _name = 'HW Information'

    def __init__(self, oaiface : OpenADCInterface):
        # oaiface = OpenADCInterface
        super().__init__()
        self.oa = oaiface
        self.oa.hwInfo = self
        self.sysFreq = 0

        self.vers = None
        self.disable_newattr()

    def versions(self):
        result = self.oa.sendMessage(CODE_READ, "VERSION_ADDR", maxResp=6)

        regver = result[0] & 0xff
        hwtype = result[1] >> 3
        hwver = result[1] & 0x07
        hwList = ["Default/Unknown", "LX9 MicroBoard", "SASEBO-W", "ChipWhisperer Rev2 LX25",
                  "Reserved?", "ZedBoard", "Papilio Pro", "SAKURA-G", "ChipWhisperer-Lite", "ChipWhisperer-CW1200", "ChipWhisperer-Husky",
                  "ChipWhisperer-Husky-Plus"]

        try:
            textType = hwList[hwtype]
        except:
            textType = "Invalid/Unknown"

        self.vers = (regver, hwtype, textType, hwver)

        #TODO: Temp fix for wrong HW reporting
        if hwtype == 1:
            self.sysFreq = 40E6

        return self.vers

    def is_cw1200(self):
        if self.vers is None:
            self.versions()
        if self.vers[1] == 9:
            return True
        else:
            return False

    def is_cwlite(self):
        if self.vers is None:
            self.versions()
        if self.vers[1] == 8:
            return True
        else:
            return False

    def is_cwhusky(self):
        if self.vers is None:
            self.versions()
        if self.vers[1] in (10,11):
            return True
        else:
            return False

    def get_fpga_buildtime(self):
        """Returns date and time when FPGA bitfile was generated.
        """
        if self.is_cwhusky():
            raw = self.oa.sendMessage(CODE_READ, "FPGA_BUILDTIME_ADDR", maxResp=4)
            # definitions: Xilinx XAPP1232
            day = raw[3] >> 3
            month = ((raw[3] & 0x7) << 1) + (raw[2] >> 7)
            year = ((raw[2] >> 1) & 0x3f) + 2000
            hour = ((raw[2] & 0x1) << 4) + (raw[1] >> 4)
            minute = ((raw[1] & 0xf) << 2) + (raw[0] >> 6)
            return "{}/{}/{}, {:02d}:{:02d}".format(month, day, year, hour, minute)
        else:
            return None


    def synthDate(self):
        return "unknown"

    def maxSamples(self):
        return self.oa.hwMaxSamples

    def sysFrequency(self, force=False):
        if (self.sysFreq > 0) & (force == False):
            return self.sysFreq

        '''Return the system clock frequency in specific firmware version'''
        temp = self.oa.sendMessage(CODE_READ, "SYSTEMCLK_ADDR", maxResp=4)
        freq = int.from_bytes(temp, byteorder='little')

        self.sysFreq = int(freq)
        return self.sysFreq

    def __del__(self):
        self.oa.hwInfo = None



class GainSettings(util.DisableNewAttr):
    _name = 'Gain Setting'

    def __init__(self, oaiface : OpenADCInterface, adc):
        # oaiface = OpenADCInterface
        super().__init__()
        self.oa = oaiface
        self.adc = adc
        self.gain_cached = 0
        self._is_husky = False
        self._vmag_highgain = 0x1f
        self._vmag_lowgain = 0
        self._clear_caches()
        self.disable_newattr()

    def _clear_caches(self):
        self.gainlow_cached = False

    def _dict_repr(self):
        rtn = OrderedDict()
        rtn['mode'] = self.mode
        rtn['gain'] = self.gain
        rtn['db'] = self.db
        return rtn

    def __repr__(self):
        return util.dict_to_str(self._dict_repr())

    def __str__(self):
        return self.__repr__()

    @property
    def db(self) -> float:
        """The gain of the ChipWhisperer's low-noise amplifier in dB. Ranges
        from -6.5 dB to 56 dB, depending on the amplifier settings.

        :Getter: Return the current gain in dB (float)

        :Setter: Set the gain level in dB

        Raises:
           ValueError: if new gain is outside of [-6.5, 56]

        Examples::

            # reading and storing
            gain_db = scope.gain.db

            # setting
            scope.gain.db = 20
        """
        return self._get_gain_db()

    @db.setter
    def db(self, val):
        return self._set_gain_db(val)

    def setMode(self, gainmode):
        """Sets the ChipWhisperer's gain to either 'low' or 'high' mode.

        This setting is applied after the gain property, resulting in the value
        of the db property. May be necessary for reaching gains higher than

        :meta private:

        Args:
           gainmode (str): Either 'low' or 'high'.

        Raises:
           ValueError: gainmode not 'low' or 'high'
        """
        if self._is_husky:
            if gainmode == "high":
                self.oa.sendMessage(CODE_WRITE, "CW_VMAG_CTRL", [self._vmag_highgain])
                self.gainlow_cached = False
            elif gainmode == "low":
                self.oa.sendMessage(CODE_WRITE, "CW_VMAG_CTRL", [self._vmag_lowgain])
                self.gainlow_cached = True
            else:
                raise ValueError("Invalid Gain Mode, only 'low' or 'high' allowed")
        else:
            if gainmode == "high":
                self.oa.setSettings(self.oa.settings() | SETTINGS_GAIN_HIGH)
                self.gainlow_cached = False
            elif gainmode == "low":
                self.oa.setSettings(self.oa.settings() & ~SETTINGS_GAIN_HIGH)
                self.gainlow_cached = True
            else:
                raise ValueError("Invalid Gain Mode, only 'low' or 'high' allowed")

    def getMode(self):
        if self._is_husky:
            if self.oa.sendMessage(CODE_READ, "CW_VMAG_CTRL")[0] == self._vmag_highgain:
                gain_high = True
            else:
                gain_high = False
        else:
            gain_high = self.oa.settings() & SETTINGS_GAIN_HIGH
        if gain_high:
            return "high"
        else:
            return "low"

    @property
    def mode(self) -> str:
        """The current mode of the LNA.

        The LNA can operate in two modes: low-gain or high-gain. Generally, the
        high-gain setting is better to use. Note that this value will be
        automatically updated if the dB gain is set.

        :Getter: Return the current gain mode ("low" or "high")

        :Setter: Set the gain mode

        Raises:
            ValueError: if mode not one of "low" or "high"
        """
        return self.getMode()

    @mode.setter
    def mode(self, val):
        return self.setMode(val)

    def setGain(self, gain):
        '''Set the Gain range: 0-78 for CW-Lite and CW-Pro; 0-109 for CW-Husky

        :meta private:

        '''
        if self._is_husky:
            maxgain = 109
        else:
            maxgain = 78
        if (gain < 0) | (gain > maxgain):
            raise ValueError("Invalid Gain, range 0-%d only" % maxgain)
        self.gain_cached = gain
        self.oa.sendMessage(CODE_WRITE, "GAIN_ADDR", [gain])
        # allow time for the new gain to "settle":
        time.sleep(0.1)

    def getGain(self, cached=False):
        if cached == False:
            self.gain_cached = self.oa.sendMessage(CODE_READ, "GAIN_ADDR")[0]

        return self.gain_cached

    @property
    def gain(self) -> int:
        """The current LNA gain setting.

        This gain is a dimensionless number in the range [0, 78]. Higher value
        causes higher gain in dB.

        Note that this function is unnecessary - the dB gain can be set
        directly. This property is only here to help convert old scripts.

        :Getter: Return the current gain setting (int)

        :Setter: Set the gain

        Raises:
            ValueError: if gain outside [0, 78]
        """
        return self.getGain()

    @gain.setter
    def gain(self, value : int):
        "test"
        self.setGain(value)

    def _get_gain_db(self):
        rawgain = self.getGain()
        if self._is_husky:
            if self.gainlow_cached:
                gaindb = -15 + 50.0*float(rawgain)/109
            else:
                gaindb = 15 + 50.0*float(rawgain)/109 
        else:
            #GAIN (dB) = 50 (dB/V) * VGAIN - 6.5 dB, (HILO = LO)
            #GAIN (dB) = 50 (dB/V) * VGAIN + 5.5 dB, (HILO = HI)
            gainV = (float(rawgain) / 256.0) * 3.3
            if self.gainlow_cached:
                gaindb = 50.0 * gainV - 6.5
            else:
                gaindb = 50.0 * gainV + 5.5

        return gaindb

    def _set_gain_db(self, gain):
        if self._is_husky:
            mingain = -15.0
            maxgain = 65.0
            use_low_thresh = 15
            steps = 109
            gainrange = 50.0
        else:
            mingain = -6.5
            maxgain = 56.0
            use_low_thresh = 5.5
            steps = 78
        if gain < mingain or gain > maxgain:
            raise ValueError("Gain %f out of range. Valid range: %3.1f to %3.1f dB" % (gain, mingain, maxgain))

        use_low = False
        if gain < use_low_thresh:
            use_low = True
            self.setMode("low")
        else:
            self.setMode("high")

        if self._is_husky:
            if use_low:
                bottom = mingain
            else:
                bottom = use_low_thresh
            g = (gain - bottom) / gainrange * steps
        else:
            if use_low:
                gv = (float(gain) - mingain) / 50.0
            else:
                gv = (float(gain) - use_low_thresh ) / 50.0
            g = (gv / 3.3) * 256.0

        g = round(g)
        g = int(g)
        if g < 0:
            g = 0
        if g > 0xFF:
            g = 0xFF

        self.setGain(g)

class TriggerSettings(util.DisableNewAttr):
    _name = 'Trigger Setup'

    def __init__(self, oaiface : OpenADCInterface):
        # oaiface = OpenADCInterface
        super().__init__()
        self._new_attributes_disabled = False
        self.oa = oaiface
        self.presamples_desired = 0
        self.presamples_actual = 0
        self.presampleTempMargin = 24
        self._timeout = 2
        self._stream_mode = False
        self._test_mode = False
        self._bits_per_sample = 10
        self._support_get_duration = True
        self._is_pro = False
        self._is_lite = False
        self._is_husky = False
        self._is_sakura_g = None
        self._clear_caches()
        self.disable_newattr()

    def _clear_caches(self):
        self._cached_samples = None
        self._cached_offset = None
        self._cached_segments = None
        self._cached_segment_cycles = None
        self._cached_decimate = None
        self._cached_presamples = None

    def _update_caches(self):
        self._cached_samples = self._get_num_samples()
        self._cached_offset = self._get_offset()
        self._cached_segments = self._get_segments()
        self._cached_segment_cycles = self._get_segment_cycles()
        self._cached_decimate = self._get_decimate()
        self._cached_presamples = self._get_presamples()

    def _dict_repr(self):
        rtn = OrderedDict()
        rtn['state']      = self.state
        rtn['basic_mode'] = self.basic_mode
        rtn['timeout']    = self.timeout
        rtn['offset']     = self.offset
        rtn['presamples'] = self.presamples
        rtn['samples']    = self.samples
        rtn['decimate']   = self.decimate
        rtn['trig_count'] = self.trig_count
        if self._is_pro or self._is_lite:
            rtn['fifo_fill_mode'] = self.fifo_fill_mode
        if self._is_pro or self._is_husky:
            rtn['stream_mode'] = self.stream_mode
        if self._is_husky:
            rtn['test_mode'] = self.test_mode
            rtn['bits_per_sample'] = self.bits_per_sample
            rtn['segments'] = self.segments
            rtn['segment_cycles'] = self.segment_cycles
            rtn['segment_cycle_counter_en'] = self.segment_cycle_counter_en
            rtn['clip_errors_disabled'] = self.clip_errors_disabled
            rtn['lo_gain_errors_disabled'] = self.lo_gain_errors_disabled
            rtn['errors'] = self.errors
            # keep these hidden:
            #rtn['stream_segment_size'] = self.stream_segment_size
            #rtn['stream_segment_threshold'] = self.stream_segment_threshold

        return rtn

    def __repr__(self):
        return util.dict_to_str(self._dict_repr())

    def __str__(self):
        return self.__repr__()

    @property
    def state(self):
        """The current state of the trigger input.

        This is a digital value (ie: high or low), which is some combination
        of the pins in the triggermux object. Read-only.

        Getter: Return the current state (True or False).
        """
        return self.extTriggerPin()

    @property
    def stream_mode(self):
        """The ChipWhisperer's streaming status. Only available on CW1200 and CW-Husky.

        When stream mode is enabled, the ChipWhisperer sends back ADC data as
        soon as it is recorded. In this mode, there is no hardware limit on the
        maximum number of samples per trace (although Python may run out of
        memory when recording billions of points). However, there is a
        maximum streaming data rate, which is approximately 10 Msamp/s.

        Note that no pre-trigger samples can be recorded when stream mode
        is enabled.

        :Getter: Return True if stream mode is enabled and False otherwise

        :Setter: Enable or disable stream mode
        """
        return self._get_stream_mode()

    @stream_mode.setter
    def stream_mode(self, enabled):
        self._set_stream_mode(enabled)

    @property
    def stream_segment_threshold(self):
        """Only available on CW-Husky. ** Internal parameter which should not
        be tweaked unless you know what you're doing. **
        
        For streaming, this many samples must be available to be read from the
        FPGA before the SAM3U starts a burst read of <stream_segment_size>
        bytes.  Normally these parameters are both set to 65536. Under some
        conditions it may be possible to obtain higher streaming performance by
        tweaking these parameters -- this depends on the sampling rate and
        capture size. But it's also easy to degrade performance.  

        :meta private:
        """ 
        return self._get_stream_segment_threshold()

    @stream_segment_threshold.setter
    def stream_segment_threshold(self, size):
        if size < 1 or size > 131070 or not type(size) is int:
            raise ValueError("Number of segments must be in range [1, 131070]")
        self._set_stream_segment_threshold(size)


    @property
    def stream_segment_size(self):
        """Only available on CW-Husky. ** Internal parameter which should not
        be tweaked unless you know what you're doing. **
        
        For streaming, this is the size of the burst that the SAM3U reads from
        from the FPGA.  A burst read doesn't start until
        <stream_segment_threshold> bytes are available to be read from the
        FPGA.  Normally these parameters are both set to 65536. Under some
        conditions it may be possible to obtain higher streaming performance by
        tweaking these parameters -- this depends on the sampling rate and
        capture size. But it's also easy to degrade performance.  

        :meta private:

        """
        return self._get_stream_segment_size()

    @stream_segment_size.setter
    def stream_segment_size(self, size):
        if size < 1 or size > 131070 or not type(size) is int:
            raise ValueError("Number of segments must be in range [1, 131070]")
        self._set_stream_segment_size(size)



    @property
    def decimate(self):
        """The ADC downsampling factor.

        This value instructs the ChipWhisperer to only record 1 sample in
        every <decimate>. In other words, if this value is set to 10, the
        sampling rate is set to 1/10th of the sampling clock.

        This setting is helpful for recording very long operations or for
        reducing the sampling rate for streaming mode.

        :Getter: Return an integer with the current decimation factor

        :Setter: Set the decimation factor

        Raises:
           ValueError: if the new factor is not positive
        """
        if self._cached_decimate is None:
            self._cached_decimate = self._get_decimate()
        return self._cached_decimate

    @decimate.setter
    def decimate(self, decfactor):
        self._set_decimate(decfactor)

    @property
    def clip_errors_disabled(self):
        """By default, ADC clipping is flagged as an error. Disable if you
        do not want this error notification.
        """
        return self.oa.clip_errors_disabled()

    @clip_errors_disabled.setter
    def clip_errors_disabled(self, disable):
        self.oa.set_clip_errors_disabled(disable)

    @property
    def lo_gain_errors_disabled(self):
        """By default, captures which use less than a quarter of the ADC's
        dynamic range flag an error, to indicate that the gain should be
        increased. Disable if you do not want this error notification.
        """
        return self.oa.lo_gain_errors_disabled()

    @lo_gain_errors_disabled.setter
    def lo_gain_errors_disabled(self, disable):
        self.oa.set_lo_gain_errors_disabled(disable)

    def disable_clip_and_lo_gain_errors(self, disabled):
        self.lo_gain_errors_disabled = disabled
        self.clip_errors_disabled = disabled

    @property
    def samples(self):
        """The number of ADC samples to record in a single capture.

        The maximum number of samples is hardware-dependent:
        - cwlite: 24400
        - cw1200: 96000
        - cwhusky: 131070

        :Getter: Return the current number of total samples (integer)

        :Setter: Set the number of samples to capture

        Raises:
           ValueError: if number of samples is negative
        """
        if self._cached_samples is None:
            self._cached_samples = self._get_num_samples()
        return self._cached_samples

    @samples.setter
    def samples(self, samples):
        if self._is_sakura_g:
            diff = (12 - (samples % 12)) % 12
            samples += diff
            if diff > 0:
                scope_logger.warning("Sakura G samples must be divisible by 12, rounding up to {}...".format(samples))

        if self._get_fifo_fill_mode() == "segment":
            diff = ((3 - (samples - 1)) % 3)
            samples += diff
            if diff > 0:
                scope_logger.warning("segment mode requires (samples-1) divisible by 3, rounding up to {}...".format(samples))

        self._cached_samples = samples
        self._set_num_samples(samples)

    @property
    def fifo_state(self):
        """Husky only, for debugging: return the state of the Husky FIFO FSM.
        """
        if not self._is_husky:
            raise ValueError("For CW-Husky only.")
        state = self.oa.sendMessage(CODE_READ, "FIFO_STATE", maxResp=1)[0]
        if state == 0:
            return 'IDLE'
        elif state == 1:
            return 'PRESAMP_FILLING'
        elif state == 2:
            return 'PRESAMP_FULL'
        elif state == 3:
            return 'TRIGGERED'
        elif state == 4:
            return 'SEGMENT_DONE'
        elif state == 5:
            return 'DONE'
        else:
            raise ValueError("Unexpected value: %d" % state)


    @property
    def timeout(self):
        """The number of seconds to wait before aborting a capture.

        If no trigger event is detected before this time limit is up, the
        capture fails and no data is returned.

        :Getter: Return the number of seconds before a timeout (float)

        :Setter: Set the timeout in seconds
        """
        return self._get_timeout()

    @timeout.setter
    def timeout(self, timeout):
        self._set_timeout(timeout)

    @property
    def offset(self):
        """The number of samples to wait before recording data after seeing a
        trigger event.

        This offset is useful for long operations. For instance, if an
        encryption is 1 million samples long, it's difficult to capture the
        entire power trace, but an offset can be used to skip to the end of
        the encryption.

        The offset must be a 32 bit unsigned integer.

        :Getter: Return the current offset (integer)

        :Setter: Set a new offset

        Raises:
           ValueError: if offset outside of range [0, 2**32)
        """
        if self._cached_offset is None:
            self._cached_offset = self._get_offset()
        return self._cached_offset

    @offset.setter
    def offset(self, setting):
        self._cached_offset = setting
        self._set_offset(setting)

    @property
    def presamples(self):
        """The number of samples to record from before the trigger event.

        This setting must be a positive integer, and it cannot be larger than
        the number of samples. When streaming mode is enabled, this value is
        set to 0.

        :Getter: Return the current number of presamples

        :Setter: Set the number of presamples.

        Raises:
           ValueError: if presamples is outside of range [0, samples]
        """
        if self._cached_presamples is None:
            self._cached_presamples = self._get_presamples()
        return self._cached_presamples

    @presamples.setter
    def presamples(self, setting):
        self._cached_presamples = self._set_presamples(setting)

    @property
    def basic_mode(self) -> str:
        """The type of event to use as a trigger.

        Only applies to the ADC capture - the glitch module
        is always a rising edge trigger.

        There are four possible types of trigger events:
         * "low": triggers when line is low (logic 0)
         * "high": triggers when line is high (logic 1)
         * "rising_edge": triggers when line transitions from low to high
         * "falling_edge:" triggers when line transitions from high to low

        .. warning:: This must be set to "rising_edge" if a trigger other than
            "basic" is used. The SAD/DecodeIO triggers will not work with any
            other setting!

        :Getter: Return the current trigger mode (one of the 4 above strings)

        :Setter: Set the trigger mode

        Raises:
           ValueError: if value is not one of the allowed strings
        """
        param_alias = {
            "rising edge": "rising_edge",
            "falling edge": "falling_edge",
            "high": "high",
            "low": "low"
        }
        return param_alias[self._get_mode()]

    @basic_mode.setter
    def basic_mode(self, mode : str):
        api_alias = {
            "rising_edge": "rising edge",
            "falling_edge": "falling edge",
            "high": "high",
            "low": "low"
        }
        if mode not in api_alias:
            raise ValueError("Invalid trigger mode %s. Valid modes: %s" % (mode, list(api_alias.keys())), mode)

        self._set_mode(api_alias[mode])

    @property
    def trig_count(self) -> int:
        """The number of samples that the trigger input was active.

        This value indicates how long the trigger was high or low last time
        a trace was captured. It is the number of samples where the input was
        low (in "low" or "falling edge" modes) or high (in "high" or "rising
        edge" modes). Read-only.

        This counter is not meaningful if the trigger is still active.

        :Getter: Return the last trigger duration (integer)
        """
        return self._get_duration()

    @property
    def fifo_fill_mode(self) -> str:
        """The ADC buffer fill strategy - allows segmented usage for CW-lite and CW-pro.

        .. warning:: THIS REQUIRES NEW FPGA BITSTREAM - NOT YET IN THE PYTHON.

        Only the 'Normal' mode is well supported, the other modes can
        be used carefully.

        For segmenting on CW-Husky, see 'segments' instead.

        There are four possible modes:
         * "normal": Trigger line & logic work as expected.
         * "enable": Capture starts with rising edge, but writing samples
                     is enabled by active-high state of trigger line.
         * "segment": Capture starts with rising edge, and writes `trigger.samples`
                     to buffer on each rising edge, stopping when the buffer
                     is full. For this to work adc.samples must be a multiple
                     of 3 (will be enforced by API).

        .. warning:: The "enable" and "segment" modes requires you to fill
                    the **full buffer** (~25K on CW-Lite, ~100K on CW-Pro).
                    This requires you to ensure the physical trigger line will
                    be high (enable mode) or toggle (segment mode) enough. The
                    ChipWhisperer hardware will currently stall until the
                    internal buffer is full, and future commands will fail.

        .. warning:: adc.basic_mode must be set to "rising_edge" if a fill_mode other than
                    "normal" is used. Bad things happen if not.

        :Getter: Return the current fifo fill mode (one of the 3 above strings)

        :Setter: Set the fifo fill mode

        Raises:
           ValueError: if value is not one of the allowed strings
        """

        return self._get_fifo_fill_mode()

    @fifo_fill_mode.setter
    def fifo_fill_mode(self, mode : str):
        known_modes = ["normal", "enable", "segment"]
        if mode not in known_modes:
            raise ValueError("Invalid fill mode %s. Valid modes: %s" % (mode, known_modes), mode)

        self._set_fifo_fill_mode(mode)

        # Segment mode requires samples have an odd divisability to work
        if mode == "segment":
            self.samples = self.samples

    def _get_fifo_fill_mode(self):
        result = self.oa.sendMessage(CODE_READ, "ADVCLOCK_ADDR", maxResp=4)
        mode = result[3] & 0x30

        if mode == 0x00:
            return "normal"

        if mode == 0x10:
            return "enable"

        if mode == 0x20:
            return "segment"

        return "????"

    def _set_fifo_fill_mode(self, mode):
        if mode == "normal":
            mask = 0
        elif mode == "enable":
            mask = 1
        elif mode == "segment":
            mask = 2
        else:
            raise ValueError("Invalid option for fifo mode: {}".format(mask))

        result = self.oa.sendMessage(CODE_READ, "ADVCLOCK_ADDR", maxResp=4)
        result[3] &= ~(0x30)
        result[3] |= mask << 4
        self.oa.sendMessage(CODE_WRITE, "ADVCLOCK_ADDR", result, readMask= [0x3f, 0xff, 0xff, 0xfd])


    @property
    def segments(self) -> int:
        """Number of sample segments to capture.

        .. warning:: Supported by CW-Husky only. For segmenting on CW-lite or
            CW-pro, see 'fifo_fill_mode' instead.

        This setting must be a 16-bit positive integer. 

        In normal operation, segments=1. 

        Multiple segments are useful in two scenarios:

        #. Capturing only subsections of a power trace, to allow longer
            effective captures.  After a trigger event, the requested number of
            samples is captured every 'segment_cycles' clock cycles, 'segments'
            times. Set 'segment_cycle_counter_en' to 1 for this segment mode.
        #. Speeding up capture times by capturing 'segments' power traces from
            a single arm + capture event. Here, the requested number of samples
            is captured at every trigger event, without having to re-arm and
            download trace data between every trigger event. Set
            'segment_cycle_counter_en' to 0 for this segment mode.

        .. warning:: when capturing multiple segments with presamples, the total number of samples 
            per segment must be a multiple of 3. Incorrect sample data will be obtained if this is not 
            the case.

        :Getter: Return the current number of presamples

        :Setter: Set the number of presamples.

        Raises:
           ValueError: if segments is outside of range [1, 2^16-1]
        """

        if self._cached_segments is None:
            self._cached_segments = self._get_segments()
        return self._cached_segments


    @segments.setter
    def segments(self, num : int):
        if num < 1 or num > 2**16-1 or not type(num) is int or not self._is_husky:
            raise ValueError("Number of segments must be in range [1, 2^16-1]. For CW-Husky only.")
        self._cached_segments = num
        self._set_segments(num)

    def _get_segments(self):
        if self.oa is None:
            return 0
        elif not self._is_husky:
            return 1
        cmd = self.oa.sendMessage(CODE_READ, "NUM_SEGMENTS", maxResp=2)
        segments = int.from_bytes(cmd, byteorder='little')
        return segments


    def _set_segments(self, num):
        self.oa.sendMessage(CODE_WRITE, "NUM_SEGMENTS", list(int.to_bytes(num, length=2, byteorder='little')))
        # necessary for streaming to work:
        self.oa.setNumSamples(self.samples, self.segments)


    @property
    def errors(self) -> Union[str, bool, int]:
        """Internal error flags (FPGA FIFO over/underflow)

        .. warning:: Supported by CW-Husky only.

        Error types and their causes:
            * 'presample error': capture trigger occurs before the requested
                    number of presamples have been collected. Reduce 
                    scope.adc.presamples or delay the capture trigger.
            * 'ADC clipped': gain is too high; reduce it (scope.gain) or disable 
                    this error (scope.adc.clip_errors_disabled).
            * 'gain too low error': gain is "too low" (4 bits or more of the ADC's
                    dynamic range did not get used); increase it (scope.gain) or 
                    disable this error (scope.adc.lo_gain_errors_disabled).
            * 'invalid downsample setting': using downsampling (aka decimating) with
                    presamples and multiple segments is not allowed.
            * 'segmenting error': the condition for starting the capture of the next
                    segment came true before the capture of the current segment
                    completed. Reduce the segment size and/or increase the time
                    between segments.
            * 'fast FIFO underflow': shouldn't occur in isolation without
                    other errors being flagged.
            * 'fast FIFO overflow': data is coming in fast than it's being read;
                    reduce scope.clock.adc_freq.
            * 'slow FIFO underflow': host tried to read more ADC samples than are
                    available.
            * 'slow FIFO overflow': data is coming in faster than it's being
                    read; reduce scope.clock.adc_freq.

        To fully understand the four different FIFO errors (fast/slow
        over/underflows), some background on Husky's sample storage
        architecture is required.  ADC samples are first stored in a "fast"
        FIFO which runs at the ADC sampling rate, then moved to a wider and
        "slower" FIFO which is read by the host. Overflows or underflows can
        occur in either FIFO. Errors can be caused from an illegal
        configuration of scope.adc (e.g. too many samples), or attempting to
        stream too fast.

        :Getter: Return the error flags.

        :Setter: Clear error flags.

        """
        return self._get_errors("FIFO_STAT")

    @errors.setter
    def errors(self, val : Any):
        """Internal error flags (FPGA FIFO over/underflow)

        .. warning:: Supported by CW-Husky only.

        """
        self.oa.sendMessage(CODE_WRITE, "FIFO_STAT", [1])
        self.oa.sendMessage(CODE_WRITE, "FIFO_STAT", [0])


    @property
    def first_error(self):
        """Reports the first error that was flagged (self.errors reports *all* errors). Useful for debugging. Read-only.

        .. warning:: Supported by CW-Husky only.

        :Getter: Return the error flags.

        """
        if not self._is_husky:
            raise ValueError("For CW-Husky only.")
        return self._get_errors("FIFO_FIRST_ERROR")


    @property
    def first_error_state(self) -> str:
        """Reports the state the FPGA FSM state at the time of the first flagged error. Useful for debugging. Read-only.

        .. warning:: Supported by CW-Husky only.

        :Getter: Return the error flags.

        """
        if not self._is_husky:
            raise ValueError("For CW-Husky only.")
        raw = self.oa.sendMessage(CODE_READ, "FIFO_FIRST_ERROR", maxResp=3)[2]
        if   raw == 0: return "IDLE"
        elif raw == 1: return "PRESAMP_FILLING"
        elif raw == 2: return "PRESAMP_FULL"
        elif raw == 3: return "TRIGGERED"
        elif raw == 4: return "SEGMENT_DONE"
        elif raw == 5: return "DONE"
        else:
            raise ValueError(raw)


    def _get_errors(self, addr):
        if self.oa is None:
            return 0
        raw = self.oa.sendMessage(CODE_READ, addr, maxResp=2)
        stat = ''
        if raw[0] & 1:   stat += 'slow FIFO underflow, '
        if raw[0] & 2:   stat += 'slow FIFO overflow, '
        if raw[0] & 4:   stat += 'fast FIFO underflow, '
        if raw[0] & 8:   stat += 'fast FIFO overflow, '
        if raw[0] & 16:  stat += 'presample error, '
        if raw[0] & 32:  stat += 'ADC clipped, '
        if raw[0] & 64:  stat += 'invalid downsample setting, '
        if raw[0] & 128: stat += 'segmenting error, '
        if raw[1] & 1:   stat += 'gain too low error, '
        if raw[1] & 2:   stat += 'trigger too soon error, '
        if self._is_pro:
            if raw[1] & 4:   stat += 'pre-DDR FIFO underflow, '
            if raw[1] & 8:   stat += 'pre-DDR FIFO overflow, '
            if raw[1] & 16:  stat += 'reading too soon, '
            if raw[1] & 32:  stat += 'DDR full, '
        if stat == '':
            stat = False
        return stat


    @property
    def segment_cycles(self) -> int:
        """Number of clock cycles separating segments.

        .. warning:: Supported by CW-Husky only. For segmenting on CW-lite or
            CW-pro, see 'fifo_fill_mode' instead.

        This setting must be a 20-bit positive integer. 

        When 'segments' is greater than one, set segment_cycles to a non-zero
        value to capture a new segment every 'segment_cycles' clock cycles
        following the initial trigger event. 'segment_cycle_counter_en' must
        also be set.

        Typically, segment_cycles should be much greater than
        scope.adc.samples. If they are too close, capture will fail (indicated by
        the blinking red lights and scope.adc.errors showing either a
        segmenting error or a FIFO over/underflow error). 
        When presamples = 0, segment_cycles >= samples + 10.
        When presamples > 0, segment_cycles >= samples + presamples AND segment_cycles >= samples + 10.

        :Getter: Return the current value of segment_cycles.

        :Setter: Set segment_cycles.

        Raises:
           ValueError: if segments is outside of range [0, 2^20-1]
        """

        if self._cached_segment_cycles is None:
            self._cached_segment_cycles = self._get_segment_cycles()
        return self._cached_segment_cycles

    @segment_cycles.setter
    def segment_cycles(self, num : int):
        if num < 0 or num > 2**20-1 or not type(num) is int or not self._is_husky:
            raise ValueError("Number of segments must be in range [0, 2^20-1]. For CW-Husky only.")
        self._cached_segment_cycles = num
        self._set_segment_cycles(num)

    def _get_segment_cycles(self):
        if self.oa is None:
            return 0
        elif not self._is_husky:
            return 0

        cmd = self.oa.sendMessage(CODE_READ, "SEGMENT_CYCLES", maxResp=3)
        segment_cycles = int.from_bytes(cmd, byteorder='little')
        return segment_cycles


    def _set_segment_cycles(self, num):
        self.oa.sendMessage(CODE_WRITE, "SEGMENT_CYCLES", list(int.to_bytes(num, length=3, byteorder='little')))



    @property
    def segment_cycle_counter_en(self) -> bool:
        """Number of clock cycles separating segments.

        .. warning:: Supported by CW-Husky only. For segmenting on CW-lite or
            CW-pro, see 'fifo_fill_mode' instead.

        Set to 0 to capture a new power trace segment every time the target
        issues a trigger event.

        Set to 1 to capture a new power trace segment every 'segment_cycles'
        clock cycles after a single trigger event.

        :Getter: Return the current value of segment_cycle_counter_en.

        :Setter: Set segment_cycles.
        """
        if not self._is_husky:
            raise ValueError("For CW-Husky only.")
        raw = self.oa.sendMessage(CODE_READ, "SEGMENT_CYCLE_COUNTER_EN", Validate=False, maxResp=1)[0]
        if raw == 1:
            return True
        elif raw == 0:
            return False
        else:
            raise ValueError("Unexpected: read %d" % raw)

    @segment_cycle_counter_en.setter
    def segment_cycle_counter_en(self, enable : bool):
        if enable:
            val = [1]
        else:
            val = [0]
        self.oa.sendMessage(CODE_WRITE, "SEGMENT_CYCLE_COUNTER_EN", val, Validate=False)



    def _set_stream_mode(self, enabled):
        if self._is_lite:
            raise ValueError("Not supported on CW-Lite.")
        self._stream_mode = enabled

        #Write to FPGA
        base = self.oa.sendMessage(CODE_READ, "SETTINGS_ADDR")[0]
        if enabled:
            val = base | (1<<4)
        else:
            val = base & ~(1<<4)
        self.oa.sendMessage(CODE_WRITE, "SETTINGS_ADDR", [val])

        #Notify capture system
        self.oa.setStreamMode(enabled)

    def _get_stream_mode(self):
        return self._stream_mode


    def _set_stream_segment_threshold(self, size):
        scope_logger.warning('Changing this parameter can degrade performance and/or cause reads to fail entirely; use at your own risk.')
        #Write to FPGA
        self.oa.sendMessage(CODE_WRITE, "STREAM_SEGMENT_THRESHOLD", list(int.to_bytes(size, length=3, byteorder='little')))


    def _set_stream_segment_size(self, size):
        scope_logger.warning('Changing this parameter can degrade performance and/or cause reads to fail entirely; use at your own risk.')
        #Notify capture system
        self.oa.setStreamSegmentSize(size)


    def _get_stream_segment_threshold(self):
        raw = self.oa.sendMessage(CODE_READ, "STREAM_SEGMENT_THRESHOLD", maxResp=3)
        return int.from_bytes(raw, byteorder='little')

    def _get_stream_segment_size(self):
        return self.oa._stream_segment_size

    def _set_test_mode(self, enabled):
        self._test_mode = enabled
        if enabled:
            fifo_config = self.oa.sendMessage(CODE_READ, "FIFO_CONFIG", maxResp=1)[0]
            fifo_config &= 0b11111101
            self.oa.sendMessage(CODE_WRITE, "FIFO_CONFIG", [fifo_config])
            self.oa.sendMessage(CODE_WRITE, "NO_CLIP_ERRORS", [1])
            if self._bits_per_sample == 8:
                self.oa.sendMessage(CODE_WRITE, "ADC_LOW_RES", [3]) # store LSB instead of MSB
        else:
            fifo_config = self.oa.sendMessage(CODE_READ, "FIFO_CONFIG", maxResp=1)[0]
            fifo_config |= 0b00000010
            self.oa.sendMessage(CODE_WRITE, "FIFO_CONFIG", [fifo_config])
            self.oa.sendMessage(CODE_WRITE, "NO_CLIP_ERRORS", [0])
            self.bits_per_sample = self._bits_per_sample #shorthand to clear the LSB setting


    def _get_test_mode(self):
        return self._test_mode

    @property
    def test_mode(self):
        """The ChipWhisperer's test mode. Only available on CW-Husky.

        When test mode is enabled, an internally-generated count-up pattern is
        captured, instead of the ADC sample data.

        :Getter: Return True if test mode is enabled and False otherwise

        :Setter: Enable or disable test mode

        :meta private:

        """
        return self._get_test_mode()

    @test_mode.setter
    def test_mode(self, enabled):
        self._set_test_mode(enabled)


    def _set_bits_per_sample(self, bits):
        self._bits_per_sample = bits
        # update FPGA:
        if bits == 8:
            if self.test_mode:
                val = 3 # store LSB instead of MSB
            else:
                val = 1
        else:
            val = 0
        self.oa.sendMessage(CODE_WRITE, "ADC_LOW_RES", [val])
        # Notify capture system:
        self.oa.setBitsPerSample(bits)
        # necessary for streaming to work:
        self.oa.setNumSamples(self.samples, self.segments)

    def _get_bits_per_sample(self):
        return self._bits_per_sample

    @property
    def bits_per_sample(self) -> int:
        """Bits per ADC sample. Only available on CW-Husky.

        Husky has a 12-bit ADC; optionally, we read back only 8 bits per
        sample.  This does *not* allow for more samples to be collected; it
        only allows for a faster sampling rate in streaming mode.

        :Getter: return the number of bits per sample that will be received.

        :Setter: set the number of bits per sample to receive.
        """
        return self._get_bits_per_sample()

    @bits_per_sample.setter
    def bits_per_sample(self, bits : int):
        if bits not in [8,12]:
            raise ValueError("Valid settings: 8 or 12.")
        if not self._is_husky:
            raise ValueError('For CW-Husky only.')
        self._set_bits_per_sample(bits)


    def fifoOverflow(self):
        return self.oa.getStatus() & STATUS_OVERFLOW_MASK

    def _set_decimate(self, decsamples):
        if self.presamples > 0 and decsamples > 1 and self._is_husky:
            raise Warning("Decimating with presamples is not supported on Husky.")
        self.oa.setDecimate(decsamples)
        self._cached_decimate = decsamples

    def _get_decimate(self):
        return self.oa.decimate()

    def clear_clip_errors(self):
        """ADC clipping errors are sticky until manually cleared by calling this.
        """
        self.oa.sendMessage(CODE_WRITE, "FIFO_STAT", [1])
        self.oa.sendMessage(CODE_WRITE, "FIFO_STAT", [0])

    def _set_num_samples(self, samples):
        if samples < 0 or not type(samples) is int:
            raise ValueError("Samples must be a positive integer")
        if self._is_husky and samples < 7:
            scope_logger.warning('There may be issues with this few samples on Husky; a minimum of 7 samples is recommended')
        if self._is_husky:
            segments = self.segments
        else:
            segments = 1
        self.oa.setNumSamples(samples, segments)

    def _get_num_samples(self):
        if self.oa is None:
            return 0
        return self.oa.numSamples()


    def _get_underflow_reads(self):
        """ Number of slow FIFO underflow reads. HW resets this on every capture.
        Count is valid even when the associated error flag is disabled.
        8 bits only, doesn't overflow. Meant for debugging.
        Husky only.
        """
        if self.oa is None or not self._is_husky:
            return 0
        return self.oa.sendMessage(CODE_READ, "FIFO_UNDERFLOW_COUNT", maxResp=1)[0]


    def _set_timeout(self, timeout):
        self._timeout = timeout
        if self.oa:
            self.oa.setTimeout(timeout)

    def _get_timeout(self):
        return self._timeout

    def _set_offset(self,  offset):
        if offset < 0 or offset >= 2**32 or not type(offset) is int:
            raise ValueError("Offset must be a non-negative 32-bit unsigned integer")
        self.oa.sendMessage(CODE_WRITE, "OFFSET_ADDR", list(int.to_bytes(offset, length=4, byteorder='little')))

    def _get_offset(self):
        if self.oa is None:
            return 0

        cmd = self.oa.sendMessage(CODE_READ, "OFFSET_ADDR", maxResp=4)
        offset = int.from_bytes(cmd, byteorder='little')
        return offset

    def _set_presamples(self, samples):
        if self._is_husky:
            min_samples = 8
            max_samples = min(self.samples-2, 32767)
            presamp_bytes = 2
            if self.decimate > 1:
                raise Warning("Decimating with presamples is not supported on Husky.")
        else:
            min_samples = 0
            max_samples = self.samples
            presamp_bytes = 4
        if samples < min_samples and samples != 0:
            raise ValueError("Number of pre-trigger samples cannot be less than %d" % min_samples)
        if samples > max_samples:
            if self._is_husky:
                raise ValueError("Number of pre-trigger samples cannot be larger than the lesser of [total number of samples, 32767] (%d)." % max_samples)
            else:
                raise ValueError("Number of pre-trigger samples cannot be larger than the total number of samples (%d)." % max_samples)

        self.presamples_desired = samples

        if self._is_pro or self._is_lite or self._is_husky:
            #CW-1200 Hardware / CW-Lite / CW-Husky
            samplesact = int(samples)
            self.presamples_actual = samplesact
        else:
            #Other Hardware
            if samples > 0:
                scope_logger.warning('Pre-sample on CW-Lite is unreliable with many FPGA bitstreams. '
                                'Check data is reliably recorded before using in capture.')

            #enforce samples is multiple of 3
            samplesact = int(samples / 3)

            #Old crappy FIFO system that requires the following
            if samplesact > 0:
                samplesact = samplesact + self.presampleTempMargin

            self.presamples_actual = samplesact * 3

        self.oa.sendMessage(CODE_WRITE, "PRESAMPLES_ADDR", list(int.to_bytes(samplesact, length=presamp_bytes, byteorder='little')))


        #print "Requested presamples: %d, actual: %d"%(samples, self.presamples_actual)

        self.oa.presamples_desired = samples

        return self.presamples_actual

    def _get_presamples(self, cached=False):
        """If cached returns DESIRED presamples"""
        if self.oa is None:
            return 0

        if cached:
            return self.presamples_desired

        if self._is_husky:
            presamp_bytes = 2
        else:
            presamp_bytes = 4

        temp = self.oa.sendMessage(CODE_READ, "PRESAMPLES_ADDR", maxResp=presamp_bytes)
        samples = int.from_bytes(temp, byteorder='little')

        #CW1200/CW-Lite/Husky reports presamples using different method
        if self._is_pro or self._is_lite or self._is_husky:
            self.presamples_actual = samples

        else:
            self.presamples_actual = samples*3

        return self.presamples_actual

    def _set_mode(self,  mode):
        """ Input to trigger module options: 'rising edge', 'falling edge', 'high', 'low' """
        if mode == 'rising edge':
            trigmode = SETTINGS_TRIG_HIGH | SETTINGS_WAIT_YES

        elif mode == 'falling edge':
            trigmode = SETTINGS_TRIG_LOW | SETTINGS_WAIT_YES

        elif mode == 'high':
            trigmode = SETTINGS_TRIG_HIGH | SETTINGS_WAIT_NO

        elif mode == 'low':
            trigmode = SETTINGS_TRIG_LOW | SETTINGS_WAIT_NO

        else:
            raise ValueError("%s invalid trigger mode. Valid modes: 'rising edge', 'falling edge', 'high', 'low'"%mode)

        cur = self.oa.settings() & ~(SETTINGS_TRIG_HIGH | SETTINGS_WAIT_YES)
        self.oa.setSettings(cur | trigmode)

    def _get_mode(self):
        if self.oa is None:
            return 'low'

        sets = self.oa.settings()
        case = sets & (SETTINGS_TRIG_HIGH | SETTINGS_WAIT_YES)

        if case == SETTINGS_TRIG_HIGH | SETTINGS_WAIT_YES:
            mode = "rising edge"
        elif case == SETTINGS_TRIG_LOW | SETTINGS_WAIT_YES:
            mode = "falling edge"
        elif case == SETTINGS_TRIG_HIGH | SETTINGS_WAIT_NO:
            mode = "high"
        else:
            mode = "low"

        return mode

    def extTriggerPin(self):
        if (self.oa is not None) and (self.oa.getStatus() & STATUS_EXT_MASK):
            return True
        else:
            return False

    def _get_duration(self):
        """Returns previous trigger duration. Cleared by arm automatically. Invalid if trigger is currently active."""
        if self.oa is None:
            return 0

        if self._support_get_duration:

            temp = self.oa.sendMessage(CODE_READ, "TRIGGER_DUR_ADDR", maxResp=4)

            #Old versions don't support this feature
            if temp is None:
                self._support_get_duration = False
                return -1

            samples = int.from_bytes(temp, byteorder='little')
            return samples

        else:

            return -1

class ClockSettings(util.DisableNewAttr):
    _name = 'Clock Setup'
    _readMask = [0x1f, 0xff, 0xff, 0xfd]

    def __init__(self, oaiface : OpenADCInterface, hwinfo=None, is_husky=False):
        super().__init__()
        self.oa = oaiface
        self._hwinfo = hwinfo
        self._freqExt = 10e6
        self._cached_adc_freq = None
        self.disable_newattr()

    def _dict_repr(self):
        rtn = OrderedDict()
        rtn['adc_src']    = self.adc_src
        rtn['adc_phase']  = self.adc_phase
        rtn['adc_freq']   = self.adc_freq
        rtn['adc_rate']   = self.adc_rate
        rtn['adc_locked'] = self.adc_locked

        rtn['freq_ctr']     = self.freq_ctr
        rtn['freq_ctr_src'] = self.freq_ctr_src

        rtn['clkgen_src']    = self.clkgen_src
        rtn['extclk_freq']   = self.extclk_freq
        rtn['clkgen_mul']    = self.clkgen_mul
        rtn['clkgen_div']    = self.clkgen_div
        rtn['clkgen_freq']   = self.clkgen_freq
        rtn['clkgen_locked'] = self.clkgen_locked

        return rtn

    def __repr__(self):
        return util.dict_to_str(self._dict_repr())

    def __str__(self):
        return self.__repr__()

    @property
    def adc_src(self) -> str:
        """The clock source for the ADC module.

        The ADC can be clocked by one of five possible sources:

         * "clkgen_x1": CLKGEN output via DCM
         * "clkgen_x4": CLKGEN output via DCM with x4 clk multiplier
         * "extclk_x1": External clock input via DCM
         * "extclk_x4": External clock input via DCM with x4 clk multiplier
         * "extclk_dir": External clock input with no DCM

        :Getter: Return the current ADC clock source (one of five strings above)

        :Setter: Set the ADC clock source and reset the ADC DCM to lock it.

        Raises:
           ValueError: if string not in valid settings
        """
        (adc_input, dcm_mul, dcm_input) = self._getAdcSource()
        if adc_input == "extclk":
            return "extclk_dir"
        else: # adc_input == "dcm"
            ret = "%s_x%d" % (dcm_input, dcm_mul)
            return ret

    @adc_src.setter
    def adc_src(self, src : str):
        # We need to pass a tuple into _setAdcSource() so the ADC source
        # parameter recognizes this input
        self._cached_adc_freq = None
        if src == "clkgen_x4":
            self._setAdcSource(("dcm", 4, "clkgen"))
        elif src == "clkgen_x1":
            self._setAdcSource(("dcm", 1, "clkgen"))
        elif src == "extclk_x4":
            self._setAdcSource(("dcm", 4, "extclk"))
        elif src == "extclk_x1":
            self._setAdcSource(("dcm", 1, "extclk"))
        elif src == "extclk_dir":
            self._setAdcSource(("extclk", 1, "extclk"))
        else:
            raise ValueError("Invalid ADC source (possible values: 'clkgen_x4', 'clkgen_x1', 'extclk_x4', 'extclk_x1', 'extclk_dir'")

        self.reset_adc()

    @property
    def adc_phase(self) -> int:
        """Fine adjustment for the ADC sampling point.

        This setting moves the sampling point approximately 5 ns forward or
        backward, regardless of the sampling frequency. It may be helpful to
        improve the stability of the measurement.

        The value of this setting is dimensionless and has a non-linear
        effect on the phase adjustment.

        :Getter: Return the current phase setting (integer)
            NOTE: This getter is currently broken due to an FPGA bug.

        :Setter: Set a new phase offset

        Raises:
           ValueError: if offset not in [-255, 255]
           TypeError: if offset not integer
        """
        return self._get_phase()

    @adc_phase.setter
    def adc_phase(self, phase : int):
        self._set_phase(phase)

    @property
    def adc_freq(self) -> int:
        """The current frequency of the ADC clock in Hz. Read-only.

        This clock frequency is derived from one of the ADC clock sources as
        described in adc_src.

        :Getter: Return the current frequency in Hz (int). May take
                up to 0.5s to stabilize after adc_locked is True.
        """
        return self._getAdcFrequency()

    @property
    def adc_rate(self) -> int:
        """The current sampling rate of the ADC clock in samples/s. Read-only.

        Note that the sampling rate may be less than the clock frequency if
        the downsampling factor is greater than 1.

        :Getter: Return the current sampling rate in samples/s (float)
        """
        return self._adcSampleRate()

    @property
    def adc_locked(self) -> bool:
        """The current status of the ADC DCM. Read-only.

        To try re-locking the ADC, see reset_adc().

        :Getter: Return whether the ADC DCM is locked (True or False)
        """
        return self._get_adcclk_locked()

    @property
    def freq_ctr(self) -> int:
        """The current frequency at the frequency counter in Hz. Read-only.

        The frequency counter can be used to check the speed of the CLKGEN
        output or the EXTCLK input. This value shows the current frequency
        reading.

        :Getter: Return the current frequency in Hz (int)
        """
        return self._get_extfrequency()

    @property
    def freq_ctr_src(self) -> str:
        """The current input to the frequency counter.

        There are two possible inputs to the frequency counter:
        - "clkgen": The CLKGEN DCM output
        - "extclk": The external input clock signal

        :Getter: Return the frequency counter input (one of the above strings)

        :Setter: Set the frequency counter source

        Raises:
           ValueError: if source is not "clkgen" or "extclk"
        """
        src = self._get_freqcounter_src()
        if src == 1:
            return "clkgen"
        elif src == 0:
            return "extclk"
        else:
            raise IOError("Invalid clock source reported by hardware: %d"%src)

    @freq_ctr_src.setter
    def freq_ctr_src(self, src : str):
        if src == "clkgen":
            s = 1
        elif src == "extclk":
            s = 0
        else:
            raise ValueError("Invalid clock source for frequency counter. Valid values: 'clkgen', 'extclk'.", src)
        self._set_freqcounter_src(s)

    @property
    def clkgen_src(self) -> str:
        """The input source for the CLKGEN DCM.

        This DCM can receive input from one of two places:

        - "extclk": The external clock input
        - "system" or "internal": The system clock (96 MHz)

        :Getter: Return the current CLKGEN input (either "extclk" or "system")

        :Setter: Change the CLKGEN source and reset all the DCMs.

        Raises:
           ValueError: if source is not one of three strings above
        """
        return self._get_clkgen_src()

    @clkgen_src.setter
    def clkgen_src(self, src : str):
        if src == "extclk":
            self._set_clkgen_src("extclk")
        elif src == "system" or src == "internal":
            self._set_clkgen_src("system")
        else:
            raise ValueError("Invalid setting for CLKGEN source (valid values: 'system', 'extclk')")

        self.reset_dcms()

    @property
    def extclk_freq(self) -> int:
        """The input frequency from the EXTCLK source in Hz.

        This value is used to help calculate the correct CLKGEN settings to
        obtain a desired output frequency when using EXTCLK as CLKGEN input.
        It is not a frequency counter - it is only helpful if the EXTCLK
        frequency is already known.

        :Getter: Return the last set EXTCLK frequency in MHz (int)

        :Setter: Update the EXTCLK frequency
        """
        return int(self._get_extclk_freq())

    @extclk_freq.setter
    def extclk_freq(self, freq : int):
        self._set_extclk_freq(freq)

    @property
    def clkgen_freq(self) -> str:
        """The CLKGEN output frequency in Hz.

        The CLKGEN module takes the input source and multiplies/divides it to
        get a faster or slower clock as desired. Minimum clock in practice
        is 3.2MHz.

        :Getter:
            Return the current calculated CLKGEN output frequency in Hz
            (float). Note that this is the theoretical frequency - use the
            freq counter to determine the actual output. May take up to 0.5s
            to stabilize after clkgen_locked is True.

        :Setter:
            Attempt to set a new CLKGEN frequency in Hz. When this value is
            set, all possible DCM multiply/divide settings are tested to find
            which is closest to the desired output speed. If EXTCLK is the
            CLKGEN source, the EXTCLK frequency must be properly set for this
            to work. Also, both DCMs are reset.
        """
        return self._get_clkgen_freq()

    @clkgen_freq.setter
    def clkgen_freq(self, freq):
        self._cached_adc_freq = None
        self._autoMulDiv(freq)
        self.reset_dcms()

    @property
    def clkgen_locked(self) -> bool:
        """The current status of the CLKGEN DCM. Read-only.

        :Getter: Return whether the CLKGEN DCM is locked (True or False)
        """
        return self._getClkgenLocked()

    def _set_freqcounter_src(self, src):
        result = self.oa.sendMessage(CODE_READ, "ADVCLOCK_ADDR", maxResp=4)
        result[3] &= ~0x08
        result[3] |= src << 3
        #print "%x"%result[3]
        self.oa.sendMessage(CODE_WRITE, "ADVCLOCK_ADDR", result, readMask=self._readMask)

    def _get_freqcounter_src(self):
        if self.oa is None:
            return 0
        result = self.oa.sendMessage(CODE_READ, "ADVCLOCK_ADDR", maxResp=4)
        return (result[3] & 0x08) >> 3

    #def _getClkgenStr(self):
    #    return str(self.getClkgen()) + " Hz"

    def _get_clkgen_freq(self):
        if self._get_clkgen_src() == "extclk":
            inpfreq = self._get_extclk_freq()
        else:
            inpfreq = self._hwinfo.sysFrequency()
        return (inpfreq * self._getClkgenMul()) / self._getClkgenDiv()

    def _autoMulDiv(self, freq):
        if freq < 3.2E6: #practical min limit of clkgen
            scope_logger.warning("Requested clock value below minimum of 3.2MHz - DCM may not lock!")
        if self._get_clkgen_src() == "extclk":
            inpfreq = self._get_extclk_freq()
        else:
            inpfreq = self._hwinfo.sysFrequency()
        if self.oa.hwInfo.is_cwhusky():
            sets = self._calculateHuskyClkGenMulDiv(freq, inpfreq)
        else:
            sets = self._calculateClkGenMulDiv(freq, inpfreq)
        self._setClkgenMulWrapper(sets[0])
        self._setClkgenDivWrapper(sets[1:])
        self._reset_dcms(False, True)


    def _calculateClkGenMulDiv(self, freq, inpfreq=30E6):
        """Calculate Multiply & Divide settings based on input frequency"""

        #Max setting for divide is 60 (see datasheet)
        #Multiply is 2-256

        lowerror = 1E99
        best = (0, 0)

        # From datasheet, if input freq is < 52MHz limit max divide
        if inpfreq < 52E6:
            maxdiv = int(inpfreq / 0.5E6)
        else:
            maxdiv = 256

        for mul in range(2, 257):
            for div in range(1, maxdiv):

                err = abs(freq - ((inpfreq * mul) / div))
                if err < lowerror:
                    lowerror = err
                    best = (mul, div)

        return best


    def _calculateHuskyClkGenMulDiv(self, freq, inpfreq=96e6, vcomin=600e6, vcomax=1200e6):
        """Calculate Multiply & Divide settings based on input frequency"""
        lowerror = 1e99
        best = (0,0,0)
        for maindiv in range(1,6):
            mmin = int(np.ceil(vcomin/inpfreq*maindiv))
            mmax = int(np.ceil(vcomax/inpfreq*maindiv))
            for mul in range(mmin,mmax+1):
                if mul/maindiv < vcomin/inpfreq or mul/maindiv > vcomax/inpfreq:
                    continue
                for secdiv in range(1,127):
                    calcfreq = inpfreq*mul/maindiv/secdiv
                    err = abs(freq - calcfreq)
                    if err < lowerror:
                        lowerror = err
                        best = (mul, maindiv, secdiv)
        if best == (0,0,0):
            raise ValueError("Couldn't find a legal div/mul combination")
        return best


    @property
    def clkgen_mul(self) -> int:
        """The multiplier in the CLKGEN DCM.

        This multiplier must be in the range [2, 256].

        :Getter: Return the current CLKGEN multiplier (integer)

        :Setter: Set a new CLKGEN multiplier.
        """
        return self._getClkgenMul()

    @clkgen_mul.setter
    def clkgen_mul(self, mul : int):
        self._setClkgenMulWrapper(mul)

    def _getClkgenMul(self):
        timeout = 2
        while timeout > 0:
            result = self.oa.sendMessage(CODE_READ, "ADVCLOCK_ADDR", maxResp=4)
            val = result[1]
            if val == 0:
                val = 1  # Fix incorrect initialization on FPGA
                self._setClkgenMul(2)
            val += 1

            if (result[3] & 0x02):
                return val

            self._clkgenLoad()

            timeout -= 1

        # raise IOError("clkgen never loaded value?")
        return 0


    def _setClkgenMulWrapper(self, mul):
        # TODO: raise ValueError?
        if mul < 2:
            mul = 2
        self._setClkgenMul(mul)

    def _setClkgenMul(self, mul):
        result = self.oa.sendMessage(CODE_READ, "ADVCLOCK_ADDR", maxResp=4)
        mul -= 1
        result[1] = mul
        result[3] |= 0x01
        self.oa.sendMessage(CODE_WRITE, "ADVCLOCK_ADDR", result, readMask=self._readMask)
        result[3] &= ~(0x01)
        self.oa.sendMessage(CODE_WRITE, "ADVCLOCK_ADDR", result, readMask=self._readMask)


    @property
    def clkgen_div(self) -> int:
        """The divider in the CLKGEN DCM.

        This divider must be in the range [1, 256].

        :Getter: Return the current CLKGEN divider (integer)

        :Setter: Set a new CLKGEN divider.
        """
        return self._getClkgenDiv()

    @clkgen_div.setter
    def clkgen_div(self, div : Union[int, List[int]]):
        self._setClkgenDivWrapper(div)

    def _getClkgenDiv(self):
        if self.oa is None:
            return 2
        timeout = 2
        while timeout > 0:
            result = self.oa.sendMessage(CODE_READ, "ADVCLOCK_ADDR", maxResp=4)
            if (result[3] & 0x02):
                # Done loading value yet
                val = result[2]
                val += 1
                return val

            self._clkgenLoad()

            timeout -= 1

        scope_logger.error("CLKGEN Failed to load divider value. Most likely clock input to CLKGEN is stopped, check CLKGEN"
                      " source settings. CLKGEN clock results are currently invalid.")
        return 1


    def _setClkgenDivWrapper(self, div):
        if hasattr(div, "__getitem__"):
            div = div[0]
        if div < 1:
            div = 1

        result = self.oa.sendMessage(CODE_READ, "ADVCLOCK_ADDR", maxResp=4)
        div -= 1
        result[2] = div
        result[3] |= 0x01
        self.oa.sendMessage(CODE_WRITE, "ADVCLOCK_ADDR", result, readMask=self._readMask)
        result[3] &= ~(0x01)
        self.oa.sendMessage(CODE_WRITE, "ADVCLOCK_ADDR", result, readMask=self._readMask)


    def reset_adc(self) -> None:
        """Reset the ADC DCM.

        After changing frequencies, the ADC DCM may become unlocked from its
        input signal. This function resets the DCM to re-lock it.

        If the DCM is still unlocked after calling this function, the clock
        may be too fast for the ADC.
        """
        self._reset_dcms(True, False)

    resetAdc = util.camel_case_deprecated(reset_adc)

    def reset_clkgen(self) -> None:
        """Reset the CLKGEN DCM.

        After changing frequencies or input sources, the CLKGEN DCM may not
        be locked. This function resets the DCM to re-lock it.

        If the DCM is still unlocked after calling this function, the clock
        may be too fast for the CLKGEN module.
        """
        self._reset_dcms(False, True)

    resetClkgen = util.camel_case_deprecated(reset_clkgen)

    def reset_dcms(self) -> None:
        """Reset the CLKGEN DCM, then the ADC DCM.

        This order is necessary because the ADC may depend on having a locked
        clock from the CLKGEN output.
        """
        self.reset_clkgen()
        self.reset_adc()

    resetDcms = util.camel_case_deprecated(reset_dcms)

    def _clkgenLoad(self):
        result = self.oa.sendMessage(CODE_READ, "ADVCLOCK_ADDR", maxResp=4)
        result[3] |= 0x01
        self.oa.sendMessage(CODE_WRITE, "ADVCLOCK_ADDR", result, readMask=self._readMask)
        result[3] &= ~(0x01)
        self.oa.sendMessage(CODE_WRITE, "ADVCLOCK_ADDR", result, readMask=self._readMask)


    def _setEnabled(self, enable):
        if enable:
            val = [0]
        else:
            val = [3]
        self.oa.sendMessage(CODE_WRITE, "CLKGEN_POWERDOWN", val, Validate=False)

    def _getEnabled(self):
        raw = self.oa.sendMessage(CODE_READ, "CLKGEN_POWERDOWN", Validate=False, maxResp=1)[0]
        if raw == 3:
            return False
        elif raw == 0:
            return True
        else:
            raise ValueError("Unexpected: read %d" % raw)


    def _getAdcSource(self):
        if self.oa is None:
            return ("dcm", 1, "extclk")

        result = self.oa.sendMessage(CODE_READ, "ADVCLOCK_ADDR", maxResp=4)
        result[0] = result[0] & 0x07

        if result[0] & 0x04:
            dcminput = "extclk"
        else:
            dcminput = "clkgen"

        if result[0] & 0x02:
            dcmout = 1
        else:
            dcmout = 4

        if result[0] & 0x01:
            source = "extclk"
        else:
            source = "dcm"

        return (source, dcmout, dcminput)

    def _setAdcSource(self, source="dcm", dcmout=4, dcminput="clkgen"):

        #Deal with being passed tuple with all 3 arguments
        if isinstance(source, (list, tuple)):
            dcminput = source[2]
            dcmout = source[1]
            source=source[0]

        result = self.oa.sendMessage(CODE_READ, "ADVCLOCK_ADDR", maxResp=4)

        result[0] = result[0] & ~0x07

        if dcminput == "clkgen":
            pass
        elif dcminput == "extclk":
            result[0] = result[0] | 0x04
        else:
            raise ValueError("dcminput must be 'clkgen' or 'extclk'")

        if dcmout == 4:
            pass
        elif dcmout == 1:
            result[0] = result[0] | 0x02
        else:
            raise ValueError("dcmout must be 1 or 4")

        if source == "dcm":
            pass
        elif source == "extclk":
            result[0] = result[0] | 0x01
        else:
            raise ValueError("source must be 'dcm' or 'extclk'")

        self.oa.sendMessage(CODE_WRITE, "ADVCLOCK_ADDR", result, readMask=self._readMask)

    def _set_clkgen_src(self, source="system"):
        result = self.oa.sendMessage(CODE_READ, "ADVCLOCK_ADDR", maxResp=4)

        result[0] = result[0] & ~0x08

        if source == "system":
            pass
        elif source == "extclk":
            result[0] = result[0] | 0x08
        else:
            raise ValueError("source must be 'system' or 'extclk'")

        self.oa.sendMessage(CODE_WRITE, "ADVCLOCK_ADDR", result, readMask=self._readMask)

    def _get_clkgen_src(self):
        if self.oa is not None and self.oa.sendMessage(CODE_READ, "ADVCLOCK_ADDR", maxResp=4)[0] & 0x08:
            return "extclk"
        else:
            return "system"

    def _set_extclk_freq(self, freq):
        self._freqExt = freq

    def _get_extclk_freq(self):
        return self._freqExt

    def _set_phase(self, phase):
        '''Set the phase adjust, range -255 to 255'''
        try:
            phase_int = int(phase)
        except ValueError as e:
            raise TypeError("Can't convert %s to int" % phase) from e

        if phase_int < -255 or phase_int > 255:
            raise ValueError("Phase %d is outside range [-255, 255]" % phase_int)

        cmd = bytearray(2)
        cmd[0] = phase_int & 0x00FF
        MSB = (phase_int & 0x0100) >> 8
        cmd[1] = MSB | 0x02 # TODO: hmm why is this being done?

        self.oa.sendMessage(CODE_WRITE, "PHASE_ADDR", cmd, False)

    def _get_phase(self):
        if self.oa is None:
            return 0
        result = self.oa.sendMessage(CODE_READ, "PHASE_ADDR", maxResp=2)

        #Current bitstream doesn't set this bit ever?
        #phase_valid = (result[1] & 0x02)
        #Temp fix - set as true always
        phase_valid = True

        if phase_valid:
            LSB = result[0]
            MSB = result[1] & 0x01

            phase = LSB | (MSB << 8)

            #Sign Extend
            phase = SIGNEXT(phase, 9)

            return phase
        else:
            scope_logger.warning("No phase shift loaded")
            return 0

    def _get_adcclk_locked(self):
        result = self._DCMStatus()
        return result[0]

    def _getClkgenLocked(self):
        result = self._DCMStatus()
        return result[1]

    def _DCMStatus(self):
        if self.oa is None:
            return (False, False)

        result = self.oa.sendMessage(CODE_READ, "ADVCLOCK_ADDR", maxResp=4)
        if (result[0] & 0x80) == 0:
            scope_logger.error("ADVCLK register not present. Version mismatch")
            return (False, False)

        if (result[0] & 0x40) == 0:
            dcmADCLocked = False
        else:
            dcmADCLocked = True

        if (result[0] & 0x20) == 0:
            dcmCLKGENLocked = False
        else:
            dcmCLKGENLocked = True

        #if (result[3] & 0x02):
        #    print "CLKGEN Programming Done"

        return (dcmADCLocked, dcmCLKGENLocked)

    def _reset_dcms(self, resetAdc=True, resetClkgen=True, resetGlitch=True):
        result = self.oa.sendMessage(CODE_READ, "ADVCLOCK_ADDR", maxResp=4)

        #Set reset high on requested blocks only
        if resetAdc:
            result[0] = result[0] | 0x10
            #NB: High-Level system will call 'get' to re-read ADC phase

        if resetClkgen:
            result[3] = result[3] | 0x04


        self.oa.sendMessage(CODE_WRITE, "ADVCLOCK_ADDR", result, Validate=False)

        #Set reset low
        result[0] = result[0] & ~(0x10)
        result[3] = result[3] & ~(0x04)
        self.oa.sendMessage(CODE_WRITE, "ADVCLOCK_ADDR", result, Validate=False)

        #Load clkgen if required
        if resetClkgen:
            self._clkgenLoad()

        if resetGlitch:
            reset = self.oa.sendMessage(CODE_READ, "CLOCKGLITCH_SETTINGS", Validate=False, maxResp=8)
            reset[5] |= (1<<1)
            self.oa.sendMessage(CODE_WRITE, "CLOCKGLITCH_SETTINGS", reset, Validate=False)
            reset[5] &= ~(1<<1)
            self.oa.sendMessage(CODE_WRITE, "CLOCKGLITCH_SETTINGS", reset, Validate=False)

    def _get_extfrequency(self):
        """Return frequency of clock measured on EXTCLOCK pin in Hz"""
        if self.oa is None:
            return 0

        #Get sample frequency
        samplefreq = float(self.oa.hwInfo.sysFrequency()) / float(pow(2,23))

        temp = self.oa.sendMessage(CODE_READ, "EXTFREQ_ADDR", maxResp=4)
        freq = int.from_bytes(temp, byteorder='little')

        measured = freq * samplefreq
        return int(measured)

    def _getAdcFrequency(self):
        """Return the external frequency measured on 'CLOCK' pin. Returned value
           is in Hz"""
        if self.oa is None:
            return 0

        if self._cached_adc_freq is None:
            #Get sample frequency
            samplefreq = float(self.oa.hwInfo.sysFrequency()) / float(pow(2,23))

            temp = self.oa.sendMessage(CODE_READ, "ADCFREQ_ADDR", maxResp=4)
            freq = int.from_bytes(temp, byteorder='little')

            self._cached_adc_freq = int(freq * samplefreq)

        return self._cached_adc_freq

    def _adcSampleRate(self):
        """Return the sample rate, takes account of decimation factor (if set)"""
        return self._getAdcFrequency() / self.oa.decimate()

    @property
    def adc_mul(self) -> int:
        """ Sets a new ADC clock frequency by multiplying this value by clkgen_freq

        On Husky, must be a positive integer, or 0. If 0, turns the ADC clock off.

        On Lite/Pro, must be either 1 or 4

        adc_freq = adc_mul * clkgen_freq

        Note that the value of adc_mul affects how closely clkgen_freq can be matched
        to the requested frequency. See clkgen_freq for more information.

        :Getter: The currently set adc multiplier

        :Setter: Set the adc multiplier

        .. versionchanged:: 6.0
            Added ChipWhisperer-Lite/Pro version of this property
        """
        x4 = ["clkgen_x4", "extclk_x4"]
        x1 = ["clkgen_x1", "extclk_x1", "extclk_dir"]
        if self.adc_src in x4:
            return 4
        elif self.adc_src in x1:
            return 1
        else:
            raise ValueError("Unknown adc_src {}".format(self.adc_src))

    @adc_mul.setter
    def adc_mul(self, mul : int):
        x4 = ["clkgen_x4", "extclk_x4"]
        x1 = ["clkgen_x1", "extclk_x1", "extclk_dir"]
        if self.adc_src in x4:
            return 4
        elif self.adc_src in x1:
            return 1
        else:
            raise ValueError("Unknown adc_src {}".format(self.adc_src))

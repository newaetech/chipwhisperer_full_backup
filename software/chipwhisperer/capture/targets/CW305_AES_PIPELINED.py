#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2015-2020, NewAE Technology Inc
# All rights reserved.
#
# Find this and more at newae.com - this file is part of the chipwhisperer
# project, http://www.chipwhisperer.com . ChipWhisperer is a registered
# trademark of NewAE Technology Inc in the US & Europe.
#
#    This file is part of chipwhisperer.
#
#    Licensed under the Apache License, Version 2.0 (the "License");
#    you may not use this file except in compliance with the License.
#    You may obtain a copy of the License at
#
#       http://www.apache.org/licenses/LICENSE-2.0
#
#    Unless required by applicable law or agreed to in writing, software
#    distributed under the License is distributed on an "AS IS" BASIS,
#    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#    See the License for the specific language governing permissions and
#    limitations under the License.
#=================================================
import logging
import time
import re
import os.path
import random
from ...common.traces import Trace
from ...common.utils import util
from .CW305 import CW305, CW305_USB
from chipwhisperer.logging import *
from chipwhisperer.analyzer.utils.aes_funcs import key_schedule_rounds
from chipwhisperer.common.utils.aes_cipher import AESCipher
from collections import OrderedDict
bytearray = util.CWByteArray # type: ignore

class CW305_AES_PIPELINED(CW305):

    """CW305 target object for pipelined AES target.

    This class contains the public API for the CW305 hardware.
    To connect to the CW305, the easiest method is::

        import chipwhisperer as cw
        scope = cw.scope()
        target = cw.target(scope, cw.targets.CW305_AES_PIPELINED,
                           bsfile=<valid FPGA bitstream file>)

    Note that connecting to the CW305_AES_PIPELINED includes programming the CW305 FPGA.
    For more help about CW305 settings, try help() on this CW305 submodule:

       * target.pll
    """


    _name = "ChipWhisperer CW305 (Artix-7)"


    def __init__(self):
        import chipwhisperer as cw
        super().__init__()
        #self._clksleeptime = 150 # need lots of idling time
        # Verilog defines file(s):
        self.default_verilog_defines = 'cw305_aes_pipelined_defines.v'
        self.default_verilog_defines_full_path = os.path.dirname(cw.__file__) +  '/../../hardware/victims/cw305_artixtarget/fpga/vivado_examples/aes128_pipelined/hdl/' + self.default_verilog_defines
        self.registers = 18 # number of registers we expect to find
        self.bytecount_size = 7 # pBYTECNT_SIZE in Verilog
        self.check_done = True
        self.target_name = 'Pipelined AES'


    def capture_trace(self, scope, plaintexts, key, pre_expand=False, check=True):
        """Capture a trace, similar to scope.capture_trace() but allows plaintexts
        to be a list of plaintexts.

        Args:
            scope (ScopeTemplate): Scope object to use for capture.
            plaintexts (bytearray or list of bytearrays): Plaintext(s) to send to the target.
            key (bytearray): Key to send to target.
            pre_expand (bool): whether or not the target expands the key prior to encrypting.
            check (bool): whether or not the received ciphertexts are verified for correctness.

        Returns:
            :class:`Trace <chipwhisperer.common.traces.Trace>` or None if capture
            timed out.

        Raises:
            Warning or OSError: Error during capture.

        """
        if type(plaintexts[0]) is int or len(plaintexts) == 1:
            plaintexts = [plaintexts]
            single_pt = True
        else:
            single_pt = False
        if len(plaintexts) > self.fifo_depth:
            target_logger.error("cannot go beyond the FIFO depth (%d)" % self.fifo_depth)

        # set the key:
        self.fpga_write(self.REG_CRYPT_KEY, key[::-1])
        if pre_expand:
            self.fpga_write(self.REG_CRYPT_PRE_EXPAND_KEY, [1])
            self.fpga_write(self.REG_KEY_GO, [1])
        else:
            self.fpga_write(self.REG_CRYPT_PRE_EXPAND_KEY, [0])

        # write the plaintexts:
        for pt in plaintexts:
            self.fpga_write(self.REG_CRYPT_TEXTIN, pt[::-1])
            self.fpga_write(self.REG_CRYPT_WRITE_PT, [1])

        scope.arm()
        self.go()
        ret = scope.capture()

        # if check, retrieve results: otherwise, assume they're correct and build ciphertexts[] from what they should be:
        ciphertexts = []
        exp_key = list(key)
        for i in range(1,11):
            exp_key.extend(key_schedule_rounds(list(key), 0, i))
        cipher = AESCipher(exp_key)

        if check:
            for i in range(len(plaintexts)):
                self.fpga_write(self.REG_CRYPT_CIPHEROUT, [1])
                ciphertexts.append(bytearray(self.fpga_read(self.REG_CRYPT_CIPHEROUT, 16)[::-1]))
        else:
            #self.fifo_flush() # this is slow because it's one read and one write per entry
            for pt in plaintexts:
                self.fpga_write(self.REG_CRYPT_CIPHEROUT, [1])
                ciphertexts.append(bytearray(cipher.cipher_block(list(pt))))


        fifo_errors = self.fifo_errors()
        if fifo_errors:
            target_logger.errors("Internal FIFO errors:" % fifo_errors)

        if ret:
            target_logger.warning("Timeout happened during capture")
            return None

        wave = scope.get_last_trace()

        if check:
            for pt,ct in zip(plaintexts, ciphertexts):
                expected = bytearray(cipher.cipher_block(list(pt)))
                if expected != ct:
                    target_logger.error("Bad encryption result: expected %s, got %s" % (expected, ct))

        if len(wave) >= 1:
            if single_pt:
                # if we have a single pt, return a "normal" Trace object (which can e.g. be directly fed to analyzer).
                return Trace(wave, plaintexts[0], ciphertexts[0], key)
            else:
                # otherwise, return a Trace where textin is a list of all the plaintexts and textout is a list of all
                # the ciphertexts; you will probably want to chop this up into trace segments that each have a single
                # associated plaintext and ciphertext
                return Trace(wave, plaintexts, ciphertexts, key)
        else:
            return None


    def _dict_repr(self):
        rtn = OrderedDict()
        rtn['target_name']      = self.target_name
        rtn['fpga_buildtime']   = self.fpga_buildtime
        rtn['core_type']        = self.core_type
        rtn['fifo_depth']       = self.fifo_depth
        rtn['half_pipe']        = self.half_pipe
        rtn['crypt_type']       = self.crypt_type
        rtn['crypt_rev']        = self.crypt_rev
        rtn['platform']         = self.platform
        for prop in self.__dir__():
            if 'REG_' in prop:
                if getattr(self, prop): # this some stock registers are delcared as None and may remain so
                    rtn[prop] = getattr(self, prop)
        return rtn

    def __repr__(self):
        return util.dict_to_str(self._dict_repr())

    def __str__(self):
        return self.__repr__()


    def fifo_empty(self):
        """ Returns 1 if the output FIFO is empty, 0 otherwise.
        """
        return self.fpga_read(self.REG_CRYPT_GO, 1)[0]

    def fifo_flush(self):
        """ Read from output FIFO until it is empty.
        """
        while not self.fifo_empty():
            self.fpga_write(self.REG_CRYPT_CIPHEROUT, [1])

    @property
    def fifo_depth(self):
        """ Returns the depth of the target's internal FIFO, which corresponds to the maximum
        number of AES blocks that can be encrypted in one go.
        """
        return int.from_bytes(self.fpga_read(self.REG_FIFO_DEPTH, 4), byteorder='little')

    def fifo_errors(self):
        raw = self.fpga_read(self.REG_FIFO_ERRORS, 1)[0]
        errors = ''
        if raw & 2**0: errors += 'output overflow, '
        if raw & 2**1: errors += 'output underflow, '
        if raw & 2**2: errors += 'input underflow, '
        if raw & 2**3: errors += 'input overflow, '
        if errors == '':
            errors = None
        return errors
        assert fifo_errors == 0, "Internal FIFO errors! (%d)" % fifo_errors

    def clear_fifo_errors(self):
        self.fpga_write(self.REG_FIFO_ERRORS, [1])
        self.fpga_write(self.REG_FIFO_ERRORS, [0])

    @property
    def half_pipe(self):
        """ Returns the value of the HALF_PIPE parameter used at buildtime.
        To help ensure that you're using the implementation that you think you are!
        """
        return self.fpga_read(self.REG_HALF_PIPE, 1)[0]


    def split_traces(self, scope, traces, start_offset, stop_offset, initial_discards=10, end_discards=10):
        """Convenience function which takes one or more power traces and splits them up into many smaller
        traces which are returned. The starting index of each subtrace increases by scope.clock.adc_mul for
        fully-pipelined targets, and twice that for half-pipelined targets.

        Args:
            scope (ScopeTemplate): Scope object (used to learn scope.clock.adc_mul)
            traces (list of Trace):
            start_offset (int): starting index for the first sub-trace
            stop_offset (int): stopping index for the first sub-trace
            initial_discards (int): number of sub-traces to discard from the start of each element of traces
            end_discards (int): number of sub-traces to discard from the end of each element of traces

        Returns:
            List of :class:`Trace <chipwhisperer.common.traces.Trace>`

        """
        N = len(traces)
        NPT = len(traces[0].textin) # number of encryptions per trace
        samples_per_clock = scope.clock.adc_mul
        if self.half_pipe:
            mx = 2
        else:
            mx = 1
        new_traces = []
        for j in range(N):
            for i in range(initial_discards, NPT-end_discards):
                short_trace = traces[j].wave[start_offset+i*samples_per_clock*mx:stop_offset+i*samples_per_clock*mx]
                #short_traces.append(short_trace)
                trace_object = Trace(short_trace, traces[j].textin[i], traces[j].textout[i], traces[0].key)
                new_traces.append(trace_object)
        return new_traces



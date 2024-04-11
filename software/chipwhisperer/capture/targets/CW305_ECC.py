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
from ecpy.curves import Curve, Point # type: ignore
from collections import OrderedDict

from chipwhisperer.logging import *

class CW305_ECC(CW305):

    """CW305 target object for ECC targets.

    This class contains the public API for the CW305 hardware.
    To connect to the CW305, the easiest method is::

        import chipwhisperer as cw
        scope = cw.scope()
        target = cw.target(scope, cw.targets.CW305_ECC,
                           bsfile=<valid FPGA bitstream file>)

    Note that connecting to the CW305_ECC includes programming the CW305 FPGA.
    For more help about CW305 settings, try help() on this CW305 submodule:

       * target.pll
    """


    _name = "ChipWhisperer CW305 (Artix-7)"


    def __init__(self):
        import chipwhisperer as cw
        self.REG_CRYPT_GX = 0
        self.REG_CRYPT_GY = 0
        self.REG_CRYPT_K = 0
        self.REG_CRYPT_RX = 0
        self.REG_CRYPT_RY = 0

        super().__init__()
        self._clksleeptime = 150 # need lots of idling time
        self.curve = Curve.get_curve('NIST-P256')
        self.pmul_cycles = 1124157
        # Verilog defines file(s):
        self.default_verilog_defines = 'cw305_pmul_defines.v'
        self.default_verilog_defines_full_path = os.path.dirname(cw.__file__) +  '/../../firmware/fpgas/ecc/hdl/' + self.default_verilog_defines
        self.registers = 20 # number of registers we expect to find
        self.bytecount_size = 8 # pBYTECNT_SIZE in Verilog
        self.check_done = True
        self.target_name = 'Cryptech ecdsa256-v1 pmul'


    def _dict_repr(self):
        rtn = OrderedDict()
        rtn['target_name']      = self.target_name
        rtn['fpga_buildtime']   = self.fpga_buildtime
        rtn['core_type']        = self.core_type
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


    def capture_trace(self, scope, k, operation="pmult", Px=None, Py=None, check=True, as_int=False):
        """Capture a trace, running the specified test vector or operation (pmult or siggen).

        Does all individual steps needed to capture a trace (arming the scope,
        running the test vector or operation, getting the trace data back, etc.)

        Args:
            scope (ScopeTemplate): Scope object to use for capture.
            k (int): multiplier for pmult
            operation (string, optional): Operation to run.
                'pmult': run a point multiplication. Requires Px, Py, and k be supplied.
            check: if set, verify the result (using ecpy)

            Px (int, optional): X coordinate of curve point for pmult. Generator point is used if not given.
            Py (int, optional): Y coordinate of curve point for pmult. Generator point is used if not given.

        Returns:
            :class:`Trace <chipwhisperer.common.traces.Trace>` or None if capture
            timed out.

        Raises:
            Warning or OSError: Error during capture.

        """
        scope.arm()
        if scope._is_husky:
            start_cycles = 0
        else:
            start_cycles = scope.adc.trig_count

        if operation == 'pmult':
            textout = self.run_pmult(k, Px, Py, check=check, verbose=False)
        else:
            target_logger.error("Please supply a valid operation to run.")

        ret = scope.capture()
        cycles = scope.adc.trig_count - start_cycles

        if ret:
            target_logger.warning("Timeout happened during capture")
            return None

        textin = {'operation': operation,
                  'Px': Px,
                  'Py': Py,
                  'k': k
                  }
        textout['cycles'] = cycles
        wave = scope.get_last_trace(as_int=as_int)

        if check and cycles != self.pmul_cycles:
            target_logger.warning ("Operation took %d cycles (%d more than we expect it to)" % (cycles, cycles-self.pmul_cycles))

        if len(wave) >= 1:
            return Trace(wave, textin, textout, None)
        else:
            return None


    def run_pmult(self, k, Px=None, Py=None, check=True, verbose=False):
        """Run an arbitrary pmult.
        Args:
            Px (int): X coordinate of curve point
            Py (int): Y coordinate of curve point
            k (int): multiplier
            check: if set, verify the result (using ecpy)

        """
        if Px == None:
            Px = self.curve.generator.x
            Py = self.curve.generator.y

        self.Px = Px
        self.Py = Py
        self.k = k

        self.fpga_write(self.REG_CRYPT_GX, list(int.to_bytes(Px, length=32, byteorder='little')))
        self.fpga_write(self.REG_CRYPT_GY, list(int.to_bytes(Py, length=32, byteorder='little')))

        self.fpga_write(self.REG_CRYPT_K, list(int.to_bytes(k, length=32, byteorder='little')))
        self.go()

        if not self.is_done():
            if self.platform == 'cw305' and self.clkusbautooff:
                    target_logger.warning ("Target not done yet, increase clksleeptime!")
            #let's wait a bit more, see what happens:
            i = 0
            while not self.is_done():
                i += 1
                time.sleep(0.05)
                if i > 100:
                    target_logger.warning("Target still did not finish operation!")
                    break

        Rx = int.from_bytes(self.fpga_read(self.REG_CRYPT_RX, 32), byteorder='little')
        Ry = int.from_bytes(self.fpga_read(self.REG_CRYPT_RY, 32), byteorder='little')


        # optionally check result:
        if check:
            P = Point(Px, Py, self.curve)
            Q = k*P
            if verbose:
                print("Expecting Qx = %32x" % Q.x)
                print("Expecting Qy = %32x" % Q.y)
            if Q.x != Rx:
                target_logger.error("Bad Rx!")
                target_logger.error("expected %32x" % Q.x)
                target_logger.error("got      %32x" % Rx)
            if Q.y != Ry:
                target_logger.error("Bad Ry!")
                target_logger.error("expected %32x" % Q.y)
                target_logger.error("got      %32x" % Ry)
        return {'Rx': Rx, 'Ry': Ry}


    def new_point(self, tries=100, bits=256):
        for i in range(tries):
            x = random.getrandbits(bits)
            y = self.curve.y_recover(x)
            if x > 0 and y:
                P = Point(x, y, self.curve, check=True)
                # shouldn't be necessary but let's check anwyway:
                assert self.curve.is_on_curve(P)
                return P
        raise ValueError("Failed to generate a random point after %d tries!" % tries)

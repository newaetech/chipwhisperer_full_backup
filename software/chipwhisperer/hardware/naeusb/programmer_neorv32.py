#
# Copyright (c) 2022, NewAE Technology Inc
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
#==========================================================================

import logging
import os
import struct
import time
import traceback
from datetime import datetime
from chipwhisperer.hardware.naeusb.programmer_targetfpga import LatticeICE40
from functools import reduce, wraps
from chipwhisperer.logging import *
import numpy as np
from ...capture.utils.IntelHex import IntelHex

import warnings
def gen_app_binary(rom):
    """Replicate image_gen.c from the neorv32 project
    """

    #
    with warnings.catch_warnings():
        warnings.simplefilter("ignore")
        signature = 0x4788CAFE
        if not isinstance(rom, bytes):
            rom = rom.read()

        checksum = np.uint32(0)
        size = 0
        for i in range(0, len(rom), 4):
            tmp = rom[i]
            tmp |= (rom[i+1] << 8)
            tmp |= (rom[i+2] << 16)
            tmp |= (rom[i+3] << 24)
            checksum += np.uint32(tmp)
            # checksum &= 0xFFFFFFFF
            size += 4

        checksum = (~checksum) + 1
        new_rom = bytes([(signature >> (i*8)) & 0xFF for i in range(4)])
        new_rom += bytes([(size >> (i*8)) & 0xFF for i in range(4)])
        new_rom += bytes([(checksum >> (i*8)) & 0xFF for i in range(4)])
        new_rom += rom
        return new_rom
     


def print_fun(s):
    print(s)

def close_on_fail(func):
    @wraps(func)
    def func_wrapper(self, *args, **kwargs):
        try:
            return func(self, *args, **kwargs)
        except:
            self.close_port()
            raise
    return func_wrapper

class Neorv32Programmer:
    """
    Class for programming a Neorv32 RISC-V processor, including FPGA loading of ChipWhisperer targets
    """

    def __init__(self, scope, timeout=200):
        """
        Set the communications instance.
        """

        self.scope = scope
        scope._getNAEUSB().check_feature("TARGET_SPI", raise_exception=True)
        if hasattr(scope, 'usart'):
            self._cwserial = scope.usart
        else:
            self._cwserial = scope.scopetype.usart
        self._timeout = timeout
        self._old_baud = None

    def load_ice40(self, bsfile=None, force_ice40type=None):
        """
        Load iCE40 Target with Bitstream, if `bsfile` is None uses default bitstream.
        """

        fpga = LatticeICE40(self.scope)
        fpga.erase_and_init()

        if bsfile is None:
            target_logger.info("ice40: Loading prebuilt Neorv32 core.")

            if force_ice40type:
                target_logger.info("ice40: Assuming device type is: %s"%force_ice40type)
            else:
                devtype = fpga.check_idcode()

                target_logger.info("ice40: Got idcode of %x (device = %s)"%(devtype[1], devtype[0]))

                # Just get string of name
                devtype = devtype[0].lower()
            
            if devtype == "ice40up5k":
                from chipwhisperer.hardware.firmware.cwtargetice40 import getsome
            else:
                raise ValueError("Device type: %s. I don't have a pre-built FPGA bitstream for you (or connection bad)."%devtype)
            
            bsdata = getsome("neorv32_iCE40CW312_MinimalBoot_directclk_7370KHz.bit").read()
        else:
            target_logger.info("ice40: Loading bitstream from %s"%bsfile)
            f = open(bsfile, "rb")
            bsdata = f.read()
            f.close()
        
        # TODO - currently this uses the 'slower' SPI programming mode as the 'fast' mode only works when passed a file. The fast
        # mode could be made to work fairly easily as getsome() returns a file-like object.
        fpga.program(bsdata)

        target_logger.info("iCE40 DONE pin not supported. Maybe it worked!?")

    def open_port(self, baud=19200):

        if self._cwserial:
            self._old_baud = self._cwserial._baud
            self._old_parity  = self._cwserial._parity
            self._old_stopbits = self._cwserial._stopbits
            self._old_timeout = self._cwserial.timeout
            self._cwserial.init(baud)
            self._cwserial.timeout = self._timeout

            self.sp = self._cwserial
        else:
            raise NotImplementedError("CW-Serial is only supported method (for now)")

    def close_port(self):
        if self._old_baud:
            self._cwserial.init(baud=self._old_baud, stopbits=self._old_stopbits, parity=self._old_parity)
            self._cwserial.timeout = self._old_timeout
        self._old_baud = None
        self._old_parity = None
        self._old_stopbits = None

        if self.scope:
            self.scope.io.nrst = 'default'

        if hasattr(self.sp, "close"):
            self.sp.close()

    @close_on_fail
    def find(self):
        return

    def do_cmd(self, cmd, expected, flush_first=True, flush_wait=0.05, cmd_wait=0.05, resp_read_len=None):
        """Run command on bootloader, check for response matching expected one"""

        if flush_first:
            time.sleep(flush_wait)
            self.sp.read()
        
        cmd = bytes(cmd, "ascii")

        self.sp.write(cmd)

        time.sleep(cmd_wait)

        if resp_read_len:
            resp = bytes(self.sp.read(resp_read_len))
        else:
            resp = bytes(self.sp.read())

        target_logger.debug(b"Response to '" + cmd + b"': " + resp)

        if bytes(expected, "ascii") not in resp:
            raise IOError(b"Sync error with bootloader - invalid response to '" + cmd + b"' command (%b)"%resp)
        
        return resp.decode('ascii')

    @close_on_fail
    def program(self, romfilename, bsfile=None, check_rom_size=True):
        """Programs memory type, dealing with opening filename as either .hex or .bin file"""
        self.lastFlashedFile = romfilename

        if romfilename.endswith(".hex"):
            f = IntelHex(romfilename)
            romdata = f.tobinarray(start=f.minaddr())
        else:
            f = open(romfilename, "rb")
            romdata = f.read()
            f.close()

        if check_rom_size:
            if len(romdata) > 64000:
                raise ValueError("This function uploads data to 'code RAM', so limited to 64KB." +
                                  " Binary appears to be too large, got %d bytes"%len(romdata))

        # NB - open_port MUST have been called before this or else the start-up messages
        # will be lost.

        #If no bitstream reloads the built-in
        self.load_ice40(bsfile)

        # This time might need to be longer or adjusted on certain builds - TODO test
        time.sleep(0.2)

        # Log startup message
        start_msg = bytes(self.sp.read())
        target_logger.debug(b"Start Message from Bootlader: " + start_msg)

        # Check the menu worked
        self.do_cmd("h", expected="Available CMDs:\r\n h: Help\r\n r: Restart\r\n u: Upload\r\n")
        
        # Upload
        self.do_cmd("u", "Awaiting neorv32_exe.bin... ")
        new_rom = gen_app_binary(romdata)

        self.sp.write(new_rom)

        time.sleep(0.1)
        resp = bytes(self.sp.read())

        target_logger.debug(b"Response to upload of data: " + resp)

        if b"OK" not in resp:
            raise IOError(b"Upload failed - bootloader responded with " + resp)

        boot_resp = self.do_cmd("e", expected="Booting", resp_read_len=13)
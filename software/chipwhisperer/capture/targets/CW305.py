#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2015-2021, NewAE Technology Inc
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
import random
from datetime import datetime
import os.path
import re
import io
from ._base import TargetTemplate
from ...hardware.naeusb.naeusb import NAEUSB,packuint32
from ...hardware.naeusb.pll_cdce906 import PLLCDCE906
from ...hardware.naeusb.fpga import FPGA
from ...common.utils import util
from ...common.utils.util import camel_case_deprecated, fw_ver_required
from ..scopes.cwhardware.ChipWhispererSAM3Update import SAMFWLoader
from ..api.cwcommon import ChipWhispererCommonInterface

from ...logging import *

class CW305_USB(object):
    REQ_SYSCFG = 0x22
    REQ_VCCINT = 0x31
    SYSCFG_CLKOFF = 0x04
    SYSCFG_CLKON = 0x05
    SYSCFG_TOGGLE = 0x06
    VCCINT_XORKEY = 0xAE


class CW305(TargetTemplate, ChipWhispererCommonInterface):

    """CW305 target object.

    This class contains the public API for the CW305 hardware.
    To connect to the CW305, the easiest method is::

        import chipwhisperer as cw
        scope = cw.scope()

        # scope can also be None here, unlike with the default SimpleSerial
        target = cw.target(scope,
                targets.CW305, bsfile=<valid FPGA bitstream file>)

    As of CW5.3, you can also specify the following::

        # can also be '35t'
        target = cw.target(scope, fpga_id='100t')

    To automatically program with the example AES bitstream

    If you're using the reference designs, typical configuration
    requires you to set the FPGA VCC-INT voltage and enable and 
    set the clock via the PLL. You'll probably also want to
    disable the USB clock during encryption to reduce noise::

        target.vccint_set(1.0) #set VCC-INT to 1V
        
        target.pll.pll_enable_set(True) #Enable PLL chip
        target.pll.pll_outenable_set(False, 0) # Disable unused PLL0
        target.pll.pll_outenable_set(True, 1)  # Enable PLL 
        target.pll.pll_outenable_set(False, 2) # Disable unused PLL2

        # optional, but reduces power trace noise
        target.clkusbautooff = True
        target.clksleeptime = 1 # 1ms typically good for sleep


    Don't forget to clock the ChipWhisperer ADC off the FPGA instead
    of the internal clock::

        scope.clock.adc_src = "extclk_x4"
        scope.clock.reset_adc() # make sure the DCM is locked

    Note that connecting to the CW305 includes programming the CW305 FPGA,
    if it isn't already.

    For more help about CW305 settings, try help() on this CW305 submodule:

       * target.pll
    """


    _name = "ChipWhisperer CW305 (Artix-7)"
    BATCHRUN_START = 0x1
    BATCHRUN_RANDOM_KEY = 0x2
    BATCHRUN_RANDOM_PT = 0x4


    def _getFWPy(self):
        from ...hardware.firmware.cw305 import fwver
        return fwver

    def __init__(self):
        import chipwhisperer as cw
        TargetTemplate.__init__(self)

        #NOTE: default values to make linter happy - these are never correct
        self.REG_CRYPT_KEY = None
        self.REG_CRYPT_TEXTIN = None
        self.REG_CRYPT_GO = None
        self.REG_USER_LED = None
        self.REG_CRYPT_CIPHEROUT = None
        self.REG_BUILDTIME = None

        self._naeusb = NAEUSB()
        self.pll = PLLCDCE906(self._naeusb, ref_freq = 12.0E6)
        self.fpga = FPGA(self._naeusb)

        self.hw = None
        self.oa = None

        self._woffset_sam3U = 0x000
        self.default_verilog_defines = 'cw305_defines.v'
        self.default_verilog_defines_full_path = os.path.dirname(cw.__file__) +  '/../../hardware/victims/cw305_artixtarget/fpga/common/' + self.default_verilog_defines
        self.registers = 12 # number of registers we expect to find
        self.bytecount_size = 7 # pBYTECNT_SIZE in Verilog

        self._clksleeptime = 1
        self._clkusbautooff = True
        self.last_key = bytearray([0]*16)
        self.target_name = 'AES'

    def _getNAEUSB(self):
        return self._naeusb

    def slurp_defines(self, defines_files=None):
        """ Parse Verilog defines file so we can access register and bit
        definitions by name and avoid 'magic numbers'.
        Args:
            defines_files (list): list of Verilog define files to parse
        """
        self.verilog_define_matches = 0

        if (defines_files is None) or (type(defines_files) != list):
            raise ValueError('defines_files must be provided as a list (even if it contains a single element)')

        for i,defines_file in enumerate(defines_files):
            if type(defines_file) == io.BytesIO:
                defines = io.TextIOWrapper(defines_file)
            else:
                if not os.path.isfile(defines_file):
                    target_logger.error('Cannot find %s. Please specify the location of %s on your filesystem.' % 
                                   (defines_files, self.default_verilog_defines))
                defines = open(defines_file, 'r', encoding='utf-8')
            define_regex_base  =   re.compile(r'`define')
            define_regex_reg   =   re.compile(r'`define\s+?REG_')
            define_regex_radix =   re.compile(r'`define\s+?(\w+).+?\'([bdh])([0-9a-fA-F]+)')
            define_regex_noradix = re.compile(r'`define\s+?(\w+?)\s+?(\d+?)')
            block_offset = 0
            for define in defines:
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
                        setattr(self, match.group(1), int(match.group(3),radix) + block_offset)
                    else:
                        match = define_regex_noradix.search(define)
                        if match:
                            self.verilog_define_matches += 1
                            setattr(self, match.group(1), int(match.group(2),10) + block_offset)
                        else:
                            target_logger.warning("Couldn't parse line: %s", define)
            defines.close()
        # make sure everything is cool:
        if self.verilog_define_matches != self.registers:
            target_logger.warning("Trouble parsing Verilog defines files (%s): didn't find the right number of defines; expected %d, got %d.\n" % (defines_files, self.registers, self.verilog_define_matches) +
                            "Ensure that the Verilog defines files above are the same that were used to build the bitfile.")


    def get_fpga_buildtime(self):
        """Returns date and time when FPGA bitfile was generated.
        """
        if self.REG_BUILDTIME is None:
            target_logger.error("target.REG_BUILDTIME unset. Have you given target a verilog defines file?")
        raw = self.fpga_read(self.REG_BUILDTIME, 4)
        # definitions: Xilinx XAPP1232
        day = raw[3] >> 3
        month = ((raw[3] & 0x7) << 1) + (raw[2] >> 7)
        year = ((raw[2] >> 1) & 0x3f) + 2000
        hour = ((raw[2] & 0x1) << 4) + (raw[1] >> 4)
        minute = ((raw[1] & 0xf) << 2) + (raw[0] >> 6)
        return "{}/{}/{}, {:02d}:{:02d}".format(month, day, year, hour, minute)
        return "FPGA build time: {}/{}/{}, {}:{}".format(month, day, year, hour, minute)


    def fpga_write(self, addr, data):
        """Write to an address on the FPGA

        Args:
            addr (int): Address to write to
            data (list): Data to write to addr

        """
        if len(data) <= 0:
            raise ValueError("Invalid data {}".format(data))
        addr = addr << self.bytecount_size
        return self._naeusb.cmdWriteMem(addr, data)

    def fpga_read(self, addr, readlen):
        """Read from an address on the FPGA

        Args:
            addr (int): Address to read from
            readlen (int): Length of data to read

        Returns:
            Requested data as a list
        """
        if readlen <= 0:
            raise ValueError("Invalid read len {}".format(readlen))
        addr = addr << self.bytecount_size
        data = self._naeusb.cmdReadMem(addr, readlen)
        return data

    def usb_clk_setenabled(self, status):
        """ Turn on or off the Data Clock to the FPGA """
        if status:
            self._naeusb.sendCtrl(CW305_USB.REQ_SYSCFG, CW305_USB.SYSCFG_CLKON)
        else:
            self._naeusb.sendCtrl(CW305_USB.REQ_SYSCFG, CW305_USB.SYSCFG_CLKOFF)

    def usb_trigger_toggle(self, _=None):
        """ Toggle the trigger line high then low """
        self._naeusb.sendCtrl(CW305_USB.REQ_SYSCFG, CW305_USB.SYSCFG_TOGGLE)

    def vccint_set(self, vccint=1.0):
        """ Set the VCC-INT for the FPGA """

        # print "vccint = " + str(vccint)

        if (vccint < 0.6) or (vccint > 1.1):
            raise ValueError("VCC-Int out of range 0.6V-1.1V")

        # Convert to mV
        vccint = int(vccint * 1000)
        vccsetting = [vccint & 0xff, (vccint >> 8) & 0xff, 0]

        # calculate checksum
        vccsetting[2] = vccsetting[0] ^ vccsetting[1] ^ CW305_USB.VCCINT_XORKEY

        self._naeusb.sendCtrl(CW305_USB.REQ_VCCINT, 0, vccsetting)

        resp = self._naeusb.readCtrl(CW305_USB.REQ_VCCINT, dlen=3)
        if resp[0] != 2:
            raise IOError("VCC-INT Write Error, response = %d" % resp[0])

    def vccint_get(self):
        """ Get the last set value for VCC-INT """

        resp = self._naeusb.readCtrl(CW305_USB.REQ_VCCINT, dlen=3)
        return float(resp[1] | (resp[2] << 8)) / 1000.0

    def _con(self, scope=None, bsfile=None, force=False, fpga_id=None, defines_files=None, slurp=True, prog_speed=10E6):
        """Connect to CW305 board, and download bitstream.

        If the target has already been programmed it skips reprogramming
        unless forced.

        Args:
            scope (ScopeTemplate): An instance of a scope object.
            bsfile (path): The path to the bitstream file to program the FPGA with.
            force (bool): Whether or not to force reprogramming.
            fpga_id (string): '100t', '35t', or None. If bsfile is None and fpga_id specified,
                              program with AES firmware for fpga_id
            defines_files (list, optional): path to cw305_defines.v
            slurp (bool, optional): Whether or not to slurp the Verilog defines.
        """
        self._naeusb.con(idProduct=[0xC305])
        if not fpga_id is None:
            if fpga_id not in ('100t', '35t'):
                raise ValueError(f"Invalid fpga {fpga_id}")
        self._fpga_id = fpga_id
        if self.fpga.isFPGAProgrammed() == False or force:
            if bsfile is None:
                if not fpga_id is None:
                    from chipwhisperer.hardware.firmware.cw305 import getsome
                    if self.target_name == 'AES':
                        bsdata = getsome(f"AES_{fpga_id}.bit")
                    elif self.target_name == 'Cryptech ecdsa256-v1 pmul':
                        bsdata = getsome(f"ECDSA256v1_pmul_{fpga_id}.bit")
                    starttime = datetime.now()
                    status = self.fpga.FPGAProgram(bsdata, exceptOnDoneFailure=False, prog_speed=prog_speed)
                    stoptime = datetime.now()
                    if status:
                        target_logger.info('FPGA Config OK, time: %s' % str(stoptime - starttime))
                    else:
                        target_logger.error('FPGA Done pin failed to go high, check bitstream is for target device.')
                else:
                    target_logger.warning("No FPGA Bitstream file specified.")
            elif not os.path.isfile(bsfile):
                target_logger.warning(("FPGA Bitstream not configured or '%s' not a file." % str(bsfile)))
            else:
                starttime = datetime.now()
                status = self.fpga.FPGAProgram(open(bsfile, "rb"), exceptOnDoneFailure=False, prog_speed=prog_speed)
                stoptime = datetime.now()
                if status:
                    target_logger.info('FPGA Config OK, time: %s' % str(stoptime - starttime))
                else:
                    target_logger.warning('FPGA Done pin failed to go high, check bitstream is for target device.')

        self.usb_clk_setenabled(True)
        self.pll.cdce906init()

        if slurp:
            # If fpga_id is provided, Verilog defines are obtained from CW305.py.
            # Otherwise, we look for it in a default location; if that doesn't exist, revert to CW305.py and warn user.
            found_defines = False
            if defines_files is None:
                if fpga_id is None:
                    verilog_defines = [self.default_verilog_defines_full_path]
                    if os.path.isfile(verilog_defines[0]):
                        found_defines = True
                    else:
                        target_logger.warning("Verilog defines not found in default location (%s).\nUsing defines from CW305.py.If this isn't what you want, either add 'slurp=False', or provide defines location in 'defines_files'" % verilog_defines[0])
                if not found_defines:
                    from chipwhisperer.hardware.firmware.cw305 import getsome
                    verilog_defines = [getsome(self.default_verilog_defines)]
            else:
                verilog_defines = defines_files
            self.slurp_defines(verilog_defines)


    def dis(self):
        # if self._naeusb:
        self._naeusb.close()

    def checkEncryptionKey(self, key):
        """Validate encryption key"""
        return key

    def loadEncryptionKey(self, key):
        """Write encryption key to FPGA"""
        if self.REG_CRYPT_KEY is None:
            target_logger.error("target.REG_CRYPT_KEY unset. Have you given target a verilog defines file?")
            return
        self.key = key
        key = key[::-1]
        self.fpga_write(self.REG_CRYPT_KEY, key)

    def loadInput(self, inputtext):
        """Write input to FPGA"""
        if self.REG_CRYPT_TEXTIN is None:
            target_logger.error("target.REG_CRYPT_TEXTIN unset. Have you given target a verilog defines file?")
            return
        self.input = inputtext
        text = inputtext[::-1]
        self.fpga_write(self.REG_CRYPT_TEXTIN, text)

    def is_done(self):
        """Check if FPGA is done"""
        if (self.REG_CRYPT_GO is None) or (self.REG_USER_LED is None):
            target_logger.error("target.REG_CRYPT_GO or target.REG_USER_LED unset. Have you given target a verilog defines file?")
            return
        result = self.fpga_read(self.REG_CRYPT_GO, 1)[0]
        if result == 0x01:
            return False
        else:
            self.fpga_write(self.REG_USER_LED, [0])
            return True

    isDone = camel_case_deprecated(is_done)

    def readOutput(self):
        """"Read output from FPGA"""
        if self.REG_CRYPT_CIPHEROUT is None:
            target_logger.error("target.REG_CRYPT_CIPHEROUT unset. Have you given target a verilog defines file?")
            return
        data = self.fpga_read(self.REG_CRYPT_CIPHEROUT, 16)
        data = data[::-1]
        #self.newInputData.emit(util.list2hexstr(data))
        return data

    def _getCWType(self):
        return 'cw305'

    @property
    def clkusbautooff(self):
        """ If set, the USB clock is automatically disabled on capture.

        The USB clock is re-enabled after self.clksleeptime milliseconds.

        Reads/Writes to the FPGA will not be possible until after
        the USB clock is reenabled, meaning :code:`usb_trigger_toggle()`
        must be used to trigger the FPGA to perform an encryption.

        :Getter: Gets whether to turn off the USB clock on capture

        :Setter: Sets whether to turn off the USB clock on capture
        """
        return self._clkusbautooff

    @clkusbautooff.setter
    def clkusbautooff(self, state):
        self._clkusbautooff = state

    @property
    def clksleeptime(self):
        """ Time (in milliseconds) that the USB clock is disabled for upon
        capture, if self.clkusbautooff is set.
        """
        return self._clksleeptime

    @clksleeptime.setter
    def clksleeptime(self, value):
        self._clksleeptime = value

    def go(self):
        """Disable USB clock (if requested), perform encryption, re-enable clock"""
        if (self.REG_USER_LED is None):
            target_logger.error("target.REG_USER_LED unset. Have you given target a verilog defines file?")
            return
        if self.clkusbautooff:
            self.usb_clk_setenabled(False)

        self.fpga_write(self.REG_USER_LED, [0x01])

        time.sleep(0.001)
        self.usb_trigger_toggle()
        # it's also possible to 'go' via register write but that won't take if
        # the USB clock was turned off:
        #self.fpga_write(self.REG_CRYPT_GO, [1])

        if self.clkusbautooff:
            time.sleep(self.clksleeptime/1000.0)
            self.usb_clk_setenabled(True)

    def simpleserial_read(self, cmd, pay_len, end='\n', timeout=250, ack=True):
        """Read data from target

        Mimics simpleserial protocol of serial based targets

        Args:
            cmd (str): Command to ues. Only accepts 'r' for now.
            pay_len: Unused
            end: Unused
            timeout: Unused
            ack: Unused

        Returns: Value from Crypto output register

        .. versionadded:: 5.1
            Added simpleserial_read to CW305
        """
        if cmd == "r":
            return self.readOutput()
        else:
            raise ValueError("Unknown command {}".format(cmd))

    def simpleserial_write(self, cmd, data, end=None):
        """Write data to target.

        Mimics simpleserial protocol of serial based targets.

        Args:
            cmd (str): Command to use. Target supports 'p' (write plaintext),
                and 'k' (write key).
            data (bytearray): Data to write to target
            end: Unused

        Raises:
            ValueError: Unknown command

        .. versionadded:: 5.1
            Added simpleserial_write to CW305
        """
        if cmd == 'p':
            self.loadInput(data)
            self.go()
        elif cmd == 'k':
            self.loadEncryptionKey(data)
        else:
            raise ValueError("Unknown command {}".format(cmd))

    def set_key(self, key, ack=False, timeout=250):
        """Checks if key is different from the last one sent. If so, send it.

        Args:
            key (bytearray):  key to send
            ack: Unused
            timeout: Unused

        .. versionadded:: 5.1
            Added set_key to CW305
        """
        if self.last_key != key:
            self.last_key = key
            self.simpleserial_write('k', key)

    def batchRun(self,batchsize=1024,random_key=True,random_pt=True,seed=None):
        """
            Run multiple encryptions on random data

            Args:
                batchsize (int): The number of encryption to run (default 1024).
                random_key (bool): True if the key is random (default False).
                random_pt (bool): True if the plaintext are random (default True).
                seed (int): random int32 for the PRG.
        """
        if seed is None:
            seed = random.randint(0,2**32)
        
        data = []
        data.extend(packuint32(1|(random_key<<1)|(random_pt<<2)|(batchsize<<16)))
        data.extend(packuint32(seed))
        self.sam3u_write(0,data)

        # generate the inputs
        if random_key:
            key = [[0 for x in range(16)] for y in range(batchsize)]
        else:
            key = None

        if random_pt:
            pt = [[0 for x in range(16)] for y in range(batchsize)]
        else:
            pt = None

        for b in range(batchsize):
            if random_key:
                for j in range(16):
                    key[b][15-j] = seed >> 24
                    seed += ((seed*seed)&0xffffffff) | 0x5
                    seed &= 0xffffffff
            if random_pt:
                for j in range(16):
                    pt[b][15-j] = seed >> 24
                    seed += ((seed*seed)&0xffffffff) | 0x5
                    seed &= 0xffffffff
        return key,pt

    def sam3u_write(self, addr, data):
        """Write to an address on the FPGA

        Args:
            addr (int): Address to write to
            data (list): Data to write to addr

        Raises:
            IOError: User attempted to write to a read-only location
        """
        if addr < self._woffset_sam3U:
            raise IOError("Write to read-only location: 0x%04x"%addr)
        if len(data) > (256+addr):
            raise IOError("Write will overflow at location: 0x%04x"%(256))

        return self._naeusb.cmdWriteMem(addr, data)

    # @fw_ver_required(0, 30)
    def spi_mode(self, enable=True, timeout=200, bsfile=None, prog_speed=10E6):
        """Enter programming mode for the onboard SPI chip
        
        Reprograms the FPGA with the appropriate bitstream and 
        returns an object with which to program the CW305 SPI chip
        (see documentation on the returned object for more info)

        Args:
            enable (bool): Enable the SPI interface before returning it. Defaults to True
            timeout (int): USB timeout in ms. Defaults to 200.
            bsfile (string): If not None, program with a bitstream pointed to by bsfile.
                             If None, program with SPI passthrough bitstream for the chip
                             specified during connection (or cw.target()) 

        Returns:
            A FPGASPI object which can be used to erase/program/verify/read the SPI
            chip on the CW305.
        """
        self._getNAEUSB().check_feature("FPGA_SPI_PASSTHROUGH", True)
            # target_logger.error("SPI mode requires fw 0.30.0 or newer. You have {}".format(self.fw_version_str))
            # return
        if self._fpga_id is None and bsfile is None:
            target_logger.warning("CW305 requires passthrough bitstream to program SPI chip, but file/chip not specified")
        else:
            bsdata = None
            if self._fpga_id:
                from chipwhisperer.hardware.firmware.cw305 import getsome
                bsdata = getsome(f"SPI_flash_{self._fpga_id}.bit")
            else:
                bsdata = open(bsfile, "rb")
            starttime = datetime.now()
            status = self.fpga.FPGAProgram(bsdata, exceptOnDoneFailure=False, prog_speed=prog_speed)
            stoptime = datetime.now()
            if status:
                target_logger.info('FPGA Config OK, time: %s' % str(stoptime - starttime))
            else:
                target_logger.warning('FPGA Done pin failed to go high, check bitstream is for target device.')

        spi = FPGASPI(self._naeusb, timeout)
        spi.enable_interface(enable)
        return spi

    def gpio_mode(self, timeout=200):
        """Allow arbitrary GPIO access on SAM3U
        
        Allows low-level IO access to SAM3U GPIO, and also SPI transfers.
        (see documentation on the returned object for more info)

        Args:
            timeout (int): USB timeout in ms. Defaults to 200.

        Returns:
            A FPGAIO object which can be used to access IO on the CW305.
        """
        self._getNAEUSB().check_feature("SAM3U_GPIO_MODE", True)
            # target_logger.error("GPIO mode requires fw 0.40.0 or newer. You have {}".format(self.fw_version_str))
        io = FPGAIO(self._naeusb, timeout)
        return io

class FPGASPI:
    """ Programmer for the CW305's onboard SPI flash
    
    Built for AT25DF321A, but has customizable codes
    for write enable/disable, chip erase, status, write, read,
    and block erase, as well as page size.
    
    To program the SPI, the CW305 must be loaded with a special bitstream
    (will be done automatically if you specify fpga_id when connecting to the CW305) 
    and have FW version > 0.30 for its USB microcontroller.
    
    Basic usage::
    
        fpga = cw.target(None, cw.targets.CW305, fpga_id='100t') #for CW305_100t
        fpga = cw.target(None, cw.targets.CW305, fpga_id='35t') #for CW305_35t
        spi = fpga.spi_mode()
        
        spi.erase_chip() # can also use spi.erase_block() for smaller/faster erases
        with open('bitfile.bit', 'rb') as f:
            data = list(f.read())
            spi.program(data) # also verifies by default

    .. warning::

        The AT25DF321A has the following behaviour:

        * Writes longer than a page (256 bytes) will use only the last 256 bytes
        * Reads can cross page boundaries (though read() and verify() will only read 256 bytes at a time)
        * Writes that don't begin on a page bound will wrap if a page boundary is crossed

        The user is responsible for ensuring that their writes begin on page boundaries.
    """

    REQ_FPGASPI_PROGRAM = 0x33
    REQ_ENABLE_SPI = 0xA0
    REQ_DISABLE_SPI = 0xA1
    
    REQ_CS_HIGH = 0xA3
    REQ_CS_LOW = 0xA2
    
    REQ_SEND_DATA = 0xA4
    
    WRITE_EN = 0x06
    WRITE_DIS = 0x04
    
    ERASE_CHIP = 0x60
    STATUS_0 = 0x05
    WRITE = 0x02
    READ = 0x03
    
    ERASE_BLOCK = {"4K": 0x20, "32K": 0x52, "64K": 0xD8}
    
    PAGE_SIZE = 256

    def __init__(self, usb, timeout=200):
        self.sendCtrl = usb.sendCtrl
        self.readCtrl = usb.readCtrl
        self._usb = usb
        self._timeout = timeout
        
        
    def enable_interface(self, enable):
        """Enable or disable the SPI interface
        
        Args:
            enable (bool): Enable (True) or disable (False) SPI interface
        """
        if enable:
            self.sendCtrl(self.REQ_FPGASPI_PROGRAM, 0xA0)
        else:
            self.sendCtrl(self.REQ_FPGASPI_PROGRAM, 0xA1)
        
    def set_cs_pin(self, status):
        """Set the SPI pin high or low
        
        Args:
            status (bool): Set CS pin high (True) or low (False)
        """
        if status:
            self.sendCtrl(self.REQ_FPGASPI_PROGRAM, 0xA3)
        else:
            self.sendCtrl(self.REQ_FPGASPI_PROGRAM, 0xA2)        

    def spi_tx_rx(self, data):
        """Write up to 64 bytes of data to the SPI chip
        
        Args:
            data (list): Write data over the SPI interface

        Raises:
            ValueError: len(data) > 64
        """
        if len(data) > 64:
            raise ValueError("Data is tooooooo long!")
        self.sendCtrl(self.REQ_FPGASPI_PROGRAM, 0xA4, data)
        readdata = self.readCtrl(self.REQ_FPGASPI_PROGRAM, dlen=len(data))
        return readdata
    
    def enable_write(self, enable):
        """Enable write/erase commands on the SPI chip.
        
        Args:
            enable (bool): Enable (True) or disable (False) write/erase on the SPI chip
        """
        self.set_cs_pin(False)
        if enable:
            self.spi_tx_rx([self.WRITE_EN])
        else:
            self.spi_tx_rx([self.WRITE_DIS])
        self.set_cs_pin(True)
            
    def erase_chip(self, timeout=None):
        """Erase the whole SPI chip. Slow (~25s)

        Args:
            timeout (int): Timeout in ms. If None, set to 0xFFFFFFFF (approx 1000 hours)

        Raises:
            IOError: Erase timed out
        """
        self.enable_write(True)
        self.set_cs_pin(False)
        self.spi_tx_rx([self.ERASE_CHIP])
        self.set_cs_pin(True)
        
        self.wait_busy(timeout)
        
    def wait_busy(self, timeout=1000):
        """Wait for the busy status on the FPGA to clear
        
        Args:
            timeout (int): Timeout in ms. If None, set to 0xFFFFFFFF (approx 1000 hours)
                            Defaults to 1000 (1s).

        Raises:
            IOError: Wait timed out

        """
        if timeout is None:
            timeout = 0xFFFFFFFF
        self.set_cs_pin(False)
        self.spi_tx_rx([self.STATUS_0])
        while (self.spi_tx_rx([0x00])[0] & 0x01) and (timeout > 0):
            time.sleep(0.001)
            timeout -= 1

        if timeout == 0:
            raise IOError("Device timed out")
        self.set_cs_pin(True)
        
    def cmd_write_mem(self, data, addr=0x000000, timeout=1000):
        """Write up to a page of data
        
        For the default chip, a page is 256 bytes.

        Args:
            data (list): Data to write
            addr (int): Address to write to, defaults to 0x00.
            timeout (int): Timeout in ms. If None, set to 0xFFFFFFFF (approx 1000 hours)
                            Defaults to 1000 (1s).

        Raises:
            ValueError: Data is longer than a page
            IOError: Wait timed out
        """
        if len(data) > self.PAGE_SIZE:
            raise ValueError(f"Data too long {len(data)} vs {self.PAGE_SIZE}")
            
        self.enable_write(True)
        self.set_cs_pin(False)
        cmd = [self.WRITE, (addr >> 16)&0xFF, (addr >> 8)&0xFF, addr & 0xFF]
        
        self.spi_tx_rx(cmd)
        
        data_len = len(data)
        data_written = 0
        
        # max USB ctrl transfer 64 bytes
        while data_len > data_written:
            cmd = data[data_written:data_written + min(64, data_len-data_written)]
            self.spi_tx_rx(cmd)
            data_written += len(cmd)
            
        self.set_cs_pin(True)
        self.wait_busy(timeout)
        
    def cmd_read_mem(self, length, addr):
        """Read up to a page of data
        
        For the default chip, a page is 256 bytes

        Args:
            length (int): Length of data to read
            addr (int): Address to read from

        Raises:
            ValueError: length is bigger than a page
        """
        if length > self.PAGE_SIZE:
            raise ValueError(f"Data too long {length} vs {self.PAGE_SIZE}")
            
        self.set_cs_pin(False)
        cmd = [self.READ, (addr >> 16)&0xFF, (addr >> 8)&0xFF, addr & 0xFF]
        self.spi_tx_rx(cmd)
        buf = []
        data_read = 0
        
        while length > data_read:
            cmd = [0x00]*min(64, length-data_read)
            buf.extend(self.spi_tx_rx(cmd))
            data_read += len(cmd)
            
        self.set_cs_pin(True)
        return buf
    
    def program(self, data, addr=0x000000, verify=True, timeout=1000):
        """Program (and optionally verify) the chip with data.
        
        Args:
            data (list): data to write
            addr (int): Address to start programming from. Defaults to 0x00. Must align with a page.
            verify (bool): If True, verify data written at end of full write. Defaults to True.
            timeout (int): Timeout in ms for writes. If None, set to 0xFFFFFFFF (approx 1000 hours)
                            Defaults to 1000 (1s).

        Raises:
            IOError: Verify failed or timeout
        """
        data_written = 0
        while len(data) > data_written:
            to_write = min(self.PAGE_SIZE, len(data)-data_written)
            self.cmd_write_mem(data[data_written:data_written+to_write], addr + data_written, timeout=timeout)
            data_written += to_write
            
        if verify:
            self.verify(data, addr)
            
    def verify(self, data, addr=0x000000):
        """Verify the data on the SPI chip
        
        Args:
            data (list): data to verify
            addr (int): Address to start verification from. Defaults to 0x00. Must align with a page.

        Raises:
            IOError: Verify failed
        """
        data_read = 0
        while len(data) > data_read:
            to_read = min(self.PAGE_SIZE, len(data)-data_read)
            response = self.cmd_read_mem(to_read, addr + data_read)
            
            if (response != data[data_read:data_read + to_read]):
                raise IOError(f"Verify failed at {addr + data_read}, {response} != {data[data_read:data_read + to_read]}")
                
            data_read += to_read
            
    def read(self, length, addr=0x000000):
        """Read data on the SPI chip
        
        Args:
            length (int): Length of data to read
            addr (int): Address to start read from. Must align with a page.
            
        """
        data_read = 0
        ret = []
        while length > data_read:
            to_read = min(self.PAGE_SIZE, length-data_read)
            ret.extend(self.cmd_read_mem(to_read, addr + data_read))
                
            data_read += to_read
            
        return ret
        
    def erase_block(self, addr, size="4K", timeout=1000):
        """Erase a block on the SPI chip. See spi.ERASE_BLOCK for available erase lengths
        
        Args:
            addr (int): Address to erase from
            size (string): See spi.ERASE_BLOCK for available sizes
            timeout (int): Timeout in ms. If None, set to 0xFFFFFFFF (approx 1000 hours)
                            Defaults to 1000 (1s).

        Raises:
            IOError: Erase timed out
        """
        if size not in self.ERASE_BLOCK:
            raise ValueError(f"{size} not in available sizes {self.ERASE_BLOCK}")
            
        self.enable_write(True)
        self.set_cs_pin(False)
        self.spi_tx_rx([self.ERASE_BLOCK[size], (addr >> 16)&0xFF, (addr >> 8)&0xFF, addr&0xFF])
        self.set_cs_pin(True)
        
        self.wait_busy(timeout)

class FPGAIO:
    """ User IO to override external bus.
    
    Allows you to use any pin on the SAM3U as a user IO. This includes pins
    such as the external IO interface, and basically anything else you can find.
    
    The pin names are strings, and come from one of three sources:
        * SAM3U pin names, such as "PC11", "PB9", etc.
        * Net names from the CW305 schematic such as "USB_A20".
        * The FPGA ball location that is connected to the SAM3U pin, such as "M2".
    
    Any function taking a pin name assumes you pass a string with one of those. You
    do not need to specify your source - it will autodetect the pin name (if possible).
    The SAM3U pin names allow access to every pin, including those which are not actually
    connected on the PCB itself.

    Beyond simple GPIO toggling, a bit-banged SPI interface can be defined and connected
    to any of the SAM3U pins. The bit-banged interface is done on the SAM3U microcontroller,
    with a default SCK frequency of around 1.5 MHz. You can additionally define waitstates
    to slow down this clock.
    
    Basic usage::
    
        fpga = cw.target(None, cw.targets.CW305)
        io = target.gpio_mode()
        
        # Take over the SAM3U blue LED (normally controlled by firmware, won't be after this)
        import time
        io.pin_set_output("LED_BLUE")
        io.pin_set_state("LED_BLUE", 0)
        time.sleep(0.5)
        io.pin_set_state("LED_BLUE", 1)

        # Example - toggle pin associated with FPGA pin C1 (would be USB_A11)
        import time
        io.pin_set_output("C1")
        io.pin_set_state("C1", 0)
        time.sleep(0.1)
        io.pin_set_state("C1", 1)

        # Setup a SPI interface based on schematic net names
        io.spi1_setpins(sdo="USB_A20", sdi="USB_A19", sck="USB_A18", cs="USB_A17")
        io.spi1_enable(True)

        somedata = [0x11, 0x22, 0x33]

        response = io.spi1_transfer(somedata)
        print(response)

    If you want to see all possible pin names, you can access them with:
        io.SAM3U_PIN_NAMES.keys()
        io.SCHEMATIC_PIN_NAMES.keys()
        io.FPGA_PIN_NAMES.keys()
    """

    REQ_FPGAIO_UTIL      = 0x34
    REQ_IO_CONFIG        = 0xA0
    REQ_IO_RELEASE       = 0xA1
    REQ_IO_OUTPUT        = 0xA2


    CONFIG_PIN_INPUT     = 0x01
    CONFIG_PIN_OUTPUT    = 0x02
    CONFIG_PIN_SPI1_SDO  = 0x10
    CONFIG_PIN_SPI1_SDI  = 0x11
    CONFIG_PIN_SPI1_SCK  = 0x12
    CONFIG_PIN_SPI1_CS   = 0x13

    REQ_FPGASPI1_XFER    = 0x35
    REQ_ENABLE_SPI       = 0xA0
    REQ_DISABLE_SPI      = 0xA1    
    REQ_CS_LOW           = 0xA2
    REQ_CS_HIGH          = 0xA3
    REQ_SEND_DATA        = 0xA4
    
    SAM3U_PIN_NAMES = { "PA0":  0,
                        "PA1":  1,
                        "PA2":  2,
                        "PA3":  3,
                        "PA4":  4,
                        "PA5":  5,
                        "PA6":  6,
                        "PA7":  7,
                        "PA8":  8,
                        "PA9":  9,
                        "PA10": 10,
                        "PA11": 11,
                        "PA12": 12,
                        "PA13": 13,
                        "PA14": 14,
                        "PA15": 15,
                        "PA16": 16,
                        "PA17": 17,
                        "PA18": 18,
                        "PA19": 19,
                        "PA20": 20,
                        "PA21": 21,
                        "PA22": 22,
                        "PA23": 23,
                        "PA24": 24,
                        "PA25": 25,
                        "PA26": 26,
                        "PA27": 27,
                        "PA28": 28,
                        "PA29": 29,
                        "PA30": 30,
                        "PA31": 31,
                        "PB0":  32,
                        "PB1":  33,
                        "PB2":  34,
                        "PB3":  35,
                        "PB4":  36,
                        "PB5":  37,
                        "PB6":  38,
                        "PB7":  39,
                        "PB8":  40,
                        "PB9":  41,
                        "PB10": 42,
                        "PB11": 43,
                        "PB12": 44,
                        "PB13": 45,
                        "PB14": 46,
                        "PB15": 47,
                        "PB16": 48,
                        "PB17": 49,
                        "PB18": 50,
                        "PB19": 51,
                        "PB20": 52,
                        "PB21": 53,
                        "PB22": 54,
                        "PB23": 55,
                        "PB24": 56,
                        "PB25": 57,
                        "PB26": 58,
                        "PB27": 59,
                        "PB28": 60,
                        "PB29": 61,
                        "PB30": 62,
                        "PB31": 63,
                        "PC0":  64,
                        "PC1":  65,
                        "PC2":  66,
                        "PC3":  67,
                        "PC4":  68,
                        "PC5":  69,
                        "PC6":  70,
                        "PC7":  71,
                        "PC8":  72,
                        "PC9":  73,
                        "PC10": 74,
                        "PC11": 75,
                        "PC12": 76,
                        "PC13": 77,
                        "PC14": 78,
                        "PC15": 79,
                        "PC16": 80,
                        "PC17": 81,
                        "PC18": 82,
                        "PC19": 83,
                        "PC20": 84,
                        "PC21": 85,
                        "PC22": 86,
                        "PC23": 87,
                        "PC24": 88,
                        "PC25": 89,
                        "PC26": 90,
                        "PC27": 91,
                        "PC28": 92,
                        "PC29": 93,
                        "PC30": 94,
                        "PC31": 95}

    FPGA_PIN_NAMES = {"A7":"USB_D0",
                    "B6":"USB_D1",
                    "D3":"USB_D2",
                    "E3":"USB_D3",
                    "F3":"USB_D4",
                    "B5":"USB_D5",
                    "K1":"USB_D6",
                    "K2":"USB_D7",
                    "F4":"USB_A0",
                    "G5":"USB_A1",
                    "J1":"USB_A2",
                    "H1":"USB_A3",
                    "H2":"USB_A4",
                    "G1":"USB_A5",
                    "G2":"USB_A6",
                    "F2":"USB_A7",
                    "E1":"USB_A8",
                    "E2":"USB_A9",
                    "D1":"USB_A10",
                    "C1":"USB_A11",
                    "K3":"USB_A12",
                    "L2":"USB_A13",
                    "J3":"USB_A14",
                    "B2":"USB_A15",
                    "C7":"USB_A16",
                    "C6":"USB_A17",
                    "D6":"USB_A18",
                    "C4":"USB_A19",
                    "D5":"USB_A20",
                    "A4":"USBRD",
                    "C2":"USBWR",
                    "A3":"USBCE",
                    "A2":"USBALE",
                    "A5":"USBSPARE0",
                    "B4":"USBSPARE1",
                    "B1":"USBSPARE2"}

    SCHEMATIC_PIN_NAMES = {"USBSPARE0":"PB17",
                        "USBSPARE1":"PB18",
                        "USBSPARE2":"PB22",
                        "USBRD":"PB19",
                        "USBWR":"PB23",
                        "USBCE":"PB20",
                        "USBALE":"PB21",
                        "USBCK0":"PA27",
                        "USBCK1":"PB24",
                        "USB_A0":"PB7",
                        "USB_A1":"PB8",
                        "USB_A2":"PC0",
                        "USB_A3":"PC1",
                        "USB_A4":"PC2",
                        "USB_A5":"PC3",
                        "USB_A6":"PC4",
                        "USB_A7":"PC5",
                        "USB_A8":"PC6",
                        "USB_A9":"PC7",
                        "USB_A10":"PC8",
                        "USB_A11":"PC9",
                        "USB_A12":"PC10",
                        "USB_A13":"PC11",
                        "USB_A14":"PC20",
                        "USB_A15":"PC21",
                        "USB_A16":"PC22",
                        "USB_A17":"PC23",
                        "USB_A18":"PC24",
                        "USB_A19":"PC25",
                        "USB_A20":"PC26",
                        "USB_D0":"PB9",
                        "USB_D1":"PB10",
                        "USB_D2":"PB11",
                        "USB_D3":"PB12",
                        "USB_D4":"PB13",
                        "USB_D5":"PB14",
                        "USB_D6":"PB15",
                        "USB_D7":"PB16",
                        "SWSTATE":"PB30",
                        "PWRON":"PB31",
                        "FPGA_INITB":"PC29",
                        "FPGA_DONE":"PA7",
                        "FPGA_PROGRAM":"PA8",
                        "SAM_MISO":"PA13",
                        "SAM_MOSI":"PA14",
                        "SAM_SCK":"PA15",
                        "PWR_SDA":"PA9",
                        "PWR_SCL":"PA10",
                        "XMEGA_PDIC":"PA17",
                        "XMEGA_PDID1":"PA18",
                        "XMEGA_PDID2":"PA19",
                        "LED_RED":"PC15",
                        "LED_GREEN":"PC16",
                        "LED_BLUE":"PC17"}

    def __init__(self, usb, timeout=200):
        self.sendCtrl = usb.sendCtrl
        self.readCtrl = usb.readCtrl
        self._usb = usb
        self._timeout = timeout

    def pin_name_to_number(self, pinname):
        """Convert from a user-friendly pin name to the number.

        This function can be passed a name from the schematic (such as "USB_A20"),
        a name from the SAM3U peripherals (such as "PB22"), or a FPGA pin location
        (such as "M2"). It will attempt to auto-detect which one you passed.

        Args:
            pinname (str): Name such as "PB22", "USB_A20", or "M2".
        """      
        if isinstance(pinname, int):
            return pinname # TODO: is this right?

        pinname = pinname.upper()

        if pinname in self.SCHEMATIC_PIN_NAMES:
            return self.SAM3U_PIN_NAMES[self.SCHEMATIC_PIN_NAMES[pinname]]

        if pinname in self.FPGA_PIN_NAMES:
            return self.SAM3U_PIN_NAMES[self.SCHEMATIC_PIN_NAMES[self.FPGA_PIN_NAMES[pinname]]]

        if pinname in self.SAM3U_PIN_NAMES:
            return self.SAM3U_PIN_NAMES[pinname]
        
        raise ValueError("I don't know what pin this is (sorry): %s"%(pinname))
        
    def pin_set_output(self, pinname):
        """Set a given pin as an output.
        
        Args:
            pinname (str): Name such as "PB22", "USB_A20", or "M2".   
        """
        pinnum = self.pin_name_to_number(pinname)
        self.sendCtrl(self.REQ_FPGAIO_UTIL, self.REQ_IO_CONFIG, [pinnum, self.CONFIG_PIN_OUTPUT])

    def pin_set_state(self, pinname, state):
        """Set the state of a pin (must have been set as output previously).
        
        Args:
            pinname (str): Name such as "PB22", "USB_A20", or "M2".   
            state (bool): Set pin high (True) or low (False)
        """
        pinnum = self.pin_name_to_number(pinname)
        if state:
            state = 1
        else:
            state = 0
        self.sendCtrl(self.REQ_FPGAIO_UTIL, self.REQ_IO_OUTPUT, [pinnum, state])

    def spi1_setpins(self, sdo, sdi, sck, cs):
        """Set the pins to be used for the SPI1 interface.

         Args:
            sdo (str): Serial Data Out (output from SAM3U) pin name such as "PB22", "USB_A20", or "M2".       
            sdi (str): Serial Data In (input to SAM3U) pin name such as "PB22", "USB_A20", or "M2".  
            sck (str): Serial Clock (output from SAM3U) pin name such as "PB22", "USB_A20", or "M2".  
            cs (str): Chip Select (output from SAM3U) name such as "PB22", "USB_A20", or "M2".  
        """

        sdo = self.pin_name_to_number(sdo)
        sdi = self.pin_name_to_number(sdi)
        sck = self.pin_name_to_number(sck)
        cs = self.pin_name_to_number(cs)

        self.sendCtrl(self.REQ_FPGAIO_UTIL, self.REQ_IO_CONFIG, [sdo, self.CONFIG_PIN_SPI1_SDO])
        self.sendCtrl(self.REQ_FPGAIO_UTIL, self.REQ_IO_CONFIG, [sdi, self.CONFIG_PIN_SPI1_SDI])
        self.sendCtrl(self.REQ_FPGAIO_UTIL, self.REQ_IO_CONFIG, [sck, self.CONFIG_PIN_SPI1_SCK])
        self.sendCtrl(self.REQ_FPGAIO_UTIL, self.REQ_IO_CONFIG, [cs, self.CONFIG_PIN_SPI1_CS])

        
    def spi1_enable(self, enable, waitcycles=0):
        """Enable or disable the SPI interface.
        
        Args:
            enable (bool): Enable (True) or disable (False) SPI interface
        """
        if enable:
            self.sendCtrl(self.REQ_FPGASPI1_XFER, 0xA0)
        else:
            self.sendCtrl(self.REQ_FPGASPI1_XFER, 0xA1)
        
        if waitcycles > 0:
            raise NotImplementedError("💩 - Colin was lazy")
        
    def spi1_set_cs_pin(self, status):
        """Set the SPI pin high or low.
        
        Args:
            status (bool): Set CS pin high (True) or low (False)
        """
        if status:
            self.sendCtrl(self.REQ_FPGASPI1_XFER, 0xA3)
        else:
            self.sendCtrl(self.REQ_FPGASPI1_XFER, 0xA2)        

    def spi1_tx_rx(self, data):
        """Write up to 64 bytes of data to the SPI interface (no CS action).
        
        This is a low-level function that performs a single transfer. Does not
        affect the CS pin.

        Args:
            data (list): Write data over the SPI interface

        Raises:
            ValueError: len(data) > 64
        """
        if len(data) > 64:
            raise ValueError("Data is tooooooo long!")
        self.sendCtrl(self.REQ_FPGASPI1_XFER, 0xA4, data)
        readdata = self.readCtrl(self.REQ_FPGASPI1_XFER, dlen=len(data))
        return readdata
    
    def spi1_transfer(self, data):
        """Writes data, dropping CS before and raising after, returns read data.

        Args:
            data (list): Write data over the SPI interface
        """

        resp = []

        datalen = len(data)

        self.spi1_set_cs_pin(False)

        for i in range(0, datalen, 64):
            dataend = i+64
            
            if dataend > datalen:
                dataend = datalen
            
            tx = data[i:dataend]
            rx = self.spi1_tx_rx(tx)
            resp.extend(rx)

        self.spi1_set_cs_pin(True)

        return resp

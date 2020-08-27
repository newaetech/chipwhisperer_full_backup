#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2015-2019, NewAE Technology Inc
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
import logging
import time
import random
from datetime import datetime
import os.path
from ._base import TargetTemplate
from chipwhisperer.hardware.naeusb.naeusb import NAEUSB,packuint32
from chipwhisperer.hardware.naeusb.pll_cdce906 import PLLCDCE906
from chipwhisperer.hardware.naeusb.fpga import FPGA
from chipwhisperer.common.utils import util
from chipwhisperer.common.utils.util import camel_case_deprecated


class CW305_USB(object):
    REQ_SYSCFG = 0x22
    REQ_VCCINT = 0x31
    SYSCFG_CLKOFF = 0x04
    SYSCFG_CLKON = 0x05
    SYSCFG_TOGGLE = 0x06
    VCCINT_XORKEY = 0xAE


class CW305(TargetTemplate):

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

    def __init__(self):
        TargetTemplate.__init__(self)
        self._naeusb = NAEUSB()
        self.pll = PLLCDCE906(self._naeusb, ref_freq = 12.0E6)
        self.fpga = FPGA(self._naeusb)

        self.hw = None
        self.oa = None

        self._woffset = 0x400
        self._woffset_sam3U = 0x000

        self._clksleeptime = 1
        self._clkusbautooff = True
        self.last_key = bytearray([0]*16)

    def _getNAEUSB(self):
        return self._naeusb


    def fpga_write(self, addr, data):
        """Write to an address on the FPGA

        Args:
            addr (int): Address to write to
            data (list): Data to write to addr

        Raises:
            IOError: User attempted to write to a read-only location
        """
        if addr < self._woffset:
            raise IOError("Write to read-only location: 0x%04x"%addr)

        return self._naeusb.cmdWriteMem(addr, data)

    def fpga_read(self, addr, readlen):
        """Read from an address on the FPGA

        Args:
            addr (int): Address to read from
            readlen (int): Length of data to read

        Returns:
            Requested data as a list
        """
        if addr > self._woffset:
            logging.info('Read from write address, confirm this is not an error')

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

        if (vccint < 0.6) or (vccint > 1.15):
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

    def _con(self, scope=None, bsfile=None, force=False, fpga_id=None):
        """Connect to CW305 board, and download bitstream.

        If the target has already been programmed it skips reprogramming
        unless forced.

        Args:
            scope (ScopeTemplate): An instance of a scope object.
            bsfile (path): The path to the bitstream file to program the FPGA with.
            force (bool): Whether or not to force reprogramming.
            fpga_id (string): '100t', '35t', or None. If bsfile is None and fpga_id specified,
                              program with AES firmware for fpga_id
        """

        from datetime import datetime
        self._naeusb.con(idProduct=[0xC305])
        if not fpga_id is None:
            if fpga_id not in ('100t', '35t'):
                raise ValueError(f"Invalid fpga {fpga_id}")
        self._fpga_id = fpga_id
        if self.fpga.isFPGAProgrammed() == False or force:
            if bsfile is None:
                if not fpga_id is None:
                    from chipwhisperer.hardware.firmware.cw305 import getsome
                    bsdata = getsome(f"AES_{fpga_id}.bit")
                    starttime = datetime.now()
                    status = self.fpga.FPGAProgram(bsdata, exceptOnDoneFailure=False)
                    stoptime = datetime.now()
                    if status:
                        logging.info('FPGA Config OK, time: %s' % str(stoptime - starttime))
                    else:
                        logging.warning('FPGA Done pin failed to go high, check bitstream is for target device.')
                else:
                    print("No FPGA Bitstream file specified.")
            elif not os.path.isfile(bsfile):
                print(("FPGA Bitstream not configured or '%s' not a file." % str(bsfile)))
            else:
                starttime = datetime.now()
                status = self.fpga.FPGAProgram(open(bsfile, "rb"), exceptOnDoneFailure=False)
                stoptime = datetime.now()
                if status:
                    logging.info('FPGA Config OK, time: %s' % str(stoptime - starttime))
                else:
                    logging.warning('FPGA Done pin failed to go high, check bitstream is for target device.')
        self.usb_clk_setenabled(True)
        self.fpga_write(0x100+self._woffset, [0])
        self.pll.cdce906init()

    def _dis(self):
        if self._naeusb:
            self._naeusb.close()

    def checkEncryptionKey(self, key):
        """Validate encryption key"""
        return key

    def loadEncryptionKey(self, key):
        """Write encryption key to FPGA"""
        self.key = key
        key = key[::-1]
        self.fpga_write(0x100+self._woffset, key)

    def loadInput(self, inputtext):
        """Write input to FPGA"""
        self.input = inputtext
        text = inputtext[::-1]
        self.fpga_write(0x200+self._woffset, text)

    def is_done(self):
        """Check if FPGA is done"""
        result = self.fpga_read(0x50, 1)[0]

        if result == 0x00:
            return False
        else:
            # Clear trigger
            self.fpga_write(0x40+self._woffset, [0])
            # LED Off
            self.fpga_write(0x10+self._woffset, [0])
            return True

    isDone = camel_case_deprecated(is_done)

    def readOutput(self):
        """"Read output from FPGA"""
        data = self.fpga_read(0x200, 16)
        data = data[::-1]
        #self.newInputData.emit(util.list2hexstr(data))
        return data

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
        if self.clkusbautooff:
            self.usb_clk_setenabled(False)

        #LED On
        self.fpga_write(0x10+self._woffset, [0x01])

        time.sleep(0.001)
        self.usb_trigger_toggle()
        # self.FPGAWrite(0x100, [1])
        # self.FPGAWrite(0x100, [0])

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
                    key[b][15-j] = seed >> 24;
                    seed += ((seed*seed)&0xffffffff) | 0x5
                    seed &= 0xffffffff
            if random_pt:
                for j in range(16):
                    pt[b][15-j] = seed >> 24;
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

        return self._naeusb.cmdWriteSam3U(addr, data)

    def spi_mode(self, enable=True, timeout=200, bsfile=None):
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
        from datetime import datetime
        if self._fpga_id is None and bsfile is None:
            logging.warning("CW305 requires passthrough bitstream to program SPI chip, but file/chip not specified")
        else:
            bsdata = None
            if self._fpga_id:
                from chipwhisperer.hardware.firmware.cw305 import getsome
                bsdata = getsome(f"SPI_flash_{self._fpga_id}.bit")
            else:
                bsdata = open(bsfile, "rb")
            starttime = datetime.now()
            status = self.fpga.FPGAProgram(bsdata, exceptOnDoneFailure=False)
            stoptime = datetime.now()
            if status:
                logging.info('FPGA Config OK, time: %s' % str(stoptime - starttime))
            else:
                logging.warning('FPGA Done pin failed to go high, check bitstream is for target device.')

        spi = FPGASPI(self._naeusb, timeout)
        spi.enable_interface(enable)
        return spi

class FPGASPI:
    """ Programmer for the CW305's onboard SPI flash
    
    Built for AT25DF321A, but has customizable codes
    for write enable/disable, chip erase, status, write, read,
    and block erase, as well as page size.
    
    To program the SPI, the CW305 must be loaded with a special bitstream
    (will be done automatically if you specify fpga_id when connecting to the CW305) 
    and have FW version > 0.20 for its USB microcontroller.
    
    Basic usage::
    
        fpga = cw.target(None, cw.targets.CW305, fpga_id='100t') #for CW305_100t
        fpga = cw.target(None, cw.targets.CW305, fpga_id='35t') #for CW305_35t
        spi = fpga.spi_mode()
        
        spi.erase_chip() # can also use spi.erase_block() for smaller/faster erases
        
        spi.program(bitstream) # also verifies by default

    .. warning::

        The AT25DF321A has the following behaviour:

        * Writes longer than a page (256 bytes) will use only the last 256 bytes
        * Reads can cross page boundaries (though read() and verify() will only read 256 bytes at a time)
        * Writes that don't begin on a page bound will wrap if a page boundary is crossed

        The user is responsible for ensuring that their writes being on page boundaries.
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
            raise ValueError(f"Data too long {len(data)} vs {self.PAGE_SIZE}")
            
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


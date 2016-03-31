# -*- coding: utf-8 -*-
#
# Copyright (c) 2014-2016, NewAE Technology Inc
# All rights reserved.
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
#==========================================================================


import time
from chipwhisperer.common.utils import Util

from naeusb import packuint32

# NOTE: These objects are currently manually maintained. Eventually it will be automatically created
#      from avrdude.conf, but I'd like to test with a few more devices before doing that.

class AVRBase(object):
    name = "INVALID DEVICE"
    signature = [0xFF, 0xFF, 0xFF]
    timeout = 200
    stabdelay = 100
    cmdexedelay = 25
    synchloops = 32
    bytedelay = 0
    pollindex = 3
    pollvalue = 0x53
    predelay = 1
    postdelay = 1
    pollmethod = 1

    memtypes = {
       "flash":{"offset":0, "size":32768, "pagesize":128},
       "eeprom":{"offset":0, "size":1024, "pagesize":4}
     }

class ATMega328P(AVRBase):
    name = "ATMega328P"
    signature = [0x1e, 0x95, 0x0f]
    memtypes = {
       "flash":{"offset":0, "size":32768, "pagesize":128},
       "eeprom":{"offset":0, "size":1024, "pagesize":4}
     }

class ATMega328(AVRBase):
    name = "ATMega328"
    signature = [0x1e, 0x95, 0x14]
    memtypes = {
       "flash":{"offset":0, "size":32768, "pagesize":128},
       "eeprom":{"offset":0, "size":1024, "pagesize":4}
     }


class ATMega168PA(AVRBase):
    name = "ATMega168PA"
    signature = [0x1e, 0x94, 0x0B]
    memtypes = {
       "flash":{"offset":0, "size":16384, "pagesize":128},
       "eeprom":{"offset":0, "size":512, "pagesize":4}
     }

class ATMega168A(AVRBase):
    name = "ATMega168A"
    signature = [0x1e, 0x94, 0x06]
    memtypes = {
       "flash":{"offset":0, "size":16384, "pagesize":128},
       "eeprom":{"offset":0, "size":512, "pagesize":4}
     }

class ATMega88PA(AVRBase):
    name = "ATMega88PA"
    signature = [0x1e, 0x93, 0x0F]
    memtypes = {
       "flash":{"offset":0, "size":8192, "pagesize":64},
       "eeprom":{"offset":0, "size":512, "pagesize":4}
     }

class ATMega88A(AVRBase):
    name = "ATMega88A"
    signature = [0x1e, 0x93, 0x0A]
    memtypes = {
       "flash":{"offset":0, "size":8192, "pagesize":64},
       "eeprom":{"offset":0, "size":512, "pagesize":4}
     }

class ATMega48PA(AVRBase):
    signature = [0x1e, 0x92, 0x0A]
    name = "ATMega48PA"
    memtypes = {
       "flash":{"offset":0, "size":4096, "pagesize":64},
       "eeprom":{"offset":0, "size":256, "pagesize":4}
     }

class ATMega48A(AVRBase):
    signature = [0x1e, 0x92, 0x05]
    name = "ATMega48A"
    memtypes = {
       "flash":{"offset":0, "size":4096, "pagesize":64},
       "eeprom":{"offset":0, "size":256, "pagesize":4}
     }

class ATMega128RFA1(AVRBase):
        signature = [0x1e, 0xA7, 0x01]
        name = "ATMega128RFA1"
        memtypes = {
            "flash": {"offset": 0, "size": 131072, "pagesize": 256},
            "eeprom": {"offset": 0, "size": 4096, "pagesize": 8}
        }

supported_avr = [ATMega328P(), ATMega328(), ATMega168A(), ATMega168PA(), ATMega88A(), ATMega88PA(), ATMega48A(), ATMega48PA(), ATMega128RFA1()]

class AVRISP(object):

    CMD_AVR_PROGRAM = 0x21
    """USB Command for AVR Programming (ISP Interface)"""

    ISP_CMD_SIGN_ON = 0x01
    ISP_CMD_SET_PARAMETER = 0x02
    ISP_CMD_GET_PARAMETER = 0x03
    ISP_CMD_OSCCAL = 0x05
    ISP_CMD_LOAD_ADDRESS = 0x06
    ISP_CMD_FIRMWARE_UPGRADE = 0x07
    ISP_CMD_RESET_PROTECTION = 0x0A
    ISP_CMD_ENTER_PROGMODE_ISP = 0x10
    ISP_CMD_LEAVE_PROGMODE_ISP = 0x11
    ISP_CMD_CHIP_ERASE_ISP = 0x12
    ISP_CMD_PROGRAM_FLASH_ISP = 0x13
    ISP_CMD_READ_FLASH_ISP = 0x14
    ISP_CMD_PROGRAM_EEPROM_ISP = 0x15
    ISP_CMD_READ_EEPROM_ISP = 0x16
    ISP_CMD_PROGRAM_FUSE_ISP = 0x17
    ISP_CMD_READ_FUSE_ISP = 0x18
    ISP_CMD_PROGRAM_LOCK_ISP = 0x19
    ISP_CMD_READ_LOCK_ISP = 0x1A
    ISP_CMD_READ_SIGNATURE_ISP = 0x1B
    ISP_CMD_READ_OSCCAL_ISP = 0x1C
    ISP_CMD_SPI_MULTI = 0x1D
    ISP_CMD_XPROG = 0x50
    ISP_CMD_XPROG_SETMODE = 0x51

    # next 3 are CW-Lite specific, not part of regular protocol
    ISP_CMD_GET_STATUS = 0x20
    ISP_CMD_SET_RAMBUF = 0x21
    ISP_CMD_GET_RAMBUF = 0x22

    ISP_STATUS_CMD_OK = 0x00
    ISP_STATUS_CMD_TOUT = 0x80
    ISP_STATUS_RDY_BSY_TOUT = 0x81
    ISP_STATUS_SET_PARAM_MISSING = 0x82
    ISP_STATUS_CMD_FAILED = 0xC0
    ISP_STATUS_CMD_UNKNOWN = 0xC9
    ISP_STATUS_ISP_READY = 0x00
    ISP_STATUS_CONN_FAIL_MOSI = 0x01
    ISP_STATUS_CONN_FAIL_RST = 0x02
    ISP_STATUS_CONN_FAIL_SCK = 0x04
    ISP_STATUS_TGT_NOT_DETECTED = 0x10
    ISP_STATUS_TGT_REVERSE_INSERTED = 0x20

    STATUS_TEXT_DESC = {
        0x00:"OK",
        0x80:"Timeout",
        0x81:"Busy Timeout",
        0x82:"Parameter Missing",
        0xC0:"Command Failed",
        0xC9:"Command Unknown",
        0x10:"Target not detected"
    };

    ISP_PARAM_BUILD_NUMBER_LOW = 0x80
    ISP_PARAM_BUILD_NUMBER_HIGH = 0x81
    ISP_PARAM_HW_VER = 0x90
    ISP_PARAM_SW_MAJOR = 0x91
    ISP_PARAM_SW_MINOR = 0x92
    ISP_PARAM_VTARGET = 0x94
    ISP_PARAM_SCK_DURATION = 0x98
    ISP_PARAM_RESET_POLARITY = 0x9E
    ISP_PARAM_STATUS_TGT_CONN = 0xA1
    ISP_PARAM_DISCHARGEDELAY = 0xA4

    CMD_SAM3U_CFG = 0x22

    MAX_BUFFER_SIZE = 256

    def __init__(self, usb, timeout = 200):
        self._usb = usb
        self._timeout = timeout

        # TEMP, user should select correct one
        self._chip = supported_avr[0]

    def _avrDoWrite(self, cmd, data=[], checkStatus=True):
        """
        Send a command to the AVR-ISP programming interface, optionally check if command executed OK, and if not
        raise IOError()
        """

        # windex selects interface
        self._usb.usbdev().ctrl_transfer(0x41, self.CMD_AVR_PROGRAM, cmd, 0, data, timeout=self._timeout)

        # Check status
        if checkStatus:
            status = self._avrDoRead(cmd=0x0020, dlen=2)
            if status[1] != 0x00:
                status_txt = "0x%02x"%status[1]
                if status[1] in self.STATUS_TEXT_DESC:
                    status_txt = status_txt + " (%s)"%self.STATUS_TEXT_DESC[status[1]]

                raise IOError("AVR-ISP Command 0x%02x failed: err=%s" % (status[0],status_txt))

    def _avrDoRead(self, cmd, dlen=1):
        """
        Read the result of some command.
        """
        # windex selects interface, set to 0
        return self._usb.usbdev().ctrl_transfer(0xC1, self.CMD_AVR_PROGRAM, cmd, 0, dlen, timeout=self._timeout)

    def enableISP(self, status):
        """
        Enable or disable ISP interface and prepare AVR chip for new status, either entering or exiting
        programming mode.

        Raises IOError() if an error occurs (such as no chip found).
        """

        if status:
            Util.chipwhisperer_extra.cwEXTRA.setAVRISPMode(status)
            time.sleep(0.1)
            self._avrDoWrite(self.ISP_CMD_ENTER_PROGMODE_ISP, [self._chip.timeout, self._chip.stabdelay, self._chip.cmdexedelay, self._chip.synchloops,
                                                               self._chip.bytedelay, self._chip.pollvalue, self._chip.pollindex, 0xAC, 0x53, 0, 0])
        else:
            try:
                self._avrDoWrite(self.ISP_CMD_LEAVE_PROGMODE_ISP, [self._chip.predelay, self._chip.postdelay])
            except:
                # Always disable ISP mode lines!
                Util.chipwhisperer_extra.cwEXTRA.setAVRISPMode(status)
                raise
            Util.chipwhisperer_extra.cwEXTRA.setAVRISPMode(status)

    def _readFuseLockSig(self, cmd, cmds, respindx=4):
        if len(cmds) != 4:
            raise ValueError("Commands must be 4 bytes")

        totalcmd = [respindx]
        totalcmd.extend(cmds)
        self._avrDoWrite(cmd, totalcmd, checkStatus=False)
        status = self._avrDoRead(cmd=0x0020, dlen=4)
        if status[1] != 0x00:
            raise IOError("AVR-ISP Command %x failed: err=%x" % (status[0], status[1]))
        return status[2]

    def readSignature(self):
        """
        Read the 3-byte signature sequence from the AVR.

        Returns:
            list.
        Raises:
            IOError
        """
        sigbytes = [0, 0, 0]
        for i in range(0, 3):
            sigbytes[i] = self._readFuseLockSig(self.ISP_CMD_READ_SIGNATURE_ISP, [0x30, 0x00, i, 0x00], 4)
        return sigbytes

    def readFuse(self, fusename):
        """
        Read single byte of fuse memory

        Args:
            fusename (str): One of 'low', 'high', or 'extended'
        Returns:
            Fuse value
        Raises:
            IOError, ValueError
        """

        if fusename == "low":
            command = [0x50, 0x00, 0x00, 0x00]
        elif fusename == "high":
            command = [0x58, 0x08, 0x00, 0x00]
        elif fusename == "extended":
            command = [0x50, 0x08, 0x00, 0x00]
        else:
            raise ValueError("Invalid fusename: %s" % fusename)

        return self._readFuseLockSig(self.ISP_CMD_READ_FUSE_ISP, command, 4)

    def readLock(self, lock):
        """
        Read lock byte and return value.
        """
        return self._readFuseLockSig(self.ISP_CMD_READ_LOCK_ISP, [0xAC, 0xE0, 0x00, 0x00], 4)


    def writeFuse(self, fusename, value):
        """
        Write single byte of fuse memory

        Args:
            fusename (str): One of 'low', 'high', or 'extended'
            value (byte): Value to write
        Raises:
            IOError, ValueError
        """
        if fusename == "low":
            command = [0xAC, 0xA0, 0x00, value]
        elif fusename == "high":
            command = [0xAC, 0xA8, 0x00, value]
        elif fusename == "extended":
            command = [0xAC, 0xA4, 0x00, value]
        else:
            raise ValueError("Invalid fusename: %s" % fusename)
        self._avrDoWrite(self.ISP_CMD_PROGRAM_FUSE_ISP, data=command)

    def writeLock(self, value):
        """
        Write lock byte value.
        """
        self._avrDoWrite(self.ISP_CMD_PROGRAM_LOCK_ISP, data=[0xAC, 0xE0, 0x00, value])

    def eraseChip(self):
        """
        Perform chip erase
        """
        # the AC 80 00 00 section comes from datasheet for chip erase, not sure if different?
        self._avrDoWrite(self.ISP_CMD_CHIP_ERASE_ISP,
                         [25,  # erase delay in mS
                         0,  # poll method
                         0xAC, 0x80, 0x00, 0x00  # erase command
                         ])

    def readMemory(self, addr, dlen, memname="flash"):
        """
        Read memory such as FLASH or EEPROM. Can specify an arbitrary length of data.

        Args:
            addr (int): Address to read from.
            dlen (in): How much data to read.
        Kwargs:
            memname (str): Type of memory, such as "flash" or "eeprom". Defaults to 'flash', but
                           this will often work for other memory types.
        Returns:
            list.
        Raises:
            IOError
        """
        memread = 0
        endptsize = 64
        # start = 0
        # end = endptsize

        self._avrDoWrite(self.ISP_CMD_LOAD_ADDRESS, data=packuint32(addr))

        membuf = []

        while memread < dlen:

            # Read into internal buffer
            ramreadln = dlen - memread

            # Check if maximum size for internal buffer
            if ramreadln > self.MAX_BUFFER_SIZE:
                ramreadln = self.MAX_BUFFER_SIZE

            self._avrDoWrite(self.ISP_CMD_READ_FLASH_ISP, data=[0x00, 0x01, 0x20])

            epread = 0

            # First we need to fill the page buffer in the USB Interface using smaller transactions
            while epread < ramreadln:

                epreadln = ramreadln - epread
                if epreadln > endptsize:
                    epreadln = endptsize

                # Read data out progressively
                membuf.extend(self._avrDoRead(self.ISP_CMD_GET_RAMBUF | (epread << 8), dlen=epreadln))

                # print epread

                epread += epreadln

            memread += ramreadln

        return membuf

    def writeMemory(self, addr, data, memname):
        """
        Write memory such as FLASH or EEPROM. Can specify an arbitrary length of data.

        Args:
            addr (int): Address to write at, should be page aligned if writing paged memory!
            data (list): Data to write.
            memname (str): Type of memory, such as "flash" or "eeprom".
        Raises:
            IOError
        """

        memspec = self._chip.memtypes[memname]

        memwritten = 0
        endptsize = 64
        start = 0
        end = endptsize
        pagesize = memspec["pagesize"]

        if addr % pagesize:
            print "WARNING: You appear to be writing to an address that is not page aligned, you will probably write the wrong data"

        self._avrDoWrite(self.ISP_CMD_LOAD_ADDRESS, data=[0, 0, 0, 0])

        while memwritten < len(data):

            epwritten = 0

            # First we need to fill the page buffer in the USB Interface using smaller transactions
            while epwritten < pagesize:

                # Check for less than full endpoint written
                if end > len(data):
                    end = len(data)

                # Get slice of data
                epdata = data[start:end]

                # print "%d %d %d" % (epwritten, len(epdata), memwritten)
                # Copy to USB interface buffer
                self._avrDoWrite(self.ISP_CMD_SET_RAMBUF | (epwritten << 8), data=epdata, checkStatus=False)

                epwritten += len(epdata)

                # Check for final write indicating we are done
                if end == len(data):
                    break

                start += endptsize
                end += endptsize


            # Copy internal buffer to final location (probably FLASH memory)
            # self._avrDoWrite(self.ISP_CMD_LOAD_ADDRESS, data=packuint32(addr + memwritten))
            infoblock = []
            infoblock.append(epwritten & 0xff)
            infoblock.append((epwritten >> 8) & 0xff)
            infoblock.append(0x41 | 0x80)  # Programming mode, enable flash page programming
            infoblock.append(6)  # 6mS delay for whatever it is here
            infoblock.extend([0x40, 0x4C, 0x20])  # Programming commands for doing stuff
            infoblock.extend([0xff, 0xff])  # We don't use value polling so don't care
            self._avrDoWrite(self.ISP_CMD_PROGRAM_FLASH_ISP, data=infoblock)

            memwritten += epwritten

    def setChip(self, chiptype):
        self._chip = chiptype


    def enableSlowClock(self, enabled):
        if enabled:
            self._usb.usbdev().ctrl_transfer(0x41, self.CMD_SAM3U_CFG, 0x01, 0, timeout=self._timeout)
        else:
            self._usb.usbdev().ctrl_transfer(0x41, self.CMD_SAM3U_CFG, 0x02, 0, timeout=self._timeout)
# -*- coding: utf-8 -*-
#
# Copyright (c) 2014-2021, NewAE Technology Inc
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
import logging, os, time
from datetime import datetime
from .naeusb import packuint32
from chipwhisperer.capture.utils.programming_files import FileReader
from chipwhisperer.logging import *

XMEGAMEM_TYPE_APP = 1
XMEGAMEM_TYPE_BOOT = 2
XMEGAMEM_TYPE_EEPROM = 3
XMEGAMEM_TYPE_FUSE = 4
XMEGAMEM_TYPE_LOCKBITS = 5
XMEGAMEM_TYPE_USERSIG = 6
XMEGAMEM_TYPE_FACTORY_CALIBRATION = 7

# NOTE: These objects are currently manually maintained. Eventually it will be automatically created
#      from avrdude.conf, but I'd like to test with a few more devices before doing that.

class XMEGADummy(object):
    signature = [0x00, 0x00, 0x00]
    name = "Unknown XMEGA device"

    memtypes = {
        "signature":{"offset":0x1000090, "size":3},
    }

class XMEGA16A4(object):
    signature = [0x1e, 0x94, 0x41]
    name = "XMEGA16A4"

    memtypes = {
       "signature":{"offset":0x1000090, "size":3},
       "flash":{"offset":0x0800000, "size":0x00005000, "pagesize":0x100, "type":XMEGAMEM_TYPE_APP},
       "boot":{"offset":0x0805000, "size":0x00001000, "pagesize":0x100, "type":XMEGAMEM_TYPE_BOOT},
       "eeprom":{"offset":0x08c0000, "size":0x0400, "pagesize":0x20, "readsize":0x100, "type":XMEGAMEM_TYPE_EEPROM},
       "fuse1":{"offset":0x8f0021, "size":1},
       "fuse2":{"offset":0x8f0022, "size":1},
       "fuse4":{"offset":0x8f0024, "size":1},
       "fuse5":{"offset":0x8f0025, "size":1},
     }

class XMEGA128A4U(object):
    signature = [0x1e, 0x97, 0x46]
    name = "XMEGA128A4U"

    memtypes = {
       "signature":{"offset":0x1000090, "size":3},
       "flash":{"offset":0x0800000, "size":0x00022000, "pagesize":0x100, "type":XMEGAMEM_TYPE_APP},
       "boot":{"offset":0x0822000, "size":0x00002000, "pagesize":0x100, "type":XMEGAMEM_TYPE_BOOT},
       "eeprom":{"offset":0x08c0000, "size":0x0800, "pagesize":0x20, "readsize":0x100, "type":XMEGAMEM_TYPE_EEPROM},
       "fuse1":{"offset":0x8f0021, "size":1},
       "fuse2":{"offset":0x8f0022, "size":1},
       "fuse4":{"offset":0x8f0024, "size":1},
       "fuse5":{"offset":0x8f0025, "size":1},
     }

class XMEGA128D4(object):
    signature = [0x1e, 0x97, 0x47]
    name = "XMEGA128D4"

    memtypes = {
       "signature":{"offset":0x1000090, "size":3},
       "flash":{"offset":0x0800000, "size":0x00022000, "pagesize":0x100, "type":XMEGAMEM_TYPE_APP},
       "boot":{"offset":0x0822000, "size":0x00002000, "pagesize":0x100, "type":XMEGAMEM_TYPE_BOOT},
       "eeprom":{"offset":0x08c0000, "size":0x0800, "pagesize":0x20, "readsize":0x100, "type":XMEGAMEM_TYPE_EEPROM},
       "fuse1":{"offset":0x8f0021, "size":1},
       "fuse2":{"offset":0x8f0022, "size":1},
       "fuse4":{"offset":0x8f0024, "size":1},
       "fuse5":{"offset":0x8f0025, "size":1},
     }

class XMEGA128A3U(object):
    signature = [0x1e, 0x97, 0x42]
    name = "XMEGA128A3U"

    memtypes = {
       "signature":{"offset":0x1000090, "size":3},
       "flash":{"offset":0x0800000, "size":0x00022000, "pagesize":0x100, "type":XMEGAMEM_TYPE_APP},
       "boot":{"offset":0x0822000, "size":0x00002000, "pagesize":0x100, "type":XMEGAMEM_TYPE_BOOT},
       "eeprom":{"offset":0x08c0000, "size":0x0800, "pagesize":0x20, "readsize":0x100, "type":XMEGAMEM_TYPE_EEPROM},
       "fuse1":{"offset":0x8f0021, "size":1},
       "fuse2":{"offset":0x8f0022, "size":1},
       "fuse4":{"offset":0x8f0024, "size":1},
       "fuse5":{"offset":0x8f0025, "size":1},
     }

supported_xmega = [XMEGA128A4U(), XMEGA128D4(), XMEGA16A4(), XMEGA128A3U()]

def print_fun(s):
    print(s)

class XMEGAPDI(object):
    """
    Class for programming an XMEGA device using the NAEUSB Firmware in the ChipWhisperer 1173/1200.
    """

    CMD_XMEGA_PROGRAM = 0x20
    """USB Command for XMEGA Programming (PDI Interface)"""

    XPROG_CMD_ENTER_PROGMODE = 0x01
    """PDI Command: Enable PDI, enter programming mode"""

    XPROG_CMD_LEAVE_PROGMODE = 0x02
    """PDI Command: Leave programming mode, disable PDI"""

    XPROG_CMD_ERASE = 0x03
    """PDI Command: Erase command, specify what to erase as well"""

    XPROG_CMD_WRITE_MEM = 0x04
    """PDI Command: Write data from RAMBUF to XMEGA memory"""

    XPROG_CMD_READ_MEM = 0x05
    """PDI Command: Read data from XMEGA memory to RAMBUF"""

    XPROG_CMD_CRC = 0x06
    """PDI Command: Perform CRC of Memory"""

    XPROG_CMD_SET_PARAM = 0x07
    """PDI Command: Set programming parameters"""

    XPROG_GET_STATUS = 0x20
    """PDI Command: Get status of last command"""

    XPROG_GET_RAMBUF = 0x21
    """PDI Command: Read data from RAMBUF"""

    XPROG_SET_RAMBUF = 0x22
    """PDI Command: Write data to RAMBUF"""

    XPROG_WRITE_BOOT = 0x2C

    XPROG_PARAM_TIMEOUT = 0x08
    """Parameter: Timeout"""


    # Chip Erase Types
    XPROG_ERASE_CHIP = 1
    """Erase type: Chip"""

    XPROG_ERASE_APP = 2
    """Erase type: Application Section"""

    XPROG_ERASE_BOOT = 3
    """Erase bootloader"""

    # Maximum size of buffer in our system
    MAX_BUFFER_SIZE = 256

    def __init__(self, usb, timeout=200):
        """
        Set the USB communications instance.
        """

        self._usb = usb
        self._timeout = timeout
        self._pdienabled = False
        self._chip = XMEGADummy()
        self.lastFlashedFile = "unknown"

#### HIGH LEVEL FUNCTIONS
    def find(self):
        self.setParamTimeout(100)
        self.enablePDI(True)

        # Read signature bytes
        data = self.readMemory(0x01000090, 3, "signature")

        # Check if it's one we know about?
        for t in supported_xmega:
            if ((data[0] == t.signature[0]) and
                (data[1] == t.signature[1]) and
                (data[2] == t.signature[2])):

                target_logger.debug("Detected known XMEGA: %s"%t.name)
                self.setChip(t)
                return data, t

        #No known chip found?
        target_logger.debug("Detected unknown XMEGA: %02x %02x %02x"%(data[0], data[1], data[2]))
        return data, None

    def erase(self, memtype="chip"):
        if memtype == "app":
            self.eraseApp()
        elif memtype == "chip":
            self.eraseChip()
        elif memtype == "bootloader":
            self.eraseBootloader()
        else:
            raise ValueError("Invalid memtype: %s" % memtype)


    def autoProgram(self, hexfile, erase=True, verify=True, logfunc=print_fun, waitfunc=None):
        """Helper funciton for GUI, auto-programs XMEGA device while printing messages to different options. Returns true/false."""

        status = "FAILED"

        fname = hexfile
        if logfunc: logfunc("***Starting FLASH program process at %s***" % datetime.now().strftime('%H:%M:%S'))
        if waitfunc: waitfunc()
        if os.path.isfile(fname):
            if logfunc: logfunc("File %s last changed on %s" % (fname, time.ctime(os.path.getmtime(fname))))

            try:
                if logfunc: logfunc("Entering Programming Mode")
                if waitfunc: waitfunc()
                self.find()

                if erase:
                    try:
                        self.erase()
                    except IOError:
                        if logfunc: logfunc("**chip-erase timeout, erasing application only**")
                        if waitfunc: waitfunc()
                        self.enablePDI(False)
                        self.enablePDI(True)
                        self.erase("app")

                if waitfunc: waitfunc()
                self.program(hexfile, memtype="flash", verify=verify, logfunc=logfunc, waitfunc=waitfunc)
                if waitfunc: waitfunc()
                if logfunc: logfunc("Exiting programming mode")
                self.close()
                if waitfunc: waitfunc()

                status = "SUCCEEDED"

            except IOError as e:
                if logfunc: logfunc("FAILED: %s" % str(e))
                try:
                    self.close()
                except IOError:
                    pass

        else:
            if logfunc: logfunc("%s does not appear to be a file, check path" % fname)

        if logfunc: logfunc("***FLASH Program %s at %s***" % (status, datetime.now().strftime('%H:%M:%S')))

        return status == "SUCCEEDED"

    def program(self, filename, memtype="flash", verify=True, logfunc=print_fun, waitfunc=None):
        """Programs memory type, dealing with opening filename as either .hex or .bin file"""
        self.lastFlashedFile = filename

        fdata, fsize = FileReader(filename)

        startaddr = self._chip.memtypes[memtype]["offset"]
        maxsize = self._chip.memtypes[memtype]["size"]

        if fsize > maxsize:
            raise IOError("File %s appears to be %d bytes, larger than %s size of %d" % (filename, fsize, memtype, maxsize))

        logfunc("XMEGA Programming %s..." % memtype)
        if waitfunc: waitfunc()
        self.writeMemory(startaddr, fdata, memtype)  # , erasePage=True

        logfunc("XMEGA Reading %s..." % memtype)
        if waitfunc: waitfunc()
        # Do verify run
        rdata = self.readMemory(startaddr, len(fdata), memtype)

        for i in range(0, len(fdata)):
            if fdata[i] != rdata[i]:
                raise IOError("Verify failed at 0x%04x, %x != %x" % (i, fdata[i], rdata[i]))

        logfunc("Verified %s OK, %d bytes" % (memtype, fsize))

#### LOW LEVEL FUNCTIONS

    def close(self):
        self.enablePDI(False)


    def setParamTimeout(self, timeoutMS):
        """
        Set timeout for executing PDI commands, sets timeout both on NAEUSB chip and host-side USB API.
        """

        self._timeout = timeoutMS + 50
        timeoutticks = int((float(timeoutMS) / 1000.0) * 2500.0)
        pload = [self.XPROG_PARAM_TIMEOUT]
        pload.extend(packuint32(timeoutticks))
        self._xmegaDoWrite(self.XPROG_CMD_SET_PARAM, pload)


    def enablePDI(self, status):
        """
        Enable or disable PDI interface and prepare XMEGA chip for new status, either entering or exiting
        programming mode.

        Raises IOError() if an error occurs (such as no chip found).
        """

        self._pdienabled = False

        if status:
            # self._xmegaDoWrite(self.XPROG_CMD_LEAVE_PROGMODE)
            self._xmegaDoWrite(self.XPROG_CMD_ENTER_PROGMODE)
            self._pdienabled = True
        else:
            self._xmegaDoWrite(self.XPROG_CMD_LEAVE_PROGMODE)

    def validate_mode(self):
        if not self._pdienabled:
            raise IOError("Enable PDI mode first")

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

        self.validate_mode()

        memspec = self._chip.memtypes[memname]

        memread = 0
        endptsize = 64
        # start = 0
        # end = endptsize

        if "readsize" in list(memspec.keys()):
            readsize = memspec["readsize"]
        elif "pagesize" in list(memspec.keys()):
            readsize = memspec["pagesize"]
        else:
            readsize = dlen

        membuf = []

        while memread < dlen:

            #Read into internal buffer
            ramreadln = dlen - memread

            # Check if maximum size for memory type
            if ramreadln > readsize:
                ramreadln = readsize

            # Check if maximum size for internal buffer
            if ramreadln > self.MAX_BUFFER_SIZE:
                ramreadln = self.MAX_BUFFER_SIZE

            infoblock = [0]  # memspec["type"]
            infoblock.extend(packuint32(addr + memread))
            infoblock.append(ramreadln & 0xff)
            infoblock.append((ramreadln >> 8) & 0xff)

            self._xmegaDoWrite(self.XPROG_CMD_READ_MEM, data=infoblock)

            epread = 0

            # First we need to fill the page buffer in the USB Interface using smaller transactions
            while epread < ramreadln:

                epreadln = ramreadln - epread
                if epreadln > endptsize:
                    epreadln = endptsize

                # Read data out progressively
                membuf.extend(self._xmegaDoRead(self.XPROG_GET_RAMBUF | (epread << 8), dlen=epreadln))

                # print epread

                epread += epreadln

            memread += ramreadln

        return membuf

    def writeMemory(self, addr, data, memname, erasePage=False, programPage=True):
        """
        Write memory such as FLASH or EEPROM. Can specify an arbitrary length of data.

        Args:
            addr (int): Address to write at, should be page aligned if writing paged memory!
            data (list): Data to write.
            memname (str): Type of memory, such as "flash" or "eeprom".
        Kwargs:
            erasePage (bool): Should we perform a page erase before writing? Defaults to FALSE.
            programPage (bool): Should we perform a page write once memory copied? Defaults to TRUE.
                                If writing internal RAM set this to FALSE, but for writing FLASH/
                                EEPROM leave as TRUE.
        Raises:
            IOError
        """

        self.validate_mode()

        PAGEMODE_WRITE = (1 << 1)
        PAGEMODE_ERASE = (1 << 0)

        memspec = self._chip.memtypes[memname]

        memwritten = 0
        endptsize = 64
        start = 0
        end = endptsize
        pagesize = memspec["pagesize"]

        if addr % pagesize:
            target_logger.warning('You appear to be writing to an address that is not page aligned, you will probably write the wrong data')

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
                self._xmegaDoWrite(self.XPROG_SET_RAMBUF | (epwritten << 8), data=epdata, checkStatus=False)

                epwritten += len(epdata)

                # Check for final write indicating we are done
                if end == len(data):
                    break

                start += endptsize
                end += endptsize

            # Copy internal buffer to final location (probably FLASH memory)

            if not ("type" in list(memspec.keys())):
                raise IOError("Write on memory type that doesn't have 'type', probably read-only?")

            # Do write into memory type
            infoblock = [memspec["type"], 0]

            if programPage:
                infoblock[1] |= PAGEMODE_WRITE

            if erasePage:
                infoblock[1] |= PAGEMODE_ERASE

            infoblock.extend(packuint32(addr + memwritten))
            infoblock.append(epwritten & 0xff)
            infoblock.append((epwritten >> 8) & 0xff)

            # print "%x" % (addr + memwritten)
            # print epwritten

            self._xmegaDoWrite(self.XPROG_CMD_WRITE_MEM, data=infoblock)

            memwritten += epwritten

    def eraseChip(self):
        self.validate_mode()
        self._xmegaDoWrite(self.XPROG_CMD_ERASE, data=[self.XPROG_ERASE_CHIP, 0, 0, 0, 0])

    def eraseBootloader(self):
        self.validate_mode()
        self._xmegaDoWrite(self.XPROG_CMD_ERASE, data=[self.XPROG_ERASE_BOOT, 0, 0, 0, 0])

    def eraseApp(self):
        self.validate_mode()
        self._xmegaDoWrite(self.XPROG_CMD_ERASE, data=[self.XPROG_ERASE_APP, 0, 0, 0, 0])

    def setChip(self, chiptype):
        self._chip = chiptype


#### INTERNAL FUNCTIONS

    def _xmegaDoWrite(self, cmd, data=[], checkStatus=True):
        """
        Send a command to the PDI programming interface, optionally check if command executed OK, and if not
        raise IOError()
        """

        # windex selects interface
        self._usb.sendCtrl(self.CMD_XMEGA_PROGRAM, cmd, data)

        # Check status
        if checkStatus:
            status = self._xmegaDoRead(cmd=0x0020, dlen=3)
            if status[1] != 0x00:
                raise IOError("XMEGA Command %x failed: err=%x, timeout=%d" % (status[0], status[1], status[2]))

    def _xmegaDoRead(self, cmd, dlen=1):
        """
        Read the result of some command.
        """
        # windex selects interface, set to 0
        return self._usb.readCtrl(self.CMD_XMEGA_PROGRAM, cmd, dlen)
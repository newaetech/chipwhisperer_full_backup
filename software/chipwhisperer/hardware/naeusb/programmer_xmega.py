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

from naeusb import packuint32

XMEGAMEM_TYPE_APP = 1
XMEGAMEM_TYPE_BOOT = 2
XMEGAMEM_TYPE_EEPROM = 3
XMEGAMEM_TYPE_FUSE = 4
XMEGAMEM_TYPE_LOCKBITS = 5
XMEGAMEM_TYPE_USERSIG = 6
XMEGAMEM_TYPE_FACTORY_CALIBRATION = 7

# NOTE: These objects are currently manually maintained. Eventually it will be automatically created
#      from avrdude.conf, but I'd like to test with a few more devices before doing that.

class XMEGA128A4U(object):
    signature = [0x1e, 0x97, 0x46]
    name = "XMEGA128A4U"

    memtypes = {
       "signature":{"offset":0x1000090, "size":3},
       "flash":{"offset":0x0800000, "size":0x00022000, "pagesize":0x100, "type":XMEGAMEM_TYPE_APP},
       "eeprom":{"offset":0x08c0000, "size":0x0800, "pagesize":0x20, "readsize":0x100, "type":XMEGAMEM_TYPE_EEPROM},
       "fuse1":{"offset":0x8f0021, "size":1},
       "fuse2":{"offset":0x8f0022, "size":1},
       "fuse4":{"offset":0x8f0024, "size":1},
       "fuse5":{"offset":0x8f0025, "size":1},
     }

class XMEGA128D4U(object):
    signature = [0x1e, 0x97, 0x47]
    name = "XMEGA128D4U"

    memtypes = {
       "signature":{"offset":0x1000090, "size":3},
       "flash":{"offset":0x0800000, "size":0x00022000, "pagesize":0x100, "type":XMEGAMEM_TYPE_APP},
       "eeprom":{"offset":0x08c0000, "size":0x0800, "pagesize":0x20, "readsize":0x100, "type":XMEGAMEM_TYPE_EEPROM},
       "fuse1":{"offset":0x8f0021, "size":1},
       "fuse2":{"offset":0x8f0022, "size":1},
       "fuse4":{"offset":0x8f0024, "size":1},
       "fuse5":{"offset":0x8f0025, "size":1},
     }


supported_xmega = [XMEGA128A4U(), XMEGA128D4U()]

class XMEGAPDI(object):
    """
    Class for programming an XMEGA device using the NAEUSB Firmware in the ChipWhisperer-Lite
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

    XPROG_PARAM_TIMEOUT = 0x08
    """Parameter: Timeout"""


    # Chip Erase Types
    XPROG_ERASE_CHIP = 1
    """Erase type: Chip"""

    XPROG_ERASE_APP = 2
    """Erase type: Application Section"""

    # Maximum size of buffer in our system
    MAX_BUFFER_SIZE = 256

    def __init__(self, usb, timeout=200):
        """
        Set the USB communications instance.
        """

        self._usb = usb
        self._timeout = timeout

    def _xmegaDoWrite(self, cmd, data=[], checkStatus=True):
        """
        Send a command to the PDI programming interface, optionally check if command executed OK, and if not
        raise IOError()
        """

        # windex selects interface
        self._usb.usbdev().ctrl_transfer(0x41, self.CMD_XMEGA_PROGRAM, cmd, 0, data, timeout=self._timeout)

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
        return self._usb.usbdev().ctrl_transfer(0xC1, self.CMD_XMEGA_PROGRAM, cmd, 0, dlen, timeout=self._timeout)

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

        if status:
            # self._xmegaDoWrite(self.XPROG_CMD_LEAVE_PROGMODE)
            self._xmegaDoWrite(self.XPROG_CMD_ENTER_PROGMODE)
        else:
            self._xmegaDoWrite(self.XPROG_CMD_LEAVE_PROGMODE)

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

        memspec = self._chip.memtypes[memname]

        memread = 0
        endptsize = 64
        # start = 0
        # end = endptsize

        if "readsize" in memspec.keys():
            readsize = memspec["readsize"]
        elif "pagesize" in memspec.keys():
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

        PAGEMODE_WRITE = (1 << 1)
        PAGEMODE_ERASE = (1 << 0)

        memspec = self._chip.memtypes[memname]

        memwritten = 0
        endptsize = 64
        start = 0
        end = endptsize
        pagesize = memspec["pagesize"]

        if addr % pagesize:
            print "WARNING: You appear to be writing to an address that is not page aligned, you will probably write the wrong data"

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

            if not ("type" in memspec.keys()):
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
        self._xmegaDoWrite(self.XPROG_CMD_ERASE, data=[self.XPROG_ERASE_CHIP, 0, 0, 0, 0])

    def eraseApp(self):
        self._xmegaDoWrite(self.XPROG_CMD_ERASE, data=[self.XPROG_ERASE_APP, 0, 0, 0, 0])

    def setChip(self, chiptype):
        self._chip = chiptype
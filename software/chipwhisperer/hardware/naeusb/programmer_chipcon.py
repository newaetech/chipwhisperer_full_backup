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

# WARNING: This ChipCon implementation is based on a quick-n-dirty port
# of Travis Goodspeed's GOODFET project. It's only designed to be able to perform the
# following: (1) chip erase, (2) read XDATA, (3) read IRAM. This is occasionally useful
# for validating SRAM contents of CC devices.

class ChipConProgrammer(object):

    CMD_CC_PROGRAM = 0x23

    #Goodfet basic commands
    CC_READ  = 0x00
    CC_WRITE = 0x01
    CC_PEEK  = 0x02
    CC_POKE  = 0x03
    CC_SETUP = 0x10
    CC_START = 0x20
    CC_STOP  = 0x21

    #CHIPCON commands
    CC_CHIP_ERASE = 0x80
    CC_WR_CONFIG = 0x81
    CC_RD_CONFIG = 0x82
    CC_GET_PC = 0x83
    CC_READ_STATUS = 0x84
    CC_SET_HW_BRKPNT = 0x85
    CC_HALT = 0x86
    CC_RESUME = 0x87
    CC_DEBUG_INSTR = 0x88
    CC_STEP_INSTR = 0x89
    CC_STEP_REPLACE = 0x8a
    CC_GET_CHIP_ID = 0x8b
    #/CHIPCON macros
    CC_READ_CODE_MEMORY = 0x90
    CC_READ_XDATA_MEMORY = 0x91
    CC_WRITE_XDATA_MEMORY = 0x92
    CC_SET_PC = 0x93
    CC_CLOCK_INIT = 0x94
    CC_WRITE_FLASH_PAGE = 0x95
    CC_READ_FLASH_PAGE = 0x96
    CC_MASS_ERASE_FLASH = 0x97
    CC_PROGRAM_FLASH = 0x98
    CC_WIPEFLASHBUFFER = 0x99
    CC_LOCKCHIP = 0x9A
    #Extra commands (not in original)
    CC_SET_FLASH_WORD_SIZE = 0xA0
    CC_READBACK = 0xA1

    CC_STATUS_ERASED = 0x80
    CC_STATUS_PCONIDLE = 0x40
    CC_STATUS_CPUHALTED = 0x20
    CC_STATUS_PM0 = 0x10
    CC_STATUS_HALTSTATUS = 0x08
    CC_STATUS_LOCKED = 0x04
    CC_STATUS_OSCSTABLE = 0x02
    CC_STATUS_OVERFLOW = 0x01

    def __init__(self, usbdev, timeout=200):
        """
        Set the USB communications instance.
        """

        self._usbdev = usbdev
        self._timeout = timeout

    def _write(self, cmd, arg1=0, data=[]):
        self._usbdev.ctrl_transfer(0x41, self.CMD_CC_PROGRAM, cmd | (arg1 << 8), 0, data, timeout=self._timeout)

    def _read(self, cmd, arg1=0, dlen=1):
        return self._usbdev.ctrl_transfer(0xC1, self.CMD_CC_PROGRAM, cmd | (arg1 << 8), 0, dlen, timeout=self._timeout)


    def enableCC(self, enabled=True):
        if enabled:
            self._write(self.CC_SETUP)
            self._write(self.CC_START)
        else:
            self._write(self.CC_STOP)

    def readSignature(self):
        """Read Signature"""
        cid = self._read(self.CC_GET_CHIP_ID, dlen=2)
        return {'chipid':cid[0], 'rev':cid[1]}

    def eraseChip(self):
        self._write(self.CC_CHIP_ERASE)

    def readXData(self, addr, len=1):
        toread = len

        data = []

        for start in range(0, len, 32):
            ahigh = start & 0xff
            alow = (start >> 8) & 0xff

            if toread < 32:
                dlen = toread
            else:
                dlen = 32

            toread = toread - dlen

            self._write(self.CC_READ_XDATA_MEMORY, data=[dlen, 0, ahigh, alow])
            data.extend(self._read(self.CC_READBACK, dlen=dlen))

        return data

    def readIRam(self, startaddr=0, dlen=1):
        data = []
        for i in range(0, dlen):
            data.append(self._read(self.CC_PEEK, arg1=(startaddr + i), dlen=1)[0])
        return data

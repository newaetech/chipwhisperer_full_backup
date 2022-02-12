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


import time
import logging
from .naeusb import packuint32, NAEUSB
from ...logging import *

class FPGA(object):

    CMD_FPGA_STATUS = 0x15
    CMD_FPGA_PROGRAM = 0x16

    def __init__(self, usb: NAEUSB, timeout=200, prog_mask=0xA0):
        self.sendCtrl = usb.sendCtrl
        self.readCtrl = usb.readCtrl
        self._usb = usb
        self._timeout = timeout
        self._prog_mask = prog_mask

    def isFPGAProgrammed(self):
        """
        Return True/False about FPGA status
        """

        status = self.readCtrl(self.CMD_FPGA_STATUS, dlen=4)

        if status[0] & 0x01:
            return True
        else:
            return False

    def eraseFPGA(self):
        self.sendCtrl(self.CMD_FPGA_PROGRAM, self._prog_mask | 0x00)
        time.sleep(0.001)
        self.sendCtrl(self.CMD_FPGA_PROGRAM, self._prog_mask | 0x01)
        time.sleep(0.001)

    def FPGAProgram(self, bitstream=None, exceptOnDoneFailure=True, prog_speed=1E6, starting_offset=0x7C):
        """
        Program FPGA with a bitstream, or if not bitstream passed just erases FPGA
        """

        # Erase the FPGA by toggling PROGRAM pin, setup
        # NAEUSB chip for FPGA programming
        prog_data = []
        if self._usb.check_feature("VARIABLE_FPGA_PROG_SPEED"):
            naeusb_logger.info("Using prog speed of {}".format(int(prog_speed)))
            prog_data = packuint32(int(prog_speed))


        self.sendCtrl(self.CMD_FPGA_PROGRAM, self._prog_mask | 0x00, data=prog_data)
        time.sleep(0.001)
        self.sendCtrl(self.CMD_FPGA_PROGRAM, self._prog_mask | 0x01)

        time.sleep(0.001)

        # Download actual bitstream now if present
        if bitstream:
            # Run the download which should program FPGA
            self._FPGADownloadBitstream(bitstream, starting_offset=starting_offset)

            wait = 5
            while wait > 0:
                # Check the status a few times
                programStatus = self.isFPGAProgrammed()
                if programStatus:
                    break
                time.sleep(0.001)
                wait -= 1

            # Exit FPGA programming mode
            self.sendCtrl(self.CMD_FPGA_PROGRAM, self._prog_mask | 0x02)

            if programStatus == False and exceptOnDoneFailure:
                target_logger.error("FPGA programming failed. Typically either bad bitstream or prog speed too high (current {})".format(prog_speed))
                raise IOError("FPGA Done pin failed to go high, bad bitstream?", bitstream)

            return programStatus
        else:
            # No bitstream, exit programming mode
            self.sendCtrl(self.CMD_FPGA_PROGRAM, self._prog_mask | 0x02)
            return False

    def _FPGADownloadBitstream(self, fwFileLike, starting_offset=0x7C, ending_clock_bytes=32):
        """
        Performs actual bitstream download, do not call directly, call FPGAProgram
        """

        transactionBytes = 2048
        t0 = 0

        buffer_ = [None] * int(16 * 1024 * 1024 / transactionBytes)
        size = 0

        # Read entire thing in
        with fwFileLike as f:
            inputStream = f.read()
        streamCnt = 0

        # Might need a few extra CCLKs at end to finish off, and as written elsewhere this is done with DO=1
        # Perhaps micro should add these instead? For now this should be reliable enough (things worked even w/o this it seemed, so this is
        # a just in case item)
        inputStream += bytes([0xff] * ending_clock_bytes)

        inputStream = inputStream[starting_offset:]

        j = transactionBytes
        for i in range(0, len(buffer_)):
            if j != transactionBytes: break
            buffer_[i] = bytearray(inputStream[streamCnt:(streamCnt + transactionBytes)])
            streamCnt += transactionBytes

            j = len(buffer_[i])

            if j < transactionBytes and j % 64 == 0:
                j += 1
            size += j

            # if j < transactionBytes and j % 64 == 0:
            #    j += 1
            # size += j

            # print i,
            # print " ",
            # print len(buffer_)

        if size < 64 or size % 64 == 0:
            raise ValueError("Invalid file size: " + str(size))

        tries = 1

        while tries > 0:
            # self.resetFpga()
            try:
                # t0 = -Date().getTime()
                bs = 0
                cs = 0

                for i in range(0, len(buffer_)):
                    if i * transactionBytes >= size:
                        break
                    j = size - i * transactionBytes
                    if j > transactionBytes:
                        j = transactionBytes

                    self._usb.writeBulkEP(buffer_[i])

                    bs += j
                    for k in range(0, len(buffer_[i])):
                        cs = (cs + (buffer_[i][k] & 0xff)) & 0xff

                # self.getFpgaState()
                # if not self.fpgaConfigured:
                #    raise IOError("FPGA configuration failed: DONE pin does not go high (size=" + self.fpgaBytes + " ,  " + (bs - self.fpgaBytes) + " bytes got lost;  checksum=" + self.fpgaChecksum + " , should be " + cs + ";  INIT_B_HIST=" + self.fpgaInitB + ")")

                # if self.enableExtraFpgaConfigurationChecks:
                #    if self.fpgaBytes != 0 and self.fpgaBytes != bs:
                #        System.err.println("Warning: Possible FPGA configuration data loss: " + (bs - self.fpgaBytes) + " bytes got lost")
                #    if self.fpgaInitB != 222:
                #        System.err.println("Warning: Possible Bitstream CRC error: INIT_B_HIST=" + self.fpgaInitB)
                tries = 0
                # t0 += Date().getTime()
            except IOError as e:
                if tries > 1:
                    print(("Warning: " + str(e) + ": Retrying it ..."))
                else:
                    raise
            tries -= 1
        # time.sleep(0.1)
        return t0

        # def detectBitstreamBitOrder(self, buf):
        #    """ Determine what bit-order bitstream is in by looking for magic bytes """
        #
        #    i = 0
        #    while i < len(buf):
        #        if ((buf[i] & 255) == 0xaa) and ((buf[i + 1] & 255) == 0x99) and ((buf[i + 2] & 255) == 0x55) and ((buf[i + 3] & 255) == 0x66):
        #            return 1
        #        if ((buf[i] & 255) == 0x55) and ((buf[i + 1] & 255) == 0x99) and ((buf[i + 2] & 255) == 0xaa) and ((buf[i + 3] & 255) == 0x66):
        #            return 0
        #        i += 1
        #    print("Warning: Unable to determine bitstream bit order: no signature found")
        #    return 0

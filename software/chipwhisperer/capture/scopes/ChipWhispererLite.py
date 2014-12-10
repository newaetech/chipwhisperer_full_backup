# -*- coding: utf-8 -*-
#
# Copyright (c) 2014, NewAE Technology Inc
# All rights reserved.
#
# Authors: Colin O'Flynn
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
#=================================================


import usb.core
import usb.util
import time

def packuint32(data):
    return [data & 0xff, (data >> 8) & 0xff, (data >> 16) & 0xff, (data >> 24) & 0xff]

class XMEGAPDI(object):

    CMD_XMEGA_PROGRAM = 0x20

    XPROG_CMD_ENTER_PROGMODE = 0x01
    XPROG_CMD_LEAVE_PROGMODE = 0x02
    XPROG_CMD_ERASE = 0x03
    XPROG_CMD_WRITE_MEM = 0x04
    XPROG_CMD_READ_MEM = 0x05
    XPROG_CMD_CRC = 0x06
    XPROG_CMD_SET_PARAM = 0x07
    XPROG_GET_STATUS = 0x20
    XPROG_GET_RAMBUF = 0x21
    XPROG_SET_RAMBUF = 0x22

    def setUSB(self, usbdev, timeout=200):
        self._usbdev = usbdev
        self._timeout = timeout

    def _xmegaDoWrite(self, cmd, data=[], windex=0):
        self._usbdev.ctrl_transfer(0x41, self.CMD_XMEGA_PROGRAM, cmd, windex, data, timeout=self._timeout)

        # Check status
        status = self._xmegaDoRead(cmd=0x0020, dlen=2)
        if status[1] != 0x00:
            raise IOError("XMEGA Command failed: %d" % status[0])

    def _xmegaDoRead(self, cmd, dlen=1, windex=0):
        return self._usbdev.ctrl_transfer(0xC1, self.CMD_XMEGA_PROGRAM, cmd, windex, dlen, timeout=self._timeout)

    def enablePDI(self, status):
        if status:
            self._xmegaDoWrite(self.XPROG_CMD_ENTER_PROGMODE)
        else:
            self._xmegaDoWrite(self.XPROG_CMD_LEAVE_PROGMODE)

    def readMemory(self, addr, dlen, baseaddr=0x0800000):
        addr = baseaddr + addr

        infoblock = [0]
        infoblock.extend(packuint32(addr))
        infoblock.append(dlen & 0xff)
        infoblock.append((dlen >> 8) & 0xff)

        # Copy from flash to buffer
        self._xmegaDoWrite(self.XPROG_CMD_READ_MEM, data=infoblock)

        # Read out buffer
        data = self._xmegaDoRead(self.XPROG_GET_RAMBUF, windex=0, dlen=dlen)

        return data

    def writeMemory(self, addr, data, baseaddr=0x0800000):
        addr = baseaddr + addr

        # Copy to page buffer
        self._xmegaDoWrite(0x0003, windex=0, data=data)

        # Do actual write
        self._xmegaDoWrite(self.XPROG_GET_RAMBUF, data=packuint32(addr))

    def chipErase(self):
        self._xmegaDoWrite(self.XPROG_CMD_ERASE)

class CWLiteUSB(object):
    """ USB Interface for ChipWhisperer-Lite """

    CMD_READMEM_BULK = 0x10
    CMD_WRITEMEM_BULK = 0x11
    CMD_READMEM_CTRL = 0x12
    CMD_WRITEMEM_CTRL = 0x13

    CMD_FPGA_STATUS = 0x15
    CMD_FPGA_PROGRAM = 0x16
    

    def con(self):
        """Connect to device using default VID/PID"""

        dev = usb.core.find(idVendor=0x03EB, idProduct=0x2423)

        if dev is None:
            raise IOError("Failed to find USB Device")

        dev.set_configuration()

        self._usbdev = dev
        self.rep = 0x81
        self.wep = 0x02
        self._timeout = 200

    def sendCtrl(self, cmd, value=0, data=[]):
        """Send data over control endpoint"""
        # Vendor-specific, OUT, interface control transfer
        return self._usbdev.ctrl_transfer(0x41, cmd, value, 0, data, timeout=self._timeout)

    def readCtrl(self, cmd, value=0, dlen=0):
        """Read data from control endpoint"""
        # Vendor-specific, IN, interface control transfer
        return self._usbdev.ctrl_transfer(0xC1, cmd, value, 0, dlen, timeout=self._timeout)

    def cmdReadMem(self, addr, dlen):
        """Send command to read over external memory interface from FPGA"""

        if dlen < 48:
            cmd = self.CMD_READMEM_CTRL
        else:
            cmd = self.CMD_READMEM_BULK

        # ADDR/LEN written LSB first
        pload = packuint32(dlen)
        pload.extend(packuint32(addr))
        self.sendCtrl(cmd, data=pload)

        # Get data
        if cmd == self.CMD_READMEM_BULK:
            data = self._usbdev.read(self.rep, dlen, timeout=self._timeout)
        else:
            data = self.readCtrl(cmd, dlen=dlen)

        return data


    def cmdWriteMem(self, addr, data):
        """Send command to write memory over external memory interface to FPGA"""

        dlen = len(data)

        if dlen < 48:
            cmd = self.CMD_WRITEMEM_CTRL
        else:
            cmd = self.CMD_WRITEMEM_BULK

        # ADDR/LEN written LSB first
        pload = packuint32(dlen)
        pload.extend(packuint32(addr))

        if cmd == self.CMD_WRITEMEM_CTRL:
            pload.extend(data)

        self.sendCtrl(cmd, data=pload)

        # Get data
        if cmd == self.CMD_WRITEMEM_BULK:
            data = self._usbdev.write(self.wep, data, timeout=self._timeout)
        else:
            pass

        return data

    def isFPGAProgrammed(self):
        """Return True/False about FPGA status"""

        status = self.readCtrl(self.CMD_FPGA_STATUS, dlen=4)

        if status[0] & 0x01:
            return True
        else:
            return False


    def FPGAProgram(self, bitstream=None):
        """ Program FPGA with a bitstream, or if not bitstream passed just erases FPGA """

        self.sendCtrl(self.CMD_FPGA_PROGRAM, 0xA0)
        self.sendCtrl(self.CMD_FPGA_PROGRAM, 0xA1)

        # Download actual bitstream now
        if bitstream:
            self._FPGADownloadBitstream(bitstream)

            wait = 4
            while wait > 0:
                programStatus = self.isFPGAProgrammed()
                if programStatus:
                    break
                time.sleep(0.01)
                wait -= 1

            self.sendCtrl(self.CMD_FPGA_PROGRAM, 0xA2)

            if programStatus == False:
                raise IOError("FPGA Done pin failed to go high, bad bitstream?")

            return programStatus
        else:
            self.sendCtrl(self.CMD_FPGA_PROGRAM, 0xA2)
            return False

    def _FPGADownloadBitstream(self, fwFileLike):
        """ Performs actual bitstream download, do not call directly, call FPGAProgram """

        transactionBytes = 2048
        t0 = 0

        buffer_ = [None] * (16 * 1024 * 1024 / transactionBytes)
        size = 0

        # Read entire thing in
        with fwFileLike as f: inputStream = f.read()
        streamCnt = 0

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
                    if  i * transactionBytes >= size:
                        break
                    j = size - i * transactionBytes
                    if j > transactionBytes:
                        j = transactionBytes

                    self._usbdev.write(self.wep, buffer_[i], timeout=self._timeout)

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
                    print("Warning: " + str(e) + ": Retrying it ...")
                else:
                    raise e
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


if __name__ == '__main__':
    cwtestusb = CWLiteUSB()

    cwtestusb.con()

    force = False

    if cwtestusb.isFPGAProgrammed() == False or force:
        from datetime import datetime
        starttime = datetime.now()
        cwtestusb.FPGAProgram(open(r"C:\E\Documents\academic\sidechannel\chipwhisperer\openadc\hdl\example_targets\cwlite_testdev\cwlite_interface.bit", "rb"))
        stoptime = datetime.now()
        print "FPGA Config time: %s" % str(stoptime - starttime)

    # print cwtestusb.cmdReadMem(10, 6)
    #print cwtestusb.cmdReadMem(0x1A, 4)
    #cwtestusb.cmdWriteMem(0x1A, [235, 126, 5, 4])
    #print cwtestusb.cmdReadMem(0x1A, 4)

    xmega = XMEGAPDI()
    xmega.setUSB(cwtestusb._usbdev)

    try:
        xmega.enablePDI(True)
    
        #Read signature bytes
        data = xmega.readMemory(0x90, 3, 0x01000000)
    
        if data[0] != 0x1E or data[1] != 0x97 or data[2] != 0x46:
            print "Signature bytes failed: %02x %02x %02x != 1E 97 46"%(data[0], data[1], data[2])
        else:
            print "Detected XMEGA128A4U"

    except:
        xmega.enablePDI(False)

    xmega.enablePDI(False)





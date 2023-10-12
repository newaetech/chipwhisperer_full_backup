#
# SAMx Programming via Python, Copyright (c) 2016-2018 NewAE Technology Inc.
#
# Originally only supported SAM3U, now extended to other devices.
#
# This file is a based on BOSSA, which is:
# Copyright (c) 2011-2012, ShumaTech
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in the
#       documentation and/or other materials provided with the distribution.
#     * Neither the name of the <organization> nor the
#       names of its contributors may be used to endorse or promote products
#       derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
# DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#

#Avoid namespace collision with 'serial'


import logging

import serial # type: ignore
import time
# from .serial import USART
try:
    from ...capture.api.cwcommon import ChipWhispererCommonInterface
except:
    ChipWhispererCommonInterface = None # type: ignore

try:
    from ...logging import *
except:
    target_logger = logging # type: ignore

class XModem(object):

    BLK_SIZE = 128
    XSTART = ord('C')
    XSOH = 0x01
    XEOT = 0x04
    XNAK = 0x15
    XACK = 0x06
    XCAN = 0x18

    MAX_RETRIES = 5


    crc16Table = [
        0x0000,0x1021,0x2042,0x3063,0x4084,0x50a5,0x60c6,0x70e7,
        0x8108,0x9129,0xa14a,0xb16b,0xc18c,0xd1ad,0xe1ce,0xf1ef,
        0x1231,0x0210,0x3273,0x2252,0x52b5,0x4294,0x72f7,0x62d6,
        0x9339,0x8318,0xb37b,0xa35a,0xd3bd,0xc39c,0xf3ff,0xe3de,
        0x2462,0x3443,0x0420,0x1401,0x64e6,0x74c7,0x44a4,0x5485,
        0xa56a,0xb54b,0x8528,0x9509,0xe5ee,0xf5cf,0xc5ac,0xd58d,
        0x3653,0x2672,0x1611,0x0630,0x76d7,0x66f6,0x5695,0x46b4,
        0xb75b,0xa77a,0x9719,0x8738,0xf7df,0xe7fe,0xd79d,0xc7bc,
        0x48c4,0x58e5,0x6886,0x78a7,0x0840,0x1861,0x2802,0x3823,
        0xc9cc,0xd9ed,0xe98e,0xf9af,0x8948,0x9969,0xa90a,0xb92b,
        0x5af5,0x4ad4,0x7ab7,0x6a96,0x1a71,0x0a50,0x3a33,0x2a12,
        0xdbfd,0xcbdc,0xfbbf,0xeb9e,0x9b79,0x8b58,0xbb3b,0xab1a,
        0x6ca6,0x7c87,0x4ce4,0x5cc5,0x2c22,0x3c03,0x0c60,0x1c41,
        0xedae,0xfd8f,0xcdec,0xddcd,0xad2a,0xbd0b,0x8d68,0x9d49,
        0x7e97,0x6eb6,0x5ed5,0x4ef4,0x3e13,0x2e32,0x1e51,0x0e70,
        0xff9f,0xefbe,0xdfdd,0xcffc,0xbf1b,0xaf3a,0x9f59,0x8f78,
        0x9188,0x81a9,0xb1ca,0xa1eb,0xd10c,0xc12d,0xf14e,0xe16f,
        0x1080,0x00a1,0x30c2,0x20e3,0x5004,0x4025,0x7046,0x6067,
        0x83b9,0x9398,0xa3fb,0xb3da,0xc33d,0xd31c,0xe37f,0xf35e,
        0x02b1,0x1290,0x22f3,0x32d2,0x4235,0x5214,0x6277,0x7256,
        0xb5ea,0xa5cb,0x95a8,0x8589,0xf56e,0xe54f,0xd52c,0xc50d,
        0x34e2,0x24c3,0x14a0,0x0481,0x7466,0x6447,0x5424,0x4405,
        0xa7db,0xb7fa,0x8799,0x97b8,0xe75f,0xf77e,0xc71d,0xd73c,
        0x26d3,0x36f2,0x0691,0x16b0,0x6657,0x7676,0x4615,0x5634,
        0xd94c,0xc96d,0xf90e,0xe92f,0x99c8,0x89e9,0xb98a,0xa9ab,
        0x5844,0x4865,0x7806,0x6827,0x18c0,0x08e1,0x3882,0x28a3,
        0xcb7d,0xdb5c,0xeb3f,0xfb1e,0x8bf9,0x9bd8,0xabbb,0xbb9a,
        0x4a75,0x5a54,0x6a37,0x7a16,0x0af1,0x1ad0,0x2ab3,0x3a92,
        0xfd2e,0xed0f,0xdd6c,0xcd4d,0xbdaa,0xad8b,0x9de8,0x8dc9,
        0x7c26,0x6c07,0x5c64,0x4c45,0x3ca2,0x2c83,0x1ce0,0x0cc1,
        0xef1f,0xff3e,0xcf5d,0xdf7c,0xaf9b,0xbfba,0x8fd9,0x9ff8,
        0x6e17,0x7e36,0x4e55,0x5e74,0x2e93,0x3eb2,0x0ed1,0x1ef0]

    def crc16Calc(self, data):
        crc16 = 0
        for d in data:
            crc16 = (crc16 << 8) ^ self.crc16Table[((crc16 >> 8) ^ d) & 0xff]
            crc16 &= 0xffff
        return crc16

    def crc16Check(self, blk):
        crc16 = blk[self.BLK_SIZE + 3] << 8 | blk[self.BLK_SIZE + 4]
        blk = blk[3:-2]
        crccalc = self.crc16Calc(blk)
        target_logger.debug("XMODEM CRC: {} {}".format(crccalc, crc16))
        return crccalc == crc16

    def crc16Add(self, blk):
        crc16 = self.crc16Calc(blk[3:-2])
        blk[self.BLK_SIZE + 3] = (crc16 >> 8) & 0xff
        blk[self.BLK_SIZE + 4] = crc16 & 0xff

    def recv(self, ser, size):
        target_logger.debug("XMODEM Receive {} bytes".format(size))
        blk = [0]*(self.BLK_SIZE+5)
        blkNum = 1

        buffer = []
        while (size > 0):
            for retries in range(0, self.MAX_RETRIES+1):
                if blkNum == 1:
                    target_logger.debug("XModem Start")
                    ser.write(b"C")

                data = list(ser.read(len(blk)))
                target_logger.debug("XMODEM: Serial RX {} bytes".format(len(data)))

                if len(data) == len(blk) and \
                    data[0] == self.XSOH and \
                    data[1] == (blkNum & 0xff) and \
                    self.crc16Check(data):
                        break

                if blkNum != 1:
                    ser.write([self.XNAK])

                if retries == self.MAX_RETRIES:
                    raise IOError("XMODEM Failed")
            ser.write([self.XACK])

            buffer.extend(data[3:(3+min(size, self.BLK_SIZE))])
            size -= self.BLK_SIZE
            blkNum += 1

        for retries in range(0, self.MAX_RETRIES+1):

            if ser.read(1)[0] == self.XEOT:
                ser.write([self.XACK])
                break
            else:
                self.ser.write([self.XNACK])

            if retries == self.MAX_RETRIES:
                raise IOError("XModem Failed")

        return buffer

    def write(self, ser, payload):
        blk = [0]*(self.BLK_SIZE+5)
        blkNum = 1
        size = len(payload)
        buffer = 0

        target_logger.debug("XMODEM Send: {} bytes".format(size))

        for retries in range(0, self.MAX_RETRIES+1):
            data = ser.read(1)
            target_logger.debug("XMODEM Start: Received {}".format(data))
            if data and data[0] == self.XSTART:
                break

            if retries == self.MAX_RETRIES:
                raise IOError("XModem Timeout")

        while size > 0:
            target_logger.debug("XMODEM sending block {}".format(blkNum))
            blk[0] = self.XSOH
            blk[1] = blkNum & 0xff
            blk[2] = (blkNum & 0xff) ^ 0xff
            for i in range(0, self.BLK_SIZE):
                blk[3+i] = 0

            for i in range(0, min(size, self.BLK_SIZE)):
                blk[3+i] = payload[buffer+i]

            self.crc16Add(blk)

            for retries in range(0, self.MAX_RETRIES+1):
                ser.write(blk)

                if ser.read(1)[0] == self.XACK:
                    break

                if retries == self.MAX_RETRIES:
                    raise IOError("Xmodem retry failed")

            size -= self.BLK_SIZE
            buffer += self.BLK_SIZE
            blkNum += 1

        for retries in range(0, self.MAX_RETRIES+1):
            ser.write([self.XEOT])

            if ser.read(1)[0] == self.XACK:
                break

            if retries == self.MAX_RETRIES:
                raise IOError("XModem retry failed")

class Samba(object):

    def con(self, port, usbmode=True):
        if ChipWhispererCommonInterface and isinstance(port, ChipWhispererCommonInterface): # type: ignore
            ser = port._get_usart()
            ser.init()
            usbmode = False
        else:
            ser = serial.Serial(
                port=port,
                baudrate=115200, #921600
                parity=serial.PARITY_NONE,
                stopbits=serial.STOPBITS_ONE,
                bytesize=serial.EIGHTBITS,
                timeout=1
            )

        self.usbmode = usbmode

        self.ser = ser

        # Auto-baud
        if not usbmode:
            ser.write(b"\x80")
            ser.flush()
            ser.write(b"\x80")
            ser.flush()
            ser.write(b"#")
            ser.flush()
            ser.read(3)

        # Binary mode
        ser.write("N#".encode("ascii"))
        res = ser.read(2)
        # print(res)

        cid = self.chip_id()

        target_logger.info('FWUP: CID = %04x' % cid)

        #Originally this was used to limit to SAM3U
        #eproc = (cid >> 5) & 0x7
        #arch = (cid >> 20) & 0xff
        #if eproc == 3 and ((0x80 <= arch <= 0x8a) or (0x93 <= arch <= 0x9a)):
        #    target_logger.info('FWUP: Detected SAM3')

        self.setup_device_specific(cid)

        target_logger.info('FWUP: Detected ' + self.flash.name)
        return True



    def setup_device_specific(self, chipid):

        chipid = chipid & 0x7fffffe0

        #Find settings by looking at following sources:
        # https://github.com/shumatech/BOSSA/blob/master/src/Device.cpp which has EXACT same parameters passed as here
        # OpenOCD files such as https://github.com/ntfreak/openocd/blob/master/src/flash/nor/at91sam4.c

        if chipid == 0x28000960 or chipid == 0x28100960:
            flash = EefcFlash(self, "ATSAM3U4", 0xE0000, 1024, 256, 2, 32, 0x20001000, 0x20008000, 0x400e0800, False)
            self.rstc_addr = 0x400E1200
        elif chipid == 0x280a0760 or chipid == 0x281a0760:
            flash = EefcFlash(self, "ATSAM3U2", 0x80000, 512, 256, 1, 16, 0x20001000, 0x20004000, 0x400e0800, False)
            self.rstc_addr = 0x400E1200
        elif chipid == 0x28090560 or chipid == 0x28190560:
            flash = EefcFlash(self, "ATSAM3U1", 0x80000, 256, 256, 1, 8, 0x20001000, 0x20002000, 0x400e0800, False)
            self.rstc_addr = 0x400E1200
        elif chipid == 0x29970ce0:
            flash = EefcFlash(self, "at91sam4sd16b", 0x400000, 2048, 512, 2, 256, 0x20001000, 0x20010000, 0x400e0a00, False)
            self.rstc_addr = 0x400E1400
        elif chipid == 0x289c0ce0:
            flash = EefcFlash(self, "at91sam4s16b", 0x400000, 2048, 512, 1, 256, 0x20001000, 0x20010000, 0x400e0a00, False)
            self.rstc_addr = 0x400E1400
        elif chipid == 0x286e0a60 or chipid == 0x285e0a60  or chipid == 0x284e0a60 :
            flash = EefcFlash(self, "ATSAM3X8", 0x80000, 2048, 256, 2, 32, 0x20001000, 0x20010000, 0x400e0a00, False)
            self.rstc_addr = 0x400E1A00
        elif chipid == 0x288b07e0 or chipid == 0x289b07e0 or chipid == 0x28ab07e0:
            flash = EefcFlash(self, "ATSAM4S2", 0x400000, 256, 512, 1, 16, 0x20001000, 0x20010000, 0x400e0a00, False)
            self.rstc_addr = 0x400E1400
        else:
            raise AttributeError("FWUP: Unsupported ChipID = %x" % chipid)

        self.flash = flash

    def reset(self):
        """ Reset via RSTC register """
        self.write_word(self.rstc_addr, 0xA500000D)

    def chip_id(self):
        """ Read chip-id """

        vector = self.read_word(0x00)

        # If the vector is a ARM7TDMI branch, then assume Atmel SAM7 registers
        if ((vector & 0xff000000) == 0xea000000):
            cid = self.read_word(0xfffff240)
        # Else use the Atmel SAM3 registers
        else:
            #This works on SAM3U/SAM4S
            cid = self.read_word(0x400e0740)

            #SAM3x seems to be different - easily detected
            if cid == 0:
                cid = self.read_word(0x400e0940)

        return cid

    def read_word(self, addr):
        """ Read a word from SAM3U """
        target_logger.debug("Read word from {:02X}".format(addr))
        cmd = "w%08X,4#" % addr
        self.ser.write(cmd.encode("ascii"))
        resp = self.ser.read(4)

        if len(resp) < 4:
            target_logger.debug("Timeout on read from {:04X}".format(addr))
            raise IOError("timeout")

        value = (resp[3] << 24 | resp[2] << 16 | resp[1] << 8 | resp[0] << 0)
        target_logger.debug("Read {:04X} from {:02X}".format(value, addr))

        return value

    def write_word(self, addr, value):
        """ Write a word to SAM3U """

        target_logger.debug("Writing {:04X} to {:02X}".format(value, addr))
        cmd = "W%08X,%08X#" % (addr, value)
        self.ser.write(cmd.encode("ascii"))

    def read_byte(self, addr):
        """ Read a byte from SAM3U """

        cmd = "o%08X,4#" % addr
        self.ser.write(cmd.encode("ascii"))
        resp = self.ser.read(1)

        if len(resp) < 1:
            target_logger.debug("Timeout on read from {:04X}".format(addr))
            raise IOError("timeout")

        return resp[0]

    def go(self, addr):
        """ Run applet """

        cmd = "G%08X#" % addr
        self.ser.write(cmd.encode("ascii"))

        if self.usbmode:
            self.flush()

    def flush(self):
        self.ser.flush()
        # #TODO - if required can add delay
        # #to enforce USB flushing, as happens
        # #after 1mS
        #time.sleep(0.001)


    def _write_buf(self, addr, buf, size):
        """ Write a buffer """

        # if self.usbmode == False:
        #     raise AttributeError("Only USB Mode Supported")

        if len(buf) != size:
            raise AttributeError("Buffer length not as reported, expected {} got {}", size, len(buf))

        target_logger.debug("Writing {} to {:02X}".format(buf, addr))
        self.ser.write(("S%08X,%08X#" % (addr, size)).encode("ascii"))

        if self.usbmode:
            # Flush to ensure transactions arrive separately to bootloader
            # (Otherwise error)
            self.flush()
            bwritten = self.ser.write(bytearray(buf))

            if (bwritten != len(buf)):
                raise IOError("Failed to write %d bytes, only %d written" % (len(buf), bwritten))
        else:
            xm = XModem()
            xm.write(self.ser, buf)

    def _read_buf(self, addr, size):
        """ Read a buffer """

        target_logger.debug("Reading {} bytes from {:02X}".format(size, addr))

        buf = []

        # Note from BOSSA:
        #   The SAM firmware has a bug reading powers of 2 over 32 bytes
        #   via USB.  If that is the case here, then read the first byte
        #   with a readByte and then read one less than the requested size.
        if self.usbmode and (size > 32) and not (size & (size - 1)):
            buf.append(self.read_byte(addr))
            addr = addr + 1
            size = size - 1

        cmd = "R%08X,%08X#" % (addr, size)
        self.ser.write(cmd.encode("ascii"))

        if self.usbmode:
            buf.extend(self.ser.read(size))
        else:
            #serial uses xmodem
            xm = XModem()
            buf = xm.recv(self.ser, size)

        target_logger.debug("Read {}".format(buf))
        return buf

    def erase(self):
        """ Erase entire chip """

        self.flash.eraseAll()
        self.flash.eraseAuto(False)

    def write(self, bindata, doprint=False):
        """ Write a buffer into chip """

        page_size = self.flash.size
        page_num = 0
        page_max = self.flash.pages

        totalpages = (len(bindata) + page_size - 1) / page_size
        if totalpages > page_max:
            raise IOError("File too big (need %d pages, have %d)" % (totalpages, page_max))

        bytesleft = len(bindata)

        i = 0
        while bytesleft > 0:

            if bytesleft < page_size:
                # Pad partial page with 0xFF
                buf = bytearray(bindata[i:(i + bytesleft)])
                # print(len(buf))
                buf.extend([0xff] * (page_size - bytesleft))
                # print(bytesleft)
            else:
                # Read full page
                buf = bindata[i:(i + page_size)]

            # if (page_num % 10) == 0:
            target_logger.info('Flashing %d/%d' % (page_num, totalpages))

            self.flash.loadBuffer(buf)
            self.flash.writePage(page_num)

            page_num += 1
            if (page_num == totalpages) or (len(buf) != page_size):
                break

            i += page_size
            bytesleft -= page_size

        target_logger.info('FWUP: Program Successful')

    def verify(self, bindata, doprint=False):
        """ Verify a buffer that was written into chip """

        page_size = self.flash.size
        page_num = 0
        page_max = self.flash.pages

        totalpages = (len(bindata) + page_size - 1) / page_size
        if totalpages > page_max:
            raise IOError("File too big (need %d pages, have %d)" % (totalpages, page_max))

        bytesleft = len(bindata)

        i = 0
        while bytesleft > 0:

            if bytesleft < page_size:
                # Pad partial page with 0xFF
                buf = bytearray(bindata[i:(i + bytesleft)])
                buf.extend([0xff] * (page_size - bytesleft))
            else:
                # Read full page
                buf = bindata[i:(i + page_size)]

            if (page_num % 10) == 0:
                target_logger.info('Verifying %d/%d' % (page_num, totalpages))

            bufferB = bytearray(self.flash.readPage(page_num))
            #print(len(bufferB))
            #print(len(buf))

            if bytearray(buf) != bytearray(bufferB):
                # target_logger.warning('FWUP: Verify FAILED at %d"=' % i)
                target_logger.warning("Verify failed at {} (got {} expected {})".format(i, bufferB, buf))
                return False
                # print "fail at %d"%i
                # print "".join(["%02x"%ord(a) for a in buf])

            page_num = page_num + 1
            if (page_num == totalpages) or (len(buf) != page_size):
                break

            i = i + page_size
            bytesleft = bytesleft - page_size

        target_logger.info('FWUP: Verify successful')

        return True


class WordCopyArm(object):
    _dst_addr = 0x00000028
    _src_addr = 0x0000002c
    _reset = 0x00000024
    _stack = 0x00000020
    _start = 0x00000000
    words = 0x00000030
    code = [0x09, 0x48, 0x0a, 0x49, 0x0a, 0x4a, 0x02, 0xe0, 0x08, 0xc9, 0x08, 0xc0, 0x01, 0x3a, 0x00, 0x2a,
            0xfa, 0xd1, 0x04, 0x48, 0x00, 0x28, 0x01, 0xd1, 0x01, 0x48, 0x85, 0x46, 0x70, 0x47, 0xc0, 0x46,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00]


class WordCopyApplet(object):
    def __init__(self, samba, addr):
        self._samba = samba
        self._addr = addr
        self._applet = WordCopyArm()
        self._samba._write_buf(addr, self._applet.code, len(self._applet.code))

    def set_dst_addr(self, addr):
        self._samba.write_word(self._addr + self._applet._dst_addr, addr)

    def set_src_addr(self, addr):
        self._samba.write_word(self._addr + self._applet._src_addr, addr)

    def set_words(self, words):
        self._samba.write_word(self._addr + self._applet.words, words)

    def set_stack(self, stack):
        self._samba.write_word(self._addr + self._applet._stack, stack)

    def run(self):
        self._samba.go(self._addr + self._applet._start + 1)

    def runv(self):
        self._samba.write_word(self._addr + self._applet._reset, self._addr + self._applet._start + 1)
        self._samba.go(self._addr + self._applet._stack)

    def size(self):
        return len(self._applet.code)

    def addr(self):
        return self._addr

class EefcFlash(object):
    def __init__(self, samba, name, addr, pages, size, planes, lock_regions, user, stack, regs, can_brownout):
        self.name = name
        self.addr = addr
        self.pages = pages
        self.size = size
        self.planes = planes
        self.lock_regions = lock_regions
        self.user = user
        self.stack = stack
        self.regs = regs
        self.can_brownout = can_brownout
        self.erase_auto = True
        self.samba = samba
        self.word_copy = WordCopyApplet(samba, user)

        self.EEFC_KEY = 0x5a

        self.EEFC0_FMR = (regs + 0x00)
        self.EEFC0_FCR = (regs + 0x04)
        self.EEFC0_FSR = (regs + 0x08)
        self.EEFC0_FRR = (regs + 0x0C)

        self.EEFC1_FMR = (regs + 0x200)
        self.EEFC1_FCR = (regs + 0x204)
        self.EEFC1_FSR = (regs + 0x208)
        self.EEFC1_FRR = (regs + 0x20C)

        self.EEFC_FCMD_GETD = 0x0
        self.EEFC_FCMD_WP = 0x1
        self.EEFC_FCMD_WPL = 0x2
        self.EEFC_FCMD_EWP = 0x3
        self.EEFC_FCMD_EWPL = 0x4
        self.EEFC_FCMD_EA = 0x5
        self.EEFC_FCMD_SLB = 0x8
        self.EEFC_FCMD_CLB = 0x9
        self.EEFC_FCMD_GLB = 0xa
        self.EEFC_FCMD_SGPB = 0xb
        self.EEFC_FCMD_CGPB = 0xc
        self.EEFC_FCMD_GGPB = 0xd

        self._fsr_failed = False

        self.word_copy.set_words(int(size / 4))
        self.word_copy.set_stack(stack)
        self._onBufferA = True
        self._pageBufferA = self.user + self.word_copy.size()
        self._pageBufferB = self._pageBufferA + size

        # SAM3 Errata (FWS must be 6)
        self.samba.write_word(self.EEFC0_FMR, 0x6 << 8)
        if (planes == 2):
            self.samba.write_word(self.EEFC1_FMR, 0x6 << 8)


    def eraseAll(self):
        self.waitFSR()
        self.writeFCR0(self.EEFC_FCMD_EA, 0)
        if (self.planes == 2):
            self.waitFSR()
            self.writeFCR1(self.EEFC_FCMD_EA, 0)

    def lockAll(self):
        for region in range(0, self.lock_regions):
            self.setLockRegion(region, True)

    def unlockAll(self):
        for region in range(0, self.lock_regions):
            self.setLockRegion(region, False)

    def loadBuffer(self, data):
        if self._onBufferA:
            self.samba._write_buf(self._pageBufferA, data, self.size)
        else:
            self.samba._write_buf(self._pageBufferB, data, self.size)

    def eraseAuto(self, enable):
        self.erase_auto = enable

    def isLocked(self):
        self.waitFSR()
        self.writeFCR0(self.EEFC_FCMD_GLB, 0)
        self.waitFSR()
        if (self.readFRR0()):
            return True
        if (self.planes == 2):
            self.writeFCR1(self.EEFC_FCMD_GLB, 0)
            self.waitFSR()
            if (self.readFRR1()):
                return True
        return False

    def getLockRegion(self, region):
        if (region >= self.lockRegions):
            raise AttributeError("Invalid flash region")

        self.waitFSR()

        if (self.planes == 2 and region >= self.lockRegions / 2):
            self.writeFCR1(self.EEFC_FCMD_GLB, 0)
            self.waitFSR()
            if self.readFRR1() & (1 << (region - (self.lock_regions / 2))):
                return True
        else:
            self.writeFCR0(self.EEFC_FCMD_GLB, 0)
            self.waitFSR()
            if self.readFRR0() & (1 << region):
                return True

        return False


    def setLockRegion(self, region, enable):

        if (region >= self.lockRegions):
            raise AttributeError("Invalid flash region")

        if (enable != self.getLockRegion(region)):
            if (self.planes == 2) and (region >= self.lockRegions / 2):
                page = (region - self.lockRegions / 2) * self.pages / self.lockRegions;
                self.waitFSR()
                if enable:
                    self.writeFCR1(self.EEFC_FCMD_SLB, page)
                else:
                    self.writeFCR1(self.EEFC_FCMD_CLB, page)
            else:
                page = int(region * self.pages / self.lockRegions)
                self.waitFSR()
                if enable:
                    self.writeFCR0(self.EEFC_FCMD_SLB, page)
                else:
                    self.writeFCR0(self.EEFC_FCMD_CLB, page)

    def getSecurity(self):
        self.waitFSR()
        self.writeFCR0(self.EEFC_FCMD_GGPB, 0)
        self.waitFSR()
        return (self.readFRR0() & (1 << 0))

    def setSecurity(self):
        self.waitFSR()
        self.writeFCR0(self.EEFC_FCMD_SGPB, 0)

    def getBod(self):
        if not self.can_brownout:
            return False

        self.waitFSR()
        self.writeFCR0(self.EEFC_FCMD_GGPB, 0)
        self.waitFSR()
        return (self.readFRR0() & (1 << 1))

    def setBod(self, enable):
        if not self.can_brownout:
            return

        self.waitFSR()
        if enable:
            self.writeFCR0(self.EEFC_FCMD_SGPB, 1)
        else:
            self.writeFCR0(self.EEFC_FCMD_CGPB, 1)

    def getBor(self):
        if not self.can_brownout:
            return False

        self.waitFSR()
        self.writeFCR0(self.EEFC_FCMD_GGPB, 0)
        self.waitFSR()
        return (self.readFRR0() & (1 << 2))

    def setBor(self, enable):
        if not self.can_brownout:
            return

        self.waitFSR()
        if enable:
            self.writeFCR0(self.EEFC_FCMD_SGPB, 2)
        else:
            self.writeFCR0(self.EEFC_FCMD_CGPB, 2)

    def getBootFlash(self):
        self.waitFSR()
        self.writeFCR0(self.EEFC_FCMD_GGPB, 0)
        self.waitFSR()
        if self.can_brownout:
            cb = 3
        else:
            cb = 1
        return (self.readFRR0() & (1 << cb))

    def setBootFlash(self, enable):
        self.waitFSR()
        if self.can_brownout:
            cb = 3
        else:
            cb = 1

        if enable:
            self.writeFCR0(self.EEFC_FCMD_SGPB, cb)
        else:
            self.writeFCR0(self.EEFC_FCMD_CGPB, cb)

    def getFlashDescriptor(self):
        self.waitFSR()
        self.writeFCR0(self.EEFC_FCMD_GETD, 0)
        self.waitFSR()
        fl_id = self.readFRR0()
        fl_size = self.readFRR0()
        fl_page_size = self.readFRR0()
        fl_nb_plane = self.readFRR0()
        fl_plane_bytes = self.readFRR0()

        flash_descriptor = {
            'FL_ID':fl_id,
            'FL_SIZE':fl_size,
            'FL_PAGE_SIZE':fl_page_size,
            'FL_NB_PLANE':fl_nb_plane,
            'FL_PLANE':[fl_plane_bytes]
        }

        return flash_descriptor


    def writePage(self, page):
        if (page >= self.pages):
            raise AttributeError("Invalid Page")

        self.word_copy.set_dst_addr(self.addr + (page * self.size))
        if self._onBufferA:
            self.word_copy.set_src_addr(self._pageBufferA)
        else:
            self.word_copy.set_src_addr(self._pageBufferB)
        self._onBufferA = not self._onBufferA
        self.waitFSR()
        self.word_copy.runv()

        if self.erase_auto:
            cmd = self.EEFC_FCMD_EWP
        else:
            cmd = self.EEFC_FCMD_WP

        target_logger.info("Writing FCR0")
        if (self.planes == 2 and page >= (self.pages / 2)):
            self.writeFCR1(cmd, page - self.pages / 2)
        else:
            self.writeFCR0(cmd, page)

    def readPage(self, page):
        if (page >= self.pages):
            raise AttributeError("Invalid page")

        # Note from BOSSA:
        #  The SAM3 firmware has a bug where it returns all zeros for reads
        #  directly from the flash so instead, we copy the flash page to
        #  SRAM and read it from there.

        if self._onBufferA:
            self.word_copy.set_dst_addr(self._pageBufferA)
        else:
            self.word_copy.set_dst_addr(self._pageBufferB)

        self.word_copy.set_src_addr(self.addr + (page * self.size))
        self.waitFSR()
        self.word_copy.runv()
        if self._onBufferA:
            return self.samba._read_buf(self._pageBufferA, self.size)
        else:
            return self.samba._read_buf(self._pageBufferB, self.size)


    def waitFSR(self, wait_ms=5000):
        tries = 0
        fsr1 = 0x1

        while (tries <= (wait_ms // 10)):
            tries = tries + 1
            try:
                fsr0 = self.samba.read_word(self.EEFC0_FSR)
            except IOError:
                # Catch serial timeout problem, this may mask
                # other errors however.
                continue

            if (fsr0 & (1 << 2)):
                raise IOError("Timeout")

            if (self.planes == 2):
                try:
                    fsr1 = self.samba.read_word(self.EEFC1_FSR)
                    if (fsr1 & (1 << 2)):
                        raise IOError("Timeout, fsr1: {:02X}".format(fsr1))
                except IOError as e:
                    print(e)
                    fsr1 = self.samba.read_word(self.EEFC1_FSR)
                    if (fsr1 & (1 << 2)):
                        raise IOError("Timeout, fsr1: {:02X}".format(fsr1))

            if (fsr0 & fsr1 & 0x1):
                break
            time.sleep(0.01)
        if (tries > wait_ms):
            raise IOError("Timeout")

    def writeFCR0(self, cmd, arg):
        self.samba.write_word(self.EEFC0_FCR, (self.EEFC_KEY << 24) | (arg << 8) | cmd)

    def writeFCR1(self, cmd, arg):
        self.samba.write_word(self.EEFC1_FCR, (self.EEFC_KEY << 24) | (arg << 8) | cmd)

    def readFRR0(self):
        return self.samba.read_word(self.EEFC0_FRR)

    def readFRR1(self):
        return self.samba.read_word(self.EEFC1_FRR)

if __name__ == "__main__":
    # Example usage
    # target_logger.setLevel(level=logging.INFO)
    sam = Samba()
    sam.con('com131')
    sam.erase()
    #data = open(r'C:\chipwhisperer\hardware\capture\chipwhisperer-lite\sam3u_fw\SAM3U_VendorExample\Debug\SAM3U_CW1173.bin', 'rb').read()
    data = open(r'c:\users\colin\dropbox\engineering\git_repos\CW1101_CWNano\cwnano-firmware\cwnano-firmware\Debug\cwnano-firmware.bin', 'rb').read()
    sam.write(data)
    if sam.verify(data):
        sam.flash.setBootFlash(True)
    sam.ser.close()


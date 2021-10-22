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

class Samba(object):

    def con(self, port, usbmode=True):
        ser = serial.Serial(
            port=port,
            baudrate=921600,  # 115200
            parity=serial.PARITY_NONE,
            stopbits=serial.STOPBITS_ONE,
            bytesize=serial.EIGHTBITS
        )

        self.usbmode = usbmode

        self.ser = ser

        # Auto-baud
        if not usbmode:
            ser.write("\x80")
            ser.flush()
            ser.write("\x80")
            ser.flush()
            ser.write("#")
            ser.flush()
            ser.read(3)

        # Binary mode
        ser.write("N#".encode("ascii"))
        res = ser.read(2)
        # print(res)

        cid = self.chip_id()

        logging.info('FWUP: CID = %04x' % cid)

        #Originally this was used to limit to SAM3U
        #eproc = (cid >> 5) & 0x7
        #arch = (cid >> 20) & 0xff
        #if eproc == 3 and ((0x80 <= arch <= 0x8a) or (0x93 <= arch <= 0x9a)):
        #    logging.info('FWUP: Detected SAM3')

        self.setup_device_specific(cid)

        logging.info('FWUP: Detected ' + self.flash.name)
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
        elif chipid == 0x286e0a60 or chipid == 0x285e0a60  or chipid == 0x284e0a60 :
            flash = EefcFlash(self, "ATSAM3X8", 0x80000, 2048, 256, 2, 32, 0x20001000, 0x20010000, 0x400e0a00, False)
            self.rstc_addr = 0x400E1A00
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

        cmd = "w%08X,4#" % addr
        self.ser.write(cmd.encode("ascii"))
        resp = self.ser.read(4)

        value = (resp[3] << 24 | resp[2] << 16 | resp[1] << 8 | resp[0] << 0)
        return value

    def write_word(self, addr, value):
        """ Write a word to SAM3U """

        cmd = "W%08X,%08X#" % (addr, value)
        self.ser.write(cmd.encode("ascii"))

    def read_byte(self, addr):
        """ Read a byte from SAM3U """

        cmd = "o%08X,4#" % addr
        self.ser.write(cmd.encode("ascii"))
        resp = self.ser.read(1)
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
        # time.sleep(0.001)


    def _write_buf(self, addr, buf, size):
        """ Write a buffer """

        if self.usbmode == False:
            raise AttributeError("Only USB Mode Supported")

        if len(buf) != size:
            raise AttributeError("Buffer length not as reported, expected {} got {}", size, len(buf))

        self.ser.write(("S%08X,%08X#" % (addr, size)).encode("ascii"))
        # Flush to ensure transactions arrive separately to bootloader
        # (Otherwise error)
        self.flush()
        bwritten = self.ser.write(bytearray(buf))

        if (bwritten != len(buf)):
            raise IOError("Failed to write %d bytes, only %d written" % (len(buf), bwritten))

    def _read_buf(self, addr, size):
        """ Read a buffer """

        if self.usbmode == False:
            raise AttributeError("Only USB Mode Supported")

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
        buf.extend(self.ser.read(size))
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

            if (page_num % 10) == 0 and doprint:
                logging.debug('Flashing %d/%d' % (page_num, totalpages))

            self.flash.loadBuffer(buf)
            self.flash.writePage(page_num)

            page_num += 1
            if (page_num == totalpages) or (len(buf) != page_size):
                break

            i += page_size
            bytesleft -= page_size

        logging.info('FWUP: Program Successful')

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

            if (page_num % 10) == 0 and doprint:
                print('Verifying %d/%d' % (page_num, totalpages))

            bufferB = bytearray(self.flash.readPage(page_num))

            if bytearray(buf) != bytearray(bufferB):
                # logging.warning('FWUP: Verify FAILED at %d"=' % i)
                logging.warning("Verify failed at {} (got {} expected {})".format(i, buf, bufferB))
                return False
                # print "fail at %d"%i
                # print "".join(["%02x"%ord(a) for a in buf])

            page_num = page_num + 1
            if (page_num == totalpages) or (len(buf) != page_size):
                break

            i = i + page_size
            bytesleft = bytesleft - page_size

        logging.info('FWUP: Verify successful')

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
            fsr0 = self.samba.read_word(self.EEFC0_FSR)
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
    logging.basicConfig(level=logging.INFO)
    sam = Samba()
    sam.con('com131')
    sam.erase()
    #data = open(r'C:\chipwhisperer\hardware\capture\chipwhisperer-lite\sam3u_fw\SAM3U_VendorExample\Debug\SAM3U_CW1173.bin', 'rb').read()
    data = open(r'c:\users\colin\dropbox\engineering\git_repos\CW1101_CWNano\cwnano-firmware\cwnano-firmware\Debug\cwnano-firmware.bin', 'rb').read()
    sam.write(data)
    if sam.verify(data):
        sam.flash.setBootFlash(True)
    sam.ser.close()


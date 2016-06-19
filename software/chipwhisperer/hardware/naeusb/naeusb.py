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
import usb.core
import usb.util


def packuint32(data):
    """Converts a 32-bit integer into format expected by USB firmware"""

    return [data & 0xff, (data >> 8) & 0xff, (data >> 16) & 0xff, (data >> 24) & 0xff]


def packuint16(data):
    """Converts a 16-bit integer into format expected by USB firmware"""

    return [data & 0xff, (data >> 8) & 0xff, (data >> 16) & 0xff, (data >> 24) & 0xff]


#List of all NewAE PID's
NEWAE_VID = 0x2B3E
NEWAE_PIDS = {
    0xACE2: "ChipWhisperer-Lite",
    0xACE3: "ChipWHisperer-CW1200",
    0xC305: "CW305 Artix FPGA Board",
}


class NAEUSB(object):
    """
    USB Interface for NewAE Products with Custom USB Firmware
    """

    CMD_FW_VERSION = 0x17

    CMD_READMEM_BULK = 0x10
    CMD_WRITEMEM_BULK = 0x11
    CMD_READMEM_CTRL = 0x12
    CMD_WRITEMEM_CTRL = 0x13

    stream = False

    # TODO: make this better
    fwversion_latest = [0, 11]
    def __init__(self):
        self._usbdev = None

    def con(self, idProduct=[0xACE2]):
        """
        Connect to device using default VID/PID
        """

        for id in idProduct:
            dev = usb.core.find(idVendor=0x2B3E, idProduct=id)
            foundId = id

            #Found something
            if dev:
                break

        if not dev:
            raise Warning("Failed to find USB Device")

        dev.set_configuration()

        # Get serial number
        try:
            # New calling syntax
            self.snum = usb.util.get_string(dev, index=3)

        except TypeError:
            # Old calling syntax
            self.snum = usb.util.get_string(dev, length=256, index=3)


        if foundId in NEWAE_PIDS:
            name = NEWAE_PIDS[foundId]
        else:
            name = "Unknown (PID = %04x)"%foundId

        print "Found %s, Serial Number = %s" % (name, self.snum)

        self._usbdev = dev
        self.rep = 0x81
        self.wep = 0x02
        self._timeout = 200

        fwver = self.readFwVersion()
        print "SAM3U Firmware version = %d.%d b%d" % (fwver[0], fwver[1], fwver[2])

        if not (fwver[0] >= self.fwversion_latest[0] and fwver[1] >= self.fwversion_latest[1]):
            print "**NOTE: Your firmware is outdated - latest is %d.%d" % (self.fwversion_latest[0], self.fwversion_latest[1])
            print "**Suggested to update firmware, as you may experience errors"

        return foundId

    def usbdev(self):
        if not self._usbdev: raise Warning("USB Device not found. Did you connect it first?")
        return self._usbdev

    def close(self):
        """
        Close USB connection
        """
        # self.usbdev().close()
        try:
            usb.util.dispose_resources(self.usbdev())
        except usb.USBError as e:
            print "INFO: USB Failure calling dispose_resources: %s" % str(e)

    def readFwVersion(self):
        try:
            data = self.readCtrl(self.CMD_FW_VERSION, dlen=3)
            return data
        except usb.USBError:
            return [0, 0, 0]

    def sendCtrl(self, cmd, value=0, data=[]):
        """
        Send data over control endpoint
        """
        # Vendor-specific, OUT, interface control transfer
        return self.usbdev().ctrl_transfer(0x41, cmd, value, 0, data, timeout=self._timeout)

    def readCtrl(self, cmd, value=0, dlen=0):
        """
        Read data from control endpoint
        """
        # Vendor-specific, IN, interface control transfer
        return self.usbdev().ctrl_transfer(0xC1, cmd, value, 0, dlen, timeout=self._timeout)

    def cmdReadMem(self, addr, dlen):
        """
        Send command to read over external memory interface from FPGA. Automatically
        decides to use control-transfer or bulk-endpoint transfer based on data length.
        """

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
            data = self.usbdev().read(self.rep, dlen, timeout=self._timeout)
        else:
            data = self.readCtrl(cmd, dlen=dlen)

        return data

    def cmdWriteMem(self, addr, data):
        """
        Send command to write memory over external memory interface to FPGA. Automatically
        decides to use control-transfer or bulk-endpoint transfer based on data length.
        """

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
            data = self.usbdev().write(self.wep, data, timeout=self._timeout)
        else:
            pass

        return data

    def enterBootloader(self, forreal=False):
        """Erase the SAM3U contents, forcing bootloader mode. Does not screw around."""

        if forreal:
            self.sendCtrl(0x22, 3)

    def flushInput(self):
        """Dump all the crap left over"""
        try:
            # TODO: This probably isn't needed, and causes slow-downs on Mac OS X.
            self.usbdev().read(self.rep, 1000, timeout=0.010)
        except:
            pass

if __name__ == '__main__':
    from fpga import FPGA
    from programmer_avr import AVRISP
    from programmer_xmega import XMEGAPDI, supported_xmega
    from serial import USART

    cwtestusb = NAEUSB()
    cwtestusb.con()

    #Connect required modules up here
    fpga = FPGA(cwtestusb)
    xmega = XMEGAPDI(cwtestusb)
    avr = AVRISP(cwtestusb)
    usart = USART(cwtestusb)

    force = True
    if fpga.isFPGAProgrammed() == False or force:
        from datetime import datetime
        starttime = datetime.now()
        fpga.FPGAProgram(open(r"C:\E\Documents\academic\sidechannel\chipwhisperer\hardware\capture\chipwhisperer-lite\hdl\cwlite_ise\cwlite_interface.bit", "rb"))
        # fpga.FPGAProgram(open(r"C:\Users\colin\dropbox\engineering\git_repos\CW305_ArtixTarget\temp\artix7test\artix7test.runs\impl_1\cw305_top.bit", "rb"))
        # fpga.FPGAProgram(open(r"C:\E\Documents\academic\sidechannel\chipwhisperer\hardware\api\chipwhisperer-lite\hdl\cwlite_ise_spifake\cwlite_interface.bit", "rb"))
        stoptime = datetime.now()
        print "FPGA Config time: %s" % str(stoptime - starttime)

    # print fpga.cmdReadMem(10, 6)
    # print fpga.cmdReadMem(0x1A, 4)
    # fpga.cmdWriteMem(0x1A, [235, 126, 5, 4])
    # print fpga.cmdReadMem(0x1A, 4)

    avrprogram = False
    if avrprogram:
        avr.enableISP(True)
        avr.enableISP(False)

    xmegaprogram = True
    if xmegaprogram:
        xmega.setChip(supported_xmega[0])
        # Worst-case is 75mS for chip erase, so give us some head-room
        xmega.setParamTimeout(200)

        try:
            print "Enable"
            xmega.enablePDI(True)

            print "Read sig"
            # Read signature bytes
            data = xmega.readMemory(0x01000090, 3, "signature")

            print data

            if data[0] != 0x1E or data[1] != 0x97 or data[2] != 0x46:
                print "Signature bytes failed: %02x %02x %02x != 1E 97 46" % (data[0], data[1], data[2])
            else:
                print "Detected XMEGA128A4U"

            print "Erasing"
            # Chip erase
            try:
                xmega.eraseChip()
            except IOError:
                xmega.enablePDI(False)
                xmega.enablePDI(True)

            fakedata = [i & 0xff for i in range(0, 2048)]
            print "Programming FLASH Memory"
            xmega.writeMemory(0x0800000, fakedata, memname="flash")

            print "Verifying"
            test = xmega.readMemory(0x0800000, 512)

            print test


        except TypeError, e:
            print str(e)

        except IOError, e:
            print str(e)

        xmega.enablePDI(False)


    # cwtestusb.cmdReadMem(0x00, 25)
    # cwtestusb.cmdWriteMem(0, [1, 2, 3, 4, 5, 0xFF])
    # cwtestusb.cmdWriteMem(0x100, [0xFF, 2, 3])
    # time.sleep(1)
    # cwtestusb.cmdWriteMem(0x100, [0x00])
    # time.sleep(1)
    # cwtestusb.cmdWriteMem(0x100, [0xFF])

    # key = [0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c]
    # key = [i for i in range(0, 16)]
    # text = [0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96, 0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a]
    # text = [i for i in range(0, 16)]
    # text = key
    # text = [0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96, 0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2b]

    # key = key[::-1]
    # text = text[::-1]

    # cwtestusb.cmdWriteMem(0x200, key)
    # cwtestusb.cmdWriteMem(0x300, text)

    # print "enable AES and wait..."
    # print cwtestusb.cmdReadMem(0x110, 1)[0]
    # cwtestusb.cmdWriteMem(0x100, [1])
    # print cwtestusb.cmdReadMem(0x110, 1)[0]

    # print "done"
    # print cwtestusb.cmdReadMem(0x110, 1)[0]
    # while cwtestusb.cmdReadMem(0x110, 1)[0] == 0:
    #    time.sleep(0.01)
    #    print "waiting..."
    # cwtestusb.cmdWriteMem(0x100, [0])
    # # time.sleep(0.5)
    # print ["%02x" % i for i in cwtestusb.cmdReadMem(0x600, 16)]
    # print ["%02x" % i for i in cwtestusb.cmdReadMem(0x200, 16)]
    # print ["%02x" % i for i in cwtestusb.cmdReadMem(0x300, 16)]


    print "Let's Rock and Roll baby"

    sertest = True

    if sertest:
        usart.init()
        usart.write("hello\n")
        time.sleep(0.1)
        print usart.read()

    # cwtestusb.sendCtrl(0x1F, 0x01)
    # flog = open("spilog.txt", "w+")
    # while True:
    #    try:
    #        data = cwtestusb._usbdev.read(cwtestusb.rep, 512)
    #        slog = ""
    #        slog += "%s: " % str(datetime.now())
    #        for d in data:
    #            slog += "%02x " % d
    #        print slog
    #        flog.write(slog + "\n")
    #        flog.flush()
    #    except usb.core.USBError:
    #        pass



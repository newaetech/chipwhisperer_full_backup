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
import logging
from chipwhisperer.common.utils import util
from chipwhisperer.hardware.naeusb.programmer_avr import supported_avr
from chipwhisperer.hardware.naeusb.programmer_xmega import supported_xmega
from chipwhisperer.capture.utils.IntelHex import IntelHex


class Programmer(object):
    lastFlashedFile = "unknown"

    def __init__(self):
        self.newTextLog = util.Signal()

    def setUSBInterface(self, iface):
        raise NotImplementedError

    def find(self):
        raise NotImplementedError

    def program(self, filename, memtype="flash", verify=True):
        raise NotImplementedError

    def erase(self):
        raise NotImplementedError

    def close(self):
        raise NotImplementedError

    def log(self, text):
        """Logs the text and broadcasts it"""
        logging.info(text)
        self.newTextLog.emit(text)


class AVRProgrammer(Programmer):
    def __init__(self):
        super(AVRProgrammer, self).__init__()
        self.supported_chips = []
        self._foundchip = False
        self.avr = None

    def setUSBInterface(self, iface):
        self.avr = iface
        # self.avr.setChip(self.supported_chips[0])

    def find(self):
        self._foundchip = False

        self.avr.enableISP(True)
        sig = self.avr.readSignature()

        # Figure out which device?
        # Check if it's one we know about?
        for t in supported_avr:
            if ((sig[0] == t.signature[0]) and
                (sig[1] == t.signature[1]) and
                (sig[2] == t.signature[2])):

                self._foundchip = True

                self.log("Detected %s" % t.name)
                self.avr.setChip(t)
                break

        # Print signature of unknown device
        if self._foundchip == False:
            self.log("Detected Unknown Chip, sig=%2x %2x %2x" % (sig[0], sig[1], sig[2]))

    def erase(self):
        self.log("Erasing Chip")
        self.avr.eraseChip()

    def program(self, filename, memtype="flash", verify=True):
        Programmer.lastFlashedFile = filename
        f = IntelHex(filename)

        maxsize = self.avr._chip.memtypes[memtype]["size"]
        fsize = f.maxaddr() - f.minaddr()

        if fsize > maxsize:
            raise IOError("File %s appears to be %d bytes, larger than %s size of %d" % (filename, fsize, memtype, maxsize))

        self.log("AVR Programming %s..." % memtype)
        util.updateUI()
        fdata = f.tobinarray(start=0)
        self.avr.writeMemory(0, fdata, memtype)

        self.log("AVR Reading %s..." % memtype)
        util.updateUI()
        # Do verify run
        rdata = self.avr.readMemory(0, len(fdata))  # memtype ?

        for i in range(0, len(fdata)):
            if fdata[i] != rdata[i]:
                raise IOError("Verify failed at 0x%04x, %x != %x" % (i, fdata[i], rdata[i]))

        self.log("Verified %s OK, %d bytes" % (memtype, fsize))
    
    def close(self):
        if self.avr is not None:
            self.avr.enableISP(False)


class XMEGAProgrammer(Programmer):

    def __init__(self):
        super(XMEGAProgrammer, self).__init__()
        self.supported_chips = supported_xmega
        self._foundchip = False

    def setUSBInterface(self, iface):
        self._foundchip = False
        self.xmega = iface
        self.xmega.setChip(self.supported_chips[0])

    def find(self):
        self._foundchip = False
        self.xmega.setParamTimeout(400)
        self.xmega.enablePDI(True)

        # Read signature bytes
        data = self.xmega.readMemory(0x01000090, 3, "signature")

        # Check if it's one we know about?
        for t in self.supported_chips:
            if ((data[0] == t.signature[0]) and
                (data[1] == t.signature[1]) and
                (data[2] == t.signature[2])):

                self._foundchip = True

                self.log("Detected %s" % t.name)
                self.xmega.setChip(t)
                break

        # Print signature of unknown device
        if self._foundchip == False:
            self.log("Detected Unknown Chip, sig=%2x %2x %2x" % (data[0], data[1], data[2]))

    def erase(self, memtype="chip"):
        self.log("Erasing Chip")
        if memtype == "app":
            self.xmega.eraseApp()
        elif memtype == "chip":
            self.xmega.eraseChip()
        else:
            raise ValueError("Invalid memtype: %s" % memtype)

    def program(self, filename, memtype="flash", verify=True):
        Programmer.lastFlashedFile = filename
        f = IntelHex(filename)

        startaddr = self.xmega._chip.memtypes[memtype]["offset"]
        maxsize = self.xmega._chip.memtypes[memtype]["size"]
        fsize = f.maxaddr() - f.minaddr()

        if fsize > maxsize:
            raise IOError("File %s appears to be %d bytes, larger than %s size of %d" % (filename, fsize, memtype, maxsize))

        self.log("XMEGA Programming %s..." % memtype)
        util.updateUI()
        fdata = f.tobinarray(start=0)
        self.xmega.writeMemory(startaddr, fdata, memtype)  # , erasePage=True

        self.log("XMEGA Reading %s..." % memtype)
        util.updateUI()
        # Do verify run
        rdata = self.xmega.readMemory(startaddr, len(fdata), memtype)

        for i in range(0, len(fdata)):
            if fdata[i] != rdata[i]:
                raise IOError("Verify failed at 0x%04x, %x != %x" % (i, fdata[i], rdata[i]))

        self.log("Verified %s OK, %d bytes" % (memtype, fsize))

    def close(self):
        self.xmega.enablePDI(False)

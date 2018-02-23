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
from chipwhisperer.hardware.naeusb.programmer_stm32fserial import supported_stm32f


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
        self.avr = None

    def setUSBInterface(self, iface):
        self.avr = iface
        # self.avr.setChip(self.supported_chips[0])

    def find(self):
        sig, chip = self.avr.find()
        if chip is None:
            self.log("AVR: Detected unknown device with signature=%2x %2x %2x" % (sig[0], sig[1], sig[2]))
        else:
            self.log("AVR: Detected device %s" % chip.name)

    def erase(self):
        self.log("Erasing Chip")
        self.avr.eraseChip()

    def program(self, filename, memtype="flash", verify=True):
        Programmer.lastFlashedFile = filename
        self.avr.program(filename, memtype, verify)
    
    def close(self):
        try:
            if self.avr is not None:
                self.avr.enableISP(False)
        except AttributeError as e:
            logging.info("AVR programmer: could not disable ISP - USB might be disconnected!")
            self.avr = None


class XMEGAProgrammer(Programmer):

    def __init__(self):
        super(XMEGAProgrammer, self).__init__()
        self.xmega = None
        self.supported_chips = supported_xmega

    def setUSBInterface(self, iface):
        self.xmega = iface
        self.xmega.setChip(self.supported_chips[0])

    def find(self):
        sig, chip = self.xmega.find()

        # Print signature of unknown device
        if chip is None:
            self.log("Detected Unknown Chip, sig=%2x %2x %2x" % (sig[0], sig[1], sig[2]))
        else:
             self.log("Detected %s" % chip.name)

    def erase(self, memtype="chip"):
        self.log("Erasing Chip")
        self.xmega.erase(memtype)


    def program(self, filename, memtype="flash", verify=True):
        Programmer.lastFlashedFile = filename
        self.xmega.program(filename, memtype, verify)

    def close(self):
        self.xmega.enablePDI(False)


class STM32FProgrammer(Programmer):

    def __init__(self):
        super(STM32FProgrammer, self).__init__()
        self.stm32 = None
        self.supported_chips = supported_stm32f

    def setUSBInterface(self, iface):
        self.stm32 = iface
        self.stm32.setChip(self.supported_chips[0])

    def open_and_find(self):
        self.stm32.open_port()
        sig, chip = self.stm32.find()

        # Print signature of unknown device
        if chip is None:
            self.log("Detected Unknown Chip, sig=%3x"% (sig))
        else:
             self.log("Detected %s" % chip.name)

    def close(self):
        self.stm32.close_port()
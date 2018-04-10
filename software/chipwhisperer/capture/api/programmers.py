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
import chipwhisperer.common.api.CWCoreAPI

from functools import wraps


def save_and_restore_pins(func):
    """Decorator to save and restore pins needed to comunicate and program hardware

        Purpose: to move from changing pins in the background needed to communicate
         with the hardware and leaving them changed (very confusing), to saving the
         pin states before the function is called, changing them during function
         executation and restoring them after the function is done executing
         (less confusing).
        """
    @wraps(func) # updates func_wrapper aatributes to be same
    def func_wrapper(self, *args, **kwargs):
        pin_setup = {
            'pdic': self.scope.io.pdic,
            'pdid': self.scope.io.pdid,
            'nrst': self.scope.io.nrst,
        }
        logging.debug('Saving pdic, pdid, and nrst pin configuration')
        # setup the pins so that so communication to the target is possible
        # Important: during the execution of func, the pin values may change if
        # the function is related to reprogramming or resetting the device. Example:
        # the stm32f uses the toggling of the nrst and pdic pins for resetting
        # and boot mode setting respectively
        logging.debug('Changing pdic, pdid, and nrst pin configuration')
        if pin_setup['pdic'] != 'high_z':
            self.scope.io.pdic = 'high_z'
        if pin_setup['pdid'] != 'high_z':
            self.scope.io.pdid = 'high_z'
        if pin_setup['nrst'] != 'high_z':
            self.scope.io.nrst = 'high_z'
        try:
            val = func(self, *args, **kwargs)
        finally:
            logging.debug('Restoring pdic, pdid, and nrst pin configuration')
            if self.scope.io.pdic != pin_setup['pdic']:
                self.scope.io.pdic = pin_setup['pdic']
            if self.scope.io.pdid != pin_setup['pdid']:
                self.scope.io.pdid = pin_setup['pdid']
            if self.scope.io.nrst != pin_setup['nrst']:
                self.scope.io.nrst = pin_setup['nrst']
        return val # only returns value when decorating a function with return value
    return func_wrapper



class Programmer(object):
    lastFlashedFile = "unknown"
    _scope = None
    pin_setup = {}

    def __init__(self):
        self.newTextLog = util.Signal()

    @property
    def scope(self):
        if self._scope:
            return self._scope

        api = chipwhisperer.common.api.CWCoreAPI.CWCoreAPI.getInstance()
        if api:
            return api.getScope()
        else:
            raise ValueError('Programmer needs scope object, retrieval from api failed, please set manually')

    @scope.setter
    def scope(self, value):
        self._scope = value

    def save_pin_setup(self):
        self.pin_setup['pdic'] = self.scope.io.pdic
        self.pin_setup['pdid'] = self.scope.io.pdid
        self.pin_setup['nrst'] = self.scope.io.nrst

    def restore_pin_setup(self):
        self.scope.io.pdic = self.pin_setup['pdic']
        self.scope.io.pdid = self.pin_setup['pdid']
        self.scope.io.nrst = self.pin_setup['nrst']

    def set_pins(self):
        raise NotImplementedError

    def setUSBInterface(self, iface):
        raise DeprecationWarning('find method now includes what setUSBInterface did')

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

    def autoProgram(self, hexfile, erase, verify, logfunc, waitfunc):
        raise NotImplementedError


class AVRProgrammer(Programmer):
    def __init__(self):
        super(AVRProgrammer, self).__init__()

    @save_and_restore_pins
    def find(self):
        avr = self.scope.scopetype.dev.avr
        sig, chip = avr.find()
        if chip is None:
            self.log("AVR: Detected unknown device with signature=%2x %2x %2x" % (sig[0], sig[1], sig[2]))
        else:
            self.log("AVR: Detected device %s" % chip.name)

    @save_and_restore_pins
    def erase(self):
        avr = self.scope.scopetype.dev.avr
        self.log("Erasing Chip")
        avr.eraseChip()

    @save_and_restore_pins
    def autoProgram(self, hexfile, erase, verify, logfunc, waitfunc):
        avr = self.scope.scopetype.dev.avr
        avr.autoProgram(hexfile, erase, verify, logfunc, waitfunc)

    @save_and_restore_pins
    def writeFuse(self, value, lfuse):
        avr = self.scope.scopetype.dev.avr
        avr.writeFuse(value, lfuse)

    @save_and_restore_pins
    def readFuse(self, value):
        avr = self.scope.scopetype.dev.avr
        return avr.readFuse(value)

    @save_and_restore_pins
    def enableSlowClock(self, value):
        avr = self.scope.scopetype.dev.avr
        avr.enableSlowClock(value)

    @save_and_restore_pins
    def program(self, filename, memtype="flash", verify=True):
        avr = self.scope.scopetype.dev.avr
        Programmer.lastFlashedFile = filename
        avr.program(filename, memtype, verify)

    @save_and_restore_pins
    def autoProgram(self, hexfile, erase, verify, logfunc, waitfunc):
        avr = self.scope.scopetype.dev.avr
        avr.autoProgram(hexfile, erase, verify, logfunc, waitfunc)

    @save_and_restore_pins
    def close(self):
        avr = self.scope.scopetype.dev.avr
        try:
            avr.enableISP(False)
        except AttributeError as e:
            logging.info("AVR programmer: could not disable ISP - USB might be disconnected!")


class XMEGAProgrammer(Programmer):

    def __init__(self):
        super(XMEGAProgrammer, self).__init__()
        self.supported_chips = supported_xmega

    @save_and_restore_pins
    def find(self, xmega=None):
        xmega = self.scope.scopetype.dev.xmega
        sig, chip = xmega.find()

        # Print signature of unknown device
        if chip is None:
            self.log("Detected Unknown Chip, sig=%2x %2x %2x" % (sig[0], sig[1], sig[2]))
        else:
             self.log("Detected %s" % chip.name)

    @save_and_restore_pins
    def erase(self, memtype="chip"):
        self.log("Erasing Chip")
        xmega = self.scope.scopetype.dev.xmega
        xmega.erase(memtype)

    @save_and_restore_pins
    def autoProgram(self, hexfile, erase, verify, logfunc, waitfunc):
        xmega = self.scope.scopetype.dev.xmega
        xmega.autoProgram(hexfile, erase, verify, logfunc, waitfunc)

    @save_and_restore_pins
    def program(self, filename, memtype="flash", verify=True):
        Programmer.lastFlashedFile = filename
        xmega = self.scope.scopetype.dev.xmega
        xmega.program(filename, memtype, verify)

    @save_and_restore_pins
    def close(self):
        xmega = self.scope.scopetype.dev.xmega
        xmega.enablePDI(False)

class STM32FProgrammer(Programmer):

    def __init__(self):
        super(STM32FProgrammer, self).__init__()
        self.supported_chips = supported_stm32f

    @save_and_restore_pins
    def open_and_find(self, log_func=None):
        stm32f = self.scope.scopetype.dev.serialstm32f
        stm32f.open_port()
        sig, chip = stm32f.find(logfunc=log_func)

        # logging is done at the lower level

    @save_and_restore_pins
    def open(self):
        stm32f = self.scope.scopetype.dev.serialstm32f
        stm32f.open_port()

    @save_and_restore_pins
    def find(self):
        stm32f = self.scope.scopetype.dev.serialstm32f
        stm32f.scope = self.scope
        sig, chip = stm32f.find()

        # logging is done at the lower level

    @save_and_restore_pins
    def program(self, filename, memtype="flash", verify=True):
        Programmer.lastFlashedFile = filename
        stm32f = self.scope.scopetype.dev.serialstm32f
        stm32f.scope = self.scope
        stm32f.program(filename, memtype, verify)

    @save_and_restore_pins
    def autoProgram(self, hexfile, erase, verify, logfunc, waitfunc):
        stm32f = self.scope.scopetype.dev.serialstm32f
        stm32f.scope = self.scope
        stm32f.autoProgram(hexfile, erase, verify, logfunc, waitfunc)

    @save_and_restore_pins
    def erase(self):
        self.log("Erasing Chip")
        stm32f = self.scope.scopetype.dev.serialstm32f
        stm32f.cmdEraseMemory()

    @save_and_restore_pins
    def close(self):
        stm32f = self.scope.scopetype.dev.serialstm32f
        stm32f.close_port()
        stm32f.releaseChip()
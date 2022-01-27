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
from ..scopes import ScopeTypes
from chipwhisperer.hardware.naeusb.programmer_avr import supported_avr
from chipwhisperer.hardware.naeusb.programmer_xmega import supported_xmega
from chipwhisperer.hardware.naeusb.programmer_stm32fserial import supported_stm32f
from chipwhisperer.hardware.naeusb.programmer_neorv32 import Neorv32Programmer

from functools import wraps

from chipwhisperer.logging import *

from typing import Dict, Optional



class Programmer:
    lastFlashedFile = "unknown"
    _scope : Optional[ScopeTypes] = None
    pin_setup : Dict[str, str] = {}

    def __init__(self):
        self.newTextLog = util.Signal()
        self._scope = None

    def open(self):
        pass

    @property
    def scope(self):
        if self._scope:
            return self._scope

        return None

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
        target_logger.info(text)
        self.newTextLog.emit(text)

    def autoProgram(self, hexfile, erase, verify, logfunc, waitfunc):
        raise NotImplementedError

def save_and_restore_pins(func):
    """Decorator to save and restore pins needed to comunicate and program hardware

        Purpose: to move from changing pins in the background needed to communicate
         with the hardware and leaving them changed (very confusing), to saving the
         pin states before the function is called, changing them during function
         executation and restoring them after the function is done executing
         (less confusing).
        """
    @wraps(func) # updates func_wrapper aatributes to be same
    def func_wrapper(self : Programmer, *args, **kwargs):

        #If no scope, we don't do any pin magic
        if self.scope is None:
            return func(self, *args, **kwargs)

        pin_setup : Dict[str, str]= {
            'pdic': self.scope.io.pdic,
            'pdid': self.scope.io.pdid,
            'nrst': self.scope.io.nrst,
        }
        target_logger.debug('Saving pdic, pdid, and nrst pin configuration')
        # setup the pins so that so communication to the target is possible
        # Important: during the execution of func, the pin values may change if
        # the function is related to reprogramming or resetting the device. Example:
        # the stm32f uses the toggling of the nrst and pdic pins for resetting
        # and boot mode setting respectively
        target_logger.debug('Changing pdic, pdid, and nrst pin configuration')
        if pin_setup['pdic'] != 'high_z':
            self.scope.io.pdic = 'high_z'
        if pin_setup['pdid'] != 'high_z':
            self.scope.io.pdid = 'high_z'
        if pin_setup['nrst'] != 'high_z':
            self.scope.io.nrst = 'high_z'
        try:
            val = func(self, *args, **kwargs)
        finally:
            target_logger.debug('Restoring pdic, pdid, and nrst pin configuration')
            if self.scope.io.pdic != pin_setup['pdic']:
                self.scope.io.pdic = pin_setup['pdic']
            if self.scope.io.pdid != pin_setup['pdid']:
                self.scope.io.pdid = pin_setup['pdid']
            if self.scope.io.nrst != pin_setup['nrst']:
                self.scope.io.nrst = pin_setup['nrst']
        return val # only returns value when decorating a function with return value
    return func_wrapper

class NEORV32Programmer(Programmer):

    @save_and_restore_pins
    def find(self):
        if self.scope is None:
            raise ValueError("Programmer not yet setup")

        self.neorv = Neorv32Programmer(self.scope)
        self.neorv.open_port()
        pass

    @save_and_restore_pins
    def erase(self):
        pass

    @save_and_restore_pins
    def program(self, filename: str, memtype="flash", verify=True):
        if filename.endswith(".hex"):
            scope_logger.error(".bin file required for NEORV32Programmer. Attempting to access .bin file at .hex location")
            filename = filename.replace(".hex", ".bin")
        self.neorv.program(filename)
        pass

    def close(self):
        pass
    

class AVRProgrammer(Programmer):
    def __init__(self, slow_clock = False):
        self.slow_clock = slow_clock
        super(AVRProgrammer, self).__init__()

    @save_and_restore_pins
    def find(self):
        avr = self.scope.scopetype.avr
        sig, chip = avr.find(self.slow_clock)
        if chip is None:
            self.log("AVR: Detected unknown device with signature=%2x %2x %2x" % (sig[0], sig[1], sig[2]))
        else:
            self.log("AVR: Detected device %s" % chip.name)

    @save_and_restore_pins
    def erase(self):
        avr = self.scope.scopetype.avr
        self.log("Erasing Chip")
        avr.eraseChip()

    @save_and_restore_pins
    def autoProgram(self, hexfile, erase, verify, logfunc, waitfunc):
        avr = self.scope.scopetype.avr
        avr.autoProgram(hexfile, erase, verify, logfunc, waitfunc)

    @save_and_restore_pins
    def writeFuse(self, value, lfuse):
        avr = self.scope.scopetype.avr
        avr.writeFuse(value, lfuse)

    @save_and_restore_pins
    def readFuse(self, value):
        avr = self.scope.scopetype.avr
        return avr.readFuse(value)

    @save_and_restore_pins
    def enableSlowClock(self, value):
        if value:
            self.slow_clock = True
        else:
            self.slow_clock = False
        avr = self.scope.scopetype.avr
        avr.enableSlowClock(value)

    @save_and_restore_pins
    def program(self, filename, memtype="flash", verify=True):
        avr = self.scope.scopetype.avr
        Programmer.lastFlashedFile = filename
        avr.program(filename, memtype, verify)

    @save_and_restore_pins
    def close(self):
        avr = self.scope.scopetype.avr
        try:
            avr.enableISP(False)
        except AttributeError as e:
            target_logger.info("AVR programmer: could not disable ISP - USB might be disconnected!")


class XMEGAProgrammer(Programmer):

    def __init__(self):
        super(XMEGAProgrammer, self).__init__()
        self.supported_chips = supported_xmega
        self.xmega = None

    def xmegaprog(self):
        if self.xmega is None:
            xmega = self.scope.scopetype.xmega
        else:
            xmega = self.xmega
        return xmega

    @save_and_restore_pins
    def find(self, xmega=None):
        xmega = self.xmegaprog()
        sig, chip = xmega.find()

        # Print signature of unknown device
        if chip is None:
            self.log("Detected Unknown Chip, sig=%2x %2x %2x" % (sig[0], sig[1], sig[2]))
        else:
            self.log("Detected %s" % chip.name)

    @save_and_restore_pins
    def erase(self, memtype="chip"):
        self.log("Erasing Chip")
        xmega = self.xmegaprog()
        try:
            xmega.erase(memtype)
        except IOError:
            target_logger.info("Full chip erase timed out. Reinitializing programmer and erasing only application memory")
            self.open()
            self.find()
            xmega.enablePDI(False)
            xmega.enablePDI(True)
            xmega.erase("app")

    @save_and_restore_pins
    def autoProgram(self, hexfile, erase, verify, logfunc, waitfunc):
        xmega = self.scope.scopetype.xmega
        xmega.autoProgram(hexfile, erase, verify, logfunc, waitfunc)

    @save_and_restore_pins
    def program(self, filename, memtype="flash", verify=True):
        Programmer.lastFlashedFile = filename
        xmega = self.xmegaprog()
        xmega.program(filename, memtype, verify)

    @save_and_restore_pins
    def close(self):
        xmega = self.xmegaprog()
        xmega.enablePDI(False)

class STM32FProgrammer(Programmer):

    def __init__(self, small_blocks=False, slow_prog=False, baud=115200):
        super(STM32FProgrammer, self).__init__()
        self.supported_chips = supported_stm32f
        self._baud = baud

        self.slow_speed = slow_prog
        self.small_blocks = small_blocks
        self.stm = None

    def stm32prog(self):

        if self.stm is None:
            stm = self.scope.scopetype.serialstm32f
        else:
            stm = self.stm

        stm.slow_speed = self.slow_speed
        stm.small_blocks = self.small_blocks

        return stm

    @save_and_restore_pins
    def open_and_find(self, log_func=None):
        stm32f = self.stm32prog()
        stm32f.open_port()
        sig, chip = stm32f.find(logfunc=log_func)

        # logging is done at the lower level

    @save_and_restore_pins
    def open(self):
        stm32f = self.stm32prog()
        stm32f.open_port(self._baud)

    @save_and_restore_pins
    def find(self):
        stm32f = self.stm32prog()
        stm32f.scope = self.scope
        sig, chip = stm32f.find()

        # logging is done at the lower level

    @save_and_restore_pins
    def program(self, filename, memtype="flash", verify=True):
        Programmer.lastFlashedFile = filename
        stm32f = self.stm32prog()
        stm32f.scope = self.scope
        stm32f.program(filename, memtype, verify)

    @save_and_restore_pins
    def autoProgram(self, hexfile, erase, verify, logfunc, waitfunc):
        stm32f = self.stm32prog()
        stm32f.scope = self.scope
        stm32f.autoProgram(hexfile, erase, verify, logfunc, waitfunc)

    @save_and_restore_pins
    def erase(self):
        self.log("Erasing Chip")
        stm32f = self.stm32prog()
        stm32f.cmdEraseMemory()

    @save_and_restore_pins
    def close(self):
        stm32f = self.stm32prog()
        stm32f.close_port()
        stm32f.releaseChip()

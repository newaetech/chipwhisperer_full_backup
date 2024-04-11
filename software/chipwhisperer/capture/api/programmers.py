# -*- coding: utf-8 -*-
#
# Copyright (c) 2014-2022, NewAE Technology Inc
# All rights reserved.
#
#
# Find this and more at newae.com - this file is part of the chipwhisperer
# project, http://www.chipwhisperer.com 
#
#=================================================
import logging
from chipwhisperer.common.utils import util
from ..scopes import ScopeTypes
from ..scopes import CWNano
from .cwcommon import ChipWhispererCommonInterface
from chipwhisperer.hardware.naeusb.programmer_avr import supported_avr
from chipwhisperer.hardware.naeusb.programmer_xmega import supported_xmega
from chipwhisperer.hardware.naeusb.programmer_stm32fserial import supported_stm32f
from chipwhisperer.hardware.naeusb.programmer_neorv32 import Neorv32Programmer
import time
from ..utils.IntelHex import IntelHex

from functools import wraps

from chipwhisperer.logging import *

from typing import Dict, Optional, Union



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

        pin_setup = [
            'pdic',
            'pdid',
            'nrst',
            'tio3'
        ]
        target_logger.debug('Saving {} pin configuration'.format(pin_setup))
        # setup the pins so that so communication to the target is possible
        # Important: during the execution of func, the pin values may change if
        # the function is related to reprogramming or resetting the device. Example:
        # the stm32f uses the toggling of the nrst and pdic pins for resetting
        # and boot mode setting respectively
        pin_states = []
        for pin in pin_setup:
            state = getattr(self.scope.io, pin)
            target_logger.debug('{} was {}'.format(pin, state))
            pin_states.append(state)
            if state != 'high_z':
                setattr(self.scope.io, pin, 'high_z')

        target_logger.debug('Changing {} pin configuration'.format(pin_setup))

        try:
            val = func(self, *args, **kwargs)
        finally:
            target_logger.debug('Restoring {} pin configuration'.format(pin_setup))
            for i, pin in enumerate(pin_setup):
                target_logger.debug('{} setting to {}'.format(pin, pin_states[i]))
                if getattr(self.scope.io, pin) != pin_states[i]:
                    setattr(self.scope.io, pin, pin_states[i])
        return val # only returns value when decorating a function with return value
    return func_wrapper

from ...hardware.naeusb.bootloader_sam3u import Samba
class SAM4SProgrammer(Programmer):
    def __init__(self, erase_pin='pdic'):
        super().__init__()
        if not erase_pin in ['pdic', 'tio3']:
            raise ValueError("Invalid erase pin {} must be pdic or tio3".format(erase_pin))
        self.erase_pin = erase_pin
        self.prog = None
        self.scope : ScopeTypes = None

    def get_prog(self):
        if self.prog is None:
            if self.scope is None:
                raise OSError("Assign prog.scope before attempting programming")
            self.prog = Samba()
        return self.prog

    @save_and_restore_pins
    def find(self, power_cycle=True):
        if power_cycle:
            # target_logger.info("Power cycling SAM4S")
            # self.scope.io.target_pwr = 0
            # time.sleep(0.2)
            # self.scope.io.target_pwr = 1
            # time.sleep(0.2)
            pass
        target_logger.info("Toggling erase({})/nrst pins".format(self.erase_pin))

        if (self.scope.io.tio1, self.scope.io.tio2) != ("serial_rx", "serial_tx"):
            target_logger.warning("Serial pins incorrect for NewAE SAM4S target") 
            target_logger.warning("(tio1, tio2) != ('serial_rx', 'serial_tx')")
            target_logger.warning("Did you forget to call scope.default_setup()?")

        setattr(self.scope.io, self.erase_pin, 1)
        time.sleep(0.5)
        setattr(self.scope.io, self.erase_pin, None)
        time.sleep(0.5)

        self.scope.io.nrst = 0
        time.sleep(0.5)
        self.scope.io.nrst = None
        time.sleep(0.5)

        self._old_baud = self.scope._get_usart()._baud
        prog = self.get_prog()
        target_logger.info("Connecting to SAMBA")
        prog.con(self.scope)
        target_logger.info("Done!")

    @save_and_restore_pins
    def erase(self):
        target_logger.info("Erasing firmware")
        prog = self.get_prog()
        prog.erase()
        target_logger.info("Done!")

    @save_and_restore_pins
    def program(self, filename : str, memtype="flash", verify=True):
        prog = self.get_prog()
        target_logger.info("Opening firmware...")

        if filename.endswith(".hex"):
            f = IntelHex(filename)
            fw_data = f.tobinarray(start=f.minaddr())
        else:
            fw_data = open(filename, "rb").read()
        
        target_logger.info("Programming...")
        prog.write(fw_data)
        target_logger.info("Verifying...")
        if prog.verify(fw_data):
            # prog.reset()
            target_logger.info("Verify OK, resetting target...")
            prog.flash.setBootFlash(1)
            if not prog.flash.getBootFlash():
                target_logger.warning("Boot flash not set, target may require power cycle")
            prog.reset()
            prog.ser.close()
            return True
        else:
            target_logger.error("Verify FAILED!")
            prog.ser.close()
            raise OSError("Verify FAILED")

    def close(self, reset=True):
        self.scope._get_usart().init(self._old_baud)
        self.scope.io.nrst = 0
        time.sleep(0.2)
        self.scope.io.nrst = None
        time.sleep(0.2)
        pass
        
    
        

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
        self.neorv.program(filename)
        pass

    def close(self):
        self.neorv.close_port()
    
class iCE40Programmer(Programmer):
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
        # only check this for non-Nano, as Nano can't adjust these pins
        if not type(self.scope) is CWNano:
            if (self.scope.io.tio1, self.scope.io.tio2) != ("serial_rx", "serial_tx"):
                target_logger.warning("Serial pins incorrect for NewAE STM32 target") 
                target_logger.warning("(tio1, tio2) != ('serial_rx', 'serial_tx')")
                target_logger.warning("Did you forget to call scope.default_setup()?")
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

#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2015-2016, NewAE Technology Inc
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
#=================================================


import os.path
try:
    from chipwhisperer.common.utils import serialport as scan
except ImportError:
    class scan:
        @staticmethod
        def scan():
            return ["pyserial not installed"]
from chipwhisperer.common.utils import util
from chipwhisperer.hardware.naeusb.bootloader_sam3u import Samba

#The firmware files, may still be useful

from chipwhisperer.common.utils.util import camel_case_deprecated

class SAMFWLoader(object):
    """ Object for easy reprogramming of ChipWhisperers

    This will work with the Lite, the Pro, and
    the Nano. If the ChipWhisperer has already been erased, pass None instead
    of the scope object and skip the enter_bootloader() call. Will also work
    for the CW305.

    Can autoprogram if the ChipWhisperer's firmware has not already been erased.

    Autoprogram Example:

     #. Attach the scope part of the hardware to your computer.

     #. Connect to the scope using::

            import chipwhisperer as cw

            # Connect to scope
            scope = cw.scope()

            # If using CW305, connect to target as well
            # Can ignore msg about no bitstream
            target = cw.target(None, cw.targets.CW305)

    #. Call the auto_program() method::

            programmer = cw.SAMFWLoader(scope=scope)
            # if CW305
            # programmer = cw.SAMFWLoader(scope=target)

            programmer.auto_program()

    Example:

     #. Attach the scope part of the hardware to your computer.

     #. Connect to the scope using::

            import chipwhisperer as cw

            # Connect to scope
            scope = cw.scope()

            # If using CW305, connect to target as well
            # Can ignore msg about no bitstream
            target = cw.target(None, cw.targets.CW305)

     #. Place the hardware in bootloader mode using::

            # use created scope object from previous step
            programmer = cw.SAMFWLoader(scope=scope)

            # or CW305 target object
            programmer = cw.SAMFWLoader(scope=target)

            # WARNING: this will erase the firmware on the device
            # and make it unusable until reprogrammed.
            programmer.enter_bootloader(really_enter=True)

     #. Unplug then plug in the hardware into your computer again.
        The device should be shown as BOSSA in the device manager
        on Windows. Make not of the port number the BOSSA device
        is attached to (for example: COM1, or COM2, and so on)

     #. Program the SAM3U with::

            import chipwhisperer as cw
            programmer = cw.SAMFWLoader(scope=None)

        Two methods:

         #. Using the firmware_path::

                # the firmware file is included with chipwhisperer 
                # and is the .bin file from the FW build
                # (ChipWhisperer Lite) chipwhisperer\hardware\capture\chipwhisperer-lite\sam3u_fw\SAM3U_VendorExample\Debug 
                # directory. 
                programmer.program(<port>, <path to firmware file>)

         #. Using the hardware_type (recommended)::

                programmer.program(<port>, hardware_type='cwlite')
                programmer.program(<port>, hardware_type='cwnano')
                programmer.program(<port>, hardware_type='cw305')
                programmer.program(<port>, hardware_type='cw1200')

        On Linux instead of 'COM#' use the Linux equivalent (usually /dev/ttyACM# or /dev/ttyUSB#)

     #. Once the programming is done. Unplug then plug in the hardware into your
        computer again. The device should show up as a ChipWhisperer again.
    """
    def __init__(self, scope=None, logfunc=print):
        self._hw_type = None
        if scope:
            self.usb = scope._getNAEUSB()
            self._hw_type = scope._getCWType()
        if logfunc is None:
            logfunc = lambda *args, **kwargs: None
        self.logfunc = logfunc
        self._warning_seen = False

    def enter_bootloader(self, really_enter=False):
        """ Enters the bootloader of the ChipWhisperer by erasing the flash

        Only use this if you want to erase the scope's firmware (i.e. the
        ChipWhisperer Lite). This does NOT erase the firmware of a target (
        i.e. an STM32 target). Will raise a Warning the first time an erase
        is attempted and really_enter=False.

        Args:
            really_enter (bool, optional): If True, enter the bootloader
                without ever raising a warning. If False, raise a warning
                if this is the user's first time calling it.

        Raises:
            Warning: really_enter=False and user hasn't seen this warning
                before

        """
        if not really_enter and not self._warning_seen:
            self._warning_seen = True
            raise Warning("""This method puts the SAM3U into bootloader mode

            If you enter the bootloader mode, you must upload new firmware to exit this mode.
            Power cycling will not exit bootloader mode. If you really want to enter bootloader
            mode, recall this method or call as enter_bootloader(really_enter=True).

            See documentation or ask at https://forum.newae.com/ for more information """)


        else:
            self.logfunc("""Entering bootloader mode...
            Please wait until the ChipWhisperer shows up as a serial port. Once it has, call
            the program(COMPORT, FWPATH) to program the ChipWhisperer

            Default firmware can be found at chipwhisperer/hardware/capture/chipwhisperer-lite/sam3u_fw/SAM3U_VendorExample/Debug/SAM3U_CW1173.bin""")
            self.usb.enterBootloader(True)

    def auto_program(self):
        """Erase and program firmware of ChipWhisperer

        Autodetects comport and hardware type.
        """
        import time, serial.tools.list_ports
        if not self._hw_type:
            raise OSError("Unable to detect chipwhisperer hardware type")
        before = serial.tools.list_ports.comports()
        before = [b.device for b in before]
        time.sleep(0.5)
        self.enter_bootloader(True)
        time.sleep(1.5)
        after = serial.tools.list_ports.comports()
        after = [a.device for a in after]
        candidate = list(set(before) ^ set(after))
        if len(candidate) == 0:
            raise OSError("Could not detect COMPORT. Continue using programmer.program()")
        com = candidate[0]
        print("Detected com port {}".format(com))
        self.program(com, hardware_type=self._hw_type)


    def program(self, port, fw_path=None, hardware_type=None, bypass_warning=False):
        """ Program the ChipWhisperer with new firmware.

        Args:
            port (str): Serial port that the ChipWhisperer bootloader is on
            fw_path (str): Path to firmware, if specified leave out hardware type.
            hardware_type (str): The type of hardware that you want to program.
                If specified leave out fw_path. Valid types: (cwlite, cwnano, cw305, cw1200)

        Returns:
            True if programming succeeded, False if it didn't

        """
        type_whitelist = [
            'cwlite',
            'cwnano',
            'cw305',
            'cw1200'
        ]


        if fw_path and hardware_type:
            raise TypeError('Only one or the other can be specified for fw_path, and hardware_type.')

        if not fw_path and not hardware_type:
            TypeError('Either the fw_path or the hardware_type needs to be given.')

        if hardware_type and (not bypass_warning):
            if self._hw_type and self._hw_type != hardware_type:
                raise ValueError("ChipWhisperer hardware type mismatch (expected {}, got {}). If you're sure you have the correct hardware_type, call this again with bypass_warning=True".format(self._hw_type, hardware_type))


        if hardware_type:
            if hardware_type not in type_whitelist:
                message = 'Invalid hardware type {}, needs to be one of: ({})'
                raise TypeError(message.format(hardware_type, ', '.join(type_whitelist)))
            else:
                if hardware_type == 'cwlite':
                    from chipwhisperer.hardware.firmware.cwlite import getsome as getsome
                    name = 'SAM3U_CW1173.bin'
                elif hardware_type == 'cwnano':
                    from chipwhisperer.hardware.firmware.cwnano import getsome as getsome
                    name = 'SAM3U_CWNANO.bin'
                elif hardware_type == 'cw305':
                    from chipwhisperer.hardware.firmware.cw305 import getsome as getsome
                    name = 'SAM3U_CW305.bin'
                elif hardware_type == 'cw1200':
                    from chipwhisperer.hardware.firmware.cw1200 import getsome as getsome
                    name = 'CW1200_SAM3UFW.bin'
                self.logfunc('Loading {} firmware...'.format(hardware_type))
                fw_data = getsome(name).read()

        if fw_path:
            self.logfunc("Opening firmware...")
            fw_data = open(fw_path, "rb").read()
            name = fw_path

        sam = Samba()

        self.logfunc("Opened!\nConnecting...")
        sam.con(port)
        self.logfunc("Connected!\nErasing...")
        sam.erase()
        self.logfunc("Erased!\nProgramming file {}...".format(name))
        sam.write(fw_data)
        self.logfunc("Programmed!\nVerifying...")
        if sam.verify(fw_data):
            self.logfunc("Verify OK!")
            sam.flash.setBootFlash(True)
            self.logfunc("Bootloader disabled. Please power cycle device.")
            sam.ser.close()
            return True
        else:
            self.logfunc("Verify FAILED!")
            sam.ser.close()
            return False



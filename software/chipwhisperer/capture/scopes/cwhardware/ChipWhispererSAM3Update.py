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
from chipwhisperer.hardware.firmware.cwlite import getsome as cwlite_getsome

from chipwhisperer.common.utils.util import camel_case_deprecated

class SAMFWLoader(object):
    """ Object for easy reprogramming of ChipWhisperers

    Despite the name of the file, this will work with the Lite, the Pro, and
    the Nano. If the ChipWhisperer has already been erased, pass None instead
    of the scope object and skip the enter_bootloader() call.

    Example:

     #. Attach the scope part of the hardware to your computer.

     #. Connect to the scope using::

            import chipwhisperer as cw

            # For the ChipWhisperer Lite or Pro
            scope = cw.scope(scope_type=cw.scopes.OpenADC)

            # For the ChipWhisperer Nano
            scope = cw.scope(scope_type=cw.scopes.CWNano)

     #. Place the hardware in bootloader mode using::

            # use created scope object from previous step
            programmer = cw.SAMFWLoader(scope=scope)

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

         #. Using the firmware_path:

                # the firmware file is included with chipwhisperer
                # and is in the .bin file in the
                # (ChipWhisperer Lite) chipwhisperer\hardware\capture\chipwhisperer-lite\sam3u_fw\SAM3U_VendorExample\Debug
                # (ChipWhisperer Pro)
                # directory.
                programmer.program(<port>, <path to firmware file>)

         #. Using the hardware_type::

                programmer.program(<port>, hardware_type='cwlite')

        On linux instead of 'COM#' use the linux equivalent to a port
        'ttyASM#' or equivalent.

     #. Once the programming is done. Unplug then plug in the hardware into your
        computer again. The device should show up as a ChipWhisperer again.
    """
    def __init__(self, scope=None):
        if scope:
            self.usb = scope._getNAEUSB()
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
            print("""Entering bootloader mode...
            Please wait until the ChipWhisperer shows up as a serial port. Once it has, call
            the program(COMPORT, FWPATH) to program the ChipWhisperer

            Default firmware can be found at chipwhisperer/hardware/capture/chipwhisperer-lite/sam3u_fw/SAM3U_VendorExample/Debug/SAM3U_CW1173.bin""")
            self.usb.enterBootloader(True)

    def program(self, port, fw_path=None, hardware_type=None):
        """ Program the ChipWhisperer with new firmware.

        Args:
            port (str): Serial port that the ChipWhisperer bootloader is on
            fw_path (str): Path to firmware, if specified leave out hardware type.
            hardware_type (str): The type of hardware that you want to program.
                If specified leave out fw_path. Valid types: (cwlite, )

        """
        type_whitelist = [
            'cwlite'
        ]

        if fw_path and hardware_type:
            raise TypeError('Only one or the other can be specified for fw_path, and hardware_type.')

        if not fw_path and not hardware_type:
            TypeError('Either the fw_path or the hardware_type needs to be given.')

        if hardware_type:
            if hardware_type not in type_whitelist:
                message = 'Invalid hardware type {}, needs to be one of: ({})'
                raise TypeError(message.format(hardware_type, ', '.join(type_whitelist)))
            else:
                print('Loading {} firmware...'.format(hardware_type))
                fw_data = cwlite_getsome('SAM3U_CW1173.bin').read()

        if fw_path:
            print("Opening firmware...")
            fw_data = open(fw_path, "rb").read()

        sam = Samba()

        print("Opened!\nConnecting...")
        sam.con(port)
        print("Connected!\nErasing...")
        sam.erase()
        print("Erased!\nProgramming file {}...".format(fw_path))
        sam.write(fw_data)
        print("Programmed!\nVerifying...")
        if sam.verify(fw_data):
            print("Verify OK!")
            sam.flash.setBootFlash(True)
            print("Bootloader disabled. Please power cycle device.")
        else:
            print("Verify FAILED!")

        sam.ser.close()


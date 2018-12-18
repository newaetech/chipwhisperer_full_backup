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

class SAMFWLoader(object):
    def __init__(self, scope=None):
        # other class gets NAEUSB object, so we need to get it as well
        if scope:
            self.usb = scope._getNAEUSB()
        self._warning_seen = False

    def enterBootloader(self, really_enter=False):
        if not really_enter and not self._warning_seen:
            self._warning_seen = True
            raise Warning("""This method puts the SAM3U into bootloader mode

            If you enter the bootloader mode, you must upload new firmware to exit this mode.
            Power cycling will not exit bootloader mode. If you really want to enter bootloader
            mode, recall this method or call as enterBootloader(really_enter=True).

            See documentation or ask at https://forum.newae.com/ for more information """)


        else:
            print("""Entering bootloader mode...
            Please wait until the ChipWhisperer shows up as a serial port. Once it has, call
            the program(COMPORT, FWPATH) to program the ChipWhisperer

            Default firmware can be found at chipwhisperer/hardware/capture/chipwhisperer-lite/sam3u_fw/SAM3U_VendorExample/Debug/SAM3U_CW1173.bin""")
            self.usb.enterBootloader(True)

    def program(self, port, fw_path):
        sam = Samba()
        print("Opening firmware...")
        fw_data = open(fw_path, "rb").read()
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


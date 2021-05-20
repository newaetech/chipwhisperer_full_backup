# -*- coding: utf-8 -*-
#
# Copyright (c) 2016, NewAE Technology Inc
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

from chipwhisperer.hardware.naeusb.fpga import FPGA
from chipwhisperer.hardware.naeusb.naeusb import NAEUSB
from chipwhisperer.hardware.naeusb.programmer_avr import AVRISP
from chipwhisperer.hardware.naeusb.programmer_xmega import XMEGAPDI
from chipwhisperer.hardware.naeusb.programmer_stm32fserial import STM32FSerial
from chipwhisperer.hardware.naeusb.serial import USART


class CWLiteUSB(object):
    _name = "ChipWisperer-Lite USB"

    def __init__(self, legacy=False):
        self._cwusb = NAEUSB(legacy)

        # Connect required modules up here
        self.fpga = FPGA(self._cwusb)
        self.xmega = XMEGAPDI(self._cwusb)
        self.avr = AVRISP(self._cwusb)
        self.usart = USART(self._cwusb)
        self.serialstm32f = STM32FSerial(cwserial=self.usart, cwapi=None)

    def get_possible_devices(self, idProduct):
        return self._cwusb.get_possible_devices(idProduct=idProduct)

    def con(self, *args, **kwargs):
        return self._cwusb.con(*args, **kwargs)

    def dis(self):
        pass

    def usbdev(self):
        return self._cwusb


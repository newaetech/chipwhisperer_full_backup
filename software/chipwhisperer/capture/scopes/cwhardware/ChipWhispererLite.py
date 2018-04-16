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

from chipwhisperer.capture.ui.programmers_dialog import XMEGAProgrammerDialog, AVRProgrammerDialog, STM32FProgrammerDialog
from chipwhisperer.common.utils.parameter import Parameterized
from chipwhisperer.hardware.naeusb.fpga import FPGA
from chipwhisperer.hardware.naeusb.naeusb import NAEUSB
from chipwhisperer.hardware.naeusb.programmer_avr import AVRISP
from chipwhisperer.hardware.naeusb.programmer_xmega import XMEGAPDI
from chipwhisperer.hardware.naeusb.programmer_stm32fserial import STM32FSerial
from chipwhisperer.hardware.naeusb.serial import USART
from chipwhisperer.common.api.CWCoreAPI import CWCoreAPI


class CWLiteUSB(Parameterized):
    _name = "ChipWisperer-Lite USB"

    def __init__(self):
        self._cwusb = NAEUSB()

        # Connect required modules up here
        self.fpga = FPGA(self._cwusb)
        self.xmega = XMEGAPDI(self._cwusb)
        self.avr = AVRISP(self._cwusb)
        self.usart = USART(self._cwusb)
        self.serialstm32f = STM32FSerial(cwserial=self.usart, cwapi=CWCoreAPI.getInstance())

        self.getParams().addChildren([
            {'name':"CW-Lite XMEGA Programmer", 'tip':"Open XMEGA Programmer (ChipWhisperer-Lite Only)", 'type':"menu", "action":lambda _:self.getCwliteXMEGA().show()},
            {'name':"CW-Lite AVR Programmer", 'tip':"Open AVR Programmer (ChipWhisperer-Lite Only)", 'type':"menu", "action":lambda _:self.getCwliteAVR().show()},
            {'name':'Serial STM32F Programmer', 'tip':"Open STM32F Programmer (Serial/ChipWhisperer)", 'type':"menu", "action":lambda _:self.getSerialSTM32F().show()}
        ])

    def get_possible_devices(self, idProduct):
        return self._cwusb.get_possible_devices(idProduct=idProduct)

    def con(self, *args, **kwargs):
        return self._cwusb.con(*args, **kwargs)

    # def __del__(self):
    #     print "here"

    def dis(self):
        if self.params is not None:
            self.getParams().delete()
        self.params = None
        # gc.collect()
        # print sys.getrefcount(self)
        # print gc.get_referrers(self)

    def usbdev(self):
        return self._cwusb

    def getCwliteXMEGA(self):
        if not hasattr(self, 'cwliteXMEGA'):
            self.cwliteXMEGA = XMEGAProgrammerDialog()
        return self.cwliteXMEGA

    def getCwliteAVR(self):
        if not hasattr(self, 'cwliteAVR'):
            self.cwliteAVR = AVRProgrammerDialog()
        return self.cwliteAVR

    def getSerialSTM32F(self):
        if not hasattr(self, 'serialSTM32F'):
            self.serialSTM32F = STM32FProgrammerDialog()
        return self.serialSTM32F

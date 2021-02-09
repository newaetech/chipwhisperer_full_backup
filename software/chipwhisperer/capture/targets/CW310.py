from .CW305 import CW305
import logging
import time
import random
from datetime import datetime
import os.path
import re
import io
from ._base import TargetTemplate
from chipwhisperer.hardware.naeusb.naeusb import NAEUSB,packuint32
from chipwhisperer.hardware.naeusb.pll_cdce906 import PLLCDCE906
from chipwhisperer.hardware.naeusb.fpga import FPGA
from chipwhisperer.common.utils import util
from chipwhisperer.common.utils.util import camel_case_deprecated, fw_ver_required

class CW310(CW305):
    def __init__(self, *args, **kwargs):
        # maybe later can hijack cw305 stuff, but for now don't
        pass
        import chipwhisperer as cw
        self._naeusb = NAEUSB()
        self.pll = PLLCDCE906(self._naeusb, ref_freq = 12.0E6)

        self.bytecount_size = 7 # pBYTECNT_SIZE in Verilog

        self.hw = None
        self.oa = None

    def _con(self, scope=None):
        # add more stuff later
        self._naeusb.con(idProduct=[0xC310])
        # self.pll.cdce906init()

    def usb_set_voltage(self, pdo_num, voltage):
        if pdo_num not in [2, 3]:
            raise ValueError("pdo_num must be 2 or 3, {}".format(pdo_num))
        if (voltage > 20 or voltage < 5):
            raise ValueError("Voltage must be between 5 and 20, {}".format(voltage))
        self._naeusb.sendCtrl(0x40, 0, [0x28, 0x89 + (pdo_num - 2) * (0x04)])
        snk_pdo = self._naeusb.readCtrl(0x41, 0, 4)
        voltage *= 20
        voltage = int(voltage)

        snk_pdo[1] &= ~0xFC
        snk_pdo[2] &= ~0x0F
        print(snk_pdo)
        snk_pdo[1] |= ((voltage << 2) & 0xFC)
        snk_pdo[2] |= ((voltage >> 6) & 0x0F)
        print(voltage)
        print(snk_pdo)
        self._naeusb.sendCtrl(0x41, 0, snk_pdo)

    def usb_set_current(self, pdo_num, current):
        if pdo_num not in [2, 3]:
            raise ValueError("pdo_num must be 2 or 3, {}".format(pdo_num))
        if (current > 5 or current < 0.5):
            raise ValueError("Voltage must be between 0.5 and 20, {}".format(voltage))
        snk_pdo = self._naeusb.readCtrl(0x41, 0, 4)
        current *= 100
        current = int(current)

        snk_pdo[0] &= ~0xFF
        snk_pdo[1] &= ~0x03
        print(snk_pdo)
        snk_pdo[0] |= (current & 0xFF)
        snk_pdo[1] |= ((current >> 8) & 0x03)
        print(current)
        print(snk_pdo)
        self._naeusb.sendCtrl(0x41, 0, snk_pdo)

    def usb_negotiate_pdo(self):
        #soft reset
        self._naeusb.sendCtrl(0x40, 0, [0x28, 0x51])
        self._naeusb.sendCtrl(0x41, 0, [0x0D])

        #send reset on pdo bus
        self._naeusb.sendCtrl(0x40, 0, [0x28, 0x1A])
        self._naeusb.sendCtrl(0x41, 0, [0x26])

    def _dis(self):
        if self._naeusb:
            self._naeusb.close()

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
import time
import os.path
# import chipwhisperer.capture.scopes._qt as openadc_qt
from chipwhisperer.capture.scopes.cwhardware.ChipWhispererFWLoader import CWLite_Loader, CW1200_Loader, CWHusky_Loader
from chipwhisperer.capture.scopes.cwhardware.ChipWhispererFWLoader import FWLoaderConfig
from chipwhisperer.common.utils.util import DictType, camel_case_deprecated
from chipwhisperer.logging import *

from chipwhisperer.hardware.naeusb.fpga import FPGA
from chipwhisperer.hardware.naeusb.naeusb import NAEUSB
from chipwhisperer.hardware.naeusb.programmer_avr import AVRISP
from chipwhisperer.hardware.naeusb.programmer_xmega import XMEGAPDI
from chipwhisperer.hardware.naeusb.programmer_stm32fserial import STM32FSerial
from chipwhisperer.hardware.naeusb.serial import USART

class OpenADCInterface_NAEUSBChip:
    _name = "NewAE USB (CWLite/CW1200)"

    def __init__(self):
        self.ser = NAEUSB()

        self.fpga = FPGA(self.ser)
        self.xmega = XMEGAPDI(self.ser)
        self.avr = AVRISP(self.ser)
        self.usart = USART(self.ser)
        self.serialstm32f = STM32FSerial(cwserial=self.usart, cwapi=None)

        self.scope = None
        self.last_id = None

        self.cwFirmwareConfig = {
            0xACE2:FWLoaderConfig(CWLite_Loader()),
            0xACE3:FWLoaderConfig(CW1200_Loader()),
            0xACE5:FWLoaderConfig(CWHusky_Loader())
        }

    def con(self, sn=None, bitstream=None, force=False):
        # try:
        nae_products = [0xACE2, 0xACE3, 0xACE5]
        found_id = self.ser.con(idProduct=nae_products, serial_number=sn)
        if force:
            self.fpga.eraseFPGA()
            time.sleep(0.5)

        if found_id != self.last_id:
            scope_logger.info("Detected ChipWhisperer with USB ID %x - switching firmware loader" % found_id)
        self.last_id = found_id

        self.getFWConfig().setInterface(self.fpga)
        # XXX: need to comment this out?
        try:
            if bitstream is None:
                self.getFWConfig().loadRequired()
            else:
                bsdata = open(bitstream, "rb")
                self.fpga.FPGAProgram(bsdata)
        except:
            self.ser.close()
            raise

    def reload_fpga(self, bitstream):
        if bitstream is None:
            raise NotImplementedError("Oops I forgot about that")
        
        bsdate = time.ctime(os.path.getmtime(bitstream))
        scope_logger.debug("FPGA: Using file %s"%bitstream)
        scope_logger.debug("FPGA: File modification date %s"%bsdate)

        bsdata = open(bitstream, "rb")

        try:
            self.fpga.FPGAProgram(bsdata)
        except:
            self.ser.close()
            raise

    def dis(self):
        if self.ser is not None:
            self.ser.close()
            self.ser = None
        else:
            scope_logger.error("Scope already disconnected!")

    def __del__(self):
        if self.ser is not None:
            self.ser.close()

    def getFWConfig(self):
        try:
            return self.cwFirmwareConfig[self.last_id]
        except KeyError as e:
            return FWLoaderConfig(CWLite_Loader())

    def get_name(self):
        return self._name
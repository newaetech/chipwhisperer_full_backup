#
# Copyright (c) 2022, NewAE Technology Inc
# All rights reserved.
#
# Find this and more at newae.com - this file is part of the chipwhisperer
# project, http://www.chipwhisperer.com . ChipWhisperer is a registered
# trademark of NewAE Technology Inc in the US & Europe.
#
#    This file is part of chipwhisperer.
#
#    Licensed under the Apache License, Version 2.0 (the "License");
#    you may not use this file except in compliance with the License.
#    You may obtain a copy of the License at
#
#       http://www.apache.org/licenses/LICENSE-2.0
#
#    Unless required by applicable law or agreed to in writing, software
#    distributed under the License is distributed on an "AS IS" BASIS,
#    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#    See the License for the specific language governing permissions and
#    limitations under the License.
#==========================================================================
import time
import logging
from ...common.utils import util
from .naeusb import packuint32, NAEUSB
from ...logging import *

class SPI(object):
    """Low-level communicates with the SPI lines (MISO/MOSI/SCK) on the 20-pin interface.

    WARNING: This overrides the `NRST` output pin control, as the SPI core is used for AVR
             programming. Currently you need to set the `nrst_default` (defaults to high)
             as the `NRST` output will change when you call the `.enable()` function. You
             need to use the `set_nrst()` functions if you need to toggle the nrst pin
             while SPI is active.

    You can ignore the CS pin if you control it manually. But if a CS pin is passed the
    function will control it based on your choice.

    Example::

        spi = SPI(scope._getNAEUSB(), cs_line=(scope.io, "pdid"))
        response = spi.transfer([0x9A, 0x7E, 0x00])

    """

    CMD_SPI = 0x35

    SPI_CMD_INIT = 0xA0
    SPI_CMD_DEINIT = 0xA1
    SPI_CMD_DATA = 0xA4
    SPI_CMD_NRST_LOW = 0xA5
    SPI_CMD_NRST_HIGH = 0xA6
    SPI_CMD_NRST_INPUT = 0xA7
    SPI_CMD_TOGGLE_SCK = 0xA8

    def __init__(self, usb: NAEUSB, timeout=200, nrst_default="high", cs_line=None):
        self.sendCtrl = usb.sendCtrl
        self.readCtrl = usb.readCtrl
        usb.check_feature("TARGET_SPI", raise_exception=True)
        self._usb = usb
        self._timeout = timeout

        if cs_line:
            self.cs_line_obj = cs_line[0]
            self.cs_line_attr = cs_line[1]
        else:
            self.cs_line_obj = None
            self.cs_line_attr = None

        if nrst_default == "high":
            self.nrst_init = self.SPI_CMD_NRST_HIGH
        elif nrst_default == "low":
            self.nrst_init = self.SPI_CMD_NRST_LOW
        #High-Z does't work as this is fed to the FPGA, not directly to output.
        #so a floating nRST will map to something else on FPGA output!
        #elif nrst_default == "high_z":
        #    self.nrst_init = self.SPI_CMD_NRST_INPUT
        else:
            raise ValueError("Invalid nrst_default value, must be 'high' or 'low'")
        
        self.spi_enabled = False
    
    def set_cs(self, state):
        """Sets CS pin high or low, if no CS pin was configured on init call this
        function silently does nothing.
        """

        if self.cs_line_obj:
            setattr(self.cs_line_obj, self.cs_line_attr, state)
    
    def enable(self, speed=400E3, set_cs_high=True):
        """Enables the SPI interface, which takes control of SPI lines & nRST."""
        speed_data = packuint32(int(speed))

        if set_cs_high:
            self.set_cs(True)

        self.sendCtrl(self.CMD_SPI, self.nrst_init)
        self.sendCtrl(self.CMD_SPI, self.SPI_CMD_INIT, speed_data)
        util.chipwhisperer_extra.cwEXTRA.setAVRISPMode(True)

    def disable(self, set_cs_highz=True):
        """Disables the SPI interface, returns SPI lines back to default."""
        util.chipwhisperer_extra.cwEXTRA.setAVRISPMode(False)
        self.sendCtrl(self.CMD_SPI, self.SPI_CMD_DEINIT)
        self.sendCtrl(self.CMD_SPI, self.SPI_CMD_NRST_INPUT)

        if set_cs_highz:
            self.set_cs(None)

    def transfer(self, data, start=True, stop=True, writeonly=False):
        """Transfers arbitrary amount of data, dropping cs (if `start` is True)
        and raising it after (if `stop` is set)."""

        if start:
            self.set_cs(False)
        
         

        if len(data) <= 64:
            readdata = self.transfer_max64(data, writeonly=writeonly)
        else:
            si = 0
            chunk = 64
            ei = si + chunk
            nowdone = False
            readdata = []

            while True:
                wdata = data[si:ei]
                if len(wdata):
                    rdata = self.transfer_max64(wdata, writeonly=writeonly)
                    if writeonly is True:
                        readdata = None
                    else:
                        readdata.extend(rdata)
                    #print("%d %d: %d"%(ei, si, len(data[si:ei])))

                if nowdone:
                    break

                si = ei
                ei = si + chunk

                if ei > len(data):
                    ei = len(data)
                    nowdone = True
        
        if stop:
            self.set_cs(True)
        
        return readdata
    
    def transfer_max64(self, data, writeonly=False):
        """Transfers up to 64 bytes, normally internal function as no CS control!
        """

        if len(data) == 0:
            raise ValueError("Length of input data must be > 0")

        self.sendCtrl(self.CMD_SPI, self.SPI_CMD_DATA, data)

        #Ignore input data
        if writeonly:
            return None
        
        # Read already happened on HW side, this just reads back from buffer
        readdata = self.readCtrl(self.CMD_SPI, self.SPI_CMD_DATA, len(data))
        return readdata
    
    def toggle_sck(self, toggles, mosistate=False):
        """Toggles SCK line an aribtrary number of times (sometimes required for synchronizing states).
        
        Does not respect the SCK setting, only uses a fixed SCK value."""

        if toggles < 1 or toggles > 0xFF:
            raise ValueError("Toggles must be in range 1 to 255")

        self.sendCtrl(self.CMD_SPI, self.SPI_CMD_TOGGLE_SCK, [int(toggles), int(mosistate)])
        
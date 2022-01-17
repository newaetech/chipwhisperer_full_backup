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

from typing import cast
import time, os, logging
from ...logging import *
from chipwhisperer.hardware.naeusb.fpga import FPGA
from chipwhisperer.hardware.naeusb.spi import SPI
from chipwhisperer.common.utils import util


class FPGASlaveSPI(object):
    """Generic 'Slave Serial' programming algorithm used by many Xilinx & Lattice
       FPGAs. Use vendor-specific classes which can parse regular bitstream files.
    """

    def __init__(self, scope, cs_line=None):
        self.scope = scope
        self.csline = cs_line

class LatticeICE40(FPGASlaveSPI):
    """Lattice ICE40 SPI programming algorithm for directly programming CRAM.
    
    Physical connections (ChipWhisperer <-> ICE40)
     * SCK/MOSI to SCK/SI (optional - MISO/SO as well)
     * nRST to CRST
     * PDIC to CDONE (optional - read not supported on CW-Lite/CW-1200)
     * PDID to CS

    Example::

        import chipwhisperer as cw
        from chipwhisperer.hardware.naeusb.programmer_targetfpga import LatticeICE40

        # Connect to scope
        scope = cw.scope()

        lattice = LatticeICE40(scope)

        lattice.erase_and_init()
        lattice.program("example_file.bin"", sck_speed=20E6)
    
    WARNING: Current the `CDONE` output is not checked!
    """

    def __init__(self, scope):
        super().__init__(scope, cs_line="pdid")
    
        self.cdone = "pdic"
        self.crst = "nrst"


    def erase_and_init(self):
        """Erase the ice40 CRAM and prepare device for slave SPI mode"""

        #DONE pin is high-z
        setattr(self.scope.io, self.cdone, None)

        #CS pin must be low when toggling CRST
        setattr(self.scope.io, self.csline, False)
        setattr(self.scope.io, self.crst, False)
        time.sleep(0.001)
        setattr(self.scope.io, self.crst, True)
        time.sleep(0.001)
        setattr(self.scope.io, self.csline, True)

    def program(self, bs_path, sck_speed=1E6, use_fast_usb=True):
        """Program bitstream file. By default uses a faster mode, you can set
           `sck_speed` up to around 20E6 successfully."""
        
        filestats = os.stat(bs_path)
        modtime = time.ctime(filestats.st_mtime)

        target_logger.info("Bitstream modified time : ", modtime )

        #Need to take control of ISP lines for erase to work
        util.chipwhisperer_extra.cwEXTRA.setAVRISPMode(False)

        self.erase_and_init()

        if use_fast_usb:
            bsfile = open(bs_path, "rb")
            try:
                fastfpga = FPGA(self.scope._getNAEUSB(), prog_mask=0xB0)
                util.chipwhisperer_extra.cwEXTRA.setAVRISPMode(True)

                #CS pin low
                setattr(self.scope.io, self.csline, False)

                fastfpga.FPGAProgram(bsfile, prog_speed=sck_speed, starting_offset=0)
            
            finally:
                util.chipwhisperer_extra.cwEXTRA.setAVRISPMode(False)
                bsfile.close()
        else:
            bsfile = open(bs_path, "rb")
            data = bsfile.read()
            bsfile.close()
            try:
                spi = SPI(self.scope._getNAEUSB())
                spi.enable(sck_speed)
                spi.transfer(data, writeonly=True)
                spi.transfer([0]*256, writeonly=True)
            finally:
                spi.disable()
        
        #PDIC doesn't support read, oops!
        #cdone = getattr(self.scope.io, self.cdone)

    def check_busy(self, spi, preclocks=600):
        """Sending 0x05, 0x00 seems to check if device is busy, needs some SCK toggles w/o CS being low first."""
        spi.transfer([0xFF]*preclocks, stop=False, start=False, writeonly=True)
        resp = spi.transfer([0x05, 0x00])
        return resp[1]

    def wait_until_notbusy(self, spi, max_times=5):
        """Potentially checks if device is busy or not (unknown exact command)"""
        for i in range(0, max_times):
            bsy = self.check_busy(spi)
            if bsy == 0x01:
                continue
            elif bsy == 0x00:
                return
            else:
                raise IOError("Invalid busy response: 0x%02X"%bsy)
        raise IOError("Device never indicated ready.")

    def check_idcode(self, csname="pdid"):
        """Reads idcode (this is undocumented feature of ICE40 so very beta).
        
        Requires MISO connection (not always required). Only tested with certain
        devices, so if this fails try programming anyway.
        """

        #Need to take control of ISP lines for erase to work
        util.chipwhisperer_extra.cwEXTRA.setAVRISPMode(False)

        self.erase_and_init()

        try:
            spi = SPI(self.scope._getNAEUSB(), cs_line=(self.scope.io, csname))
            spi.enable(1E6)
            spi.transfer([0x7E, 0xAA, 0x99, 0x7E, 0x01, 0x0E])

            self.wait_until_notbusy(spi)

            spi.transfer([0xFF]*2, stop=False, start=False, writeonly=True)
            spi.transfer([0x82, 0x00, 0x00, 0x20, 0x00, 0x15, 0xF2, 0xF0, 0xA2, 0x00, 0x00, 0x00])

            self.wait_until_notbusy(spi)

            spi.transfer([0xFF]*2, stop=False, start=False, writeonly=True)
            spi.transfer([0x83, 0x00, 0x00, 0x25, 0x20])

            self.wait_until_notbusy(spi)

            spi.transfer([0xFF]*2, stop=False, start=False, writeonly=True)
            spi.transfer([0x03], stop=False)
            full_idcode = spi.transfer([0x0]*13, start=False, stop=False)

            # I think this is ID code? No documentation though.
            # Ohter bytes
            idcode = full_idcode[12]

            #Other data used for finerprinting(? not done by ice)
            full_afteridcode = spi.transfer([0x0]*128, start=False)

            # ice programmer does the following, shouldn't matter since we are done with
            # the idcoe read by now?
            self.wait_until_notbusy(spi)
            spi.transfer([0xFF]*2, stop=False, start=False, writeonly=True)
            spi.transfer([0x83, 0x00, 0x00, 0x25, 0x00])
        finally:
            spi.disable()
        
        if idcode == 0x20:
            name = "iCE40UP5K"
        elif idcode == 0x12:
            name = "iCE5LP4K"
        else:
            name = "unknown"
        
        return (name, idcode, list(full_idcode), list(full_afteridcode))
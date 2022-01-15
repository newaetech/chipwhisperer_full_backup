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
import time
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
                spi.transfer([0]*128, writeonly=True)
            finally:
                spi.disable()
        
        #PDIC doesn't support read, oops!
        #cdone = getattr(self.scope.io, self.cdone)

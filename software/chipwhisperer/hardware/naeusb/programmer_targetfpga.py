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

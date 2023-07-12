from .CW310 import CW310
from ...hardware.naeusb.pll_cdce906 import PLLCDCE906
from ...logging import *

class CW340(CW310):
    PROG_MODES = ["serial", "parallel", "parallel16"]
    DEFAULT_PROG_SPEED = [int(20E6), 4, 5]
    REASONABLE_SPEEDS = [[int(1E3), int(20E6)+1], [1, 128+32], [1, 128+32]]
    platform="cw340"
    bytecount_size=0

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.bytecount_size = 0
        self.pll = PLLCDCE906(self._naeusb, ref_freq = 12.0E6, board="CW340")

    def _con(self, scope=None, bsfile=None, force=False, fpga_id=None, defines_files=None, slurp=True, prog_speed=None, sn=None, hw_location=None, prog_mode="serial"):
        self._naeusb.con(idProduct=[0xC340], serial_number=sn, hw_location=hw_location)
        if prog_mode not in self.PROG_MODES:
            raise ValueError("Unknown prog mode {}, valid are {}".format(prog_mode, self.PROG_MODES))
        if prog_speed is None:
            prog_speed = self.DEFAULT_PROG_SPEED[self.PROG_MODES.index(prog_mode)]
        
        prog_speed = int(prog_speed)

        if prog_speed not in range(*self.REASONABLE_SPEEDS[self.PROG_MODES.index(prog_mode)]):
            raise ValueError("Invalid prog speed {} for prog mode {}, try {}".format(prog_speed, prog_mode, self.REASONABLE_SPEEDS[self.PROG_MODES.index(prog_mode)]))
        self.pll.cdce906init()
        if fpga_id:
            target_logger.warning("fpga_id is currently unused")

        if defines_files is None:
            if fpga_id is None:
                verilog_defines = [self.default_verilog_defines_full_path]
            else:
                from ...hardware.firmware.cw305 import getsome
                verilog_defines = [getsome(self.default_verilog_defines)]
        else:
            verilog_defines = defines_files
        if slurp:
            self.slurp_defines(verilog_defines)

        if bsfile and (force or not self.fpga.isFPGAProgrammed()):
            status = self.fpga.FPGAProgram(open(bsfile, "rb"), prog_speed=prog_speed, prog_mode=self.PROG_MODES.index(prog_mode))


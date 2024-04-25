from .CW310 import CW310
from ...hardware.naeusb.pll_cdce906 import PLLCDCE906
from ...logging import *
from ...hardware.naeusb.serial import USART
from ...hardware.naeusb.fpga import FPGA

class CW340(CW310):
    """CW340 Luna Board target object.

    This class contains the public API for the CW340 hardware.
    To connect to the CW340, the easiest method is::

        import chipwhisperer as cw
        scope = cw.scope()

        # scope can also be None here, unlike with the default SimpleSerial
        target = cw.target(scope,
                targets.CW340, bsfile=<valid FPGA bitstream file>)

    Inherits the CW305 and CW310 objects, so you can use the same methods as
    the CW305/CW310, provided the register interface in your FPGA build is the
    same.

    To use the methods provided by this class to communicate with the FPGA, a
    jumper cable must be connected from:
        * the lower left pin of the blue J24 header (pin 13, USBSPARE3 on the schematic) 
    to:
        * pin 1 of the J2 header (part of the long 47x2 header next to the FPGA connector, 
        OT_IOA0 on the schematic).

    You can also set the voltage and current settings for the USB-C Power port on the CW340

        # set USB PDO 3 to 20V 5A
        target.usb_set_voltage(3, 20)
        target.usb.set_current(3, 5)

        # renegotiate PDO (applies above settings)
        target.usb_negotiate_pdo()

    For more help about CW340 settings, try help() on this CW340 submodule:

       * target.pll
    """

    PROG_MODES = ["serial", "parallel", "parallel16"]
    DEFAULT_PROG_SPEED = [int(20E6), 4, 5]
    REASONABLE_SPEEDS = [[int(1E3), int(20E6)+1], [1, 128+32], [1, 128+32]]
    platform="cw340"
    _name = "ChipWhisperer CW340 (Ultrascale)"

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.bytecount_size = 0
        # TODO- temporary until these are added to the parsed defines file
        self.REG_XADC_DRP_ADDR = 0xc0
        self.REG_XADC_DRP_DATA = 0xc1
        self.REG_XADC_STAT     = 0xc2
        self.pll = PLLCDCE906(self._naeusb, ref_freq = 12.0E6, board="CW340")
        self.jumper_warned = False

    def _getCWType(self):
        return "luna"

    def _get_usart(self, num=0):
        if num == 0:
            return self._usart0
        elif num == 1:
            return self._usart1
        else:
            raise ValueError("Invalid usart {}".format(num))

    def _get_fpga_programmer(self):
        return self.fpga

    def _con(self, scope=None, bsfile=None, force=False, fpga_id=None, defines_files=None, slurp=True, prog_speed=None, sn=None, hw_location=None, prog_mode="serial"):
        self._naeusb.con(idProduct=[0xC340], serial_number=sn, hw_location=hw_location)
        self.fpga = FPGA(self._naeusb)
        self._usart0 = USART(self._naeusb, usart_num=0)
        self._usart1 = USART(self._naeusb, usart_num=1)
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

    @property
    def vaux12(self):
        return self.get_xadc_vaux(12)

    def jumper_warning_check(fn):
        def inner(self=None, *args, **kwargs):
            if not self.jumper_warned:
                target_logger.warning("""
                    Reading/writing the FPGA with this class requires a jumper cable from:
                    - the third bottom left pin of the blue J24 header (pin 9, USB_SPI_CS on the schematic) 
                    to:
                    - the third bottom left pin of the blue J23 header (pin 9, OT_SPI_DEV_CS_L on the schematic)
                    You can also simply connect a ribbon cable between J23 and J24.
                    This warning is printed once as a reminder; set target.jumper_warned to True to
                    avoid seeing it.
                    """)
                self.jumper_warned = True
            return fn(self, *args, **kwargs)
        return inner

    @jumper_warning_check # type: ignore
    def fpga_write(self, *args, **kwargs):
        return super().fpga_write(*args, **kwargs)

    @jumper_warning_check # type: ignore
    def fpga_read(self, *args, **kwargs):
        return super().fpga_read(*args, **kwargs)



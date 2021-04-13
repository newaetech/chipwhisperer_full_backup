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
        self.fpga = FPGA(self._naeusb)

        self.hw = None
        self.oa = None

        self._woffset_sam3U = 0x000
        self.default_verilog_defines = 'cw305_defines.v'
        self.default_verilog_defines_full_path = os.path.dirname(cw.__file__) +  '/../../hardware/victims/cw305_artixtarget/fpga/common/' + self.default_verilog_defines
        self.registers = 12 # number of registers we expect to find
        self.bytecount_size = 7 # pBYTECNT_SIZE in Verilog

        self._clksleeptime = 1
        self._clkusbautooff = True
        self.last_key = bytearray([0]*16)
        self.target_name = 'AES'

    def _con(self, scope=None, bsfile=None, force=False, fpga_id=None, defines_files=None, slurp=True):
        # add more stuff later
        self._naeusb.con(idProduct=[0xC310])
        # self.pll.cdce906init()

        if defines_files is None:
            if fpga_id is None:
                verilog_defines = [self.default_verilog_defines_full_path]
            else:
                from chipwhisperer.hardware.firmware.cw305 import getsome
                verilog_defines = [getsome(self.default_verilog_defines)]
        else:
            verilog_defines = defines_files
        if slurp:
            self.slurp_defines(verilog_defines)

        if bsfile:
            status = self.fpga.FPGAProgram(open(bsfile, "rb"))

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

    def go_reg(self):
        """Disable USB clock (if requested), perform encryption, re-enable clock"""
        self.fpga_write(self.REG_USER_LED, [0x01])

        self.fpga_write(self.REG_CRYPT_GO, [1])
        time.sleep(0.01)
        self.fpga_write(self.REG_CRYPT_GO, [0])

    def gpio_mode(self, timeout=200):
        """Allow arbitrary GPIO access on SAM3U
        
        Allows low-level IO access to SAM3U GPIO, and also SPI transfers.
        (see documentation on the returned object for more info)

        Args:
            timeout (int): USB timeout in ms. Defaults to 200.

        Returns:
            A FPGAIO object which can be used to access IO on the CW305.
        """
        io = FPGAIO(self._naeusb, timeout)
        return io


class FPGAIO:
    """ User IO to override external bus.
    
    Allows you to use any pin on the SAM3U as a user IO. This includes pins
    such as the external IO interface, and basically anything else you can find.
    
    The pin names are strings, and come from one of three sources:
        * SAM3U pin names, such as "PC11", "PB9", etc.
        * Net names from the CW305 schematic such as "USB_A20".
        * The FPGA ball location that is connected to the SAM3U pin, such as "M2".
    
    Any function taking a pin name assumes you pass a string with one of those. You
    do not need to specify your source - it will autodetect the pin name (if possible).
    The SAM3U pin names allow access to every pin, including those which are not actually
    connected on the PCB itself.

    Beyond simple GPIO toggling, a bit-banged SPI interface can be defined and connected
    to any of the SAM3U pins. The bit-banged interface is done on the SAM3U microcontroller,
    with a default SCK frequency of around 1.5 MHz. You can additionally define waitstates
    to slow down this clock.
    
    Basic usage::
    
        fpga = cw.target(None, cw.targets.CW305)
        io = target.gpio_mode()
        
        # Take over the SAM3U blue LED (normally controlled by firmware, won't be after this)
        import time
        io.pin_set_output("LED_BLUE")
        io.pin_set_state("LED_BLUE", 0)
        time.sleep(0.5)
        io.pin_set_state("LED_BLUE", 1)

        # Example - toggle pin associated with FPGA pin C1 (would be USB_A11)
        import time
        io.pin_set_output("C1")
        io.pin_set_state("C1", 0)
        time.sleep(0.1)
        io.pin_set_state("C1", 1)

        # Setup a SPI interface based on schematic net names
        io.spi1_setpins(sdo="USB_A20", sdi="USB_A19", sck="USB_A18", cs="USB_A17")
        io.spi1_enable(True)

        somedata = [0x11, 0x22, 0x33]

        response = io.spi1_transfer(somedata)
        print(response)

    If you want to see all possible pin names, you can access them with:
        io.SAM3X_PIN_NAMES.keys()
        io.SCHEMATIC_PIN_NAMES.keys()
        io.FPGA_PIN_NAMES.keys()
    """

    REQ_FPGAIO_UTIL      = 0x34
    REQ_IO_CONFIG        = 0xA0
    REQ_IO_RELEASE       = 0xA1
    REQ_IO_OUTPUT        = 0xA2


    CONFIG_PIN_INPUT     = 0x01
    CONFIG_PIN_OUTPUT    = 0x02
    CONFIG_PIN_SPI1_SDO  = 0x10
    CONFIG_PIN_SPI1_SDI  = 0x11
    CONFIG_PIN_SPI1_SCK  = 0x12
    CONFIG_PIN_SPI1_CS   = 0x13

    REQ_FPGASPI1_XFER    = 0x35
    REQ_ENABLE_SPI       = 0xA0
    REQ_DISABLE_SPI      = 0xA1    
    REQ_CS_LOW           = 0xA2
    REQ_CS_HIGH          = 0xA3
    REQ_SEND_DATA        = 0xA4
    
    SAM3X_PIN_NAMES = { "PA0":  0,
                        "PA1":  1,
                        "PA2":  2,
                        "PA3":  3,
                        "PA4":  4,
                        "PA5":  5,
                        "PA6":  6,
                        "PA7":  7,
                        "PA8":  8,
                        "PA9":  9,
                        "PA10": 10,
                        "PA11": 11,
                        "PA12": 12,
                        "PA13": 13,
                        "PA14": 14,
                        "PA15": 15,
                        "PA16": 16,
                        "PA17": 17,
                        "PA18": 18,
                        "PA19": 19,
                        "PA20": 20,
                        "PA21": 21,
                        "PA22": 22,
                        "PA23": 23,
                        "PA24": 24,
                        "PA25": 25,
                        "PA26": 26,
                        "PA27": 27,
                        "PA28": 28,
                        "PA29": 29,
                        "PB0":  32,
                        "PB1":  33,
                        "PB2":  34,
                        "PB3":  35,
                        "PB4":  36,
                        "PB5":  37,
                        "PB6":  38,
                        "PB7":  39,
                        "PB8":  40,
                        "PB9":  41,
                        "PB10": 42,
                        "PB11": 43,
                        "PB12": 44,
                        "PB13": 45,
                        "PB14": 46,
                        "PB15": 47,
                        "PB16": 48,
                        "PB17": 49,
                        "PB18": 50,
                        "PB19": 51,
                        "PB20": 52,
                        "PB21": 53,
                        "PB22": 54,
                        "PB23": 55,
                        "PB24": 56,
                        "PB25": 57,
                        "PB26": 58,
                        "PB27": 59,
                        "PB28": 60,
                        "PB29": 61,
                        "PB30": 62,
                        "PB31": 63,
                        "PC0":  64,
                        "PC1":  65,
                        "PC2":  66,
                        "PC3":  67,
                        "PC4":  68,
                        "PC5":  69,
                        "PC6":  70,
                        "PC7":  71,
                        "PC8":  72,
                        "PC9":  73,
                        "PC10": 74,
                        "PC11": 75,
                        "PC12": 76,
                        "PC13": 77,
                        "PC14": 78,
                        "PC15": 79,
                        "PC16": 80,
                        "PC17": 81,
                        "PC18": 82,
                        "PC19": 83,
                        "PC20": 84,
                        "PC21": 85,
                        "PC22": 86,
                        "PC23": 87,
                        "PC24": 88,
                        "PC25": 89,
                        "PC26": 90,
                        "PC27": 91,
                        "PC28": 92,
                        "PC29": 93,
                        "PC30": 94,
                        "PD0":  96,
                        "PD1":  97,
                        "PD2":  98,
                        "PD3":  99,
                        "PD4":  100,
                        "PD5":  101,
                        "PD6":  102,
                        "PD7":  103,
                        "PD8":  104,
                        "PD9":  105,
                        "PD10": 106}

    FPGA_PIN_NAMES = {"A7":"USB_D0",
                    "B6":"USB_D1",
                    "D3":"USB_D2",
                    "E3":"USB_D3",
                    "F3":"USB_D4",
                    "B5":"USB_D5",
                    "K1":"USB_D6",
                    "K2":"USB_D7",
                    "F4":"USB_A0",
                    "G5":"USB_A1",
                    "J1":"USB_A2",
                    "H1":"USB_A3",
                    "H2":"USB_A4",
                    "G1":"USB_A5",
                    "G2":"USB_A6",
                    "F2":"USB_A7",
                    "E1":"USB_A8",
                    "E2":"USB_A9",
                    "D1":"USB_A10",
                    "C1":"USB_A11",
                    "K3":"USB_A12",
                    "L2":"USB_A13",
                    "J3":"USB_A14",
                    "B2":"USB_A15",
                    "C7":"USB_A16",
                    "C6":"USB_A17",
                    "D6":"USB_A18",
                    "C4":"USB_A19",
                    "D5":"USB_A20",
                    "A4":"USBRD",
                    "C2":"USBWR",
                    "A3":"USBCE",
                    "A2":"USBALE",
                    "A5":"USBSPARE0",
                    "B4":"USBSPARE1",
                    "B1":"USBSPARE2"}

    SCHEMATIC_PIN_NAMES = {"USBSPARE0":"PC10",
                        "USBSPARE1":"PC11",
                        "USBSPARE2":"PC12",
                        "USBSPARE3":"PC13",
                        "USBRD":"PA29",
                        "USBWR":"PC18",
                        "USBCE":"PA6",
                        "USBALE":"PC17",
                        "USBCK0":"PB22",
                        "USBCK1":"PA24",
                        "USB_A0":"PC21",
                        "USB_A1":"PC22",
                        "USB_A2":"PC23",
                        "USB_A3":"PC24",
                        "USB_A4":"PC25",
                        "USB_A5":"PC26",
                        "USB_A6":"PC27",
                        "USB_A7":"PC28",
                        "USB_A8":"PC29",
                        "USB_A9":"PC30",
                        "USB_A10":"PD0",
                        "USB_A11":"PD1",
                        "USB_A12":"PD2",
                        "USB_A13":"PD3",
                        "USB_A14":"PD4",
                        "USB_A15":"PD5",
                        "USB_A16":"PD6",
                        "USB_A17":"PD7",
                        "USB_A18":"PD8",
                        "USB_A19":"PD9",
                        "USB_D0":"PC2",
                        "USB_D1":"PC3",
                        "USB_D2":"PC4",
                        "USB_D3":"PC5",
                        "USB_D4":"PC6",
                        "USB_D5":"PC7",
                        "USB_D6":"PC8",
                        "USB_D7":"PC9",
                        "SWSTATE":"PB26",
                        "PWRON":"PB27",
                        "LEDSURGE":"PB14",
                        "SAM_FPGA_CFG_CS":"PB16",
                        "CFG_INITB":"PB18",
                        "CFG_DONE":"PB17",
                        "CFB_PROGRAMB":"PB19",
                        "SAM_FPGA_COPI":"PB20",
                        "SAM_FPGA_CIPO":"PB21",
                        "SAM_FPGA_CCLK":"PB24",
                        "USB_CLK1":"PA24",
                        "USB_SPI_CIPO":"PA25",
                        "USB_SPI_COPI":"PA26",
                        "USB_SPI_SCK":"PA27",
                        "USB_SPI_CS":"PA28"}

    def __init__(self, usb, timeout=200):
        self.sendCtrl = usb.sendCtrl
        self.readCtrl = usb.readCtrl
        self._usb = usb
        self._timeout = timeout

    def pin_name_to_number(self, pinname):
        """Convert from a user-friendly pin name to the number.

        This function can be passed a name from the schematic (such as "USB_A20"),
        a name from the SAM3U peripherals (such as "PB22"), or a FPGA pin location
        (such as "M2"). It will attempt to auto-detect which one you passed.

        Args:
            pinname (str): Name such as "PB22", "USB_A20", or "M2".
        """      
        if isinstance(pinname, int):
            return datain

        pinname = pinname.upper()

        if pinname in self.SCHEMATIC_PIN_NAMES:
            return self.SAM3X_PIN_NAMES[self.SCHEMATIC_PIN_NAMES[pinname]]

        if pinname in self.FPGA_PIN_NAMES:
            return self.SAM3X_PIN_NAMES[self.SCHEMATIC_PIN_NAMES[self.FPGA_PIN_NAMES[pinname]]]

        if pinname in self.SAM3X_PIN_NAMES:
            return self.SAM3X_PIN_NAMES[pinname]
        
        raise ValueError("I don't know what pin this is (sorry): %s"%(pinname))
        
    def pin_set_output(self, pinname):
        """Set a given pin as an output.
        
        Args:
            pinname (str): Name such as "PB22", "USB_A20", or "M2".   
        """
        pinnum = self.pin_name_to_number(pinname)
        self.sendCtrl(self.REQ_FPGAIO_UTIL, self.REQ_IO_CONFIG, [pinnum, self.CONFIG_PIN_OUTPUT])

    def pin_set_state(self, pinname, state):
        """Set the state of a pin (must have been set as output previously).
        
        Args:
            pinname (str): Name such as "PB22", "USB_A20", or "M2".   
            state (bool): Set pin high (True) or low (False)
        """
        pinnum = self.pin_name_to_number(pinname)
        if state:
            state = 1
        else:
            state = 0
        self.sendCtrl(self.REQ_FPGAIO_UTIL, self.REQ_IO_OUTPUT, [pinnum, state])

    def spi1_setpins(self, sdo, sdi, sck, cs):
        """Set the pins to be used for the SPI1 interface.

         Args:
            sdo (str): Serial Data Out (output from SAM3X) pin name such as "PB22", "USB_A20", or "M2".       
            sdi (str): Serial Data In (input to SAM3X) pin name such as "PB22", "USB_A20", or "M2".  
            sck (str): Serial Clock (output from SAM3X) pin name such as "PB22", "USB_A20", or "M2".  
            cs (str): Chip Select (output from SAM3X) name such as "PB22", "USB_A20", or "M2".  
        """

        sdo = self.pin_name_to_number(sdo)
        sdi = self.pin_name_to_number(sdi)
        sck = self.pin_name_to_number(sck)
        cs = self.pin_name_to_number(cs)

        self.sendCtrl(self.REQ_FPGAIO_UTIL, self.REQ_IO_CONFIG, [sdo, self.CONFIG_PIN_SPI1_SDO])
        self.sendCtrl(self.REQ_FPGAIO_UTIL, self.REQ_IO_CONFIG, [sdi, self.CONFIG_PIN_SPI1_SDI])
        self.sendCtrl(self.REQ_FPGAIO_UTIL, self.REQ_IO_CONFIG, [sck, self.CONFIG_PIN_SPI1_SCK])
        self.sendCtrl(self.REQ_FPGAIO_UTIL, self.REQ_IO_CONFIG, [cs, self.CONFIG_PIN_SPI1_CS])

        
    def spi1_enable(self, enable, waitcycles=0):
        """Enable or disable the SPI interface.
        
        Args:
            enable (bool): Enable (True) or disable (False) SPI interface
        """
        if enable:
            self.sendCtrl(self.REQ_FPGASPI1_XFER, 0xA0)
        else:
            self.sendCtrl(self.REQ_FPGASPI1_XFER, 0xA1)
        
        if waitcycles > 0:
            raise NotImplementedError("💩 - Colin was lazy")
        
    def spi1_set_cs_pin(self, status):
        """Set the SPI pin high or low.
        
        Args:
            status (bool): Set CS pin high (True) or low (False)
        """
        if status:
            self.sendCtrl(self.REQ_FPGASPI1_XFER, 0xA3)
        else:
            self.sendCtrl(self.REQ_FPGASPI1_XFER, 0xA2)        

    def spi1_tx_rx(self, data):
        """Write up to 64 bytes of data to the SPI interface (no CS action).
        
        This is a low-level function that performs a single transfer. Does not
        affect the CS pin.

        Args:
            data (list): Write data over the SPI interface

        Raises:
            ValueError: len(data) > 64
        """
        if len(data) > 64:
            raise ValueError("Data is tooooooo long!")
        self.sendCtrl(self.REQ_FPGASPI1_XFER, 0xA4, data)
        readdata = self.readCtrl(self.REQ_FPGASPI1_XFER, dlen=len(data))
        return readdata
    
    def spi1_transfer(self, data):
        """Writes data, dropping CS before and raising after, returns read data.

        Args:
            data (list): Write data over the SPI interface
        """

        resp = []

        datalen = len(data)

        self.spi1_set_cs_pin(False)

        for i in range(0, datalen, 64):
            dataend = i+64
            
            if dataend > datalen:
                dataend = datalen
            
            tx = data[i:dataend]
            rx = self.spi1_tx_rx(tx)
            resp.extend(rx)

        self.spi1_set_cs_pin(True)

        return resp

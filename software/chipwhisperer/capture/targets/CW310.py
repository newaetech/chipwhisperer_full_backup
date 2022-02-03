# Copyright (c) 2020-2021, NewAE Technology Inc
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

from .CW305 import CW305
import time
import os.path
from ...hardware.naeusb.naeusb import NAEUSB
from ...hardware.naeusb.pll_cdce906 import PLLCDCE906
from ...hardware.naeusb.fpga import FPGA
from ...logging import *
from collections import OrderedDict
from ...common.utils import util

class CW310(CW305):
    """CW310 Bergen Board target object.

    This class contains the public API for the CW310 hardware.
    To connect to the CW310, the easiest method is::

        import chipwhisperer as cw
        scope = cw.scope()

        # scope can also be None here, unlike with the default SimpleSerial
        target = cw.target(scope,
                targets.CW310, bsfile=<valid FPGA bitstream file>)


    Inherits the CW305 object, so you can use the same methods as the CW305, provided the register interface
    in your FPGA build is the same.

    You can also set the voltage and current settings for the USB-C Power port on the CW310

        # set USB PDO 3 to 20V 5A
        target.usb_set_voltage(3, 20)
        target.usb.set_current(3, 5)

        # renegotiate PDO (applies above settings)
        target.usb_negotiate_pdo()

    For more help about CW305 settings, try help() on this CW310 submodule:

       * target.pll
    """
    USB_I2C_SETUP = 0x43
    USB_I2C_DATA = 0x44
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
        self._io = FPGAIO(self._naeusb, 200)

        # TODO- temporary until these are added to the parsed defines file
        self.REG_XADC_DRP_ADDR = 0x17
        self.REG_XADC_DRP_DATA = 0x18
        self.REG_XADC_STAT     = 0x19

    def _dict_repr(self):
        rtn = OrderedDict()
        rtn['fpga_buildtime'] = self.get_fpga_buildtime()
        rtn['xadc_status'] = self.xadc_status
        rtn['max temp'] = self.get_xadc_temp(maximum=True)
        rtn['temp'] = self.get_xadc_temp()
        rtn['max vccint'] = self.get_xadc_vcc('vccint', maximum=True)
        rtn['max vccbram'] = self.get_xadc_vcc('vccbram', maximum=True)
        rtn['max vccaux'] = self.get_xadc_vcc('vccaux', maximum=True)
        rtn['current vccint'] = self.get_xadc_vcc('vccint')
        rtn['current vccbram'] = self.get_xadc_vcc('vccbram')
        rtn['current vccaux'] = self.get_xadc_vcc('vccaux')
        rtn['vaux0'] = self.get_xadc_vaux(0)
        rtn['vaux1'] = self.get_xadc_vaux(1)
        rtn['vaux8'] = self.get_xadc_vaux(8)
        return rtn

    def __repr__(self):
        return util.dict_to_str(self._dict_repr())

    def __str__(self):
        return self.__repr__()

    def _getFWPy(self):
        from ...hardware.firmware.cwbergen import fwver
        return fwver
        

    def _con(self, scope=None, bsfile=None, force=False, fpga_id=None, defines_files=None, slurp=True, prog_speed=10E6):
        # add more stuff later
        self._naeusb.con(idProduct=[0xC310])
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
            status = self.fpga.FPGAProgram(open(bsfile, "rb"), prog_speed=prog_speed)


    def _xadc_drp_write(self, addr, data):
        """Write XADC DRP register. UG480 for register definitions.
        Args:
            addr (int): 6-bit address
            data (int): 16-bit write data
        """
        self.fpga_write(self.REG_XADC_DRP_DATA, [data  & 0xff, data >> 8])
        self.fpga_write(self.REG_XADC_DRP_ADDR, [addr + 0x80])


    def _xadc_drp_read(self, addr):
        """Read XADC DRP register. UG480 for register definitions.
        Args:
            addr (int): 6-bit address
        Returns:
            A 16-bit integer.
        """
        self.fpga_write(self.REG_XADC_DRP_ADDR, [addr])
        raw = self.fpga_read(self.REG_XADC_DRP_DATA, 2)
        return raw[0] + (raw[1] << 8)

    @property
    def xadc_status(self):
        """Read XADC alarm status bits
        :Getter: Returns status string.

        :Setter: Clear the XADC status error bits (they are sticky).
        """
        raw = self.fpga_read(self.REG_XADC_STAT, 1)[0]
        stat = ''
        if raw & 1:  stat += 'Over temperature alarm, '
        if raw & 2:  stat += 'User temperature alarm, '
        if raw & 4:  stat += 'VCCint alarm, '
        if raw & 8:  stat += 'VCCaux alarm, '
        if raw & 16: stat += 'VCCbram alarm, '
        if stat == '':
            stat = 'good'
        return stat

    @xadc_status.setter
    def xadc_status(self, clear):
        self.fpga_write(self.REG_XADC_STAT, [0x0])


    def get_xadc_temp(self, maximum=False):
        """Read XADC temperature.
        Args: 
            maximum (bool): if True, return the maximum observed temperature (since last reset);
                otherwise, return current measured temperature
        Returns:
            Temperature in celcius (float).
        """
        if maximum:
            addr = 0x20
        else:
            addr = 0x0
        raw = self._xadc_drp_read(addr)
        return (raw>>4) * 503.975/4096 - 273.15 # ref: UG480


    def get_xadc_vcc(self, rail='vccint', maximum=False):
        """Read XADC vcc.
        Args:
            rail (string): 'vccint', 'vccaux', or 'vccbram'
            maximum (bool): if True, return the maximum observed voltage (since last reset);
                otherwise, return current measured voltage
        Returns:
            voltage (float).
        """
        if rail not in ('vccint', 'vccaux', 'vccbram'):
            raise ValueError("Invalid rail")

        if not maximum:
            if rail == 'vccint':
                addr = 0x01
            elif rail == 'vccaux':
                addr = 0x02
            elif rail == 'vccbram':
                addr = 0x06
        else:
            if rail == 'vccint':
                addr = 0x21
            elif rail == 'vccaux':
                addr = 0x22
            elif rail == 'vccbram':
                addr = 0x23

        raw = self._xadc_drp_read(addr)
        return (raw>>4)/4096 * 3 # ref: UG480


    def get_xadc_vaux(self, n=0):
        """Read XADC vaux.
        Args:
            n (int): 0, 1 or 8, for vauxp/n[0|1|8]
        Returns:
            voltage (float).
        """
        assert n in [0, 1, 8]
        addr = n + 0x10
        raw = self._xadc_drp_read(addr)
        return (raw>>4)/4096 # ref: UG480

    def _i2c_write(self, data):
        self._naeusb.sendCtrl(self.USB_I2C_DATA, 0, data)

    def _i2c_write_settings(self, chip, addr):
        self._naeusb.sendCtrl(self.USB_I2C_SETUP, 0, [chip, addr])
        pass

    def _i2c_read(self, dlen):
        msg = self._naeusb.readCtrl(self.USB_I2C_DATA, 0, dlen+1)
        return msg[0], msg[1:]

    def _i2c_read_settings(self, addr_len=1):
        msg = self._naeusb.readCtrl(self.USB_I2C_SETUP, 0, 2+addr_len)
        return msg[0], msg[1], msg[2:]

    def usb_i2c_read(self, addr, dlen, retry=True):
        self._i2c_write_settings(chip=0x28, addr=addr)
        status, data = self._i2c_read(dlen+1)
        if status != 0:
            status, data = self._i2c_read(dlen+1)
        return status, data

    def usb_i2c_write(self, addr, data, retry=True):
        self._i2c_write_settings(chip=0x28, addr=addr)
        self._i2c_write(data)
        msg = self._i2c_read_settings()
        if msg[0] != 0:
            self._i2c_write(data)
            msg = self._i2c_read_settings()
        return msg[0]


    def usb_set_voltage(self, pdo_num, voltage):
        """Set the voltage for one of the USBC PDOs.

        PDO1 is always 5V 1A and cannot be changed.

        Args:
            pdo_num (int): The PDO to set the voltage for. Can be 2 or 3
            voltage (float): The voltage to set. Must be between 5 and 20 inclusive. Has
                            a resolution of 0.05V
        """
        if pdo_num not in [2, 3]:
            raise ValueError("pdo_num must be 2 or 3, {}".format(pdo_num))
        if (voltage > 20 or voltage < 5):
            raise ValueError("Voltage must be between 5 and 20, {}".format(voltage))
        
        # self._naeusb.sendCtrl(0x43, 0, [0x28, 0x89 + (pdo_num - 2) * (0x04)])
        # snk_pdo = self._naeusb.readCtrl(0x44, 0, 4)
        pdo_reg = 0x89 + (pdo_num - 2) * 0x04
        status, snk_pdo = self.usb_i2c_read(pdo_reg, 4)

        voltage *= 20
        voltage = int(voltage)

        snk_pdo[1] &= ~0xFC
        snk_pdo[2] &= ~0x0F
        target_logger.info(snk_pdo)
        snk_pdo[1] |= ((voltage << 2) & 0xFC)
        snk_pdo[2] |= ((voltage >> 6) & 0x0F)
        target_logger.info(voltage)
        target_logger.info(snk_pdo)
        # self._naeusb.sendCtrl(0x44, 0, snk_pdo)
        status = self.usb_i2c_write(pdo_reg, snk_pdo)

    def usb_set_current(self, pdo_num, current):
        """Set the current for one of the USBC PDOs.

        PDO1 is always 5V 1A and cannot be changed.

        Args:
            pdo_num (int): The PDO to set the current for. Can be 2 or 3
            voltage (float): The current to set. Must be between 0.5 and 5 inclusive. Has
                            a resolution of 0.01A
        """
        if pdo_num not in [2, 3]:
            raise ValueError("pdo_num must be 2 or 3, {}".format(pdo_num))
        if (current > 5 or current < 0.5):
            raise ValueError("Current must be between 0.5 and 5, {}".format(current))

        pdo_reg = 0x88 + (pdo_num-2) * 0x04
        status, snk_pdo = self.usb_i2c_read(pdo_reg, 4)
        current *= 100
        current = int(current)

        snk_pdo[0] &= ~0xFF
        snk_pdo[1] &= ~0x03
        target_logger.info(snk_pdo)
        snk_pdo[0] |= (current & 0xFF)
        snk_pdo[1] |= ((current >> 8) & 0x03)
        
        target_logger.info(current)
        target_logger.info(snk_pdo)
        status = self.usb_i2c_write(pdo_reg, snk_pdo)

    def usb_negotiate_pdo(self):
        """Renegotate the USBC PDOs. Must be done for new PDOs settings to take effect
        """
        #soft reset
        # self._naeusb.sendCtrl(0x43, 0, [0x28, 0x51])
        # self._naeusb.sendCtrl(0x44, 0, [0x0D])
        status = self.usb_i2c_write(0x51, [0x0D])

        #send reset on pdo bus
        # self._naeusb.sendCtrl(0x43, 0, [0x28, 0x1A])
        # self._naeusb.sendCtrl(0x44, 0, [0x26])
        self.usb_i2c_write(0x1A, 0x26)

    def _getCWType(self):
        return 'cwbergen'

    def dis(self):
        if self._naeusb:
            self._naeusb.close()
            self._naeusb = None

    def go_reg(self):
        """Disable USB clock (if requested), perform encryption, re-enable clock"""
        self.fpga_write(self.REG_USER_LED, [0x01])

        self.fpga_write(self.REG_CRYPT_GO, [1])
        time.sleep(0.01)
        self.fpga_write(self.REG_CRYPT_GO, [0])

    def temp_sensor_send(self, addr, data_byte):
        self._naeusb.sendCtrl(0x42, addr & 0xFF, [data_byte & 0xFF])

    def temp_sensor_read(self, addr):
        return self._naeusb.readCtrl(0x42, addr & 0xFF, 2)[1]

    @property
    def fpga_temp(self):
        return self.temp_sensor_read(0x01)

    def reset_fpga_power(self):
        self._naeusb.sendCtrl(0x22, 0x07) #kill power

        time.sleep(0.5)
        self._naeusb.sendCtrl(0x22, 0x08) #enable power

    def gpio_mode(self, timeout=200):
        """Allow arbitrary GPIO access on SAM3U
        
        Allows low-level IO access to SAM3U GPIO, and also SPI transfers.
        (see documentation on the returned object for more info)

        Args:
            timeout (int): USB timeout in ms. Defaults to 200.

        Returns:
            A FPGAIO object which can be used to access IO on the CW305.
        """
        self._io._timeout = timeout
        return self._io

    @property
    def cdc_settings(self):
        """Check or set whether USART settings can be changed via the USB CDC connection

        i.e. whether you can change USART settings (baud rate, 8n1) via a serial client like PuTTY

        :getter: An array of length two for the two CDC ports

        :setter: Can set either via an integer (which sets both ports) or an array of length 2 (which sets each port)

        Returns None if using firmware before the CDC port was added
        """
        rawver = self._naeusb.readFwVersion()
        ver = '{}.{}'.format(rawver[0], rawver[1])
        if ver < '0.30':
            return None
        self._naeusb.CMD_CDC_SETTINGS_EN = 0x41
        return self._naeusb.get_cdc_settings()

    @cdc_settings.setter
    def cdc_settings(self, port):
        rawver = self._naeusb.readFwVersion()
        ver = '{}.{}'.format(rawver[0], rawver[1])
        if ver < '0.30':
            return None

        self._naeusb.CMD_CDC_SETTINGS_EN = 0x41
        return self._naeusb.set_cdc_settings(port)

    def _test(self):
        print("Testing PGOOD/FPGA Power")
        print("Turning off FPGA power...")
        self._io.pin_set_state("PB27", 0)
        time.sleep(0.5)
        pgood_states = [self._io.pin_get_state("PC16") , self._io.pin_get_state("PC19") , 
                        self._io.pin_get_state("PC20") , self._io.pin_get_state("PB11")]
        if 1 in pgood_states:
            print("ERROR: PGOOD high when power set to low: {}".format(pgood_states))
        else:
            print("OK: PGOOD low when power off")
            
        resp = input("Power LEDs off? [y/n]")
        if resp == 'y' or resp == 'Y':
            print("PGOOD LEDs ok")
        
        print("Turning on FPGA power...")
        self._io.pin_set_state("PB27", 1)
        time.sleep(0.5)
        pgood_states = [self._io.pin_get_state("PC16") , self._io.pin_get_state("PC19") , 
                        self._io.pin_get_state("PC20") , self._io.pin_get_state("PB11")]
        
        if 0 in pgood_states:
            print("ERROR: PGOOD low when power set to high: {}".format(pgood_states))
        else:
            print("OK: PGOOD high when power high") 
            
        resp = input("Power LEDs on? [y/n]")
        if resp == 'y' or resp == 'Y':
            print("PGOOD LEDs ok")
            
        print("Testing reset power button - please press it")
        
        button_ok = False
        for i in range(500):
            if self._io.pin_get_state("PB23") == 0:
                button_ok = True
                break
            time.sleep(0.01)
        if not button_ok:
            print("ERROR: Couldn't detect button press")
        else:
            print("OK: Button press detected")
            
        print("Setting temp LEDs high")
        input("Press ENTER when ready to look")
        self._io.pin_set_state("PA0", 1)
        self._io.pin_set_state("PA1", 1)
        
        resp = input("Did both go or stay on? [y/n]")
        if resp == "y" or resp == "Y":
            print("Temp LEDs ok")
            
        print("Setting temp LEDs low")
        input("Press ENTER when ready to look")
        self._io.pin_set_state("PA0", 0)
        self._io.pin_set_state("PA1", 0)
        
        resp = input("Did both go or stay off? [y/n]")
        if resp == "y" or resp == "Y":
            print("Temp LEDs ok")


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
            return pinname

        pinname = pinname.upper()

        if pinname in self.SCHEMATIC_PIN_NAMES:
            return self.SAM3X_PIN_NAMES[self.SCHEMATIC_PIN_NAMES[pinname]]

        if pinname in self.FPGA_PIN_NAMES:
            return self.SAM3X_PIN_NAMES[self.SCHEMATIC_PIN_NAMES[self.FPGA_PIN_NAMES[pinname]]]

        if pinname in self.SAM3X_PIN_NAMES:
            return self.SAM3X_PIN_NAMES[pinname]
        
        raise ValueError("I don't know what pin this is (sorry): %s"%(pinname))

    def pin_get_state(self, pinname):
        """Get the state of a pin

        Args:
            pinname (str): Name such as "PB22", "USB_A20", or "M2".   
        """
        pinnum = self.pin_name_to_number(pinname)
        return self.readCtrl(self.REQ_FPGAIO_UTIL, pinnum, 1)[0]
        
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


def test_cw310(fpgaio):
    print("Testing")

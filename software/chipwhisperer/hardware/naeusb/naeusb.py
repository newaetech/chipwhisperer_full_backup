#
# Copyright (c) 2014-2021, NewAE Technology Inc
# All rights reserved.
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
# ==========================================================================
import time
import warnings
import math
from threading import Thread
import usb1  # type: ignore
import os
import array
from typing import Optional, Union, List, Tuple, Dict, cast

from ..firmware import cwlite as fw_cwlite
from ..firmware import cw1200 as fw_cw1200
from ..firmware import cw305  as fw_cw305
from ..firmware import cwnano  as fw_nano
from ..firmware import cwhusky as fw_cwhusky

from ...logging import *

SAM_FW_FEATURES = [
    "WCID", #0
    "CDC", #1
    "VARIABLE_FPGA_PROG_SPEED", #2
    "TX_IN_WAITING", #3
    "RESET", #4
    "FPGA_LOCK_RESET", #5
    "SAM_BUILD_DATE", #6
    "USART_SELECTION", #7
    "SERIAL_200_BUFFER", #8
    "EXT_FPGA_PROG", #9
    "FPGA_SPI_PASSTHROUGH", #10
    "SAM3U_GPIO_MODE", #11
    "FPGA_TARGET_BULK_WRITE", #12
    "MPSSE", #13
    "TARGET_SPI", #14
]

class CWFirmwareError(Exception):
    pass

SAM_FW_FEATURE_BY_DEVICE = {
    0xACE0: {
        SAM_FW_FEATURES[0]: '0.23.0',
        SAM_FW_FEATURES[1]: '0.30.0',
        SAM_FW_FEATURES[3]: '0.20.0',
        SAM_FW_FEATURES[4]: '0.30.0',
        SAM_FW_FEATURES[6]: '0.50.0',
        SAM_FW_FEATURES[7]: '0.50.0',
        SAM_FW_FEATURES[8]: '0.30.0',
        SAM_FW_FEATURES[13]: '0.60.0'
    },

    0xACE2: {
        SAM_FW_FEATURES[0]: '0.23.0',
        SAM_FW_FEATURES[1]: '0.30.0',
        SAM_FW_FEATURES[2]: '0.52.0',
        SAM_FW_FEATURES[3]: '0.20.0',
        SAM_FW_FEATURES[4]: '0.30.0',
        SAM_FW_FEATURES[5]: '0.30.0',
        SAM_FW_FEATURES[6]: '0.50.0',
        SAM_FW_FEATURES[7]: '0.50.0',
        SAM_FW_FEATURES[8]: '0.30.0',
        SAM_FW_FEATURES[9]: '0.52.0',
        SAM_FW_FEATURES[13]: '0.60.0',
        SAM_FW_FEATURES[14]: '0.60.0'
    },

    0xACE3: {
        SAM_FW_FEATURES[0]: '1.23.0',
        SAM_FW_FEATURES[1]: '1.30.0',
        SAM_FW_FEATURES[2]: '1.52.0',
        SAM_FW_FEATURES[3]: '1.20.0',
        SAM_FW_FEATURES[4]: '1.30.0',
        SAM_FW_FEATURES[5]: '1.30.0',
        SAM_FW_FEATURES[6]: '1.50.0',
        SAM_FW_FEATURES[7]: '1.50.0',
        SAM_FW_FEATURES[8]: '1.30.0',
        SAM_FW_FEATURES[9]: '1.52.0',
        SAM_FW_FEATURES[13]: '1.60.0',
        SAM_FW_FEATURES[14]: '1.60.0'
    },

    0xACE5: {
        SAM_FW_FEATURES[0]: '1.0.0',
        SAM_FW_FEATURES[1]: '1.0.0',
        SAM_FW_FEATURES[2]: '1.0.0',
        SAM_FW_FEATURES[3]: '1.0.0',
        SAM_FW_FEATURES[4]: '1.0.0',
        SAM_FW_FEATURES[5]: '1.0.0',
        SAM_FW_FEATURES[6]: '1.0.0',
        SAM_FW_FEATURES[7]: '1.0.0',
        SAM_FW_FEATURES[8]: '1.0.0',
        SAM_FW_FEATURES[9]: '1.0.0',
        SAM_FW_FEATURES[13]: '1.1.0',
        SAM_FW_FEATURES[14]: '1.1.0'
    },

    0xC305: {
        SAM_FW_FEATURES[0]: '0.32.0',
        SAM_FW_FEATURES[2]: '0.52.0',
        SAM_FW_FEATURES[4]: '0.32.0',
        SAM_FW_FEATURES[5]: '0.32.0',
        SAM_FW_FEATURES[6]: '0.50.0',
        SAM_FW_FEATURES[10]: '0.30.0',
        SAM_FW_FEATURES[11]: '0.40.0',
        SAM_FW_FEATURES[12]: '0.53.0',
    },

    0xC310: {
        SAM_FW_FEATURES[0]: '1.0.0',
        SAM_FW_FEATURES[1]: '1.0.0',
        SAM_FW_FEATURES[2]: '1.0.0',
        SAM_FW_FEATURES[3]: '1.0.0',
        SAM_FW_FEATURES[4]: '1.0.0',
        SAM_FW_FEATURES[5]: '1.0.0',
        SAM_FW_FEATURES[6]: '1.0.0',
        SAM_FW_FEATURES[7]: '1.0.0',
        SAM_FW_FEATURES[8]: '1.0.0',
        SAM_FW_FEATURES[10]: '1.0.0',
        SAM_FW_FEATURES[11]: '1.0.0',
        SAM_FW_FEATURES[12]: '1.1.0',
        SAM_FW_FEATURES[13]: '1.2.0'
    }
}

def _check_sam_feature(feature, fw_version, prod_id):
    if feature not in SAM_FW_FEATURES:
        raise ValueError("Unknown feature {}".format(feature))
    feature_set = SAM_FW_FEATURE_BY_DEVICE[prod_id]

    if feature not in feature_set:
        return False

    if feature_set[feature] > fw_version:
        return False

    return True

def _WINDOWS_USB_CHECK_DRIVER(device) -> Optional[str]:
    """Checks which driver device is using

    Checks whether the device is connected to the PC (harder than you'd think)

    Does not check the actual driver in use for custom interfaces in composite devices.
    Instead, it just resolves to usbcggp, which is the composite device driver for Windows.
    """
    try:
        import winreg
        keyhandle = winreg.OpenKey(winreg.HKEY_LOCAL_MACHINE, "SYSTEM")
        subkey = r"ControlSet001\Enum\USB"
        subkey += "\\VID_{:04X}&PID_{:04X}".format(device.getVendorID(), device.getProductID())

        def get_enum_by_name(handle, name):
            try:
                cnt = 0
                enum_name = ""
                myenum = None
                naeusb_logger.debug('Looking for {}...'.format(name))
                while enum_name != name:
                    myenum = winreg.EnumValue(handle, cnt)
                    enum_name = myenum[0]
                    cnt += 1
                    naeusb_logger.debug('Found {}'.format(enum_name))
                return myenum[1]
            except OSError as e:
                return None

        # get devices with same PID/VID
        try:
            keyhandle_device = winreg.OpenKey(keyhandle, subkey)
        except Exception as e:
            naeusb_logger.info("Could not get keyhandle device " + str(e))
            return None
        i = 0
        address = None
        sn : str = ""
        attached = False

        # get devices that are connected and have the same port number
        while (address != device.getPortNumber()) or (attached is False):
            try:
                sn = winreg.EnumKey(keyhandle_device, i)
            except Exception as e:
                naeusb_logger.info("Could not get sn " + str(e)) 
                return None
            # print("sn: " + sn)
            keyhandle_sn = winreg.OpenKey(keyhandle_device, sn)
            with keyhandle_sn as h:
                address = get_enum_by_name(h, "Address")
                if address is None:
                    naeusb_logger.info("Could not find Address in device {}".format(sn))
                    return None
                service = get_enum_by_name(h, "Service")

                # now we need to figure out if this device is attached
                # Windows really doesn't make this easy...
                try:
                    keyhandle_driver = winreg.OpenKey(winreg.HKEY_LOCAL_MACHINE, "SYSTEM\\CurrentControlSet\\Services\\{}\\Enum".format(service))
                except Exception as e:
                    naeusb_logger.info("Could not get keyhandle driver " + str(e))
                    return None
                num_enums = get_enum_by_name(keyhandle_driver, "Count")
                if num_enums:
                    attached = False
                    for j in range(num_enums + 1):
                        device_id = get_enum_by_name(keyhandle_driver, str(j))
                        if device_id == "USB\\VID_{:04X}&PID_{:04X}\\{}".format(device.getVendorID(), device.getProductID(), sn):
                            attached = True
                            break
                    
                keyhandle_driver.Close()
                i += 1
        try:
            keyhandle_sn = winreg.OpenKey(keyhandle_device, sn)
        except Exception as e:
            naeusb_logger.debug("Could not get keyhandle sn " + str(e))
            return None

        service = get_enum_by_name(keyhandle_sn, "Service")
        if service is None:
            naeusb_logger.debug("Could not find service name in device {}".format(sn))
            return None

        keyhandle_sn.Close()
        keyhandle_device.Close()
        keyhandle.Close()
        return service
        
            
    except Exception as e:
        naeusb_logger.warning("Could not check driver ({}), assuming WINUSB is used".format(str(e)))
        return None


def packuint32(data):
    """Converts a 32-bit integer into format expected by USB firmware"""

    data = int(data)
    return [data & 0xff, (data >> 8) & 0xff, (data >> 16) & 0xff, (data >> 24) & 0xff]

def unpackuint32(buf):
    """"Converts an array into a 32-bit integer"""

    pint = buf[0]
    pint |= buf[1] << 8
    pint |= buf[2] << 16
    pint |= buf[3] << 24
    return pint

def packuint16(data):
    """Converts a 16-bit integer into format expected by USB firmware"""

    data = int(data)

    return [data & 0xff, (data >> 8) & 0xff, (data >> 16) & 0xff, (data >> 24) & 0xff]


#List of all NewAE PID's
NEWAE_VID = 0x2B3E
NEWAE_PIDS : Dict[int, Dict[str, Union[str, List[int]]]]= {
    0xACE2: {'name': "ChipWhisperer-Lite",     'fwver': fw_cwlite.fwver},
    0xACE3: {'name': "ChipWhisperer-CW1200",   'fwver': fw_cw1200.fwver},
    0xC305: {'name': "CW305 Artix FPGA Board", 'fwver': fw_cw305.fwver},
    0xACE0: {'name': "ChipWhisperer-Nano", 'fwver': fw_nano.fwver},
    0xACE5: {'name': "ChipWhisperer-Husky",   'fwver': fw_cwhusky.fwver},
}

class NAEUSB_Backend:
    """
    Backend to talk to the USB device.
    """

    CMD_READMEM_BULK = 0x10
    CMD_WRITEMEM_BULK = 0x11
    CMD_READMEM_CTRL = 0x12
    CMD_WRITEMEM_CTRL = 0x13
    CMD_MEMSTREAM = 0x14

    def __init__(self):
        self._usbdev = None
        self._timeout = 500
        self.device = None
        self.handle = None

        try:
            self.usb_ctx = usb1.USBContext()
            self.usb_ctx.open()
        except OSError as e:
            # naeusb_logger.error("Could not import libusb dll. Try pip uninstall libusb1, then pip install libusb1")
            self.usb_ctx = None
            raise OSError("Could not import libusb dll. Try \npip uninstall libusb1\npip install libusb1") from e
        self.handle = None
        self.device = None

    def usbdev(self) -> usb1.USBDeviceHandle:
        """Safely get USB device, throwing error if not connected"""

        if not self._usbdev: raise OSError("USB Device not found. Did you connect it first?")
        return self._usbdev

    def is_accessable(self, dev : usb1.USBDevice) -> bool:
        try:
            dev.getSerialNumber()
            return True
        except:
            return False

    def find(self, serial_number : Optional[str]=None, idProduct : Optional[List[int]]=None, 
        hw_location : Optional[Tuple[int, int]]=None) -> usb1.USBDevice:
        # check if we got anything
        dev_list = self.get_possible_devices(idProduct, attempt_access=(not hw_location))
        if len(dev_list) == 0:
            raise OSError("Could not find ChipWhisperer. Is it connected?")

        # if more than one CW, we require a serial number
        if hw_location:
            naeusb_logger.info("Attempting hw_location access: {}".format(hw_location))
            dev_list = [dev for dev in dev_list if (dev.getBusNumber(), dev.getDeviceAddress()) == hw_location]
            if len(dev_list) != 1:
                raise OSError("Unable to find ChipWhisperer with hw_location {}, got {}".format(hw_location, dev_list))
            return dev_list[0]
        sns = ["{}:{}".format(dev.getProduct(), dev.getSerialNumber()) for dev in dev_list]
        if (len(dev_list) > 1) and (serial_number is None):
            if len(dev_list) > 1:
                raise Warning("Multiple ChipWhisperers connected, please specify serial number." \
                            "\nDevices:\n \
                            {}".format(sns))

        # get all devices that match serial number
        if serial_number:
            dev_list = [dev for dev in dev_list if dev.getSerialNumber() == serial_number]
        if len(dev_list) == 0:
            raise Warning("Unable to find ChipWhisperer with serial number {}. \nDevices: {}"\
                .format(serial_number, sns))

        # finally, we know we have the right device and can return
        return dev_list[0]


    def open(self, serial_number : Optional[str]=None, idProduct : Optional[List[int]]=None, 
        connect_to_first : bool =False, hw_location : Optional[Tuple[int, int]]=None) -> Optional[usb1.USBDeviceHandle]:
        """
        Connect to device using default VID/PID
        """

        self.device = self.find(serial_number, idProduct, hw_location=hw_location)
        if connect_to_first == False:
            return None
        try:
            self.handle = self.device.open()
        except usb1.USBError as e:
            naeusb_logger.error("Could not open USB device.")
            if e.value == -3:
                naeusb_logger.error("Check that the ChipWhisperer is not already connected")
                naeusb_logger.error("Or that you have the proper permissions to access it")
            raise
        self._usbdev = self.handle

        # claim bulk interface, may not be necessary?
        self.handle.claimInterface(0)

        self.sn = self.handle.getSerialNumber()
        self.pid = self.device.getProductID()
        naeusb_logger.debug('Found %s, Serial Number = %s' % (self.handle.getProduct(), self.sn))

        # Husky has different endpoints for faster transfer
        if self.device.getProductID() == 0xace5:
            naeusb_logger.debug("Husky found, using new endpoints")
            self.rep = 0x85
            self.wep = 0x06
        else:
            self.rep = 0x81
            self.wep = 0x02
        self._timeout = 20000

        return self.handle

    def __del__(self):
        # need to make sure device and handle are closed before everything is deleted
        self.close()

    def close(self):
        # """Close the USB connection"""
        if self.device:
            del self.device
            self.device = None
        if self.handle:
            # self._usbdev.close()
            self._usbdev = None
            del self.handle
            self.handle = None

    def get_possible_devices(self, idProduct : Optional[List[int]]=None, dictonly : bool=True, 
        attempt_access : bool=False) -> List[usb1.USBDevice]:
        """Get list of USB devices that match NewAE vendor ID (0x2b3e) and
        optionally a product ID

        Checks VendorID, then makes sure the devices are accessable
        Args:
            idProduct (list of int, optional): If not None, the product ID to match
            sn (string, optional): If not None,
        Returns:
            List of USBDevice that match Vendor/Product IDs
            """
        
        dev_list = [dev for dev in self.usb_ctx.getDeviceIterator() if dev.getVendorID() == 0x2b3e]
        naeusb_logger.info("Found NAEUSB devices {}".format(dev_list))
        
        if os.name == "nt":
            for dev in dev_list:
                win_driver = _WINDOWS_USB_CHECK_DRIVER(dev)
                if win_driver is None:
                    continue
                if (win_driver.lower() != "usbccgp") and (win_driver.upper() != "WINUSB"):
                    naeusb_logger.warning("Invalid driver {} detected. If you have connection problems, try upgrading your driver".format(win_driver))
                    naeusb_logger.warning("See https://chipwhisperer.readthedocs.io/en/latest/drivers.html for more information")
        if not (idProduct is None):
            dev_list = [dev for dev in dev_list if dev.getProductID() in idProduct]

        naeusb_logger.info("Found NAEUSB devices {}".format(dev_list))
        
        if len(dev_list) == 0:
            return []

        for dev in dev_list:
            try:
                a = dev.getSerialNumber()
                naeusb_logger.info("Found ChipWhisperer with serial number {}".format(a))
            except Exception as e:
                naeusb_logger.info("Attempt to access ChipWhisperer failed. Attempting interface claim")
                naeusb_logger.info("Access failed with error {}".format(str(e)))
                dev_list.remove(dev)

        if len(dev_list) == 0:
            raise OSError("Unable to communicate with found ChipWhisperer. Check that \
\nanother process isn't connected to it and that you have permission to communicate with it.")

        return dev_list

    def sendCtrl(self, cmd : int, value : int=0, data : bytearray=bytearray()):
        """
        Send data over control endpoint
        """
        # Vendor-specific, OUT, interface control transfer
        naeusb_logger.debug("WRITE_CTRL: bmRequestType: {:02X}, \
                    bRequest: {:02X}, wValue: {:04X}, wIndex: {:04X}, data: {}".format(0x41, cmd, \
                        value, 0, data))
        self.handle.controlWrite(0x41, cmd, value, 0, data, timeout=self._timeout)
        #return self.usbdev().ctrl_transfer(0x41, cmd, value, 0, data, timeout=self._timeout)

    def readCtrl(self, cmd : int, value : int=0, dlen : int=0) -> bytearray:
        """
        Read data from control endpoint
        """
        # Vendor-specific, IN, interface control transfer
        response = self.handle.controlRead(0xC1, cmd, value, 0, dlen, timeout=self._timeout)
        naeusb_logger.debug("READ_CTRL: bmRequestType: {:02X}, \
                    bRequest: {:02X}, wValue: {:04X}, wIndex: {:04X}, data_len: {:04X}, response: {}".format(0xC1, cmd, \
                        value, 0, dlen, response))
        return response


    def cmdReadMem(self, addr : int, dlen : int) -> bytearray:
        """
        Send command to read over external memory interface from FPGA. Automatically
        decides to use control-transfer or bulk-endpoint transfer based on data length.
        """

        dlen = int(dlen)

        if dlen < 48:
            cmd = self.CMD_READMEM_CTRL
        else:
            cmd = self.CMD_READMEM_BULK

        # ADDR/LEN written LSB first
        pload = packuint32(dlen)
        pload.extend(packuint32(addr))
        try:
            self.sendCtrl(cmd, data=pload)
        except usb1.USBErrorPipe:
            naeusb_logger.info("Attempting pipe error fix - typically safe to ignore")
            self.sendCtrl(0x22, 0x11)
            self.sendCtrl(cmd, data=pload)
        # Get data
        if cmd == self.CMD_READMEM_BULK:
            data = self.handle.bulkRead(self.rep, dlen, timeout=self._timeout)
        else:
            data = self.readCtrl(cmd, dlen=dlen)

        naeusb_logger.debug("FPGA_READ: bulk: {}, addr: {:08X}, dlen: {:08X}, response: {}"\
            .format("yes" if dlen >= 48 else "no", addr, dlen, data))
        return data

    def cmdWriteMem(self, addr : int, data : bytearray):
        """
        Send command to write memory over external memory interface to FPGA. Automatically
        decides to use control-transfer or bulk-endpoint transfer based on data length.
        """

        dlen = len(data)

        if dlen < 48:
            cmd = self.CMD_WRITEMEM_CTRL
        else:
            cmd = self.CMD_WRITEMEM_BULK

        # ADDR/LEN written LSB first
        pload = packuint32(dlen)
        pload.extend(packuint32(addr))

        if cmd == self.CMD_WRITEMEM_CTRL:
            pload.extend(data)

        self.sendCtrl(cmd, data=pload)


        # Get data
        if cmd == self.CMD_WRITEMEM_BULK:
            self.handle.bulkWrite(self.wep, data, timeout=self._timeout)
        else:
            #logging.warning("Write ignored")

            pass

        naeusb_logger.debug("FPGA_WRITE: bulk: {}, addr: {:08X}, dlen: {:08X}, response: {}"\
            .format("yes" if dlen >= 48 else "no", addr, dlen, data))

        return None

    def cmdWriteBulk(self, data : bytearray):
        """
        Write data directly to the bulk endpoint.
        :param data: Data to be written
        :return:
        """
        naeusb_logger.debug("BULK WRITE: data = {}".format(data))
        self.handle.bulkWrite(self.wep, data, timeout=self._timeout)

    writeBulk = cmdWriteBulk

    def flushInput(self):
        """Dump all the crap left over"""
        try:
            # TODO: This probably isn't needed, and causes slow-downs on Mac OS X.
            self.handle.bulkRead(self.rep, 1000, timeout=0.010)
        except:
            pass

    def read(self, dbuf : bytearray, timeout : int) -> bytearray:
        resp = self.handle.bulkRead(self.rep, dbuf, timeout)

        naeusb_logger.debug("BULK READ: data = {}".format(dbuf))
        return resp

class NAEUSB:
    """
    USB Interface for NewAE Products with Custom USB Firmware. This function allows use of a daemon backend, as it is
    not directly touching the USB device itself.
    """

    CMD_FW_VERSION = 0x17
    CMD_CDC_SETTINGS_EN = 0x31

    CMD_READMEM_BULK = 0x10
    CMD_WRITEMEM_BULK = 0x11
    CMD_READMEM_CTRL = 0x12
    CMD_WRITEMEM_CTRL = 0x13
    CMD_MEMSTREAM = 0x14
    CMD_WRITEMEM_CTRL_SAM3U = 0x15
    CMD_SMC_READ_SPEED = 0x27

    CMD_FW_BUILD_DATE = 0x40

    stream = False

    # TODO: make this better
    fwversion_latest = [0, 11]
    def __init__(self):
        self._usbdev = None
        self.handle=None
        self.usbtx = NAEUSB_Backend()
        self.usbserializer = self.usbtx
        self._fw_ver = None
        self.streamModeCaptureStream = None

    def get_possible_devices(self, idProduct : List[int]) -> usb1.USBDevice:
        return self.usbtx.get_possible_devices(idProduct)

    def get_cdc_settings(self) -> list:
        if self.check_feature("CDC"):
            return self.usbtx.readCtrl(self.CMD_CDC_SETTINGS_EN, dlen=4)
        else:
            return [0, 0, 0, 0]

    def enable_MPSSE(self):
        if self.check_feature("MPSSE", True):
            try:
                self.sendCtrl(0x22, 0x42)
            except usb1.USBError:
                pass
            self.close()

    def set_cdc_settings(self, port : Tuple=(1, 1, 0, 0)):
        if self.check_feature("CDC"):
            if isinstance(port, int):
                port = (port, port, 0, 0)
            self.usbtx.sendCtrl(self.CMD_CDC_SETTINGS_EN, (port[0]) | (port[1] << 1) | (port[2] << 2) | (port[3] << 3))

    def set_smc_speed(self, val : int):
        """
        val = 0: normal read timing
        val = 1: fast read timing, should only be used for reading ADC samples; FPGA must also be set in fast FIFO
                 read mode for this to work correctly.
        """
        self.usbtx.sendCtrl(self.CMD_SMC_READ_SPEED, data=[val])

    def get_fw_build_date(self) -> str:
        if self.check_feature("SAM_BUILD_DATE"):
            try:
                build_date = bytes(self.usbtx.readCtrl(0x40, dlen=100)).decode()
                return build_date
            except usb1.USBErrorPipe:
                naeusb_logger.info("Build date unavailable") 
                return "UNKNOWN"
        return "UNKNOWN"

    def get_serial_ports(self) -> Optional[List[Dict[str, int]]]:
        """May have multiple com ports associated with one device, so returns a list of port + interface
        """
        if self.check_feature("CDC", True):
            if not self.usbtx._usbdev:
                raise OSError("Connect to device before calling this")
            import serial.tools.list_ports # type: ignore
            if serial.__version__ < '3.5':
                raise OSError("Pyserial >= 3.5 (found {}) required for this method".format(serial.__version__))
            devices = []
            for port in serial.tools.list_ports.comports():
                if port.serial_number == self.usbtx._usbdev.getSerialNumber().upper():
                    devices.append({"port": port.device, "interface": int(port.location.split('.')[-1])})
            return devices
        return None

    def con(self, idProduct : Tuple[int]=(0xACE2,), connect_to_first : bool=False, 
        serial_number : Optional[str]=None, hw_location : Optional[Tuple[int, int]]=None, **kwargs) -> int:
        """
        Connect to device using default VID/PID
        """
        self.usbtx.open(idProduct=idProduct, serial_number=serial_number, connect_to_first=True, hw_location=hw_location)


        self.snum=self.usbtx.sn
        fwver = self.readFwVersion()
        naeusb_logger.info('SAM3U Firmware version = %d.%d b%d' % (fwver[0], fwver[1], fwver[2]))


        fw_latest : List[int] = [0, 0]

        if self.usbtx.pid in NEWAE_PIDS:
            name = NEWAE_PIDS[self.usbtx.pid]['name']
            fw_latest = cast(List[int], NEWAE_PIDS[self.usbtx.pid]['fwver'])
        else:
            name = "Unknown (PID = %04x)"%self.usbtx.pid

        latest = fwver[0] > fw_latest[0] or (fwver[0] == fw_latest[0] and fwver[1] >= fw_latest[1])
        if not latest:
            naeusb_logger.warning('Your firmware is outdated - latest is %d.%d' % (fw_latest[0], fw_latest[1]) +
                             '. Suggested to update firmware, as you may experience errors' +
                             '\nSee https://chipwhisperer.readthedocs.io/en/latest/api.html#firmware-update')

        return self.usbtx.pid

    def usbdev(self):
        raise AttributeError("Do Not Call Me")

    def close(self):
        """Close USB connection."""
        self.usbtx.close()
        self.snum = None

    def readFwVersion(self) -> bytearray:
        if self._fw_ver is None:
            try:
                data = self.readCtrl(self.CMD_FW_VERSION, dlen=3)
                self._fw_ver = data
                return data
            except:
                return bytearray([0, 0, 0])
        return self._fw_ver

    def sendCtrl(self, cmd : int, value : int=0, data : bytearray=bytearray()):
        """
        Send data over control endpoint
        """
        # Vendor-specific, OUT, interface control transfer
        self.usbserializer.sendCtrl(cmd, value, data)

    def readCtrl(self, cmd : int, value : int=0, dlen : int=0) -> bytearray:
        """
        Read data from control endpoint
        """
        # Vendor-specific, IN, interface control transfer
        return self.usbserializer.readCtrl(cmd, value, dlen)

    def cmdReadMem(self, addr : int, dlen : int) -> bytearray:
        """
        Send command to read over external memory interface from FPGA. Automatically
        decides to use control-transfer or bulk-endpoint transfer based on data length.
        """

        return self.usbserializer.cmdReadMem(addr, dlen)

    def cmdWriteMem(self, addr : int, data : bytearray):
        """
        Send command to write memory over external memory interface to FPGA. Automatically
        decides to use control-transfer or bulk-endpoint transfer based on data length.
        """

        return self.usbserializer.cmdWriteMem(addr, data)

    def writeBulkEP(self, data : bytearray):
        """
        Write directoly to the bulk endpoint.
        :param data: Data to be written.
        :return:
        """

        return self.usbserializer.writeBulk(data)

    def flushInput(self):
        """Dump all the crap left over"""
        self.usbserializer.flushInput()

    class StreamModeCaptureThreadHusky(Thread):
        def __init__(self, serial, dlen, segment_size, dbuf_temp, timeout_ms=2000, is_husky=False):
            """TODO UPDATE THIS DESC
            Reads from the FIFO in streaming mode. Requires the FPGA to be previously configured into
            streaming mode and then arm'd, otherwise this may return incorrect information.

            Args:
                dlen: Number of samples to request.
                dbuf_temp: Temporary data buffer, must be of size cmdReadStream_bufferSize(dlen) or bad things happen
                timeout_ms: Timeout in ms to wait for stream to start, otherwise returns a zero-length buffer
                is_husky: False for CW-Pro, True for CW-Husky
            Returns:
                Tuple of (samples_per_block, total_bytes_rx)
            """
            Thread.__init__(self)
            self.dlen = dlen
            self.segment_size = segment_size
            self.dbuf_temp = dbuf_temp
            # self.dbuf_temp.extend([0] * (dlen - len(self.dbuf_temp)))
            self.timeout_ms = timeout_ms
            self.serial = serial
            self.timeout = False
            self.drx = 0
            self._is_husky = is_husky
            self.stop = False

        def run(self):
            naeusb_logger.debug("Streaming: starting USB read")
            start = time.time()
            transfer_list = []
            self.drx = 0
            try:
                # self.drx = self.serial.usbtx.read(self.dbuf_temp, timeout=self.timeout_ms)
                num_transfers = int(self.dlen // self.segment_size)
                if (self.dlen % self.segment_size) != 0:
                    num_transfers += 1
                naeusb_logger.info("Doing {} transfers".format(num_transfers))
                naeusb_logger.info("Cal'd from dlen = {} and segment_len = {}".format(self.dlen, self.segment_size))
                for i in range(num_transfers):
                    transfer = self.serial.usbtx.handle.getTransfer()
                    transfer.setBulk(usb1.ENDPOINT_IN | 0x05, \
                        self.segment_size, \
                        callback=self.callback)
                    transfer.submit()
                    transfer_list.append(transfer)
            except IOError as e:
                raise

            diff = time.time() - start
            while any(x.isSubmitted() for x in transfer_list):
                # handleEvents does the callbacks
                try:
                    self.serial.usbtx.usb_ctx.handleEvents()
                    if self.stop:
                        self.stop = False
                        for transfer in transfer_list:
                            if transfer.isSubmitted():
                                transfer.cancel()
                except usb1.USBErrorInterrupted:
                    pass
            naeusb_logger.info("Streaming: Received %d bytes in time %.20f)" % (self.drx, diff))

        def callback(self, transfer : usb1.USBTransfer):
            """ Handle finished asynchronous bulk transfer"""
            if transfer.getStatus() == usb1.TRANSFER_CANCELLED:
                return
            if (self.drx >= self.dlen):
                self.drx += transfer.getActualLength()
                return
            if (transfer.getActualLength() == 0) and (self.drx < self.dlen):
                transfer.submit()
                naeusb_logger.info("Got 0 bytes back from stream with error {}".format(transfer.getStatus()))
                return

            self.dbuf_temp[self.drx:self.drx+transfer.getActualLength()] = array.array('B', transfer.getBuffer()[:transfer.getActualLength()])
            self.drx += transfer.getActualLength()
            if transfer.getStatus() != usb1.TRANSFER_COMPLETED:
                transfer.submit()
                naeusb_logger.error("Stream failed with error {}, retrying".format(transfer.getStatus()))
                return
            naeusb_logger.debug("stream completed with {} bytes".format(transfer.getActualLength()))

    class StreamModeCaptureThreadPro(Thread):
        def __init__(self, serial, dlen : int, dbuf_temp : bytearray, timeout_ms : int=2000):
            """
            Reads from the FIFO in streaming mode. Requires the FPGA to be previously configured into
            streaming mode and then arm'd, otherwise this may return incorrect information.
            Args:
                dlen: Number of samples to request.
                dbuf_temp: Temporary data buffer, must be of size cmdReadStream_bufferSize(dlen) or bad things happen
                timeout_ms: Timeout in ms to wait for stream to start, otherwise returns a zero-length buffer
            Returns:
                Tuple of (samples_per_block, total_bytes_rx)
            """
            Thread.__init__(self)
            self.dlen = dlen
            self.dbuf_temp = dbuf_temp
            self.timeout_ms = timeout_ms
            self.serial = serial
            self.timeout = False
            self.drx = 0

        def run(self):
            naeusb_logger.info("Streaming: starting USB read")
            start = time.time()
            try:
                x = self.serial.usbtx.read(len(self.dbuf_temp), timeout=self.timeout_ms)
                self.dbuf_temp[:] = array.array('B', x)[:]
                self.drx = len(x)
            except Exception as e:
                naeusb_logger.warning('Streaming: USB stream read timed out')
            diff = time.time() - start
            naeusb_logger.info("Streaming: Received %d bytes in time %.20f)" % (self.drx, diff))
            naeusb_logger.info("Expected {}".format(len(self.dbuf_temp)))

    def cmdReadStream_getStatus(self) -> Tuple[int, int, int]:
        """
        Gets the status of the streaming mode capture, tells you samples left to stream out along
        with overflow buffer status. When an overflow occurs the samples left to stream goes to
        zero.
        samples_left_to_stream is number of samples not yet streamed out of buffer.
        overflow_lcoation is the value of samples_left_to_stream when a buffer overflow occured.
        unknown_overflow is a flag indicating if an overflow occured at an unknown time.
        Returns:
            Tuple indicating (samples_left_to_stream, overflow_location, unknown_overflow)
        """
        data = self.readCtrl(self.CMD_MEMSTREAM, dlen=9)

        status = data[0]
        samples_left_to_stream = unpackuint32(data[1:5])
        overflow_location = unpackuint32(data[5:9])

        if status == 0:
            unknown_overflow = False
        else:
            unknown_overflow = True

        return (samples_left_to_stream, overflow_location, unknown_overflow)

    def cmdReadStream_size_of_fpgablock(self) -> int:
        """ Asks the hardware how many BYTES are read in one go from FPGA, which indicates where the sync
            bytes will be located. These sync bytes must be removed in post-processing. CW-pro only. """
        return 4096

    def cmdReadStream_bufferSize(self, dlen : int):
        """
        Args:
            dlen: Number of samples to be requested (will be rounded to something else)
        Returns:
            Tuple: (Size of temporary buffer required, actual samples in buffer)
        """
        num_samplebytes = int(math.ceil(float(dlen) * 4 / 3))
        num_blocks = int(math.ceil(float(num_samplebytes) / 4096))
        num_totalbytes = num_samplebytes + num_blocks
        num_totalbytes = int(math.ceil(float(num_totalbytes) / 4096) * 4096)
        return num_totalbytes



    def initStreamModeCapture(self, dlen : int, dbuf_temp : bytearray, timeout_ms : int=1000,
        is_husky : bool=False, segment_size : int=0):
        #Enter streaming mode for requested number of samples
        if self.streamModeCaptureStream:
            self.streamModeCaptureStream.join()
        if is_husky:
            data=list(int.to_bytes(segment_size, length=4, byteorder='little')) + \
                list(int.to_bytes(3, length=4, byteorder='little')) + list(int.to_bytes(dlen, length=4, byteorder="little"))
        else:
            data = packuint32(dlen)
        self.sendCtrl(NAEUSB.CMD_MEMSTREAM, data=bytearray(data))
        if is_husky:
            self.streamModeCaptureStream = NAEUSB.StreamModeCaptureThreadHusky(self, dlen, segment_size, dbuf_temp, timeout_ms, is_husky)
        else:
            self.streamModeCaptureStream = NAEUSB.StreamModeCaptureThreadPro(self, dlen, dbuf_temp, timeout_ms)
        self.streamModeCaptureStream.start()

    def cmdReadStream_isDone(self, is_husky : bool=False) -> bool:
        if is_husky:
            return self.streamModeCaptureStream.drx >= self.streamModeCaptureStream.dlen
        else:
            return self.streamModeCaptureStream.drx >= self.streamModeCaptureStream.dlen
            # return self.streamModeCaptureStream.isAlive() == False

    def cmdReadStream(self, is_husky : bool=False) -> Tuple[int, int]:
        """
        Gets data acquired in streaming mode.
        initStreamModeCapture should be called first in order to make it work.
        """
        self.streamModeCaptureStream.join()
        # Flush input buffers in case anything was left
        try:
            #self.cmdReadMem(self.rep)
            self.usbtx.read(4096, timeout=10)
            self.usbtx.read(4096, timeout=10)
            self.usbtx.read(4096, timeout=10)
            self.usbtx.read(4096, timeout=10)
        except:
            pass

        # Ensure stream mode disabled
        if not is_husky:
            self.sendCtrl(NAEUSB.CMD_MEMSTREAM, data=packuint32(0))
        return self.streamModeCaptureStream.drx, self.streamModeCaptureStream.timeout

    # def readCDCSettings(self):
    #     try:
    #         data = self.readCtrl(self.CMD_FW_VERSION, dlen=3)
    #         return data
    #     except:
    #         return [0, 0]

    def enterBootloader(self, forreal : bool=False):
        """Erase the SAM3U contents, forcing bootloader mode. Does not screw around."""

        if forreal:
            self.sendCtrl(0x22, 3)

    def reset(self):
        """ Reset the SAM3U. Requires firmware 0.30 or later
        """
        if self.check_feature("RESET", True):
            self.sendCtrl(0x22, 0x10)

    def read(self, dlen : int, timeout : int=2000) -> bytearray:
        return self.usbserializer.read(dlen, timeout)

    def check_feature(self, feature, raise_exception=False) -> bool:
        prod_id = self.usbtx.device.getProductID()
        fw_ver_list = self.readFwVersion()
        fw_ver_str = '{}.{}.{}'.format(fw_ver_list[0], fw_ver_list[1], fw_ver_list[2])
        ret = _check_sam_feature(feature, fw_ver_str, prod_id)
        if not ret:
            naeusb_logger.info("Feature {} not available".format(feature))
            if raise_exception:
                raise CWFirmwareError("Feature {} not available. FW {} required (have {})".format(feature, SAM_FW_FEATURE_BY_DEVICE[prod_id][feature], fw_ver_str))
        return ret

    def feature_list(self):
        feature_list = []
        for feature in SAM_FW_FEATURES:
            if self.check_feature(feature):
                feature_list.append(feature)

        return feature_list


if __name__ == '__main__':
    import chipwhisperer as cw
    scope = cw.scope()

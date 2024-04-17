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
import sys
import array
from typing import Optional, Union, List, Tuple, Dict, cast
from ...common.utils import util
from ...common.utils.util import CWByteArray # type: ignore

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
    "MPSSE_ENABLED", #15
    "HUSKY_PIN_CONTROL", #16
    "NANO_CLOCK_RESET", #17
    "SAM_ERR_LED", #18
    "XON_XOFF", #19
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
        SAM_FW_FEATURES[8]: '0.23.0',
        SAM_FW_FEATURES[13]: '0.60.0',
        SAM_FW_FEATURES[15]: '0.62.0',
        SAM_FW_FEATURES[17]: '0.64.0',
        SAM_FW_FEATURES[18]: '0.66.0',
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
        SAM_FW_FEATURES[14]: '0.60.0',
        SAM_FW_FEATURES[15]: '0.62.0',
        SAM_FW_FEATURES[18]: '0.65.0',
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
        SAM_FW_FEATURES[14]: '1.60.0',
        SAM_FW_FEATURES[15]: '1.62.0',
    },

    0xACE5: {
        SAM_FW_FEATURES[0]: '1.0.0',
        SAM_FW_FEATURES[1]: '1.0.0',
        SAM_FW_FEATURES[2]: '1.1.0',
        SAM_FW_FEATURES[3]: '1.0.0',
        SAM_FW_FEATURES[4]: '1.0.0',
        SAM_FW_FEATURES[5]: '1.0.0',
        SAM_FW_FEATURES[6]: '1.0.0',
        SAM_FW_FEATURES[7]: '1.0.0',
        SAM_FW_FEATURES[8]: '1.0.0',
        SAM_FW_FEATURES[9]: '1.0.0',
        SAM_FW_FEATURES[13]: '1.1.0',
        SAM_FW_FEATURES[14]: '1.1.0',
        SAM_FW_FEATURES[15]: '1.3.0',
        SAM_FW_FEATURES[16]: '1.4.0',
        SAM_FW_FEATURES[18]: '1.5.0',
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
        SAM_FW_FEATURES[4]: '0.40.1',
        SAM_FW_FEATURES[5]: '1.0.0',
        SAM_FW_FEATURES[6]: '1.0.0',
        SAM_FW_FEATURES[7]: '1.0.0',
        SAM_FW_FEATURES[8]: '1.0.0',
        SAM_FW_FEATURES[10]: '1.0.0',
        SAM_FW_FEATURES[11]: '1.0.0',
        SAM_FW_FEATURES[12]: '1.1.0',
        SAM_FW_FEATURES[13]: '1.2.0',
        SAM_FW_FEATURES[19]: '1.4.0',
    },
    
    0xC340: {
        SAM_FW_FEATURES[0]: '0.1.0',
        SAM_FW_FEATURES[1]: '0.1.0',
        SAM_FW_FEATURES[2]: '0.1.0',
        SAM_FW_FEATURES[3]: '0.1.0',
        SAM_FW_FEATURES[4]: '0.1.0',
        SAM_FW_FEATURES[5]: '0.1.0',
        SAM_FW_FEATURES[6]: '0.1.0',
        SAM_FW_FEATURES[7]: '0.1.0',
        SAM_FW_FEATURES[8]: '0.1.0',
        SAM_FW_FEATURES[10]: '0.1.0',
        SAM_FW_FEATURES[12]: '0.1.0',
    }
}

def quick_firmware_erase(product_id, serial_number=None):
    naeusb = NAEUSB()
    naeusb.con(serial_number=serial_number, idProduct=[product_id])
    naeusb.enterBootloader(True)


def _check_sam_feature(feature, fw_version, prod_id):
    if prod_id not in SAM_FW_FEATURE_BY_DEVICE:
        naeusb_logger.debug("Features for ProdID {:04X} not stored, skipping...".format(prod_id))
        return
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
        keyhandle = winreg.OpenKey(winreg.HKEY_LOCAL_MACHINE, "SYSTEM") # type: ignore
        subkey = r"ControlSet001\Enum\USB"
        subkey += "\\VID_{:04X}&PID_{:04X}".format(device.getVendorID(), device.getProductID())

        def get_enum_by_name(handle, name):
            try:
                cnt = 0
                enum_name = ""
                myenum = None
                naeusb_logger.debug('Looking for {}...'.format(name))
                while enum_name != name:
                    myenum = winreg.EnumValue(handle, cnt) # type: ignore
                    enum_name = myenum[0]
                    cnt += 1
                    naeusb_logger.debug('Found {}'.format(enum_name))
                return myenum[1]
            except OSError as e:
                return None

        # get devices with same PID/VID
        try:
            keyhandle_device = winreg.OpenKey(keyhandle, subkey) # type: ignore
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
                sn = winreg.EnumKey(keyhandle_device, i) # type: ignore
            except Exception as e:
                naeusb_logger.info("Could not get sn " + str(e)) 
                return None
            # print("sn: " + sn)
            keyhandle_sn = winreg.OpenKey(keyhandle_device, sn) # type: ignore
            with keyhandle_sn as h:
                address = get_enum_by_name(h, "Address")
                if address is None:
                    naeusb_logger.info("Could not find Address in device {}".format(sn))
                    return None
                service = get_enum_by_name(h, "Service")

                # now we need to figure out if this device is attached
                # Windows really doesn't make this easy...
                try:
                    keyhandle_driver = winreg.OpenKey(winreg.HKEY_LOCAL_MACHINE, "SYSTEM\\CurrentControlSet\\Services\\{}\\Enum".format(service)) # type: ignore
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
            keyhandle_sn = winreg.OpenKey(keyhandle_device, sn) # type: ignore
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

LEN_ADDR_HDR_SIZE = 8
    
def set_len_addr(buf, dlen, addr):
    """Populates a buffer with the command header.
    """
    # Little endian
    util.pack_u32_into(buf, 0, dlen)
    util.pack_u32_into(buf, 4, addr)

def make_len_addr(dlen, addr):
    """Creates a command header buffer.

    Return:
        A bytearray with the populated command parameters.
    """
    buf = bytearray(LEN_ADDR_HDR_SIZE)
    set_len_addr(buf, dlen, addr)
    return buf

NAEUSB_CTRL_IO_MAX = 128
NAEUSB_CTRL_IO_THRESHOLD = 48

#List of all NewAE PID's
NEWAE_VID = 0x2B3E
NEWAE_PIDS = {
    0xACE2: {'name': "ChipWhisperer-Lite",     'fwver': fw_cwlite.fwver},
    0xACE3: {'name': "ChipWhisperer-CW1200",   'fwver': fw_cw1200.fwver},
    0xC305: {'name': "CW305 Artix FPGA Board", 'fwver': fw_cw305.fwver},
    0xACE0: {'name': "ChipWhisperer-Nano", 'fwver': fw_nano.fwver},
    0xACE5: {'name': "ChipWhisperer-Husky",   'fwver': fw_cwhusky.fwver},
    0xC521: {'name': "CW521 Ballistic-Gel",   'fwver': None},
    0xC610: {'name': "PhyWhisperer-USB",   'fwver': None},
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
        if os.name == "nt" or sys.platform == "darwin":
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
        
        dev_list = [dev for dev in self.usb_ctx.getDeviceIterator(skip_on_error=True) if dev.getVendorID() == 0x2b3e]
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

        for dev in dev_list[:]:
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
        if len(data) > NAEUSB_CTRL_IO_MAX:
            naeusb_logger.error("The naeusb fw ctrl buffer is 128 bytes, but len(data) > 128. If you get a pipe error, this is why.")
        self.handle.controlWrite(0x41, cmd, value, 0, data, timeout=self._timeout)
        #return self.usbdev().ctrl_transfer(0x41, cmd, value, 0, data, timeout=self._timeout)

    def readCtrl(self, cmd : int, value : int=0, dlen : int=0) -> bytearray:
        """
        Read data from control endpoint
        """
        # Vendor-specific, IN, interface control transfer
        if dlen > NAEUSB_CTRL_IO_MAX:
            naeusb_logger.error("The naeusb fw ctrl buffer is 128 bytes, but len(data) > 128. If you get a pipe error, this is why.")
        response = self.handle.controlRead(0xC1, cmd, value, 0, dlen, timeout=self._timeout)
        naeusb_logger.debug("READ_CTRL: bmRequestType: {:02X}, \
                    bRequest: {:02X}, wValue: {:04X}, wIndex: {:04X}, data_len: {:04X}, response: {}".format(0xC1, cmd, \
                        value, 0, dlen, response))
        return response

    def _get_timeout(self, timeout):
        """Gets the default timeout if the operation caller did not specify one.

        Returns:
            A valid timeout value.
        """
        if timeout is None:
            timeout = self._timeout
        return timeout

    def _bulk_read(self, data, timeout):
        """Reads data over the bulk-transfer endpoint.

        Returns:
            The received data.
        """
        timeout = self._get_timeout(timeout)
        return self.handle.bulkRead(self.rep, data, timeout)

    def _bulk_write(self, data, timeout):
        """Writes data over the bulk-transfer endpoint.
        """
        timeout = self._get_timeout(timeout)
        self.handle.bulkWrite(self.wep, data, timeout)

    def _cmd_ctrl_send_data(self, pload, cmd : int):
        """Sends data over the control-transfer channel and attempts a pipe error fix if an initial
        error occured.
        """
        try:
            self.sendCtrl(cmd, data=pload)
        except usb1.USBErrorPipe:
            naeusb_logger.info("Attempting pipe error fix - typically safe to ignore")
            self.sendCtrl(0x22, 0x11)
            self.sendCtrl(cmd, data=pload)

    def _cmd_ctrl_send_header(self, addr : int, dlen : int, cmd : int):
        """Sends the standard length/addr header over the control-transfer endpoint.
        """
        # TODO: Alloc header class member? Won't hafta alloc mem every read and writectrl call...
        pload = make_len_addr(dlen, addr)
        self._cmd_ctrl_send_data(pload, cmd)

    def _cmd_readmem_ctrl(self, addr : int, dlen : int):
        """Reads data from the external memory interface over the control-transfer endpoint.

        Returns:
            The received data.
        """
        self._cmd_ctrl_send_header(addr, dlen, self.CMD_READMEM_CTRL);
        return self.readCtrl(self.CMD_READMEM_CTRL, dlen=dlen)

    def _cmd_readmem_bulk(self, addr : int, dlen : int):
        """Reads data from the external memory interface over the bulk-transfer endpoint.

        Returns:
            The received data.
        """
        self._cmd_ctrl_send_header(addr, dlen, self.CMD_READMEM_BULK);
        return self._bulk_read(dlen, None)

    def cmdReadMem(self, addr : int, dlen : int) -> bytearray:
        """
        Send command to read over external memory interface from FPGA. Automatically
        decides to use control-transfer or bulk-endpoint transfer based on data length.
        """
        dlen = int(dlen)
        if dlen < NAEUSB_CTRL_IO_THRESHOLD:
            data = self._cmd_readmem_ctrl(addr, dlen)
        else:
            data = self._cmd_readmem_bulk(addr, dlen)

        naeusb_logger.debug("FPGA_READ: bulk: {}, addr: {:08X}, dlen: {:08X}, response: {}"\
            .format("yes" if dlen >= NAEUSB_CTRL_IO_THRESHOLD else "no", addr, dlen, data))
        return data

    def _cmd_writemem_ctrl(self, addr : int, data):
        """Writes data to the external memory interface via the control-transfer endpoint.
        """
        # TODO: Investigate if we don't hafta combine header with the data and can send separately.
        # Is this is a FW implementation or a limitation from middleware interfaces?
        pload = bytearray(LEN_ADDR_HDR_SIZE + len(data))
        set_len_addr(pload, len(data), addr)
        util.bytes_fast_copy(pload, LEN_ADDR_HDR_SIZE, data)
        self._cmd_ctrl_send_data(pload, self.CMD_WRITEMEM_CTRL)

    def _cmd_writemem_bulk(self, addr : int, data):
        """Writes data to the external memory interface via the bulk-transfer endpoint.
        """
        self._cmd_ctrl_send_header(addr, len(data), self.CMD_WRITEMEM_BULK)
        self._bulk_write(data, None)

    def cmdWriteMem(self, addr : int, data):
        """
        Send command to write memory over external memory interface to FPGA. Automatically
        decides to use control-transfer or bulk-endpoint transfer based on data length.
        """
        pload = util.get_bytes_memview(data)
        if len(pload) < NAEUSB_CTRL_IO_THRESHOLD:
            self._cmd_writemem_ctrl(addr, pload)
        else:
            self._cmd_writemem_bulk(addr, pload)

        naeusb_logger.debug("FPGA_WRITE: bulk: {}, addr: {:08X}, dlen: {:08X}, response: {}"\
            .format("yes" if len(pload) >= NAEUSB_CTRL_IO_THRESHOLD else "no", addr, len(pload), data))

        return None

    def cmdWriteBulk(self, data : bytearray, timeout = None):
        """
        Write data directly to the bulk endpoint.
        :param data: Data to be written
        :return:
        """
        naeusb_logger.debug("BULK WRITE: data = {}".format(data))
        self._bulk_write(data, timeout)

    writeBulk = cmdWriteBulk

    def flushInput(self):
        """Dump all the crap left over"""
        try:
            # TODO: This probably isn't needed, and causes slow-downs on Mac OS X.
            self._bulk_read(1000, 0.010)
        except:
            pass

    def read(self, dbuf : bytearray, timeout : int) -> bytearray:
        resp = self._bulk_read(dbuf, timeout)
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

    def is_MPSSE_enabled(self):
        if self.check_feature("MPSSE_ENABLED"):
            return self.readCtrl(0x22, 0x42, 1)[0] == 0x01

    def hw_location(self):
        return (self.usbtx.device.getBusNumber(), self.usbtx.device.getDeviceAddress())

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

    def set_husky_tms_wr(self, num):
        # TODO: add in check_feature
        if self.check_feature("HUSKY_PIN_CONTROL"):
            num &= 0xFF
            self.usbtx.sendCtrl(0x22, 0x43 | (num << 8))
        else:
            naeusb_logger.error("Cannot set Husky TMS direction pin. SWD mode will not work! A firmware update to >=1.4 is highly recommended!")

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
            name = NEWAE_PIDS[self.usbtx.pid]['name'] # type: ignore
            fw_latest = cast(List[int], NEWAE_PIDS[self.usbtx.pid]['fwver']) # type: ignore
        else:
            name = "Unknown (PID = %04x)"%self.usbtx.pid

        latest = fwver[0] > fw_latest[0] or (fwver[0] == fw_latest[0] and fwver[1] >= fw_latest[1])
        if not latest:
            naeusb_logger.warning('Your firmware (%d.%d) is outdated - latest is %d.%d' 
                             % (fwver[0], fwver[1], fw_latest[0], fw_latest[1]) +
                             ' See https://chipwhisperer.readthedocs.io/en/latest/firmware.html for more information')

        return self.usbtx.pid

    def usbdev(self):
        raise AttributeError("Do Not Call Me")

    def set_led_settings(self, setting=0):
        if self.check_feature("SAM_ERR_LED"):
            setting &= 0xFF
            self.sendCtrl(0x22, 0x12 | (setting << 8))

    def clear_sam_errors(self):
        if self.check_feature("SAM_ERR_LED"):
            self.sendCtrl(0x22, 0x13)

    def check_sam_errors(self):
        if self.check_feature("SAM_ERR_LED"):
            data = self.readCtrl(0x22, dlen=3)
            return (data[0] & 0xFF) | (data[1] << 8)
    
    def get_led_settings(self):
        if self.check_feature("SAM_ERR_LED"):
            data = self.readCtrl(0x22, dlen=3)
            return data[2]
        

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

    def writeBulkEP(self, data : bytearray, timeout = None):
        """
        Write directoly to the bulk endpoint.
        :param data: Data to be written.
        :return:
        """
        return self.usbserializer.writeBulk(data, timeout=timeout)

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
            # basically just setup a bunch of async transfers, then handle them via callback
            naeusb_logger.info("Streaming: starting USB read")
            transfer_list = []
            unsubmitted_transfers = []
            self.drx = 0
            stream_start =  time.time()
            try:
                num_transfers = int(self.dlen // self.segment_size)
                if (self.dlen % self.segment_size) != 0:
                    num_transfers += 1
                naeusb_logger.debug("Doing {} transfers".format(num_transfers))
                naeusb_logger.debug("Calc'd from dlen = {} and segment_len = {}".format(self.dlen, self.segment_size))
                for i in range(num_transfers):
                    transfer = self.serial.usbtx.handle.getTransfer()
                    transfer.setBulk(usb1.ENDPOINT_IN | 0x05, \
                        self.segment_size, \
                        callback=self.callback)
                    try:
                        transfer.submit()
                        transfer_list.append(transfer)
                    except usb1.USBError as e:
                        # On Linux, trying to allocate for > ~10M samples seems to not work (ENOMEM)
                        # Putting them in a list and attempting a resubmit later seems to fix things
                        unsubmitted_transfers.append(transfer)
                        naeusb_logger.info("Unsubmitted transfer, will try again later. Err = {}".format(str(e)))
            except IOError as e:
                raise

            # basically poll all the transfers we've setup
            start = time.time()
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
                    
                # try resubmitting transfers that failed earlier (likely due to enomem)
                for transfer in unsubmitted_transfers:
                    try:
                        naeusb_logger.info("Attempting transfer resubmit")
                        transfer.submit()
                        unsubmitted_transfers.remove(transfer)
                        transfer_list.append(transfer)
                    except usb1.USBError as e:
                        naeusb_logger.info("Still can't handle this: {}".format(str(e))) # this will probably still happen a lot before it works
                        diff = (time.time() - start) * 1000
                        if diff > self.timeout_ms: # if the capture has timed out
                            naeusb_logger.error("Libusb async transfer request failed with: {}".format(str(e)))
                            naeusb_logger.error("NOTE: If you're doing a long transfer, try increasing scope.adc.timeout")

                            # cancel all submitted transfers to prevent pipe errors
                            for transfer in transfer_list:
                                if transfer.isSubmitted():
                                    transfer.cancel()
                            raise e
                
            naeusb_logger.info("Streaming: Received %d bytes in time %.20f)" % (self.drx, time.time() - stream_start))

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
            naeusb_logger.debug("Streaming: Received %d bytes in time %.20f)" % (self.drx, diff))
            naeusb_logger.debug("Expected {}".format(len(self.dbuf_temp)))

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

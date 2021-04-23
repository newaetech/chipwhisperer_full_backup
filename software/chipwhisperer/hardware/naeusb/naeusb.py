#
# Copyright (c) 2014-2018, NewAE Technology Inc
# All rights reserved.
#
# Find this and more at newae.com - this file is part of the chipwhisperer
# project, http://www.chipwhisperer.com
#
#    This file is part of chipwhisperer.
#
#    chipwhisperer is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    chipwhisperer is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU Lesser General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with chipwhisperer.  If not, see <http://www.gnu.org/licenses/>.
# ChipWhisperer is a trademark of NewAE Technology Inc., registered in the
# United States of America, the European Union, and other jurisdictions.
# ==========================================================================
import logging
import warnings
import time
import math
from threading import Condition, Thread
import struct
import pickle
import traceback
import os

from usb.backend import libusb0, libusb1
import usb.core
import usb.util

from chipwhisperer.hardware.firmware import cwlite as fw_cwlite
from chipwhisperer.hardware.firmware import cw1200 as fw_cw1200
from chipwhisperer.hardware.firmware import cw305  as fw_cw305
from chipwhisperer.hardware.firmware import cwnano  as fw_nano
from chipwhisperer.hardware.firmware import cwhusky as fw_cwhusky

from chipwhisperer.logging import *


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
NEWAE_PIDS = {
    0xACE2: {'name': "ChipWhisperer-Lite",     'fwver': fw_cwlite.fwver},
    0xACE3: {'name': "ChipWhisperer-CW1200",   'fwver': fw_cw1200.fwver},
    0xC305: {'name': "CW305 Artix FPGA Board", 'fwver': fw_cw305.fwver},
    0xACE0: {'name': "ChipWhisperer-Nano", 'fwver': fw_nano.fwver},
    0xACE5: {'name': "ChipWhisperer-Husky",   'fwver': fw_cwhusky.fwver},
}

class NAEUSB_Serializer_base(object):
    """
    Base clase for packaging USB commands. The seralizer class is used when the USB may talk via a daemon instead
    of directly running from the Python code. This is useful when you want the USB device to be handled asynchronously,
    such as required to keep serial data being sent to/from the device.
    """

    FLUSH_INPUT = 0xE0
    CLOSE = 0xF0
    OPEN = 0xF1
    WRITE_CTRL = 0xF2
    READ_CTRL = 0xF3
    CMD_WRITE_MEM = 0xF4
    CMD_READ_MEM = 0xF5
    GET_POSSIBLE_DEVICES = 0xF6
    WRITE_BULK = 0xF7
    READ_BULK = 0xF8
    CMD_WRITE_MEM_SAM3U = 0xF9


    ACK = 0xA0
    ERROR = 0xA1

    def make_cmd(self, cmd, datalist):

        if datalist is None:
            pcmd = None
        else:
            pcmd = pickle.dumps(datalist)

        hdr = bytes(bytearray([cmd]))

        if pcmd:
            hdr += struct.pack("!I", len(pcmd))
            cmdpacket = hdr + pcmd
        else:
            hdr += struct.pack("!I", 0)
            cmdpacket = hdr

        return cmdpacket

class NAEUSB_Serializer(NAEUSB_Serializer_base):
    """
    This class does not talk to libusb directly, but instead uses a class that expects special commands passed
    to it. The function which will process commands must be passed to the initilizer, which allows passing the
    data over a network or similar.
    """

    def __init__(self, transmitfunc):

        self.txrx = transmitfunc


    def get_possible_devices(self, idProduct=None):
        """Get a list of connected USB devices."""
        cmdpacket = self.make_cmd(self.GET_POSSIBLE_DEVICES, idProduct)
        return self.process_rx(self.txrx(tx=cmdpacket))

    def process_rx(self, inp):
        """Process the data received back from the end, normally means find errors or return payload."""
        resp = inp[0]

        plen = struct.unpack("!I", inp[1:5])[0]

        if plen > 0:

            pdata = inp[5:]
            if plen != len(pdata):
                raise ValueError("This pickle smells funny")

            payload = pickle.loads(pdata)
        else:
            payload = None


        if resp == self.ERROR:
            raise payload

        return payload

    def open(self, serial_number = None):
        """Opens USB device"""
        cmdpacket = self.make_cmd(self.OPEN, serial_number)
        return self.process_rx(self.txrx(tx=cmdpacket))

    def close(self, serial_number):
        """Close USB connection."""
        cmdpacket = self.make_cmd(self.CLOSE, serial_number)
        return self.process_rx(self.txrx(tx=cmdpacket))

    def sendCtrl(self, cmd, value=0, data=[]):
        """
        Send data over control endpoint
        """
        # Vendor-specific, OUT, interface control transfer

        cmdpacket = [0x41, cmd, value, 0, len(data)]
        cmdpacket.extend(data)

        cmdpacket = self.make_cmd(self.WRITE_CTRL, cmdpacket)

        self.process_rx(self.txrx(tx=cmdpacket))

    def readCtrl(self, cmd, value=0, dlen=0):
        """
        Read data from control endpoint
        """

        cmdpacket = [0xC1, cmd, value, 0, dlen]
        cmdpacket = self.make_cmd(self.READ_CTRL, cmdpacket)
        return self.process_rx(self.txrx(tx=cmdpacket))


    def cmdReadMem(self, addr, dlen):
        """
        Send command to read over external memory interface from FPGA. Automatically
        decides to use control-transfer or bulk-endpoint transfer based on data length.
        """

        dlen = int(dlen)
        payload = [addr, dlen]
        cmdpacket = self.make_cmd(self.CMD_READ_MEM, payload)

        return self.process_rx(self.txrx(tx=cmdpacket))

    def cmdWriteMem(self, addr, data):
        """
        Send command to write memory over external memory interface to FPGA. Automatically
        decides to use control-transfer or bulk-endpoint transfer based on data length.
        """

        dlen = len(data)

        payload = [addr]
        payload.extend(data)
        cmdpacket = self.make_cmd(self.CMD_WRITE_MEM, payload)
        return self.process_rx(self.txrx(tx=cmdpacket))

    def cmdWriteSam3U(self, addr, data):
        """
        Send command to write memory over the SAM3U. 
        """

        dlen = len(data)

        payload = [addr]
        payload.extend(data)
        cmdpacket = self.make_cmd(self.CMD_WRITE_MEM_SAM3U, payload)
        return self.process_rx(self.txrx(tx=cmdpacket))


    def writeBulk(self, data):
        """
        Low-level function.
        Writes bulk data to the bulk USB endpoint.
        :param data: Data to write to the endpoint
        :return:
        """
        cmdpacket = self.make_cmd(self.WRITE_BULK, data)
        return self.process_rx(self.txrx(tx=cmdpacket))

    def flushInput(self):
        """
        Of dubious value: flushes the USB endpoints. Causes slowdowns on MAC OS X, so need to investigate
        the usefulness of this.
        """

        cmdpacket = self.make_cmd(self.FLUSH_INPUT, None)
        return self.process_rx(self.txrx(tx=cmdpacket))


class NAEUSB_Backend(NAEUSB_Serializer_base):
    """
    This backend actually talks to the USB device itself. It is designed to mostly be used via the serializer, but
    can be called directly too.
    """

    CMD_READMEM_BULK = 0x10
    CMD_WRITEMEM_BULK = 0x11
    CMD_READMEM_CTRL = 0x12
    CMD_WRITEMEM_CTRL = 0x13
    CMD_MEMSTREAM = 0x14
    CMD_WRITEMEM_CTRL_SAM3U = 0x15

    def __init__(self):
        self._usbdev = None
        self._timeout = 500

    def usbdev(self):
        """Safely get USB device, throwing error if not connected"""

        if not self._usbdev: raise OSError("USB Device not found. Did you connect it first?")
        return self._usbdev

    def txrx(self, tx=[]):
        """
        Process USB command, and returns a result such as data or an encoded exception. Excepts that happen
        are not raised, but instead only printed (for debug) and passed back.
        """

        response = None

        try:
            #Get command
            cmd = tx[0]
            pickle_len = struct.unpack("!I", tx[1:5])[0]

            if pickle_len > 0:
                pickle_data = tx[5:]

                if len(pickle_data) != pickle_len:
                    raise ValueError("Pickle smells funny. Check best before date.")

                payload = pickle.loads(pickle_data)
            else:
                payload = None

            if cmd == self.READ_CTRL:
                response = self.usbdev().ctrl_transfer(payload[0], payload[1], payload[2], payload[3], payload[4], timeout=self._timeout)
                naeusb_logger.debug("READ_CTRL: bmRequestType: {:02X}, \
                    bRequest: {:02X}, wValue: {:04X}, wIndex: {:04X}, data_len: {:04X}, response: {}".format(payload[0], payload[1], \
                        payload[2], payload[3], payload[4], response))
            elif cmd == self.WRITE_CTRL:
                if payload[4] != len(payload[5:]):
                    raise ValueError("Specified payload length & actual do not match")
                self.usbdev().ctrl_transfer(payload[0], payload[1], payload[2], payload[3], payload[5:], timeout=self._timeout)
                naeusb_logger.debug("READ_CTRL: bmRequestType: {:02X}, \
                    bRequest: {:02X}, wValue: {:04X}, wIndex: {:04X}, payload: {}".format(payload[0], payload[1], payload[2], payload[3],\
                         payload[4], payload[5:]))
            elif cmd == self.CMD_READ_MEM:
                addr = payload[0]
                dlen = payload[1]
                response = self.cmdReadMem(addr, dlen)
                naeusb_logger.debug("CMD_READ_MEM: addr: {:08X}, dlen: {:08X}, response: {}".format(addr, dlen, response))
            elif cmd == self.CMD_WRITE_MEM:
                addr = payload[0]
                data = payload[1:]
                self.cmdWriteMem(addr, data)
                naeusb_logger.debug("CMD_WRITE_MEM: addr: {:08X}, data: {}".format(addr, data))
            elif cmd == self.CMD_WRITE_MEM_SAM3U:
                addr = payload[0]
                data = payload[1:]
                self.cmdWriteSam3U(addr, data)
                #naeusb_logger.debug("CMD_WRITE_MEM_SAM3U: addr: {}") #skip this one for now
            elif cmd == self.GET_POSSIBLE_DEVICES:
                response = self.get_possible_devices(payload)
            elif cmd == self.OPEN:
                response = self.open(serial_number=payload)
            elif cmd == self.CLOSE:
                self.close()
            elif cmd == self.WRITE_BULK:
                self.cmdWriteBulk(payload)
            elif cmd == self.FLUSH_INPUT:
                self.flushInput()
            elif cmd == self.READ:
                dlen = payload[0]
                response = self.read(dlen)
            else:
                raise ValueError("Unknown Command: %02x"%cmd)
        except Exception as e:
            #traceback.print_exc()
            raise #ugg
            return self.make_cmd(self.ERROR, e)

        return self.make_cmd(self.ACK, response)

    def open(self, serial_number=None, connect_to_first=False):
        """
        Connect to device using default VID/PID
        """

        devlist = self.get_possible_devices(dictonly=False)
        snlist = [d.serial_number + " (" + d.product + ")\n" for d in devlist]
        snlist = "".join(snlist)

        if len(devlist) == 0:
            raise OSError("Failed to find USB Device")

        elif serial_number:
            dev = None
            for d in devlist:
                if d.serial_number == serial_number:
                    dev = d
                    break

            if dev is None:
                raise OSError("Failed to find USB device with S/N %s\n. Found S/N's:\n" + snlist)

        elif len(devlist) == 1:
            dev = devlist[0]

        else:
            if connect_to_first:
                dev = devlist[0]
            else:
                #User did not help us out - throw it in their face
                raise Warning("Found multiple potential USB devices. Please specify device to use. Possible S/Ns:\n" + snlist)

        # Get serial number
        try:
            # New calling syntax
            self.snum = usb.util.get_string(dev, index=3)

        except TypeError:
            # Old calling syntax
            self.snum = usb.util.get_string(dev, length=256, index=3)

        foundId = dev.idProduct

        if foundId in NEWAE_PIDS:
            name = NEWAE_PIDS[foundId]['name']
            fw_latest = NEWAE_PIDS[foundId]['fwver']
        else:
            name = "Unknown (PID = %04x)"%foundId
            fw_latest = [0, 0]

        naeusb_logger.info('Found %s, Serial Number = %s' % (name, self.snum))

        self._usbdev = dev
        # TODO: how to tell if we're Husky at this point? maybe a try block?
        # for husky:
        # TEMPORARY: fix rep/wep in the notebook instead, so that the old values can be used for the CW305 target
        # self.rep = 0x85
        # self.wep = 0x06
        # for older capture HW:
        # self.rep = 0x81
        # self.wep = 0x02
        self.rep = None
        self.wep = None
        self._timeout = 200

        return foundId

    def close(self):
        """Close the USB connection"""
        try:
            usb.util.dispose_resources(self.usbdev())
        except usb.USBError as e:
            naeusb_logger.info('USB Failure calling dispose_resources: %s' % str(e))


    def get_possible_devices(self, idProduct=None, dictonly=True, backend="libusb1"):
        """
        Get a list of matching devices being based a list of PIDs. Returns list of usbdev that match (or empty if none)
        """
        if idProduct is None:
            idProduct = [None]

        my_kwargs = {'find_all': True, 'idVendor': 0x2B3E}
        if os.name == "nt":
            #on windows, need to manually load libusb because of 64bit python loading the wrong one
            libusb_backend = None
            if backend == "libusb1":
                import pathlib
                libusb_dir = pathlib.Path(__file__).parent.absolute()
                libusb_path = pathlib.Path(libusb_dir, "libusb-1.0.dll")
                libusb_backend = libusb1.get_backend(find_library=lambda x: str(libusb_path))
            if not libusb_backend:
                libusb_backend = libusb0.get_backend(find_library=lambda x: r"c:\Windows\System32\libusb0.dll")
                naeusb_logger.info("Using libusb0 backend")
            my_kwargs['backend'] = libusb_backend
        devlist = []
        try:
            for id in idProduct:
                if id:
                    dev = list(usb.core.find(idProduct=id, **my_kwargs))
                else:
                    dev = list(usb.core.find(**my_kwargs))
                #naeusb_logger.info("Devices found: {}".format(dev))
                if len(dev) > 0:
                    if len(dev) == 1:
                        devlist.extend(dev)
                    else:
                        # Deals with the multiple chipwhisperers attached but user only
                        # has permission to access a subset. The langid error is usually
                        # raised when there are improper permissions, so it is used to
                        # skip the those devices. However, the user is warned when this
                        # happens because the langid error is occasionally raised when
                        # there are backend errors.
                        for d in dev:
                            try:
                                d.serial_number
                                devlist.append(d)
                            except ValueError as e:
                                if "langid" in str(e):
                                    naeusb_logger.info('A device raised the "no langid" error, it is being skipped')
                                else:
                                    raise
            if dictonly:
                devlist = [{'sn': d.serial_number, 'product': d.product, 'pid': d.idProduct, 'vid': d.idVendor} for d in devlist]

            try:
                naeusb_logger.info("Devices found: {}".format(devlist))
                if len(devlist) > 0:
                    if dictonly:
                        devlist[0]['sn']
                    else:
                        devlist[0].serial_number
            except ValueError as e:
                if backend == "libusb1":
                    return self.get_possible_devices(idProduct, dictonly, "libusb0")
                raise
            return devlist
        except ValueError as e:
            if "langid" not in str(e):
                raise
            if backend == "libusb1":
                return self.get_possible_devices(idProduct, dictonly, "libusb0")
            raise OSError("Unable to communicate with found ChipWhisperer. Check that another process isn't connected to it and that you have permission to communicate with it.")

    def sendCtrl(self, cmd, value=0, data=[]):
        """
        Send data over control endpoint
        """
        # Vendor-specific, OUT, interface control transfer
        return self.usbdev().ctrl_transfer(0x41, cmd, value, 0, data, timeout=self._timeout)

    def readCtrl(self, cmd, value=0, dlen=0):
        """
        Read data from control endpoint
        """
        # Vendor-specific, IN, interface control transfer
        return self.usbdev().ctrl_transfer(0xC1, cmd, value, 0, dlen, timeout=self._timeout)

    def cmdReadMem(self, addr, dlen):
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
        except usb.USBError as e:
            if "Pipe error" in str(e):
                naeusb_logger.info("Attempting pipe error fix - typically safe to ignore")
                self.sendCtrl(0x22, 0x11)
                self.sendCtrl(cmd, data=pload)
            else:
                raise

        # Get data
        if cmd == self.CMD_READMEM_BULK:
            data = self.usbdev().read(self.rep, dlen, timeout=self._timeout)
            # XXX Husky debug:
            naeusb_logger.info('YYY BULK rep=%d, dlen=%d, got len=%d' % (self.rep, dlen, len(data)))
        else:
            data = self.readCtrl(cmd, dlen=dlen)

        return data

    def cmdWriteMem(self, addr, data):
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
        if (self.rep is None) or (self.wep is None):
            if self.usbdev().idProduct == 0xACE5: 
                self.rep = 0x85
                self.wep = 0x06
            else:
                self.rep = 0x81
                self.wep = 0x02
        if cmd == self.CMD_WRITEMEM_BULK:
            data = self.usbdev().write(self.wep, data, timeout=self._timeout)
        else:
            pass

        return data

    def cmdWriteSam3U(self, addr, data):
        """
        Send command to write memory over memory of SAMU3. 
        """

        dlen = len(data)

        if dlen < 48:
            cmd = self.CMD_WRITEMEM_CTRL_SAM3U
        else:
            cmd = self.ERROR

        # ADDR/LEN written LSB first
        pload = packuint32(dlen)
        pload.extend(packuint32(addr))

        if cmd == self.CMD_WRITEMEM_CTRL_SAM3U:
            pload.extend(data)

        self.sendCtrl(cmd, data=pload)
        
        return data

    def cmdWriteBulk(self, data):
        """
        Write data directly to the bulk endpoint.
        :param data: Data to be written
        :return:
        """

        if (self.rep is None) or (self.wep is None):
            if self.usbdev().idProduct == 0xACE5: 
                self.rep = 0x85
                self.wep = 0x06
            else:
                self.rep = 0x81
                self.wep = 0x02
        self.usbdev().write(self.wep, data, timeout=self._timeout)

    def flushInput(self):
        """Dump all the crap left over"""
        try:
            # TODO: This probably isn't needed, and causes slow-downs on Mac OS X.
            if (self.rep is None) or (self.wep is None):
                if self.usbdev().idProduct == 0xACE5: 
                    self.rep = 0x85
                    self.wep = 0x06
                else:
                    self.rep = 0x81
                    self.wep = 0x02
            self.usbdev().read(self.rep, 1000, timeout=0.010)
        except:
            pass

    def read(self, dbuf, timeout):
        if (self.rep is None) or (self.wep is None):
            if self.usbdev().idProduct == 0xACE5: 
                self.rep = 0x85
                self.wep = 0x06
            else:
                self.rep = 0x81
                self.wep = 0x02
        return self.usbdev().read(self.rep, dbuf, timeout)

class NAEUSB(object):
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

    stream = False

    # TODO: make this better
    fwversion_latest = [0, 11]
    def __init__(self):
        self._usbdev = None
        self.usbtx = NAEUSB_Backend()
        self.usbseralizer = NAEUSB_Serializer(self.usbtx.txrx)

    def get_possible_devices(self, idProduct):
        return self.usbseralizer.get_possible_devices(idProduct)

    def get_cdc_settings(self):
        return self.usbtx.readCtrl(self.CMD_CDC_SETTINGS_EN, dlen=2)

    def set_cdc_settings(self, port=[1, 1]):
        if isinstance(port, int):
            port = [port, port]
        self.usbtx.sendCtrl(self.CMD_CDC_SETTINGS_EN, (port[0]) | (port[1] << 1))


    def get_serial_ports(self):
        """May have multiple com ports associated with one device, so returns a list of port + interface
        """
        if not self.usbtx._usbdev:
            raise OSError("Connect to device before calling this")
        import serial.tools.list_ports
        if serial.__version__ < '3.5':
            raise OSError("Pyserial >= 3.5 (found {}) required for this method".format(serial.__version__))
        devices = []
        for port in serial.tools.list_ports.comports():
            if port.serial_number == self.usbtx._usbdev.serial_number.upper():
                devices.append({"port": port.device, "interface": port.location.split('.')[-1]})
        return devices

    def con(self, idProduct=[0xACE2], connect_to_first=False, serial_number=None):
        """
        Connect to device using default VID/PID
        """

        devlist = self.get_possible_devices(idProduct)

        snlist = [d['sn'] + " (" + d['product'] + ")\n" for d in devlist]
        snlist = "".join(snlist)

        if len(devlist) == 0:
            raise Warning("Failed to find USB Device")

        elif serial_number:
            dev = None
            for d in devlist:
                if d["sn"] == serial_number:
                    dev = d
                    break

            if dev is None:
                raise Warning("Failed to find USB device with S/N %s\n. Found S/N's:\n" + snlist)

        elif len(devlist) == 1:
            dev = devlist[0]

        else:
            if connect_to_first:
                dev = devlist[0]
            else:
                #User did not help us out - throw it in their face
                raise Warning("Found multiple potential USB devices. Please specify device to use. Possible S/Ns:\n" + snlist)

        self.usbseralizer.open(dev['sn'])
        foundId = dev['pid']

        if foundId in NEWAE_PIDS:
            name = NEWAE_PIDS[foundId]['name']
            fw_latest = NEWAE_PIDS[foundId]['fwver']
        else:
            name = "Unknown (PID = %04x)"%foundId
            fw_latest = [0, 0]

        self.snum = dev['sn']

        naeusb_logger.info('Found %s, Serial Number = %s' % (name, self.snum))

        fwver = self.readFwVersion()
        naeusb_logger.info('SAM3U Firmware version = %d.%d b%d' % (fwver[0], fwver[1], fwver[2]))

        latest = fwver[0] > fw_latest[0] or (fwver[0] == fw_latest[0] and fwver[1] >= fw_latest[1])
        if not latest:
            naeusb_logger.warning('Your firmware is outdated - latest is %d.%d' % (fw_latest[0], fw_latest[1]) +
                             '. Suggested to update firmware, as you may experience errors' +
                             '\nSee https://chipwhisperer.readthedocs.io/en/latest/api.html#firmware-update')
        return foundId

    def usbdev(self):
        raise AttributeError("Do Not Call Me")

    def close(self):
        """Close USB connection."""
        self.usbseralizer.close(self.snum)
        self.snum = None

    def readCDCSettings(self):
        try:
            data = self.readCtrl(self.CMD_FW_VERSION, dlen=3)
            return data
        except usb.USBError:
            return [0, 0]

    def readFwVersion(self):
        try:
            data = self.readCtrl(self.CMD_FW_VERSION, dlen=3)
            return data
        except usb.USBError:
            return [0, 0, 0]

    def sendCtrl(self, cmd, value=0, data=[]):
        """
        Send data over control endpoint
        """
        # Vendor-specific, OUT, interface control transfer
        self.usbseralizer.sendCtrl(cmd, value, data)

    def readCtrl(self, cmd, value=0, dlen=0):
        """
        Read data from control endpoint
        """
        # Vendor-specific, IN, interface control transfer
        return self.usbseralizer.readCtrl(cmd, value, dlen)

    def cmdReadMem(self, addr, dlen):
        """
        Send command to read over external memory interface from FPGA. Automatically
        decides to use control-transfer or bulk-endpoint transfer based on data length.
        """

        return self.usbseralizer.cmdReadMem(addr, dlen)

    def cmdWriteMem(self, addr, data):
        """
        Send command to write memory over external memory interface to FPGA. Automatically
        decides to use control-transfer or bulk-endpoint transfer based on data length.
        """

        return self.usbseralizer.cmdWriteMem(addr, data)

    def cmdWriteSam3U(self, addr, data):
        """
        Send command to write memory over external memory interface to FPGA. Automatically
        decides to use control-transfer or bulk-endpoint transfer based on data length.
        """

        return self.usbseralizer.cmdWriteSam3U(addr, data)

    def writeBulkEP(self, data):
        """
        Write directoly to the bulk endpoint.
        :param data: Data to be written.
        :return:
        """

        return self.usbseralizer.writeBulk(data)

    def flushInput(self):
        """Dump all the crap left over"""
        self.usbseralizer.flushInput()

    def cmdReadStream_getStatus(self):
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

    def cmdReadStream_size_of_fpgablock(self):
        """ Asks the hardware how many BYTES are read in one go from FPGA, which indicates where the sync
            bytes will be located. These sync bytes must be removed in post-processing. """
        return 4096

    def cmdReadStream_bufferSize(self, dlen):
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
        return (num_totalbytes, num_samplebytes)


    def initStreamModeCapture(self, dlen, dbuf_temp, timeout_ms=1000):
        #Enter streaming mode for requested number of samples
        if hasattr(self, "streamModeCaptureStream"):
            self.streamModeCaptureStream.join()
        self.sendCtrl(NAEUSB.CMD_MEMSTREAM, data=packuint32(dlen))
        self.streamModeCaptureStream = NAEUSB.StreamModeCaptureThread(self, dlen, dbuf_temp, timeout_ms)
        self.streamModeCaptureStream.start()

    def cmdReadStream_isDone(self):
        return self.streamModeCaptureStream.isAlive() == False

    def cmdReadStream(self):
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
        except IOError:
            pass

        # Ensure stream mode disabled
        self.sendCtrl(NAEUSB.CMD_MEMSTREAM, data=packuint32(0))

        return self.streamModeCaptureStream.drx, self.streamModeCaptureStream.timeout

    def enterBootloader(self, forreal=False):
        """Erase the SAM3U contents, forcing bootloader mode. Does not screw around."""

        if forreal:
            self.sendCtrl(0x22, 3)

    def reset(self):
        """ Reset the SAM3U. Requires firmware 0.30 or later
        """
        self.sendCtrl(0x22, 0x10)

    def read(self, dlen, timeout=2000):
        self.usbserializer.read(dlen, timeout)

    class StreamModeCaptureThread(Thread):
        def __init__(self, serial, dlen, dbuf_temp, timeout_ms=2000):
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
                self.drx = self.serial.usbtx.read(self.dbuf_temp, timeout=self.timeout_ms)
            except IOError as e:
                naeusb_logger.warning('Streaming: USB stream read timed out')
            diff = time.time() - start
            naeusb_logger.info("Streaming: Received %d bytes in time %.20f)" % (self.drx, diff))


if __name__ == '__main__':
    from chipwhisperer.hardware.naeusb.fpga import FPGA
    from chipwhisperer.hardware.naeusb.programmer_avr import AVRISP
    from chipwhisperer.hardware.naeusb.programmer_xmega import XMEGAPDI, supported_xmega
    from chipwhisperer.hardware.naeusb.serial import USART

    cwtestusb = NAEUSB()
    cwtestusb.con()

    #Connect required modules up here
    fpga = FPGA(cwtestusb)
    xmega = XMEGAPDI(cwtestusb)
    avr = AVRISP(cwtestusb)
    usart = USART(cwtestusb)

    force = True
    if fpga.isFPGAProgrammed() == False or force:
        from datetime import datetime
        starttime = datetime.now()
        fpga.FPGAProgram(open(r"C:\E\Documents\academic\sidechannel\chipwhisperer\hardware\capture\chipwhisperer-lite\hdl\cwlite_ise\cwlite_interface.bit", "rb"))
        # fpga.FPGAProgram(open(r"C:\Users\colin\dropbox\engineering\git_repos\CW305_ArtixTarget\temp\artix7test\artix7test.runs\impl_1\cw305_top.bit", "rb"))
        # fpga.FPGAProgram(open(r"C:\E\Documents\academic\sidechannel\chipwhisperer\hardware\api\chipwhisperer-lite\hdl\cwlite_ise_spifake\cwlite_interface.bit", "rb"))
        stoptime = datetime.now()
        print("FPGA Config time: %s" % str(stoptime - starttime))

    # print fpga.cmdReadMem(10, 6)
    # print fpga.cmdReadMem(0x1A, 4)
    # fpga.cmdWriteMem(0x1A, [235, 126, 5, 4])
    # print fpga.cmdReadMem(0x1A, 4)

    avrprogram = False
    if avrprogram:
        avr.enableISP(True)
        avr.enableISP(False)

    xmegaprogram = True
    if xmegaprogram:
        xmega.setChip(supported_xmega[0])
        # Worst-case is 75mS for chip erase, so give us some head-room
        xmega.setParamTimeout(200)

        try:
            print("Enable")
            xmega.enablePDI(True)

            print("Read sig")
            # Read signature bytes
            data = xmega.readMemory(0x01000090, 3, "signature")

            print(data)

            if data[0] != 0x1E or data[1] != 0x97 or data[2] != 0x46:
                print("Signature bytes failed: %02x %02x %02x != 1E 97 46" % (data[0], data[1], data[2]))
            else:
                print("Detected XMEGA128A4U")

            print("Erasing")
            # Chip erase
            try:
                xmega.eraseChip()
            except IOError:
                xmega.enablePDI(False)
                xmega.enablePDI(True)

            fakedata = [i & 0xff for i in range(0, 2048)]
            print("Programming FLASH Memory")
            xmega.writeMemory(0x0800000, fakedata, memname="flash")

            print("Verifying")
            test = xmega.readMemory(0x0800000, 512)

            print(test)


        except TypeError as e:
            print(str(e))

        except IOError as e:
            print(str(e))

        xmega.enablePDI(False)

    print("Let's Rock and Roll baby")

    sertest = True

    if sertest:
        usart.init()
        usart.write("hello\n")
        time.sleep(0.1)
        print(usart.read())

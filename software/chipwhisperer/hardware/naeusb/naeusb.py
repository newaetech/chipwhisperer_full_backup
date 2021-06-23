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
import time
import math
from threading import Thread
import usb1
import array

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

class NAEUSB_Backend:
    """
    Backend to talk to the USB device. TODO: Need to make one for pyusb as people might still require libusb0
    """

    CMD_READMEM_BULK = 0x10
    CMD_WRITEMEM_BULK = 0x11
    CMD_READMEM_CTRL = 0x12
    CMD_WRITEMEM_CTRL = 0x13
    CMD_MEMSTREAM = 0x14

    def __init__(self):
        self._usbdev = None
        self._timeout = 500
        self.usb_ctx = usb1.USBContext()
        self.handle = None

    def __del__(self):
        if self.handle:
            self.handle.close()
            self.handle = None
        if self.usb_ctx:
            self.usb_ctx.exit()
            self.usb_ctx = None

    def usbdev(self):
        """Safely get USB device, throwing error if not connected"""

        if not self._usbdev: raise OSError("USB Device not found. Did you connect it first?")
        return self._usbdev

    def is_accessable(self, dev):
        try:
            dev.getSerialNumber()
            return True
        except:
            return False

    def find(self, serial_number=None, idProduct=None):
        # check if we got anything
        dev_list = self.get_possible_devices(idProduct)
        if len(dev_list) == 0:
            raise OSError("Could not find ChipWhisperer. Is it connected?")

        # if more than one CW, we require a serial number
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


    def open(self, serial_number=None, idProduct=None, connect_to_first=False):
        """
        Connect to device using default VID/PID
        """

        self.device = self.find(serial_number, idProduct)
        if connect_to_first == False:
            return
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
        self._timeout = 200

        return self.handle

    def close(self):
        """Close the USB connection"""
        try:
            self.handle.close()
            self.handle = None
            self.usb_ctx.exit()
            self.usb_ctx = None
        except:
            logging.info('USB Failure calling dispose_resources: %s' % str(e))

    def get_possible_devices(self, idProduct=None, dictonly=True):
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
        if not (idProduct is None):
            dev_list = [dev for dev in dev_list if dev.getProductID() in idProduct]

        naeusb_logger.info("Found NAEUSB devices {}".format(dev_list))
        
        if len(dev_list) == 0:
            return []

        for dev in dev_list:
            try:
                a = dev.getSerialNumber()
                naeusb_logger.info("Found ChipWhisperer with serial number {}".format(a))
            except:
                naeusb_logger.info("Attempt to access ChipWhisperer failed, skipping to next")
                dev_list.remove(dev)

        if len(dev_list) == 0:
            raise OSError("Unable to communicate with found ChipWhisperer. Check that \
                \nanother process isn't connected to it and that you have permission to communicate with it.")

        return dev_list

    def sendCtrl(self, cmd, value=0, data=[]):
        """
        Send data over control endpoint
        """
        # Vendor-specific, OUT, interface control transfer
        self.handle.controlWrite(0x41, cmd, value, 0, data, timeout=self._timeout)
        naeusb_logger.debug("WRITE_CTRL: bmRequestType: {:02X}, \
                    bRequest: {:02X}, wValue: {:04X}, wIndex: {:04X}, data: {}".format(0x41, cmd, \
                        value, 0, data))
        #return self.usbdev().ctrl_transfer(0x41, cmd, value, 0, data, timeout=self._timeout)

    def readCtrl(self, cmd, value=0, dlen=0):
        """
        Read data from control endpoint
        """
        # Vendor-specific, IN, interface control transfer
        response = self.handle.controlRead(0xC1, cmd, value, 0, dlen, timeout=self._timeout)
        naeusb_logger.debug("READ_CTRL: bmRequestType: {:02X}, \
                    bRequest: {:02X}, wValue: {:04X}, wIndex: {:04X}, data_len: {:04X}, response: {}".format(0xC1, cmd, \
                        value, 0, dlen, response))
        return response


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
        if cmd == self.CMD_WRITEMEM_BULK:
            data = self.handle.bulkWrite(self.wep, data, timeout=self._timeout)
        else:
            #logging.warning("Write ignored")

            pass

        naeusb_logger.debug("FPGA_WRITE: bulk: {}, addr: {:08X}, dlen: {:08X}, response: {}"\
            .format("yes" if dlen >= 48 else "no", addr, dlen, data))

        return data

    def cmdWriteBulk(self, data):
        """
        Write data directly to the bulk endpoint.
        :param data: Data to be written
        :return:
        """
        self.handle.bulkWrite(self.wep, data, timeout=self._timeout)
        naeusb_logger.debug("BULK WRITE: data = {}".format(data))

    writeBulk = cmdWriteBulk

    def flushInput(self):
        """Dump all the crap left over"""
        try:
            # TODO: This probably isn't needed, and causes slow-downs on Mac OS X.
            self.handle.bulkRead(self.rep, 1000, timeout=0.010)
        except:
            pass

    def read(self, dbuf, timeout):
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

    stream = False

    # TODO: make this better
    fwversion_latest = [0, 11]
    def __init__(self):
        self._usbdev = None
        self.handle=None
        self.usbtx = NAEUSB_Backend()
        self.usbseralizer = self.usbtx

    def get_possible_devices(self, idProduct):
        return self.usbtx.get_possible_devices(idProduct)

    def get_cdc_settings(self):
        return self.usbtx.readCtrl(self.CMD_CDC_SETTINGS_EN, dlen=2)

    def set_cdc_settings(self, port=[1, 1]):
        if isinstance(port, int):
            port = [port, port]
        self.usbtx.sendCtrl(self.CMD_CDC_SETTINGS_EN, (port[0]) | (port[1] << 1))

    def set_smc_speed(self, val):
        """
        val = 0: normal read timing
        val = 1: fast read timing, should only be used for reading ADC samples; FPGA must also be set in fast FIFO
                 read mode for this to work correctly.
        """
        self.usbtx.sendCtrl(self.CMD_SMC_READ_SPEED, data=[val])

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
        self.usbtx.open(idProduct=idProduct, serial_number=serial_number, connect_to_first=True)


        self.snum=self.usbtx.sn
        fwver = self.readFwVersion()
        naeusb_logger.info('SAM3U Firmware version = %d.%d b%d' % (fwver[0], fwver[1], fwver[2]))

        if self.usbtx.pid in NEWAE_PIDS:
            name = NEWAE_PIDS[self.usbtx.pid]['name']
            fw_latest = NEWAE_PIDS[self.usbtx.pid]['fwver']
        else:
            name = "Unknown (PID = %04x)"%self.usbtx.pid
            fw_latest = [0, 0]

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
            bytes will be located. These sync bytes must be removed in post-processing. CW-pro only. """
        return 4096

    def cmdReadStream_bufferSize(self, dlen):
        """
        Args:
            dlen: Number of samples to be requested (will be rounded to something else)
        Returns:
            Tuple: (Size of temporary buffer required, actual samples in buffer)
        """
        # TODO: previous husky branch code:
        #if is_husky:
        #    if bits_per_sample == 8:
        #        num_totalbytes = dlen
        #        num_samplebytes = dlen
        #    elif bits_per_sample == 12:
        #        # TODO-husky check these are correct
        #        num_totalbytes = int(np.ceil(dlen*1.5))
        #        num_samplebytes = int(np.ceil(dlen*1.5))
        #    else:
        #        raise ValueError("bits_per_sample=%d" % bits_per_sample)
        #else:
        #    num_samplebytes = int(math.ceil(float(dlen) * 4 / 3))
        #    num_blocks = int(math.ceil(float(num_samplebytes) / 4096))
        #    num_totalbytes = num_samplebytes + num_blocks
        #    num_totalbytes = int(math.ceil(float(num_totalbytes) / 4096) * 4096)
        num_samplebytes = int(math.ceil(float(dlen) * 4 / 3))
        num_blocks = int(math.ceil(float(num_samplebytes) / 4096))
        num_totalbytes = num_samplebytes + num_blocks
        num_totalbytes = int(math.ceil(float(num_totalbytes) / 4096) * 4096)
        return (num_totalbytes, num_samplebytes)


    def initStreamModeCapture(self, dlen, dbuf_temp, timeout_ms=1000, is_husky=False, segment_size=0):
        #Enter streaming mode for requested number of samples
        if hasattr(self, "streamModeCaptureStream"):
            self.streamModeCaptureStream.join()
        if is_husky:
            data=list(int.to_bytes(segment_size, length=4, byteorder='little')) + \
                list(int.to_bytes(3, length=4, byteorder='little')) + list(int.to_bytes(dlen, length=4, byteorder="little"))
        else:
            data = packuint32(dlen)
        self.sendCtrl(NAEUSB.CMD_MEMSTREAM, data=data)
        self.streamModeCaptureStream = NAEUSB.StreamModeCaptureThread(self, dlen, segment_size, dbuf_temp, timeout_ms, is_husky)
        self.streamModeCaptureStream.start()

    def cmdReadStream_isDone(self):
        return self.streamModeCaptureStream.drx >= self.streamModeCaptureStream.dlen

    def cmdReadStream(self, is_husky=False):
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
        # self.sendCtrl(NAEUSB.CMD_MEMSTREAM, data=packuint32(0))
        return self.streamModeCaptureStream.drx, self.streamModeCaptureStream.timeout

    def readCDCSettings(self):
        try:
            data = self.readCtrl(self.CMD_FW_VERSION, dlen=3)
            return data
        except usb.USBError:
            return [0, 0]

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
        def __init__(self, serial, dlen, segment_size, dbuf_temp, timeout_ms=2000, is_husky=False):
            """
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
                            transfer.cancel()
                except usb1.USBErrorInterrupted:
                    pass
            naeusb_logger.info("Streaming: Received %d bytes in time %.20f)" % (self.drx, diff))

        def callback(self, transfer):
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

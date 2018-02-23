#  Java host software API of ZTEX EZ-USB FX2 SDK
#  Python version based on code that is:
#  Copyright (C) 2009-2011 ZTEX GmbH.
#  http://www.ztex.de
#
#  Python conversion done via java2python + Colin O'Flynn's fixes
#  this version is VERY limited compared to the full Java version.
#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License version 3 as
#  published by the Free Software Foundation.
#  This program is distributed in the hope that it will be useful, but
#  WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
#  General Public License for more details.
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, see http://www.gnu.org/licenses/.
import logging

import usb
import time

cypressVendorId = 0x4b4

#  * EZ-USB USB product ID: 0x8613
cypressProductId = 0x8613

#  * ZTEX vendor ID: 0x221a
ztexVendorId = 0x221A

#
#      * USB product ID for ZTEX devices that support ZTEX descriptor 1: 0x100.
#      * This product ID is intended for general purpose use and can be shared by all devices that base on ZTEX modules.
#      * Different products are identified by a second product ID, namely the PRODUCT_ID field of the <a href="#descriptor"> ZTEX descriptor 1</a>.
#      * <p>
#      * Please read the <a href="http://www.ztex.de/firmware-kit/usb_ids.e.html">informations about USB vendor and product ID's<a>.
#      * @see #ztexProductIdMax
#
ztexProductId = 0x100


class EzUsb(object):
    """ generated source for class EzUsb """
    #  ******* reset **************************************************************
    #
    #      * Controls the reset state of a Cypress EZ-USB device.
    #      * @param handle The handle of the device.
    #      * @param r The reset state (true means reset).
    #      * @throws FirmwareUploadException if an error occurred while attempting to control the reset state.
    #

    def __init__(self, dev):
        self.dev = dev

    def reset(self, r):
        """ generated source for method reset """
        buffer_ = [int((1 if r else 0))]

        k = self.dev.ctrl_transfer(0x40, 0xA0, 0xE600, 0, buffer_, 1000)

        #  upload j bytes
        if k < 0:
            raise IOError("unable to set reset=" + buffer_[0])
        elif k != 1:
            raise IOError("Unable to set reset=" + buffer_[0])

        # Give some recovery time
        time.sleep(0.050 if r else 0.400)

    #  ******* uploadFirmware ******************************************************
    #
    #      * Uploads the Firmware to a Cypress EZ-USB device.
    #      * @param handle The handle of the device.
    #      * @param ihxFile The firmware image.
    #      * @return the upload time in ms.
    #      * @throws FirmwareUploadException if an error occurred while attempting to upload the firmware.
    #
    def uploadFirmware(self, ihxFile):
        """ generated source for method uploadFirmware """
        transactionBytes = 256
        buffer_ = [None] * transactionBytes
        self.reset(True)

        #  reset = 1
        # t0 = Date().getTime()
        j = 0
        i = 0
        for i in range(0, len(ihxFile.ihxData) + 1):
            if i >= len(ihxFile.ihxData) or ihxFile.ihxData[i] < 0 or j >= transactionBytes:
                if j > 0:
                    #  upload j bytes
                    k = self.dev.ctrl_transfer(0x40, 0xA0, i - j, 0, buffer_[0:j], 1000)
                    if k < 0:
                        raise IOError("Unknown USB Error #1?")
                    elif k != j:
                        raise IOError("Unknown USB Error #2?")

                    time.sleep(0.001)
                j = 0

            if i < len(ihxFile.ihxData) and ihxFile.ihxData[i] >= 0 and ihxFile.ihxData[i] <= 255:
                buffer_[j] = int(ihxFile.ihxData[i])
                j += 1
            i += 1
        # t1 = Date().getTime()
        try:
            self.reset(False)
            #  error (may caused re-numeration) can be ignored
        except IOError:
            pass
        # return t1 - t0
        return 0


class IhxFile(object):
    """ generated source for class IhxFile """
    #
    #      * This array stores the firmware image.
    #      * Values &lt;0 and &gt;255 mean that the data is undefined.
    #
    ihxData = [None] * 65536

    #  ******* readHexDigit ********************************************************
    @classmethod
    def readHexDigit(self, in_):
        """ generated source for method readHexDigit """
        b = in_.read(1)
        if len(b) == 0:
            raise ValueError("Inexpected end of file")
        b = ord(b)
        if b >= ord('0') and b <= ord('9'):
            return b - ord('0')
        if b >= ord('a') and b <= ord('f'):
            return 10 + b - ord('a')
        if b >= ord('A') and b <= ord('F'):
            return 10 + b - ord('A')
        raise ValueError("Hex digit expected: " + chr(b))

    #  ******* readHexByte *********************************************************
    def readHexByte(self, in_):
        """ generated source for method readHexByte """
        data = (self.readHexDigit(in_) << 4) | self.readHexDigit(in_)
        # print "%x" % data
        return data

    #  ******* IhxFile *************************************************************
    #
    #      * Constructs an instance from a given file name.
    #      * This method can also read system resources, e.g. files from the current jar archive.
    #      * @param fileName The file name.
    #      * @throws IOException If an read error occurred.
    #      * @throws IhxFileDamagedException If the ihx file is damaged.
    #
    def __init__(self, fname):
        """ generated source for method __init__ """

        #Allow usage of both filelike type and filenames
        if hasattr(fname, "read"):
            in_ = fname
        else:
            in_ = open(fname, "r")
        b = None
        lenfield = int()
        cs = int()
        addr = int()
        buf = [None] * 255
        eof = False
        line = 0

        for i in range(0, len(self.ihxData)):
            self.ihxData[i] = -1

        try:
            while not eof:
                while b != ':':
                    b = in_.read(1)
                    if len(b) == 0: raise ValueError("Unexpected end of file")

                line += 1
                lenfield = self.readHexByte(in_)
                #  length field
                cs = lenfield
                b = self.readHexByte(in_)
                #  address field
                cs += b
                addr = b << 8
                b = self.readHexByte(in_)
                cs += b
                addr |= b
                b = self.readHexByte(in_)
                #  record type field
                cs += b
                for i in range(0, lenfield):
                    #  data
                    buf[i] = int(self.readHexByte(in_))
                    cs += buf[i]
                cs += self.readHexByte(in_)
                #  checksum

                if (cs & 0xff) != 0:
                    raise ValueError("Checksum error")
                if b == 0:
                    #  data record
                    for i in range(0, lenfield):
                        if self.ihxData[addr + i] >= 0:
                            logging.warning('Memory at position ' + "%x" % i + " overwritten")
                        self.ihxData[addr + i] = int((buf[i] & 255))
                elif b == 1:
                    #  eof record
                    eof = True
                else:
                    raise ValueError("Invalid record type: " + str(b))
        except ValueError as e:
            # raise IhxFileDamagedException(fileName, line, e.getLocalizedMessage())
            raise ValueError("Some Problem with decoding IHX file :(: %s" % str(e))
        try:
            in_.close()
        except Exception as e:
            logging.error('Error closing file ' + fname + ": " + str(e))

    #  ******* dataInfo ************************************************************
    #
    #      * Print out some information about the memory usage.
    #      * @param out Where the data is printed out.
    #
    def dataInfo(self, out):
        """ generated source for method dataInfo """
        addr = -1
        i = 0
        while i <= 65536:
            #  data
            if (i == 65536 or self.ihxData[i] < 0) and addr >= 0:
                logging.info(i - addr + " Bytes from " + "%x" % addr + " to " + "%x" % (i - 1))
                addr = -1
            if i < 65536 and self.ihxData[i] >= 0 and addr < 0:
                addr = i
            i += 1


class Ztex1v1(object):

    CAPABILITY_FPGA = 1

    def __init__(self):
        self.dev = None
        self.firmwareProgrammed = False
        self.fpgaProgrammed = False
        self.deviceInfo = None

    def checkCapability(self, i):
        if self.deviceInfo:
            if self.deviceInfo["interfaceCapabilities"][i // 8] & (1 << (i % 8)) != 0:
                return True

        raise IOError("ZTEX: Device does not have required capability: %d" % i)
        # return False

    def vendorRequest2(self, cmd, name="Unknown", value=0, index=0, maxlen=0):
        data = self.dev.ctrl_transfer(0xc0, cmd, value, index, maxlen, 500)
        if len(data) != maxlen:
            raise IOError("ZTEX USB: %s: Expected %d bytes, received %d" % (name, maxlen, len(data)))
        return data

    def vendorCommand(self, cmd, name="Unknown", value=0, index=0, data=[]):
        self.dev.ctrl_transfer(0x40, cmd, value, index, data, 200)

    def getFpgaState(self):
        """ generated source for method getFpgaState """
        self.checkCapability(self.CAPABILITY_FPGA)
        buffer_ = self.vendorRequest2(0x30, "getFpgaState", maxlen=9)
        self.fpgaConfigured = buffer_[0] == 0
        self.fpgaChecksum = buffer_[1] & 0xff
        self.fpgaBytes = ((buffer_[5] & 0xff) << 24) | ((buffer_[4] & 0xff) << 16) | ((buffer_[3] & 0xff) << 8) | (buffer_[2] & 0xff)
        self.fpgaInitB = buffer_[6] & 0xff
        self.fpgaFlashResult = buffer_[7]
        self.fpgaFlashBitSwap = buffer_[8] != 0


    def probe(self, verbose=False):
        self.dev = None
        self.firmwareProgrammed = False
        self.fpgaProgrammed = False
        self.deviceInfo = None

        # Find unprogrammed chips
        dev = usb.core.find(idVendor=cypressVendorId, idProduct=cypressProductId)
        if dev:
            self.dev = dev
            self.firmwareProgrammed = False
            if verbose: logging.info('Found unprogrammed device')

        # Find programmed chips
        dev = usb.core.find(idVendor=ztexVendorId, idProduct=ztexProductId)
        if dev:
            self.dev = dev
            self.firmwareProgrammed = True
            if verbose: logging.info('Found programmed device')

            # Probe the device to determine additional information
            buf = self.dev.ctrl_transfer(0xc0, 0x22, 0, 0, 40, 500)

            self.deviceInfo = {
                               'productId':list(buf[6:10]),
                               'fwVersion':buf[10],
                               'interfaceVersion':buf[11],
                               'interfaceCapabilities':list(buf[12:18]),
                               'moduleReserved': list(buf[18:30])}

        if not self.dev:
            raise Warning("Failed to find USB Device")

    def resetFpga(self):
        """ generated source for method resetFpga """
        self.checkCapability(self.CAPABILITY_FPGA)
        self.vendorCommand(0x31, "resetFpga")

    def configureFpgaLS(self, fwFileLike, force=False, bs=-1):
        """ generated source for method configureFpgaLS """
        transactionBytes = 2048  # 256 if certainWorkarounds else 2048
        t0 = 0
        self.checkCapability(self.CAPABILITY_FPGA)
        if self.fpgaConfigured:
            raise IOError("Attempting to configure FPGA when already configured...")

        buffer_ = [None] * (16 * 1024 * 1024 / transactionBytes)
        size = 0
        try:
            # Read entire thing in
            #with fwFileLike as f: inputStream = f.read()
            inputStream = fwFileLike.read()
            fwFileLike.close()
            streamCnt = 0

            j = transactionBytes
            for i in range(0, len(buffer_)):
                if j != transactionBytes: break
                buffer_[i] = bytearray(inputStream[streamCnt:(streamCnt + transactionBytes)])
                streamCnt += transactionBytes

                j = len(buffer_[i])

                if j < transactionBytes and j % 64 == 0:
                    j += 1
                size += j

                # if j < transactionBytes and j % 64 == 0:
                #    j += 1
                # size += j

        except IOError as e:
            # raise BitstreamReadException(e.getLocalizedMessage())
            logging.error('Shit - IO Error in configureFpgaLS')

        if size < 64 or size % 64 == 0:
            raise ValueError("Invalid file size: " + str(size))
        if bs < 0 or bs > 1:
            bs = self.detectBitstreamBitOrder(buffer_[0])
        if bs == 1:
            self.swapBits(buffer_, size)
        tries = 10

        while tries > 0:
            self.resetFpga()
            try:
                # t0 = -Date().getTime()
                bs = 0
                cs = 0

                for i in range(0, len(buffer_)):
                    if  i * transactionBytes >= size:
                        break
                    j = size - i * transactionBytes
                    if j > transactionBytes:
                        j = transactionBytes
                    self.vendorCommand(0x32, "sendFpgaData", 0, 0, buffer_[i])

                    bs += j
                    for k in range(0, len(buffer_[i])):
                        cs = (cs + (buffer_[i][k] & 0xff)) & 0xff

                self.getFpgaState()
                if not self.fpgaConfigured:
                    raise IOError("FPGA configuration failed: DONE pin does not go high (size=" + self.fpgaBytes + " ,  " + (bs - self.fpgaBytes) + " bytes got lost;  checksum=" + self.fpgaChecksum + " , should be " + cs + ";  INIT_B_HIST=" + self.fpgaInitB + ")")

                #if self.enableExtraFpgaConfigurationChecks:
                #    if self.fpgaBytes != 0 and self.fpgaBytes != bs:
                #        System.err.println("Warning: Possible FPGA configuration data loss: " + (bs - self.fpgaBytes) + " bytes got lost")
                #    if self.fpgaInitB != 222:
                #        System.err.println("Warning: Possible Bitstream CRC error: INIT_B_HIST=" + self.fpgaInitB)
                tries = 0
                #t0 += Date().getTime()
            except IOError as e:
                if tries > 1:
                    logging.warning(str(e) + ": Retrying it ...")
                else:
                    raise
            tries -= 1
        time.sleep(0.1)
        return t0

    def detectBitstreamBitOrder(self, buf):
        """ generated source for method detectBitstreamBitOrder """
        i = 0
        while i < len(buf):
            if ((buf[i] & 255) == 0xaa) and ((buf[i + 1] & 255) == 0x99) and ((buf[i + 2] & 255) == 0x55) and ((buf[i + 3] & 255) == 0x66):
                return 1
            if ((buf[i] & 255) == 0x55) and ((buf[i + 1] & 255) == 0x99) and ((buf[i + 2] & 255) == 0xaa) and ((buf[i + 3] & 255) == 0x66):
                return 0
            i += 1
        logging.warning('Unable to determine bitstream bit order: no signature found')
        return 0

    #  ******* swapBits ************************************************************
    def swapBits(self, buf, size):
        """ generated source for method swapBits """
        j = 0
        k = 0
        i = 0
        for i in range(0, size):
            while k >= len(buf[j]):
                j += 1
                k = 0
            b = buf[j][k]
            buf[j][k] = int((((b & 128) >> 7) | ((b & 64) >> 5) | ((b & 32) >> 3) | ((b & 16) >> 1) | ((b & 8) << 1) | ((b & 4) << 3) | ((b & 2) << 5) | ((b & 1) << 7)))
            k += 1

    def uploadFirmware(self, ihxFile):
        """ generated source for method uploadFirmware """
        ez = EzUsb(self.dev)
        time = ez.uploadFirmware(ihxFile)
        return time


if __name__ == '__main__':
    ztex = Ztex1v1()
    ztex.probe(True)

    if ztex.firmwareProgrammed == False:
        f = IhxFile("OpenADC.ihx")
        ztex.uploadFirmware(f)
        time.sleep(1)
        ztex.probe()
    else:
        print "ZTEX Firmware upload skipped"

    if ztex.deviceInfo["interfaceVersion"] != 1:
        raise IOError("Unknown interface version, invalid ZTEX Firmware?. Device info: %s" % str(ztex.deviceInfo))

    if ztex.deviceInfo["productId"] != [10, 12, 0, 0]:
        raise IOError("Unknown productId, invalid ZTEX Firmware/Module?. Device info: %s" % str(ztex.deviceInfo))

    ztex.getFpgaState()
    if ztex.fpgaConfigured == False:
        print "Programming FPGA..."
        ztex.configureFpgaLS(open(r"C:\E\Documents\academic\sidechannel\chipwhisperer\hardware\capture\chipwhisperer-rev2\hdl\ztex_rev2_1.11c_ise\interface.bit", "rb"))

    else:
        print "FPGA Configuration skipped"

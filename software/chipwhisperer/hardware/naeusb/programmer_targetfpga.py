#
# Copyright (c) 2022, NewAE Technology Inc
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
#==========================================================================

from typing import cast
import time, os, logging, io
from ...logging import *
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

class XilinxGeneric(FPGASlaveSPI):

    def __init__(self, scope):
        super().__init__(scope, cs_line=None)

    def done_setup(self):
        raise NotImplementedError
    
    def done_state(self):
        raise NotImplementedError

    def initb_setup(self):
        raise NotImplementedError

    def initb_state(self):
        #Return "True" if not implemented to bypass logic
        raise NotImplementedError

    def erase_and_init(self):

        #Done pin High-Z
        self.done_setup()

        #init-b High-Z
        self.initb_setup()

        #Pulse PGM low then high
        setattr(self.scope.io, self.pgm, False)
        time.sleep(0.01)
        setattr(self.scope.io, self.pgm, True)

        #Check done pin status now
        time.sleep(0.05)
        done = self.done_state()

        #check initb is high if using
        initb = self.initb_state()

        if done == True:
            raise IOError("Erase Error: DONE pin should be low, reads high, abort")

        if initb == False:
            raise IOError("Erase Error: INITB should be HIGH, reads LOW, abort!")

    def program(self, bs_path_or_data_or_iobytes, sck_speed=1E6, use_fast_usb=True):
        """Program bitstream file. By default uses a faster mode, you can set
           `sck_speed` up to around 20E6 successfully."""

        if type(bs_path_or_data_or_iobytes) is io.BytesIO:
            bs_path = None
            bs_data = None
            bs_iobytes = bs_path_or_data_or_iobytes
        # No-body has 5K path lengths right !?
        elif len(bs_path_or_data_or_iobytes) < 5E3:
            bs_path = bs_path_or_data_or_iobytes
            bs_data = None
            bs_iobytes = None
        else:
            bs_path = None
            bs_data = bs_path_or_data_or_iobytes
            bs_iobytes = None
            if use_fast_usb:
                logging.info("Falling back to 'slow' USB when data is passed")

            if bs_path:
                filestats = os.stat(bs_path)
                modtime = time.ctime(filestats.st_mtime)

                target_logger.info("Bitstream modified time : ", modtime )

        #Need to take control of ISP lines for erase to work
        util.chipwhisperer_extra.cwEXTRA.setAVRISPMode(False)

        self.erase_and_init()

        if use_fast_usb and (bs_path or bs_iobytes):
            if bs_path:
                bsfile = open(bs_path, "rb")
            else:
                bsfile = bs_iobytes
            try:
                fastfpga = FPGA(self.scope._getNAEUSB(), prog_mask=0xB0)
                util.chipwhisperer_extra.cwEXTRA.setAVRISPMode(True)

                fastfpga.FPGAProgram(bsfile, prog_speed=sck_speed, starting_offset=0)
            
            finally:
                util.chipwhisperer_extra.cwEXTRA.setAVRISPMode(False)
                bsfile.close()
        else:
            if bs_iobytes:
                data = bs_iobytes.read()
                bs_iobytes.close()
            elif bs_path:
                bsfile = open(bs_path, "rb")
                data = bsfile.read()
                bsfile.close()
            else:
                data = bs_data
           
            try:
                spi = SPI(self.scope._getNAEUSB())
                spi.enable(sck_speed)
                spi.transfer(data, writeonly=True)
                spi.transfer([0xff]*256, writeonly=True)
            finally:
                spi.disable()

        done = self.done_state()

        if done != True:
            #raise IOError("FPGA error: DONE pin did not go high.")
            # TEMPORARY: DONE pin may not be seen high because it's shared with IO4. Target still appears to work so let's just warn for now.
            target_logger.warning("FPGA warning: DONE pin did not go high.")

class CW312T_XC7A35T(XilinxGeneric):
    """Xilinx XC7A35T Programmer for CW312T Target.

    Programmer for CW312T-XC7A35T target:
     * Assumes done is 'PDID' (default, but R22/R29 can change that to TIO pin instead)
     * Assumes program is 'PDIC'

    Note - suggested to set HS2 to 'none' during programming, as seems to cause programming to be less reliable.

    Example::
        from chipwhisperer.hardware.naeusb.programmer_targetfpga import CW312T_XC7A35T
        fpga = CW312T_XC7A35T(scope)
        scope.io.hs2 = None
        fpga.program('top_cw312a35.bit', sck_speed=10e6)
        scope.io.hs2 = "clkgen"
    """
    def __init__(self, scope):
        super().__init__(scope)
        self.pgm = "pdic"

    def done_setup(self):
        self.scope.io.pdid = None
    
    def done_state(self):
        return self.scope.io.pdid_state

    def initb_setup(self):
        self.scope.io.tio3 = None

    def initb_state(self):
        return self.scope.io.tio_states[2]


class LatticeICE40(FPGASlaveSPI):
    """Lattice ICE40 SPI programming algorithm for directly programming CRAM.
    
    Physical connections (ChipWhisperer <-> ICE40)
     * SCK/MOSI to SCK/SI (optional - MISO/SO as well)
     * nRST to CRST
     * PDIC to CDONE (optional - read not supported on CW-Lite/CW-1200)
     * PDID to CS

    Example::

        import chipwhisperer as cw
        from chipwhisperer.hardware.naeusb.programmer_targetfpga import LatticeICE40

        # Connect to scope
        scope = cw.scope()

        lattice = LatticeICE40(scope)

        lattice.erase_and_init()
        lattice.program("example_file.bin"", sck_speed=20E6)
    
    WARNING: Current the `CDONE` output is not checked!
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

    def program(self, bs_path_or_data_or_iobytes, sck_speed=1E6, use_fast_usb=True, start=True):
        """Program bitstream file. By default uses a faster mode, you can set
           `sck_speed` up to around 20E6 successfully."""

        if type(bs_path_or_data_or_iobytes) is io.BytesIO:
            bs_path = None
            bs_data = None
            bs_iobytes = bs_path_or_data_or_iobytes
        # No-body has 5K path lengths right !?
        elif len(bs_path_or_data_or_iobytes) < 5E3:
            bs_path = bs_path_or_data_or_iobytes
            bs_data = None
            bs_iobytes = None
        else:
            bs_path = None
            bs_data = bs_path_or_data_or_iobytes
            bs_iobytes = None

            if use_fast_usb:
                logging.info("Falling back to 'slow' USB when data is passed")
        
        if bs_path:
            filestats = os.stat(bs_path)
            modtime = time.ctime(filestats.st_mtime)

            target_logger.info("Bitstream modified time : ", modtime )

        #Need to take control of ISP lines for erase to work
        util.chipwhisperer_extra.cwEXTRA.setAVRISPMode(False)

        self.erase_and_init()

        if start is False:
            #Only supported with slow SPI
            use_fast_usb = False

        if use_fast_usb and (bs_path or bs_iobytes):
            if bs_path:
                bsfile = open(bs_path, "rb")
            else:
                bsfile = bs_iobytes
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
            if bs_iobytes:
                data = bs_iobytes.read()
                bs_iobytes.close()
            elif bs_path:
                bsfile = open(bs_path, "rb")
                data = bsfile.read()
                bsfile.close()
            else:
                data = bs_data

            if start is False:
                if list(data[-3:]) != [1, 6, 0]:
                    #Check our crappy hack works, force user to change it
                    raise IOError("Expected bitstream to end with 0x01 0x06 0x00, got: %s"%data[-3:])
                data = data[:-3]

            try:
                spi = SPI(self.scope._getNAEUSB())
                spi.enable(sck_speed)
                spi.transfer(data, writeonly=True)
                if start is True:
                    spi.transfer([0]*256, writeonly=True)
            finally:
                if start is True:
                    spi.disable()
        
        #PDIC doesn't support read, oops!
        #cdone = getattr(self.scope.io, self.cdone)
    
    def read_block(self, spi, blocksize, cmd, shiftamount=5):
        """Read a block, used with readback mode"""

        HUGE_HACK = [0x00]
        res = spi.transfer(cmd + HUGE_HACK) # Should end 0xF8, 0x00
        if bytes(res[-3:-1]) != b'\xf8\x00': # Change to [-2:] if HUGE_HACK removed
            raise IOError("Sync Error, expected F8 00, got %02x %02x with %s %s"%(res[-3], res[-1], blocksize, cmd))
            
        # These 5 toggles need to be with previous transaction or things seem to mess up.
        # As a MAJOR HACK we sent an extra 8 cycles with previous transaction. This means
        # everything is now off as 3 bits got read already. Options are toggle 5 bits now
        # and throw away that first byte, or fix it after. Right now we fix it after. Also seems
        # the final CRAM needs only 4 instead of 5? Not really investigated... a later problem
        #spi.toggle_sck(5)
        
        # lattice puts a 1ms delay here it seems, not sure if needed
        time.sleep(0.001)
        
        #Top 3 bits of res[-1] valid only
        data = [res[-1]] + spi.transfer([0x00]* (blocksize) ) 
        
        #Now bit-shift an ENTIRE ARRAY (this is horrible), throw away last element
        data = [(((data[i] << shiftamount) & 0xff) | (data[i+1] >> (8-shiftamount))) for i in range(0, len(data)-1)]
        
        return data

    def readback(self, sck_speed=1E6, device="iCE40UP5K"):
        """Readback CRAM + BRAM data. Must have NOT started FPGA for this to work.

        Example::
            lattice = LatticeICE40(scope)

            lattice.erase_and_init()
            lattice.program("example_file.bin"", sck_speed=20E6, start=False)
            cram, bram = lattice.readback()
            lattice.start()
        """
        # The following is done WITHOUT setting cs low - leave CS high
        setattr(self.scope.io, self.csline, True)

        spi = SPI(self.scope._getNAEUSB())
        spi.enable(10E6)

        if device.lower() == "ice40up5k":
            cmd1 = [0x11, 0x00] + [0]*126
            cram_cmds = [[0x7E, 0xAA, 0x99, 0x7E, 0x92, 0x00, 0x20, 0x62, 0x02, 0xB3, 0x82, 0x00, 0x00, 0x72, 0x01, 0x50, 0x11, 0x00, 0x01, 0x02, 0x00, 0x00],
                        [0x7E, 0xAA, 0x99, 0x7E, 0x72, 0x00, 0xB0, 0x11, 0x01, 0x01, 0x02, 0x00, 0x00],
                        [0x7E, 0xAA, 0x99, 0x7E, 0x72, 0x01, 0x50, 0x11, 0x02, 0x01, 0x02, 0x00, 0x00],
                        [0x7E, 0xAA, 0x99, 0x7E, 0x72, 0x00, 0xB0, 0x11, 0x03, 0x01, 0x02, 0x00, 0x00]]
            blocksize_read = [29064, 15224, 29064, 15224]
            #No idea why final CRAM has different shift amount - see notes below
            blocksize_shiftamount = [5, 5, 5, 4]
            blocksize = 29064

            bram_cmds = [[0x7E, 0xAA, 0x99, 0x7E, 0x62, 0x00, 0x9F, 0x72, 0x00, 0x80, 0x11, 0x00, 0x82, 0x00, 0x00, 0x01, 0x04, 0x00, 0x00],
                        [0x7E, 0xAA, 0x99, 0x7E, 0x82, 0x00, 0x80, 0x01, 0x04, 0x00, 0x00],
                        [0x7E, 0xAA, 0x99, 0x7E, 0x11, 0x01, 0x62, 0x00, 0x4F, 0x82, 0x00, 0x00, 0x01, 0x04, 0x00, 0x00],
                        [0x7E, 0xAA, 0x99, 0x7E, 0x82, 0x00, 0x80, 0x01, 0x04, 0x00, 0x00],
                        [0x7E, 0xAA, 0x99, 0x7E, 0x62, 0x00, 0x9F, 0x11, 0x02, 0x82, 0x00, 0x00, 0x01, 0x04, 0x00, 0x00],
                        [0x7E, 0xAA, 0x99, 0x7E, 0x82, 0x00, 0x80, 0x01, 0x04, 0x00, 0x00],
                        [0x7E, 0xAA, 0x99, 0x7E, 0x62, 0x00, 0x4F, 0x11, 0x03, 0x82, 0x00, 0x00, 0x01, 0x04, 0x00, 0x00],
                        [0x7E, 0xAA, 0x99, 0x7E, 0x82, 0x00, 0x80, 0x01, 0x04, 0x00, 0x00],
                        ]
            bram_blocksize = [2560, 2560, 1280, 1280, 2560, 2560, 1280, 1280]
            bram_shiftamount = [5, 5, 5, 5, 5, 5, 5, 5]
        else:
            raise NotImplementedError("Only ice40UP5K supported for readback")

        #Initial clocks after bitstream loaded
        spi.transfer([0xFF]*126, writeonly=True)
            
        #The following is done with CS high (inactive)
        spi.transfer(cmd1)
        time.sleep(0.002)
        spi.transfer([1, 2, 0, 0])
        spi.toggle_sck(5)

        time.sleep(0.002)

        #Setup first block
        spi.transfer([0x00]*blocksize)

        cram = []

        for i in range(0, len(blocksize_read)):
            cram.append( self.read_block(spi, blocksize_read[i], cram_cmds[i], blocksize_shiftamount[i]) )

        bram = []

        for i in range(0, len(bram_blocksize)):
            bram.append( self.read_block(spi, bram_blocksize[i], bram_cmds[i], bram_shiftamount[i]) )
            
        #Exit this mode so we can finish start sequence
        spi.transfer([0x7E, 0xAA, 0x99, 0x7E, 0x01, 0x0B])

        return (cram, bram)

    def start(self, sck_speed=1E6):
        #Finally send go command + extra clocks, need CS pin again
        spi = SPI(self.scope._getNAEUSB(), cs_line=(self.scope.io, self.csline))
        spi.enable(sck_speed)
        spi.transfer([1, 6, 0])
        spi.transfer([0]*256, writeonly=True)
        spi.disable()

    def check_busy(self, spi, preclocks=600):
        """Sending 0x05, 0x00 seems to check if device is busy, needs some SCK toggles w/o CS being low first."""
        spi.transfer([0xFF]*preclocks, stop=False, start=False, writeonly=True)
        resp = spi.transfer([0x05, 0x00])
        return resp[1]

    def wait_until_notbusy(self, spi, max_times=5):
        """Potentially checks if device is busy or not (unknown exact command)"""
        for i in range(0, max_times):
            bsy = self.check_busy(spi)
            if bsy == 0x01:
                continue
            elif bsy == 0x00:
                return
            else:
                raise IOError("Invalid busy response: 0x%02X"%bsy)
        raise IOError("Device never indicated ready.")

    def check_idcode(self, csname="pdid"):
        """Reads idcode (this is undocumented feature of ICE40 so very beta).
        
        Requires MISO connection (not always required). Only tested with certain
        devices, so if this fails try programming anyway.
        """

        #Need to take control of ISP lines for erase to work
        util.chipwhisperer_extra.cwEXTRA.setAVRISPMode(False)

        self.erase_and_init()

        try:
            spi = SPI(self.scope._getNAEUSB(), cs_line=(self.scope.io, csname))
            spi.enable(1E6)
            spi.transfer([0x7E, 0xAA, 0x99, 0x7E, 0x01, 0x0E])

            self.wait_until_notbusy(spi)

            spi.transfer([0xFF]*2, stop=False, start=False, writeonly=True)
            spi.transfer([0x82, 0x00, 0x00, 0x20, 0x00, 0x15, 0xF2, 0xF0, 0xA2, 0x00, 0x00, 0x00])

            self.wait_until_notbusy(spi)

            spi.transfer([0xFF]*2, stop=False, start=False, writeonly=True)
            spi.transfer([0x83, 0x00, 0x00, 0x25, 0x20])

            self.wait_until_notbusy(spi)

            spi.transfer([0xFF]*2, stop=False, start=False, writeonly=True)
            spi.transfer([0x03], stop=False)
            full_idcode = spi.transfer([0x0]*13, start=False, stop=False)

            # I think this is ID code? No documentation though.
            # Ohter bytes
            idcode = full_idcode[12]

            #Other data used for finerprinting(? not done by ice)
            full_afteridcode = spi.transfer([0x0]*128, start=False)

            # ice programmer does the following, shouldn't matter since we are done with
            # the idcoe read by now?
            self.wait_until_notbusy(spi)
            spi.transfer([0xFF]*2, stop=False, start=False, writeonly=True)
            spi.transfer([0x83, 0x00, 0x00, 0x25, 0x00])
        finally:
            spi.disable()
        
        if idcode == 0x20:
            name = "iCE40UP5K"
        elif idcode == 0x12:
            name = "iCE5LP4K"
        else:
            name = "unknown"
        
        return (name, idcode, list(full_idcode), list(full_afteridcode))

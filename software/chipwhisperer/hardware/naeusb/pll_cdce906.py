# -*- coding: utf-8 -*-
#
# Copyright (c) 2014-2018, NewAE Technology Inc
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

import math
import time

class PLLCDCE906(object):

    def __init__(self, usb, ref_freq):
        self._usb = usb
        self.reffreq = ref_freq
        self._pll0source = 'PLL0'
        self._pll0slew = '+0nS'
        self._pll1slew = '+0nS'
        self._pll2slew = '+0nS'

    def pll_outfreq_set(self, freq, outnum):
        """Set the output frequency of a PLL

        Args:
            freq (int): The desired output frequency of the PLL. Must be in
                range [630kHz, 167MHz]
            outnum (int): The PLL to set the output frequency of

        Raises:
            ValueError: Desired frequency is bigger than 167MHz or smaller than
                630kHz
        """
        if freq is None or (freq < 630E3) or (freq > 167E6):
            raise ValueError("Illegal clock frequency = %d" % freq)
        best = self.calcMulDiv(freq, self.reffreq)
        self.pllwrite(outnum, N=best[0], M=best[1], outdiv=best[2])
        self.outputUpdateOutputs(outnum)

    def outnumToPin(self, outnum):
        """Convert from PLL Number to actual output pin"""
        if outnum == 0:
            return 0
        elif outnum == 1:
            return 1
        elif outnum == 2:
            return 4
        else:
            raise ValueError("Invalid output number = %d" % outnum)

    def outputUpdateOutputs(self, outnum, pllsrc_new=None, pllenabled_new=None, pllslewrate_new=None):
        """Update the output pins with enabled/disabled, slew rate, etc"""
        # Map to output pins on CDCE906 Chip
        if outnum == 0:
            outpin = 0
            if pllsrc_new is None:
                src = self._pll0source
            else:
                src = pllsrc_new

            if src == 'PLL0':
                divsrc = 0
            elif src == 'PLL1':
                divsrc = 1
            elif src == 'PLL2':
                divsrc = 2
        elif outnum == 1:
            outpin = 1
            divsrc = 1
        elif outnum == 2:
            outpin = 4
            divsrc = 2

        if pllenabled_new is None:
            pll_enabled = self.pll_outenable_get(outnum)
        else:
            pll_enabled = pllenabled_new

        if pllslewrate_new is None:
            if   (outnum == 0): pll_slewrate = self._pll0slew
            elif (outnum == 1): pll_slewrate = self._pll1slew
            elif (outnum == 2): pll_slewrate = self._pll2slew
        else:
            pll_slewrate = pllslewrate_new

        self.cdce906setoutput(outpin, divsrc, slewrate=pll_slewrate, enabled=pll_enabled)

    def pll_outfreq_get(self, outnum):
        """Read the programmed output frequency from a PLL on the CW305

        Args:
            outnum (int): PLL to read from (i.e. for PLL 0, use outnum=0)

        Returns:
            The output frequency of the specified PLL
        """
        settings = self.pllread(outnum)
        freq = ((self.reffreq * float(settings[0])) / float(settings[1])) / float(settings[2])
        return freq

    def pll_outenable_set(self, enabled, outnum):
        """Enable or disable a PLL

        Args:
            enabled (bool): Whether to enable (True) or disable (False) the
                specified PLL.
            outnum (int): The PLL to enable or disable
        """
        self.outputUpdateOutputs(outnum, pllenabled_new=enabled)

    def pll_outenable_get(self, outnum):
        """Get whether a PLL is enabled or not

        Args:
            outnum (int): The PLL to get the enable status of

        Returns:
            True if the PLL is enabled, False if it isn't
        """
        outpin = self.outnumToPin(outnum)
        data = self.cdce906read(19 + outpin)
        return bool(data & (1 << 3))

    def pll_outslew_set(self, slew, outnum):
        """ Set clock slew rate for the selected clock output.

        Args:
            slew (string): Desired slew rate. Allowed values: '+3nS', '+2nS', 
                            '+1nS', '+0nS'.

            outnum (int): PLL output

        Raises:
            ValueError: Invalid PLL output or invalid slew value
        """
        if slew in ['+3nS', '+2nS', '+1nS', '+0nS']:
            if   outnum == 0: self._pll0slew = slew
            elif outnum == 1: self._pll1slew = slew
            elif outnum == 2: self._pll2slew = slew
            else:
                raise ValueError("Invalid clock number: %d" % outnum)
            self.outputUpdateOutputs(outnum, pllslewrate_new=slew)
        else:
            raise ValueError("Invalid slew value: %s" % slew)

    def pll_outslew_get(self, outnum):
        """Get slew rate of PLL output"""
        outpin = self.outnumToPin(outnum)
        data = self.cdce906read(19 + outpin)
        slew = data & (3 << 4)

        if slew == (0 << 4):
            return "+3nS"
        elif slew == (1 << 4):
            return "+2nS"
        elif slew == (2 << 4):
            return "+1nS"
        else:
            return "+0nS"

    def pll_outsource_set(self, source, outnum):
        """Update clock source for the selected clock output.

        Output 0 can be configured for PLL0, PLL1, or PLL2.

        Output 1 is restricted to PLL1.

        Output 2 is restricted to PLL2.

        Args:
            source (String): Desired clock source ('PLL0', 'PLL1', 'PLL2')
            outnum (int): Output to configure. 0 goes to CLK-SMA X6, 1 goes
                          to FPGA pin N13, and 2 goes to FPGA pin E12.

        Raises:
            ValueError: Invalid source for specified clock output
        """
        if outnum==0 and source in ['PLL0', 'PLL1', 'PLL2']:
            self._pll0source = source
            self.outputUpdateOutputs(outnum, pllsrc_new=source)
        elif (outnum==1 and source == 'PLL1') or (outnum==2 and source == 'PLL2'):
            self.outputUpdateOutputs(outnum, pllsrc_new=source)
        else:
            raise ValueError("Invalid source %s for clock %d" % (source, outnum))

    def pll_outsource_get(self, outnum):
        """Get output source settings"""
        outpin = self.outnumToPin(outnum)
        data = self.cdce906read(19 + outpin)
        divsrc = data & 0x07

        if divsrc == 0:
            return "PLL0"
        elif divsrc == 1:
            return "PLL1"
        elif divsrc == 2:
            return "PLL2"
        else:
            return "UNKNOWN"

    def pll_writedefaults(self):
        """Save PLL settings to EEPROM, making them power-on defaults"""
        # Set bit high to enable write
        data = self.cdce906read(26) & (~(1 << 7))
        self.cdce906write(26, data | (1 << 7))

        # Wait until done
        while (self.cdce906read(24) & (1 << 7)):
            time.sleep(0.05)

        # Set bit low
        self.cdce906write(26, data)

    def cdce906write(self, addr, data):
        """ Write a byte to the CDCE906 External PLL Chip """

        # print "Write %d = %x" % (addr, data)
        self._usb.sendCtrl(0x30, data=[0x01, addr, data])
        resp = self._usb.readCtrl(0x30, dlen=2)
        if resp[0] != 2:
            time.sleep(0.01)
            self._usb.sendCtrl(0x30, data=[0x01, addr, data])
            resp = self._usb.readCtrl(0x30, dlen=2)
            if resp[0] != 2:
                raise IOError("CDCE906 Write Error, response = %d" % resp[0])

    def cdce906read(self, addr):
        """ Read a byte from the CDCE906 External PLL Chip """
        self._usb.sendCtrl(0x30, data=[0x00, addr, 0])
        resp = self._usb.readCtrl(0x30, dlen=2)
        if resp[0] != 2:
            time.sleep(0.01)
            self._usb.sendCtrl(0x30, data=[0x00, addr, 0])
            resp = self._usb.readCtrl(0x30, dlen=2)
            if resp[0] != 2:
                raise IOError("CDCE906 Read Error, response = %d" % resp[0])
        return resp[1]

    def cdce906init(self):
        """ Set defaults on CDCE906 PLL Chip """
        # Default is:
        # Y0 = P0
        # Y1 = P1
        # Y2,Y3,Y5 = disabled
        # Y4 = P2
        self.outputUpdateOutputs(0)
        self.outputUpdateOutputs(1)
        self.outputUpdateOutputs(2)

        self.cdce906setoutput(2, 0, enabled=False)
        self.cdce906setoutput(3, 0, enabled=False)
        self.cdce906setoutput(5, 0, enabled=False)

        self.cdce906write(9, (1 << 5) | (self.cdce906read(9) & 0x1F))  # PLL 1
        self.cdce906write(10, (2 << 5) | (self.cdce906read(10) & 0x1F))  # PLL2 not SSC
        self.cdce906write(11, 4 | (self.cdce906read(11) & 0xF8))  # PLL 3

    def cdce906setoutput(self, outpin, divsource, slewrate='+0nS', enabled=True, inverted=False):
        """Setup the outputs for the PLL chip"""
        data = 0
        if enabled:
            data |= 1 << 3
        data |= divsource
        if slewrate == "+0nS":
            data |= (3 << 4)
        elif slewrate == "+1nS":
            data |= (2 << 4)
        elif slewrate == "+2nS":
            data |= (1 << 4)
        elif slewrate == "+3nS":
            data |= (0 << 4)
        else:
            raise ValueError("Invalid slew rate: %s" % str(slewrate))
        if inverted:
            data |= 1 << 6
        self.cdce906write(19 + outpin, data)

    def pll_enable_set(self, enabled):
        """Enable or disable the PLL chip"""
        base = self.cdce906read(12)
        if enabled:
            self.cdce906write(12, base & ~(1 << 6))
        else:
            self.cdce906write(12, base | (1 << 6))

    def pll_enable_get(self):
        """Read if PLL chip is enabled or disabled"""
        base = self.cdce906read(12)
        if base & (1 << 6):
            return False
        else:
            return True

    def calcMulDiv(self, freqdesired, freqsource):
        """Calculate Multiply & Divide settings for PLL based on input frequency"""

        lowerror = 1E99
        best = (0, 0, 0)

        # Figured out divider settings to put fvco in range 80-300
        maxoutdiv = int(math.floor(300E6 / freqdesired))
        minoutdiv = int(math.ceil(80E6 / freqdesired))
        maxoutdiv = min(maxoutdiv, 127)

        # If finds an exact match, this is very fast... if not this
        # can take a little while to calculate
        for outdiv in range(minoutdiv, maxoutdiv + 1):
            fvco = freqdesired * outdiv
            for N in range(1, 4096):
                for M in range(1, 512):
                    err = abs(fvco - ((freqsource * N) / M))
                    if err < lowerror:
                        lowerror = err / outdiv
                        best = (N, M, outdiv)
                        if lowerror == 0:
                            return best
        return best

    def pllwrite(self, pllnum, N, M, outdiv):
        """Write N/M/output divisors to PLL chip"""
        offset = 3 * pllnum

        self.cdce906write(1 + offset, M & 0xFF)
        self.cdce906write(2 + offset, N & 0xFF)
        base = self.cdce906read(3 + offset)
        base = base & 0xE0  # Mask out lower bits
        base |= (M & (0x100)) >> 8
        base |= (N & (0xF00)) >> 7
        self.cdce906write(3 + offset, base)
        self.cdce906write(13 + pllnum, outdiv & 0x7F)

        # Set PLL Mode (high-speed or regular)
        fvco = (self.reffreq * float(N)) / float(M)
        if fvco < 80E6 or fvco > 300E6:
            raise IOError("Fvco set to $d, out of range" % fvco)

        data = self.cdce906read(6)
        if pllnum == 0:
            pllbit = 7
        elif pllnum == 1:
            pllbit = 6
        elif pllnum == 2:
            pllbit = 5

        data &= ~(1 << pllbit)
        if fvco > 180E6:
            # High-speed mode (180-300 MHz)
            data |= 1 << pllbit
        self.cdce906write(6, data)

    def pllread(self, pllnum):
        """Read N/M/output divisors from PLL chip"""
        offset = 3 * pllnum

        # Read M & N
        M = self.cdce906read(1 + offset)
        N = self.cdce906read(2 + offset)
        temp = self.cdce906read(3 + offset)
        M |= ((temp & 0x01)) << 8
        N |= (temp & 0x1E) << 7

        outdiv = self.cdce906read(13 + pllnum) & 0x7F

        return (N, M, outdiv)

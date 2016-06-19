#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2013-2015, NewAE Technology Inc
# All rights reserved.
#
# Authors: Colin O'Flynn
#
# Find this and more at newae.com - this file is part of the chipwhisperer
# project, http://www.assembla.com/spaces/chipwhisperer
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
#=================================================

import zipfile
import StringIO
import base64
import chipwhisperer.capture.scopes.cwhardware.PartialReconfiguration as pr
from chipwhisperer.common.utils.parameter import Parameterized, Parameter, setupSetParam

glitchaddr = 51
glitchoffsetaddr = 25
glitchreadbackaddr = 56
CODE_READ       = 0x80
CODE_WRITE      = 0xC0

# sign extend b low bits in x
# from "Bit Twiddling Hacks"
def SIGNEXT(x, b):
    m = 1 << (b - 1)
    x = x & ((1 << b) - 1)
    return (x ^ m) - m


class ChipWhispererGlitch(Parameterized):
    """
    Drives the Glitch Module inside the ChipWhisperer Capture Hardware Rev2, or can be used to drive this FPGA module inserted into other systems.
    """
    CLKSOURCE0_BIT = 0b00000000
    CLKSOURCE1_BIT = 0b00000001
    CLKSOURCE_MASK = 0b00000011
    _name= 'Glitch Module'

    def __init__(self, parentParam, cwtype, scope, oa):

        # Setup FPGA partial configuration dataZ
        self.prCon = pr.PartialReconfigConnection()
        self.oa = oa

        self.params = Parameter(name=self.getName(), type='group').register()
        self.params.addChildren([
            {'name':'Clock Source', 'type':'list', 'values':{'Target IO-IN':self.CLKSOURCE0_BIT, 'CLKGEN':self.CLKSOURCE1_BIT},'set':self.setGlitchClkSource, 'get':self.glitchClkSource},
            {'name':'Glitch Width (as % of period)', 'key':'width', 'type':'float', 'limits':(0, 100), 'step':0.39062, 'readonly':True, 'value':10, 'action':self.updatePartialReconfig},
            {'name':'Glitch Width (fine adjust)', 'key':'widthfine', 'type':'int', 'limits':(-255, 255), 'set':self.setGlitchWidthFine, 'get':self.getGlitchWidthFine},
            {'name':'Glitch Offset (as % of period)', 'key':'offset', 'type':'float', 'limits':(0, 100), 'step':0.39062, 'readonly':True, 'value':10, 'action':self.updatePartialReconfig},
            {'name':'Glitch Offset (fine adjust)', 'key':'offsetfine', 'type':'int', 'limits':(-255, 255), 'set':self.setGlitchOffsetFine, 'get':self.getGlitchOffsetFine},
            {'name':'Glitch Trigger', 'type':'list', 'values':{'Ext Trigger:Continous':1, 'Manual':0, 'Continuous':2, 'Ext Trigger:Single-Shot':3}, 'set':self.setGlitchTrigger, 'get':self.glitchTrigger},
            {'name':'Single-Shot Arm', 'type':'list', 'key':'ssarm', 'values':{'Before Scope Arm':1, 'After Scope Arm':2}, 'value':2},
            {'name':'Ext Trigger Offset', 'type':'int', 'range':(0, 50000000), 'set':self.setTriggerOffset, 'get':self.triggerOffset},
            {'name':'Repeat', 'type':'int', 'limits':(1,255), 'set':self.setNumGlitches, 'get':self.numGlitches},
            {'name':'Manual Trigger / Single-Shot Arm', 'type':'action', 'action': lambda _ : self.glitchManual()},
            {'name':'Output Mode', 'type':'list', 'values':{'Clock XORd':0, 'Clock ORd':1, 'Glitch Only':2, 'Clock Only':3, 'Enable Only':4}, 'set':self.setGlitchType, 'get':self.glitchType},
            {'name':'Read Status', 'type':'action', 'action':self.checkLocked},
            {'name':'Reset DCM', 'type':'action', 'action':self.resetDCMs},
        ])

        # Check if we've got partial reconfiguration stuff for this scope
        try:
            if cwtype == "cwrev2" or cwtype == "cwcrev2":
                settingprefix = "cwcrev2"
                partialbasename = "s6lx25"
                self.glitchPR = pr.PartialReconfigDataMulti()
            elif cwtype == "cwlite":
                settingprefix = "cwlite"
                partialbasename = "cwlite"
                self.glitchPR = pr.PartialReconfigDataMulti()
            elif cwtype == "cw1200":
                settingprefix = "cw1200"
                partialbasename = "cw1200"
                self.glitchPR = pr.PartialReconfigDataMulti()
            else:
                raise ValueError("Invalid ChipWhisperer Mode: %s" % cwtype)

            if scope.cwFirmwareConfig.loader._release_mode != "debug":

                if scope.cwFirmwareConfig.loader._release_mode == "builtin":
                    filelike = scope.cwFirmwareConfig.loader._bsBuiltinData
                    zfile = zipfile.ZipFile(filelike)
                elif scope.cwFirmwareConfig.loader._release_mode == "zipfile":
                    fileloc = scope.cwFirmwareConfig.loader._bsZipLoc
                    if zipfile.is_zipfile(fileloc):
                        zfile = zipfile.ZipFile(fileloc, "r")
                    else:
                        print "Partial Reconfiguration DISABLED: no zip-file for FPGA"
                        zfile = None
                else:
                    print "Partial Reconfiguration DISABLED: no PR data for FPGA"
                    zfile = None
                    raise ValueError("Unknown FPGA mode: %s"%scope.cwFirmwareConfig.loader._release_mode)

                if zfile:
                    self.glitchPR.load(zfile.open("%s-glitchwidth.p" % partialbasename))
                    self.glitchPR.load(zfile.open("%s-glitchoffset.p" % partialbasename))
                    self.prEnabled = True
                else:
                    self.prEnabled = False
            else:
                print "Partial Reconfiguration DISABLED: Debug bitstream mode"
                self.prEnabled = False

        except IOError as e:
            print str(e)
            self.prEnabled = False
        except ValueError as e:
            print str(e)
            self.prEnabled = False
        except OSError as e:  # Also catches WindowsError
            print str(e)
            self.prEnabled = False

        if self.prEnabled:
            # Enable glitch width, check what we've got access to
            self.findParam('width').setReadonly(False)
            lim = (self.glitchPR.limitList[0][0] / 2.55, self.glitchPR.limitList[0][1] / 2.55)
            self.findParam('width').setLimits(lim)

            self.findParam('offset').setReadonly(False)
            lim = (self.glitchPR.limitList[1][0] / 2.55, self.glitchPR.limitList[1][1] / 2.55)
            self.findParam('offset').setLimits(lim)

        self.setOpenADC(oa)


    def setOpenADC(self, oa):
        self.oa = None
        if self.prEnabled:
            self.prCon.con(oa)

            # Check this is actually working
            if self.prCon.isPresent() == False:
                self.prEnabled = False
                print "WARNING: Partial Reconfiguration block not detected, PR disabled"
                return

            # Reset FPGA back to defaults in case previous bitstreams loaded
            self.updatePartialReconfig()
        self.oa = oa

        try:
            self.params.refreshAllParameters()
        except TypeError:
            return

    def updatePartialReconfig(self, anything=None):
        """
        Reads the values set via the GUI & updates the hardware settings for partial reconfiguration. Checks that PR
        is enabled with self.prEnabled.
        """

        width = float(self.findParam('width').getValue())
        offset = float(self.findParam('offset').getValue())

        widthint = round((width / 100) * 256)
        offsetint = round((offset / 100) * 256)

        if (widthint == 0):
            print "WARNING: Partial reconfiguration for width = 0 may not work"

        if (offsetint == 0):
            print "WARNING: Partial reconfiguration for width = 0 may not work"

        bs = self.glitchPR.getPartialBitstream([widthint, offsetint])

        if self.prEnabled:
            self.prCon.program(bs)
            if self.oa is not None:
                self.resetDCMs(keepPhase=True)
                # print "Partial: %d %d" % (widthint, offsetint)

            self.updateGlitchReadBack()

    def updateGlitchReadBack(self, test=False):
        """Updates the readback register in the FPGA with glitch information, used for LCD update on CW1200 hardware."""

        width = float(self.findParam('width').getValue())
        offset = float(self.findParam('offset').getValue())

        #Don't write if PR disable by accident
        if self.oa is None:
            return

        widthint = int(round((width / 100) * 256))
        offsetint = int(round((offset / 100) * 256))

        cmd = bytearray(8)

        #Integer downloads
        cmd[0] = offsetint & 0xff
        cmd[1] = (offsetint >> 8) & 0xff
        cmd[2] = widthint & 0xff
        cmd[3] = (widthint >> 8) & 0xff

        #Floating point info
        cmd[4] = int(offset) & 0xff
        cmd[5] = int(("%f"%offset).split(".")[1][0:2]) & 0xff

        cmd[6] = int(width) & 0xff
        cmd[7] = int(("%f"%width).split(".")[1][0:2]) & 0xff

        self.oa.sendMessage(CODE_WRITE, glitchreadbackaddr, cmd, Validate=False)

    @setupSetParam("Ext Trigger Offset")
    def setTriggerOffset(self, offset):
        """Set offset between trigger event and glitch in clock cycles"""
        cmd = bytearray(4)
        cmd[0] = ((offset >> 0) & 0xFF)
        cmd[1] = ((offset >> 8) & 0xFF)
        cmd[2] = ((offset >> 16) & 0xFF)
        cmd[3] = ((offset >> 24) & 0xFF)
        self.oa.sendMessage(CODE_WRITE, glitchoffsetaddr, cmd)

    def triggerOffset(self):
        """Get offset between trigger event and glitch in clock cycles"""
        cmd = self.oa.sendMessage(CODE_READ, glitchoffsetaddr, maxResp=4)
        offset = cmd[0]
        offset |= cmd[1] << 8
        offset |= cmd[2] << 16
        offset |= cmd[3] << 24
        return offset

    @setupSetParam("Glitch Offset (fine adjust)")
    def setGlitchOffsetFine(self, fine):
        """Set the fine glitch offset adjust, range -255 to 255"""
        current = self.oa.sendMessage(CODE_READ, glitchaddr, Validate=False, maxResp=8)

        if current is None or len(current) < 8:
            print "Glitch Module not present?"
            return

        LSB = fine & 0x00FF
        MSB = (fine & 0x0100) >> 8

        current[0] = LSB #7..0
        current[1] = (current[1] & ~0x01) | MSB #15..8

        #Start adjust
        current[2] = current[2] | 0x04  # 23..16
        #assign clockglitch_settings_read[37] = phase1_done_reg;
        #assign clockglitch_settings_read[38] = phase2_done_reg;

        self.oa.sendMessage(CODE_WRITE, glitchaddr, current, Validate=False)

    def getGlitchWidthFine(self):
        return self.getDCMStatus()[0]

    @setupSetParam("Glitch Width (fine adjust)")
    def setGlitchWidthFine(self, fine):
        """Set the fine glitch width adjust, range -255 to 255"""
        current = self.oa.sendMessage(CODE_READ, glitchaddr, Validate=False, maxResp=8)

        if current is None or len(current) < 8:
            print "Glitch Module not present?"
            return

        LSB = fine & 0x00FF
        MSB = (fine & 0x0100) >> 8

        current[1] = (current[1] & 0x01) | ((LSB & 0x7F) << 1)
        current[2] = (current[2] & ~0x03) | ((LSB >> 7) | (MSB << 1))

        #Start adjust
        current[2] = current[2] | 0x04  # 23..16
        #assign clockglitch_settings_read[37] = phase1_done_reg;
        # assign clockglitch_settings_read[38] = phase2_done_reg;
        self.oa.sendMessage(CODE_WRITE, glitchaddr, current, Validate=False)

    def getGlitchOffsetFine(self):
        return self.getDCMStatus()[1]

    def getDCMStatus(self):
        current = self.oa.sendMessage(CODE_READ, glitchaddr, Validate=False, maxResp=8)

        phase1 = current[2] >> 3
        phase1 |= (current[3] & 0x0F) << 5
        phase1 = SIGNEXT(phase1, 9)

        phase2 = (current[3] & 0xF0) >> 4
        phase2 |= (current[4] & 0x1F) << 4
        phase2 = SIGNEXT(phase2, 9)

        dcm1Lock = False
        dcm2Lock = False

        if current[4] & 0x80:
            dcm1Lock = True

        if current[5] & 0x01:
            dcm2Lock = True

        return (phase1, phase2, dcm1Lock, dcm2Lock)

    def resetDCMs(self, keepPhase=False):
        """Reset the DCMs for the Glitch width & Glitch offset. Required after doing a PR operation"""

        reset = self.oa.sendMessage(CODE_READ, glitchaddr, Validate=False, maxResp=8)
        reset[5] |= (1<<1)
        self.oa.sendMessage(CODE_WRITE, glitchaddr, reset, Validate=False)
        reset[5] &= ~(1<<1)
        self.oa.sendMessage(CODE_WRITE, glitchaddr, reset, Validate=False)

        # Reload any special phase offset
        if keepPhase:
            self.setGlitchWidthFine(self.findParam('widthfine').getValue())
            self.setGlitchOffsetFine(self.findParam('offsetfine').getValue())
        else:
            self.findParam('widthfine').setValue(0)
            self.findParam('offsetfine').setValue(0)

    def checkLocked(self):
        """Check if the DCMs are locked and print results """

        stat = self.getDCMStatus()
        print "DCM1: Phase %d, Locked %r" % (stat[0], stat[2])
        print "DCM2: Phase %d, Locked %r" % (stat[1], stat[3])

    @setupSetParam("Repeat")
    def setNumGlitches(self, num):
        """Set number of glitches to occur after a trigger"""
        resp = self.oa.sendMessage(CODE_READ, glitchaddr, Validate=False, maxResp=8)

        if resp is None or len(resp) < 8:
            print "Glitch Module not present?"
            return

        if num < 1:
            num = 1
        resp[6] = num-1
        self.oa.sendMessage(CODE_WRITE, glitchaddr, resp, Validate=False)

    def numGlitches(self):
        """Get number of glitches to occur after a trigger"""
        resp = self.oa.sendMessage(CODE_READ, glitchaddr, Validate=False, maxResp=8)
        return resp[6]+1

    @setupSetParam("Glitch Trigger")
    def setGlitchTrigger(self, trigger):
        """Set glitch trigger type (manual, continous, adc-trigger)"""
        resp = self.oa.sendMessage(CODE_READ, glitchaddr, Validate=False, maxResp=8)
        resp[5] = (resp[5] & ~(0x0C)) | (trigger << 2)
        self.oa.sendMessage(CODE_WRITE, glitchaddr, resp, Validate=False)

    def glitchTrigger(self):
        """Get glitch trigger type"""
        resp = self.oa.sendMessage(CODE_READ, glitchaddr, Validate=False, maxResp=8)
        return (resp[5] & 0x0C) >> 2

    @setupSetParam("Output Mode")
    def setGlitchType(self, t):
        """Set glitch output type (ORd with clock, XORd with clock, clock only, glitch only)"""
        resp = self.oa.sendMessage(CODE_READ, glitchaddr, Validate=False, maxResp=8)
        resp[5] = (resp[5] & ~(0x70)) | (t << 4)
        self.oa.sendMessage(CODE_WRITE, glitchaddr, resp, Validate=False)

    def glitchType(self):
        resp = self.oa.sendMessage(CODE_READ, glitchaddr, Validate=False, maxResp=8)
        return (resp[5] & 0x70) >> 4

    def glitchManual(self):
        """
        Cause a single glitch event to occur. Depending on setting of numGlitches() this may mean
        multiple glitches in a row
        """
        resp = self.oa.sendMessage(CODE_READ, glitchaddr, Validate=False, maxResp=8)
        resp[5] = resp[5] | (1 << 7)
        self.oa.sendMessage(CODE_WRITE, glitchaddr, resp, Validate=False)
        resp[5] = resp[5] & ~(1 << 7)
        self.oa.sendMessage(CODE_WRITE, glitchaddr, resp, Validate=False)

    def glitchArm(self):
        """If trigger is set to single-shot mode, this must be called before the selected trigger occurs"""
        self.glitchManual()

    @setupSetParam("Clock Source")
    def setGlitchClkSource(self, source):
        """Set the source of the glitched clock, either the HS1-In or the CLKGEN Module"""
        resp = self.oa.sendMessage(CODE_READ, glitchaddr, Validate=False, maxResp=8)
        resp[7] = (resp[7] & ~self.CLKSOURCE_MASK) | source
        self.oa.sendMessage(CODE_WRITE, glitchaddr, resp, Validate=False)

    def glitchClkSource(self):
        resp = self.oa.sendMessage(CODE_READ, glitchaddr, Validate=False, maxResp=8)
        return (resp[7] & self.CLKSOURCE_MASK)

    def armPreScope(self):
        """Called before scope trigger is armed"""
        if self.findParam('ssarm').getValue() == 1:
            self.glitchArm()

    def armPostScope(self):
        """Called after scope trigger is armed"""
        if self.findParam('ssarm').getValue() == 2:
            self.glitchArm()

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
import chipwhisperer.capture.scopes.cwhardware.PartialReconfiguration as pr
from chipwhisperer.common.utils import util
from pyqtgraph.parametertree import Parameter
from chipwhisperer.capture.api.ExtendedParameter import ExtendedParameter

glitchaddr = 51
glitchoffsetaddr = 25
CODE_READ       = 0x80
CODE_WRITE      = 0xC0

# sign extend b low bits in x
# from "Bit Twiddling Hacks"
def SIGNEXT(x, b):
    m = 1 << (b - 1)
    x = x & ((1 << b) - 1)
    return (x ^ m) - m

class ChipWhispererGlitch():
    """
    Drives the Glitch Module inside the ChipWhisperer Capture Hardware Rev2, or can be used to drive this FPGA module inserted into other systems.
    """

    CLKSOURCE0_BIT = 0b00000000
    CLKSOURCE1_BIT = 0b00000001
    CLKSOURCE_MASK = 0b00000011

    paramListUpdated = util.Signal()

    def __init__(self, showScriptParameter=None, cwtype="cwrev2"):
        paramSS = [
                {'name':'Clock Source', 'type':'list', 'values':{'Target IO-IN':self.CLKSOURCE0_BIT, 'CLKGEN':self.CLKSOURCE1_BIT}, 'value':self.CLKSOURCE0_BIT, 'set':self.setGlitchClkSource, 'get':self.glitchClkSource},
                {'name':'Glitch Width (as % of period)', 'key':'width', 'type':'float', 'limits':(0, 100), 'step':0.39062, 'readonly':True, 'value':0, 'set':self.updatePartialReconfig},
                {'name':'Glitch Width (fine adjust)', 'key':'widthfine', 'type':'int', 'limits':(-255, 255), 'set':self.setGlitchWidthFine},
                {'name':'Glitch Offset (as % of period)', 'key':'offset', 'type':'float', 'limits':(0, 100), 'step':0.39062, 'readonly':True, 'value':0, 'set':self.updatePartialReconfig},
                {'name':'Glitch Offset (fine adjust)', 'key':'offsetfine', 'type':'int', 'limits':(-255, 255), 'set':self.setGlitchOffsetFine},
                {'name':'Glitch Trigger', 'type':'list', 'values':{'Ext Trigger:Continous':1, 'Manual':0, 'Continuous':2, 'Ext Trigger:Single-Shot':3}, 'value':0, 'set':self.setGlitchTrigger, 'get':self.glitchTrigger},
                {'name':'Single-Shot Arm', 'type':'list', 'key':'ssarm', 'values':{'Before Scope Arm':1, 'After Scope Arm':2}, 'value':2},
                {'name':'Ext Trigger Offset', 'type':'int', 'range':(0, 50000000), 'value':0, 'set':self.setTriggerOffset, 'get':self.triggerOffset},
                {'name':'Repeat', 'type':'int', 'limits':(1,255), 'set':self.setNumGlitches, 'get':self.numGlitches},
                {'name':'Manual Trigger / Single-Shot Arm', 'type':'action', 'action':self.glitchManual},
                {'name':'Output Mode', 'type':'list', 'values':{'Clock XORd':0, 'Clock ORd':1, 'Glitch Only':2, 'Clock Only':3, 'Enable Only':4}, 'set':self.setGlitchType, 'get':self.glitchType},
                {'name':'Read Status', 'type':'action', 'action':self.checkLocked},
                {'name':'Reset DCM', 'type':'action', 'action':self.resetDCMs},
                ]

        # Setup FPGA partial configuration dataZ
        self.prCon = pr.PartialReconfigConnection()
        self.oa = None
        self.showScriptParameter = showScriptParameter

        # Check if we've got partial reconfiguration stuff for this scope
        try:
            if cwtype == "cwrev2" or cwtype == "cwcrev2":
                settingprefix = "cwcrev2"
                partialbasename = "s6lx25"
                self.glitchPR = pr.PartialReconfigDataMulti()
            elif cwtype == "cwlite":
                settingprefix = "cwlite"
                partialbasename = "cwlite"
                self.glitchPR = pr.PartialReconfigDataOffsetWidth()
            else:
                raise ValueError("Invalid ChipWhisperer Mode: %s" % cwtype)

            if util.globalSettings["%s-fpga-bitstream-mode" % settingprefix] == "zip":
                fileloc = util.globalSettings["%s-zipbitstream-location" % settingprefix]

                if fileloc:
                    if fileloc.startswith("."):
                        fileloc = util.globalSettings["cwcapture-starting-root"] + "/" + fileloc
                    zfile = zipfile.ZipFile(fileloc, "r")

                    if cwtype == "cwlite":
                        self.glitchPR.load(zfile.open(("%s-glitchoffsetwidth.p" % partialbasename)))
                    else:
                        self.glitchPR.load(zfile.open("%s-glitchwidth.p" % partialbasename))
                        self.glitchPR.load(zfile.open("%s-glitchoffset.p" % partialbasename))
                    self.prEnabled = True
                else:
                    print "Partial Reconfiguration DISABLED: no zip-file for FPGA"
                    self.prEnabled = False
            else:
                print "Partial Reconfiguration DISABLED: Debug bitstream mode"
                self.prEnabled = False

        except IOError, e:
            print str(e)
            self.prEnabled = False
        except ValueError, e:
            print str(e)
            self.prEnabled = False
        except OSError, e:  # Also catches WindowsError
            print str(e)
            self.prEnabled = False

        # self.prEnabled = False

        self.params = Parameter.create(name='Glitch Module', type='group', children=paramSS)
        ExtendedParameter.setupExtended(self.params, self)

        if self.prEnabled:
            # Enable glitch width, check what we've got access to
            self.findParam('width').setReadonly(False)
            lim = (self.glitchPR.limitList[0][0] / 2.55, self.glitchPR.limitList[0][1] / 2.55)
            self.findParam('width').setLimits(lim)

            self.findParam('offset').setReadonly(False)
            lim = (self.glitchPR.limitList[1][0] / 2.55, self.glitchPR.limitList[1][1] / 2.55)
            self.findParam('offset').setLimits(lim)

    def paramTreeChanged(self, param, changes):
        if self.showScriptParameter is not None:
            self.showScriptParameter(param, changes, self.params)

    def setOpenADC(self, oa):

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
            self.params.getAllParameters()
        except TypeError:
            return

    def paramList(self):
        p = []
        p.append(self.params)
        return p

    def updatePartialReconfig(self, anything=None):
        """
        Reads the values set via the GUI & updates the hardware settings for partial reconfiguration. Checks that PR
        is enabled with self.prEnabled.
        """

        width = self.findParam('width').value()
        offset = self.findParam('offset').value()

        widthint = round((width / 100) * 256)
        offsetint = round((offset / 100) * 256)

        bs = self.glitchPR.getPartialBitstream([widthint, offsetint])

        if self.prEnabled:
            self.prCon.program(bs)
            if self.oa is not None:
                self.resetDCMs(keepPhase=True)
                # print "Partial: %d %d" % (widthint, offsetint)

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

    def setGlitchOffsetFine(self, fine):
        """Set the fine glitch offset adjust, range -255 to 255"""
        current = self.oa.sendMessage(CODE_READ, glitchaddr, Validate=False, maxResp=8)

        if current is None or len(current) < 8:
            print "Glitch Module not present?"
            return

        LSB = fine & 0x00FF;
        MSB = (fine & 0x0100) >> 8;

        current[0] = LSB #7..0
        current[1] = (current[1] & ~0x01) | MSB #15..8

        #Start adjust
        current[2] = current[2] | 0x04  # 23..16
        #assign clockglitch_settings_read[37] = phase1_done_reg;
        #assign clockglitch_settings_read[38] = phase2_done_reg;

        self.oa.sendMessage(CODE_WRITE, glitchaddr, current, Validate=False)

    def getGlitchWidthFine(self):
        return self.getDCMStatus()[0]

    def setGlitchWidthFine(self, fine):
        """Set the fine glitch width adjust, range -255 to 255"""
        current = self.oa.sendMessage(CODE_READ, glitchaddr, Validate=False, maxResp=8)

        if current is None or len(current) < 8:
            print "Glitch Module not present?"
            return

        LSB = fine & 0x00FF;
        MSB = (fine & 0x0100) >> 8;

        current[1] = (current[1] & 0x01) | ((LSB & 0x7F) << 1);
        current[2] = (current[2] & ~0x03) | ((LSB >> 7) | (MSB << 1));

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
            self.setGlitchWidthFine(self.findParam('widthfine').value())
            self.setGlitchOffsetFine(self.findParam('offsetfine').value())
        else:
            self.findParam('widthfine').setValue(0)
            self.findParam('offsetfine').setValue(0)

    def checkLocked(self):
        """Check if the DCMs are locked and print results """

        stat = self.getDCMStatus()
        print "DCM1: Phase %d, Locked %r"%(stat[0], stat[2])
        print "DCM2: Phase %d, Locked %r" % (stat[1], stat[3])

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

    def setGlitchTrigger(self, trigger):
        """Set glitch trigger type (manual, continous, adc-trigger)"""
        resp = self.oa.sendMessage(CODE_READ, glitchaddr, Validate=False, maxResp=8)
        resp[5] = (resp[5] & ~(0x0C)) | (trigger << 2)
        self.oa.sendMessage(CODE_WRITE, glitchaddr, resp, Validate=False)

    def glitchTrigger(self):
        """Get glitch trigger type"""
        resp = self.oa.sendMessage(CODE_READ, glitchaddr, Validate=False, maxResp=8)
        return (resp[5] & 0x0C) >> 2

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
        if self.findParam('ssarm').value() == 1:
            self.glitchArm()

    def armPostScope(self):
        """Called after scope trigger is armed"""
        if self.findParam('ssarm').value() == 2:
            self.glitchArm()


#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2014, NewAE Technology Inc
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
import logging
import time
from ..base import ScopeTemplate
from chipwhisperer.common.utils import util, timer
from chipwhisperer.common.utils.parameter import setupSetParam, Parameterized
from chipwhisperer.common.utils.pluginmanager import Plugin

"""
This module has an interface to the PicoTech PicoScope device. It uses the
picoscope library at https://github.com/colinoflynn/pico-python which you
must install
"""
from picoscope import ps2000
from picoscope import ps5000a
from picoscope import ps6000


class PicoScopeBase(Parameterized):
    _name = 'Pico Scope'

    def __init__(self, psClass=None):
        self.ps = psClass
        self.dataUpdated = util.Signal()

        chlist = {}
        for t in self.ps.CHANNELS:
            if self.ps.CHANNELS[t] < self.ps.CHANNELS['MaxChannels']:
                chlist[t] = self.ps.CHANNELS[t]

        # Rebuild channel range as string + api value
        chRange = util.DictType()
        for key in sorted(self.ps.CHANNEL_RANGE):
            chRange[ key['rangeStr'] ] = key['rangeV']

        self.getParams().addChildren([
            {'name':'Trace Measurement', 'key':'trace', 'type':'group', 'children':[
                {'name':'Source', 'key':'tracesource', 'type':'list', 'values':chlist, 'value':0, 'action':self.updateCurrentSettings},
                {'name':'Probe Att.', 'key':'traceprobe', 'type':'list', 'values':{'1:1':1, '1:10':10}, 'value':1, 'action':self.updateCurrentSettings},
                {'name':'Coupling', 'key':'tracecouple', 'type':'list', 'values':self.ps.CHANNEL_COUPLINGS, 'value':0, 'action':self.updateCurrentSettings},
                {'name':'Y-Range', 'key':'traceyrange', 'type':'list', 'values':chRange, 'value':1.0, 'action':self.updateCurrentSettings},
            ]},
            {'name':'Trigger', 'key':'trig', 'type':'group', 'children':[
                {'name':'Source', 'key':'trigsource', 'type':'list', 'values':chlist, 'value':1, 'action':self.updateCurrentSettings},
                {'name':'Probe Att.', 'key':'trigprobe', 'type':'list', 'values':{'1:1':1, '1:10':10}, 'value':10, 'action':self.updateCurrentSettings},
                {'name':'Coupling', 'key':'trigcouple', 'type':'list', 'values':self.ps.CHANNEL_COUPLINGS, 'value':1, 'action':self.updateCurrentSettings},
                {'name':'Y-Range', 'key':'trigrange', 'type':'list', 'values':chRange, 'value':5.0, 'action':self.updateCurrentSettings},
                {'name':'Trigger Direction', 'key':'trigtype', 'type':'list', 'values':self.ps.THRESHOLD_TYPE, 'value':self.ps.THRESHOLD_TYPE["Rising"], 'action':self.updateCurrentSettings},
                {'name':'Trigger Level', 'key':'triglevel', 'type':'float', 'step':1E-2, 'siPrefix':True, 'suffix':'V', 'limits':(-5, 5), 'value':0.5, 'action':self.updateCurrentSettings},
            ]},
            {'name':'Sample Rate', 'key':'samplerate', 'type':'int', 'step':1E6, 'limits':(10000, 5E9), 'value':100E6, 'action':self.updateSampleRateFreq, 'siPrefix':True, 'suffix': 'S/s'},
            {'name':'Sample Length', 'key':'samplelength', 'type':'int', 'step':250, 'limits':(1, 500E6), 'value':500, 'action':self.updateSampleRateFreq},
            {'name':'Sample Offset', 'key':'sampleoffset', 'type':'int', 'step':1000, 'limits':(0, 100E6), 'value':0, 'action':self.updateSampleRateFreq},
        ])


    @setupSetParam("")
    def updateSampleRateFreq(self, _=None):
        if self.ps.handle is not None:
            paramSR = self.findParam('samplerate')
            paramSL = self.findParam('samplelength')

            try:
                self.ps.setSamplingFrequency(paramSR.getValue(), paramSL.getValue() + self.findParam('sampleoffset').getValue(), 1)
            except IOError:
                raise Warning("Invalid combination of sample rate, sample length, and/or sample offset!")

            #Need this callback as otherwise the setValue() is overwritten when the user toggles off, might be fixed
            #with Adriel's new parameterTypes, will have to test again
            self._faketimer = timer.runTask(self.updateSampleRateFreqUI, 0.1, True, True)

    def updateSampleRateFreqUI(self):
        paramSR = self.findParam('samplerate')
        paramSL = self.findParam('samplelength')

        if self.ps.sampleRate != paramSR.getValue():
            paramSR.setValue(self.ps.sampleRate)

        newSL = min(self.ps.maxSamples, paramSL.getValue())
        if newSL != paramSL.getValue():
            paramSL.setValue(newSL)

    def con(self):
        self.ps.open()
        self.updateCurrentSettings()
        self.params.init()

    def dis(self):
        self.ps.close()

    @setupSetParam("")
    def updateCurrentSettings(self, _=False):
        if self.ps.handle is None: return

        try:
            # Turn off all channels
            for c in self.ps.CHANNELS:
                self.ps.setChannel(c, enabled=False)
        except IOError:
            pass

        try:
            # Trace Channel
            TraceCh = self.findParam(['trace', 'tracesource']).getValue()
            TraceCo = self.findParam(['trace', 'tracecouple']).getValue()
            TraceY = self.findParam(['trace', 'traceyrange']).getValue()
            TraceP = self.findParam(['trace', 'traceprobe']).getValue()
            self.ps.setChannel(channel=TraceCh, coupling=TraceCo, VRange=TraceY, probeAttenuation=TraceP, enabled=True)

            # Trigger Channel
            TrigCh = self.findParam(['trig', 'trigsource']).getValue()
            TrigCo = self.findParam(['trig', 'trigcouple']).getValue()
            TrigY = self.findParam(['trig', 'trigrange']).getValue()
            TrigP = self.findParam(['trig', 'trigprobe']).getValue()
            self.ps.setChannel(channel=TrigCh, coupling=TrigCo, VRange=TrigY, probeAttenuation=TrigP, enabled=True)

            # Trigger
            self.ps.setSimpleTrigger(TrigCh, self.findParam(['trig', 'triglevel']).getValue(), direction=self.findParam(['trig', 'trigtype']).getValue(), timeout_ms=1000)

            self.updateSampleRateFreq()
        except IOError, e:
            raise IOError("Caught Error: %s" % str(e))

    def arm(self):
        self.ps.runBlock()

    def capture(self):

        #Hack for picoscope - report channel 0 only right now
        channelNum = 0

        while not self.ps.isReady():
            time.sleep(0.01)
        data = self.ps.getDataV(self.findParam(['trace', 'tracesource']).getValue(), self.findParam('samplelength').getValue(), startIndex=self.findParam('sampleoffset').getValue(), returnOverflow=True)
        if data[1] is True:
            logging.warning('Overflow in data')
        self.datapoints = data[0]
        self.dataUpdated.emit(channelNum, self.datapoints, 0, self.ps.sampleRate)

        # No timeout?
        return False


class PicoScope6000(PicoScopeBase, Plugin):
    _name = "PS6000"
    def __init__(self):
        PicoScopeBase.__init__(self, ps6000.PS6000(connect=False))


class PicoScope5000a(PicoScopeBase, Plugin):
    _name = "PS5000a"
    def __init__(self):
        PicoScopeBase.__init__(self, ps5000a.PS5000a(connect=False))


class PicoScope2000(PicoScopeBase, Plugin):
    _name = "PS2000"
    def __init__(self):
        PicoScopeBase.__init__(self, ps2000.PS2000(connect=False))

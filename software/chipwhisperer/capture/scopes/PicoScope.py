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

"""
This module has an interface to the PicoTech PicoScope device. It uses the
picoscope library at https://github.com/colinoflynn/pico-python which you
must install
"""

import time
from ._base import ScopeTemplate
from chipwhisperer.common.utils import util, timer
from picoscope import ps2000
from picoscope import ps5000a
from picoscope import ps6000


class PicoScope(ScopeTemplate): #TODO: ScopeBase instead?
    _name = 'Pico Scope'

    def __init__(self, parentParam=None, psClass=None):
        ScopeTemplate.__init__(self, parentParam)
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

        self.params.addChildren([
            {'name':'Trace Measurement', 'type':'group', 'children':[
                {'name':'Source', 'key':'tracesource', 'type':'list', 'values':chlist, 'value':0, 'set':self.updateCurrentSettings},
                {'name':'Probe Att.', 'key':'traceprobe', 'type':'list', 'values':{'1:1':1, '1:10':10}, 'value':1, 'set':self.updateCurrentSettings},
                {'name':'Coupling', 'key':'tracecouple', 'type':'list', 'values':self.ps.CHANNEL_COUPLINGS, 'value':0, 'set':self.updateCurrentSettings},
                {'name':'Y-Range', 'key':'traceyrange', 'type':'list', 'values':chRange, 'value':1.0, 'set':self.updateCurrentSettings},
            ]},
            {'name':'Trigger', 'type':'group', 'children':[
                {'name':'Source', 'key':'trigsource', 'type':'list', 'values':chlist, 'value':1, 'set':self.updateCurrentSettings},
                {'name':'Probe Att.', 'key':'trigprobe', 'type':'list', 'values':{'1:1':1, '1:10':10}, 'value':10, 'set':self.updateCurrentSettings},
                {'name':'Coupling', 'key':'trigcouple', 'type':'list', 'values':self.ps.CHANNEL_COUPLINGS, 'value':1, 'set':self.updateCurrentSettings},
                {'name':'Y-Range', 'key':'trigrange', 'type':'list', 'values':chRange, 'value':5.0, 'set':self.updateCurrentSettings},
                {'name':'Trigger Direction', 'key':'trigtype', 'type':'list', 'values':self.ps.THRESHOLD_TYPE, 'value':2, 'set':self.updateCurrentSettings},
                {'name':'Trigger Level', 'key':'triglevel', 'type':'float', 'step':1E-2, 'siPrefix':True, 'suffix':'V', 'limits':(-5, 5), 'value':0.5, 'set':self.updateCurrentSettings},
            ]},
            {'name':'Sample Rate', 'key':'samplerate', 'type':'int', 'step':1E6, 'limits':(10000, 5E9), 'value':100E6, 'set':self.updateSampleRateFreq, 'siPrefix':True, 'suffix': 'S/s'},
            {'name':'Sample Length', 'key':'samplelength', 'type':'int', 'step':5000, 'limits':(1, 500E6), 'value':5000, 'set':self.updateSampleRateFreq},
            {'name':'Sample Offset', 'key':'sampleoffset', 'type':'int', 'step':1000, 'limits':(0, 100E6), 'value':0, 'set':self.updateSampleRateFreq},
        ])
            
    def updateSampleRateFreq(self, ignored=None):
        if self.ps.handle is not None:
            paramSR = self.findParam('samplerate')
            paramSL = self.findParam('samplelength')
            self.ps.setSamplingFrequency(paramSR.value(), paramSL.value() + self.findParam('sampleoffset').value(), 1)

            self._faketimer = timer.runTask(self.updateSampleRateFreqUI, 0.1, True, True)

    def updateSampleRateFreqUI(self):
        paramSR = self.findParam('samplerate')
        paramSL = self.findParam('samplelength')

        print self.ps.sampleRate

        if self.ps.sampleRate != paramSR.value():
            paramSR.setValue(self.ps.sampleRate)

        newSL = min(self.ps.maxSamples, paramSL.value())
        if newSL != paramSL.value():
            paramSL.setValue(newSL)


    def con(self):
        self.ps.open()
        self.updateCurrentSettings()
    
    def updateCurrentSettings(self, ignored=False):
        if self.ps.handle is None: return

        try:
            # Turn off all channels
            for c in self.ps.CHANNELS:
                self.ps.setChannel(c, enabled=False)
        except IOError:
            pass

        try:
            # Trace Channel
            TraceCh = self.findParam('tracesource').value()
            TraceCo = self.findParam('tracecouple').value()
            TraceY = self.findParam('traceyrange').value()
            TraceP = self.findParam('traceprobe').value()
            self.ps.setChannel(channel=TraceCh, coupling=TraceCo, VRange=TraceY, probeAttenuation=TraceP, enabled=True)

            # Trigger Channel
            TrigCh = self.findParam('trigsource').value()
            TrigCo = self.findParam('trigcouple').value()
            TrigY = self.findParam('trigrange').value()
            TrigP = self.findParam('trigprobe').value()
            self.ps.setChannel(channel=TrigCh, coupling=TrigCo, VRange=TrigY, probeAttenuation=TrigP, enabled=True)

            # Trigger
            self.ps.setSimpleTrigger(TrigCh, self.findParam('triglevel').value(), direction=self.findParam('trigtype').value(), timeout_ms=1000)

            self.updateSampleRateFreq()
        except IOError, e:
            raise IOError("Caught Error: %s" % str(e))

    def arm(self):       
        self.ps.runBlock()
        
    def capture(self):
        while(self.ps.isReady() == False): time.sleep(0.01)
        data = self.ps.getDataV(self.findParam('tracesource').value(), self.findParam('samplelength').value(), startIndex=self.findParam('sampleoffset').value(), returnOverflow=True)
        if data[1] is True:
            print "WARNING: OVERFLOW IN DATA"
        self.datapoints = data[0]
        self.dataUpdated.emit(self.datapoints, 0)

        # No timeout?
        return False


class PicoScopeInterface(ScopeTemplate):
    _name = "PicoScope"

    def __init__(self, parentParam=None):
        super(PicoScopeInterface, self).__init__(parentParam)

        scopes = {"PS6000": ps6000.PS6000(connect=False), "PS5000a": ps5000a.PS5000a(connect=False),
                  "PS2000": ps2000.PS2000(connect=False)}
        # self.connectChildParamsSignals(scopes) #TODO: Fix

        self.params.addChildren([
            {'name':'Scope Type', 'key':'type', 'type':'list', 'values':scopes, 'value':scopes["PS5000a"], 'set':self.setCurrentScope}
        ])
        self.setupActiveParams([lambda: self.lazy(self.scopetype)])

        self.scopetype = None
        self.advancedSettings = None
        self.setCurrentScope(self.findParam('type').value())

    def passUpdated(self, lst, offset):
        self.datapoints = lst
        self.offset = offset
        self.dataUpdated.emit(lst, offset)

    def setCurrentScope(self, scope, update=True):
        if scope is not None:
            self.scopetype = PicoScope(self, scope)
            self.scopetype.dataUpdated.connect(self.passUpdated)
        else:
            self.scopetype = scope

        if update:
            self.paramListUpdated.emit()
   
    def _con(self):
        if self.scopetype is not None:
            self.scopetype.con()
            return True
        return False

    def _dis(self):
        if self.scopetype is not None:
            self.scopetype.dis()  
        return True

    def doDataUpdated(self, l, offset=0):
        self.datapoints = l
        self.offset = offset
        if len(l) > 0:
            self.dataUpdated.emit(l, offset)

    def arm(self):
        try:
            self.scopetype.arm()
        except Exception:
            self.dis()
            raise

    def capture(self, update=True, NumberPoints=None):
        """Raises IOError if unknown failure, returns 'True' if successful, 'False' if timeout"""
        return self.scopetype.capture(update, NumberPoints)

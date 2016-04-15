#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2014, Colin O'Flynn <coflynn@newae.com>
# All rights reserved.
#
# Find this and more at colinoflynn.com - this file is part of the chipwhisperer
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

import time
import numpy as np
from matplotlib.mlab import find
from _base import AuxiliaryTemplate


try:
    from picoscope import ps5000a
except ImportError:
    ps5000a = None


class FreqMeasure():
    
    def __init__(self, ps):
        self.ps = ps
        self.isOpen = False
    
    def openScope(self):
        if self.isOpen == False:
            self.ps.open()

        self.isOpen = True

        self.ps.setChannel("A", coupling="DC", VRange=5.0, probeAttenuation=10)
        self.ps.setChannel("B", enabled=False)
        self.ps.setChannel("C", enabled=False)
        self.ps.setChannel("D", enabled=False)
        res = self.ps.setSamplingFrequency(1000E6, 50000)
        self.sampleRate = res[0]
        print "Sampling @ %f MHz, %d samples" % (res[0] / 1E6, res[1])

        # Use external trigger to mark when we sample
        self.ps.setSimpleTrigger(trigSrc="External", threshold_V=0.150, timeout_ms=5000)

    def closeScope(self):
        self.ps.close()
        self.isOpen = False

    def armMeasure(self):
        self.ps.runBlock()

    def freq_from_crossings(self, sig):
        """Estimate frequency by counting zero crossings"""
        # From https://gist.github.com/endolith/255291:

        fs = self.sampleRate

        # Find all indices right before a rising-edge zero crossing
        indices = find((sig[1:] >= 0) & (sig[:-1] < 0))
        # More accurate, using linear interpolation to find intersample
        # zero-crossings (Measures 1000.000129 Hz for 1000 Hz, for instance)
        crossings = [i - sig[i] / (sig[i + 1] - sig[i]) for i in indices]
        # Some other interpolation based on neighboring points might be better. Spline, cubic, whatever
        return fs / np.mean(np.diff(crossings))

    def measure(self):
        # print "Waiting for trigger"
        while(self.ps.isReady() == False): time.sleep(0.01)
        # print "Sampling Done"

        data = self.ps.getDataV("A", 50000)
        data = data - np.mean(data)
        freq = self.freq_from_crossings(data)

        print "Aux: Frequency Measurement Done, %d Hz" % freq

        return freq


class FrequencyMeasure(AuxiliaryTemplate):
    name = "Frequency Counter"

    def setupParameters(self):
        scopes = {"None":None}
        if ps5000a is not None:
            scopes["PicoScope 5000A"] = ps5000a.PS5000a(connect=False)

        return [{'name':'Device', 'type':'list', 'key':'device', 'values':scopes, 'value':"None", 'set':self.setConnection}]

    def setConnection(self, con):
        self.fm = FreqMeasure(con)

    def captureInit(self):
        self.fm.openScope()
        self.data = []
        
    def close(self):
        self.fm.closeScope()
        
    def traceArm(self):
        self.fm.armMeasure()

    def traceDone(self):
        freq = self.fm.measure()
        self.data.append(freq)

    def captureComplete(self):
        np.save("frequency-%s.npy" % self.prefix, self.data)

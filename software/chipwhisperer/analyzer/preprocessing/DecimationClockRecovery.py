#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2013-2014, NewAE Technology Inc
# All rights reserved.
#
# Author: Colin O'Flynn
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

import sys

try:
    from PySide.QtCore import *
    from PySide.QtGui import *
except ImportError:
    print "ERROR: PySide is required for this program"
    sys.exit()

from chipwhisperer.analyzer.preprocessing.PreprocessingBase import PreprocessingBase
from openadc.ExtendedParameter import ExtendedParameter
from pyqtgraph.parametertree import Parameter

# from functools import partial
import scipy as sp
import scipy.fftpack
import numpy as np

from matplotlib.mlab import find
        
def fft(signal, freq=None):
    FFT = abs(scipy.fft(signal))
    # FFTdb = 20*scipy.log10(FFT)
    # freqs = scipy.fftpack.fftfreq(len(signal), 1/freq)

    # FFTdb = FFTdb[2:len(freqs)/2]
    # freqs = freqs[2:len(freqs)/2]
    # return (freqs, FFTdb)

    print FFT

class DecimationClockRecovery(PreprocessingBase):
    """
    Attempts Clock recovery & then decimates based on that
    """

    descrString = "Attempts to 'recover' the clock by band-pass filtering, and then uses that to "\
                  "decimate to only points of interest.\n****CURRENTLY NOT SUPPORTED****"
     
    def setupParameters(self):

        resultsParams = [{'name':'Enabled', 'type':'bool', 'value':True, 'set':self.setEnabled},
                         {'name':'Filter Design', 'type':'group', 'children':[
                                {'name':'Form', 'key':'form', 'type':'list', 'values':{"Butterworth":sp.signal.butter}, 'set':self.updateFilter},
                                {'name':'Type', 'key':'type', 'type':'list', 'values':["bandpass"], 'value':'bandpass', 'set':self.updateFilter},
                                #{'name':'Critical Freq #1 (0-1)', 'key':'freq1', 'type':'float', 'limits':(0, 1), 'step':0.05, 'value':0.035, 'set':self.updateFilter},
                                #{'name':'Critical Freq #2 (0-1)', 'key':'freq2', 'type':'float', 'limits':(0, 1), 'step':0.05, 'value':0.055, 'set':self.updateFilter},
                                {'name':'Critical Freq BW (%)', 'key':'freqbw', 'type':'float', 'limits':(0, 200), 'step':1, 'value':20, 'set':self.updateFilter},
                                {'name':'Recalc Passband/Trace', 'key':'recalcpertrace', 'type':'bool', 'value':False},
                                {'name':'Order', 'key':'order', 'type':'int', 'limits':(1, 32), 'value':3, 'set':self.updateFilter}, ]},
                         {'name':'Enable Zero-Crossing', 'key':'enableZero', 'type':'bool', 'value':True},
                         {'name':'Enable Decimation by ZC', 'key':'decimate', 'type':'bool', 'value':True},
                      ]
        
        self.params = Parameter.create(name='Clock Decimation', type='group', children=resultsParams)
        ExtendedParameter.setupExtended(self.params, self)

        self.updateFilter()
   
    def updateFilter(self, param1=None, tnum=0):
        filt = self.findParam('form').value()
        N = self.findParam('order').value()
        ftype = self.findParam('type').value()
        
        # freq1 = self.findParam('freq1').value()
        # freq2 = self.findParam('freq2').value()

        # if ftype == "bandpass":
        #    self.findParam('freq2').show()
        #    freq = [freq1, freq2]
        # else:
        #    self.findParam('freq2').hide()
        #    freq = freq1
        
        freqbw = self.findParam('freqbw').value() / 100

        try:
            fftdata = scipy.fft(self.trace.getTrace(tnum))
            fftdata = abs(fftdata[2:(len(fftdata) / 2)])
            maxindx = fftdata.argmax() + 2
            centerfreq = float(maxindx) / float(len(fftdata) + 2)

            # centerfreq in range 0 - 1 now

        except:
            print "FFT FAILED!!!"
            self.b = None
            self.a = None
            return

        freq = ((centerfreq - (centerfreq * freqbw) / 2), (centerfreq + (centerfreq * freqbw) / 2))

        print "Designing filter for passband: %f-%f" % (freq[0], freq[1])

        b, a = filt(N, freq, ftype)
        self.b = b
        self.a = a
   
    def getTrace(self, n):
        if self.enabled:
            trace = self.trace.getTrace(n)
            if trace is None:
                return None
            
            if self.a is None:
                self.updateFilter()
                
            if self.findParam('recalcpertrace').value():
                self.updateFilter(tnum=n)

            # Filter Trace
            inputtrace = sp.signal.lfilter(self.b, self.a, trace)
            
            if self.findParam('enableZero').value():

                # Problem: the filter has some start-up time, so we can't use data right away. But
                #         if you only capture a waveform AFTER the trigger, this means you need to
                #         throw away data from the capture until the filter is running. If the system
                #         clock frequency changes, you've now thrown away a differing amount of clock
                #         cycles and syncronization is lost.
                #
                # For now we just throw away a fixed number of samples, but it doesn't work for varying
                # clock frequencies.
                for i in range(0, 250):
                    inputtrace[i] = 0

                indices = find((inputtrace[1:] >= 0) & (inputtrace[:-1] < 0))
                filttrace = np.zeros(len(inputtrace))

                if self.findParam('decimate').value():
                    for idx, val in enumerate(indices):
                        filttrace[idx] = trace[val]
                else:
                    for i in indices:
                        filttrace[i] = 1
            else:
                filttrace = inputtrace
                    


            #print len(trace)
            #print len(filttrace)
            
            return filttrace
           
            
        else:
            return self.trace.getTrace(n)       
    
   

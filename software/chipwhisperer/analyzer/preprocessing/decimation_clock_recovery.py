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
import logging

from ._base import PreprocessingBase
import scipy # type: ignore
import scipy.signal # type: ignore
import scipy.fftpack # type: ignore
import numpy as np
from matplotlib.mlab import find # type: ignore


def fft(signal, freq=None):
    FFT = abs(scipy.fft(signal))
    # FFTdb = 20*scipy.log10(FFT)
    # freqs = scipy.fftpack.fftfreq(len(signal), 1/freq)

    # FFTdb = FFTdb[2:len(freqs)/2]
    # freqs = freqs[2:len(freqs)/2]
    # return (freqs, FFTdb)

    print(FFT)


class DecimationClockRecovery(PreprocessingBase):
    """
    Attempts Clock recovery & then decimates based on that. Currently broken.
    """
    _name = "Decimation: Clock Recovery"
    _description = "Attempts to 'recover' the clock by band-pass filtering, and then uses that to "\
                  "decimate to only points of interest.\n****CURRENTLY NOT SUPPORTED****"

    def __init__(self, traceSource=None, name=None):
        PreprocessingBase.__init__(self, traceSource, name=name)
        self.setFilterOptions()
        self.getParams().addChildren([
            {'name':'Filter Design', 'type':'group', 'children':[
                # {'name':'Form', 'key':'form', 'type':'list', 'values':{"Butterworth":sp.signal.butter}, 'set':self.updateScript},
                {'name':'Type', 'key':'type', 'type':'list', 'values':["bandpass"], 'default':'bandpass', 'value':'bandpass'},
                {'name':'Critical Freq BW (%)', 'key':'freqbw', 'type':'float', 'limits':(0, 200), 'step':1, 'default':20, 'value':20},
                {'name':'Recalc Passband/Trace', 'key':'recalcpertrace', 'type':'bool', 'default':False, 'value':False},
                {'name':'Order', 'key':'order', 'type':'int', 'limits':(1, 32), 'default':3, 'value':3},
             ]},
            {'name':'Enable Zero-Crossing', 'key':'enableZero', 'type':'bool', 'default':True, 'value':True},
            {'name':'Enable Decimation by ZC', 'key':'decimate', 'type':'bool', 'default':True, 'value':True},
        ])
   
    def setFilterOptions(self, recalcPerTrace=False, enableZC=True, enableDecimation=True):
        self._recalcPerTrace = recalcPerTrace
        self._enableZeroCrossing = enableZC
        self._enableDecimation = enableDecimation
   
    def setFilterParams(self, form='low', freqbw=0.15, order=5, tnum=0, useCached=False):
        if useCached:
            freqbw = self._freqbw
            form = self._form
            order = self._order
        else:
            self._freqbw = freqbw
            self._form = form
            self._order = order

        try:
            fftdata = scipy.fft(self._traceSource.getTrace(tnum))
            fftdata = abs(fftdata[2:(len(fftdata) / 2)])
            maxindx = fftdata.argmax() + 2
            centerfreq = float(maxindx) / float(len(fftdata) + 2)
            # centerfreq in range 0 - 1 now

        except:
#            print "FFT FAILED!!!"
            self.b = None
            self.a = None
            return

        freq = ((centerfreq - (centerfreq * freqbw) / 2), (centerfreq + (centerfreq * freqbw) / 2))

        logging.info('Designing filter for passband: %f-%f' % (freq[0], freq[1]))

        b, a = scipy.signal.butter(order, freq, form)
        self.b = b
        self.a = a
   
    def getTrace(self, n):
        if self.enabled:
            trace = self._traceSource.getTrace(n)
            if trace is None:
                return None
            
            if self.a is None:
                self.setFilterParams(useCached=True)
                
            if self._recalcPerTrace:
                self.setFilterParams(tnum=n, useCached=True)

            # Filter Trace
            inputtrace = scipy.signal.lfilter(self.b, self.a, trace)
            
            if self._enableZeroCrossing:

                # Problem: the filter has some start-up time, so we can't use data right away. But
                #         if you only api a waveform AFTER the trigger, this means you need to
                #         throw away data from the api until the filter is running. If the system
                #         clock frequency changes, you've now thrown away a differing amount of clock
                #         cycles and syncronization is lost.
                #
                # For now we just throw away a fixed number of samples, but it doesn't work for varying
                # clock frequencies.
                for i in range(0, 250):
                    inputtrace[i] = 0

                indices = find((inputtrace[1:] >= 0) & (inputtrace[:-1] < 0))
                filttrace = np.zeros(len(inputtrace))

                if self._enableDecimation:
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
            return self._traceSource.getTrace(n)

    def getSampleRate(self):
        return 0  # TODO: it is not zero!

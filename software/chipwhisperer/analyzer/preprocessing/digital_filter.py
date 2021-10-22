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
#
#    Date         Author                Changes
#    -----------  --------------------  -----------------------------------
#    20-Jun-2016  marc                  Added "bandstop" filter option

from ._base import PreprocessingBase
from scipy import signal # type: ignore
from chipwhisperer.common.utils.parameter import setupSetParam

class Filter(PreprocessingBase):
    """
    Generic filter, pulls in from SciPy for doing the actual filtering of things
    """
    _name = "Digital Filter"
    _description = "Frequency specific filter."

    valid_types = ["low", "high", "bandpass", "bandstop"]

    def __init__(self, traceSource=None, name=None):
        PreprocessingBase.__init__(self, traceSource, name=name)

        self._type = "low"
        self._freq1 = 0.1
        self._freq2 = 0.8
        self._order = 5

        self.getParams().addChildren([
            {'name':'Form', 'key':'form', 'type':'list', 'values':{"Butterworth":"sp.signal.butter"}, 'default':"sp.signal.butter", 'value':"sp.signal.butter"},
            {'name':'Type', 'key':'type', 'type':'list', 'values':self.valid_types, 'get':self._getType, 'set':self._setType},
            {'name':'Critical Freq #1 (0-1)', 'key':'freq1', 'type':'float', 'limits':(0, 1), 'step':0.05, 'get':self._getFreq1, 'set':self._setFreq1},
            {'name':'Critical Freq #2 (0-1)', 'key':'freq2', 'type':'float', 'limits':(0, 1), 'step':0.05, 'get':self._getFreq2, 'set':self._setFreq2},
            {'name':'Order', 'key':'order', 'type':'int', 'limits':(1, 32), 'get':self._getOrder, 'set':self._setOrder},
        ])
        self._updateFilterParams()

    @property
    def form(self):
        """The filter implementation. Only supported option is Butterworth, so
        this parameter is read-only.
        """
        return "butterworth"

    def _getType(self):
        return self._type

    @setupSetParam("Type")
    def _setType(self, type):
        self._type = type
        self._updateFilterParams()

    @property
    def type(self):
        """The type of filter. Options are:
        - "low": Low-pass filter (pass signals below critical freq)
        - "high": High-pass filter (pass signals above critical freq)
        - "bandpass": Band-pass filter (pass signals between critical freqs)
        - "bandstop": Band-stop (or notch) filter (stop signals between freqs)

        Setter raises ValueError if filter type isn't one of these
        """
        return self._getType()

    @type.setter
    def type(self, type):
        if type not in self.valid_types:
            raise ValueError("Unrecognized filter mode; expected one of %s" % self.valid_types, type)
        self._setType(type)

    def _getFreq1(self):
        return self._freq1

    @setupSetParam("Critical Freq #1 (0-1)")
    def _setFreq1(self, freq):
        self._freq1 = freq
        self._updateFilterParams()

    @property
    def freq1(self):
        """The first critical frequency as a float in the range [0, 1].

        This is the sole cutoff frequency for low/high pass filters and the
        bottom frequency for bandpass/bandstop filters.

        Setter raises TypeError if value not a float.
        """
        return self._getFreq1()

    @freq1.setter
    def freq1(self, freq):
        if not isinstance(freq, float):
            raise TypeError("Expected float; got %s" % type(freq), freq)
        self._setFreq1(freq)

    def _getFreq2(self):
        return self._freq2

    @setupSetParam("Critical Freq #2 (0-1)")
    def _setFreq2(self, freq):
        self._freq2 = freq
        self._updateFilterParams()

    @property
    def freq2(self):
        """The second critical frequency as a float in the range [0, 1].

        For bandpass/bandstop filters, this is the upper critical frequency.
        For low/high pass filters, this value is not used.

        Setter raises TypeError if value not a float.
        """
        return self._getFreq2()

    @freq2.setter
    def freq2(self, freq):
        if not isinstance(freq, float):
            raise TypeError("Expected float; got %s" % type(freq), freq)
        self._setFreq2(freq)

    def _getOrder(self):
        return self._order

    @setupSetParam("Order")
    def _setOrder(self, order):
        self._order = order
        self._updateFilterParams()

    @property
    def order(self):
        """The order of the digital filter.

        Setter raises TypeError if value not an integer.
        """
        return self._getOrder()

    @order.setter
    def order(self, order):
        if not isinstance(order, int):
            raise TypeError("Expected int; got %s" % type(order), order)
        self._setOrder(order)

    def _setFilterForm(self, filtform=signal.butter):
        """Set the filter type in object"""
        self.filterForm = filtform

    def _setFilterParams(self, form='low', freq=0.8, order=5):
        self.b, self.a = self.filterForm(order, freq, form)

    def _updateFilterParams(self):
        if self._type in ("bandpass", "bandstop"):
            freqs = (self._freq1, self._freq2)
        else:
            freqs = self._freq1
        self._setFilterForm()
        self._setFilterParams(self._type, freqs, self._order)

    def getTrace(self, n):
        if self.enabled:
            trace = self._traceSource.getTrace(n)
            if trace is None:
                return None
            return signal.lfilter(self.b, self.a, trace)
        else:
            return self._traceSource.getTrace(n)

#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2017, NewAE Technology Inc
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
import numpy as np

from ._base import PreprocessingBase
from chipwhisperer.common.api import CWCoreAPI

class CacheTraces(PreprocessingBase):
    """
    Automatically Caches Traces to Speed Up Preprocessing
    """
    _name = "Trace Cache"
    _description = "Caches traces into giant file."

    def __init__(self, traceSource=None):

        #TODO: Where is project file?
        self.trace_cache_file = CWCoreAPI.CWCoreAPI.getInstance().project().getDataFilepath("tempcache.npy")['abs']
        self.tracehash_cache_file = CWCoreAPI.CWCoreAPI.getInstance().project().getDataFilepath("tempcachehash.npy")['abs']
        self.cache_shape = (-1, -1)

        PreprocessingBase.__init__(self, traceSource)
        self.updateScript()

    def recalculateHash(self):
        parent = self
        settinghash = 0
        while hasattr(parent, "_traceSource"):
            parent = parent._traceSource
            #print "Parent: %s" % str(parent)

            if hasattr(parent, "attrSettings"):
                #print "   params: %s"%str(parent.attrSettings())
                #Hash?
                settinghash ^= hash(str(parent.attrSettings()))
            else:
                #print "Found trace manager"
                pass

        return settinghash

    def processTraces(self):
        #Settings changed - has hash changed?
        self.settingshash =  self.recalculateHash()
       #print self.settingshash

        #Get number of trace + points, make cache now
        if self.cache_shape != (self.numTraces(), self.numPoints()):
            self.trace_cache = np.memmap(self.trace_cache_file, dtype="float32", mode="r+", shape=(self.numTraces(), self.numPoints()))
            self.tracehash_cache = np.memmap(self.tracehash_cache_file, dtype="int64", mode="r+", shape=(self.numTraces(), 1))

    def getTrace(self, n):
        """Get trace number n"""
        if self.enabled and self.settingshash:
            #Check the hash list?
            if self.tracehash_cache[n] == self.settingshash:
                trace = self.trace_cache[n]
                #print "Trace %d: cache hit"%n
            else:
                #print "Trace %d: cache miss"%n
                trace = self._traceSource.getTrace(n)
                self.tracehash_cache[n] = self.settingshash
                self.trace_cache[n] = trace
            return trace
        else:
            return self._traceSource.getTrace(n)

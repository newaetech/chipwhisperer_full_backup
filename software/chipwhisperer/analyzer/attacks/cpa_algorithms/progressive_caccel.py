#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2013-2014, NewAE Technology Inc
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

import numpy as np
import os
import sys
from ctypes import *

from ..algorithmsbase import AlgorithmsBase


class aesmodel_setup_t(Structure):

    TARGET_SBOXOUT_HW = 1
    TARGET_TARGET_INVSBOX_LASTROUND_HD = 2

    _fields_ = [("bnum", c_uint),
                ("roundkeys", POINTER(c_uint8)),
                ("rtarget", c_uint),
                ("leakagemode", c_uint),
                ("privatedata", c_void_p),
                ("privatedatasize", c_uint)
                ]
            
    def __init__(self, bnum=0):  
        super(aesmodel_setup_t, self).__init__()
        self.bnum = c_uint(bnum)
        self.rtarget = c_uint(0)
        self.privatedatasize = c_uint(0)
        self.leakagemode = c_uint(self.TARGET_SBOXOUT_HW)


class analysis_state_t(Structure):
    _fields_=[("sumhq",POINTER(c_double)),
              ("sumtq",POINTER(c_double)),
            ("sumt",POINTER(c_double)),
            ("sumh",POINTER(c_double)),
            ("sumht",POINTER(c_double)),
            ("totalTraces",c_int),
            ("hyp",POINTER(c_double))]
            
    def __init__(self, npoint=0, ntrace=0):  
        super(analysis_state_t,self).__init__()
      
        nguess = 256
      
        self._sumhq = np.zeros(nguess, dtype=np.float64)
        self.sumhq = self._sumhq.ctypes.data_as(POINTER(c_double))
      
        self._sumtq = np.zeros(npoint, dtype=np.float64)
        self.sumtq = self._sumtq.ctypes.data_as(POINTER(c_double))
      
        self._sumt= np.zeros(npoint, dtype=np.float64)
        self.sumt = self._sumt.ctypes.data_as(POINTER(c_double))
      
        self._sumh = np.zeros(nguess, dtype=np.float64)
        self.sumh = self._sumh.ctypes.data_as(POINTER(c_double))
      
      
        self._sumht = np.zeros((nguess, npoint), dtype=np.float64)
        self.sumht = self._sumht.ctypes.data_as(POINTER(c_double))
      
        self.totalTraces = c_int(0)

        self._hyp = np.zeros(ntrace, dtype=np.float64)
        self.hyp = self._hyp.ctypes.data_as(POINTER(c_double))


c_analysis_state_t_ptr = POINTER(analysis_state_t)
c_aesmodel_setup_t_ptr = POINTER(aesmodel_setup_t)


class CPAProgressiveOneSubkey(object):
    """This class is the basic progressive CPA attack, capable of adding traces onto a variable with previous data"""
    def __init__(self):
        self.clearStats()
        dir = os.path.dirname(__file__)
        
        #Determine correct library to load for 64-bit vs. 32-bit

        is_64bits = sys.maxsize > 2 ** 32

        if os.name == 'nt':
            if is_64bits:
                libname = 'libcpa_x64.dll'
            else:
                libname = 'libcpa.dll'
        elif os.name == 'posix':
            if is_64bits:
                libname = 'libcpa_x64.so'
            else:
                libname = 'libcpa.so'

        libname = os.path.join(dir, 'c_accel/%s' % libname)
        try:
            dll = CDLL(libname)
        except Exception:
            raise Exception("Could not import library file. Compile it for your platform first (there is a makefile for that): " + libname)

        self.osk = dll.oneSubkey
        self.modelstate = {'knownkey':None}

    def clearStats(self):
        self.anstate = None

    def oneSubkey(self, bnum, pointRange, traces_all, numtraces, plaintexts, ciphertexts, knownkeys, progressBar, model, state, pbcnt):

        if pointRange == None:
            traces = traces_all
            pointstart = 0
        else:
            # traces = np.array(traces_all[:, pointRange[0] : pointRange[1]])
            traces = traces_all[:, pointRange[0] : pointRange[1]]
            pointstart = pointRange[0]


        if (pointRange[0] != 0):
            raise NotImplementedError("C-Accel only works with full point range on current version.")

        #TODO: Captured traces seem to have extra point, this is temp fix as need to figure out why this is
        #      happening!?
        npoints = np.shape(traces)[1]+1

        if self.anstate is None:
            self.anstate = analysis_state_t(npoints, numtraces)
            
        mstate = aesmodel_setup_t(bnum=bnum)
        
        guessdata = np.zeros((model.getPermPerSubkey(), npoints-pointstart), dtype=np.float64)

        if hasattr(model.getHwModel(), 'c_model_enum_value'):
            mstate.leakagemode = model.getHwModel().c_model_enum_value
        else:
            mstate.leakagemode = model.getHwModel()

        self.osk(traces.ctypes.data_as(POINTER(c_double)),
                 plaintexts.ctypes.data_as(POINTER(c_uint8)),
                 ciphertexts.ctypes.data_as(POINTER(c_uint8)),
                 c_size_t(len(traces)),
                 c_size_t(npoints),
                 c_size_t(0),
                 c_size_t(numtraces),
                 c_size_t(pointstart),
                 c_size_t(npoints),
                 c_analysis_state_t_ptr(self.anstate),
                 c_void_p(0),
                c_aesmodel_setup_t_ptr(mstate),
                 guessdata.ctypes.data_as(POINTER(c_double)))
      
        if progressBar:
            progressBar.updateStatus(pbcnt, (self.anstate.totalTraces - numtraces, self.anstate.totalTraces-1, bnum))

        pbcnt = pbcnt + model.getPermPerSubkey()

        return (guessdata, pbcnt)


class CPAProgressive_CAccel(AlgorithmsBase):
    """
    CPA Attack done as a loop, but using an algorithm which can progressively add traces & give output stats
    """
    _name = "Progressive-C Accel"

    def __init__(self):
        AlgorithmsBase.__init__(self)

        self.getParams().addChildren([
            {'name':'Iteration Mode', 'key':'itmode', 'type':'list', 'values':{'Depth-First':'df', 'Breadth-First':'bf'}, 'value':'bf'},
            {'name':'Skip when PGE=0', 'key':'checkpge', 'type':'bool', 'value':False},
        ])
        self.updateScript()


    def addTraces(self, traceSource, tracerange, progressBar=None, pointRange=None):
        brange=self.brange

        numtraces = tracerange[1] - tracerange[0]

        if progressBar:
            progressBar.setText("Attacking traces: from %d to %d (total = %d)" % (tracerange[0], tracerange[1], numtraces))
            progressBar.setStatusMask("Trace Interval: %d-%d. Current Subkey: %d", (0,0,0))
            progressBar.setMaximum(len(brange) * self.model.getPermPerSubkey() * (numtraces / self._reportingInterval + 1))
        pbcnt = 0
        #r = Parallel(n_jobs=4)(delayed(traceOneSubkey)(bnum, pointRange, traces_all, numtraces, plaintexts, ciphertexts, keyround, modeltype, progressBar, self.model, pbcnt) for bnum in brange)
        #self.all_diffs, pb = zip(*r)
        #pbcnt = 0
        cpa = [None]*(max(brange)+1)
        for bnum in brange:
            cpa[bnum] = CPAProgressiveOneSubkey()

        brangeMap = [None]*(max(brange)+1)
        i = 1
        for bnum in brange:
            brangeMap[bnum] = i
            i += 1

        skipPGE = False  # self.findParam('checkpge').getValue()
        bf = True  # self.findParam('itmode').getValue() == 'bf'

        #bf specifies a 'breadth-first' search. bf means we search across each
        #subkey by only the amount of traces specified. Depth-First means we
        #search each subkey completely, then move onto the next.
        if bf:
            brange_df = [0]
            brange_bf = brange
        else:
            brange_bf = [0]
            brange_df = brange


        for bnum_df in brange_df:

            tstart = 0
            tend = self._reportingInterval

            while tstart < numtraces:
                if tend > numtraces:
                    tend = numtraces

                if tstart > numtraces:
                    tstart = numtraces

                data = []
                textins = []
                textouts = []
                knownkeys = []
                for i in range(tstart, tend):
                    # Handle Offset
                    tnum = i + tracerange[0]

                    d = traceSource.getTrace(tnum)

                    if d is None:
                        continue

                    data.append(d)
                    textins.append(traceSource.getTextin(tnum))
                    textouts.append(traceSource.getTextout(tnum))
                    knownkeys.append(traceSource.getKnownKey(tnum))

                traces = np.array(data)
                textins = np.array(textins)
                textouts = np.array(textouts)

                for bnum_bf in brange_bf:

                    if bf:
                        bnum = bnum_bf
                    else:
                        bnum = bnum_df


                    skip = False
                    if (self.stats.simplePGE(bnum) != 0) or (skipPGE == False):
                        if isinstance(pointRange, list):
                            bptrange = pointRange[bnum]
                        else:
                            bptrange = pointRange
                        (data, pbcnt) = cpa[bnum].oneSubkey(bnum, bptrange, traces, tend - tstart, textins, textouts, knownkeys, progressBar, self.model, cpa[bnum].modelstate, pbcnt)
                        self.stats.updateSubkey(bnum, data, tnum=tend)
                    else:
                        skip = True

                    if skip:
                        pbcnt = brangeMap[bnum] * self.model.getPermPerSubkey() * (numtraces / self._reportingInterval + 1)

                        if bf is False:
                            tstart = numtraces

                tend += self._reportingInterval
                tstart += self._reportingInterval

                if self.sr is not None:
                    self.sr()

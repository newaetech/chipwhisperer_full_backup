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
import math

from ..algorithmsbase import AlgorithmsBase
from chipwhisperer.logging import *


class CPAProgressiveOneSubkey(object):
    """This class is the basic progressive CPA attack, capable of adding traces onto a variable with previous data"""
    def __init__(self, model):
        self.model = model
        self.sumhq = [0] * self.model.getPermPerSubkey()
        self.sumtq = [0]
        self.sumt = [0]
        self.sumh = [0] * self.model.getPermPerSubkey()
        self.sumht = [0] * self.model.getPermPerSubkey()
        self.totalTraces = 0
        self.modelstate = {'knownkey':None}

    def oneSubkey(self, bnum, pointRange, traces_all, numtraces, plaintexts, ciphertexts, knownkeys, progressBar, state, pbcnt):
        diffs = [0] * self.model.getPermPerSubkey()
        self.totalTraces += numtraces

        if pointRange == None:
            traces = traces_all
            # padbefore = 0
            # padafter = 0
        else:
            traces = traces_all[:, pointRange[0] : pointRange[1]]
            # padbefore = pointRange[0]
            # padafter = len(traces_all[0, :]) - pointRange[1]
            # print "%d - %d (%d %d)" % (pointRange[0], pointRange[1], padbefore, padafter)

        self.sumtq += np.sum(np.square(traces), axis=0, dtype=np.longdouble)
        self.sumt += np.sum(traces, axis=0, dtype=np.longdouble)
        sumden2 = np.square(self.sumt) - self.totalTraces * self.sumtq

        #For each 0..0xFF possible value of the key byte
        for key in range(0, self.model.getPermPerSubkey()):
            #Initialize arrays & variables to zero
            sumnum = np.zeros(len(traces[0,:]))
            # sumden1 = np.zeros(len(traces[0,:]))
            # sumden2 = np.zeros(len(traces[0,:]))

            hyp = [0] * numtraces

            #Formula for CPA & description found in "Power Analysis Attacks"
            # by Mangard et al, page 124, formula 6.2.
            #
            # This has been modified to reduce computational requirements such that adding a new waveform
            # doesn't require you to recalculate everything

            prev_cts = np.insert(ciphertexts[:-1], 0, 0, axis=0)
            prev_pts = np.insert(plaintexts[:-1], 0, 0, axis=0)
            #Generate hypotheticals
            for tnum in range(numtraces):

                if len(plaintexts) > 0:
                    pt = plaintexts[tnum]

                if len(ciphertexts) > 0:
                    ct = ciphertexts[tnum]

                if len(prev_cts) > 0:
                    prev_ct = prev_cts[tnum]

                if len(prev_pts) > 0:
                    prev_pt = prev_pts[tnum]


                if knownkeys and len(knownkeys) > 0:
                    nk = knownkeys[tnum]
                else:
                    nk = None

                state['knownkey'] = nk

                if self.model._has_prev:
                    hypint = self.model.leakage(pt, ct, prev_pt, prev_ct, key, bnum, state)
                else:
                    hypint = self.model.leakage(pt, ct, key, bnum, state)

                hyp[tnum] = hypint

            hyp = np.array(hyp)

            self.sumh[key] += np.sum(hyp, axis=0, dtype=np.longdouble)
            self.sumht[key] += np.sum(np.multiply(np.transpose(traces), hyp), axis=1, dtype=np.longdouble)

            #WARNING: not casting to np.float64 causes algorithm degredation... always be careful
            #meanh = self.sumh[key] / np.float64(self.totalTraces)
            #meant = self.sumt[key] / np.float64(self.totalTraces)

            #numtraces * meanh * meant = sumh * meant
            #sumnum =  self.sumht[key] - meant*self.sumh[key] - meanh*self.sumt[key] + (self.sumh[key] * meant)
            #sumnum =  self.sumht[key] - meanh*self.sumt[key]
#            sumnum =  self.sumht[key] - meanh*self.sumt[key]
            #sumnum =  self.sumht[key] - self.sumh[key]*self.sumt[key] / np.float64(self.totalTraces)
            sumnum = self.totalTraces * self.sumht[key] - self.sumh[key] * self.sumt

            self.sumhq[key] += np.sum(np.square(hyp),axis=0, dtype=np.longdouble)

            #numtraces * meanh * meanh = sumh * meanh
            #sumden1 = sumhq - (2*meanh*self.sumh) + (numtraces*meanh*meanh)
            #sumden1 = sumhq - (2*meanh*self.sumh) + (self.sumh * meanh)
            # sumden1 = sumhq - meanh*self.sumh
            # similarly for sumden2
            #sumden1 = self.sumhq[key] - meanh*self.sumh[key]
            #sumden2 = self.sumtq[key] - meant*self.sumt[key]
            # sumden = sumden1 * sumden2

            #Sumden1/Sumden2 are variance of these variables, may be numeric unstability
            #See http://en.wikipedia.org/wiki/Algorithms_for_calculating_variance for online update
            #algorithm which might be better
            sumden1 = (np.square(self.sumh[key]) - self.totalTraces * self.sumhq[key])

            sumden = sumden1 * sumden2

            if ((key == 0x2B) and (bnum == 0)):
                other_logger.info("sumden1: {}".format(sumden1))

            #if sumden.any() < 1E-12:
            #    print "WARNING: sumden small"

            diffs[key] = sumnum / np.sqrt(sumden)

            if progressBar:
                progressBar.updateStatus(pbcnt, (self.totalTraces-numtraces, self.totalTraces-1, bnum))
            pbcnt = pbcnt + 1

            # if padafter > 0:
            #    diffs[key] = np.concatenate([diffs[key], np.zeros(padafter)])

            # if padbefore > 0:
            #    diffs[key] = np.concatenate([np.zeros(padbefore), diffs[key]])

        return (diffs, pbcnt)


class CPAProgressive(AlgorithmsBase):
    """
    CPA Attack done as a loop, but using an algorithm which can progressively add traces & give output stats
    """
    _name = "Progressive"

    def __init__(self):
        AlgorithmsBase.__init__(self)

        self.getParams().addChildren([
            {'name':'Iteration Mode', 'key':'itmode', 'type':'list', 'values':{'Depth-First':'df', 'Breadth-First':'bf'}, 'value':'bf', 'action':self.updateScript},
            {'name':'Skip when PGE=0', 'key':'checkpge', 'type':'bool', 'value':False, 'action':self.updateScript},
        ])
        self.updateScript()

    def addTraces(self, traceSource, tracerange, progressBar=None, pointRange=None):
        numtraces = tracerange[1] - tracerange[0] + 1
        if progressBar:
            progressBar.setText("Attacking traces subset: from %d to %d (total = %d)" % (tracerange[0], tracerange[1], numtraces))
            progressBar.setStatusMask("Trace Interval: %d-%d. Current Subkey: %d")
            progressBar.setMaximum(len(self.brange) * self.model.getPermPerSubkey() * math.ceil(float(numtraces) / self._reportingInterval) - 1)

        pbcnt = 0
        cpa = [None]*(max(self.brange)+1)
        for bnum in self.brange:
            cpa[bnum] = CPAProgressiveOneSubkey(self.model)

        brangeMap = [None]*(max(self.brange)+1)
        i = 1
        for bnum in self.brange:
            brangeMap[bnum] = i
            i += 1

        skipPGE = False  # self.findParam('checkpge').getValue()
        bf = True  # self.findParam('itmode').getValue() == 'bf'

        #bf specifies a 'breadth-first' search. bf means we search across each
        #subkey by only the amount of traces specified. Depth-First means we
        #search each subkey completely, then move onto the next.
        if bf:
            brange_df = [0]
            brange_bf = self.brange
        else:
            brange_bf = [0]
            brange_df = self.brange

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

                    try:
                        data.append(traceSource.get_trace(tnum))
                        textins.append(traceSource.get_textin(tnum))
                        textouts.append(traceSource.get_textout(tnum))
                        knownkeys.append(traceSource.get_known_key(tnum))
                    except Exception as e:
                        if progressBar:
                            progressBar.abort(e.message)
                        return

                traces = np.array(data)
                textins = np.array(textins)
                textouts = np.array(textouts)
                # knownkeys = np.array(knownkeys)

                for bnum_bf in brange_bf:
                    if bf:
                        bnum = bnum_bf
                    else:
                        bnum = bnum_df

                    skip = False
                    if (self.stats.simple_PGE(bnum) != 0) or (skipPGE == False):
                        bptrange = pointRange
                        (data, pbcnt) = cpa[bnum].oneSubkey(bnum, bptrange, traces, tend - tstart, textins, textouts, knownkeys, progressBar, cpa[bnum].modelstate, pbcnt)
                        self.stats.update_subkey(bnum, data, tnum=tend)
                    else:
                        skip = True

                    if skip:
                        pbcnt = brangeMap[bnum] * self.model.getPermPerSubkey() * (numtraces / self._reportingInterval + 1)

                        if bf is False:
                            tstart = numtraces

                    if progressBar and progressBar.wasAborted():
                        return

                tend += self._reportingInterval
                tstart += self._reportingInterval

                if self.sr:
                    self.sr()

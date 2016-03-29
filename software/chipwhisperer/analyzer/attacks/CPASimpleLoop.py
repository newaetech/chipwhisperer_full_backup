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
from chipwhisperer.analyzer.attacks.AttackStats import DataTypeDiffs


class CPASimpleLoop(object):
    """
    CPA Attack done as a loop - the 'classic' attack provided for familiarity to textbook samples.
    This attack does not provide trace-by-trace statistics however, you can only gather results once
    all the traces have been run through the attack.
    """

    def __init__(self, targetModel, leakageFunction):
        self.model = targetModel
        self.leakage = leakageFunction
        self.stats = DataTypeDiffs()
        self.modelstate = {'knownkey':None}

    def oneSubkey(self, bnum, pointRange, traces_all, numtraces, plaintexts, ciphertexts, knownkeys, progressBar, model, leakagetype, state, pbcnt):
        diffs = [0]*256

        if pointRange == None:
            traces = traces_all
            # padbefore = 0
            # padafter = 0
        else:
            traces = np.array(traces_all[:, pointRange[0] : pointRange[1]])
            # padbefore = pointRange[0]
            # padafter = len(traces_all[0, :]) - pointRange[1]
            # print "%d - %d (%d %d)"%( pointRange[0],  pointRange[1], padbefore, padafter)

        #For each 0..0xFF possible value of the key byte
        for key in range(0, 256):
            #Initialize arrays & variables to zero
            sumnum = np.zeros(len(traces[0,:]))
            sumden1 = np.zeros(len(traces[0,:]))
            sumden2 = np.zeros(len(traces[0,:]))

            hyp = [0] * numtraces

            #Formula for CPA & description found in "Power Analysis Attacks"
            # by Mangard et al, page 124, formula 6.2.

            #Generate hypotheticals
            for tnum in range(numtraces):

                if len(plaintexts) > 0:
                    pt = plaintexts[tnum]
                else:
                    pt = []

                if len(ciphertexts) > 0:
                    ct = ciphertexts[tnum]
                else:
                    ct = []

                if knownkeys and len(knownkeys) > 0:
                    nk = knownkeys[tnum]
                else:
                    nk = None

                state['knownkey'] = nk

                hypint = model.leakage(pt, ct, key, bnum, leakagetype, state)

                hyp[tnum] = hypint

            hyp = np.array(hyp)

            #Mean of hypothesis
            meanh = np.mean(hyp, dtype=np.float64)

            #Mean of all points in trace
            meant = np.mean(traces, axis=0, dtype=np.float64)

            #For each trace, do the following
            for tnum in range(numtraces):
                hdiff = (hyp[tnum] - meanh)
                tdiff = traces[tnum,:] - meant

                sumnum = sumnum + (hdiff*tdiff)
                sumden1 = sumden1 + hdiff * hdiff
                sumden2 = sumden2 + tdiff * tdiff

            diffs[key] = sumnum / np.sqrt( sumden1 * sumden2 )

            if progressBar:
                progressBar.updateStatus(pbcnt, (bnum, key))
                if progressBar.wasCanceled():
                    raise KeyboardInterrupt
            pbcnt = pbcnt + 1

            # if padafter > 0:
            #    diffs[key] = np.concatenate([diffs[key], np.zeros(padafter)])

            # if padbefore > 0:
            #    diffs[key] = np.concatenate([np.zeros(padbefore), diffs[key]])

        return (diffs, pbcnt)

    def setTargetBytes(self, brange):
        self.brange = brange

    def setReportingInterval(self, ri):
        # Not used for simpleloop
        pass

    def addTraces(self, tracedata, tracerange, progressBar, pointRange=None, tracesLoop=None):
        brange=self.brange

        if progressBar:
            progressBar.setMaximum(len(brange) * 256)

        self.all_diffs = range(0,16)
        numtraces = tracerange[1] - tracerange[0]

        # Load all traces
        data = []
        textins = []
        textouts = []
        knownkeys = []
        for i in range(tracerange[0], tracerange[1]):

            # Handle Offset
            tnum = i + tracerange[0]

            d = tracedata.getTrace(tnum)

            if d is None:
                continue

            data.append(d)
            textins.append(tracedata.getTextin(tnum))
            textouts.append(tracedata.getTextout(tnum))
            knownkeys.append(tracedata.getKnownKey(tnum))

        traces = np.array(data)
        textins = np.array(textins)
        textouts = np.array(textouts)

        pbcnt = 0
        for bnum in brange:
            (data, pbcnt) = self.oneSubkey(bnum, pointRange, traces, numtraces, textins, textouts, knownkeys, progressBar, self.model, self.leakage, self.modelstate, pbcnt)
            self.stats.updateSubkey(bnum, data)

    def getStatistics(self):
        return self.stats

    def setStatsReadyCallback(self, sr):
        pass

    def processKnownKey(self, inpkey):
        if hasattr(self.model, 'processKnownKey'):
            return self.model.processKnownKey(self.leakage, inpkey)
        else:
            return inpkey

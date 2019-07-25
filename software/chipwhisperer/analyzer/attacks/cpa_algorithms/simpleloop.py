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

from ..algorithmsbase import AlgorithmsBase


class CPASimpleLoop(AlgorithmsBase):
    """
    CPA Attack done as a loop - the 'classic' attack provided for familiarity to textbook samples.
    This attack does not provide trace-by-trace statistics however, you can only gather results once
    all the traces have been run through the attack.
    """
    _name = "Simple"

    def __init__(self):
        AlgorithmsBase.__init__(self)
        self.modelstate = {'knownkey':None}
        self.updateScript()

    def oneSubkey(self, bnum, pointRange, traces_all, numtraces, plaintexts, ciphertexts, knownkeys, progressBar, model, state, pbcnt):
        diffs = [0]*self.model.getPermPerSubkey()

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
        for key in range(0, self.model.getPermPerSubkey()):
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

                hypint = model.leakage(pt, ct, key, bnum, state)

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
                if progressBar.wasAborted():
                    break
            pbcnt = pbcnt + 1

            # if padafter > 0:
            #    diffs[key] = np.concatenate([diffs[key], np.zeros(padafter)])

            # if padbefore > 0:
            #    diffs[key] = np.concatenate([np.zeros(padbefore), diffs[key]])

        return (diffs, pbcnt)

    def addTraces(self, traceSource, tracerange, progressBar=None, pointRange=None, tracesLoop=None):
        brange=self.brange
        numtraces = tracerange[1] - tracerange[0] + 1

        if progressBar:
            progressBar.setText("Attacking traces: from %d to %d (total = %d)" % (tracerange[0], tracerange[1], numtraces))
            progressBar.setMaximum(len(brange) * self.model.getPermPerSubkey())

        # Load all traces
        data = []
        textins = []
        textouts = []
        knownkeys = []
        for i in range(tracerange[0], tracerange[1]+1):
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

        pbcnt = 0
        for bnum in brange:
            if progressBar:
                progressBar.setStatusMask("Current Subkey: %d", bnum)
            (data, pbcnt) = self.oneSubkey(bnum, pointRange, traces, numtraces, textins, textouts, knownkeys, progressBar, self.model, self.modelstate, pbcnt)
            self.stats.updateSubkey(bnum, data, tnum=tracerange[1])
            if self.sr:
                self.sr()
            if progressBar:
                progressBar.updateStatus(pbcnt, bnum)

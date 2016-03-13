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

from PySide.QtCore import *
from PySide.QtGui import *
import numpy as np
import inspect
from pyqtgraph.parametertree import Parameter

from chipwhisperer.common.api.ExtendedParameter import ExtendedParameter
from chipwhisperer.analyzer.attacks.AttackStats import DataTypeDiffs
from chipwhisperer.common.api.autoscript import AutoScript

class CPAProgressiveOneSubkey(object):
    """This class is the basic progressive CPA attack, capable of adding traces onto a variable with previous data"""
    def __init__(self):
        self.clearStats()

    def clearStats(self):
        self.sumhq = [0]*256
        self.sumtq = [0]
        self.sumt = [0]
        self.sumh = [0]*256
        self.sumht = [0]*256
        self.totalTraces = 0

        self.modelstate = {'knownkey':None}

    def oneSubkey(self, bnum, pointRange, traces_all, numtraces, plaintexts, ciphertexts, knownkeys, progressBar, model, leakagetype, state, pbcnt):

        diffs = [0]*256
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

        self.sumtq += np.sum(np.square(traces), axis=0, dtype=np.float64)
        self.sumt += np.sum(traces, axis=0)
        sumden2 = (np.square(self.sumt) - self.totalTraces * self.sumtq)

        #For each 0..0xFF possible value of the key byte
        for key in range(0, 256):
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

            #Generate hypotheticals
            for tnum in range(numtraces):

                if len(plaintexts) > 0:
                    pt = plaintexts[tnum]

                if len(ciphertexts) > 0:
                    ct = ciphertexts[tnum]

                if knownkeys and len(knownkeys) > 0:
                    nk = knownkeys[tnum]
                else:
                    nk = None

                state['knownkey'] = nk

                hypint = model.leakage(pt, ct, key, bnum, leakagetype, state)

                hyp[tnum] = hypint

            hyp = np.array(hyp)

            self.sumh[key] += np.sum(hyp, axis=0)
            self.sumht[key] += np.sum(np.multiply(np.transpose(traces), hyp), axis=1)

            #WARNING: not casting to np.float64 causes algorithm degredation... always be careful
            #meanh = self.sumh[key] / np.float64(self.totalTraces)
            #meant = self.sumt[key] / np.float64(self.totalTraces)

            #numtraces * meanh * meant = sumh * meant
            #sumnum =  self.sumht[key] - meant*self.sumh[key] - meanh*self.sumt[key] + (self.sumh[key] * meant)
            #sumnum =  self.sumht[key] - meanh*self.sumt[key]
#            sumnum =  self.sumht[key] - meanh*self.sumt[key]
            #sumnum =  self.sumht[key] - self.sumh[key]*self.sumt[key] / np.float64(self.totalTraces)
            sumnum = self.totalTraces * self.sumht[key] - self.sumh[key] * self.sumt

            self.sumhq[key] += np.sum(np.square(hyp),axis=0, dtype=np.float64)

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

            #if sumden.any() < 1E-12:
            #    print "WARNING: sumden small"


            if progressBar:
                progressBar.setValue(pbcnt)
                progressBar.updateStatus((self.totalTraces-numtraces, self.totalTraces), bnum)
                pbcnt = pbcnt + 1
                if progressBar.wasCanceled():
                    raise KeyboardInterrupt

                if progressBar.wasSkipped():
                    return (diffs, pbcnt)

            diffs[key] = sumnum / np.sqrt(sumden)

            # if padafter > 0:
            #    diffs[key] = np.concatenate([diffs[key], np.zeros(padafter)])

            # if padbefore > 0:
            #    diffs[key] = np.concatenate([np.zeros(padbefore), diffs[key]])

        return (diffs, pbcnt)

class CPAProgressive(AutoScript, QObject):
    """
    CPA Attack done as a loop, but using an algorithm which can progressively add traces & give output stats
    """
    paramListUpdated = Signal(list)

    def __init__(self, targetModel, leakageFunction, showScriptParameter=None, parent=None):
        super(CPAProgressive, self).__init__()

        resultsParams = [{'name':'Iteration Mode', 'key':'itmode', 'type':'list', 'values':{'Depth-First':'df', 'Breadth-First':'bf'}, 'value':'bf'},
                         {'name':'Skip when PGE=0', 'key':'checkpge', 'type':'bool', 'value':False},
                         ]
        self.params = Parameter.create(name='Progressive CPA', type='group', children=resultsParams)
        if showScriptParameter is not None:
            self.showScriptParameter = showScriptParameter
            # print self.showScriptParameter
        ExtendedParameter.setupExtended(self.params, self)

        self.model = targetModel
        self.leakage = leakageFunction
        self.sr = None

        self.stats = DataTypeDiffs()
        self.updateScript()

    def updateScript(self, ignored=None):
        # self.addFunction('init', 'setReportingInterval', '%d' % self.findParam('reportinterval').value())
        pass

    def paramList(self):
        return [self.params]

    def setTargetBytes(self, brange):
        self.brange = brange

    def setReportingInterval(self, ri):
        self._reportingInterval = ri

    def addTraces(self, tracedata, tracerange, progressBar=None, pointRange=None):
        brange=self.brange

        self.all_diffs = range(0,16)

        numtraces = tracerange[1] - tracerange[0]

        if progressBar:
            pbcnt = 0
            progressBar.setMinimum(0)
            progressBar.setMaximum(len(brange) * 256 * (numtraces / self._reportingInterval + 1))

        pbcnt = 0
        cpa = [None]*(max(brange)+1)
        for bnum in brange:
            cpa[bnum] = CPAProgressiveOneSubkey()

        brangeMap = [None]*(max(brange)+1)
        i = 1
        for bnum in brange:
            brangeMap[bnum] = i
            i += 1

        skipPGE = False  # self.findParam('checkpge').value()
        bf = True  # self.findParam('itmode').value() == 'bf'

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
                # knownkeys = np.array(knownkeys)

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
                        (data, pbcnt) = cpa[bnum].oneSubkey(bnum, bptrange, traces, tend - tstart, textins, textouts, knownkeys, progressBar, self.model, self.leakage, cpa[bnum].modelstate, pbcnt)
                        self.stats.updateSubkey(bnum, data, tnum=tend)
                    else:
                        skip = True

                    if progressBar.wasSkipped() or skip:
                        progressBar.clearSkipped()
                        pbcnt = brangeMap[bnum] * 256 * (numtraces / self._reportingInterval + 1)

                        if bf is False:
                            tstart = numtraces

                tend += self._reportingInterval
                tstart += self._reportingInterval

                if self.sr is not None:
                    self.sr()

    def getStatistics(self):
        return self.stats

    def setStatsReadyCallback(self, sr):
        self.sr = sr

    def processKnownKey(self, inpkey):
        if hasattr(self.model, 'processKnownKey'):
            return self.model.processKnownKey(self.leakage, inpkey)
        else:
            return inpkey

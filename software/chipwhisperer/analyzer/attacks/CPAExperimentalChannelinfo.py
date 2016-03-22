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
import sys

try:
    from PySide.QtCore import *
    from PySide.QtGui import *
except ImportError:
    print "ERROR: PySide is required for this program"
    sys.exit()

import numpy as np
import scipy as sp


from chipwhisperer.capture.api.config_parameter import ConfigParameter
from chipwhisperer.analyzer.attacks.AttackStats import DataTypeDiffs

from functools import partial

try:
    import pyximport; pyximport.install()
    import attacks.CPACython as CPACython
except ImportError:
    CPACython = None

class CPAProgressiveOneSubkey(object):
    """This class is the basic progressive CPA attack, capable of adding traces onto a variable with previous data"""
    def __init__(self):
        self.clearStats()

    def clearStats(self):
        self.sumhq = [0]*256
        self.sumtq = [0]*256
        self.sumt = [0]*256
        self.sumh = [0]*256
        self.sumht = [0]*256
        self.totalTraces = 0

    def oneSubkey(self, bnum, pointRange, traces_all, numtraces, plaintexts, ciphertexts, keyround, modeltype, progressBar, model, pbcnt):

        diffs = [0]*256
        self.totalTraces += numtraces

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
            sumnum = 0
            sumden1 = 0
            sumden2 = 0

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

                if keyround == "first":
                    ct = None
                elif keyround == "last":
                    pt = None
                else:
                    raise ValueError("keyround invalid")

                #Generate the output of the SBOX
                if modeltype == "Hamming Weight":
                    hypint = model.HypHW(pt, ct, key, bnum)
                elif modeltype == "Hamming Weight (inverse)":
                    hypint = model.HypHW(pt, ct, key, bnum)
                    hypint = 8 - hypint
                elif modeltype == "Hamming Distance":
                    hypint = model.HypHD(pt, ct, key, bnum)
                else:
                    raise ValueError("modeltype invalid")
                hyp[tnum] = hypint

            hyp = np.array(hyp)

            self.sumt[key] += np.sum(traces, axis=0)
            self.sumh[key] += np.sum(hyp, axis=0)
            self.sumht[key] += np.sum(np.multiply(traces, hyp), axis=0)

            #WARNING: not casting to np.float64 causes algorithm degredation... always be careful
            #meanh = self.sumh[key] / np.float64(self.totalTraces)
            #meant = self.sumt[key] / np.float64(self.totalTraces)

            #numtraces * meanh * meant = sumh * meant
            #sumnum =  self.sumht[key] - meant*self.sumh[key] - meanh*self.sumt[key] + (self.sumh[key] * meant)
            #sumnum =  self.sumht[key] - meanh*self.sumt[key]
#            sumnum =  self.sumht[key] - meanh*self.sumt[key]
            #sumnum =  self.sumht[key] - self.sumh[key]*self.sumt[key] / np.float64(self.totalTraces)
            sumnum = self.totalTraces*self.sumht[key] - self.sumh[key]*self.sumt[key]

            self.sumhq[key] += np.sum(np.square(hyp),axis=0, dtype=np.float64)
            self.sumtq[key] += np.sum(np.square(traces),axis=0, dtype=np.float64)

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
            sumden2 = (np.square(self.sumt[key]) - self.totalTraces * self.sumtq[key])
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

class MinDistOneSubkey(object):
    """This class is the basic progressive CPA attack, capable of adding traces onto a variable with previous data"""
    def __init__(self):
        self.clearStats()
#        self.template = np.load(r'Y:\channelestimate\july2014\A_atmega328p_1318\randplain_randkey_key0_data\analysis\templates-0-2499-csi.npz')["mean"]
        self.template = np.load(r'Y:\channelestimate\july2014\A_atmega328p_1318\randplain_fixedkey_key1_data\analysis\templates-0-2499-csi.npz')["mean"]

    def clearStats(self):
        self.diff = [0] * 256
        self.totalTraces = 0

    def oneSubkey(self, bnum, pointRange, traces_all, numtraces, plaintexts, ciphertexts, keyround, modeltype, progressBar, model, pbcnt):
        self.totalTraces += numtraces

        traces = traces_all

        # For each 0..0xFF possible value of the key byte
        for key in range(0, 256):

            # Generate hypotheticals
            for tnum in range(numtraces):

                if len(plaintexts) > 0:
                    pt = plaintexts[tnum]

                if len(ciphertexts) > 0:
                    ct = ciphertexts[tnum]

                if keyround == "first":
                    ct = None
                elif keyround == "last":
                    pt = None
                else:
                    raise ValueError("keyround invalid")

                # Generate the output of the SBOX
                if modeltype == "Hamming Weight":
                    hypint = model.HypHW(pt, ct, key, bnum);
                elif modeltype == "Hamming Weight (inverse)":
                    hypint = model.HypHW(pt, ct, key, bnum);
                    hypint = 8 - hypint
                elif modeltype == "Hamming Distance":
                    hypint = model.HypHD(pt, ct, key, bnum);
                else:
                    raise ValueError("modeltype invalid")

                self.diff[key] += -abs(traces[tnum] - self.template[bnum][hypint])

            if progressBar:
                progressBar.setValue(pbcnt)
                progressBar.updateStatus((self.totalTraces - numtraces, self.totalTraces), bnum)
                pbcnt = pbcnt + 1
                if progressBar.wasCanceled():
                    raise KeyboardInterrupt

                if progressBar.wasSkipped():
                    return (self.diff, pbcnt)

        return (self.diff, pbcnt)

import scipy
from scipy.stats import norm

class TemplateOneSubkey(object):
    """This class is the basic progressive CPA attack, capable of adding traces onto a variable with previous data"""
    def __init__(self):
        self.clearStats()
        self.template = np.load(r'Y:\channelestimate\july2014\A_atmega328p_1318\randplain_fixedkey_key1_data\analysis\templates-0-2499-csi.npz')

    def clearStats(self):
        self.diff = [0] * 256
        self.totalTraces = 0

    def oneSubkey(self, bnum, pointRange, traces_all, numtraces, plaintexts, ciphertexts, keyround, modeltype, progressBar, model, pbcnt):
        self.totalTraces += numtraces

        traces = traces_all

        logpdf = []
        for tnum in range(0, numtraces):
            hdata = [norm.logpdf(traces[tnum], loc=self.template['mean'][bnum][hypint], scale=self.template['cov'][bnum][hypint]) for hypint in range(0, 9)]
            logpdf.append(hdata)

        # For each 0..0xFF possible value of the key byte
        for key in range(0, 256):

            # Generate hypotheticals
            for tnum in range(numtraces):

                if len(plaintexts) > 0:
                    pt = plaintexts[tnum]

                if len(ciphertexts) > 0:
                    ct = ciphertexts[tnum]

                if keyround == "first":
                    ct = None
                elif keyround == "last":
                    pt = None
                else:
                    raise ValueError("keyround invalid")

                # Generate the output of the SBOX
                if modeltype == "Hamming Weight":
                    hypint = model.HypHW(pt, ct, key, bnum);
                elif modeltype == "Hamming Weight (inverse)":
                    hypint = model.HypHW(pt, ct, key, bnum);
                    hypint = 8 - hypint
                elif modeltype == "Hamming Distance":
                    hypint = model.HypHD(pt, ct, key, bnum);
                else:
                    raise ValueError("modeltype invalid")

                self.diff[key] += logpdf[tnum][hypint]

            if progressBar:
                progressBar.setValue(pbcnt)
                progressBar.updateStatus((self.totalTraces - numtraces, self.totalTraces), bnum)
                pbcnt = pbcnt + 1
                if progressBar.wasCanceled():
                    raise KeyboardInterrupt

                if progressBar.wasSkipped():
                    return (self.diff, pbcnt)

        return (self.diff, pbcnt)


class CPAExperimentalChannelinfo(QObject):
    """
    CPA Attack done as a loop, but using an algorithm which can progressively add traces & give output stats
    """
    paramListUpdated = Signal(list)

    def __init__(self, model, parent=None):
        super(CPAExperimentalChannelinfo, self).__init__()

        resultsParams = [{'name':'Reporting Interval', 'key':'reportinterval', 'type':'int', 'value':100},
                         {'name':'Iteration Mode', 'key':'itmode', 'type':'list', 'values':{'Depth-First':'df', 'Breadth-First':'bf'}, 'value':'bf'},
                         {'name':'Skip when PGE=0', 'key':'checkpge', 'type':'bool', 'value':False},
                         ]
        self.params = ConfigParameter.create_extended(self, name='Progressive CPA', type='group', children=resultsParams)

        self.model = model
        self.sr = None
        self._parent = parent

        # print self._parent.parent

        self.stats = DataTypeDiffs()

    def paramList(self):
        return [self.params]

    def setByteList(self, brange):
        self.brange = brange

    def addTraces(self, tracedata, tracerange, progressBar=None, pointRange=None):
        keyround=self.keyround
        modeltype=self.modeltype
        brange=self.brange

        foundkey = []

        self.all_diffs = range(0,16)


        tdiff = self.findParam('reportinterval').value()

        numtraces = tracerange[1] - tracerange[0]

        if progressBar:
            pbcnt = 0
            progressBar.setMinimum(0)
            progressBar.setMaximum(len(brange) * 256 * (numtraces/tdiff + 1))

        pbcnt = 0
        #r = Parallel(n_jobs=4)(delayed(traceOneSubkey)(bnum, pointRange, traces_all, numtraces, plaintexts, ciphertexts, keyround, modeltype, progressBar, self.model, pbcnt) for bnum in brange)
        #self.all_diffs, pb = zip(*r)
        #pbcnt = 0
        cpa = [None]*(max(brange)+1)
        for bnum in brange:
            cpa[bnum] = CPAProgressiveOneSubkey()
            # cpa[bnum] = MinDistOneSubkey()
            # cpa[bnum] = TemplateOneSubkey()

        brangeMap = [None]*(max(brange)+1)
        i = 1
        for bnum in brange:
            brangeMap[bnum] = i
            i += 1

        skipPGE = self.findParam('checkpge').value()
        bf = self.findParam('itmode').value() == 'bf'

        #bf specifies a 'breadth-first' search. bf means we search across each
        #subkey by only the amount of traces specified. Depth-First means we
        #search each subkey completely, then move onto the next.
        if bf:
            brange_df = [0]
            brange_bf = brange
        else:
            brange_bf = [0]
            brange_df = brange


        #H = np.load('channelinfo-masked.npy')
        #H = np.load('csi-masked-newkey.npy')
        #H = np.load('channelinfo.npy')
        #mio = sio.loadmat('equalizer.mat')
        #H = mio['equaltotal']
        # H = np.load('equalization.npy')
        # self.project() ?
        project = self._parent.parent().proj
        section = project.getDataConfig("Template Data", "Equalization Matrix")
       # section = project.getDataConfig("Template Data", "AOF Matrix")
        fname = project.convertDataFilepathAbs(section[0]["filename"])
        H = np.load(fname)

        #for j in range(0, 16):
            #4 = 500-800
            #test = H.copy()
            #for i in range(0, 5):
            #    threshold = max(abs(test[j]))
            #    test[j, abs(test[j,:]) >= threshold ] = 0

            #print "%f %d"%(threshold, (abs(H[j,:]) > threshold).sum())

            #H[j, abs(H[j,:]) < threshold] = 0

        for bnum_df in brange_df:

            #CPAMemoryOneSubkey
            #CPASimpleOneSubkey
            #(self.all_diffs[bnum], pbcnt) = sCPAMemoryOneSubkey(bnum, pointRange, traces_all, numtraces, plaintexts, ciphertexts, keyround, modeltype, progressBar, self.model, pbcnt)

            tstart = 0
            tend = tdiff


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


                for bnum_bf in brange_bf:

                    if bf:
                        bnum = bnum_bf
                    else:
                        bnum = bnum_df

                    traces_fixed = np.dot(traces - traces.mean(axis=0), H[bnum]) + 4
                    skip = False
                    if (self.stats.simplePGE(bnum) != 0) or (skipPGE == False):
                        (data, pbcnt) = cpa[bnum].oneSubkey(bnum, pointRange, traces_fixed, tend - tstart, textins, textouts, keyround, modeltype, progressBar, self.model, pbcnt)
                        self.stats.updateSubkey(bnum, data, tnum=tend)
                    else:
                        skip = True

                    if progressBar.wasSkipped() or skip:
                        progressBar.clearSkipped()
                        pbcnt = brangeMap[bnum] * 256 * (numtraces/tdiff + 1)

                        if bf is False:
                            tstart = numtraces

                tend += tdiff
                tstart += tdiff

                if self.sr is not None:
                    self.sr()

            if progressBar is not None:
                pbcnt = 0

    def getStatistics(self):
        return self.stats

    def setStatsReadyCallback(self, sr):
        self.sr = sr

from chipwhisperer.analyzer.utils.Partition import Partition

# This is actually used by ProfilingTemplate via a hack, which requires more work...
class TemplateCSI(object):
    """
    Template using Multivariate Stats (mean + covariance matrix)
    """
    def __init__(self, tmanager=None):
        self._tmanager = None
        self.partObject = Partition(self)

    def traceManager(self):
        return self._tmanager

    def setTraceManager(self, tmanager):
        self._tmanager = tmanager

    def setProject(self, proj):
        self._project = proj

    def project(self):
        return self._project

    def generate(self, trange, poiList, numPartitions):
        """Generate templates for all partitions over entire trace range"""

        section = self.project().getDataConfig("Template Data", "Equalization Matrix")
        fname = self.project().convertDataFilepathAbs(section[0]["filename"])
        H = np.load(fname)

        # Number of subkeys
        subkeys = len(poiList)

        tstart = trange[0]
        tend = trange[1]

        templateTraces = [ [ [] for j in range(0, numPartitions) ] for i in range(0, subkeys) ]

        templateMeans = [ np.zeros(numPartitions) for i in range (0, subkeys) ]
        templateCovs = [ np.zeros(numPartitions) for i in range (0, subkeys) ]

        for tnum in range(tstart, tend):
            partData = self.traceManager().getAuxData(tnum, self.partObject.attrDictPartition)["filedata"]

            for bnum in range(0, subkeys):
                for i in range(0, numPartitions):

                    if tnum in partData[bnum][i]:
                        trace = self.traceManager().getTrace(tnum)
                        trace_fixed = np.dot(trace - trace.mean(), H[bnum]) + 4
                        templateTraces[bnum][i].append(trace_fixed)

            if tnum % 100 == 0:
                print tnum

        for bnum in range(0, subkeys):
            for i in range(0, numPartitions):
                templateMeans[bnum][i] = np.mean(templateTraces[bnum][i], axis=0)
                cov = np.cov(templateTraces[bnum][i], rowvar=0)
                # print "templateTraces[%d][%d] = %d" % (bnum, i, len(templateTraces[bnum][i]))

                if len(templateTraces[bnum][i]) > 0:
                    templateCovs[bnum][i] = cov
                else:
                    print "WARNING: Insufficient template data to generate covariance matrix for bnum=%d, partition=%d" % (bnum, i)
                    templateCovs[bnum][i] = np.zeros((len(poiList[bnum]), len(poiList[bnum])))

                # except ValueError:
                #    raise ValueError("Insufficient template data to generate covariance matrix for bnum=%d, partition=%d" % (bnum, i))

        self.templateMeans = templateMeans
        self.templateCovs = templateCovs
        self.templateSource = (tstart, tend)

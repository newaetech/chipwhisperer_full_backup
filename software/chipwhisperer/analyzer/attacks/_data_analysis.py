#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2014, NewAE Technology Inc
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
import logging

import numpy as np


class DataAnalysis(object):

    def loadData(self, filename):
        self.rawdata = np.load(filename)

    def setKnownkey(self, knownkey):
        numsubkeys = len(self.rawdata[0]["diffsmax"])
        if len(knownkey) != numsubkeys:
            raise ValueError("Knownkey of length %d differs from trace keylength of %d" % (len(knownkey), numsubkeys))

        self.knownkey = knownkey

    def calculate(self, useAbs=True):
        nkeys = len(self.knownkey)
        nhyp = len(self.rawdata[0]["diffsmax"][0])
        ntests = len(self.rawdata)
        data = {}

        for tindex in range(0, ntests):
            tnum = self.rawdata[tindex]["tracecnt"][0]

            if __debug__: logging.debug(tnum)

            if tnum not in data.keys():
                data[tnum] = {'pge':[0] * nkeys, 'gsr':0, 'tests':0}

            pge = [0] * nkeys
            maxes = [0] * nkeys
            for i in range(0, nkeys):
                maxes[i] = np.zeros(nhyp, dtype=[('hyp', 'i2'), ('value', 'f8')])

            for i in range(0, nkeys):
                for hyp in range(0, nhyp):
                    if useAbs:
                        v1 = abs(self.rawdata[tindex]["diffsmax"][i][hyp])
                        v2 = abs(self.rawdata[tindex]["diffsmin"][i][hyp])
                        mvalue = max(v1, v2)
                    else:
                        mvalue = self.rawdata[tindex]["diffsmax"][i][hyp]

                    maxes[i][hyp]['hyp'] = hyp
                    maxes[i][hyp]['value'] = mvalue

                    # TODO: workaround for PGE, as NaN's get ranked first
                    numnans = np.isnan(maxes[i]['value']).sum()

                    maxes[i].sort(order='value')
                    maxes[i] = maxes[i][::-1]

                    try:
                        pge[i] = np.where(maxes[i]['hyp'] == self.knownkey[i])[0][0] - numnans
                        if pge[i] < 0:
                            pge[i] = 128
                    except IndexError:
                        pge[i] = 255

            if all(v == 0 for v in pge):
                gsr = 1
            else:
                gsr = 0

            data[tnum]['pge'] = [data[tnum]['pge'][k] + pge[k] for k in range(0, len(pge))]
            data[tnum]['gsr'] += gsr
            data[tnum]['tests'] += 1

        return data

# fdir = "C:\\E\\Documents\\academic\\sidechannel\\eclipse-workspace\\chipwhisperer\\chipwhisperer\\software\\chipwhisperer\\analyzer\\"
# fname = "tempstats_20141009_213041.npy"

# test = DataAnalysis()
# test.loadData(fdir + fname)
# test.setKnownkey([0xea, 0x79, 0x79, 0x20, 0xc8, 0x71, 0x44, 0x7d, 0x46, 0x62, 0x5f, 0x51, 0x85, 0xc1, 0x3b, 0xcb])

# print "Calculating..."
# data = test.calculate()
# print "Done"

# print data

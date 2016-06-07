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


class DataTypeDiffs(object):
    """
    Data type used for attacks generating peaks indicating the 'best' success. Examples include
    standard DPA & CPA attacks.
    """
    numSubkeys = 16
    numPerms = 256

    def __init__(self, numSubkeys=16, numPerms=256):
        self.numSubkeys = numSubkeys
        self.numPerms = numPerms
        self.knownkey = None
        self.clear()

    def clear(self):
        #Diffs from CPA/DPA Attack
        self.diffs = [None]*self.numSubkeys

        #Maximum diff & location of maximum
        self.maxes = [0]*self.numSubkeys
        for i in range(0, self.numSubkeys):
            self.maxes[i] = np.zeros(self.numPerms, dtype=[('hyp', 'i2'), ('point', 'i4'), ('value', 'f8')])

        #If maximum diffs are valid & sorted correctly
        self.maxValid = [False]*self.numSubkeys
        self.pge = [255]*self.numSubkeys
        self.diffs_tnum = [None]*self.numSubkeys
        self.pge_total = []
        self.maxes_list = [list() for i in range(0, self.numSubkeys)]

        #TODO: Ensure this gets called by attack algorithms when rerunning

    def simplePGE(self, bnum):
        if self.maxValid[bnum] == False:
            #TODO: should sort
            return 1
        return self.pge[bnum]

    def setKnownkey(self, knownkey):
        self.knownkey = knownkey

    def updateSubkey(self, bnum, data, copy=True, forceUpdate=False, tnum=None):
        if (id(data) != id(self.diffs[bnum])) or forceUpdate:
            self.maxValid[bnum] = False

            if data is not None and copy:
                self.diffs[bnum] = data[:]
                self.diffs_tnum[bnum] = tnum
            else:
                self.diffs[bnum] = data
                self.diffs_tnum[bnum] = tnum

    def findMaximums(self, bytelist=None, useAbsolute=True, useSingle=False):
        if bytelist is None:
            bytelist = range(0, self.numSubkeys)

        # print useAbsolute

        for i in bytelist:
            if self.diffs[i] is None:
                self.maxValid[i] = False
                continue

            if self.maxValid[i] == False:
                for hyp in range(0, 256):
                    if useAbsolute:
                        v = np.nanmax(np.fabs(self.diffs[i][hyp]))
                    else:
                        v = np.nanmax(self.diffs[i][hyp])

                    mvalue = v

                    #Get maximum value for this hypothesis
                    try:
                        mindex = np.amin(np.where(v == mvalue))
                    except ValueError:
                        mindex = 255
                    self.maxes[i][hyp]['hyp'] = hyp
                    self.maxes[i][hyp]['point'] = mindex
                    self.maxes[i][hyp]['value'] = mvalue

                #TODO: why does this fail?
                #self.maxes[i][np.isnan(self.maxes[i]['value'])]['value'] = 0
                #TODO: workaround for PGE, as NaN's get ranked first
                numnans = np.isnan(self.maxes[i]['value']).sum()

                if useSingle:
                    #All table values are taken from same point MAX is taken from
                    where = self.maxes[i][0]['point']
                    for j in range(0,256):
                        self.maxes[i][j]['point'] = where
                        self.maxes[i][j]['value'] = self.diffs[i][self.maxes[i][j]['hyp']][where]

                self.maxes[i][::-1].sort(order='value') # sorts nunpy array in place and in reverse order
                self.maxValid[i] = True

                if self.knownkey is not None:
                    try:
                        self.pge[i] = np.where(self.maxes[i]['hyp'] == self.knownkey[i])[0][0] - numnans
                        if self.pge[i] < 0:
                            self.pge[i] = 128
                    except IndexError:
                        self.pge[i] = 255

            tnum = self.diffs_tnum[i]
            self.pge_total.append({'trace':tnum, 'subkey':i, 'pge':self.pge[i]})
            
            if len(self.maxes_list[i]) == 0 or self.maxes_list[i][-1]['trace'] != tnum:
                self.maxes_list[i].append({'trace':tnum, 'maxes':np.array(self.maxes[i])})

        return self.maxes

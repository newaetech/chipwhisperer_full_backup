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
import logging

import numpy as np

from chipwhisperer.common.api.autoscript import AutoScript
from chipwhisperer.common.utils import util
from chipwhisperer.common.utils.tracesource import PassiveTraceObserver

try:
    from scipy.stats import multivariate_normal
except ImportError:
    multivariate_normal = None


class TemplateBasic(AutoScript, PassiveTraceObserver):
    """
    Template using Multivariate Stats (mean + covariance matrix)
    """
    scriptsUpdated = util.Signal()

    def __init__(self):
        AutoScript.__init__(self)
        PassiveTraceObserver.__init__(self)
        self.getParams().getChild("Input").hide()

    def setProject(self, proj):
        self._project = proj

    def project(self):
        return self._project

    def generate(self, trange, poiList, partMethod, progressBar=None):
        """Generate templates for all partitions over entire trace range"""

        # Number of subkeys
        subkeys = len(poiList)

        numPartitions = partMethod.getNumPartitions()

        tstart = trange[0]
        tend = trange[1]

        templateTraces = [ [ [] for j in range(0, numPartitions) ] for i in range(0, subkeys) ]

        templateMeans = [ np.zeros((numPartitions, len(poiList[i]))) for i in range (0, subkeys) ]
        templateCovs = [ np.zeros((numPartitions, len(poiList[i]), len(poiList[i]))) for i in range (0, subkeys) ]

        # partData = generatePartitions(self, partitionClass=None, saveFile=False, loadFile=False, traces=None)
        # partData = partObj.loadPartitions(trange)

        if progressBar:
            progressBar.setText('Generating Trace Matrix:')
            progressBar.setMaximum(tend - tstart + subkeys)

        for tnum in range(tstart, tend):
            # partData = self.getTraceSource().getAuxData(tnum, self.partObject.attrDictPartition)["filedata"]
            pnum = partMethod.getPartitionNum(self.getTraceSource(), tnum)
            t = self.getTraceSource().getTrace(tnum)
            for bnum in range(0, subkeys):
                templateTraces[bnum][pnum[bnum]].append(t[poiList[bnum]])

            if progressBar:
                progressBar.updateStatus(tnum - tstart)
                if progressBar.wasAborted():
                    return None

        if progressBar:
            progressBar.setText('Generating Trace Covariance and Mean Matrices:')

        for bnum in range(0, subkeys):
            for i in range(0, numPartitions):
                templateMeans[bnum][i] = np.mean(templateTraces[bnum][i], axis=0)
                cov = np.cov(templateTraces[bnum][i], rowvar=0)
                if __debug__: logging.debug('templateTraces[%d][%d] = %d' % (bnum, i, len(templateTraces[bnum][i])))

                if len(templateTraces[bnum][i]) > 0:
                    templateCovs[bnum][i] = cov
                else:
                    logging.warning('Insufficient template data to generate covariance matrix for bnum=%d, partition=%d' % (bnum, i))
                    templateCovs[bnum][i] = np.zeros((len(poiList[bnum]), len(poiList[bnum])))

            if progressBar:
                progressBar.updateStatus(tend + bnum)
                if progressBar.wasAborted():
                    return None

        self.template = {
         "mean":templateMeans,
         "cov":templateCovs,
         "trange":(tstart, tend),
         "poi":poiList,
         "partitiontype":partMethod.__class__.__name__
        }

        if progressBar:
            progressBar.close()

        return self.template
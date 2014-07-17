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
#
# ChipWhisperer is a trademark of NewAE Technology Inc.
#============================================================================
import sys

try:
    from PySide.QtCore import *
    from PySide.QtGui import *
except ImportError:
    print "ERROR: PySide is required for this program"
    sys.exit()

import os
import numpy as np
from openadc.ExtendedParameter import ExtendedParameter
import ast
try:
    from pyqtgraph.parametertree import Parameter
except ImportError:
    print "ERROR: PyQtGraph is required for this program"
    sys.exit()

import scipy
try:
    from scipy.stats import multivariate_normal
except ImportError:
    print "WARNING: Version of SciPy too old, require > 0.14, have %s" % (scipy.version.version)
    # sys.exit()

from chipwhisperer.common.autoscript import AutoScript
from chipwhisperer.analyzer.attacks.AttackStats import DataTypeDiffs
from chipwhisperer.analyzer.attacks.models.AES128_8bit import HypHW
from chipwhisperer.analyzer.attacks.AttackProgressDialog import AttackProgressDialog
from chipwhisperer.analyzer.utils.Partition import Partition

class TemplateBasic(AutoScript, QObject):
    """
    Template using Multivariate Stats (mean + covariance matrix)
    """
    scriptsUpdated = Signal()

    def __init__(self, tmanager=None):
        super(TemplateBasic, self).__init__()
        self._tmanager = None

    def traceManager(self):
        return self._tmanager

    def setTraceManager(self, tmanager):
        self._tmanager = tmanager

    def setProject(self, proj):
        self._project = proj

    def project(self):
        return self._project

    def generate(self, trange, poiList, partMethod, showProgressBar=True):
        """Generate templates for all partitions over entire trace range"""

        if showProgressBar:
            progressBar = QProgressDialog()
            # progressBar.setWindowModality(Qt.WindowModal)
            # progressBar.setMinimumDuration(1000)
            # progressBar.offset = trange[0]
        else:
            progressBar = None

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
            progressBar.setWindowTitle('Generating Templates')
            progressBar.setLabelText('Generating Trace Matrix')
            progressBar.setMinimum(0)
            progressBar.setMaximum(tend - tstart + subkeys)

        for tnum in range(tstart, tend):
            # partData = self.traceManager().getAuxData(tnum, self.partObject.attrDictPartition)["filedata"]
            pnum = partMethod.getPartitionNum(self.traceManager(), tnum)
            t = self.traceManager().getTrace(tnum)
            for bnum in range(0, subkeys):
                templateTraces[bnum][pnum[bnum]].append(t[poiList[bnum]])

            if progressBar:
                progressBar.setValue(tnum - tstart)
                if progressBar.wasCanceled():
                    progressBar.setValue(progressBar.maximum())
                    return None

        if progressBar:
            progressBar.setLabelText('Generating Trace Covariance and Mean Matrices')

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

            if progressBar:
                progressBar.setValue(tend + bnum)
                if progressBar.wasCanceled():
                    progressBar.setValue(progressBar.maximum())
                    return None


                # except ValueError:
                #    raise ValueError("Insufficient template data to generate covariance matrix for bnum=%d, partition=%d" % (bnum, i))

        # self.templateMeans = templateMeans
        # self.templateCovs = templateCovs
        # self.templateSource = (tstart, tend)
        # self.templatePoiList = poiList
        # self.templatePartMethod = partMethod

        self.template = {
         "mean":templateMeans,
         "cov":templateCovs,
         "trange":(tstart, tend),
         "poi":poiList,
         "partitiontype":partMethod.__class__
        }

        return self.template


class ProfilingTemplate(AutoScript, QObject):
    """
    Template Attack done as a loop, but using an algorithm which can progressively add traces & give output stats
    """
    paramListUpdated = Signal(list)

    def __init__(self, parent, showScriptParameter=None, tmanager=None):
        super(ProfilingTemplate, self).__init__()

        self.parent = parent
        self._tmanager = None
        self._project = None

        resultsParams = [{'name':'Template Generation', 'type':'group', 'children':[
                            {'name':'Trace Start', 'key':'tgenstart', 'value':0, 'type':'int', 'set':self.updateScript},
                            {'name':'Trace End', 'key':'tgenstop', 'value':1000, 'type':'int', 'set':self.updateScript},
                            {'name':'POI Selection', 'key':'poimode', 'type':'list', 'values':{'TraceExplorer Table':0, 'Read from Project File':1}, 'value':0, 'set':self.updateScript},
                            {'name':'Read POI', 'type':'action', 'action':self.updateScript},
                            {'name':'Generate Templates', 'type':'action', 'action': lambda:self.runScriptFunction.emit("generateTemplates")}
                            ]},

                         {'name':'Reporting Interval', 'key':'reportinterval', 'type':'int', 'value':100, 'set':self.updateScript},
                         ]
        self.params = Parameter.create(name='Template Attack', type='group', children=resultsParams)
        if showScriptParameter is not None:
            self.showScriptParameter = showScriptParameter
            # print self.showScriptParameter
        ExtendedParameter.setupExtended(self.params, self)

        self.addGroup("generateTemplates")

        self.sr = None

        self.stats = DataTypeDiffs()
        self.setProfileAlgorithm(TemplateBasic)

        # Not needed as setProfileAlgorithm calls this
        # self.updateScript()

    def setProfileAlgorithm(self, algo):
        self.profiling = algo()
        self.profiling.setTraceManager(self._tmanager)
        self.profiling.setProject(self._project)
        self.profiling.scriptsUpdated.connect(self.updateScript)
        self.updateScript()

    def updateScript(self, ignored=None):



        self.addFunction('init', 'setReportingInterval', '%d' % self.findParam('reportinterval').value())
        try:
            ted = self.parent.parent.utilList[0].exampleScripts[0]
        except AttributeError:
            return
        #self.addVariable('generateTemplates', 'ted', 'self.parent.parent.utilList[0].exampleScripts[0]')

        self.addFunction('generateTemplates', 'initAnalysis', '', obj='userScript')
        self.addVariable('generateTemplates', 'tRange', '(%d, %d)' % (self.findParam('tgenstart').value(), self.findParam('tgenstop').value()))
        # self.addVariable('generateTemplates', 'numPartitions', '9')

        if self.findParam('poimode').value() == 0:
            self.addVariable('generateTemplates', 'poiList', '%s' % ted.poi.poiArray)
            self.addVariable('generateTemplates', 'partMethod', '%s()' % ted.partObject.partMethod.moduleName)
            self.importsAppend("from chipwhisperer.analyzer.utils.Partition import %s" % ted.partObject.partMethod.moduleName)
        else:
            poidata = self.loadPOIs()[-1]
            self.addVariable('generateTemplates', 'poiList', '%s' % poidata["poi"])
            self.addVariable('generateTemplates', 'partMethod', '%s()' % poidata["partitiontype"])
            self.importsAppend("from chipwhisperer.analyzer.utils.Partition import %s" % poidata["partitiontype"])

        self.addFunction('generateTemplates', 'profiling.generate', 'tRange, poiList, partMethod')

        # Generate templates
        # self.profiling.generate(tRange, poiList , numParts)

        # cfgsec = self.project().addDataConfig(sectionName="Template Data", subsectionName="Templates")
        # cfgsec["tracestart"] = tRange[0]
        # cfgsec["traceend"] = tRange[1]

        # fname = self.project().getDataFilepath('templates-%d-%d.npz' % (tRange[0], tRange[1]), 'analysis')

        # Save template file
        # np.savez(fname["abs"], mean=self.profiling.templateMeans, cov=self.profiling.templateCovs)

        # cfgsec["filename"] = fname["rel"]

        self.scriptsUpdated.emit()

    def paramList(self):
        return [self.params]

    def setByteList(self, brange):
        self.brange = brange

    def setKeyround(self, keyround):
        self.keyround = keyround

    def setReportingInterval(self, intv):
        self._reportinginterval = intv

    def setModeltype(self, modeltype):
        self.modeltype = modeltype

    def traceManager(self):
        return self._tmanager

    def setTraceManager(self, tmanager):
        self._tmanager = tmanager
        # Set for children
        self.profiling.setTraceManager(tmanager)

    def setProject(self, proj):
        self._project = proj
        # Set for children
        self.profiling.setProject(proj)

    def project(self):
        return self._project

    def generateTemplates(self, numParts, tRange, poiList):
        tRange = (self.findParam('tgenstart').value(), self.findParam('tgenstop').value())

        # TODO - Fix this
        # numParts = 256
        numParts = 9
        poiList = self.loadPOIs()[0]["poi"]

        # Generate templates
        self.profiling.generate(tRange, poiList , numParts)

        cfgsec = self.project().addDataConfig(sectionName="Template Data", subsectionName="Templates")
        cfgsec["tracestart"] = tRange[0]
        cfgsec["traceend"] = tRange[1]

        fname = self.project().getDataFilepath('templates-%d-%d.npz' % (tRange[0], tRange[1]), 'analysis')

        # Save template file
        np.savez(fname["abs"], mean=self.profiling.templateMeans, cov=self.profiling.templateCovs)

        cfgsec["filename"] = fname["rel"]


    def loadTemplates(self):
        # Load Template
        foundsecs = self.parent.project().getDataConfig(sectionName="Template Data", subsectionName="Templates")

        templates = []

        for f in foundsecs:
            fname = self.parent.project().convertDataFilepathAbs(f["filename"])
            templates.append(np.load(fname))

        return templates

    def loadPOIs(self):
        section = self.project().getDataConfig("Template Data", "Points of Interest")

        poiList = []

        for s in section:
            poistr = str(s["poi"])
            poistr = poistr.replace('"', '')
            poistr = poistr.replace("'", "")
            try:
                poieval = ast.literal_eval(poistr)
            except ValueError:
                raise ValueError("Failed to convert %s to list" % (poistr))
            
            poiList.append(s.copy())
            poiList[-1]["poi"] = poieval

        return poiList

    def addTraces(self, traces, plaintexts, ciphertexts, progressBar=None, pointRange=None):

        # Hack for now - just use first template & POI list found
        template = self.loadTemplates()[0]
        pois = self.loadPOIs()[0]

        numparts = len(template['mean'][0])

        results = np.zeros((16, 256))

        # trace = np.mean(traces, axis=0)
        # print np.shape(trace)

        tdiff = self.findParam('reportinterval').value()

        if progressBar:
            progressBar.setMinimum(0)
            progressBar.setMaximum(16 * len(traces))
            pcnt = 0

        for tnum in range(0, len(traces)):
            for bnum in range(0, 16):
                newresultsint = [multivariate_normal.logpdf(traces[tnum][pois['poi'][bnum]], mean=template['mean'][bnum][i], cov=np.diag(template['cov'][bnum][i])) for i in range(0, numparts)]
                
                #TODO: call out HW/HW method?
                if numparts == 9:
                    newresults = []
                    # Map to key guess format
                    for i in range(0, 256):
                        # Get hypothetical hamming weight
                        hypint = HypHW(plaintexts[tnum], None, i, bnum);
                        newresults.append(newresultsint[ hypint ])
                else:
                    newresults = newresultsint
                
                results[bnum] += newresults
                self.stats.updateSubkey(bnum, results[bnum], tnum=(tnum + 1))

                if progressBar:
                    progressBar.setValue(pcnt)
                    progressBar.updateStatus((tnum, len(traces)), bnum)
                    pcnt += 1

                    if progressBar.wasCanceled():
                        raise KeyboardInterrupt


            # Do plotting if required
            if (tnum % tdiff) == 0 and self.sr:
                self.sr()

    def getStatistics(self):
        return self.stats

    def setStatsReadyCallback(self, sr):
        self.sr = sr


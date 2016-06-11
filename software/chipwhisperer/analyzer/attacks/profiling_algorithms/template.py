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
import scipy
from ._base import TemplateBasic, multivariate_normal
from chipwhisperer.analyzer.attacks._stats import DataTypeDiffs
from chipwhisperer.analyzer.attacks.models import AES128_8bit as AESModel
from chipwhisperer.analyzer.attacks.models.AES128_8bit import getHW
from chipwhisperer.common.api.autoscript import AutoScript
from chipwhisperer.common.utils import util
from chipwhisperer.common.utils.tracesource import PassiveTraceObserver
from chipwhisperer.common.utils.pluginmanager import Plugin
from chipwhisperer.common.utils.parameter import setupSetParam
from chipwhisperer.analyzer.ui.CWAnalyzerGUI import CWAnalyzerGUI


class ProfilingTemplate(AutoScript, PassiveTraceObserver, Plugin):
    """
    Template Attack done as a loop, but using an algorithm which can progressively add traces & give output stats
    """
    _name= 'Template Attack'
    path = 'profiling_algorithms.template'

    def __init__(self, parent):
        AutoScript.__init__(self)
        PassiveTraceObserver.__init__(self)
        self.getParams().getChild("Input").hide()
        self._project = None

        self.params.addChildren([
            {'name':'Load Template', 'type':'group', 'children':[]},
            {'name':'Generate New Template', 'type':'group', 'children':[
                {'name':'Trace Start', 'key':'tgenstart', 'value':0, 'type':'int', 'action':lambda _:self.updateScript()},
                {'name':'Trace End', 'key':'tgenstop', 'value':parent.traceMax, 'type':'int', 'action':lambda _:self.updateScript()},
                {'name':'POI Selection', 'key':'poimode', 'type':'list', 'values':{'TraceExplorer Table':0, 'Read from Project File':1}, 'value':0, 'action':lambda _:self.updateScript()},
                {'name':'Read POI', 'type':'action', 'action':lambda _:self.updateScript()},
                {'name':'Generate Templates', 'type':'action', 'action': lambda _:self.runScriptFunction.emit("generateTemplates")}
            ]},
        ])

        self.addGroup("generateTemplates")

        self.sr = None
        self.stats = DataTypeDiffs()
        self.setProfileAlgorithm(TemplateBasic)

    def setProfileAlgorithm(self, algo):
        self.profiling = algo()
        self.profiling.setTraceSource(self._traceSource)
        self.profiling.setProject(self._project)
        self.profiling.scriptsUpdated.connect(self.updateScript)
        self.updateScript()

    def updateScript(self, ignored=None):
        #self.addFunction('init', 'setReportingInterval', '%d' % self.findParam('reportinterval').getValue())

        try:
            ted = CWAnalyzerGUI.getInstance().traceExplorerDialog.exampleScripts[0]
        except AttributeError:
            print("INFO: Delaying script for template attack until TraceExplorer exists...")
            return

        self.addFunction('generateTemplates', 'initPreprocessing', '', obj='UserScript')
        self.addFunction('generateTemplates', 'initAnalysis', '', obj='UserScript')
        self.addVariable('generateTemplates', 'tRange', '(%d, %d)' % (self.findParam(["Generate New Template",'tgenstart']).getValue(), self.findParam(["Generate New Template",'tgenstop']).getValue()))

        if self.findParam(["Generate New Template",'poimode']).getValue() == 0:
            self.addVariable('generateTemplates', 'poiList', '%s' % ted.poi.poiArray)
            self.addVariable('generateTemplates', 'partMethod', '%s()' % ted.partObject.partMethod.__class__.__name__)
            self.importsAppend("from chipwhisperer.analyzer.utils.Partition import %s" % ted.partObject.partMethod.__class__.__name__)
        else:
            poidata = self.loadPOIs()[-1]
            self.addVariable('generateTemplates', 'poiList', '%s' % poidata["poi"])
            self.addVariable('generateTemplates', 'partMethod', '%s()' % poidata["partitiontype"])
            self.importsAppend("from chipwhisperer.analyzer.utils.Partition import %s" % poidata["partitiontype"])

        self.addFunction('generateTemplates', 'profiling.generate', 'tRange, poiList, partMethod', 'templatedata')

        #Save template data to project
        self.addFunction('generateTemplates', 'saveTemplatesToProject', 'tRange, templatedata', 'tfname')

        self.scriptsUpdated.emit()

    def traceLimitsChanged(self, traces, points):
        tstart = self.findParam(["Generate New Template",'tgenstart'])
        tend = self.findParam(["Generate New Template",'tgenstop'])
        tstart.setLimits((0, traces-1))
        tend.setLimits((0, traces-1))
        tend.setValue(traces-1)

    def setByteList(self, brange):
        self.brange = brange

    def setKeyround(self, keyround):
        self.keyround = keyround

    def setReportingInterval(self, intv):
        self._reportinginterval = intv

    @setupSetParam('Input')
    def setTraceSource(self, traceSource):
        PassiveTraceObserver.setTraceSource(self, traceSource, blockSignal=True)
        # Set for children
        self.profiling.setTraceSource(traceSource)

    def setProject(self, proj):
        self._project = proj
        # Set for children
        self.profiling.setProject(proj)

    def project(self):
        return self._project

    def saveTemplatesToProject(self, trange, templatedata):
        cfgsec = self.project().addDataConfig(sectionName="Template Data", subsectionName="Templates")
        cfgsec["tracestart"] = trange[0]
        cfgsec["traceend"] = trange[1]
        cfgsec["poi"] = templatedata["poi"]
        cfgsec["partitiontype"] = templatedata["partitiontype"]

        fname = self.project().getDataFilepath('templates-%s-%d-%d.npz' % (cfgsec["partitiontype"], trange[0], trange[1]), 'analysis')

        # Save template file
        np.savez(fname["abs"], **templatedata)  # mean=self.profiling.templateMeans, cov=self.profiling.templateCovs)
        cfgsec["filename"] = fname["rel"]

        print "Saved template to: %s" % fname["abs"]

        return fname["abs"]

    def loadTemplatesFromProject(self):
        # Load Template
        foundsecs = self.project().getDataConfig(sectionName="Template Data", subsectionName="Templates")
        templates = []

        for f in foundsecs:
            fname = self.project().convertDataFilepathAbs(f["filename"])
            t = np.load(fname)
            templates.append(t)

            # Validate in case someone tries to change via project file
            if f["partitiontype"] != t["partitiontype"]:
                print "WARNING: PartitionType for template from .npz file (%s) differs from project file (%s). npz file being used."

            if (util.strListToList(str(f["poi"])) != t["poi"]).any():
                print "WARNING: POI for template from .npz file (%s) differs from project file (%s). npz file being used."

        return templates

    def loadPOIs(self):
        section = self.project().getDataConfig("Template Data", "Points of Interest")

        poiList = []

        for s in section:
            poistr = str(s["poi"])
            poieval = util.strListToList(poistr)
            poiList.append(s.copy())
            poiList[-1]["poi"] = poieval

        return poiList

    def addTraces(self, traces, plaintexts, ciphertexts, knownkeys=None, progressBar=None, pointRange=None):

        if multivariate_normal is None:
            raise Warning("Version of SciPy too old, require > 0.14, have %s. "
                          "Update to support this attack" % (scipy.version.version))

        # Hack for now - just use last template found
        template = self.loadTemplatesFromProject()[-1]
        pois = template["poi"]
        numparts = len(template['mean'][0])
        results = np.zeros((16, 256))

        tdiff = self._reportinginterval

        if progressBar:
            progressBar.setStatusMask("Current Trace = %d-%d Current Subkey = %d")
            progressBar.setMaximum(16 * len(traces))
        pcnt = 0

        for tnum in range(0, len(traces)):
            for bnum in self.brange:
                try:
                    newresultsint = [multivariate_normal.logpdf(traces[tnum][pois[bnum]], mean=template['mean'][bnum][i], cov=np.diag(template['cov'][bnum][i])) for i in range(0, numparts)]
                except np.linalg.LinAlgError, e:
                    print("WARNING: Error in applying template, probably template is poorly formed or POI incorrect. Error: " + str(e))
                    print("         Byte %d for tnum %d skipped due to this error."%(bnum, tnum))
                    newresultsint = [0] * 256

                ptype = template["partitiontype"]

                if ptype == "PartitionHWIntermediate":
                    newresults = []
                    # Map to key guess format
                    for i in range(0, 256):
                        # Get hypothetical hamming weight
                        # hypint = HypHW(plaintexts[tnum], None, i, bnum)
                        hypint = AESModel.leakage(plaintexts[tnum], ciphertexts[tnum], i, bnum, AESModel.LEAK_HW_SBOXOUT_FIRSTROUND, None)
                        newresults.append(newresultsint[ hypint ])
                elif ptype == "PartitionHDLastRound":
                    newresults = []
                    # Map to key guess format
                    for i in range(0, 256):
                        # Get hypothetical hamming distance
                        # hypint = HypHD(plaintexts[tnum], None, i, bnum)
                        # hypint = HypHD(None, ciphertexts[tnum], i, bnum)
                        hypint = AESModel.leakage(plaintexts[tnum], ciphertexts[tnum], i, bnum, AESModel.LEAK_HD_LASTROUND_STATE, None)
                        newresults.append(newresultsint[ hypint ])

                # TODO Temp
                elif ptype == "PartitionHDRounds":
                    newresults = []
                    # Map to key guess format
                    for i in range(0, 256):
                        # Get hypothetical hamming distance
                        # hypint = HypHD(plaintexts[tnum], None, i, bnum)
                        if bnum == 0:
                            hypint = getHW(plaintexts[tnum][bnum] ^ i)
                        else:
                            knownkey = [0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c]
                            s1 = plaintexts[tnum][bnum - 1] ^ knownkey[bnum - 1]
                            s2 = plaintexts[tnum][bnum] ^ i
                            hypint = getHW(s1 ^ s2)

                        newresults.append(newresultsint[ hypint ])
                else:
                    newresults = newresultsint

                results[bnum] += newresults
                self.stats.updateSubkey(bnum, results[bnum], tnum=(tnum + 1))

                pcnt += 1
                if progressBar:
                    progressBar.updateStatus(pcnt, (tnum, len(traces)-1, bnum))
                    if progressBar.wasAborted():
                        return

            # Do plotting if required
            if (tnum % tdiff) == 0 and self.sr:
                self.sr()

    def getStatistics(self):
        return self.stats

    def setStatsReadyCallback(self, sr):
        self.sr = sr
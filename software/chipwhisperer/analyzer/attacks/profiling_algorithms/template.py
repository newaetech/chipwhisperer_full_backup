# #!/usr/bin/python
# # -*- coding: utf-8 -*-
# #
# # Copyright (c) 2013-2014, NewAE Technology Inc
# # All rights reserved.
# #
# # Authors: Colin O'Flynn
# #
# # Find this and more at newae.com - this file is part of the chipwhisperer
# # project, http://www.assembla.com/spaces/chipwhisperer
# #
# #    This file is part of chipwhisperer.
# #
# #    chipwhisperer is free software: you can redistribute it and/or modify
# #    it under the terms of the GNU General Public License as published by
# #    the Free Software Foundation, either version 3 of the License, or
# #    (at your option) any later version.
# #
# #    chipwhisperer is distributed in the hope that it will be useful,
# #    but WITHOUT ANY WARRANTY; without even the implied warranty of
# #    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# #    GNU Lesser General Public License for more details.
# #
# #    You should have received a copy of the GNU General Public License
# #    along with chipwhisperer.  If not, see <http://www.gnu.org/licenses/>.
# #=================================================
# import logging
# import numpy as np
# import scipy # type: ignore
# import sys

# from chipwhisperer.common.utils import util
# from chipwhisperer.analyzer.ui.CWAnalyzerGUI import CWAnalyzerGUI
# from ..algorithmsbase import AlgorithmsBase


# class TemplateUsingMVS(object):
#     """
#     Template using Multivariate Stats (mean + covariance matrix)
#     """

#     @staticmethod
#     def generate(traceSource, trange, poiList, partMethod, progressBar=None):
#         """Generate templates for all partitions over entire trace range"""

#         # Number of subkeys
#         subkeys = len(poiList)

#         numPartitions = partMethod.getNumPartitions()

#         tstart = trange[0]
#         tend = trange[1]

#         templateTraces = [ [ [] for j in range(0, numPartitions) ] for i in range(0, subkeys) ]

#         templateMeans = [ np.zeros((numPartitions, len(poiList[i]))) for i in range (0, subkeys) ]
#         templateCovs = [ np.zeros((numPartitions, len(poiList[i]), len(poiList[i]))) for i in range (0, subkeys) ]

#         if progressBar:
#             progressBar.setText('Generating Trace Matrix:')
#             progressBar.setMaximum(tend - tstart + subkeys)

#         for tnum in range(tstart, tend):
#             # partData = traceSource.getAuxData(tnum, self.partObject.attrDictPartition)["filedata"]
#             pnum = partMethod.getPartitionNum(traceSource, tnum)
#             t = traceSource.getTrace(tnum)
#             for bnum in range(0, subkeys):
#                 templateTraces[bnum][pnum[bnum]].append(t[poiList[bnum]])

#             if progressBar:
#                 progressBar.updateStatus(tnum - tstart)
#                 if progressBar.wasAborted():
#                     return None

#         if progressBar:
#             progressBar.setText('Generating Trace Covariance and Mean Matrices:')

#         for bnum in range(0, subkeys):
#             for i in range(0, numPartitions):
#                 templateMeans[bnum][i] = np.mean(templateTraces[bnum][i], axis=0)
#                 cov = np.cov(templateTraces[bnum][i], rowvar=0)
#                 if __debug__: logging.debug('templateTraces[%d][%d] = %d' % (bnum, i, len(templateTraces[bnum][i])))

#                 if len(templateTraces[bnum][i]) > 0:
#                     templateCovs[bnum][i] = cov
#                 else:
#                     logging.warning('Insufficient template data to generate covariance matrix for bnum=%d, partition=%d' % (bnum, i))
#                     templateCovs[bnum][i] = np.zeros((len(poiList[bnum]), len(poiList[bnum])))

#             if progressBar:
#                 progressBar.updateStatus(tend + bnum)
#                 if progressBar.wasAborted():
#                     return None

#         template = {
#          "mean":templateMeans,
#          "cov":templateCovs,
#          "trange":(tstart, tend),
#          "poi":poiList,
#          "partitiontype":partMethod.__class__.__name__
#         }

#         if progressBar:
#             progressBar.close()

#         return template


# class ProfilingTemplate(AlgorithmsBase):
#     """
#     Template Attack done as a loop, but using an algorithm which can progressively add traces & give output stats
#     """
#     _name= 'Template Attack'

#     def __init__(self):
#         AlgorithmsBase.__init__(self)
#         self.profiling = None

#         self.getParams().addChildren([
#             {'name':'Load Template', 'type':'group', 'children':[]},
#             {'name':'Generate New Template', 'type':'group', 'children':[
#                 {'name':'Trace Start', 'key':'tgenstart', 'value':0, 'type':'int', 'action':self.updateScript},
#                 {'name':'Trace End', 'key':'tgenstop', 'value':0, 'type':'int', 'action':self.updateScript},
#                 {'name':'POI Selection', 'key':'poimode', 'type':'list', 'values':{'TraceExplorer Table':0, 'Read from Project File':1}, 'value':0, 'action':self.updateScript},
#                 {'name':'Read POI', 'type':'action', 'action':self.updateScript},
#                 {'name':'Generate Templates', 'type':'action', 'action':util.Command(self.runScriptFunction.emit, "generateTemplates")}
#             ]},
#         ])
#         self.setProfileAlgorithm(TemplateUsingMVS)
#         self.updateScript()

#     def tracesUpdated(self, traceSource):
#         if traceSource is None:
#             return
#         traces = traceSource.numTraces()
#         tstart = self.findParam(["Generate New Template",'tgenstart'])
#         tend = self.findParam(["Generate New Template",'tgenstop'])
#         tstart.setLimits((0, traces-1))
#         tend.setLimits((0, traces-1))
#         tend.setValue(traces-1)

#         self.updateScript()

#     def setProfileAlgorithm(self, algo):
#         self.profiling = algo
#         self.updateScript()

#     def updateScript(self, _=None):
#         try:
#             ted = CWAnalyzerGUI.getInstance().traceExplorerDialog.exampleScripts[0]
#         except AttributeError:
#             logging.debug('Delaying script for template attack until TraceExplorer exists...')
#             return


#         if self.findParam(["Generate New Template",'poimode']).getValue() == 0:
#             pass
#         else:
#             poidata = self.loadPOIs()[-1]

#         profilingPath = sys.modules[self.profiling.__module__].__name__ + '.' + self.profiling.__name__

#         #Save template data to project

#         self.scriptsUpdated.emit()

#     def saveTemplatesToProject(self, trange, templatedata):
#         cfgsec = self.project().addDataConfig(sectionName="Template Data", subsectionName="Templates")
#         cfgsec["tracestart"] = trange[0]
#         cfgsec["traceend"] = trange[1]
#         cfgsec["poi"] = templatedata["poi"]
#         cfgsec["partitiontype"] = templatedata["partitiontype"]

#         fname = self.project().getDataFilepath('templates-%s-%d-%d.npz' % (cfgsec["partitiontype"], trange[0], trange[1]), 'analysis')

#         # Save template file
#         np.savez(fname["abs"], **templatedata)  # mean=self.profiling.templateMeans, cov=self.profiling.templateCovs)
#         cfgsec["filename"] = fname["rel"]

#         logging.info('Saved template to: %s' % fname["abs"])

#         return fname["abs"]

#     def loadTemplatesFromProject(self):
#         # Load Template
#         foundsecs = self.project().getDataConfig(sectionName="Template Data", subsectionName="Templates")
#         templates = []

#         for f in foundsecs:
#             fname = self.project().convertDataFilepathAbs(f["filename"])
#             t = np.load(fname)
#             templates.append(t)

#             # Validate in case someone tries to change via project file
#             if f["partitiontype"] != t["partitiontype"]:
#                 logging.warning('PartitionType for template from .npz file (%s) differs from project file (%s). npz file being used.')

#             if (util.strListToList(str(f["poi"])) != t["poi"]).any():
#                 logging.warning('POI for template from .npz file (%s) differs from project file (%s). npz file being used.')

#         return templates

#     def loadPOIs(self):
#         section = self.project().getDataConfig("Template Data", "Points of Interest")

#         poiList = []

#         for s in section:
#             poistr = str(s["poi"])
#             poieval = util.strListToList(poistr)
#             poiList.append(s.copy())
#             poiList[-1]["poi"] = poieval

#         return poiList

#     def addTraces(self, traceSource, tracerange, progressBar=None, pointRange=None):
#         data = []
#         textins = []
#         textouts = []

#         for i in range(tracerange[0], tracerange[1]+1):
#             d = traceSource.getTrace(i)

#             if d is None:
#                 continue

#             startingPoint, endingPoint = pointRange  # TODO:support start/end point different per byte
#             d = d[startingPoint:endingPoint]

#             data.append(d)
#             textins.append(traceSource.getTextin(i))
#             textouts.append(traceSource.getTextout(i))

#         try:
#             from scipy.stats import multivariate_normal
#         except ImportError:
#             raise Warning("Version of SciPy too old, require >= 0.14, have %s. "
#                           "Update to support this attack" % (scipy.version.version))

#         # Hack for now - just use last template found
#         template = self.loadTemplatesFromProject()[-1]
#         pois = template["poi"]
#         numparts = len(template['mean'][0])
#         results = np.zeros((self.model.getNumSubKeys(), self.model.getPermPerSubkey()))

#         if progressBar:
#             progressBar.setStatusMask("Current Trace = %d Current Subkey = %d", (0, 0))
#             progressBar.setMaximum(self.model.getNumSubKeys() * len(data))
#         pcnt = 0

#         for tnum in range(0, len(data)):
#             for bnum in self.brange:
#                 try:
#                     newresultsint = [multivariate_normal.logpdf(data[tnum][pois[bnum]], mean=template['mean'][bnum][i], cov=np.diag(template['cov'][bnum][i])) for i in range(0, numparts)]
#                 except np.linalg.LinAlgError as e:
#                     logging.warning('Error in applying template, probably template is poorly formed or POI incorrect. Byte %d for tnum %d skipped.' % (bnum, tnum))
#                     logging.debug(e)
#                     newresultsint = [0] * self.model.getPermPerSubkey()

#                 ptype = template["partitiontype"]

#                 newresults = []
#                 # Map to key guess format
#                 for i in range(0, self.model.getPermPerSubkey()):
#                     if ptype == "PartitionHWIntermediate":
#                         self.model.setHwModel(self.model.hwModels['HW: AES SBox Output, First Round (Enc)'])
#                         hypint = self.model.leakage(textins[tnum], textouts[tnum], i, bnum, None)
#                     elif ptype == "PartitionHDLastRound":
#                         self.model.setHwModel(self.model.hwModels['HD: AES Last-Round State'])
#                         hypint = self.model.leakage(textins[tnum], textouts[tnum], i, bnum, None)
#                     # TODO Temp
#                     elif ptype == "PartitionHDRounds":
#                         if bnum == 0:
#                             hypint = self.model.getHW(textins[tnum][bnum] ^ i)
#                         else:
#                             knownkey = [0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c]
#                             s1 = textins[tnum][bnum - 1] ^ knownkey[bnum - 1]
#                             s2 = textins[tnum][bnum] ^ i
#                             hypint = self.model.getHW(s1 ^ s2)
#                     else:
#                         hypint = i
#                     newresults.append(newresultsint[hypint])

#                 results[bnum] += newresults
#                 self.stats.updateSubkey(bnum, results[bnum], tnum=(tnum + 1))

#                 pcnt += 1
#                 if progressBar:
#                     progressBar.updateStatus(pcnt, (tnum, bnum))
#                     if progressBar.wasAborted():
#                         return

#             # Do plotting if required
#             if (tnum % self._reportingInterval) == 0 and self.sr:
#                 self.sr()

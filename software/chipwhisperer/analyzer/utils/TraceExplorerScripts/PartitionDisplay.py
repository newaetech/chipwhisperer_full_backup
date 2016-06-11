#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2014, NewAE Technology Inc
# All rights reserved.
#
# Author: Colin O'Flynn
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

from functools import partial
import numpy as np
import copy
from PySide.QtCore import *
from PySide.QtGui import *
import chipwhisperer.common.utils.qt_tweaks as QtFixes
import pyqtgraph as pg
from chipwhisperer.analyzer.utils.Partition import Partition
from chipwhisperer.common.utils import util
from chipwhisperer.common.api.autoscript import AutoScript
from chipwhisperer.common.api.CWCoreAPI import CWCoreAPI
from chipwhisperer.common.ui.GraphWidget import GraphWidget
from chipwhisperer.common.utils.parameter import Parameterized, Parameter, setupSetParam
from chipwhisperer.common.ui.CWMainGUI import CWMainGUI


class DifferenceModeTTest(object):
    sectionName = "Difference of Partitions using Welch's T-Test"
    moduleName = "PartitionDifferencesWelchTTest"
    differenceType = "Welch's T-Test"

    def difference(self, numkeys, numparts, trace, numpoints, stats, pbDialog=None):
        means = stats["mean"]
        var = stats["variance"]
        num = stats["number"]

        if pbDialog:
            pbDialog.setMinimum(0)
            pbDialog.setMaximum(numkeys * numparts)

        # TODO: This is because we run through loop multiple times
        # When numparts == 2 (default for rand vs. fixed), we actually
        # run loop 4 times I think, but with all same data (i.e. part1 vs part2, part2 vs part1)
        # need to verify this...
        scalingFactor = 1.0 / (numparts * numparts)

        SADSeg = np.zeros((numkeys, numpoints))
        for bnum in range(0, numkeys):
            for i in range(0, numparts):
                if pbDialog:
                    pbDialog.updateStatus(numparts * bnum + i)
                    util.updateUI()
                    if pbDialog.wasAborted():
                        return SADSeg
                for j in range(0, numparts):
                    if means[bnum][i] is not None and means[bnum][j] is not None:

                        ttest = np.subtract(means[bnum][i], means[bnum][j])
                        ttest /= np.sqrt((var[bnum][i]/num[bnum][i]) + (var[bnum][j]/num[bnum][j]))

                        # if t-test is NaN indicates perhaps exact same data
                        ttest = np.nan_to_num(ttest)

                        SADSeg[bnum] = np.add(SADSeg[bnum], np.abs(ttest) * scalingFactor)

        if pbDialog:
            pbDialog.updateStatus(numkeys * numparts)

        return SADSeg


class DifferenceModeSAD(object):
    sectionName = "Difference of Partitions using SAD"
    moduleName = "PartitionDifferencesSAD"
    differenceType = "Sum of Absolute Difference"

    def difference(self, numkeys, numparts, trace, numpoints, stats, pbDialog=None):

        averageData = stats["mean"]

        if pbDialog:
            pbDialog.setMinimum(0)
            pbDialog.setMaximum(numkeys * numparts)

        SADSeg = np.zeros((numkeys, numpoints))
        for bnum in range(0, numkeys):
            for i in range(0, numparts):
                if pbDialog:
                    pbDialog.updateStatus(numparts * bnum + i)
                    util.updateUI()
                    if pbDialog.wasAborted():
                        return SADSeg
                for j in range(0, numparts):
                    if averageData[bnum][i] is not None and averageData[bnum][j] is not None:
                        SADSeg[bnum] = np.add(SADSeg[bnum], np.abs(np.subtract(averageData[bnum][i], averageData[bnum][j])))
        return SADSeg


class DifferenceMode(object):
    attrDictCombination = {
                "sectionName":"Difference Based on XXXX",
                "moduleName":"PartitionDifferences",
                "module":None,
                "values":{
                    "mode":{"value":"sad", "desc":"How Differences are Generated", "changed":False, "definesunique":True},
                    "partmode":{"value":"0", "desc":"Partition Mode in Use", "changed":False, "definesunique":True},
                    "combomode":{"value":"0", "desc":"Partition Mode in Use", "changed":False, "definesunique":True},
                    "filename":{"value":None, "desc":"Combination File", "changed":False, "headerLabel":"Difference Data"},
                    },
                }

    supportedMethods = [DifferenceModeTTest, DifferenceModeSAD]

    def __init__(self):
        super(DifferenceMode, self).__init__()
        self.setDiffMethod(self.supportedMethods[0])

    def setDiffMethod(self, mode):
        self.mode = mode()
        self.diffMethodClass = mode
        self.attrDictCombination["sectionName"] = self.mode.sectionName
        self.attrDictCombination["moduleName"] = self.mode.moduleName
        # self.attrDictCombination["values"]["mode"]["value"] = self.

    def difference(self, numkeys, numparts, trace, numpoints, stats, pbDialog=None):
        self.data = self.mode.difference(numkeys, numparts, trace, numpoints, stats, pbDialog)
        return self.data

    def save(self, trace):
        newCfgDict = copy.deepcopy(self.attrDictCombination)
        updatedDict = trace.addAuxDataConfig(newCfgDict)
        trace.saveAuxData(self.data, updatedDict)

    def load(self, trace):
        # Check if trace has stuff
        cfg = trace.getAuxDataConfig(self.attrDictCombination)
        if cfg is None:
            return None
        return trace.loadAuxData(cfg["filename"])


class POI(QWidget):
    def __init__(self, parent):
        super(POI, self).__init__()

        self.parent = parent

        layout = QVBoxLayout()

        self.setWindowTitle('Point of Interest Selection')

        self.mainTable = QTableWidget()

        layout.addWidget(self.mainTable)
        pbSave = QPushButton('Set as POI in Project')
        pbSave.clicked.connect(self.savePOI)
        pbCalc = QPushButton('Recalc POI Values')
        pbCalc.clicked.connect(self.parent.updatePOI)
        # pbSaveNPY = QPushButton('Save to NPY File')
        # pbLoadNPY = QPushButton('Load NPY File')

        pbLayout = QHBoxLayout()
        pbLayout.addWidget(pbSave)
        pbLayout.addWidget(pbCalc)
        # pbLayout.addWidget(pbSaveNPY)
        # pbLayout.addWidget(pbLoadNPY)

        layout.addLayout(pbLayout)
        self.setLayout(layout)
        self.diffs = []
        self.poiArray = []

    def setDifferences(self, diffs):
        self.diffs = diffs
        self.parent.findParam(["Points of Interest",'poi-pointrng']).setValue((0, len(self.parent.SADList[0])))

    def savePOI(self):
        poiDict = {"poi":self.poiArray, "partitiontype":self.parent.partObject.partMethod.__class__.__name__}
        CWCoreAPI.getInstance().project().addDataConfig(poiDict, "Template Data", "Points of Interest")

    def calcPOI(self, numMax, pointRange, minSpace, diffs=None):
        if diffs:
            self.setDifferences(diffs)

        # Setup Table for current stuff
        self.mainTable.setRowCount(len(self.diffs))
        self.mainTable.setColumnCount(2)
        self.mainTable.setHorizontalHeaderLabels(["Subkey Number", "Point List"])
        self.mainTable.verticalHeader().hide()

        self.poiArray = []

        startPoint = pointRange[0]
        endPoint = pointRange[1]

        if startPoint == endPoint:
            endPoint += 1

        for bnum in range(0, len(self.diffs)):

            maxarray = []

            # Copy since we will be overwriting it a bunch
            data = copy.deepcopy(self.diffs[bnum][startPoint:endPoint])

            while len(maxarray) < numMax:
                # Find maximum location
                mloc = np.argmax(data)

                # Store this maximum
                maxarray.append(mloc + startPoint)

                # set to -INF data within +/- the minspace
                mstart = max(0, mloc - minSpace)
                mend = min(mloc + minSpace, len(data))
                data[mstart:mend] = -np.inf

            # print maxarray
            self.poiArray.append(maxarray)

            self.mainTable.setItem(bnum, 0, QTableWidgetItem("%d" % bnum))
            self.mainTable.setCellWidget(bnum, 1, QtFixes.QLineEdit(str(maxarray)))
        return {"poi":self.poiArray}


class PartitionDisplay(Parameterized, AutoScript):
    _name = "Partition Comparison"

    def __init__(self, parent):
        AutoScript.__init__(self)
        self._autoscript_init = False
        self.parent = parent
        self.poi = POI(self)
        self.poiDock = CWMainGUI.getInstance().addDock(self.poi, "Partition Comparison POI Table", area=Qt.TopDockWidgetArea)
        self.poiDock.hide()
        self.defineName()
        self._traces = None

        self.api = CWCoreAPI.getInstance()
        self.graph = GraphWidget()
        self.bselection = QToolBar()
        self.graph.addWidget(self.bselection)
        self.graphDock = CWMainGUI.getInstance().addDock(self.graph, "Partition Comparison Graph", area=Qt.TopDockWidgetArea)
        self.graphDock.hide()

    def defineName(self):
        self.partObject = Partition(self)
        partModeList = {}
        for a in self.partObject.supportedMethods:
            partModeList[a.partitionType] = a

        self.diffObject = DifferenceMode()
        diffModeList = {}
        for a in self.diffObject.supportedMethods:
            diffModeList[a.differenceType] = a

        self.addGroup("generatePartitions")
        self.addGroup("generatePartitionStats")
        self.addGroup("generatePartitionDiffs")
        self.addGroup("displayPartitionDiffs")

        self.getParams().addChildren([
              {'name':'Comparison Mode', 'key':'diffmode', 'type':'list', 'values':diffModeList, 'value':self.diffObject.diffMethodClass, 'action':lambda _: self.updateScript()},
              {'name':'Partition Mode', 'key':'partmode', 'type':'list', 'values':partModeList, 'value':self.partObject.partMethodClass, 'action':lambda _: self.updateScript()},
              {'name':'Display', 'type':'action', 'action':lambda _:self.runAction()},

              {'name':'Auto-Save Data to Project', 'key':'part-saveints', 'type':'bool', 'value':False, 'action':lambda _: self.updateScript()},
              {'name':'Auto-Load Data from Project', 'key':'part-loadints', 'type':'bool', 'value':False, 'action':lambda _: self.updateScript()},

              {'name':'Points of Interest', 'key':'poi', 'type':'group', 'children':[
                 {'name':'Selection Mode', 'type':'list', 'values':{'Max N Points/Subkey':'maxn'}, 'value':'maxn'},
                 {'name':'Point Range', 'key':'poi-pointrng', 'type':'range', 'limits':(0, 0), 'default':(0, 0), 'value':(0, 0), 'action':lambda _: self.updatePOI()},
                 {'name':'Num POI/Subkey', 'key':'poi-nummax', 'type':'int', 'limits':(1, 200), 'value':1, 'action':lambda _: self.updatePOI()},
                 {'name':'Min Spacing between POI', 'key':'poi-minspace', 'type':'int', 'limits':(1, 100E6), 'value':1, 'step':100, 'action':lambda _: self.updatePOI()},
                 # {'name':'Threshold', 'key':'threshold', 'type':'int', 'visible':False},
                 {'name':'Open POI Table', 'type':'action', 'action':lambda _: self.poiDock.show()},
              ]},
        ])

    def updatePOI(self, ignored=None):
        self.updateScript()

        if self._autoscript_init == False:
            return

        # Some sort of race condition - applying Therac-25 type engineering and just
        # randomly hope this is enough delay
        QTimer.singleShot(500, lambda:self.runScriptFunction.emit("TraceExplorerDialog_PartitionDisplay_findPOI"))

    def setBytePlot(self, num, sel):
        self.enabledbytes[num] = sel
        if self.doRedraw:
            self.redrawPlot()

    def setByteAll(self, status):
        self.doRedraw = False
        for i, t in enumerate(self.byteNumAct):
            t.defaultWidget().setChecked(status)
            self.setBytePlot(i, status)
        self.doRedraw = True
        self.redrawPlot()

    def redrawPlot(self):
        self.graph.clearPushed()

        for bnum in range(0, self.numKeys):
            if self.enabledbytes[bnum]:
                self.graph.setColorInt(bnum, self.numKeys)
                self.graph.passTrace(self.SADList[bnum], pen=pg.mkPen(pg.intColor(bnum, 16)), idString = str(bnum))

    def updateScript(self, ignored=None):
        ##Partitioning & Differences
        try:
            diffMethodStr = self.findParam('diffmode').getValue().__name__
            partMethodStr = self.findParam('partmode').getValue().__name__
        except AttributeError as e:
            return

        self.importsAppend('from chipwhisperer.analyzer.utils.Partition import PartitionRandDebug, PartitionRandvsFixed, PartitionEncKey, PartitionHWIntermediate, PartitionHDLastRound')
        self.importsAppend('from chipwhisperer.analyzer.utils.TraceExplorerScripts.PartitionDisplay import DifferenceModeTTest, DifferenceModeSAD')
        self.importsAppend('from chipwhisperer.analyzer.ui.CWAnalyzerGUI import CWAnalyzerGUI')

        self.addGroup("displayPartitionStats")
        self.addVariable('displayPartitionStats', 'ted', 'self.')
        self.addFunction('displayPartitionStats', 'setTraceSource', 'UserScript.traces', obj='ted')
        self.addFunction('displayPartitionStats', 'parent.getProgressIndicator', '', 'progressBar', obj='ted')
        self.addFunction('displayPartitionStats', 'partObject.setPartMethod', partMethodStr, obj='ted')
        self.addFunction('displayPartitionStats', 'partObject.generatePartitions', 'saveFile=True, loadFile=False', 'partData', obj='ted')
        self.addFunction('displayPartitionStats', 'generatePartitionStats',
                            'partitionData={"partclass":%s, "partdata":partData}, saveFile=True, progressBar=progressBar' % partMethodStr,
                            'partStats', obj='ted')
        self.addFunction('displayPartitionStats', 'generatePartitionDiffs',
                            '%s, statsInfo={"partclass":%s, "stats":partStats}, saveFile=True, loadFile=False, progressBar=progressBar'%
                            (diffMethodStr, partMethodStr),
                            'partDiffs', obj='ted')
        self.addFunction('displayPartitionStats', 'displayPartitions', 'differences={"partclass":%s, "diffs":partDiffs}' % partMethodStr, obj='ted')
        self.addFunction('displayPartitionStats', 'poi.setDifferences', 'partDiffs', obj='ted')

        ##Points of Interest
        ptrng = self.findParam(["Points of Interest",'poi-pointrng']).getValue()
        self.addGroup("findPOI")
        self.addVariable('findPOI', 'ted', 'self.')
        self.addFunction('findPOI', 'poi.calcPOI', 'numMax=%d, pointRange=(%d, %d), minSpace=%d' % (
                            self.findParam(["Points of Interest",'poi-nummax']).getValue(),
                            ptrng[0], ptrng[1],
                            self.findParam(["Points of Interest",'poi-minspace']).getValue()),
                          obj='ted')

        #Check if this updateScript was called as a result of showing the TraceExplorer window
        if ignored == "traceexplorer_show":
            self._autoscript_init = True

    def generatePartitionStats(self, partitionData={"partclass":None, "partdata":None}, saveFile=False, loadFile=False,  tRange=(0, -1), progressBar=None):

        traces = self._traces

        if tRange[1] < 0:
            tRange = (tRange[0], traces.numTraces() + 1 + tRange[1])

        self.partObject.setPartMethod(partitionData["partclass"])

        self.numKeys = len(self.partObject.partMethod.getPartitionNum(traces, 0))

        numPoints = traces.numPoints()

        if loadFile:
            cfgsecs = self.api.project().getDataConfig(sectionName="Trace Statistics", subsectionName="Total Trace Statistics")
            foundsecs = []
            for cfg in cfgsecs:
                desiredsettings = {}
                desiredsettings["tracestart"] = tRange[0]
                desiredsettings["traceend"] = tRange[1]
                desiredsettings["partitiontype"] = self.partObject.partMethod.__class__.__name__
                if self.api.project().checkDataConfig(cfg, desiredsettings):
                    foundsecs.append(cfg)
        else:
            foundsecs = []

        if len(foundsecs) > 1:
            IOError("Too many sections!!!")
        elif len(foundsecs) == 1:
            fname = self.api.project().convertDataFilepathAbs(foundsecs[0]["filename"])
            stats = np.load(fname)
        else:
            # Array to hold average + stddev of all traces/partitions
            A_k = []
            A_j = []
            Q_k = []
            dataArrays = [A_k, A_j, Q_k]
            ACnt = []
            for bnum in range(0, self.numKeys):
                for d in dataArrays:
                    d.append([])
                ACnt.append([])
                for i in range(0, self.partObject.partMethod.getNumPartitions()):
                    for d in dataArrays:
                        d[bnum].append(np.zeros(numPoints))
                    ACnt[bnum].append(0)

            # Get segment list
            segList = traces.getSegmentList()

            if progressBar:
                progressBar.setWindowTitle("Phase 1: Trace Statistics")
                progressBar.setMaximum(bnum)  # len(segList['offsetList']) * self.numKeys)
                progressBar.show()

            # TODO: Double-check this fix
            # for tsegn, segtrace in enumerate(segList['offsetList']):
            tsegn = 0
            if progressBar: progressBar.setText("Segment %d" % tsegn)

            # Average data needs to be calculated
            # Require partition list
            partData = partitionData["partdata"]


            # print "Calculating Average + Std-Dev"
            # Std-Dev calculation:
            # A[0] = 0
            # A[k] = A[k-1] + (x[k] - A[k-1]) / k
            # Q[0] = 0
            # Q[k] = Q[k-1] + (x[k] - A[k-1])(x[k] - A[k])
            for bnum in range(0, self.numKeys):
                progressBar.updateStatus(tsegn * self.numKeys + bnum)
                if progressBar.wasAborted():
                    break
                for i in range(0, self.partObject.partMethod.getNumPartitions()):
                    util.updateUI()
                    tlist = partData[bnum][i]
                    if len(tlist) > 0:
                        for tnum in tlist:
                            if tnum > tRange[0] and tnum < tRange[1]:
                                t = traces.getTrace(tnum)
                                ACnt[bnum][i] += 1
                                A_k[bnum][i] = A_k[bnum][i] + (t - A_j[bnum][i]) / ACnt[bnum][i]
                                Q_k[bnum][i] = Q_k[bnum][i] + ((t - A_j[bnum][i]) * (t - A_k[bnum][i]))
                                A_j[bnum][i] = A_k[bnum][i]

            if progressBar.wasAborted():
                progressBar.hide()
                return

            # Finally get variance
            for bnum in range(0, self.numKeys):
                    for i in range(0, self.partObject.partMethod.getNumPartitions()):
                        # TODO: Should be using population variance or sample variance (e.g. /n or /n-1)?
                        #      Since this is taken over very large sample sizes I imagine it won't matter
                        #      ultimately.
                        Q_k[bnum][i] = Q_k[bnum][i] / max(ACnt[bnum][i] - 1, 1)

            # Average is in A_k
            stats = {"mean":A_k, "variance":Q_k, "number":ACnt}

            # Wasn't cancelled - save this to project file for future use if requested
            if saveFile:
                progressBar.setText("Saving Mean/Variance Partitions")
                cfgsec = self.api.project().addDataConfig(sectionName="Trace Statistics", subsectionName="Total Trace Statistics")
                cfgsec["tracestart"] = tRange[0]
                cfgsec["traceend"] = tRange[1]
                cfgsec["partitiontype"] = self.partObject.partMethod.__class__.__name__
                fname = self.api.project().getDataFilepath('tracestats-%s-%d-%s.npz' % (cfgsec["partitiontype"], tRange[0], tRange[1]), 'analysis')

                # Save mean/variance for trace
                np.savez(fname["abs"], mean=A_k, variance=Q_k, number=ACnt)
                cfgsec["filename"] = fname["rel"]

        progressBar.hide()
        return stats

    def generatePartitionDiffs(self, diffModule, statsInfo={"partclass":None, "stats":None}, saveFile=False, loadFile=False, tRange=(0, -1), progressBar=None):

        traces = self._traces

        if tRange[1] < 0:
            tRange = (tRange[0], traces.numTraces() + 1 + tRange[1])

        self.partObject.setPartMethod(statsInfo["partclass"])
        self.diffObject.setDiffMethod(diffModule)

        self.numKeys = len(self.partObject.partMethod.getPartitionNum(traces, 0))

        numPoints = traces.numPoints()

        foundsecs = []
        if loadFile:
            cfgsecs = self.api.project().getDataConfig(sectionName="Trace Statistics", subsectionName="Partition Differences")
            for cfg in cfgsecs:
                desiredsettings = {}
                desiredsettings["tracestart"] = tRange[0]
                desiredsettings["traceend"] = tRange[1]
                desiredsettings["partitiontype"] = self.partObject.partMethod.__class__.__name__
                desiredsettings["comparetype"] = self.diffObject.mode.moduleName
                if self.api.project().checkDataConfig(cfg, desiredsettings):
                    foundsecs.append(cfg)
        else:
            cfgsecs = []

        if len(foundsecs) > 1:
            IOError("Too many sections!!!")
        elif len(foundsecs) == 1:
            fname = self.api.project().convertDataFilepathAbs(foundsecs[0]["filename"])
            SADList = np.load(fname)
        else:
            progressBar.setWindowTitle("Phase 2: Calculating Partition Differences")
            progressBar.setText("Calculating all Differences based on " + self.diffObject.mode.differenceType)
            SADList = self.diffObject.difference(self.numKeys, self.partObject.partMethod.getNumPartitions(), None, numPoints, statsInfo["stats"], progressBar)

            if saveFile:
                cfgsec = self.api.project().addDataConfig(sectionName="Trace Statistics", subsectionName="Partition Differences")
                cfgsec["tracestart"] = tRange[0]
                cfgsec["traceend"] = tRange[1]
                cfgsec["partitiontype"] = self.partObject.partMethod.__class__.__name__
                cfgsec["comparetype"] = self.diffObject.mode.moduleName
                fname = self.api.project().getDataFilepath('partdiffs-%s-%s-%d-%s.npy' % (cfgsec["partitiontype"], cfgsec["comparetype"], tRange[0], tRange[1]), 'analysis')
                np.save(fname["abs"], SADList)
                cfgsec["filename"] = fname["rel"]

        progressBar.updateStatus(progressBar.maximum)
        progressBar.setWindowTitle('Debug Fail')
        progressBar.setText('You should never see this text')
        if progressBar.wasAborted():
            return

        self.SADList = SADList

        return SADList

    def displayPartitions(self, differences={"partclass":None, "diffs":None}, tRange=(0, -1)):
        self.graphDock.show()
        traces = self._traces

        if tRange[1] < 0:
            tRange = (tRange[0], traces.numTraces() + 1 + tRange[1])

        self.partObject.setPartMethod(differences["partclass"])
        self.numKeys = len(self.partObject.partMethod.getPartitionNum(traces, 0))
        self.SADList = differences["diffs"]

        # Place byte selection option on graph
        if hasattr(self, 'enabledbytes') and len(self.enabledbytes) == self.numKeys:
            pass
        else:
            self.enabledbytes = [False] * self.numKeys

        self.doRedraw = True

        self.byteNumAct = []
        for i in range(0, self.numKeys):
            ql = QToolButton()
            ql.setText('%d' % i)
            color = pg.intColor(i, self.numKeys)
            ql.setStyleSheet("color: rgb(%d, %d, %d)" % (color.red(), color.green(), color.blue()))
            qa = QWidgetAction(self.graph)
            qa.setDefaultWidget(ql)
            qa.setStatusTip('%d' % i)
            ql.setCheckable(True)
            ql.setChecked(self.enabledbytes[i])
            ql.clicked[bool].connect(partial(self.setBytePlot, i))
            self.byteNumAct.append(qa)

        byteNumAllOn = QAction('All On', self.graph)
        byteNumAllOff = QAction('All Off', self.graph)
        byteNumAllOn.triggered.connect(partial(self.setByteAll, True))
        byteNumAllOff.triggered.connect(partial(self.setByteAll, False))

        self.bselection.clear()
        for i in range(0, self.numKeys):
            self.bselection.addAction(self.byteNumAct[i])
        self.bselection.addAction(byteNumAllOn)
        self.bselection.addAction(byteNumAllOff)
        self.graph.setPersistance(True)

        self.poi.setDifferences(self.SADList)

        self.findParam(["Points of Interest",'poi-pointrng']).setLimits((0, len(self.SADList[0])))
        self.findParam(["Points of Interest",'poi-pointrng']).setValue((0, len(self.SADList[0])))
        self.redrawPlot()

    def runAction(self):
        self.runScriptFunction.emit('TraceExplorerDialog_PartitionDisplay_displayPartitionStats')

    def setTraceSource(self, traces):
        self._traces = traces
        self.partObject.setTraceSource(self._traces)
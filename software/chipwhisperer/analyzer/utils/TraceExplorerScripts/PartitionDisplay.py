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

try:
    from PySide.QtCore import *
    from PySide.QtGui import *
except ImportError:
    print "ERROR: PySide is required for this program"
    sys.exit()

from chipwhisperer.analyzer.utils.Partition import Partition
from functools import partial
import numpy as np
import copy

class CombinationModeAverage(QObject):
    sectionName = "Combination of Partitions using Average"
    moduleName = "CombinationAverage"
    combinationType = "Average"

    def combine(self, numkeys, numparts, trace, numpoints, partdata):
        averageData = []
        for bnum in range(0, numkeys):
            averageData.append([])
            for i in range(0, numparts):
                averageData[bnum].append(np.zeros(numpoints))
                tlist = partdata[bnum][i]
                if len(tlist) == 0:
                    averageData[bnum][i] = None
                else:
                    for tnum in tlist:
                        averageData[bnum][i] = np.add(averageData[bnum][i], trace.getTrace(tnum))
                    averageData[bnum][i] = averageData[bnum][i] / len(tlist)

        self.data = averageData
        return averageData

class CombinationMode(QObject):
    attrDictCombination = {
                "sectionName":"Combinations Based on XXXX",
                "moduleName":"Combinations",
                "module":None,
                "values":{
                    "mode":{"value":"average", "desc":"How Partitions are Combined", "changed":False, "definesunique":True},
                    "partmode":{"value":"0", "desc":"Partition Mode in Use", "changed":False, "definesunique":True},
                    "filename":{"value":None, "desc":"Combination File", "changed":False, "headerLabel":"Combination Data"},
                    },
                }

    supportedMethods = [CombinationModeAverage]
    
    def __init__(self):
        super(CombinationMode, self).__init__()
        self.setCombMethod(self.supportedMethods[0])

    def combine(self, numkeys, numparts, trace, numpoints, partdata):
        self.data = self.mode.combine(numkeys, numparts, trace, numpoints, partdata)
        return self.data
        
    def setCombMethod(self, mode):
        self.mode = mode()
        self.combMethodClass = mode
        self.attrDictCombination["sectionName"] = self.mode.sectionName
        self.attrDictCombination["moduleName"] = self.mode.moduleName
        # self.attrDictCombination["values"]["mode"]["value"] = self.

    def save(self, trace):
        newCfgDict = copy.deepcopy(self.attrDictCombination)

        updatedDict = trace.addAuxDataConfig(newCfgDict)
        trace.saveAuxData(self.data, updatedDict)
        
    def load(self, trace):
        #Check if trace has stuff
        cfg = trace.getAuxDataConfig(self.attrDictCombination)

        if cfg is None:
            return None
        
        return trace.loadAuxData(cfg["filename"])

class DifferenceModeSAD(QObject):
    sectionName = "Difference of Partitions using SAD"
    moduleName = "PartitionDifferencesSAD"
    differenceType = "Sum of Absolute Difference"

    def difference(self, numkeys, numparts, trace, numpoints, averageData):
        SADSeg = np.zeros((numkeys, numpoints))
        for bnum in range(0, numkeys):
            for i in range(0, numparts):
                for j in range(0, numparts):
                    if averageData[bnum][i] is not None and averageData[bnum][j] is not None:
                        SADSeg[bnum] = np.add(SADSeg[bnum], np.abs(np.subtract(averageData[bnum][i], averageData[bnum][j])))
        return SADSeg


class DifferenceMode(QObject):
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

    supportedMethods = [DifferenceModeSAD]

    def __init__(self):
        super(DifferenceMode, self).__init__()
        self.setDiffMethod(self.supportedMethods[0])

    def setDiffMethod(self, mode):
        self.mode = mode()
        self.diffMethodClass = mode
        self.attrDictCombination["sectionName"] = self.mode.sectionName
        self.attrDictCombination["moduleName"] = self.mode.moduleName
        # self.attrDictCombination["values"]["mode"]["value"] = self.

    def difference(self, numkeys, numparts, trace, numpoints, averageData):
        self.data = self.mode.difference(numkeys, numparts, trace, numpoints, averageData)
        return self.data

    def save(self, trace):
        newCfgDict = copy.deepcopy(self.attrDictCombination)
        updatedDict = trace.addAuxDataConfig(newCfgDict)
        trace.saveAuxData(self.data, updatedDict)

    def load(self, trace):
        # Check if trace has stuff
        cfg = trace.getAuxDataConfig(self.attrDictCombination)

        print cfg

        if cfg is None:
            return None

        return trace.loadAuxData(cfg["filename"])


class PartitionDisplay(QObject):

    def __init__(self, parent):
        super(PartitionDisplay, self).__init__(parent)
        self.parent = parent
        self.defineName()

    def defineName(self):
        self.name = 'Partition Comparison'
        
        self.partObject = Partition(None)
        partModeList = {}
        for a in self.partObject.supportedMethods:
            partModeList[a.partitionType] = a

        self.combObject = CombinationMode()
        combModeList = {}
        for a in self.combObject.supportedMethods:
            combModeList[a.combinationType] = a

        self.diffObject = DifferenceMode()
        diffModeList = {}
        for a in self.diffObject.supportedMethods:
            diffModeList[a.differenceType] = a
                    
        self.params = [
              {'name':'Comparison Mode', 'key':'diffmode', 'type':'list', 'values':diffModeList, 'value':self.diffObject.diffMethodClass, 'set':self.diffObject.setDiffMethod},
              {'name':'Partition Mode', 'key':'partmode', 'type':'list', 'values':partModeList, 'value':self.partObject.partMethodClass, 'set':self.partObject.setPartMethod},
              {'name':'Combination Mode', 'key':'combomode', 'type':'list', 'values':combModeList, 'value':self.combObject.combMethodClass, 'set':self.combObject.setCombMethod},
              {'name':'Display', 'type':'action', 'action':self.runAction}
             ]

    def setBytePlot(self, num, sel):
        self.enabledbytes[num] = sel
        if self.doRedraw:
            self.redrawPlot()

    def setByteAll(self, status):
        self.doRedraw = False
        for t in self.byteNumAct:
            t.setChecked(status)
            t.trigger()
        self.doRedraw = True
        self.redrawPlot()

    def redrawPlot(self):
        self.graph.clearPushed()

        for bnum in range(0, self.numKeys):
            if self.enabledbytes[bnum]:
                self.graph.passTrace(self.SADList[bnum])

    def runAction(self):
        # Get traces
        traces = self.parent.getTraceSource()
        self.numKeys = len(traces.findMappedTrace(0).getKnownKey())

        # Get Plotting Figure
        self.graph = self.parent.getGraphWidgets(["Partition Differences"])[0]

        self.enabledbytes = [False] * self.numKeys
        self.doRedraw = True

        self.byteNumAct = []
        for i in range(0, self.numKeys):
            self.byteNumAct.append(QAction('%d' % i, self))
            self.byteNumAct[i].triggered[bool].connect(partial(self.setBytePlot, i))
            self.byteNumAct[i].setCheckable(True)

        byteNumAllOn = QAction('All On', self)
        byteNumAllOff = QAction('All Off', self)
        byteNumAllOn.triggered.connect(partial(self.setByteAll, False))
        byteNumAllOff.triggered.connect(partial(self.setByteAll, True))

        self.bselection = QToolBar()

        for i in range(0, self.numKeys):
            self.bselection.addAction(self.byteNumAct[i])
        self.bselection.addAction(byteNumAllOn)
        self.bselection.addAction(byteNumAllOff)
        self.graph.layout().addWidget(self.bselection)

        numPoints = traces.findMappedTrace(0).numPoints()

        #Make array to hold SAD data
        SADList = np.zeros((self.numKeys, numPoints))
        
        #Get segment list
        segList = traces.getTraceList(end=4000)

        print len(segList['traceList'])

        for i, titem in enumerate(segList['traceList']):

            SADSeg = self.diffObject.load(titem)
            if SADSeg is None:
                # Calculate SAD data and save to trace file

                # Require average data
                averageData = self.combObject.load(titem)

                if averageData is None:
                    # Average data needs to be calculated
                    # Require partition list
                    cfg = titem.getAuxDataConfig(self.partObject.attrDictPartition)
                    if cfg is not None:
                        # Partition data already existing
                        partData = titem.loadAuxData(cfg["filename"])
                    else:
                        # Partition data needs to be calculated
                        print "Generating Partition Data"

                    # Combine Data
                    averageData = self.combObject.combine(self.numKeys, self.partObject.partMethod.getNumPartitions(), titem, numPoints, partData)
                    # Save data to traces
                    self.combObject.save(titem)

                # Calculate SAD over all combinations (long!)
                SADSeg = self.diffObject.difference(self.numKeys, self.partObject.partMethod.getNumPartitions(), None, numPoints, averageData)
                # Save SAD data to traces
                self.diffObject.save(titem)

            # Combine SAD across segments
            for bnum in range(0, self.numKeys):
                SADList[bnum] += SADList[bnum] + SADSeg[bnum]

        self.SADList = SADList

                

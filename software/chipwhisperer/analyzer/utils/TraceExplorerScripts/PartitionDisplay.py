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
        if cfg is None:
            return None
        return trace.loadAuxData(cfg["filename"])

class POI(QWidget):
    def __init__(self, parent):
        super(POI, self).__init__()

        layout = QVBoxLayout()

        self.setWindowTitle('Point of Interest Selection')

        self.mainTable = QTableWidget()

        layout.addWidget(self.mainTable)
        pbSave = QPushButton('Set as POI in Project')
        pbCalc = QPushButton('Recalc POI Values')
        pbCalc.clicked.connect(self.calcPOI)
        pbSaveNPY = QPushButton('Save to NPY File')
        pbLoadNPY = QPushButton('Load NPY File')

        pbLayout = QHBoxLayout()
        pbLayout.addWidget(pbSave)
        pbLayout.addWidget(pbCalc)
        pbLayout.addWidget(pbSaveNPY)
        pbLayout.addWidget(pbLoadNPY)

        layout.addLayout(pbLayout)

        self.setLayout(layout)

        self.diffs = []

    def setDifferences(self, diffs):
        self.diffs = diffs
        self.calcPOI()

    def setMinSpace(self, minspace):
        self.minSpace = minspace
        self.calcPOI()

    def setNumMax(self, nummax):
        self.numMax = nummax
        self.calcPOI()

    def calcPOI(self):
        # Setup Table for current stuff
        self.mainTable.setRowCount(len(self.diffs))
        self.mainTable.setColumnCount(2)
        self.mainTable.setHorizontalHeaderLabels(["Subkey Number", "Point List"])
        self.mainTable.verticalHeader().hide()

        self.poiArray = []

        for bnum in range(0, len(self.diffs)):

            maxarray = []

            # Copy since we will be overwriting it a bunch
            data = copy.deepcopy(self.diffs[bnum])

            while len(maxarray) < self.numMax:
                # Find maximum location
                mloc = np.argmax(data)

                # Store this maximum
                maxarray.append(mloc)

                # set to -INF data within +/- the minspace
                mstart = max(0, mloc - self.minSpace)
                mend = min(mloc + self.minSpace, len(data))
                data[mstart:mend] = -np.inf

            # print maxarray
            self.poiArray.append(maxarray)

            self.mainTable.setItem(bnum, 0, QTableWidgetItem("%d" % bnum))
            self.mainTable.setCellWidget(bnum, 1, QLineEdit(str(maxarray)))

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

        self.poi = POI(self)
        self.poidock = self.parent.addDock(self.poi, "Points of Interest", area=Qt.RightDockWidgetArea)
        self.poidock.hide()
                    
        self.params = [
              {'name':'Comparison Mode', 'key':'diffmode', 'type':'list', 'values':diffModeList, 'value':self.diffObject.diffMethodClass, 'set':self.diffObject.setDiffMethod},
              {'name':'Partition Mode', 'key':'partmode', 'type':'list', 'values':partModeList, 'value':self.partObject.partMethodClass, 'set':self.partObject.setPartMethod},
              {'name':'Combination Mode', 'key':'combomode', 'type':'list', 'values':combModeList, 'value':self.combObject.combMethodClass, 'set':self.combObject.setCombMethod},
              {'name':'Display', 'type':'action', 'action':self.runAction},

              {'name':'Points of Interest', 'key':'poi', 'type':'group', 'children':[
                 {'name':'Selection Mode', 'type':'list', 'values':{'Max N Points/Subkey':'maxn'}, 'value':'maxn'},
                 {'name':'Num POI/Subkey', 'key':'pointskey', 'type':'int', 'limits':(1, 200), 'value':1, 'set':self.poi.setNumMax},
                 {'name':'Min Spacing between POI', 'key':'minspacing', 'type':'int', 'limits':(1, 100E6), 'value':1, 'step':100, 'set':self.poi.setMinSpace},
                 {'name':'Threshold', 'key':'threshold', 'type':'int', 'visible':False},
                 {'name':'Open POI Table', 'type':'action', 'action':self.poidock.show},
              ]},
             ]

        self.poi.setNumMax(1)
        self.poi.setMinSpace(1)

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

        # Get progress indicator
        pbar = self.parent.getProgressIndicator()

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
        segList = traces.getTraceList()

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

        self.poi.setDifferences(self.SADList)

                

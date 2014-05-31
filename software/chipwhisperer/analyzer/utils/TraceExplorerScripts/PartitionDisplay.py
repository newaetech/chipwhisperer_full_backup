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
                    
        self.params = [
              {'name':'Comparison Mode', 'key':'compmode', 'type':'list', 'values':{'Sum of Absolute Differences':'SAD'}, 'value':'SAD'},
              {'name':'Partition Mode', 'key':'partmode', 'type':'list', 'values':partModeList, 'value':self.partObject.partMethodClass, 'set':self.partObject.setPartMethod},
              {'name':'Combination Mode', 'key':'combomode', 'type':'list', 'values':{'Average':'avg'}, 'value':'avg'},
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

        # Load partition data
        self.partObject.setTraceSource(traces)
        partTable = self.partObject.loadPartitions(0, 10000)
        
        data = []
        numPoints = traces.findMappedTrace(0).numPoints()

        # Generate averages for each partition
        for bnum in range(0, self.numKeys):
            data.append([])
            for i in range(0, self.partObject.partMethod.getNumPartitions()):
                data[bnum].append(np.zeros(numPoints))
                tlist = partTable[bnum][i]
                for tnum in tlist:
                    data[bnum][i] = np.add(data[bnum][i], traces.getTrace(tnum))
                data[bnum][i] = data[bnum][i] / len(tlist)

        # Calculate SAD over all combinations (long!)
        self.SADList = []
        for bnum in range(0, self.numKeys):
            sad = np.zeros(numPoints)
            for i in range(0, self.partObject.partMethod.getNumPartitions()):
                for j in range(0, self.partObject.partMethod.getNumPartitions()):
                    sad = np.add(sad, np.abs(np.subtract(data[bnum][i], data[bnum][j])))

            self.SADList.append(sad)
                    
            # graph.passTrace(sad)

                    
                
                

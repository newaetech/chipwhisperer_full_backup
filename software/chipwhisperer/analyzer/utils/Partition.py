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

import sys
import copy

try:
    from PySide.QtCore import *
    from PySide.QtGui import *
except ImportError:
    print "ERROR: PySide is required for this program"
    sys.exit()

import numpy as np

from openadc.ExtendedParameter import ExtendedParameter
from pyqtgraph.parametertree import Parameter

class PartitionDialog(QDialog):
    """Open dialog to run partioning"""
    
    def __init__(self, parent, partInst):
        super(PartitionDialog, self).__init__(parent)
        
        self.part = partInst

        self.setWindowTitle("Partition Traces")
        self.setObjectName("Partition Traces")
        
        layoutPart = QHBoxLayout()
        
        pbStart = QPushButton("Generate Partitions")
        pbStart.clicked.connect(self.runGenerate)
        layoutPart.addWidget(pbStart)
        self.setLayout(layoutPart)
        
    def runGenerate(self):
        pb = QProgressBar(self)

        # TODO: Partition generation doesn't work
        pb.setMinimum(0)
        pb.setMinimum(self.part.trace.numTrace())

        self.part.runPartitions(report=pb.setValue)

class PartitionEncKey(object):

    sectionName = "Partition Based on Key Value"
    moduleName = "PartitionKeyValue"
    partitionType = "Key Value"

    def getNumPartitions(self):
        return 256

    def getPartitionNum(self, trace, tnum):
        key = trace.getKnownKey(tnum)
        return key
        

class Partition(QObject):
    """
    Base Class for all partioning modules
    """
    paramListUpdated = Signal(list)

    descrString = "Partition traces based on some method"

    attrDictPartition = {
                "sectionName":"Partition Based on XXXX",
                "moduleName":"Partitions",
                "module":None,
                "values":{
                    "round":{"value":0, "desc":"Round", "changed":False, "definesunique":True},
                    "filename":{"value":None, "desc":"Partition File", "changed":False, "headerLabel":"Partition Data"},
                    },
                }

    supportedMethods = [PartitionEncKey]

    def __init__(self, parent, console=None, showScriptParameter=None):
        """Pass None/None if you don't have/want console/showScriptParameter"""
        super(Partition, self).__init__()
        self.console = console
        self.showScriptParameter = showScriptParameter
        self.parent = parent
        self._tmanager = None
        if parent is not None:
            self.setTraceManager(parent.traceManager())
        self.setupParameters()

    def setupParameters(self):
        """Setup parameters specific to preprocessing module"""
        # ssParams = [{'name':'Enabled', 'type':'bool', 'value':True, 'set':self.setEnabled},
        #            # PUT YOUR PARAMETERS HERE
        #            {'name':'Desc', 'type':'text', 'value':self.descrString}]
        # self.params = Parameter.create(name='Name of Module', type='group', children=ssParams)
        # ExtendedParameter.setupExtended(self.params, self)
        
        self.setPartMethod(PartitionEncKey)

    def setPartMethod(self, method):
        self.partMethodClass = method
        self.partMethod = method()
        self.attrDictPartition["sectionName"] = self.partMethod.sectionName
        self.attrDictPartition["moduleName"] = self.partMethod.moduleName

    def paramList(self):
        """Returns the parameter list"""
        return [self.params]

    def init(self):
        """Do any initilization required once all traces are loaded"""
        pass

    def setTraceManager(self, tmanager):
        """Set the input trace source"""
        self._tmanager = tmanager

    def traceManager(self):
        if self._tmanager is None and self.parent is not None:
            self._tmanager = self.parent.traceManager()
        return self._tmanager
        
    def createBlankTable(self, t):
        # Create storage for partition information
        partitionTable = []
        for j in range(0, len(t.getKnownKey())):
            partitionTable.append([])
            for i in range(0, self.partMethod.getNumPartitions()):
                partitionTable[j].append([])

        return partitionTable

    def loadPartitions(self, start=0, end=-1):
        """Load partitions from trace files, convert to mapped range"""
        if end == -1:
            end = self.traceManager().numTrace()

        partitionTable = None

        tnum = start
        while tnum < end:
            t = self.traceManager().findMappedTrace(tnum)
            # Discover where this trace starts & ends
            tmapstart = t.mappedRange[0]
            tmapend = t.mappedRange[1]
            tmapend = min(tmapend, end)

            partdata = t.loadAuxiliaryData(self.partMethod.moduleName)

            if partitionTable is None:
                partitionTable = self.createBlankTable(t)

            # Merge tables now - better way to do this?
            for j in range(0, len(t.getKnownKey())):
                for i in range(0, self.partMethod.getNumPartitions()):
                    # for pd in partdata[j][i]:
                    #    partitionTable[j][i].append(pd + tmapstart)
                    partitionTable[j][i] = partitionTable[j][i] + partdata[j][i]

            print tmapstart

            # Next trace round
            tnum = tmapend + 1
        
        return partitionTable

    def runPartitions(self, start=0, end=-1, report=None):
        """Run partioning & save results to .npz file"""
        
        if end == -1:
            end = self.traceManager().numTrace()
        
        tnum = start
        while tnum < end:
            t = self.traceManager().findMappedTrace(tnum)
            #Discover where this trace starts & ends
            tmapstart = t.mappedRange[0]
            tmapend = t.mappedRange[1]
                        
            partitionTable = self.createBlankTable(t)
                
            for tnum in range(tmapstart, tmapend+1):
                #Check each trace, write partition number
                partNum = self.partMethod.getPartitionNum(t, tnum - tmapstart)
                for i, pn in enumerate(partNum):
                    partitionTable[i][pn].append(tnum - tmapstart)

                if report:
                    report(tnum)
            
            # Save partition table, reference it in config file
            newCfgDict = copy.deepcopy(self.attrDictPartition)
            updatedDict = t.addAuxDataConfig(newCfgDict)
            t.saveAuxData(partitionTable, updatedDict)

            # Debug - Dump Table
            # for t in range(0, self.partMethod.getNumPartitions()):
            #    print "Traces in %d:" % t
            #    print "  ",
            #    print partitionTable[0][t]

            tnum = tmapend + 1




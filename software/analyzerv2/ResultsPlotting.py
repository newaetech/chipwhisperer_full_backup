#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2013, Colin O'Flynn <coflynn@newae.com>
# All rights reserved.
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

try:
    from PySide.QtCore import *
    from PySide.QtGui import *
except ImportError:
    print "ERROR: PySide is required for this program"
    sys.exit()

sys.path.append('../common')
sys.path.append('../../openadc/controlsw/python/common')
sys.path.append('../common/traces')
imagePath = '../common/images/'

import numpy as np

try:
    import pyqtgraph as pg
    import pyqtgraph.multiprocess as mp
    import pyqtgraph.parametertree.parameterTypes as pTypes
    from pyqtgraph.parametertree import Parameter, ParameterTree, ParameterItem, registerParameterType
    #print pg.systemInfo()    
except ImportError:
    print "ERROR: PyQtGraph is required for this program"
    sys.exit()

from ExtendedParameter import ExtendedParameter
   
class ResultsPlotting(QObject):
    paramListUpdated = Signal(list)
    
    """Interface to main program, various routines for plotting output data"""
    def __init__(self):
        super(ResultsPlotting, self).__init__()

        #TODO: Could probably have moved parameters into actual classes?
        resultsParams = [{'name':'Ranked Table', 'type':'group', 'children':[
                         {'name':'Show', 'type':'bool', 'value':False, 'set':self.tableShow},
                         {'name':'Use Absolute Value for Rank', 'type':'bool', 'value':True, 'set':self.tableSetAbs},
                         {'name':'Use single point for Rank', 'type':'bool', 'value':False, 'set':self.tableSetSingle}
                        ]},
                      ]

        self.params = Parameter.create(name='Results', type='group', children=resultsParams)
        ExtendedParameter.setupExtended(self.params)
        
        self.table = ResultsTable()
        
    def paramList(self):
        p = []
        p.append(self.params)     
        return p            

    def tableShow(self, enabled):
        pass
    
    def tableSetAbs(self, enabled):
        self.table.setAbsoluteMode(enabled)

    def tableSetSingle(self, enabled):
        self.table.setSingleMode(enabled)
        
    def dockList(self):
        return [self.table.ResultsTable]
    
    def setAttack(self, attack):
        self.attack = attack
        self.table.setAttack(attack)        
        self.attack.attackDone.connect(self.attackDone)
        self.attack.statsUpdated.connect(self.attackStatsUpdated)
        
    def attackDone(self):
        self.table.setBytesEnabled(self.attack.bytesEnabled())
        self.table.updateTable()
    
    def attackStatsUpdated(self):
        pass
        
class ResultsPlotData(QObject):
    """
    Generic data plotting stuff. Adds ability to highlight certain guesses, used in plotting for example the
    correlation over all data points, or the most likely correlation over number of traces
    """
     
    def __init__(self):
        super(ResultsPlotData, self).__init__()

   
class ResultsTable(QObject):
    """Table of results, showing all guesses based on sorting output of attack"""
    def __init__(self, subkeys=16, permPerSubkey=256, useAbs=True):
        super(ResultsTable, self).__init__()

        self.table = QTableWidget(permPerSubkey, subkeys)
        self.ResultsTable = QDockWidget("Results Table")
        self.ResultsTable.setObjectName("Results Table")
        self.ResultsTable.setAllowedAreas(Qt.BottomDockWidgetArea | Qt.RightDockWidgetArea| Qt.LeftDockWidgetArea)
        self.ResultsTable.setWidget(self.table)       
        self.ResultsTable.setVisible(False)
        
        self.numKeys = subkeys
        self.numPerms = permPerSubkey
        self.setBytesEnabled([])
        self.useAbs = useAbs
        self.knownkey = None
        self.useSingle = False
        
    def setBytesEnabled(self, enabledbytes):
        self.enabledBytes = enabledbytes
        
    def setAttack(self, attack):
        self.attack = attack
        
    def setKnownkey(self, knownkey):
        self.knownkey = knownkey
        
    def setAbsoluteMode(self, enabled):
        self.useAbs = enabled    
        
    def setSingleMode(self, enabled):
        self.useSingle = enabled    

    def updateTable(self):
        self.pge = [self.numPerms-1]*self.numKeys
        data = self.attack.getStatistics()
        
        for bnum in range(0, self.numKeys):
            if bnum in self.enabledBytes:
                self.table.setColumnHidden(bnum, False)
                diffs = data[bnum]                

                maxes = np.zeros(256,dtype=[('hyp','i2'),('point','i4'),('value','f8')] )

                for hyp in range(0, 256):
                    if self.useAbs:
                        v = np.amax(np.fabs(diffs[hyp]))
                    else:
                        v = np.amax(diffs[hyp])                    
                    
                    #Get maximum value for this hypothesis
                    mvalue = v
                    mindex = np.amin(np.where(v == mvalue))
                    maxes[hyp] = (hyp,mindex,mvalue)

                maxes.sort(order='value')
                maxes = maxes[::-1]

                if self.useSingle:
                    #All table values are taken from same point MAX is taken from
                    where = maxes[0]['point']
                    for j in range(0,256):
                        maxes[j]['point'] = where
                        maxes[j]['value'] = diffs[maxes[j]['hyp']][where]
            
                for j in range(0,self.numPerms):
                    self.table.setItem(j,bnum,QTableWidgetItem("%02X\n%.4f"%(maxes[j]['hyp'],maxes[j]['value'])))

                    if self.knownkey is not None:
                        if maxes[j]['hyp'] == self.knownkey[bnum]:
                            self.pge[bnum] = j
            else:
                self.table.setColumnHidden(bnum, True)

        self.table.resizeRowsToContents()
        self.table.resizeColumnsToContents()
        self.ResultsTable.setVisible(True)

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
    
from subprocess import Popen, PIPE
sys.path.append('../common')
sys.path.append('../../openadc/controlsw/python/common')
sys.path.append('../common/traces')
imagePath = '../common/images/'

import numpy as np
from ExtendedParameter import ExtendedParameter

try:
    import pyqtgraph as pg
    import pyqtgraph.multiprocess as mp
    import pyqtgraph.parametertree.parameterTypes as pTypes
    from pyqtgraph.parametertree import Parameter, ParameterTree, ParameterItem, registerParameterType
    #print pg.systemInfo()    
except ImportError:
    print "ERROR: PyQtGraph is required for this program"
    sys.exit()

def enforceLimits(value, limits):
    if value < limits[0]:
        value = limits[0]
    elif value > limits[1]:
        value = limits[1]
    return value


from functools import partial
#TODO: This should be broken into a separate function I think
class AttackBaseClass(QObject):
    """Generic Attack Interface"""
        
    paramListUpdated = Signal(list)
    
    #statsUpdated called new data is available
    statsUpdated = Signal()
    
    #attack done called once entire attack is complete, stats are available. Note that the
    #statsUpdated() signal is not called even though new data is available, which avoids
    #double-processing data
    attackDone = Signal()
    
    def __init__(self, parent=None, log=None):
        super(AttackBaseClass, self).__init__(parent)

        self.parent = parent

        #TODO: Where to get this from?
        self.numsubkeys = 16

        self.allPointsSame = True
        self.startPoint = [0]*self.numsubkeys
        self.endPoint = [0]*self.numsubkeys

        self.log=log        
        self.setupParameters()
        self.setupPointsParam()

    def setupParameters(self):
        attackParams = [{'name':'Hardware Model', 'type':'group', 'children':[
                        {'name':'Crypto Algorithm', 'type':'list', 'values':{'AES-128 (8-bit)':None}, 'value':'AES-128'},
                        {'name':'Key Round', 'type':'list', 'values':['first', 'last'], 'value':'first'}
                        ]},
                       {'name':'Take Absolute', 'type':'bool', 'value':True},
                       
                       #TODO: Should be called from the AES module to figure out # of bytes
                       {'name':'Attacked Bytes', 'type':'group', 'children':
                         self.getByteList()                                                 
                        },                 
                      ]

        self.params = Parameter.create(name='Attack Settings', type='group', children=attackParams)
        ExtendedParameter.setupExtended(self.params)
        

    def getByteList(self):
        init = [dict(name='Byte %d'%bnum, type='bool', value=True, bytenum=bnum) for bnum in range(0,self.numsubkeys)]
        init.insert(0,{'name':'All On', 'type':'action', 'action':self.allBytesOn})
        init.insert(0,{'name':'All Off', 'type':'action', 'action':self.allBytesOff})
        return init
    
    def getTraceRange(self):
        return (self.startTrace, self.endTrace)
    
    def getPointRange(self, bnum):        
        (startparam, endparam) = self.findPointParam(self.pointsParams, bnum)
        
        if (startparam is None) & (bnum is not None):
            #We don't have per-byte difference actually, just get regular
            (startparam, endparam) = self.findPointParam(self.pointsParams)
        
        val = (startparam.value(), endparam.value())
        print val
        return val
    
    def copyPointsFromOutput(self, bnum=None):
        if self.parent is not None:
            xran = self.parent.results.graphoutput.xRange()        
            self.setPointRange(xran[0],xran[1], bnum)
    
    def copyPointsFromTrace(self, bnum=None):
        if self.parent is not None:
            xran = self.parent.waveformDock.widget().xRange()        
            self.setPointRange(xran[0],xran[1], bnum)
    
    def setTraceLimits(self, traces, points):
        self.setPointRange(0, points, setlimits=True)
        self.startTrace = 0
        self.endTrace = traces
    
    def setPointRange(self, start, end, bnum=None, setlimits=False):
        start = int(start)
        end = int(end)
        
        (startparam, endparam) = self.findPointParam(self.pointsParams, bnum)
        if startparam is not None:
            if setlimits:
                startparam.setLimits((start, end))
                startparam.setDefault(start)
                self.startPointLimits = (start, end)
                
            start = enforceLimits(start, self.startPointLimits)                 
            startparam.setValue(start)
            
        if endparam is not None:
            if setlimits:
                endparam.setLimits((start, end))
                endparam.setDefault(end)
                self.endPointLimits = (start, end)
            end = enforceLimits(end, self.endPointLimits)
            endparam.setValue(end)
            
        if bnum is None:
            self.startPoint[:] = [start] * len(self.startPoint)
            self.endPoint[:] = [end] * len(self.endPoint)
        else:
            self.startPoint[bnum] = start
            self.endPoint[bnum] = end
        
            
        self.paramListUpdated.emit(None)
    
    def findPointParam(self, paramtree, bnum=None):
        """Find parameters dealing with input trace plotting"""
        pointstart = None
        pointend = None
        
        for t in paramtree.children():
            if bnum is None:
                if t.name() == 'Starting Point':
                    pointstart = t
                    
                if t.name() == 'Ending Point':
                    pointend = t
            else:
                if t.name() == 'Byte %d'%bnum:
                    for q in t.children():                                    
                        if q.name() == 'Starting Point':
                            pointstart = q
                            
                        if q.name() == 'Ending Point':
                            pointend = q
                                
        return (pointstart, pointend)
    
    def setAllPointsSame(self, val):
        self.allPointsSame = val
        self.setupPointsParam()
        self.paramListUpdated.emit(None)        
    
    def setupPointsParam(self):
        self.pointsParams = Parameter.create(name='Point Setup', type='group', children=self.getPointList())
        ExtendedParameter.setupExtended(self.pointsParams)       
    
    def getPointList(self):        
        if self.allPointsSame == False:
            init = [{'name':'Byte %d'%bnum, 'type':'group', 'children': [
                        {'name':'Starting Point', 'type':'int', 'value':self.startPoint[bnum], 'limits':(self.startPoint[bnum],self.endPoint[bnum])},
                        {'name':'Ending Point', 'type':'int', 'value':self.endPoint[bnum], 'limits':(self.startPoint[bnum],self.endPoint[bnum])},
                        {'name':'Copy from Output Graph', 'type':'action', 'action':partial(self.copyPointsFromOutput, bnum)},
                        {'name':'Copy from Trace Graph', 'type':'action', 'action':partial(self.copyPointsFromTrace, bnum)},         
                        ]} for bnum in range(0, 16)]
        else:
            init = [{'name':'Starting Point', 'type':'int', 'value':self.startPoint[0], 'limits':(self.startPoint[0],self.endPoint[0])},
                    {'name':'Ending Point', 'type':'int', 'value':self.endPoint[0], 'limits':(self.startPoint[0],self.endPoint[0])},
                    {'name':'Copy from Output Graph', 'type':'action', 'action':self.copyPointsFromOutput},
                    {'name':'Copy from Trace Graph', 'type':'action', 'action':self.copyPointsFromTrace},         
                    ]
            
        #NOT ACTUALLY SUPPORTED
        init.insert(0,{'name':'Points Same across Subkeys', 'type':'bool', 'value':self.allPointsSame, 'set':self.setAllPointsSame, 'readonly':True})            
        return init


    def bytesEnabled(self):
        blist = []
        for t in self.bytesParameters():
            if t.value() == True:
                blist.append(t.opts['bytenum'])
        return blist

    def allBytesOn(self):
        for t in self.bytesParameters():
            t.setValue(True)
    
    def allBytesOff(self):
        for t in self.bytesParameters():
            t.setValue(False)

    def bytesParameters(self):
        blist = []
        for p in self.params.children():
            if p.name() == 'Attacked Bytes':
                for t in p.children():
                    if t.name().startswith('Byte'):
                        blist.append(t)

        return blist
    
    def setTraceManager(self, tmanager):
        self.trace = tmanager
    
    def doAttack(self):
        self.attackDone.emit()

    def passTrace(self, powertrace, plaintext=None, ciphertext=None, knownkey=None):
        self.statusUpdated.emit()
    
    def getStatistics(self):
        return None
            
    def paramList(self):
        return [self.params, self.pointsParams]
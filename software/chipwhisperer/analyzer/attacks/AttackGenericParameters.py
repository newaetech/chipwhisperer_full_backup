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


from chipwhisperer.common.api.config_parameter import ConfigParameter
from chipwhisperer.common.api.autoscript import AutoScript
from chipwhisperer.common.utils import Util

def enforceLimits(value, limits):
    if value < limits[0]:
        value = limits[0]
    elif value > limits[1]:
        value = limits[1]
    return value


class AttackGenericParameters(AutoScript):
    def __init__(self):
        super(AttackGenericParameters, self).__init__()
        self._traceSource = None
        self._project = None

        self.maxSubKeys = 32
        self.useAbs = True

        #TODO: Where to get this from?
        self.numsubkeys = 16

        self.allPointsSame = True
        self.startPoint = [0]*self.numsubkeys
        self.endPoint = [0]*self.numsubkeys
        self.traceMax = 1

        self.paramListUpdated = Util.Signal()
        self.traceManagerChanged = Util.Signal()
        self.projectChanged = Util.Signal()
        self.settingsChanged = Util.Signal()
        self.traceLimitsChanged = Util.Signal()

        self.setupTraceParam()
        self.setupPointsParam()
        self.setupParameters()

    def setupParameters(self):
        attackParams = [{'name':'Hardware Model', 'type':'group', 'children':[
                        {'name':'Crypto Algorithm', 'type':'list', 'values':{'AES-128 (8-bit)':None}, 'value':'AES-128'},
                        {'name':'Key Round', 'type':'list', 'values':['first', 'last'], 'value':'first'}
                        ]},
                       {'name':'Take Absolute', 'type':'bool', 'value':True, 'set':self.setAbsoluteMode},

                       #TODO: Should be called from the AES module to figure out # of bytes
                       {'name':'Attacked Bytes', 'type':'group', 'children':
                         self.getByteList()
                        },
                      ]

        self.params = ConfigParameter.create_extended(self, name='Attack Settings', type='group', children=attackParams)
        self.updateBytesVisible()

    def setAbsoluteMode(self, mode):
        self.useAbs = mode
        self.settingsChanged.emit(mode)

    def getByteList(self):
        init = [dict(name='Byte %d' % bnum, type='bool', key='bnumenabled%d' % bnum, value=True, bytenum=bnum, set=self.updateScriptBytesEnabled) for bnum in range(0, self.maxSubKeys)]
        init.insert(0,{'name':'All On', 'type':'action', 'action':self.allBytesOn})
        init.insert(0,{'name':'All Off', 'type':'action', 'action':self.allBytesOff})
        return init

    def updateScriptBytesEnabled(self, ignored=None):
        self.addFunction("init", "setTargetBytes", str(self.bytesEnabled()))

    def updateBytesVisible(self):
        for i,t in enumerate(self.bytesParameters()):
            if i < self.numsubkeys:
                t.show()
            else:
                t.hide()
        self.updateScriptBytesEnabled()

    def bytesEnabled(self):
        blist = []
        for i,t in enumerate(self.bytesParameters()):
            if i < self.numsubkeys:
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
        for i in range(0, 64):
            p = self.findParam('bnumenabled%d' % i)
            if p:
                blist.append(p)
        return blist

    def setTraceSource(self, traceSource):
        """Set the input trace source"""
        self._traceSource = traceSource
        self.traceManagerChanged.emit(traceSource)

    def traceSource(self):
        return self._traceSource

    def setProject(self, proj):
        self._project = proj
        self.projectChanged.emit(proj)

    def project(self):
        return self._project

############ Trace-Specific
    def setupTraceParam(self):
        self.traceParams = ConfigParameter.create_extended(self, name='Trace Setup', type='group', children=[
            {'name':'Starting Trace', 'key':'strace', 'type':'int', 'set':self.updateGenericScript},
            {'name':'Traces per Attack', 'key':'atraces', 'type':'int', 'limits':(1, 1E6), 'value':1, 'set':self.updateGenericScript},
            {'name':'Attack Runs', 'key':'runs', 'type':'int', 'limits':(1, 1E6), 'value':1, 'set':self.updateGenericScript},
            {'name':'Reporting Interval', 'key':'reportinterval', 'type':'int', 'value':10, 'set':self.updateGenericScript},
            ])

        self.addFunction("init", "setTraceStart", "0")
        self.addFunction("init", "setTracesPerAttack", "1")
        self.addFunction("init", "setIterations", "1")
        self.addFunction("init", "setReportingInterval", "10")

        self.singleEmit = True

    def updateGenericScript(self, ignored=None):
        runs = self.findParam('runs', self.traceParams)
        atraces = self.findParam('atraces', self.traceParams)
        strace = self.findParam('strace', self.traceParams)
        ri = self.findParam('reportinterval', self.traceParams)

        #print "runs = %d\natraces= %d\nstrace = %d\n"%(runs.value(), atraces.value(), strace.value())

        if (runs.value() * atraces.value() + strace.value()) > (self.traceMax):
            solv = (self.traceMax - strace.value()) / runs.value()
            solv = int(solv)
            atraces.setValue(solv)
            atraces.setLimits((1, solv))
            self.singleEmit = True
        else:
            lim = (1, self.traceMax)
            #WORK-AROUND: need to emit an extra sigLimitsChanged???
            if atraces.setLimits(lim) is None and self.singleEmit:
                self.singleEmit = False
                atraces.sigLimitsChanged.emit(atraces, lim)

        pointrng = (self.findParam('startpoint', self.pointsParams).value(), self.findParam('endpoint', self.pointsParams).value())

        self.addFunction("init", "setTraceStart", "%d" % strace.value())
        self.addFunction("init", "setTracesPerAttack", "%d" % atraces.value())
        self.addFunction("init", "setIterations", "%d" % runs.value())
        self.addFunction("init", "setReportingInterval", "%d" % ri.value())
        self.addFunction("init", "setPointRange", "(%d,%d)" % (pointrng[0], pointrng[1]))

############# Points-Specific
    def setupPointsParam(self):
        self.pointsParams = ConfigParameter.create_extended(self, name='Point Setup', type='group', children=self.getPointList())

    def getPointList(self):
    #   init = [{'name':'Point Range', 'key':'pointrng', 'type':'rangegraph', 'value':(0,0), 'limits':(self.startPoint[0], self.endPoint[0]), 'default':(0, 0), 'set':self.updateGenericScript, 'graphwidget':self.waveformDock.widget()},
        init = [{'name':'Starting Point', 'key':'startpoint', 'type':'int', 'value':self.startPoint[0], 'limits':(self.startPoint[0], self.endPoint[0]), 'set':self.updateGenericScript},
                    {'name':'Ending Point', 'key':'endpoint', 'type':'int', 'value':self.endPoint[0], 'limits':(self.startPoint[0], self.endPoint[0]), 'set':self.updateGenericScript},
                    ]
    #
    #    #NOT ACTUALLY SUPPORTED
    #    init.insert(0,{'name':'Points Same across Subkeys', 'type':'bool', 'value':self.allPointsSame, 'set':self.setAllPointsSame, 'readonly':True})
        return init

    # def updatePointRange(self, bnum):
    #    (startparam, endparam) = self.findPointParam(self.pointsParams, bnum)
    #
    #    if (startparam is None) & (bnum is not None):
    #        #We don't have per-byte difference actually, just get regular
    #        (startparam, endparam) = self.findPointParam(self.pointsParams)
    #
    #    val = (startparam.value(), endparam.value())
    #    return val

    # def copyPointsFromOutput(self, bnum=None):
    #    if self.MainWindow is not None:
    #        xran = self.MainWindow.results.graphoutput.xRange()
    #        self.setPointRange(xran[0],xran[1], bnum)

    # def copyPointsFromTrace(self, bnum=None):
    #    if self.MainWindow is not None:
    #        xran = self.MainWindow.waveformDock.widget().xRange()
    #        self.setPointRange(xran[0],xran[1], bnum)

    def setTraceLimits(self, traces, points):
        self.setGenericPointRange(0, points, setlimits=True)
        self.traceMax = traces

        self.addFunction("init", "setPointRange", "(%d,%d)" % (0, points))

        # self.addFunction("init", "setTraceStart", "%d" % 0)
        # self.addFunction("init", "setTracesPerAttack", "%d" % traces)
        # self.addFunction("init", "setIterations", "%d" % 1)

        strace = self.findParam('strace', self.traceParams)
        self.findParam('runs', self.traceParams).setValue(1)
        atrace = self.findParam('atraces', self.traceParams)

        strace.setValue(0)
        strace.setLimits((0,traces))
        atrace.setValue(traces)
        atrace.setLimits((1, traces))

        self.traceLimitsChanged.emit(traces, points)

    def setGenericPointRange(self, start, end, bnum=None, setlimits=False):
        start = int(start)
        end = int(end)

        startparam = self.findParam('startpoint', self.pointsParams)
        endparam = self.findParam('endpoint', self.pointsParams)

        if startparam:
            if setlimits:
                startparam.setLimits((start, end))
                startparam.setDefault(start)
                self.startPointLimits = (start, end)

            start = enforceLimits(start, self.startPointLimits)
            startparam.setValue(start)

        if endparam:
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

    # def setAllPointsSame(self, val):
    #    self.allPointsSame = val
    #    self.setupPointsParam()
    #    self.paramListUpdated.emit(None)

    def paramList(self):
        return [self.params, self.pointsParams, self.traceParams]

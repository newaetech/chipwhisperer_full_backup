#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2013-2014, NewAE Technology Inc
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

import ConfigParser
import os.path
import re

from chipwhisperer.common.traces.TraceContainerNative import TraceContainerNative
from chipwhisperer.common.utils import util
from chipwhisperer.common.utils.tracesource import TraceSource


class TraceManager(TraceSource):
    """
    When using traces in ChipWhisperer, you may have remapped a bunch of trace files into one
    block of traces. This class is used to handle the remapping and provide methods to save,
    load and manage the traces.
    """

    def __init__(self, name = "Trace Management"):
        TraceSource.__init__(self, name)
        self.name = name
        self.dirty = util.Observable(False)
        self._numTraces = 0
        self._numPoints = 0
        self.lastMapped = None
        self.traceSets = []

    def newProject(self):
        """Creates a new empty set of traces"""
        self.traceSets = []
        self.dirty.setValue(False)
        self.sigTracesChanged.emit()

    def saveProject(self, config, configfilename):
        config[self.name].clear()
        for indx, t in enumerate(self.traceSets):
            config[self.name]['tracefile%d' % indx] = os.path.normpath(os.path.relpath(t.config.configFilename(), os.path.split(configfilename)[0]))
            config[self.name]['enabled%d' % indx] = str(t.enabled)
        self.dirty.setValue(False)

    def loadProject(self, configfilename):
        config = ConfigParser.RawConfigParser()
        config.read(configfilename)
        alltraces = config.items(self.name)
        self.newProject()

        fdir = os.path.split(configfilename)[0] + "/"

        for t in alltraces:
            if t[0].startswith("tracefile"):
                fname = fdir + t[1]
                fname = os.path.normpath(fname.replace("\\", "/"))
                # print "Opening %s"%fname
                ti = TraceContainerNative()
                try:
                    ti.config.loadTrace(fname)
                except Exception, e:
                    print e.message
                self.traceSets.append(ti)
            if t[0].startswith("enabled"):
                tnum = re.findall(r'[0-9]+', t[0])
                self.traceSets[int(tnum[0])].enabled = t[1] == "True"
        self.setModified()
        self.dirty.setValue(False)

    def removeTraceSet(self, pos):
        self.traceSets.pop(pos)
        self.setModified()

    def setTraceSetStatus(self, pos, newStatus):
        self.traceSets[pos].enabled = newStatus
        self.setModified()

    def getSegmentList(self, start=0, end=-1):
        """
        Get a list of segments.
        """
        tnum = start
        if end == -1:
            end = self._numTraces

        dataDict = {'offsetList':[], 'lengthList':[]}

        while(tnum < end):
            t = self.findMappedTrace(tnum)
            dataDict['offsetList'].append(t.mappedRange[0])
            dataDict['lengthList'].append(t.mappedRange[1] - t.mappedRange[0] + 1)
            tnum = t.mappedRange[1] + 1

        return dataDict

    def findMappedTrace(self, n):
        if self.lastMapped is not None:
            if self.lastMapped.mappedRange is not None and n >= self.lastMapped.mappedRange[0] and n <= self.lastMapped.mappedRange[1]:
                return self.lastMapped
            else:
                # Only load one segment at a time for memory reasons
                self.lastMapped.unloadAllTraces()
                self.lastMapped = None

        for t in self.traceSets:
            if t.mappedRange:
                if n >= t.mappedRange[0] and n <= t.mappedRange[1]:
                    if not t.isLoaded():
                        t.loadAllTraces(None, None)
                    self.lastMapped = t
                    return t

        raise ValueError("Error: Trace %d is not in mapped range." % n)

    def getAuxData(self, n, auxDic):
        t = self.findMappedTrace(n)
        cfg = t.getAuxDataConfig(auxDic)
        if cfg is not None:
            filedata = t.loadAuxData(cfg["filename"])
        else:
            filedata = None

        return {'cfgdata':cfg, 'filedata':filedata}

    def getTrace(self, n):
        t = self.findMappedTrace(n)
        return t.getTrace(n - t.mappedRange[0])

    def getTextin(self, n):
        t = self.findMappedTrace(n)
        return t.getTextin(n - t.mappedRange[0])

    def getTextout(self, n):
        t = self.findMappedTrace(n)
        return t.getTextout(n - t.mappedRange[0])

    def getKnownKey(self, n):
        try:
            t = self.findMappedTrace(n)
            return t.getKnownKey(n - t.mappedRange[0])
        except ValueError:
            return None

    def updateRanges(self):
        startTrace = 0
        self._numPoints = 0
        for t in self.traceSets:
            if t.enabled:
                tlen = t.numTraces()
                t.mappedRange = [startTrace, startTrace+tlen-1]
                startTrace = startTrace + tlen
                self._numPoints = max(self._numPoints, int(t.config.attr("numPoints")))
            else:
                t.mappedRange = None
        self._numTraces = startTrace

    def numPoints(self):
        return self._numPoints

    def numTraces(self):
        return self._numTraces

    def appendTraceSet(self, ti):
        ti.enabled = True
        self.traceSets.append(ti)
        self.setModified()

    def setModified(self):
        self.dirty.setValue(True)
        self.updateRanges()
        self.sigTracesChanged.emit()
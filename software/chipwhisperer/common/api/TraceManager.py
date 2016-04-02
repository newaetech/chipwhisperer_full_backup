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

import os.path
import re
from chipwhisperer.common.utils import Util
import chipwhisperer.common.traces.TraceContainerConfig
import chipwhisperer.common.traces.TraceContainerNative
import ConfigParser


class TraceManager(object):
    """
    When using traces in ChipWhisperer, you may have remapped a bunch of trace files into one
    block of traces. This class is used to handle the remapping and provide methods to save,
    load and manage the traces.
    """

    secName = "Trace Management"

    def __init__(self):
        self.dirty = Util.Observable(False)
        self.tracesChanged = Util.Signal()
        self._numTraces = 0
        self._numPoints = 0
        self.lastMapped = None
        self.traceSets = []

    def newProject(self):
        """Creates a new empty set of traces"""
        self.traceSets = []
        self.dirty.setValue(False)
        self.tracesChanged.emit()

    def saveProject(self, config, configfilename):
        config[self.secName].clear()
        for indx, t in enumerate(self.traceSets):
            config[self.secName]['tracefile%d' % indx] = os.path.normpath(os.path.relpath(t.config.configFilename(), os.path.split(configfilename)[0]))
            config[self.secName]['enabled%d' % indx] = str(t.enabled)
        self.dirty.setValue(False)

    def loadProject(self, configfilename):
        config = ConfigParser.RawConfigParser()
        config.read(configfilename)
        alltraces = config.items(self.secName)
        self.newProject()

        fdir = os.path.split(configfilename)[0] + "/"

        for t in alltraces:
            if t[0].startswith("tracefile"):
                fname = fdir + t[1]
                fname = os.path.normpath(fname.replace("\\", "/"))
                # print "Opening %s"%fname
                ti = chipwhisperer.common.traces.TraceContainerNative.TraceContainerNative()
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
        t = self.findMappedTrace(n)
        return t.getKnownKey(n - t.mappedRange[0])

    def updateRanges(self):
        startTrace = 0
        for t in self.traceSets:
            if t.enabled:
                tlen = t.numTraces()
                t.mappedRange = [startTrace, startTrace+tlen-1]
                startTrace = startTrace + tlen
                if t.traces is None:
                    if t.config.configFilename() is not None:
                        path = os.path.split(t.config.configFilename())[0]
                        pref = t.config.attr("prefix")
                    else:
                        path = None
                        pref = None
                    t.directory = path
                    t.prefix = pref
            else:
                t.mappedRange = None
        self.updateTraces()

    def updateTraces(self):
        #Find total (last mapped range)
        num = [0]
        pts = [0]

        for t in self.traceSets:
            if t.mappedRange is not None:
                num.append(t.mappedRange[1])
                pts.append(int(t.config.attr("numPoints")))

        self._numTraces = max(num)
        self._numPoints = max(pts)

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
        self.tracesChanged.emit()
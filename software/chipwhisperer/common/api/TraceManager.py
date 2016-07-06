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
import logging
import os.path
import re

from chipwhisperer.common.traces.TraceContainerNative import TraceContainerNative
from chipwhisperer.common.utils import util
from chipwhisperer.common.utils.tracesource import TraceSource


class TraceManager(TraceSource):
    """
    When using traces in ChipWhisperer, you may have remapped a bunch of trace files into one
    segment of traces. This class is used to handle the remapping and provide methods to save,
    load and manage the traces.
    """

    def __init__(self, name = "Trace Management"):
        TraceSource.__init__(self, name)
        self.name = name
        self.dirty = util.Observable(False)
        self._numTraces = 0
        self._numPoints = 0
        self.lastUsedSegment = None
        self.traceSegments = []
        if __debug__: logging.debug('Created: ' + str(self))

    def newProject(self):
        """Create a new empty set of traces."""
        self.traceSegments = []
        self.dirty.setValue(False)
        self.sigTracesChanged.emit()

    def saveProject(self, config, configfilename):
        """Save the trace segments information to a project file."""
        config[self.name].clear()
        for indx, t in enumerate(self.traceSegments):
            config[self.name]['tracefile%d' % indx] = os.path.normpath(os.path.relpath(t.config.configFilename(), os.path.split(configfilename)[0]))
            config[self.name]['enabled%d' % indx] = str(t.enabled)
        self.dirty.setValue(False)

    def loadProject(self, configfilename):
        """Load the trace segments information from a project file."""
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
                self.traceSegments.append(ti)
            if t[0].startswith("enabled"):
                tnum = re.findall(r'[0-9]+', t[0])
                self.traceSegments[int(tnum[0])].enabled = t[1] == "True"
        self._setModified()
        self.dirty.setValue(False)

    def removeTraceSegments(self, positions):
        """Remove a list of trace segments. Do not repeat numbers!!"""
        if not isinstance(positions, list):
            positions = [positions]
        else:
            positions.sort(reverse=True)
        for pos in positions:
            self.traceSegments.pop(pos)
        self._setModified()

    def setTraceSegmentStatus(self, pos, newStatus):
        """Set a trace segment as enabled/disabled."""
        self.traceSegments[pos].enabled = newStatus
        self._setModified()

    def getSegmentList(self, start=0, end=-1):
        """Return a list of segments."""
        tnum = start
        if end == -1:
            end = self._numTraces

        dataDict = {'offsetList':[], 'lengthList':[]}

        while(tnum < end):
            t = self.getSegment(tnum)
            dataDict['offsetList'].append(t.mappedRange[0])
            dataDict['lengthList'].append(t.mappedRange[1] - t.mappedRange[0] + 1)
            tnum = t.mappedRange[1] + 1

        return dataDict

    def getSegment(self, traceIndex):
        """Return the trace segment with the specified trace in the list with all enabled segments."""
        if self.lastUsedSegment is not None:
            if self.lastUsedSegment.mappedRange is not None and self.lastUsedSegment.mappedRange[0] <= traceIndex <= \
                    self.lastUsedSegment.mappedRange[1]:
                return self.lastUsedSegment
            else:
                # Only load one segment at a time for memory reasons
                self.lastUsedSegment.unloadAllTraces()
                self.lastUsedSegment = None

        for traceSegment in self.traceSegments:
            if traceSegment.mappedRange and traceSegment.mappedRange[0] <= traceIndex <= traceSegment.mappedRange[1]:
                if not traceSegment.isLoaded():
                    traceSegment.loadAllTraces(None, None)
                self.lastUsedSegment = traceSegment
                return traceSegment

        raise ValueError("Error: Trace %d is not in mapped range." % traceIndex)

    def getAuxData(self, n, auxDic):
        """Return data about a segment"""
        t = self.getSegment(n)
        cfg = t.getAuxDataConfig(auxDic)
        if cfg is not None:
            filedata = t.loadAuxData(cfg["filename"])
        else:
            filedata = None

        return {'cfgdata':cfg, 'filedata':filedata}

    def getTrace(self, n):
        """Return the trace with index n in the list of enabled segments"""
        t = self.getSegment(n)
        return t.getTrace(n - t.mappedRange[0])

    def getTextin(self, n):
        """Return the input text of trace with index n in the list of enabled segments"""
        t = self.getSegment(n)
        return t.getTextin(n - t.mappedRange[0])

    def getTextout(self, n):
        """Return the output text of trace with index n in the list of enabled segments"""
        t = self.getSegment(n)
        return t.getTextout(n - t.mappedRange[0])

    def getKnownKey(self, n):
        """Return the known encryption key."""
        try:
            t = self.getSegment(n)
            return t.getKnownKey(n - t.mappedRange[0])
        except ValueError:
            return []

    def _updateRanges(self):
        """Update the trace range for each segments."""
        startTrace = 0
        self._numPoints = 0
        for t in self.traceSegments:
            if t.enabled:
                tlen = t.numTraces()
                t.mappedRange = [startTrace, startTrace+tlen-1]
                startTrace = startTrace + tlen
                self._numPoints = max(self._numPoints, int(t.config.attr("numPoints")))
            else:
                t.mappedRange = None
        self._numTraces = startTrace

    def numPoints(self):
        """Return the number of points in traces of the selected segments."""
        return self._numPoints

    def numTraces(self):
        """Return the number of traces in the current list of enabled segments."""
        return self._numTraces

    def appendSegment(self, ti, enabled=True):
        """Append a new segment to the list of trace segments."""
        ti.enabled = enabled
        self.traceSegments.append(ti)
        self._setModified()

    def _setModified(self):
        """Notify passive and active observers to be updated."""
        self.dirty.setValue(True)
        self._updateRanges()
        self.sigTracesChanged.emit()

    def __del__(self):
        if __debug__: logging.debug('Deleted: ' + str(self))

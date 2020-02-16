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

import configparser
import logging
import os.path
import re

from chipwhisperer.common.traces.TraceContainerNative import TraceContainerNative
from chipwhisperer.common.utils import util
from chipwhisperer.common.utils.tracesource import TraceSource

from datetime import datetime
from pathlib import Path
import copy

class TraceManager(TraceSource):
    """
    When using traces in ChipWhisperer, you may have remapped a bunch of trace
    files into one segment of traces. This class is used to handle the
    remapping and provide methods to save, load and manage the traces.

    Don't use anything that modifies the trace manager -> stick to stuff
    that gives you information about it instead (i.e. get_trace, get_known_key)
    """

    def __init__(self, name = "Trace Management"):
        TraceSource.__init__(self, name)
        self.name = name
        self.dirty = util.Observable(False)
        self._numTraces = 0
        self._numPoints = 0
        self._sampleRate = 0
        self.lastUsedSegment = None
        self.traceSegments = []
        self.saved = False
        if __debug__: logging.debug('Created: ' + str(self))

    def new_project(self):
        """Create a new empty set of traces."""
        self.traceSegments = []
        self.dirty.setValue(False)
        self.sigTracesChanged.emit()

    newProject = new_project

    def save_project(self, config, configfilename):
        """Save the trace segments information to a project file."""
        config[self.name].clear()
        for indx, t in enumerate(self.traceSegments):
            starttime = datetime.now()
            prefix = starttime.strftime('%Y.%m.%d-%H.%M.%S') + "_" + str(indx)
            t.config.setConfigFilename(os.path.splitext(configfilename)[0] + "_data" + "/traces/config_" + prefix + ".cfg")
            t.config.setAttr("prefix", prefix)
            t.config.setAttr("date", starttime.strftime('%Y-%m-%d %H:%M:%S'))
            config[self.name]['tracefile%d' % indx] = os.path.normpath(os.path.relpath(t.config.configFilename(), os.path.split(configfilename)[0]))
            config[self.name]['enabled%d' % indx] = str(t.enabled)

            t.saveAllTraces(os.path.dirname(t.config.configFilename()), prefix)
        self.dirty.setValue(False)
        self.saved = True

    saveProject = util.camel_case_deprecated(save_project)

    def loadProject(self, configfilename):
        """Load the trace segments information from a project file."""
        config = configparser.RawConfigParser()
        config.read(configfilename)
        alltraces = config.items(self.name)
        self.newProject()

        path, name = os.path.split(configfilename)

        for t in alltraces:
            if t[0].startswith("tracefile"):
                fname = os.path.join(path, t[1])
                fname = os.path.normpath(fname.replace("\\", "/"))
                # print "Opening %s"%fname
                ti = TraceContainerNative()
                try:
                    ti.config.loadTrace(fname)
                    ti.loadAllTraces()
                except Exception as e:
                    logging.error(str(e))
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
            t = self.get_segment(tnum)
            dataDict['offsetList'].append(t.mappedRange[0])
            dataDict['lengthList'].append(t.mappedRange[1] - t.mappedRange[0] + 1)
            tnum = t.mappedRange[1] + 1

        return dataDict

    def get_segment(self, traceIndex):
        """Return the trace segment with the specified trace in the list with all enabled segments."""
        if self.lastUsedSegment is not None:
            if self.lastUsedSegment.mappedRange is not None and self.lastUsedSegment.mappedRange[0] <= traceIndex <= \
                    self.lastUsedSegment.mappedRange[1]:
                return self.lastUsedSegment
            else:
                # Only load one segment at a time for memory reasons
                # If the traces are actually saved :)
                if self.saved:
                    self.lastUsedSegment.unloadAllTraces()
                    self.lastUsedSegment = None

        for traceSegment in self.traceSegments:
            if traceSegment.mappedRange and traceSegment.mappedRange[0] <= traceIndex <= traceSegment.mappedRange[1]:
                if not traceSegment.isLoaded():
                    traceSegment.loadAllTraces(None, None)
                self.lastUsedSegment = traceSegment
                return traceSegment

        raise ValueError("Error: Trace %d is not in mapped range." % traceIndex)

    getSegment = util.camel_case_deprecated(get_segment)

    def getAuxData(self, n, auxDic):
        """Return data about a segment"""
        t = self.get_segment(n)
        cfg = t.getAuxDataConfig(auxDic)
        if cfg is not None:
            filedata = t.loadAuxData(cfg["filename"])
        else:
            filedata = None

        return {'cfgdata':cfg, 'filedata':filedata}

    def get_trace(self, n):
        """Return the trace with index n in the list of enabled segments"""
        t = self.get_segment(n)
        return t.getTrace(n - t.mappedRange[0])

    getTrace = util.camel_case_deprecated(get_trace)

    def get_textin(self, n):
        """Return the input text of trace with index n in the list of enabled segments"""
        t = self.get_segment(n)
        return t.getTextin(n - t.mappedRange[0])

    getTextin = util.camel_case_deprecated(get_textin)

    def get_textout(self, n):
        """Return the output text of trace with index n in the list of enabled segments"""
        t = self.get_segment(n)
        return t.getTextout(n - t.mappedRange[0])

    getTextout = util.camel_case_deprecated(get_textout)

    def get_known_key(self, n):
        """Return the known encryption key."""
        try:
            t = self.get_segment(n)
            return t.getKnownKey(n - t.mappedRange[0])
        except ValueError:
            return []

    getKnownKey = util.camel_case_deprecated(get_known_key)

    def _updateRanges(self):
        """Update the trace range for each segments."""
        startTrace = 0
        self._sampleRate = 0
        self._numPoints = 0
        for t in self.traceSegments:
            if t.enabled:
                tlen = t.numTraces()
                t.mappedRange = [startTrace, startTrace+tlen-1]
                startTrace = startTrace + tlen
                np = int(t.config.attr("numPoints"))
                if self._numPoints != np and np != 0:
                    if self._numPoints == 0:
                        self._numPoints = np
                    else:
                        logging.warning("Selected trace segments have different number of points: %d!=%d" % (self._numPoints, np))
                        self._numPoints = min(self._numPoints, np)

                sr = int(float(t.config.attr("scopeSampleRate")))
                if self._sampleRate != sr and sr != 0:
                    if self._sampleRate == 0:
                        self._sampleRate = sr
                    else:
                        logging.warning("Selected trace segments have different sample rates: %d!=%d" % (self._sampleRate, sr))
            else:
                t.mappedRange = None
        self._numTraces = startTrace

    def num_points(self):
        """Return the number of points in traces of the selected segments."""
        return self._numPoints

    numPoints = util.camel_case_deprecated(num_points)

    def num_traces(self):
        """Return the number of traces in the current list of enabled segments."""
        return self._numTraces

    numTraces = util.camel_case_deprecated(num_traces)

    def appendSegment(self, ti, enabled=True):
        """Append a new segment to the list of trace segments."""
        ti.enabled = enabled
        ti._isloaded = True
        self.traceSegments.append(ti)
        self._setModified()

    def _setModified(self):
        """Notify passive and active observers to be updated."""
        self.dirty.setValue(True)
        self._updateRanges()
        self.sigTracesChanged.emit()

    def getSampleRate(self, ):
        return self._sampleRate

    def changeSegmentAttribute(self, segmentNum, attribute, value):
        """Change the value of a segment attribute. Changes are saved instantly."""
        self.traceSegments[segmentNum].config.setAttr(attribute, value)
        self.traceSegments[segmentNum].config.saveTrace()
        logging.info('Trace attribute "%s" of segment %d changed to: %s' % (attribute, segmentNum, value))
        self._updateRanges()
        self.sigTracesChanged.emit()

    def __del__(self):
        if __debug__: logging.debug('Deleted: ' + str(self))

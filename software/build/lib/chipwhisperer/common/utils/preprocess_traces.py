#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2017, NewAE Technology Inc
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

from chipwhisperer.common.api.CWCoreAPI import CWCoreAPI
from chipwhisperer.common.traces.TraceContainerNative import TraceContainerNative

class CopyProject(object):

    def __init__(self, oldprojectname, newprojectname):

        cwapi_new = CWCoreAPI()
        cwapi_old = CWCoreAPI()

        cwapi_old.openProject(oldprojectname)

        cwapi_new.newProject()
        cwapi_new.saveProject(newprojectname)

        tm = cwapi_old.project().traceManager()
        ntraces = tm.numTraces()

        segs = tm.getSegmentList()

        numtraces = 0

        for offset, seg_idx in enumerate(segs['offsetList']):
            seg_len = segs['lengthList'][seg_idx]

            seg = tm.getSegment(seg_idx)
            tc = TraceContainerNative()
            tc.clear()
            # Copy all aux data over
            # seg.config._configfile
            tc.config.config = seg.config.config
            prefix = tc.config.config['Trace Config']['prefix']
            tc.config.setConfigFilename(cwapi_new.project().datadirectory + "traces/config_" + prefix + ".cfg")

            tc.setDirty(True)
            tc.config.syncFile()

            for tnum in range(offset, seg_len + offset):
                trace = tm.getTrace(tnum)
                textin = tm.getTextin(tnum)
                textout = tm.getTextout(tnum)
                key = tm.getKnownKey(tnum)

                try:
                    textin, textout, trace, key = self.trace_callback(textin, textout, trace, key)
                except StopIteration:
                    continue

                numtraces += 1
                tc.addTrace(trace, textin, textout, key)
            tc.closeAll()
            cwapi_new.project().traceManager().appendSegment(tc)

        cwapi_new.saveProject()

    def trace_callback(self, textin, textout, trace, key):

        #If you want to preprocess traces, simply reimplement this.
        #Raise StopIteration() when you want a trace skipped (dropped).

        return (textin, textout, trace, key)


if __name__ == "__main__":
    from chipwhisperer.common.utils import util

    class copyRandOnly(CopyProject):
        def trace_callback(self, textin, textout, trace, key):
            interleavedPlaintext128 = util.hexStrToByteArray("da 39 a3 ee 5e 6b 4b 0d 32 55 bf ef 95 60 18 90")
            interleavedPlaintext192 = util.hexStrToByteArray("da 39 a3 ee 5e 6b 4b 0d 32 55 bf ef 95 60 18 88")
            interleavedPlaintext256 = util.hexStrToByteArray("da 39 a3 ee 5e 6b 4b 0d 32 55 bf ef 95 60 18 95")

            if (textin == interleavedPlaintext128).all() or \
                    (textin == interleavedPlaintext192).all() or \
                    (textin == interleavedPlaintext256).all():
                raise StopIteration()

            return (textin, textout, trace, key)


    filename = r'tvla-test-set.cwp'
    filename_grouprand = filename.replace(".cwp", "_randonly.cwp")

    copyRandOnly(filename, filename_grouprand)
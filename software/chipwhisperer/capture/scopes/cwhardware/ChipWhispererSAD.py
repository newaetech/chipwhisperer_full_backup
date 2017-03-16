#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2014, NewAE Technology Inc
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
import logging

import numpy as np
from chipwhisperer.common.utils.parameter import Parameter, Parameterized, setupSetParam
from chipwhisperer.common.results.base import ResultsBase

sadcfgaddr  = 53
saddataaddr = 54
CODE_READ   = 0x80
CODE_WRITE  = 0xC0


class ChipWhispererSAD(Parameterized):
    """
    Communicates and drives with the Sum of Absolute Differences (SAD) Module inside the ChipWhisperer System. You
    need to configure the trigger module as active & set the trigger polarity to "high" for this to work.    
    """
    _name = 'SAD Trigger Module'
    STATUS_RUNNING_MASK = 1 << 3
    STATUS_RESET_MASK = 1 << 0
    STATUS_START_MASK = 1 << 1
             
    def __init__(self, oa):

        self.oldlow = None
        self.oldhigh = None
        self.oa = oa
        self.sadref = [0]

        try:
            # Update SAD calculation when data changes
            ResultsBase.registeredObjects["Trace Output Plot"].dataChanged.connect(self.dataChanged)
            outwid = ResultsBase.registeredObjects["Trace Output Plot"]
            rangewidget = {'name':'Point Range', 'key':'pointrng', 'type':'rangegraph', 'limits':(0, 0), 'value':(0, 0), 'default':(0, 0),
                                       'graphwidget':outwid, 'action':self.updateSADTraceRef, 'fixedsize':128}
        except KeyError:
            rangewidget = {'name':'Point Range', 'key':'pointrng', 'type':'range', 'limits':(0, 0), 'value':(0, 0), 'default':(0, 0),
                                       'action':self.updateSADTraceRef, 'fixedsize':128}

        self.params = Parameter(name=self.getName(), type='group')
        self.params.addChildren([
            # {'name':'Open SAD Viewer', 'type':'action'},
            {'name':'SAD Ref From Captured', 'key':'sad', 'type':'group', 'children':[
                rangewidget,
                {'name':'Set SAD Reference from Current Trace', 'key':'docopyfromcapture', 'type':'action', 'action':self.copyFromCaptureTrace},
                {'name':'SAD Reference vs. Cursor', 'key':'sadrefcur', 'type':'int', 'value':0, 'limits':(-1, 100E6), 'readonly':True},
            ]},
            {'name':'SAD Threshold', 'type':'int', 'limits':(0, 100000), 'default':0, 'set':self.setThreshold, 'get':self.getThreshold}
        ])

    def dataChanged(self, data, offset):
        """Called when data in the trace window has changed. Used to update the limits for the point selection dialog."""

        low = offset
        up = offset + len(data) - 1

        if self.oldlow != low or self.oldup != up:
            self.oldlow = low
            self.oldup = up
            self.findParam(['sad', 'pointrng']).setLimits((low, up))
            self.findParam(['sad', 'pointrng']).setValue((low, min(up, low + 128)))

        self.updateSADTraceRef()

    def getCaptueTraceRef(self):
        """ Get the reference data for SAD algorithm from the api trace window """

        try:
            waveformWidget = ResultsBase.registeredObjects["Trace Output Plot"]
        except KeyError:
            logging.warning('SAD Trigger: Trace Output Plot not running, no data source')
            return [0.0]*128
        pstart = self.findParam(['sad', 'pointrng']).getValue()[0] - waveformWidget.lastStartOffset
        pend = self.findParam(['sad', 'pointrng']).getValue()[1] - waveformWidget.lastStartOffset
        data = waveformWidget.lastTraceData[pstart:pend]
        data = np.array(data)
        data = (data + 0.5) * 1024
        return data

    def copyFromCaptureTrace(self, _=None):
        """ Send reference data to hardware from the trace window """
        ds_param = Parameter.findParameter(['OpenADC', 'Trigger Setup', 'Downsample Factor'])
        if ds_param is not None and ds_param.getValue() != 1:
            logging.warning("OpenADC downsampling is enabled - SAD trigger will not work")

        data = self.getCaptueTraceRef()

        if len(data) != 128:
            logging.warning('Reference IS NOT 128 samples long, got %d' % len(data))

        self.sadref = data.copy()
        self.setRefWaveform(data)
        
    def updateSADTraceRef(self, ignored=None):
        """ Update the calculated SAD value parameter """

        data = self.getCaptueTraceRef()
        diff = data - self.sadref
        diff = sum(abs(diff))
        self.findParam(['sad','sadrefcur']).setValue(diff, ignoreReadonly=True)

    def reset(self):
        """ Reset the SAD hardware block. The ADC clock must be running! """

        data = self.oa.sendMessage(CODE_READ, sadcfgaddr, maxResp=4)
        data[0] = 0x01
        self.oa.sendMessage(CODE_WRITE, sadcfgaddr, data)

        if self.checkStatus():
            raise IOError("SAD Reset in progress, but SAD reports still running. Is ADC Clock stopped?")

        data[0] = 0x00
        self.oa.sendMessage(CODE_WRITE, sadcfgaddr, data)

    def start(self):
        """ Start the SAD algorithm, which causes the reference data to be loaded from the FIFO """

        data = self.oa.sendMessage(CODE_READ, sadcfgaddr, maxResp=4)
        data[0] = 0x02
        self.oa.sendMessage(CODE_WRITE, sadcfgaddr, data, Validate=False)
        data[0] = 0x00
        self.oa.sendMessage(CODE_WRITE, sadcfgaddr, data, Validate=False)

    def checkStatus(self):
        """ Check if the SAD module is running & outputting valid data """

        data = self.oa.sendMessage(CODE_READ, sadcfgaddr, maxResp=4)
        if not (data[0] & self.STATUS_RUNNING_MASK):
            return False
        else:
            return True

    def getThreshold(self):
        """ Get the threshold. When the SAD output falls below this threshold the system triggers """

        data = self.oa.sendMessage(CODE_READ, sadcfgaddr, maxResp=4)
        threshold = data[1]
        threshold |= data[2] << 8
        threshold |= data[3] << 16
        return threshold

    @setupSetParam("SAD Threshold")
    def setThreshold(self, threshold):
        """ Set the threshold. When the SAD output falls below this threshold the system triggers """

        data = self.oa.sendMessage(CODE_READ, sadcfgaddr, maxResp=4)
        data[1] = threshold & 0xff
        data[2] = (threshold >> 8) & 0xff
        data[3] = (threshold >> 16) & 0xff
        self.oa.sendMessage(CODE_WRITE, sadcfgaddr, data, Validate=False)

        if self.checkStatus() == False:
            raise IOError("SAD Threshold set, but SAD compare not running. No valid trigger will be present. Did you load a reference waveform?")

    def setRefWaveform(self, dataRef):
        """ Download a reference waveform. Resets the SAD module & starts it again after loading the new data. ADC Clock must be running! """

        dataRefInt = [int(i) for i in dataRef]

        self.reset()

        # print dataRefInt
        dataRefInt = dataRefInt[::-1]

        wavedata = []
        for d in dataRefInt:
            wavedata.append((d >> 8) & 0xff)
            wavedata.append(d & 0xff)

        self.oa.sendMessage(CODE_WRITE, saddataaddr, wavedata, Validate=False)
        self.start()


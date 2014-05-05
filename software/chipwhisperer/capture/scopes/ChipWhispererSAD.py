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

import sys
from functools import partial
import time

from PySide.QtCore import *
from PySide.QtGui import *

try:
    from pyqtgraph.parametertree import Parameter
except ImportError:
    print "ERROR: PyQtGraph is required for this program"
    sys.exit()

from openadc.ExtendedParameter import ExtendedParameter
import chipwhisperer.common.ParameterTypesCustom

sadcfgaddr = 53
saddataaddr = 54
CODE_READ       = 0x80
CODE_WRITE      = 0xC0


class ChipWhispererSAD(QObject):
    paramListUpdated = Signal(list)
             
    def __init__(self, showScriptParameter=None, CWMainWindow=None):

        self.waveformDock = CWMainWindow.waveformDock
        self.waveformDock.widget().dataChanged.connect(self.dataChanged)

        paramSS = [
                # {'name':'Open SAD Viewer', 'type':'action'},
                 {'name':'SAD Ref From Captured', 'type':'group', 'children':[
                    {'name':'Point Range', 'key':'pointrng', 'type':'rangegraph', 'limits':(0, 0), 'value':(0, 0), 'graphwidget':self.waveformDock.widget()},
                    {'name':'Copy Data from Current Trace', 'key':'docopyfromcapture', 'type':'action', 'action':self.copyFromCaptureTrace},
                    ]},
                {'name':'SAD Threshold', 'type':'int', 'range':(0, 100000), 'value':0, 'set':self.setThreshold, 'get':self.getThreshold}
                ]
            
        self.oa = None
        self.params = Parameter.create(name='SAD Trigger Module', type='group', children=paramSS)
        ExtendedParameter.setupExtended(self.params, self)
        self.showScriptParameter = showScriptParameter
        
    def paramTreeChanged(self, param, changes):
        if self.showScriptParameter is not None:
            self.showScriptParameter(param, changes, self.params)

    def dataChanged(self, data, offset):
        low = offset
        up = offset + len(data) - 1
        self.findParam('pointrng').setLimits((low, up))
        self.findParam('pointrng').setValue((low, min(up, low + 128)))

    def copyFromCaptureTrace(self):
        pstart = self.findParam('pointrng').value()[0]
        pend = self.findParam('pointrng').value()[1]

        data = self.waveformDock.widget().lastTraceData[pstart:pend]
        print len(data)
        self.setRefWaveform(data)


    def reset(self):
        data = self.oa.sendMessage(CODE_READ, sadcfgaddr, maxResp=4)
        data[0] = 0x01
        self.oa.sendMessage(CODE_WRITE, sadcfgaddr, data)
        data[0] = 0x00
        self.oa.sendMessage(CODE_WRITE, sadcfgaddr, data)

    def start(self):
        data = self.oa.sendMessage(CODE_READ, sadcfgaddr, maxResp=4)
        data[0] = 0x02
        self.oa.sendMessage(CODE_WRITE, sadcfgaddr, data)
        data[0] = 0x00
        self.oa.sendMessage(CODE_WRITE, sadcfgaddr, data)

    def getThreshold(self):
        data = self.oa.sendMessage(CODE_READ, sadcfgaddr, maxResp=4)
        threshold = data[1]
        threshold |= data[2] << 8
        threshold |= data[3] << 16
        return threshold

    def setThreshold(self, threshold):
        data = self.oa.sendMessage(CODE_READ, sadcfgaddr, maxResp=4)
        data[1] = threshold & 0xff
        data[2] = (threshold >> 8) & 0xff
        data[3] = (threshold >> 16) & 0xff
        self.oa.sendMessage(CODE_WRITE, sadcfgaddr, data)

    def setRefWaveform(self, dataRefFP):
        # HACK: Covert back to ADC Type
        dataRef = [((i + 0.5) * 1024) for i in dataRefFP]
        dataRefInt = [int(i) for i in dataRef]

        self.reset()

        print dataRefInt

        wavedata = []
        for d in dataRefInt:
            wavedata.append((d >> 8) & 0xff)
            wavedata.append(d & 0xff)

        self.oa.sendMessage(CODE_WRITE, saddataaddr, wavedata, Validate=False)
        self.start()

    def setOpenADC(self, oa):
        self.oa = oa.sc
            
        try:
            self.params.getAllParameters()
        except TypeError:
            return
        
    def paramList(self):
        p = []
        p.append(self.params)            
        return p
    

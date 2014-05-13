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
import numpy as np

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
             
    STATUS_RUNNING_MASK = 1<<3
    STATUS_RESET_MASK = 1<<0
    STATUS_START_MASK = 1 << 1
             
    def __init__(self, showScriptParameter=None, CWMainWindow=None):

        self.waveformDock = CWMainWindow.waveformDock
        self.waveformDock.widget().dataChanged.connect(self.dataChanged)

        paramSS = [
                # {'name':'Open SAD Viewer', 'type':'action'},
                 {'name':'SAD Ref From Captured', 'type':'group', 'children':[
                    {'name':'Point Range', 'key':'pointrng', 'type':'rangegraph', 'limits':(0, 0), 'value':(0, 0),
                                           'graphwidget':self.waveformDock.widget(), 'set':self.updateSADTraceRef, 'fixedsize':128},
                    {'name':'Set SAD Reference from Current Trace', 'key':'docopyfromcapture', 'type':'action', 'action':self.copyFromCaptureTrace},
                    {'name':'SAD Reference vs. Cursor', 'key':'sadrefcur', 'type':'int', 'limits':(-1, 100E6), 'readonly':True},
                    ]},
                {'name':'SAD Threshold', 'type':'int', 'range':(0, 100000), 'value':0, 'set':self.setThreshold, 'get':self.getThreshold}
                ]
            
        self.oldlow = None
        self.oldhigh = None
        self.oa = None
        self.sadref = [0]
        self.params = Parameter.create(name='SAD Trigger Module', type='group', children=paramSS)
        ExtendedParameter.setupExtended(self.params, self)
        self.showScriptParameter = showScriptParameter
        
    def paramTreeChanged(self, param, changes):
        if self.showScriptParameter is not None:
            self.showScriptParameter(param, changes, self.params)

    def dataChanged(self, data, offset):
        low = offset
        up = offset + len(data) - 1

        if self.oldlow != low or self.oldup != up:
            self.oldlow = low
            self.oldup = up
            self.findParam('pointrng').setLimits((low, up))
            self.findParam('pointrng').setValue((low, min(up, low + 128)))

        self.updateSADTraceRef()

    def getCaptueTraceRef(self):
        pstart = self.findParam('pointrng').value()[0]
        pend = self.findParam('pointrng').value()[1]
        data = self.waveformDock.widget().lastTraceData[pstart:pend]
        data = np.array(data)
        data = (data + 0.5) * 1024
        return data

    def copyFromCaptureTrace(self):
        data = self.getCaptueTraceRef()

        if len(data) != 128:
            print "WARNING: Reference IS NOT 128 samples long"

        self.sadref = data.copy()
        self.setRefWaveform(data)
        
    def updateSADTraceRef(self, ignored=None):
        data = self.getCaptueTraceRef()
        diff = data - self.sadref
        diff = sum(abs(diff))
        self.findParam('sadrefcur').setValue(diff)

    def reset(self):
        data = self.oa.sendMessage(CODE_READ, sadcfgaddr, maxResp=4)
        data[0] = 0x01
        self.oa.sendMessage(CODE_WRITE, sadcfgaddr, data)

        if self.checkStatus():
            raise IOError("SAD Reset in progress, but SAD reports still running! Critical Error.")

        data[0] = 0x00
        self.oa.sendMessage(CODE_WRITE, sadcfgaddr, data)

    def start(self):
        data = self.oa.sendMessage(CODE_READ, sadcfgaddr, maxResp=4)
        data[0] = 0x02
        self.oa.sendMessage(CODE_WRITE, sadcfgaddr, data, Validate=False)
        data[0] = 0x00
        self.oa.sendMessage(CODE_WRITE, sadcfgaddr, data, Validate=False)

    def checkStatus(self):
        data = self.oa.sendMessage(CODE_READ, sadcfgaddr, maxResp=4)
        if not (data[0] & self.STATUS_RUNNING_MASK):
            return False
        else:
            return True

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
        self.oa.sendMessage(CODE_WRITE, sadcfgaddr, data, Validate=False)

        if self.checkStatus() == False:
            raise IOError("SAD Threshold set, but SAD compare not running. No valid trigger will be present. Did you load a reference waveform?")

    def setRefWaveform(self, dataRef):
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
    

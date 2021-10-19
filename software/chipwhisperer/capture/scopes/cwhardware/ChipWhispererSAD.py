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
import numpy as np
from collections import OrderedDict
from ....common.utils import util

from ....logging import *

sadcfgaddr  = 53
saddataaddr = 54
CODE_READ   = 0x80
CODE_WRITE  = 0xC0


class ChipWhispererSAD:
    """Communicates with the SAD module inside the CW Pro

    This submodule is only available on the ChipWhisperer1200 Pro

    Example::

        trace, ret = cw.capture_trace(scope, data, text, key)
        cw.SAD.reference = trace[400:528]
        cw.SAD.threshold = 1000
        cw.SAD.start()
        cw.trigger.module = 'SAD'

        #SAD trigger active
        trace, ret = cw.capture_trace(scope, data, text, key)
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

    def _dict_repr(self):
        rtn = OrderedDict()
        rtn['threshold'] = self.threshold
        rtn['reference'] = self.reference
        return rtn

    def __repr__(self):
        return util.dict_to_str(self._dict_repr())

    def __str__(self):
        return self.__repr__()

    @property
    def threshold(self):
        """ The threshold for the SAD trigger.

        The threshold has a maximum value of 100 000. You should set the
        reference waveform via SAD.reference before setting this value.

        :Getter: Return the current threshold

        :Setter: Set the current threshold

        Raises:
            ValueError: The user attempted to set a threshold above 100 000
            IOError: User attempted to set the threshold before the reference
                waveform.
        """
        return self.getThreshold()

    @threshold.setter
    def threshold(self, value):
        self.setThreshold(value)

    @property
    def reference(self):
        """Set the reference data for the SAD Trigger.

        The reference must be 128 samples long. Through this interface,
        it is represented as a numpy array of floats between -0.5 and 0.5
        (the same as trace data).

        :Getter: Gets the currently set SAD reference

        :Setter: Sets the SAD reference

        Raises:
            ValueError: Data not 128 samples long
        """
        return np.array(self.sadref)


    @reference.setter
    def reference(self, data):
        self.set_reference(data)

    def set_reference(self, data):
        data = np.array(data)
        data = (data + 0.5) * 1024
        self.sadref = data
        self.setRefWaveform(data)



    def reset(self):
        """ Reset the SAD hardware block. The ADC clock must be running! """

        data = self.oa.sendMessage(CODE_READ, sadcfgaddr, maxResp=4)
        data[0] = 0x01
        self.oa.sendMessage(CODE_WRITE, sadcfgaddr, data)

        if self.check_status():
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

    def check_status(self):
        """ Check if the SAD module is running & outputting valid data """

        data = self.oa.sendMessage(CODE_READ, sadcfgaddr, maxResp=4)
        if not (data[0] & self.STATUS_RUNNING_MASK):
            return False
        else:
            return True

    checkStatus = util.camel_case_deprecated(check_status)

    def getThreshold(self):
        """ Get the threshold. When the SAD output falls below this threshold the system triggers """

        data = self.oa.sendMessage(CODE_READ, sadcfgaddr, maxResp=4)
        threshold = data[1]
        threshold |= data[2] << 8
        threshold |= data[3] << 16
        return threshold

    def setThreshold(self, threshold):
        """ Set the threshold. When the SAD output falls below this threshold the system triggers """

        if (threshold > 100000) or (threshold < 0):
            raise ValueError("Invalid threshold {}. Must be in range (0, 100000)".format(threshold))
        data = self.oa.sendMessage(CODE_READ, sadcfgaddr, maxResp=4)
        data[1] = threshold & 0xff
        data[2] = (threshold >> 8) & 0xff
        data[3] = (threshold >> 16) & 0xff
        self.oa.sendMessage(CODE_WRITE, sadcfgaddr, data, Validate=False)

        if self.check_status() == False:
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


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

# CW-Pro SAD register addresses:
sadcfgaddr  = 53
saddataaddr = 54

# CW-Husky SAD register addresses:
ADDR_SAD_REF = 100
ADDR_SAD_THRESHOLD = 101
ADDR_SAD_STATUS = 102
ADDR_SAD_BITS_PER_SAMPLE = 103
ADDR_SAD_REF_SAMPLES = 104
ADDR_SAD_MULTIPLE_TRIGGERS = 106

CODE_READ   = 0x80
CODE_WRITE  = 0xC0


class ChipWhispererSAD(util.DisableNewAttr):
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
        super().__init__()

        self.oldlow = None
        self.oldhigh = None
        self.oa = oa
        self.sadref = [0]
        self.disable_newattr()

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


class HuskySAD(util.DisableNewAttr):
    """Communicates with the SAD module inside CW-Husky.

    This submodule is only available on ChipWhisperer Husky.
    If you wish for the SAD capture to include the SAD pattern, set
    scope.adc.presamples to scope.SAD._sad_reference_length + 6

    Example::

        trace = cw.capture_trace(scope, data, text, key)
        scope.SAD.reference = trace[400:]
        scope.SAD.threshold = 20
        scope.trigger.module = 'SAD'

        #SAD trigger active
        scope.adc.presamples to scope.SAD._sad_reference_length + 6
        trace = cw.capture_trace(scope, data, text, key)
    """
    _name = 'Husky SAD Trigger Module'

    def __init__(self, oa):
        super().__init__()
        self.oa = oa
        self.disable_newattr()

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
        """Threshold for SAD triggering.
        If the sum of absolute differences for the past
        scope.trigger._sad_reference_length ADC samples (measured in 8-bit
        resolution) is less than <threshold>, a trigger will be generated.
        """
        return  int.from_bytes(self.oa.sendMessage(CODE_READ, ADDR_SAD_THRESHOLD, Validate=False, maxResp=4), byteorder='little')

    @threshold.setter
    def threshold(self, val):
        if not (0 < val < 2**32):
            raise ValueError("Out of range")
        self.oa.sendMessage(CODE_WRITE, ADDR_SAD_THRESHOLD, list(int.to_bytes(val, length=4, byteorder='little')))

    @property
    def reference(self):
        """Reference waveform used for SAD triggering. Can be provided as
        integers or floats, but returned as integers. Can be provided in 8-bit
        or 12-bit sample resolution, but returned as
        scope.trigger._sad_bits_per_sample resolution. Can be of any length
        (minimum scope.trigger._sad_reference_length, and only the first
        scope.trigger._sad_reference_length samples are used).
        Args:
            wave: (list of ints or floats): reference waveform
            bits_per_sample: (int, optional): number of bits per sample in
                wave. If not provided, we use scope.adc.bits_per_sample.
        """
        return list(self.oa.sendMessage(CODE_READ, ADDR_SAD_REF, Validate=False, maxResp=self._sad_reference_length))

    @reference.setter
    def reference(self, wave, bits_per_sample=None):
        if bits_per_sample is None:
            wave_bits_per_sample = self.oa._bits_per_sample
        reflen = self._sad_reference_length
        if len(wave) < reflen:
            scope_logger.error('Reference provided is too short, it needs to be at least %d samples long' % reflen)
        # first, trim and translate reference waveform to ints:
        if type(wave[0]) is not int:
            refints = []
            for s in wave[:reflen]:
                refints.append(int(s*2**wave_bits_per_sample) + 2**(wave_bits_per_sample-1))
        else:
            refints = wave[:reflen]
        # next: if the reference resolution is 12 bits/sample, reduce it to 8:
        if self._sad_bits_per_sample != 8:
            scope_logger.error('Internal error: FPGA requires SAD reference resolution to be %d bits per sample.' % self._sad_bits_per_sample)
        else:
            if wave_bits_per_sample == 12:
                for i in range(len(refints)):
                    refints[i] = refints[i] >> 4
            self.oa.sendMessage(CODE_WRITE, ADDR_SAD_REF, refints)

    @property
    def _sad_bits_per_sample(self):
        """Read-only. Returns the number of bits per sample used by the SAD module (which is independent
        of scope.adc.bits_per_sample). Build-time parameter.
        """
        return self.oa.sendMessage(CODE_READ, ADDR_SAD_BITS_PER_SAMPLE, Validate=False, maxResp=1)[0]

    @property
    def _sad_reference_length(self):
        """Read-only. Returns the number of samples that are used by the SAD module. Build-time parameter.
        """
        return  self.oa.sendMessage(CODE_READ, ADDR_SAD_REF_SAMPLES, Validate=False, maxResp=1)[0]

    @property
    def multiple_triggers(self):
        """Set whether the SAD trigger module can issue multiple triggers once armed.
        If False, only one trigger is issued per arming, even if multiple matching patterns are observed.
        If True, beware that this can result in triggers being too close together which can result in
        segmenting errors.
        """
        if self.oa.sendMessage(CODE_READ, ADDR_SAD_MULTIPLE_TRIGGERS, Validate=False, maxResp=1)[0]:
            return True
        else:
            return False

    @multiple_triggers.setter
    def multiple_triggers(self, val):
        if val:
            self.oa.sendMessage(CODE_WRITE, ADDR_SAD_MULTIPLE_TRIGGERS, [1])
        else:
            self.oa.sendMessage(CODE_WRITE, ADDR_SAD_MULTIPLE_TRIGGERS, [0])

    @property
    def status(self):
        """SAD module status and errors.
        Intended for debugging.
        Write any value to clear.
        """
        raw = self.oa.sendMessage(CODE_READ, ADDR_SAD_STATUS, Validate=False, maxResp=1)[0]
        stat = ''
        if raw & 1:   stat += 'triggered, '
        if raw & 2:   stat += 'SAD FIFO underflow, '
        if raw & 4:   stat += 'SAD FIFO overflow, '
        if raw & 8:   stat += 'SAD FIFO not empty, '
        if stat == '':
            stat = 'not triggered'
        return stat

    @status.setter
    def status(self, val):
        # value written doesn't matter: the write action clears the status flags
        self.oa.sendMessage(CODE_WRITE, ADDR_SAD_STATUS, [1])



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
import math
from ....common.utils import util

from ....logging import *

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
        rtn = {}
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

        data = self.oa.sendMessage(CODE_READ, "SAD_STATUSCFG_ADDR", maxResp=4)
        data[0] = 0x01
        self.oa.sendMessage(CODE_WRITE, "SAD_STATUSCFG_ADDR", data)

        if self.check_status():
            raise IOError("SAD Reset in progress, but SAD reports still running. Is ADC Clock stopped?")

        data[0] = 0x00
        self.oa.sendMessage(CODE_WRITE, "SAD_STATUSCFG_ADDR", data)

    def start(self):
        """ Start the SAD algorithm, which causes the reference data to be loaded from the FIFO """

        data = self.oa.sendMessage(CODE_READ, "SAD_STATUSCFG_ADDR", maxResp=4)
        data[0] = 0x02
        self.oa.sendMessage(CODE_WRITE, "SAD_STATUSCFG_ADDR", data, Validate=False)
        data[0] = 0x00
        self.oa.sendMessage(CODE_WRITE, "SAD_STATUSCFG_ADDR", data, Validate=False)

    def check_status(self):
        """ Check if the SAD module is running & outputting valid data """

        data = self.oa.sendMessage(CODE_READ, "SAD_STATUSCFG_ADDR", maxResp=4)
        if not (data[0] & self.STATUS_RUNNING_MASK):
            return False
        else:
            return True

    checkStatus = util.camel_case_deprecated(check_status)

    def getThreshold(self):
        """ Get the threshold. When the SAD output falls below this threshold the system triggers """

        data = self.oa.sendMessage(CODE_READ, "SAD_STATUSCFG_ADDR", maxResp=4)
        threshold = data[1]
        threshold |= data[2] << 8
        threshold |= data[3] << 16
        return threshold

    def setThreshold(self, threshold):
        """ Set the threshold. When the SAD output falls below this threshold the system triggers """

        if (threshold > 100000) or (threshold < 0):
            raise ValueError("Invalid threshold {}. Must be in range (0, 100000)".format(threshold))
        data = self.oa.sendMessage(CODE_READ, "SAD_STATUSCFG_ADDR", maxResp=4)
        data[1] = threshold & 0xff
        data[2] = (threshold >> 8) & 0xff
        data[3] = (threshold >> 16) & 0xff
        self.oa.sendMessage(CODE_WRITE, "SAD_STATUSCFG_ADDR", data, Validate=False)

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

        self.oa.sendMessage(CODE_WRITE, "SAD_REFDATA_ADDR", wavedata, Validate=False)
        self.start()


class Lister(list):
    """Class that behaves like a list, but can set individual elements using a getter/setter.
    """
    def __setitem__(self, *args, **kwargs):
        oldval = self._getter()
        oldval[args[0]] = args[1]
        self._setter(oldval)
        pass

    def __repr__(self):
        oldrepr = super().__repr__()
        return f"Lister({oldrepr})"

    def __init__(self, *args, **kwargs):
        if "getter" not in kwargs:
            raise KeyError("Lister requires a getter")
        if "setter" not in kwargs:
            raise KeyError("Lister requires a setter")
        
        self._getter = kwargs.pop("getter")
        self._setter = kwargs.pop("setter")
        super().__init__(*args, **kwargs)



class HuskySAD(util.DisableNewAttr):
    """Communicates with the SAD module inside CW-Husky.

    Because SAD is by far the largest FPGA module, XADC alarms
    (temperature/voltage) shut down the SAD.

    This submodule is only available on ChipWhisperer Husky.
    If you wish for the SAD capture to include the SAD pattern, set
    scope.adc.presamples to scope.SAD.sad_reference_length + scope.SAD.latency
    (sad_reference_length because the trigger is issued *after* the reference
    samples have passed; latency because the SAD trigger logic has additional
    latency due to pipelined logic)

    Example::

        trace = cw.capture_trace(scope, data, text, key)
        scope.SAD.reference = trace[400:]
        scope.SAD.threshold = 100
        scope.trigger.module = 'SAD'

        #SAD trigger active
        scope.adc.presamples = scope.SAD.sad_reference_length + scope.SAD.latency
        trace = cw.capture_trace(scope, data, text, key)
    """
    _name = 'Husky SAD Trigger Module'

    def __init__(self, oa):
        super().__init__()
        self.oa = oa
        self._samples_enabled = self.sad_reference_length # TODO: emode?
        self._reference = []

        # determine size of SAD_TRIGGER_TIME register:
        regsizebits = math.ceil(math.log2(self._samples_enabled))
        regsizebytes = regsizebits // 8
        if regsizebits % 8:
            regsizebytes += 1
        self._trigger_advance_bytes = regsizebytes

        self.disable_newattr()

    def _dict_repr(self):
        rtn = {}
        rtn['threshold'] = self.threshold
        if self._im:
            rtn['interval_threshold'] = self.interval_threshold
        if not self.emode:
            rtn['trigger_advance'] = self.trigger_advance
        rtn['reference'] = self.reference
        rtn['sad_reference_length'] = self.sad_reference_length
        rtn['samples_enabled'] = self.samples_enabled
        rtn['multiple_triggers'] = self.multiple_triggers
        rtn['num_triggers_seen'] = self.num_triggers_seen
        rtn['always_armed'] = self.always_armed
        return rtn

    def __repr__(self):
        return util.dict_to_str(self._dict_repr())

    def __str__(self):
        return self.__repr__()

    @property
    def threshold(self):
        """Threshold for SAD triggering.
        TODO: update for interval mode
        If the sum of absolute differences for the past
        scope.trigger.sad_reference_length ADC samples (measured in 8-bit
        resolution) is less than <threshold>, a trigger will be generated.
        Husky uses 128 or 64 samples at 8 bits resolution for SAD (independent of
        scope.adc.bits_per_sample), while CW-Pro uses 128 samples at 10 bits
        resolution, so if you're used to setting SAD thresholds on CW-Pro,
        simply scale accordingly.
        The maximum threshold is a build-time parameter.

        Raises:
            ValueError: if setting a threshold higher than what the hardware
            supports.  If you would like a higher threshold than what's
            possible, you can turn off comparison for some samples via
            enabled_samples, which effectively increases the threshold range.
        """
        return  int.from_bytes(self.oa.sendMessage(CODE_READ, "SAD_THRESHOLD", Validate=False, maxResp=4), byteorder='little')

    @threshold.setter
    def threshold(self, val):
        max_threshold = 2**(self._sad_counter_width-1)
        if not (0 < val <= max_threshold):
            raise ValueError("Out of range; maximum supported by hardware: %d" % max_threshold)
        self.oa.sendMessage(CODE_WRITE, "SAD_THRESHOLD", list(int.to_bytes(val, length=4, byteorder='little')))


    @property
    def interval_threshold(self):
        """Interval threshold for SAD triggering.
        TODO...
        """
        return self.oa.sendMessage(CODE_READ, "SAD_INTERVAL_THRESHOLD", Validate=False, maxResp=1)[0]

    @interval_threshold.setter
    def interval_threshold(self, val):
        max_threshold = 2**self._sad_bits_per_sample-1
        if not (0 < val <= max_threshold):
            raise ValueError("Out of range; maximum supported by hardware: %d" % max_threshold)
        self.oa.sendMessage(CODE_WRITE, "SAD_INTERVAL_THRESHOLD", [val])


    @property
    def reference(self):
        """Reference waveform used for SAD triggering. Can be provided as
        integers or floats, but returned as integers. Can be provided in 8-bit
        or 12-bit sample resolution, but returned as
        scope.trigger._sad_bits_per_sample resolution. Can be of any length
        (minimum scope.SAD.sad_reference_length) but only the first
        scope.SAD.sad_reference_length samples are used).
        TODO: update for non-readability

        Args:
            wave: (list of ints or floats): reference waveform
            bits_per_sample: (int, optional): number of bits per sample in wave. If not provided, we use scope.adc.bits_per_sample.
        """
        return self._reference
        if self.sad_reference_length > 128:
            # in this case we have to read in blocks of 128 bytes:
            base = 0
            bytes_read = 0
            ref = []
            while bytes_read < self.sad_reference_length:
                self.oa.sendMessage(CODE_WRITE, "SAD_REFERENCE_BASE", [base])
                ref.extend(list(self.oa.sendMessage(CODE_READ, "SAD_REFERENCE", Validate=False, maxResp=128)))
                bytes_read += 128
                base += 1
            # reset the base register to normal:
            self.oa.sendMessage(CODE_WRITE, "SAD_REFERENCE_BASE", [0])
            return ref[:self.sad_reference_length]

        else:
            return list(self.oa.sendMessage(CODE_READ, "SAD_REFERENCE", Validate=False, maxResp=self.sad_reference_length))

    @reference.setter
    def reference(self, wave, bits_per_sample=None):
        if not self._writing_allowed:
            import datetime
            now = datetime.datetime.now()
            scope_logger.error('writing not allowed! this is going to screw things up (%s)' % now)
            return
        if bits_per_sample is None:
            wave_bits_per_sample = self.oa._bits_per_sample
        reflen = self.sad_reference_length
        if len(wave) < reflen:
            scope_logger.info('Reference provided is too short (it should be at least %d samples long); extending with zeros' % reflen)
            wave.extend([0]*(reflen-len(wave)))
        # if emode is supported but off, we still need to write the full set of reference samples:
        if self._emode_off:
            reflen *= 2
            if len(wave) < reflen:
                scope_logger.info('XXX extending...')
                reflen *= 2
                wave.extend([0]*(reflen-len(wave)))

        # first, trim and translate reference waveform to ints:
        refints = []
        if type(wave[0]) not in [int, np.uint8, np.uint16]:
            for s in wave[:reflen]:
                refints.append(int(s*2**self._sad_bits_per_sample) + 2**(self._sad_bits_per_sample-1))
        else:
            for i in range(reflen):
                refints.append(int(wave[i] >> (wave_bits_per_sample-self._sad_bits_per_sample)))
        if self._sad_bits_per_sample != 8:
            scope_logger.error('Internal error: FPGA requires SAD reference resolution to be %d bits per sample.' % self._sad_bits_per_sample)
        else:
            # TODO: shouldn't need to break it down in blocks anymore? or does the batch transfer have an upper limit?
            self._reference = refints
            if len(refints) > 128:
                # in this case we have to write in blocks of 128 bytes:
                base = 0
                bytes_written = 0
                while bytes_written < len(refints):
                    self.oa.sendMessage(CODE_WRITE, "SAD_REFERENCE_BASE", [base])
                    start = base*128
                    stop = min(start + 128, len(refints))
                    bytes_written += (stop-start)
                    self.oa.sendMessage(CODE_WRITE, "SAD_REFERENCE", refints[start:stop])
                    base += 1
                # reset the base register to normal:
                self.oa.sendMessage(CODE_WRITE, "SAD_REFERENCE_BASE", [0])
            else:
                self.oa.sendMessage(CODE_WRITE, "SAD_REFERENCE", refints)

    @property
    def _sad_bits_per_sample(self):
        """Read-only. Returns the number of bits per sample used by the SAD module (which is independent
        of scope.adc.bits_per_sample). Build-time parameter.
        """
        return self.oa.sendMessage(CODE_READ, "SAD_BITS_PER_SAMPLE", Validate=False, maxResp=1)[0]

    @property
    def _sad_counter_width(self):
        """Read-only. Returns the number of bits per sample used by the SAD module (which is independent
        of scope.adc.bits_per_sample), which in turn determines the maximum threshold. Build-time parameter.
        """
        return self.oa.sendMessage(CODE_READ, "SAD_COUNTER_WIDTH", Validate=False, maxResp=1)[0]

    @property
    def sad_reference_length(self):
        """Read-only. Returns the number of samples that are used by the SAD module. Hardware property.
        TODO: clearly document how emode is handled here!
        """
        l = int.from_bytes(self.oa.sendMessage(CODE_READ, "SAD_REF_SAMPLES", Validate=False, maxResp=2), byteorder='little')
        if self._emode_off:
            l //= 2
        return l

    @property
    def latency(self):
        """Read-only. Returns the SAD module's triggering latency. This is implementation-dependent
        so it is read from an FPGA register.
        """
        raw = self.oa.sendMessage(CODE_READ, "SAD_VERSION", Validate=False, maxResp=1)[0]
        if raw == 0:
            # assume this is an earlier bitfile which did not implement the SAD_VERSION register, and assume its latency is 9:
            return 9
        else:
            return raw & 0x1f

    @property
    def _implementation(self):
        """Read-only. Indicates which SAD module was used to create the current FPGA bitfile.
        """
        raw = self.oa.sendMessage(CODE_READ, "SAD_VERSION", Validate=False, maxResp=1)[0]
        version_bits = (raw & 0xe0) >> 5
        if version_bits == 0:
            return 'base'
        elif version_bits == 2:
            return 'X2_slow'
        elif version_bits == 4:
            return 'X4_slow'
        elif version_bits == 5:
            return 'X2_fast'
        elif version_bits == 6:
            return 'eSAD'
        elif version_bits == 7:
            return 'single_counter'
        else:
            raise ValueError("Unexpected version bits: %d" % version_bits)

    @property
    def _esad_support(self):
        """Read-only. Indicates whether the SAD implementation supports eSAD.
        """
        raw = self.oa.sendMessage(CODE_READ, "SAD_VERSION", Validate=False, maxResp=2)[1]
        if raw & 0x02:
            return True
        else:
            return False

    @property
    def _emode_off(self):
        """Read-only. Returns true for implementations which support emode but have it
        turned off
        """
        return self._esad_support and not self.emode


    @property
    def _im(self):
        """Read-only. Indicates whether the SAD implementation uses interval matching
        """
        # TODO-temp!
        return True
        raw = self.oa.sendMessage(CODE_READ, "SAD_VERSION", Validate=False, maxResp=2)[1]
        if raw & 0x01:
            return True
        else:
            return False

    @property
    def _max_threshold_bit(self):
        """Read-only. Helps determine the max threshold supported by the SAD implementation.
        TODO: use it to validate user-supplied max threshold
        """
        raw = self.oa.sendMessage(CODE_READ, "SAD_VERSION", Validate=False, maxResp=2)[1]
        return (raw & 0x04) >> 2


    @property
    def half_pattern(self):
        """If set, reduces by half the number of samples used by the SAD module.
        Can be useful when a higher effective threshold is needed.
        """
        scope_logger.warning('This property no longer exists, use the samples_enabled property instead.')
        return False

    @half_pattern.setter
    def half_pattern(self, val):
        scope_logger.info('You can now use the samples_enabled (or, more generally, enabled_samples) property to reduce the number of samples used for SAD by any number. Executing the equivalent via samples_enabled for you.')
        num = self.sad_reference_length
        if val:
            num = num//2
        self.samples_enabled = num
        #enables = [1]*self.sad_reference_length
        #if val:
        #    enables[self.sad_reference_length//2:] = [0]*(self.sad_reference_length//2)
        #self.enabled_samples = enables


    @property
    def samples_enabled(self):
        """TODO-describe
        """
        return self._samples_enabled

    @samples_enabled.setter
    def samples_enabled(self, num):
        if num < 1 or num > self.sad_reference_length:
            raise ValueError("Must be in range [1, scope.SAD.sad_reference_length]")
        enables = [1]*self.sad_reference_length
        enables[num:] = [0]*(self.sad_reference_length-num)
        self._samples_enabled = num
        self.enabled_samples = enables


    @property
    def enabled_samples(self):
        """Control which samples of the reference pattern are enabled
        for the SAD computation.
        """
        return self.get_enabled_samples()

    @enabled_samples.setter
    def enabled_samples(self, enables):
        self.set_enabled_samples(enables)

    def get_enabled_samples(self):
        """Whether specified samples are to be used in the SAD computation.
        TODO: not readable
        """
        enables = self.read_enabled_samples()
        if type(enables) is bool:
            return enables
        else:
            return Lister(enables, setter=self.set_enabled_samples, getter=self.read_enabled_samples)

    def read_enabled_samples(self):
        # TODO: not readable
        raw = list(self.oa.sendMessage(CODE_READ, "SAD_REFEN", Validate=False, maxResp=self.sad_reference_length//8))
        enables = []
        for item in raw:
            for bit in range(8):
                if item & 2**bit:
                    enables.append(True)
                else:
                    enables.append(False)
        return enables

    def set_enabled_samples(self, enables):
        # TODO: emode/off
        raw = 0
        for i, item in enumerate(enables):
            if item: raw += 2**i
        #print('XXX DEBUG: got enables=%s, raw=%d' % (enables, raw))
        rawlist = list(int.to_bytes(raw, length=len(enables)//8, byteorder='little'))
        self.oa.sendMessage(CODE_WRITE, "SAD_REFEN", rawlist)

    @property
    def multiple_triggers(self):
        """Set whether the SAD trigger module can issue multiple triggers once armed.
        If False, only one trigger is issued per arming, even if multiple matching patterns are observed.
        If True, beware that this can result in triggers being too close together which can result in
        segmenting errors (if this happens, reduce scope.adc.samples).
        """
        if self.oa.sendMessage(CODE_READ, "SAD_CONTROL", Validate=False, maxResp=1)[0] & 0x02:
            return True
        else:
            return False

    @multiple_triggers.setter
    def multiple_triggers(self, val):
        raw = self.oa.sendMessage(CODE_READ, "SAD_CONTROL", Validate=False, maxResp=1)[0]
        if val:
            raw |= 0x02
        else:
            raw &= 0xfd
        self.oa.sendMessage(CODE_WRITE, "SAD_CONTROL", [raw])

    @property
    def triggered(self):
        """SAD module status. Intended for debugging.
        Indicates that a SAD trigger has occurred; it gets cleared by setting
        this (or num_triggers_seen) to any value or by re-arming.
        """
        raw = self.oa.sendMessage(CODE_READ, "SAD_STATUS", Validate=False, maxResp=1)[0]
        stat = ''
        if raw & 1:
            return True
        else: 
            return False

    @property
    def _writing_allowed(self):
        """TODO
        """
        raw = self.oa.sendMessage(CODE_READ, "SAD_STATUS", Validate=False, maxResp=1)[0]
        stat = ''
        if raw & 2:
            return False
        else: 
            return True


    @triggered.setter
    def triggered(self, val):
        # value written doesn't matter: the write action clears the status flags
        self.oa.sendMessage(CODE_WRITE, "SAD_STATUS", [1])

    @property
    def num_triggers_seen(self):
        """ Indicates how many triggers were generated by the SAD module;
        gets cleared by setting this (or triggered) to any value or by
        re-arming.
        """
        raw = self.oa.sendMessage(CODE_READ, "SAD_STATUS", Validate=False, maxResp=3)
        return  int.from_bytes(raw[1:], byteorder='little')

    @num_triggers_seen.setter
    def num_triggers_seen(self, val):
        self.triggered = val

    @property
    def always_armed(self):
        """ Control whether the SAD module can trigger without the scope being
        armed.  The intended use is to allow the SAD module to trigger after
        the ADC capture has completed, which can be very useful when
        calibrating the SAD threshold when using multiple SAD triggers.
        """
        if self.oa.sendMessage(CODE_READ, "SAD_CONTROL", Validate=False, maxResp=1)[0] & 0x01:
            return True
        else:
            return False

    @always_armed.setter
    def always_armed(self, val):
        raw = self.oa.sendMessage(CODE_READ, "SAD_CONTROL", Validate=False, maxResp=1)[0]
        if val:
            raw |= 0x01
        else:
            raw &= 0xfe
        self.oa.sendMessage(CODE_WRITE, "SAD_CONTROL", [raw])

    @property
    def emode(self):
        """ Control whether the SAD module... TODO: block access (to setting only; read is required for all) if not _esad_support
        """
        if self.oa.sendMessage(CODE_READ, "SAD_CONTROL", Validate=False, maxResp=1)[0] & 0x04:
            return True
        else:
            return False

    @emode.setter
    def emode(self, val):
        raw = self.oa.sendMessage(CODE_READ, "SAD_CONTROL", Validate=False, maxResp=1)[0]
        if val:
            raw |= 0x04
        else:
            raw &= 0xfb
        self.oa.sendMessage(CODE_WRITE, "SAD_CONTROL", [raw])

    @property
    def trigger_advance(self):
        """ Control whether the SAD module... TODO
        TODO: limit access to implementations that support it;
        tie register size to # of ref samples;
        explain that not supported in emode
        """
        raw = self.oa.sendMessage(CODE_READ, "SAD_TRIGGER_TIME", Validate=False, maxResp=self._trigger_advance_bytes)
        return raw # TODO...

    @trigger_advance.setter
    def trigger_advance(self, val):
        # TODO: sanity check on val
        # TODO: handle emode/off?
        triggerer_init = (-self.val-3) % self.sad_reference_length
        self.oa.sendMessage(CODE_WRITE, "SAD_TRIGGER_TIME", list(int.to_bytes(triggerer_init, length=self._trigger_advance_bytes, byteorder='little')))


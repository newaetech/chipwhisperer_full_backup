# -*- coding: cp1252 -*-

# This file is part of the OpenADC Project. See www.newae.com for more details,
# or the codebase at http://www.assembla.com/spaces/openadc .
#
# Copyright (c) 2012-2013, Colin O'Flynn <coflynn@newae.com>. All rights reserved.
# This project is released under the Modified FreeBSD License. See LICENSE
# file which should have came with this code.
import logging
import sys
import time
import datetime
from chipwhisperer.common.utils import util
import array
import numpy as np
from collections import OrderedDict
import copy

ADDR_GAIN       = 0
ADDR_SETTINGS   = 1
ADDR_STATUS     = 2
ADDR_ADCDATA    = 3
ADDR_ECHO       = 4
ADDR_FREQ       = 5
ADDR_ADVCLK     = 6
ADDR_SYSFREQ    = 7
ADDR_ADCFREQ    = 8
ADDR_PHASE      = 9
ADDR_VERSIONS   = 10
ADDR_OFFSET     = 26
ADDR_DECIMATE   = 15
ADDR_SAMPLES    = 16
ADDR_PRESAMPLES = 17
ADDR_BYTESTORX  = 18
ADDR_TRIGGERDUR = 20
ADDR_MULTIECHO  = 34

CODE_READ       = 0x80
CODE_WRITE      = 0xC0

SETTINGS_RESET     = 0x01
SETTINGS_GAIN_HIGH = 0x02
SETTINGS_GAIN_LOW  = 0x00
SETTINGS_TRIG_HIGH = 0x04
SETTINGS_TRIG_LOW  = 0x00
SETTINGS_ARM       = 0x08
SETTINGS_WAIT_YES  = 0x20
SETTINGS_WAIT_NO   = 0x00
SETTINGS_TRIG_NOW  = 0x40

STATUS_ARM_MASK    = 0x01
STATUS_FIFO_MASK   = 0x02
STATUS_EXT_MASK    = 0x04
STATUS_DCM_MASK    = 0x08
STATUS_DDRCAL_MASK = 0x10
STATUS_DDRERR_MASK = 0x20
STATUS_DDRMODE_MASK= 0x40
STATUS_OVERFLOW_MASK = 0x80

# sign extend b low bits in x
# from "Bit Twiddling Hacks"
def SIGNEXT(x, b):
    m = 1 << (b - 1)
    x = x & ((1 << b) - 1)
    return (x ^ m) - m


class HWInformation(util.DisableNewAttr):
    _name = 'HW Information'

    def __init__(self, oaiface):
        self.oa = oaiface
        self.oa.hwInfo = self
        self.sysFreq = 0

        self.vers = None
        self.disable_newattr()

    def versions(self):
        result = self.oa.sendMessage(CODE_READ, ADDR_VERSIONS, maxResp=6)

        regver = result[0] & 0xff
        hwtype = result[1] >> 3
        hwver = result[1] & 0x07
        hwList = ["Default/Unknown", "LX9 MicroBoard", "SASEBO-W", "ChipWhisperer Rev2 LX25",
                  "Reserved?", "ZedBoard", "Papilio Pro", "SAKURA-G", "ChipWhisperer-Lite", "ChipWhisperer-CW1200"]

        try:
            textType = hwList[hwtype]
        except:
            textType = "Invalid/Unknown"

        self.vers = (regver, hwtype, textType, hwver)

        #TODO: Temp fix for wrong HW reporting
        if hwtype == 1:
            self.sysFreq = 40E6

        return self.vers

    def is_cw1200(self):
        if self.vers is None:
            self.versions()

        if self.vers[1] == 9:
            return True
        else:
            return False

    def is_cwlite(self):
        if self.vers is None:
            self.versions()

        if self.vers[1] == 8:
            return True
        else:
            return False

    def synthDate(self):
        return "unknown"

    def maxSamples(self):
        return self.oa.hwMaxSamples

    def sysFrequency(self, force=False):
        if (self.sysFreq > 0) & (force == False):
            return self.sysFreq

        '''Return the system clock frequency in specific firmware version'''
        freq = 0x00000000

        temp = self.oa.sendMessage(CODE_READ, ADDR_SYSFREQ, maxResp=4)
        freq = freq | (temp[0] << 0)
        freq = freq | (temp[1] << 8)
        freq = freq | (temp[2] << 16)
        freq = freq | (temp[3] << 24)

        self.sysFreq = int(freq)
        return self.sysFreq

    def __del__(self):
        self.oa.hwInfo = None


class GainSettings(util.DisableNewAttr):
    _name = 'Gain Setting'

    def __init__(self, oaiface):
        self.oa = oaiface
        self.gainlow_cached = False
        self.gain_cached = 0
        self.disable_newattr()

    def _dict_repr(self):
        dict = OrderedDict()
        dict['mode'] = self.mode
        dict['gain'] = self.gain
        dict['db'] = self.db
        return dict

    def __repr__(self):
        return util.dict_to_str(self._dict_repr())

    def __str__(self):
        return self.__repr__()

    @property
    def db(self):
        """The gain of the ChipWhisperer's low-noise amplifier in dB. Ranges
        from -6.5 dB to 56 dB, depending on the amplifier settings.

        :Getter: Return the current gain in dB (float)

        :Setter: Set the gain level in dB

        Raises:
           ValueError: if new gain is outside of [-6.5, 56]

        Examples::

            # reading and storing
            gain_db = scope.gain.db

            # setting
            scope.gain.db = 20
        """
        return self._get_gain_db()

    @db.setter
    def db(self, val):
        return self._set_gain_db(val)

    def setMode(self, gainmode):
        """Sets the ChipWhisperer's gain to either 'low' or 'high' mode.

        This setting is applied after the gain property, resulting in the value
        of the db property. May be necessary for reaching gains higher than


        Args:
           gainmode (str): Either 'low' or 'high'.

        Raises:
           ValueError: gainmode not 'low' or 'high'
        """
        if gainmode == "high":
            self.oa.setSettings(self.oa.settings() | SETTINGS_GAIN_HIGH)
            self.gainlow_cached = False
        elif gainmode == "low":
            self.oa.setSettings(self.oa.settings() & ~SETTINGS_GAIN_HIGH)
            self.gainlow_cached = True
        else:
            raise ValueError("Invalid Gain Mode, only 'low' or 'high' allowed")

    def getMode(self):
        gain_high = self.oa.settings() & SETTINGS_GAIN_HIGH
        if gain_high:
            return "high"
        else:
            return "low"

    @property
    def mode(self):
        """The current mode of the LNA.

        The LNA can operate in two modes: low-gain or high-gain. Generally, the
        high-gain setting is better to use. Note that this value will be
        automatically updated if the dB gain is set.

        :Getter: Return the current gain mode ("low" or "high")

        :Setter: Set the gain mode

        Raises:
            ValueError: if mode not one of "low" or "high"
        """
        return self.getMode()

    @mode.setter
    def mode(self, val):
        return self.setMode(val)

    def setGain(self, gain):
        '''Set the Gain range 0-78'''
        if (gain < 0) | (gain > 78):
            raise ValueError("Invalid Gain, range 0-78 Only")

        self.gain_cached = gain

        cmd = bytearray(1)
        cmd[0] = gain
        self.oa.sendMessage(CODE_WRITE, ADDR_GAIN, cmd)

    def getGain(self, cached=False):
        if cached == False:
            self.gain_cached = self.oa.sendMessage(CODE_READ, ADDR_GAIN)[0]

        return self.gain_cached

    @property
    def gain(self):
        """The current LNA gain setting.

        This gain is a dimensionless number in the range [0, 78]. Higher value
        causes higher gain in dB.

        Note that this function is unnecessary - the dB gain can be set
        directly. This property is only here to help convert old scripts.

        :Getter: Return the current gain setting (int)

        :Setter: Set the gain

        Raises:
            ValueError: if gain outside [0, 78]
        """
        return self.getGain()

    @gain.setter
    def gain(self, value):
        self.setGain(value)

    def _get_gain_db(self):
        #GAIN (dB) = 50 (dB/V) * VGAIN - 6.5 dB, (HILO = LO)
        # GAIN (dB) = 50 (dB/V) * VGAIN + 5.5 dB, (HILO = HI)

        gainV = (float(self.gain_cached) / 256.0) * 3.3

        if self.gainlow_cached:
            gaindb = 50.0 * gainV - 6.5
        else:
            gaindb = 50.0 * gainV + 5.5

        return gaindb

    def _set_gain_db(self, gain):
        if gain < -6.5 or gain > 56:
            raise ValueError("Gain " + gain + "out of range. Valid range: -6.5 to 56 dB")

        use_low = False

        if gain < 5.5:
            use_low = True

        if use_low:
            gv = (float(gain) - (-6.5)) / 50.0
        else:
            gv = (float(gain) - (5.5) ) / 50.0
        g = (gv / 3.3) * 256.0
        g = round(g)
        g = int(g)
        if g < 0:
            g = 0
        if g > 0xFF:
            g = 0xFF

        if use_low:
            self.setMode("low")
        else:
            self.setMode("high")
        self.setGain(g)

class TriggerSettings(util.DisableNewAttr):
    _name = 'Trigger Setup'

    def __init__(self, oaiface):
        self._new_attributes_disabled = False
        self.oa = oaiface
        self._numSamples = 0
        self.presamples_desired = 0
        self.presamples_actual = 0
        self.presampleTempMargin = 24
        self._timeout = 2
        self._stream_mode = False
        self._support_get_duration = True
        self._is_pro = False


        self.disable_newattr()

    def _dict_repr(self):
        dict = OrderedDict()
        dict['state']      = self.state
        dict['basic_mode'] = self.basic_mode
        dict['timeout']    = self.timeout
        dict['offset']     = self.offset
        dict['presamples'] = self.presamples
        dict['samples']    = self.samples
        dict['decimate']   = self.decimate
        dict['trig_count'] = self.trig_count
        if self._is_pro:
            dict['stream_mode'] = self.stream_mode



        return dict

    def __repr__(self):
        return util.dict_to_str(self._dict_repr())

    def __str__(self):
        return self.__repr__()

    @property
    def state(self):
        """The current state of the trigger input.

        This is a digital value (ie: high or low), which is some combination
        of the pins in the triggermux object. Read-only.

        Getter: Return the current state (True or False).
        """
        return self.extTriggerPin()

    @property
    def stream_mode(self):
        """The ChipWhisperer's streaming status. Only available on CW1200.

        When stream mode is enabled, the ChipWhisperer sends back ADC data as
        soon as it is recorded. In this mode, there is no hardware limit on the
        maximum number of samples per trace (although Python may run out of
        memory when recording billions of points). However, there is a
        maximum streaming data rate, which is approximately 10 Msamp/s.

        Note that no pre-trigger samples can be recorded when stream mode
        is enabled.

        :Getter: Return True if stream mode is enabled and False otherwise

        :Setter: Enable or disable stream mode
        """
        return self._get_stream_mode()

    @stream_mode.setter
    def stream_mode(self, enabled):
        self._set_stream_mode(enabled)

    @property
    def decimate(self):
        """The ADC downsampling factor.

        This value instructs the ChipWhisperer to only record 1 sample in
        every <decimate>. In other words, if this value is set to 10, the
        sampling rate is set to 1/10th of the sampling clock.

        This setting is helpful for recording very long operations or for
        reducing the sampling rate for streaming mode.

        :Getter: Return an integer with the current decimation factor

        :Setter: Set the decimation factor

        Raises:
           ValueError: if the new factor is not positive
        """
        return self._get_decimate()

    @decimate.setter
    def decimate(self, decfactor):
        self._set_decimate(decfactor)

    @property
    def samples(self):
        """The number of ADC samples to record in a single capture.

        The maximum number of samples is hardware-dependent:
        - cwlite: 24400
        - cw1200: 96000

        :Getter: Return the current number of total samples (integer)

        :Setter: Set the number of samples to capture

        Raises:
           ValueError: if number of samples is negative
        """
        return self._get_num_samples()

    @samples.setter
    def samples(self, samples):
        self._set_num_samples(samples)

    @property
    def timeout(self):
        """The number of seconds to wait before aborting a capture.

        If no trigger event is detected before this time limit is up, the
        capture fails and no data is returned.

        :Getter: Return the number of seconds before a timeout (float)

        :Setter: Set the timeout in seconds
        """
        return self._get_timeout()

    @timeout.setter
    def timeout(self, timeout):
        self._set_timeout(timeout)

    @property
    def offset(self):
        """The number of samples to before recording data after seeing a
        trigger event.

        This offset is useful for long operations. For instance, if an
        encryption is 1 million samples long, it's difficult to capture the
        entire power trace, but an offset can be used to skip to the end of
        the encryption.

        The offset must be a 32 bit unsigned integer.

        :Getter: Return the current offset (integer)

        :Setter: Set a new offset

        Raises:
           ValueError: if offset outside of range [0, 2**32)
        """
        return self._get_offset()

    @offset.setter
    def offset(self, setting):
        self._set_offset(setting)

    @property
    def presamples(self):
        """The number of samples to record from before the trigger event.

        This setting must be a positive integer, and it cannot be larger than
        the number of samples. When streaming mode is enabled, this value is
        set to 0.

        :Getter: Return the current number of presamples

        :Setter: Set the number of presamples.

        Raises:
           ValueError: if presamples is outside of range [0, samples]
        """
        return self._get_presamples()

    @presamples.setter
    def presamples(self, setting):
        self._set_presamples(setting)

    @property
    def basic_mode(self):
        """The type of event to use as a trigger.

        There are four possible types of trigger events:
         * "low": triggers when line is low (logic 0)
         * "high": triggers when line is high (logic 1)
         * "rising_edge": triggers when line transitions from low to high
         * "falling_edge:" triggers when line transitions from high to low

        .. warning:: This must be set to "rising_edge" if a trigger other than
            "basic" is used. The SAD/DecodeIO triggers will not work with any
            other setting!

        :Getter: Return the current trigger mode (one of the 4 above strings)

        :Setter: Set the trigger mode

        Raises:
           ValueError: if value is not one of the allowed strings
        """
        param_alias = {
            "rising edge": "rising_edge",
            "falling edge": "falling_edge",
            "high": "high",
            "low": "low"
        }
        return param_alias[self._get_mode()]

    @basic_mode.setter
    def basic_mode(self, mode):
        api_alias = {
            "rising_edge": "rising edge",
            "falling_edge": "falling edge",
            "high": "high",
            "low": "low"
        }
        if mode not in api_alias:
            raise ValueError("Invalid trigger mode %s. Valid modes: %s" % (mode, list(api_alias.keys())), mode)

        self._set_mode(api_alias[mode])

    @property
    def trig_count(self):
        """The number of samples that the trigger input was active.

        This value indicates how long the trigger was high or low last time
        a trace was captured. It is the number of samples where the input was
        low (in "low" or "falling edge" modes) or high (in "high" or "rising
        edge" modes). Read-only.

        This counter is not meaningful if the trigger is still active.

        :Getter: Return the last trigger duration (integer)
        """
        return self._get_duration()

    def _set_stream_mode(self, enabled):
        self._stream_mode = enabled

        #Write to FPGA
        base = self.oa.sendMessage(CODE_READ, ADDR_SETTINGS)[0]
        if enabled:
            val = base | (1<<4)
        else:
            val = base & ~(1<<4)
        self.oa.sendMessage(CODE_WRITE, ADDR_SETTINGS, [val])

        #Notify capture system
        self.oa.setStreamMode(enabled)

    def _get_stream_mode(self):
        return self._stream_mode

    def fifoOverflow(self):
        return self.oa.getStatus() & STATUS_OVERFLOW_MASK

    def _set_decimate(self, decsamples):
        self.oa.setDecimate(decsamples)

    def _get_decimate(self):
        return self.oa.decimate()

    def _set_num_samples(self, samples):
        if samples < 0:
            raise ValueError("Can't use negative number of samples")
        # TODO: raise ValueError or round down for sample counts too high
        # TODO: raise TypeError for non-integers
        self._numSamples = samples
        self.oa.setNumSamples(samples)

    def _get_num_samples(self, cached=False):
        if self.oa is None:
            return 0

        if cached:
            return self._numSamples
        else:
            return self.oa.numSamples()

    def _set_timeout(self, timeout):
        self._timeout = timeout
        if self.oa:
            self.oa.setTimeout(timeout)

    def _get_timeout(self):
        return self._timeout

    def _set_offset(self,  offset):
        if offset < 0:
            raise ValueError("Offset must be a non-negative integer")
        if offset >= 2**32:
            raise ValueError("Offset must fit into a 32-bit unsigned integer")

        cmd = bytearray(4)
        cmd[0] = ((offset >> 0) & 0xFF)
        cmd[1] = ((offset >> 8) & 0xFF)
        cmd[2] = ((offset >> 16) & 0xFF)
        cmd[3] = ((offset >> 24) & 0xFF)
        self.oa.sendMessage(CODE_WRITE, ADDR_OFFSET, cmd)

    def _get_offset(self):
        if self.oa is None:
            return 0

        cmd = self.oa.sendMessage(CODE_READ, ADDR_OFFSET, maxResp=4)
        offset = cmd[0]
        offset |= cmd[1] << 8
        offset |= cmd[2] << 16
        offset |= cmd[3] << 24
        return offset

    def _set_presamples(self, samples):
        if samples < 0:
            raise ValueError("Number of pre-trigger samples must be non-negative")
        if samples > self.samples:
            raise ValueError("Number of pre-trigger samples cannot be larger than total number of samples")

        self.presamples_desired = samples

        if self.oa.hwInfo.is_cw1200() or self.oa.hwInfo.is_cwlite():
            #CW-1200 Hardware / CW-Lite
            samplesact = samples
            self.presamples_actual = samples
        else:
            #Other Hardware
            if samples > 0:
                logging.warning('Pre-sample on CW-Lite is unreliable with many FPGA bitstreams. '
                                'Check data is reliably recorded before using in capture.')

            #enforce samples is multiple of 3
            samplesact = int(samples / 3)

            #Old crappy FIFO system that requires the following
            if samplesact > 0:
                samplesact = samplesact + self.presampleTempMargin

            self.presamples_actual = samplesact * 3

        cmd = bytearray(4)
        cmd[0] = ((samplesact >> 0) & 0xFF)
        cmd[1] = ((samplesact >> 8) & 0xFF)
        cmd[2] = ((samplesact >> 16) & 0xFF)
        cmd[3] = ((samplesact >> 24) & 0xFF)
        self.oa.sendMessage(CODE_WRITE, ADDR_PRESAMPLES, cmd)


        #print "Requested presamples: %d, actual: %d"%(samples, self.presamples_actual)

        self.oa.presamples_desired = samples

        return self.presamples_actual

    def _get_presamples(self, cached=False):
        """If cached returns DESIRED presamples"""
        if self.oa is None:
            return 0

        if cached:
            return self.presamples_desired

        samples = 0x00000000

        temp = self.oa.sendMessage(CODE_READ, ADDR_PRESAMPLES, maxResp=4)
        samples = samples | (temp[0] << 0)
        samples = samples | (temp[1] << 8)
        samples = samples | (temp[2] << 16)
        samples = samples | (temp[3] << 24)

        #CW1200/CW-Lite reports presamples using different method
        if (self.oa.hwInfo.vers and self.oa.hwInfo.vers[1] == 9) or (self.oa.hwInfo.vers and self.oa.hwInfo.vers[1] == 8):
            self.presamples_actual = samples

        else:
            self.presamples_actual = samples*3

        return self.presamples_actual

    def _set_mode(self,  mode):
        """ Input to trigger module options: 'rising edge', 'falling edge', 'high', 'low' """
        if mode == 'rising edge':
            trigmode = SETTINGS_TRIG_HIGH | SETTINGS_WAIT_YES

        elif mode == 'falling edge':
            trigmode = SETTINGS_TRIG_LOW | SETTINGS_WAIT_YES

        elif mode == 'high':
            trigmode = SETTINGS_TRIG_HIGH | SETTINGS_WAIT_NO

        elif mode == 'low':
            trigmode = SETTINGS_TRIG_LOW | SETTINGS_WAIT_NO

        else:
            raise ValueError("%s invalid trigger mode. Valid modes: 'rising edge', 'falling edge', 'high', 'low'"%mode)

        cur = self.oa.settings() & ~(SETTINGS_TRIG_HIGH | SETTINGS_WAIT_YES)
        self.oa.setSettings(cur | trigmode)

    def _get_mode(self):
        if self.oa is None:
            return 'low'

        sets = self.oa.settings()
        case = sets & (SETTINGS_TRIG_HIGH | SETTINGS_WAIT_YES)

        if case == SETTINGS_TRIG_HIGH | SETTINGS_WAIT_YES:
            mode = "rising edge"
        elif case == SETTINGS_TRIG_LOW | SETTINGS_WAIT_YES:
            mode = "falling edge"
        elif case == SETTINGS_TRIG_HIGH | SETTINGS_WAIT_NO:
            mode = "high"
        else:
            mode = "low"

        return mode

    def extTriggerPin(self):
        if (self.oa is not None) and (self.oa.getStatus() & STATUS_EXT_MASK):
            return True
        else:
            return False

    def _get_duration(self):
        """Returns previous trigger duration. Cleared by arm automatically. Invalid if trigger is currently active."""
        if self.oa is None:
            return 0

        samples = 0x00000000

        if self._support_get_duration:

            temp = self.oa.sendMessage(CODE_READ, ADDR_TRIGGERDUR, maxResp=4)

            #Old versions don't support this feature
            if temp is None:
                self._support_get_duration = False
                return -1

            samples = samples | (temp[0] << 0)
            samples = samples | (temp[1] << 8)
            samples = samples | (temp[2] << 16)
            samples = samples | (temp[3] << 24)

            return samples

        else:

            return -1

class ClockSettings(util.DisableNewAttr):
    _name = 'Clock Setup'
    _readMask = [0x1f, 0xff, 0xff, 0xfd]

    def __init__(self, oaiface, hwinfo=None):
        self.oa = oaiface
        self._hwinfo = hwinfo
        self._freqExt = 10e6
        self.disable_newattr()

    def _dict_repr(self):
        dict = OrderedDict()
        dict['adc_src']    = self.adc_src
        dict['adc_phase']  = self.adc_phase
        dict['adc_freq']   = self.adc_freq
        dict['adc_rate']   = self.adc_rate
        dict['adc_locked'] = self.adc_locked

        dict['freq_ctr']     = self.freq_ctr
        dict['freq_ctr_src'] = self.freq_ctr_src

        dict['clkgen_src']    = self.clkgen_src
        dict['extclk_freq']   = self.extclk_freq
        dict['clkgen_mul']    = self.clkgen_mul
        dict['clkgen_div']    = self.clkgen_div
        dict['clkgen_freq']   = self.clkgen_freq
        dict['clkgen_locked'] = self.clkgen_locked

        return dict

    def __repr__(self):
        return util.dict_to_str(self._dict_repr())

    def __str__(self):
        return self.__repr__()

    @property
    def adc_src(self):
        """The clock source for the ADC module.

        The ADC can be clocked by one of five possible sources:
         * "clkgen_x1": CLKGEN output via DCM
         * "clkgen_x4": CLKGEN output via DCM with x4 clk multiplier
         * "extclk_x1": External clock input via DCM
         * "extclk_x4": External clock input via DCM with x4 clk multiplier
         * "extclk_dir": External clock input with no DCM

        :Getter: Return the current ADC clock source (one of five strings above)

        :Setter: Set the ADC clock source and reset the ADC DCM to lock it.

        Raises:
           ValueError: if string not in valid settings
        """
        (adc_input, dcm_mul, dcm_input) = self._getAdcSource()
        if adc_input == "extclk":
            return "extclk_dir"
        else: # adc_input == "dcm"
            ret = "%s_x%d" % (dcm_input, dcm_mul)
            return ret

    @adc_src.setter
    def adc_src(self, src):
        # We need to pass a tuple into _setAdcSource() so the ADC source
        # parameter recognizes this input
        if src == "clkgen_x4":
            self._setAdcSource(("dcm", 4, "clkgen"))
        elif src == "clkgen_x1":
            self._setAdcSource(("dcm", 1, "clkgen"))
        elif src == "extclk_x4":
            self._setAdcSource(("dcm", 4, "extclk"))
        elif src == "extclk_x1":
            self._setAdcSource(("dcm", 1, "extclk"))
        elif src == "extclk_dir":
            self._setAdcSource(("extclk", 1, "extclk"))
        else:
            raise ValueError("Invalid ADC source (possible values: 'clkgen_x4', 'clkgen_x1', 'extclk_x4', 'extclk_x1', 'extclk_dir'")

        self.reset_adc()

    @property
    def adc_phase(self):
        """Fine adjustment for the ADC sampling point.

        This setting moves the sampling point approximately 5 ns forward or
        backward, regardless of the sampling frequency. It may be helpful to
        improve the stability of the measurement.

        The value of this setting is dimensionless and has a non-linear
        effect on the phase adjustment.

        :Getter: Return the current phase setting (integer)
            NOTE: This getter is currently broken due to an FPGA bug.

        :Setter: Set a new phase offset

        Raises:
           ValueError: if offset not in [-255, 255]
           TypeError: if offset not integer
        """
        return self._get_phase()

    @adc_phase.setter
    def adc_phase(self, phase):
        self._set_phase(phase)

    @property
    def adc_freq(self):
        """The current frequency of the ADC clock in Hz. Read-only.

        This clock frequency is derived from one of the ADC clock sources as
        described in adc_src.

        :Getter: Return the current frequency in MHz (float)
        """
        return self._getAdcFrequency()

    @property
    def adc_rate(self):
        """The current sampling rate of the ADC clock in samples/s. Read-only.

        Note that the sampling rate may be less than the clock frequency if
        the downsampling factor is greater than 1.

        :Getter: Return the current sampling rate in MS/s (float)
        """
        return self._adcSampleRate()

    @property
    def adc_locked(self):
        """The current status of the ADC DCM. Read-only.

        To try re-locking the ADC, see reset_adc().

        :Getter: Return whether the ADC DCM is locked (True or False)
        """
        return self._get_adcclk_locked()

    @property
    def freq_ctr(self):
        """The current frequency at the frequency counter in MHz. Read-only.

        The frequency counter can be used to check the speed of the CLKGEN
        output or the EXTCLK input. This value shows the current frequency
        reading.

        :Getter: Return the current frequency in MHz (float)
        """
        return self._get_extfrequency()

    @property
    def freq_ctr_src(self):
        """The current input to the frequency counter.

        There are two possible inputs to the frequency counter:
        - "clkgen": The CLKGEN DCM output
        - "extclk": The external input clock signal

        :Getter: Return the frequency counter input (one of the above strings)

        :Setter: Set the frequency counter source

        Raises:
           ValueError: if source is not "clkgen" or "extclk"
        """
        src = self._get_freqcounter_src()
        if src == 1:
            return "clkgen"
        elif src == 0:
            return "extclk"
        else:
            raise IOError("Invalid clock source reported by hardware: %d"%src)

    @freq_ctr_src.setter
    def freq_ctr_src(self, src):
        if src == "clkgen":
            s = 1
        elif src == "extclk":
            s = 0
        else:
            raise ValueError("Invalid clock source for frequency counter. Valid values: 'clkgen', 'extclk'.", src)
        self._set_freqcounter_src(s)

    @property
    def clkgen_src(self):
        """The input source for the CLKGEN DCM.

        This DCM can receive input from one of two places:
        - "extclk": The external clock input
        - "system" or "internal": The system clock (96 MHz)

        :Getter: Return the current CLKGEN input (either "extclk" or "system")

        :Setter: Change the CLKGEN source and reset all the DCMs.

        Raises:
           ValueError: if source is not one of three strings above
        """
        return self._get_clkgen_src()

    @clkgen_src.setter
    def clkgen_src(self, src):
        if src == "extclk":
            self._set_clkgen_src("extclk")
        elif src == "system" or src == "internal":
            self._set_clkgen_src("system")
        else:
            raise ValueError("Invalid setting for CLKGEN source (valid values: 'system', 'extclk')")

        self.reset_dcms()

    @property
    def extclk_freq(self):
        """The input frequency from the EXTCLK source in Hz.

        This value is used to help calculate the correct CLKGEN settings to
        obtain a desired output frequency when using EXTCLK as CLKGEN input.
        It is not a frequency counter - it is only helpful if the EXTCLK
        frequency is already known.

        :Getter: Return the last set EXTCLK frequency in MHz (int)

        :Setter: Update the EXTCLK frequency
        """
        return int(self._get_extclk_freq())

    @extclk_freq.setter
    def extclk_freq(self, freq):
        self._set_extclk_freq(freq)

    @property
    def clkgen_freq(self):
        """The CLKGEN output frequency in Hz.

        The CLKGEN module takes the input source and multiplies/divides it to
        get a faster or slower clock as desired.

        :Getter:
            Return the current calculated CLKGEN output frequency in Hz
            (float). Note that this is the theoretical frequency - use the
            freq counter to determine the actual output.

        :Setter:
            Attempt to set a new CLKGEN frequency in Hz. When this value is
            set, all possible DCM multiply/divide settings are tested to find
            which is closest to the desired output speed. If EXTCLK is the
            CLKGEN source, the EXTCLK frequency must be properly set for this
            to work. Also, both DCMs are reset.
        """
        return self._get_clkgen_freq()

    @clkgen_freq.setter
    def clkgen_freq(self, freq):
        self._autoMulDiv(freq)
        self.reset_dcms()

    @property
    def clkgen_locked(self):
        """The current status of the CLKGEN DCM. Read-only.

        :Getter: Return whether the CLKGEN DCM is locked (True or False)
        """
        return self._getClkgenLocked()

    def _set_freqcounter_src(self, src):
        result = self.oa.sendMessage(CODE_READ, ADDR_ADVCLK, maxResp=4)
        result[3] &= ~0x08
        result[3] |= src << 3
        #print "%x"%result[3]
        self.oa.sendMessage(CODE_WRITE, ADDR_ADVCLK, result, readMask=self._readMask)

    def _get_freqcounter_src(self):
        if self.oa is None:
            return 0
        result = self.oa.sendMessage(CODE_READ, ADDR_ADVCLK, maxResp=4)
        return (result[3] & 0x08) >> 3

    #def _getClkgenStr(self):
    #    return str(self.getClkgen()) + " Hz"

    def _get_clkgen_freq(self):
        if self._get_clkgen_src() == "extclk":
            inpfreq = self._get_extclk_freq()
        else:
            inpfreq = self._hwinfo.sysFrequency()
        return (inpfreq * self._getClkgenMul()) / self._getClkgenDiv()

    def _autoMulDiv(self, freq):
        if self._get_clkgen_src() == "extclk":
            inpfreq = self._get_extclk_freq()
        else:
            inpfreq = self._hwinfo.sysFrequency()
        sets = self._calculateClkGenMulDiv(freq, inpfreq)
        self._setClkgenMulWrapper(sets[0])
        self._setClkgenDivWrapper(sets[1])
        self._reset_dcms(False, True)

    def _calculateClkGenMulDiv(self, freq, inpfreq=30E6):
        """Calculate Multiply & Divide settings based on input frequency"""

        #Max setting for divide is 60 (see datasheet)
        #Multiply is 2-256

        lowerror = 1E99
        best = (0, 0)

        # From datasheet, if input freq is < 52MHz limit max divide
        if inpfreq < 52E6:
            maxdiv = int(inpfreq / 0.5E6)
        else:
            maxdiv = 256

        for mul in range(2, 257):
            for div in range(1, maxdiv):

                err = abs(freq - ((inpfreq * mul) / div))
                if err < lowerror:
                    lowerror = err
                    best = (mul, div)

        return best

    @property
    def clkgen_mul(self):
        """The multiplier in the CLKGEN DCM.

        This multiplier must be in the range [2, 256].

        :Getter: Return the current CLKGEN multiplier (integer)

        :Setter: Set a new CLKGEN multiplier.
        """
        return self._getClkgenMul()

    def _getClkgenMul(self):
        timeout = 2
        while timeout > 0:
            result = self.oa.sendMessage(CODE_READ, ADDR_ADVCLK, maxResp=4)
            val = result[1]
            if val == 0:
                val = 1  # Fix incorrect initialization on FPGA
                self._setClkgenMul(2)
            val += 1

            if (result[3] & 0x02):
                return val

            self._clkgenLoad()

            timeout -= 1

        # raise IOError("clkgen never loaded value?")
        return 0

    @clkgen_mul.setter
    def clkgen_mul(self, mul):
        self._setClkgenMulWrapper(mul)

    def _setClkgenMulWrapper(self, mul):
        # TODO: raise ValueError?
        if mul < 2:
            mul = 2
        self._setClkgenMul(mul)

    def _setClkgenMul(self, mul):
        result = self.oa.sendMessage(CODE_READ, ADDR_ADVCLK, maxResp=4)
        mul -= 1
        result[1] = mul
        result[3] |= 0x01
        self.oa.sendMessage(CODE_WRITE, ADDR_ADVCLK, result, readMask=self._readMask)
        result[3] &= ~(0x01)
        self.oa.sendMessage(CODE_WRITE, ADDR_ADVCLK, result, readMask=self._readMask)

    @property
    def clkgen_div(self):
        """The divider in the CLKGEN DCM.

        This divider must be in the range [1, 256].

        :Getter: Return the current CLKGEN divider (integer)

        :Setter: Set a new CLKGEN divider.
        """
        return self._getClkgenDiv()

    def _getClkgenDiv(self):
        if self.oa is None:
            return 2
        timeout = 2
        while timeout > 0:
            result = self.oa.sendMessage(CODE_READ, ADDR_ADVCLK, maxResp=4)
            val = result[2]
            val += 1

            if (result[3] & 0x02):
                # Done loading value yet
                return val

            self._clkgenLoad()

            timeout -= 1

        logging.error("CLKGEN Failed to load divider value. Most likely clock input to CLKGEN is stopped, check CLKGEN"
                      " source settings. CLKGEN clock results are currently invalid.")
        return 1

    @clkgen_div.setter
    def clkgen_div(self, div):
        self._setClkgenDivWrapper(div)

    def _setClkgenDivWrapper(self, div):
        # TODO: valueerror
        if div < 1:
            div = 1

        result = self.oa.sendMessage(CODE_READ, ADDR_ADVCLK, maxResp=4)
        div -= 1
        result[2] = div
        result[3] |= 0x01
        self.oa.sendMessage(CODE_WRITE, ADDR_ADVCLK, result, readMask=self._readMask)
        result[3] &= ~(0x01)
        self.oa.sendMessage(CODE_WRITE, ADDR_ADVCLK, result, readMask=self._readMask)

    def reset_adc(self):
        """Reset the ADC DCM.

        After changing frequencies, the ADC DCM may become unlocked from its
        input signal. This function resets the DCM to re-lock it.

        If the DCM is still unlocked after calling this function, the clock
        may be too fast for the ADC.
        """
        self._reset_dcms(True, False)

    resetAdc = util.camel_case_deprecated(reset_adc)

    def reset_clkgen(self):
        """Reset the CLKGEN DCM.

        After changing frequencies or input sources, the CLKGEN DCM may not
        be locked. This function resets the DCM to re-lock it.

        If the DCM is still unlocked after calling this function, the clock
        may be too fast for the CLKGEN module.
        """
        self._reset_dcms(False, True)

    resetClkgen = util.camel_case_deprecated(reset_clkgen)

    def reset_dcms(self):
        """Reset the CLKGEN DCM, then the ADC DCM.

        This order is necessary because the ADC may depend on having a locked
        clock from the CLKGEN output.
        """
        self.reset_clkgen()
        self.reset_adc()

    resetDcms = util.camel_case_deprecated(reset_dcms)

    def _clkgenLoad(self):
        result = self.oa.sendMessage(CODE_READ, ADDR_ADVCLK, maxResp=4)
        result[3] |= 0x01
        self.oa.sendMessage(CODE_WRITE, ADDR_ADVCLK, result, readMask=self._readMask)
        result[3] &= ~(0x01)
        self.oa.sendMessage(CODE_WRITE, ADDR_ADVCLK, result, readMask=self._readMask)

    def _getAdcSource(self):
        if self.oa is None:
            return ("dcm", 1, "extclk")

        result = self.oa.sendMessage(CODE_READ, ADDR_ADVCLK, maxResp=4)
        result[0] = result[0] & 0x07

        if result[0] & 0x04:
            dcminput = "extclk"
        else:
            dcminput = "clkgen"

        if result[0] & 0x02:
            dcmout = 1
        else:
            dcmout = 4

        if result[0] & 0x01:
            source = "extclk"
        else:
            source = "dcm"

        return (source, dcmout, dcminput)

    def _setAdcSource(self, source="dcm", dcmout=4, dcminput="clkgen"):

        #Deal with being passed tuple with all 3 arguments
        if isinstance(source, (list, tuple)):
            dcminput = source[2]
            dcmout = source[1]
            source=source[0]

        result = self.oa.sendMessage(CODE_READ, ADDR_ADVCLK, maxResp=4)

        result[0] = result[0] & ~0x07

        if dcminput == "clkgen":
            pass
        elif dcminput == "extclk":
            result[0] = result[0] | 0x04
        else:
            raise ValueError("dcminput must be 'clkgen' or 'extclk'")

        if dcmout == 4:
            pass
        elif dcmout == 1:
            result[0] = result[0] | 0x02
        else:
            raise ValueError("dcmout must be 1 or 4")

        if source == "dcm":
            pass
        elif source == "extclk":
            result[0] = result[0] | 0x01
        else:
            raise ValueError("source must be 'dcm' or 'extclk'")

        self.oa.sendMessage(CODE_WRITE, ADDR_ADVCLK, result, readMask=self._readMask)

    def _set_clkgen_src(self, source="system"):
        result = self.oa.sendMessage(CODE_READ, ADDR_ADVCLK, maxResp=4)

        result[0] = result[0] & ~0x08

        if source == "system":
            pass
        elif source == "extclk":
            result[0] = result[0] | 0x08
        else:
            raise ValueError("source must be 'system' or 'extclk'")

        self.oa.sendMessage(CODE_WRITE, ADDR_ADVCLK, result, readMask=self._readMask)

    def _get_clkgen_src(self):
        if self.oa is not None and self.oa.sendMessage(CODE_READ, ADDR_ADVCLK, maxResp=4)[0] & 0x08:
            return "extclk"
        else:
            return "system"

    def _set_extclk_freq(self, freq):
        self._freqExt = freq

    def _get_extclk_freq(self):
        return self._freqExt

    def _set_phase(self, phase):
        '''Set the phase adjust, range -255 to 255'''
        try:
            phase_int = int(phase)
        except ValueError:
            raise TypeError("Can't convert %s to int" % phase)

        if phase_int < -255 or phase_int > 255:
            raise ValueError("Phase %d is outside range [-255, 255]" % phase_int)

        LSB = phase_int & 0x00FF
        MSB = (phase_int & 0x0100) >> 8

        cmd = bytearray(2)
        cmd[0] = LSB
        cmd[1] = MSB | 0x02
        self.oa.sendMessage(CODE_WRITE, ADDR_PHASE, cmd, False)

    def _get_phase(self):
        if self.oa is None:
            return 0
        result = self.oa.sendMessage(CODE_READ, ADDR_PHASE, maxResp=2)

        #Current bitstream doesn't set this bit ever?
        #phase_valid = (result[1] & 0x02)
        #Temp fix - set as true always
        phase_valid = True

        if phase_valid:
            LSB = result[0]
            MSB = result[1] & 0x01

            phase = LSB | (MSB << 8)

            #Sign Extend
            phase = SIGNEXT(phase, 9)

            return phase
        else:
            logging.warning("No phase shift loaded")
            return 0

    def _get_adcclk_locked(self):
        result = self._DCMStatus()
        return result[0]

    def _getClkgenLocked(self):
        result = self._DCMStatus()
        return result[1]

    def _DCMStatus(self):
        if self.oa is None:
            return (False, False)

        result = self.oa.sendMessage(CODE_READ, ADDR_ADVCLK, maxResp=4)
        if (result[0] & 0x80) == 0:
            logging.error("ADVCLK register not present. Version mismatch")
            return (False, False)

        if (result[0] & 0x40) == 0:
            dcmADCLocked = False
        else:
            dcmADCLocked = True

        if (result[0] & 0x20) == 0:
            dcmCLKGENLocked = False
        else:
            dcmCLKGENLocked = True

        #if (result[3] & 0x02):
        #    print "CLKGEN Programming Done"

        return (dcmADCLocked, dcmCLKGENLocked)

    def _reset_dcms(self, resetAdc=True, resetClkgen=True):
        result = self.oa.sendMessage(CODE_READ, ADDR_ADVCLK, maxResp=4)

        #Set reset high on requested blocks only
        if resetAdc:
            result[0] = result[0] | 0x10
            #NB: High-Level system will call 'get' to re-read ADC phase

        if resetClkgen:
            result[3] = result[3] | 0x04


        self.oa.sendMessage(CODE_WRITE, ADDR_ADVCLK, result, Validate=False)

        #Set reset low
        result[0] = result[0] & ~(0x10)
        result[3] = result[3] & ~(0x04)
        self.oa.sendMessage(CODE_WRITE, ADDR_ADVCLK, result, Validate=False)

        #Load clkgen if required
        if resetClkgen:
            self._clkgenLoad()

    def _get_extfrequency(self):
        """Return frequency of clock measured on EXTCLOCK pin in Hz"""
        if self.oa is None:
            return 0
        freq = 0x00000000

        #Get sample frequency
        samplefreq = float(self.oa.hwInfo.sysFrequency()) / float(pow(2,23))

        temp = self.oa.sendMessage(CODE_READ, ADDR_FREQ, maxResp=4)
        freq |= temp[0] << 0
        freq |= temp[1] << 8
        freq |= temp[2] << 16
        freq |= temp[3] << 24

        measured = freq * samplefreq
        return int(measured)

    def _getAdcFrequency(self):
        """Return the external frequency measured on 'CLOCK' pin. Returned value
           is in Hz"""
        if self.oa is None:
            return 0
        freq = 0x00000000

        #Get sample frequency
        samplefreq = float(self.oa.hwInfo.sysFrequency()) / float(pow(2,23))

        temp = self.oa.sendMessage(CODE_READ, ADDR_ADCFREQ, maxResp=4)
        freq = freq | (temp[0] << 0)
        freq = freq | (temp[1] << 8)
        freq = freq | (temp[2] << 16)
        freq = freq | (temp[3] << 24)

        measured = freq * samplefreq

        return int(measured)

    def _adcSampleRate(self):
        """Return the sample rate, takes account of decimation factor (if set)"""
        return self._getAdcFrequency() / self.oa.decimate()


class OpenADCInterface(object):

    def __init__(self, serial_instance):
        self.serial = serial_instance
        self.offset = 0.5
        self.ddrMode = False
        self.sysFreq = 0
        self._streammode = False
        self._sbuf = []
        self.settings()
        self._support_decimate = True
        self._nosampletimeout = 100
        self._timeout = 2
        self.presamples_desired = 0
        self.presamples_actual = 0
        self.presampleTempMargin = 24
        self._stream_mode = False
        self._support_get_duration = True

        # Send clearing function if using streaming mode
        if hasattr(self.serial, "stream") and self.serial.stream == False:
            pass
        else:
            nullmessage = bytearray([0] * 20)
            self.serial.write(str(nullmessage))

        self.setReset(True)
        self.setReset(False)

    def setStreamMode(self, stream):
        self._streammode = stream
        self.updateStreamBuffer()

    def setTimeout(self, timeout):
        self._timeout = timeout

    def timeout(self):
        return self._timeout

    def testAndTime(self):
        totalbytes = 0
        totalerror = 0

        for n in range(10):
            # Generate 500 bytes
            testData = bytearray(list(range(250)) + list(range(250))) #bytearray(random.randint(0,255) for r in xrange(500))
            self.sendMessage(CODE_WRITE, ADDR_MULTIECHO, testData, False)
            testDataEcho = self.sendMessage(CODE_READ, ADDR_MULTIECHO, None, False, 502)
            testDataEcho = testDataEcho[2:]

            #Compare
            totalerror = totalerror + len([(i,j) for i,j in zip(testData,testDataEcho) if i!=j])
            totalbytes = totalbytes + len(testData)

            logging.error('%d errors in %d' % (totalerror, totalbytes))

    def sendMessage(self, mode, address, payload=None, Validate=True, maxResp=None, readMask=None):
        """Send a message out the serial port"""

        if payload is None:
            payload = []

        #Get length
        length = len(payload)

        if ((mode == CODE_WRITE) and (length < 1)) or ((mode == CODE_READ) and (length != 0)):
            logging.warning('Invalid payload for mode')
            return None

        if mode == CODE_READ:
            self.flushInput()

        #Flip payload around
        pba = bytearray(payload)

        #Check if stream or newaechip mode expected
        if hasattr(self.serial, "stream") and self.serial.stream is False:
            #The serial interface is actually special USB Chip
            if mode == CODE_READ:
                if maxResp:
                    datalen = maxResp
                elif ADDR_ADCDATA == address:
                    datalen = 65000
                else:
                    datalen = 1

                data = bytearray(self.serial.cmdReadMem(address, datalen))
                return data

            else:
                # Write output to memory
                self.serial.cmdWriteMem(address, pba)

                # Check write was successful if validation requested
                if Validate:
                    check =  bytearray(self.serial.cmdReadMem(address, len(pba)))

                    if readMask:
                        try:
                            for i, m in enumerate(readMask):
                                check[i] = check[i] & m
                                pba[i] = pba[i] & m
                        except IndexError:
                            pass

                    if check != pba:
                        errmsg = "For address 0x%02x=%d" % (address, address)
                        errmsg += "  Sent data: "
                        for c in pba: errmsg += "%02x" % c
                        errmsg += " Read data: "
                        if check:
                            for c in check: errmsg += "%02x" % c
                        else:
                            errmsg += "<Timeout>"

                        logging.error(errmsg)
        else:
            # ## Setup Message
            message = bytearray([])

            # Message type
            message.append(mode | address)

            # Length
            lenpayload = len(pba)
            message.append(lenpayload & 0xff)
            message.append((lenpayload >> 8) & 0xff)

            # append payload
            message = message + pba

            # ## Send out serial port
            self.serial.write(str(message))

            # for b in message: print "%02x "%b,
            # print ""

            # ## Wait Response (if requested)
            if mode == CODE_READ:
                if maxResp:
                    datalen = maxResp
                elif ADDR_ADCDATA == address:
                    datalen = 65000
                else:
                    datalen = 1

                result = self.serial.read(datalen)

                # Check for timeout, if so abort
                if len(result) < 1:
                    self.flushInput()
                    logging.warning('Timeout in read: %d (address: 0x%02x)' % (len(result), address))
                    return None

                rb = bytearray(result)

                return rb
            else:
                if Validate:
                    check = self.sendMessage(CODE_READ, address, maxResp=len(pba))

                    if readMask:
                        try:
                            for i, m in enumerate(readMask):
                                check[i] = check[i] & m
                                pba[i] = pba[i] & m
                        except IndexError:
                            pass

                    if check != pba:
                        errmsg = "For address 0x%02x=%d" % (address, address)
                        errmsg += "  Sent data: "
                        for c in pba: errmsg += "%02x" % c
                        errmsg += " Read data: "
                        if check:
                            for c in check: errmsg += "%02x" % c
                        else:
                            errmsg += "<Timeout>"

                        logging.error(errmsg)

### Generic
    def setSettings(self, state, validate=True):
        cmd = bytearray(1)
        cmd[0] = state
        self.sendMessage(CODE_WRITE, ADDR_SETTINGS, cmd, Validate=validate)

    def settings(self):
        sets = self.sendMessage(CODE_READ, ADDR_SETTINGS)
        if sets:
            return sets[0]
        else:
            return 0

    def setReset(self, value):
        if value:
            self.setSettings(self.settings() | SETTINGS_RESET, validate=False)
            #TODO: Hack to adjust the hwMaxSamples since the number should be smaller than what is being returned
            self.hwMaxSamples = self.numSamples() - 45
            self.setNumSamples(self.hwMaxSamples)
        else:
            self.setSettings(self.settings() & ~SETTINGS_RESET)

    def triggerNow(self):
        initial = self.settings()
        self.setSettings(initial | SETTINGS_TRIG_NOW)
        time.sleep(0.001)
        self.setSettings(initial & ~SETTINGS_TRIG_NOW)

    def getStatus(self):
        result = self.sendMessage(CODE_READ, ADDR_STATUS)

        if len(result) == 1:
            return result[0]
        else:
            return None

    def setNumSamples(self, samples):
        cmd = bytearray(4)
        cmd[0] = ((samples >> 0) & 0xFF)
        cmd[1] = ((samples >> 8) & 0xFF)
        cmd[2] = ((samples >> 16) & 0xFF)
        cmd[3] = ((samples >> 24) & 0xFF)
        self.sendMessage(CODE_WRITE, ADDR_SAMPLES, cmd)

        self.updateStreamBuffer(samples)

    def updateStreamBuffer(self, samples=None):
        if samples is not None:
            self._stream_len = samples

        bufsizebytes = 0
        if self._streammode:
            nae = self.serial
            #Save the number we will return
            bufsizebytes, self._stream_len_act = nae.cmdReadStream_bufferSize(self._stream_len)

        #Generate the buffer to save buffer
        self._sbuf = array.array('B', [0]) * bufsizebytes


    def setDecimate(self, decsamples):
        cmd = bytearray(2)
        if decsamples <= 0:
            raise ValueError("Decsamples is <= 0 (%d), makes no sense" % decsamples)
        decsamples -= 1
        cmd[0] = ((decsamples >> 0) & 0xFF)
        cmd[1] = ((decsamples >> 8) & 0xFF)
        self.sendMessage(CODE_WRITE, ADDR_DECIMATE, cmd)


    def decimate(self):
        if self._support_decimate:
            decnum = 0x00000000
            temp = self.sendMessage(CODE_READ, ADDR_DECIMATE, maxResp=2)
            #If we don't support decimate just return 1 in the future to avoid
            if temp:
                decnum |= temp[0] << 0
                decnum |= temp[1] << 8
                decnum += 1
            else:
                self._support_decimate = False
                decnum = 1
        else:
            decnum = 1
        return decnum

    def numSamples(self):
        """Return the number of samples captured in one go. Returns max after resetting the hardware"""
        samples = 0x00000000
        temp = self.sendMessage(CODE_READ, ADDR_SAMPLES, maxResp=4)
        samples |= temp[0] << 0
        samples |= temp[1] << 8
        samples |= temp[2] << 16
        samples |= temp[3] << 24
        return samples

    def getBytesInFifo(self):
        samples = 0
        temp = self.sendMessage(CODE_READ, ADDR_BYTESTORX, maxResp=4)

        samples |= temp[0] << 0
        samples |= temp[1] << 8
        samples |= temp[2] << 16
        samples |= temp[3] << 24
        return samples

    def flushInput(self):
        try:
            self.serial.flushInput()
        except AttributeError:
            return

    def devicePresent(self):

        #Re-init these settings:
        self._support_decimate = True

        #Send "ping" message, wait for pong
        msgin = bytearray([])
        msgin.append(0xAC)

        self.flushInput()

        #Reset... will automatically clear by the time we are done
        self.setReset(True)
        self.flushInput()

        #Send ping
        self.sendMessage(CODE_WRITE, ADDR_ECHO, msgin)

        #Pong?
        msgout = self.sendMessage(CODE_READ, ADDR_ECHO)

        if (msgout != msgin):
            return False

        #Init stuff
        state = self.getStatus()

        if state & STATUS_DDRMODE_MASK:
            self.ddrMode = True
        else:
            self.ddrMode = False

        return True

    # def setDDRAddress(self, addr):
    #     cmd = bytearray(1)
    #     cmd[0] = ((addr >> 0) & 0xFF)
    #     self.sendMessage(CODE_WRITE, ADDR_DDR1, cmd)
    #     cmd[0] = ((addr >> 8) & 0xFF)
    #     self.sendMessage(CODE_WRITE, ADDR_DDR2, cmd)
    #     cmd[0] = ((addr >> 16) & 0xFF)
    #     self.sendMessage(CODE_WRITE, ADDR_DDR3, cmd)
    #     cmd[0] = ((addr >> 24) & 0xFF)
    #     self.sendMessage(CODE_WRITE, ADDR_DDR4, cmd)
    #
    # def getDDRAddress(self):
    #     addr = 0x00000000
    #     temp = self.sendMessage(CODE_READ, ADDR_DDR1)
    #     addr = addr | (temp[0] << 0)
    #     temp = self.sendMessage(CODE_READ, ADDR_DDR2)
    #     addr = addr | (temp[0] << 8)
    #     temp = self.sendMessage(CODE_READ, ADDR_DDR3)
    #     addr = addr | (temp[0] << 16)
    #     temp = self.sendMessage(CODE_READ, ADDR_DDR4)
    #     addr = addr | (temp[0] << 24)
    #     return addr

    def arm(self, enable=True):
        if enable:
            #Must arm first
            self.setSettings(self.settings() | SETTINGS_ARM)
        else:
            self.setSettings(self.settings() & ~SETTINGS_ARM)

    def startCaptureThread(self):
        # Then init the stream mode stuff
        if self._streammode:
            # Stream mode adds 500mS of extra timeout on USB traffic itself...
            self.serial.initStreamModeCapture(self._stream_len, self._sbuf, timeout_ms=int(self._timeout * 1000) + 500)

    def capture(self, offset=None):
        timeout = False
        sleeptime = 0
        if offset:
            sleeptime = 4*offset/100000 #rougly 4ms per 100k offset
        if self._streammode:

            # Wait for a trigger, letting the UI run when it can
            starttime = datetime.datetime.now()
            while self.serial.cmdReadStream_isDone() == False:
                # Wait for a moment before re-running the loop
                #time.sleep(0.05)
                diff = datetime.datetime.now() - starttime

                # If we've timed out, don't wait any longer for a trigger
                if (diff.total_seconds() > self._timeout):
                    logging.warning('Timeout in OpenADC capture(), trigger FORCED')
                    timeout = True
                    self.triggerNow()
                    break


            self._stream_rx_bytes, stream_timeout = self.serial.cmdReadStream()
            timeout |= stream_timeout
            #Check the status now
            bytes_left, overflow_bytes_left, unknown_overflow = self.serial.cmdReadStream_getStatus()
            logging.debug("Streaming done, results: rx_bytes = %d, bytes_left = %d, overflow_bytes_left = %d"%(self._stream_rx_bytes, bytes_left, overflow_bytes_left))
            self.arm(False)

            if stream_timeout:
                if self._stream_rx_bytes == 0: # == (self._stream_len - 3072):
                    logging.warning("Streaming mode OVERFLOW occured as trigger too fast - Adjust offset upward (suggest = 200 000)")
                else:
                    logging.warning("Streaming mode OVERFLOW occured during capture - ADC sample clock probably too fast for stream mode (keep ADC Freq < 10 MHz)")
                timeout = True
        else:
            status = self.getStatus()
            starttime = datetime.datetime.now()

            # Wait for a trigger, letting the UI run when it can
            while ((status & STATUS_ARM_MASK) == STATUS_ARM_MASK) | ((status & STATUS_FIFO_MASK) == 0):
                status = self.getStatus()

                # Wait for a moment before re-running the loop
                #time.sleep(0.01) ## <-- This causes the capture slowdown
                util.better_delay(sleeptime) ## faster sleep method
                diff = datetime.datetime.now() - starttime

                # If we've timed out, don't wait any longer for a trigger
                if (diff.total_seconds() > self._timeout):
                    logging.warning('Timeout in OpenADC capture(), trigger FORCED')
                    timeout = True
                    self.triggerNow()

                # Give the UI a chance to update (does nothing if not using UI)
                # util.updateUI()

            self.arm(False)

            # If using large offsets, system doesn't know we are delaying api
            nosampletimeout = self._nosampletimeout * 10
            while (self.getBytesInFifo() == 0) and nosampletimeout:
                logging.debug("Bytes in Fifo: {}".format(self.getBytesInFifo()))
                time.sleep(0.005)
                nosampletimeout -= 1

            if nosampletimeout == 0:
                logging.warning('No samples received. Either very long offset, or no ADC clock (try "Reset ADC DCM"). '
                                'If you need such a long offset, increase "scope.qtadc.sc._nosampletimeout" limit.')
                timeout = True

        return timeout

    def flush(self):
        # Flush output FIFO
        self.sendMessage(CODE_READ, ADDR_ADCDATA, None, False, None)

    def readData(self, NumberPoints=None, progressDialog=None):
        logging.debug("Reading data fromm OpenADC...")
        if self._streammode:
            # Process data
            bsize = self.serial.cmdReadStream_size_of_fpgablock()
            num_bytes, num_samples = self.serial.cmdReadStream_bufferSize(self._stream_len)

            # Remove sync bytes from trace
            data = np.zeros(num_bytes, dtype=np.uint8)
            data[0] = self._sbuf[0]
            dbuf2_idx = 1
            for i in range(0, self._stream_rx_bytes, bsize):
                if self._sbuf[i] != 0xAC:
                    logging.warning("Stream mode: Expected sync byte (AC) at location %d but got %x" % (i, self._sbuf[i]))
                    break
                s = i + 1
                data[dbuf2_idx: (dbuf2_idx + (bsize - 1))] = self._sbuf[s:(s + (bsize - 1))]

                # Write to next section
                dbuf2_idx += (bsize - 1)

            logging.debug("Stream mode: read %d bytes"%len(data))

            # Turn raw bytes into samples
            datapoints = self.processData(data, 0.0)

            if datapoints is not None and len(datapoints):
                logging.debug("Stream mode: done, %d samples processed"%len(datapoints))
            else:
                logging.warning("Stream mode: done, no samples resulted from processing")
                datapoints = []

            if len(datapoints) > NumberPoints:
                datapoints = datapoints[0:NumberPoints]

            return datapoints

        else:
            datapoints = []

            if NumberPoints == None:
                NumberPoints = 0x1000

            if self.ddrMode:
                # We were passed number of samples to read. DDR interface
                # reads 3 points per 4 bytes, and reads in blocks of
                # 256 bytes (e.g.: 192 samples)
                NumberPackages = NumberPoints / 192

                # If user requests we send extra then scale back afterwards
                if (NumberPoints % 192) > 0:
                    NumberPackages = NumberPackages + 1

                start = 0
                self.setDDRAddress(0)


                BytesPerPackage = 257

                if progressDialog:
                    progressDialog.setMinimum(0)
                    progressDialog.setMaximum(NumberPackages)
            else:
                # FIFO takes 3 samples at a time... todo figure this out
                NumberPackages = 1

                # We get 3 samples in each word returned (word = 4 bytes)
                # So need to convert samples requested to words, rounding
                # up if we request an incomplete number
                nwords = NumberPoints / 3
                if NumberPoints % 3:
                    nwords = nwords + 1

                # Return 4x as many bytes as words, +1 for sync byte
                BytesPerPackage = nwords * 4 + 1

            for status in range(0, NumberPackages):
                # Address of DDR is auto-incremented following a read command
                # so no need to write new address

                # print "Address=%x"%self.getDDRAddress()

                # print "bytes = %d"%self.getBytesInFifo()

                bytesToRead = self.getBytesInFifo()

                # print bytesToRead

                if bytesToRead == 0:
                    bytesToRead = BytesPerPackage

                #If bytesToRead is huge, we only read what is needed
                #Bytes get packed 3 samples / 4 bytes
                #Add some extra in case needed
                hypBytes = (NumberPoints * 4)/3 + 256

                bytesToRead = min(hypBytes, bytesToRead)

                # +1 for sync byte
                data = self.sendMessage(CODE_READ, ADDR_ADCDATA, None, False, bytesToRead + 1)  # BytesPerPackage)

                # for p in data:
                #       print "%x "%p,

                if data is not None:
                    data = np.array(data)
                    datapoints = self.processData(data, 0.0)

                if progressDialog:
                    progressDialog.setValue(status)

                    if progressDialog.wasCanceled():
                        break

            # for point in datapoints:
            #       print "%3x"%(int((point+0.5)*1024))

            if len(datapoints) > NumberPoints:
                datapoints = datapoints[0:NumberPoints]

            # if len(datapoints) < NumberPoints:
            # print len(datapoints),
            # print NumberPoints

            return datapoints

    def processData(self, data, pad=float('NaN'), debug=False):
        if data[0] != 0xAC:
            logging.warning('Unexpected sync byte in processData(): 0x%x' % data[0])
            return None

        orig_data = copy.copy(data)
        if debug:
            fpData = []
            # Slow, verbose processing method
            # Useful for fixing issues in ADC read
            for i in range(1, len(data) - 3, 4):
                # Convert
                temppt = (data[i + 3] << 0) | (data[i + 2] << 8) | (data[i + 1] << 16) | (data[i + 0] << 24)

                # print("%2x "%data[i])

                # print "%x %x %x %x"%(data[i +0], data[i +1], data[i +2], data[i +3]);
                # print "%x"%temppt

                intpt1 = temppt & 0x3FF
                intpt2 = (temppt >> 10) & 0x3FF
                intpt3 = (temppt >> 20) & 0x3FF

                # print "%x %x %x" % (intpt1, intpt2, intpt3)

                if trigfound == False:
                    mergpt = temppt >> 30
                    if (mergpt != 3):
                        trigfound = True
                        trigsamp = trigsamp + mergpt
                        # print "Trigger found at %d"%trigsamp
                    else:
                        trigsamp += 3

                # input validation test: uncomment following and use
                # ramp input on FPGA
                ##if (intpt != lastpt + 1) and (lastpt != 0x3ff):
                ##    print "intpt: %x lstpt %x\n"%(intpt, lastpt)
                ##lastpt = intpt;

                # print "%x %x %x"%(intpt1, intpt2, intpt3)

                fpData.append(float(intpt1) / 1024.0 - self.offset)
                fpData.append(float(intpt2) / 1024.0 - self.offset)
                fpData.append(float(intpt3) / 1024.0 - self.offset)
        else:
            # Fast, efficient NumPy implementation

            # Figure out how many bytes we're going to process
            # Cut off some bytes at the end: we need the length to be a multiple of 4, and we probably have extra data
            numbytes = len(data) - 1
            extralen = (len(data) - 1) % 4

            # Copy the data into a NumPy array. For long traces this is the longest part
            data = data[1:1+numbytes-extralen]

            # Split data into groups of 4 bytes and combine into words
            data = np.reshape(data, (-1, 4))
            data = np.left_shift(data, [24, 16, 8, 0])
            data = np.sum(data, 1)

            # Split words into samples and trigger bytes
            data = np.right_shift(np.reshape(data, (-1, 1)), [0, 10, 20, 30]) & 0x3FF
            fpData = np.reshape(data[:, [0, 1, 2]], (-1))
            trigger = data[:, 3] % 4
            fpData = fpData / 1024.0 - self.offset
            logging.debug("Trigger_data: {} len={}".format(trigger, len(trigger)))

            # Search for the trigger signal
            trigfound = False
            trigsamp = 0
            for t in trigger:
                if(t != 3):
                    trigfound = True
                    trigsamp = trigsamp + (t & 0x3)
                    logging.debug("Trigger found at %d"%trigsamp)
                    break
                else:
                    trigsamp += 3

        #print len(fpData)

        if trigfound == False:
            logging.warning('Trigger not found in ADC data. No data reported!')
            logging.debug('Trigger not found typically caused by the actual \
            capture starting too late after the trigger event happens')
            logging.debug('Data: {}'.format(orig_data))


        #Ensure that the trigger point matches the requested by padding/chopping
        diff = self.presamples_desired - trigsamp
        if diff > 0:
               #fpData = [pad]*diff + fpData
               fpData = np.append([pad]*diff, fpData)
               logging.warning('Pretrigger not met: Do not use downsampling and pretriggering at same time.')
               logging.debug('Pretrigger not met: can attempt to increase presampleTempMargin(in the code).')
        else:
               fpData = fpData[-diff:]

        logging.debug("Processed data, ended up with %d samples total"%len(fpData))

        return fpData

if __name__ == "__main__":
    import serial

    ser = serial.Serial()
    ser.port     = "com6"
    ser.baudrate = 512000
    ser.timeout  = 1.0

    try:
        ser.open()
    except serial.SerialException as e:
        print("Could not open %s" % ser.name)
        sys.exit()
    except ValueError as s:
        print("Invalid settings for serial port")
        ser.close()
        ser = None
        sys.exit()

    adc = OpenADCInterface(ser)
    adc.devicePresent()

    adc_settings = OpenADCSettings()
    adc_settings.setInterface(adc)

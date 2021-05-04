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

from chipwhisperer.logging import *

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
ADDR_DATA_SOURCE = 27
ADDR_ADC_LOW_RES = 29
ADDR_DRP_ADDR   = 30
ADDR_DRP_DATA   = 31
ADDR_SEGMENTS   = 32
ADDR_SEGMENT_CYCLES = 33
ADDR_FAST_FIFO_READ = 36

ADDR_XADC_DRP_ADDR = 41
ADDR_XADC_DRP_DATA = 42
ADDR_XADC_STAT     = 43
ADDR_FIFO_STAT     = 44
ADDR_NO_CLIP_ERRORS = 45

ADDR_HUSKY_ADC_CTRL = 60
ADDR_HUSKY_VMAG_CTRL = 61

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
                  "Reserved?", "ZedBoard", "Papilio Pro", "SAKURA-G", "ChipWhisperer-Lite", "ChipWhisperer-CW1200","ChipWhisperer-Husky"]

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

    def is_cwhusky(self):
        if self.vers is None:
            self.versions()
        if self.vers[1] == 10:
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
        temp = self.oa.sendMessage(CODE_READ, ADDR_SYSFREQ, maxResp=4)
        freq = int.from_bytes(temp, byteorder='little')

        self.sysFreq = int(freq)
        return self.sysFreq

    def __del__(self):
        self.oa.hwInfo = None


class XADCSettings(util.DisableNewAttr):
    ''' Husky FPGA XADC temperature and voltage monitoring.
    '''
    _name = 'Husky XADC Setting'

    def __init__(self, oaiface):
        self.oa = oaiface
        self.disable_newattr()

    def _dict_repr(self):
        dict = OrderedDict()
        dict['status'] = self.status
        dict['current temperature [C]'] = self.temp
        dict['maximum temperature [C]'] = self.max_temp
        dict['temperature alarm trigger [C]'] = self.temp_trigger
        dict['temperature reset trigger [C]'] = self.temp_reset
        dict['vccint'] = self.vccint
        dict['vccaux'] = self.vccaux
        dict['vccbram'] = self.vccbram
        return dict

    def __repr__(self):
        return util.dict_to_str(self._dict_repr())

    def __str__(self):
        return self.__repr__()

    def _drp_write(self, addr, data):
        """Write XADC DRP register. UG480 for register definitions.
        Args:
            addr (int): 6-bit address
            data (int): 16-bit write data
        """
        self.oa.sendMessage(CODE_WRITE, ADDR_XADC_DRP_DATA, [data  & 0xff, data >> 8])
        self.oa.sendMessage(CODE_WRITE, ADDR_XADC_DRP_ADDR, [addr + 0x80])

    def _drp_read(self, addr):
        """Read XADC DRP register. UG480 for register definitions.
        Args:
            addr (int): 6-bit address
        Returns:
            A 16-bit integer.
        """
        self.oa.sendMessage(CODE_WRITE, ADDR_XADC_DRP_ADDR, [addr])
        raw = self.oa.sendMessage(CODE_READ, ADDR_XADC_DRP_DATA, maxResp=2)
        return int.from_bytes(raw, byteorder='little')

    @property
    def status(self):
        """Read XADC alarm status bits
        Args: none
        Returns: status string
        """
        raw = self.oa.sendMessage(CODE_READ, ADDR_XADC_STAT, maxResp=1)[0]
        stat = ''
        if raw & 1:  stat += 'Over temperature alarm, '
        if raw & 2:  stat += 'User temperature alarm, '
        if raw & 4:  stat += 'VCCint alarm, '
        if raw & 8:  stat += 'VCCaux alarm, '
        if raw & 16: stat += 'VCCbram alarm, '
        if stat == '':
            stat = 'good'
        return stat

    @property
    def temp(self):
        return self.get_temp(0)

    @property
    def max_temp(self):
        return self.get_temp(32)

    @property
    def temp_trigger(self):
        return self.get_temp(0x50)

    @property
    def temp_reset(self):
        return self.get_temp(0x54)

    @temp_trigger.setter
    def temp_trigger(self, temp):
        return self.set_temp(temp, 0x50)

    @temp_reset.setter
    def temp_reset(self, temp):
        return self.set_temp(temp, 0x54)


    def get_temp(self, addr=0):
        """Read XADC temperature.
        Args: 
            addr (int): DRP address (0: current; 32: max; 36: min)
        Returns:
            Temperature in celcius (float).
        """
        raw = self._drp_read(addr)
        return (raw>>4) * 503.975/4096 - 273.15 # ref: UG480

    def set_temp(self, temp, addr=0):
        """Set XADC temperature thresholds.
        Args: 
            addr (int): DRP address
            temp (float): temperature threshold [celcius]
        Returns:
            Temperature in celcius (float).
        """
        raw = (int((temp + 273.15)*4096/503.975) << 4) & 0xffff
        self.oa.sendMessage(CODE_WRITE, ADDR_XADC_DRP_DATA, list(int.to_bytes(raw, length=2, byteorder='little')))
        self.oa.sendMessage(CODE_WRITE, ADDR_XADC_DRP_ADDR, [0x80 + addr])


    @property
    def vccint(self):
        return self.get_vcc('vccint')

    @property
    def vccaux(self):
        return self.get_vcc('vccaux')

    @property
    def vccbram(self):
        return self.get_vcc('vccbram')


    def get_vcc(self, rail='vccint'):
        """Read XADC vcc.
        Args:
            rail (string): 'vccint', 'vccaux', or 'vccbram'
        Returns:
            voltage (float).
        """
        if rail == 'vccint':
            addr = 1
        elif rail == 'vccaux':
            addr = 2
        elif rail == 'vccbram':
            addr = 6
        else:
            raise ValueError("Invalid rail")
        raw = self._drp_read(addr)
        return (raw>>4)/4096 * 3 # ref: UG480


class ADS4128Settings(util.DisableNewAttr):
    ''' Husky ADS4128 ADC settings. Mostly for testing, not needed in normal use.
    '''
    _name = 'Husky ADS4128 ADC Setting'

    def __init__(self, oaiface):
        self.oa = oaiface
        self.adc_reset()
        self.set_default_settings()
        self.disable_newattr()

    def _dict_repr(self):
        dict = OrderedDict()
        dict['mode'] = self.mode
        return dict

    def __repr__(self):
        return util.dict_to_str(self._dict_repr())

    def __str__(self):
        return self.__repr__()

    def adc_reset(self):
        """Resets the ADC.
        Note this is done by the FPGA - see reg_husky_adc.v
        """
        self.oa.sendMessage(CODE_WRITE, ADDR_HUSKY_ADC_CTRL, [0x41])
        self.oa.sendMessage(CODE_WRITE, ADDR_HUSKY_ADC_CTRL, [0xc1])
        self.oa.sendMessage(CODE_WRITE, ADDR_HUSKY_ADC_CTRL, [0x41])

    def _adc_write(self, address, data):
        """Write ADC configuration register.
        Note this is done by the FPGA - see reg_husky_adc.v
        """
        self.oa.sendMessage(CODE_WRITE, ADDR_HUSKY_ADC_CTRL, [0x41])
        for i in range(8):
            bit = (address >> (7-i)) & 1
            val = (bit << 4) + 1
            self.oa.sendMessage(CODE_WRITE, ADDR_HUSKY_ADC_CTRL, [val])
            val = (bit << 4) + 0
            self.oa.sendMessage(CODE_WRITE, ADDR_HUSKY_ADC_CTRL, [val])
        
        for i in range(8):
            bit = (data >> (7-i)) & 1
            val = (bit << 4) + 1
            self.oa.sendMessage(CODE_WRITE, ADDR_HUSKY_ADC_CTRL, [val])
            val = (bit << 4) + 0
            self.oa.sendMessage(CODE_WRITE, ADDR_HUSKY_ADC_CTRL, [val])
        self.oa.sendMessage(CODE_WRITE, ADDR_HUSKY_ADC_CTRL, [val])

    def _adc_read(self, address):
        """Read ADC configuration register.
        Note this is done by the FPGA - see reg_husky_adc.v
        """
        # first, enable readout:
        self._adc_write(0x0, 0x1)
        self.oa.sendMessage(CODE_WRITE, ADDR_HUSKY_ADC_CTRL, [0x41])
        for i in range(8):
            bit = (address >> (7-i)) & 1
            self.oa.sendMessage(CODE_WRITE, ADDR_HUSKY_ADC_CTRL, [(bit<<4) + 1])
            self.oa.sendMessage(CODE_WRITE, ADDR_HUSKY_ADC_CTRL, [(bit<<4) + 0])
        for i in range(8):
            self.oa.sendMessage(CODE_WRITE, ADDR_HUSKY_ADC_CTRL, [0x01])
            self.oa.sendMessage(CODE_WRITE, ADDR_HUSKY_ADC_CTRL, [0x02])
        self.oa.sendMessage(CODE_WRITE, ADDR_HUSKY_ADC_CTRL, [0x41])
        data =  self.oa.sendMessage(CODE_READ, ADDR_HUSKY_ADC_CTRL, maxResp=1)[0]
        # finished, disable readout:
        self._adc_write(0x0, 0x0)
        return data

    def set_default_settings(self):
        self._adc_write(0x42, 0x00) # enable low-latency mode
        self._adc_write(0x25, 0x00) # disable test patterns
        self._adc_write(0x03, 0x03) # set high performance mode
        self._adc_write(0x4a, 0x01) # set high performance mode
        self._adc_write(0x3d, 0xc0) # set offset binary output
        self.mode_cached = "normal"

    def set_test_settings(self):
        self._adc_write(0x42, 0x08) # disable low-latency mode
        self._adc_write(0x25, 0x04) # set test pattern to ramp
        self._adc_write(0x3d, 0xc0) # set offset binary output
        self.mode_cached = "test ramp"

    @property
    def mode(self):
        """The current mode of the ADC.

        :Getter: Return the current ADC operating mode ("normal" or "test ramp")

        :Setter: Set the operating mode.

        Raises:
            ValueError: if mode not one of "normal" or "test ramp"
        """
        return self.mode_cached

    @mode.setter
    def mode(self, val):
        return self.setMode(val)

    def setMode(self, mode):
        if mode == "normal":
            self.set_default_settings()
            self.oa.sendMessage(CODE_WRITE, ADDR_NO_CLIP_ERRORS, [0])
        elif mode == "test ramp":
            self.set_test_settings()
            self.oa.sendMessage(CODE_WRITE, ADDR_NO_CLIP_ERRORS, [1])
        else:
            raise ValueError("Invalid mode, only 'normal' or 'test ramp' allowed")




class GainSettings(util.DisableNewAttr):
    _name = 'Gain Setting'

    def __init__(self, oaiface):
        self.oa = oaiface
        self.gainlow_cached = False
        self.gain_cached = 0
        self._is_husky = False
        self._vmag_highgain = 0x1f
        self._vmag_lowgain = 0
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
        if self._is_husky:
            if gainmode == "high":
                self.oa.sendMessage(CODE_WRITE, ADDR_HUSKY_VMAG_CTRL, [self._vmag_highgain])
                self.gainlow_cached = False
            elif gainmode == "low":
                self.oa.sendMessage(CODE_WRITE, ADDR_HUSKY_VMAG_CTRL, [self._vmag_lowgain])
                self.gainlow_cached = True
            else:
                raise ValueError("Invalid Gain Mode, only 'low' or 'high' allowed")
        else:
            if gainmode == "high":
                self.oa.setSettings(self.oa.settings() | SETTINGS_GAIN_HIGH)
                self.gainlow_cached = False
            elif gainmode == "low":
                self.oa.setSettings(self.oa.settings() & ~SETTINGS_GAIN_HIGH)
                self.gainlow_cached = True
            else:
                raise ValueError("Invalid Gain Mode, only 'low' or 'high' allowed")

    def getMode(self):
        if self._is_husky:
            if self.oa.sendMessage(CODE_READ, ADDR_HUSKY_VMAG_CTRL)[0] == self._vmag_highgain:
                gain_high = True
            else:
                gain_high = False
        else:
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
        '''Set the Gain range: 0-78 for CW-Lite and CW-Pro; 0-109 for CW-Husky'''
        if self._is_husky:
            maxgain = 109
        else:
            maxgain = 78
        if (gain < 0) | (gain > maxgain):
            raise ValueError("Invalid Gain, range 0-%d only" % maxgain)
        self.gain_cached = gain
        self.oa.sendMessage(CODE_WRITE, ADDR_GAIN, [gain])

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
        rawgain = self.getGain()
        if self._is_husky:
            if self.gainlow_cached:
                gaindb = -15 + 50.0*float(rawgain)/109
            else:
                gaindb = 15 + 50.0*float(rawgain)/109 
        else:
            #GAIN (dB) = 50 (dB/V) * VGAIN - 6.5 dB, (HILO = LO)
            #GAIN (dB) = 50 (dB/V) * VGAIN + 5.5 dB, (HILO = HI)
            gainV = (float(rawgain) / 256.0) * 3.3
            if self.gainlow_cached:
                gaindb = 50.0 * gainV - 6.5
            else:
                gaindb = 50.0 * gainV + 5.5

        return gaindb

    def _set_gain_db(self, gain):
        if self._is_husky:
            mingain = -15.0
            maxgain = 65.0
            use_low_thresh = 15
            steps = 109
            gainrange = 50.0
        else:
            mingain = -6.5
            maxgain = 56.0
            use_low_thresh = 5.5
            steps = 78
        if gain < mingain or gain > maxgain:
            raise ValueError("Gain %f out of range. Valid range: %3.1f to %3.1f dB" % (gain, mingain, maxgain))

        use_low = False
        if gain < use_low_thresh:
            use_low = True
            self.setMode("low")
        else:
            self.setMode("high")

        if self._is_husky:
            if use_low:
                bottom = mingain
            else:
                bottom = use_low_thresh
            g = (gain - bottom) / gainrange * steps
        else:
            if use_low:
                gv = (float(gain) - mingain) / 50.0
            else:
                gv = (float(gain) - use_low_thresh ) / 50.0
            g = (gv / 3.3) * 256.0

        g = round(g)
        g = int(g)
        if g < 0:
            g = 0
        if g > 0xFF:
            g = 0xFF

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
        self._test_mode = False
        self._bits_per_sample = 10
        self._support_get_duration = True
        self._is_pro = False
        self._is_lite = False
        self._is_husky = False
        self._cached_samples = None
        self._cached_offset = None
        self._is_sakura_g = None

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
        if self._is_pro or self._is_lite:
            dict['fifo_fill_mode'] = self.fifo_fill_mode
        if self._is_pro:
            dict['stream_mode'] = self.stream_mode
        if self._is_husky:
            dict['test_mode'] = self.test_mode
            dict['bits_per_sample'] = self.bits_per_sample
            dict['segments'] = self.segments
            dict['segment_cycles'] = self.segment_cycles
            dict['errors'] = self.errors

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
        - cwhusky: 131070

        :Getter: Return the current number of total samples (integer)

        :Setter: Set the number of samples to capture

        Raises:
           ValueError: if number of samples is negative
        """
        if self._cached_samples is None:
            self._cached_samples = self._get_num_samples()
        return self._cached_samples

    @samples.setter
    def samples(self, samples):
        if self._is_sakura_g:
            diff = (12 - (samples % 12)) % 12
            samples += diff
            if diff > 0:
                scope_logger.warning("Sakura G samples must be divisible by 12, rounding up to {}...".format(samples))

        if self._get_fifo_fill_mode() == "segment":
            diff = (3 - (samples - 1) % 3)
            samples += diff
            if diff > 0:
                scope_logger.warning("segment mode requires (samples-1) divisible by 3, rounding up to {}...".format(samples))

        self._cached_samples = samples
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
        if self._cached_offset is None:
            self._cached_offset = self._get_offset()
        return self._cached_offset

    @offset.setter
    def offset(self, setting):
        self._cached_offset = setting
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

        Only applies to the ADC capture - the glitch module
        is always a rising edge trigger.

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

    @property
    def fifo_fill_mode(self):
        """The ADC buffer fill strategy - allows segmented usage for CW-lite and CW-pro.

        .. warning:: THIS REQUIRES NEW FPGA BITSTREAM - NOT YET IN THE PYTHON.

        Only the 'Normal' mode is well supported, the other modes can
        be used carefully.

        For segmenting on CW-Husky, see 'segments' instead.

        There are four possible modes:
         * "normal": Trigger line & logic work as expected.
         * "enable": Capture starts with rising edge, but writing samples
                     is enabled by active-high state of trigger line.
         * "segment": Capture starts with rising edge, and writes `trigger.samples`
                     to buffer on each rising edge, stopping when the buffer
                     is full. For this to work adc.samples must be a multiple
                     of 3 (will be enforced by API).

        .. warning:: The "enable" and "segment" modes requires you to fill
                    the **full buffer** (~25K on CW-Lite, ~100K on CW-Pro).
                    This requires you to ensure the physical trigger line will
                    be high (enable mode) or toggle (segment mode) enough. The
                    ChipWhisperer hardware will currently stall until the
                    internal buffer is full, and future commands will fail.

        .. warning:: adc.basic_mode must be set to "rising_edge" if a fill_mode other than
                    "normal" is used. Bad things happen if not.

        :Getter: Return the current fifo fill mode (one of the 3 above strings)

        :Setter: Set the fifo fill mode

        Raises:
           ValueError: if value is not one of the allowed strings
        """

        return self._get_fifo_fill_mode()

    @fifo_fill_mode.setter
    def fifo_fill_mode(self, mode):
        known_modes = ["normal", "enable", "segment"]
        if mode not in known_modes:
            raise ValueError("Invalid fill mode %s. Valid modes: %s" % (mode, known_modes), mode)

        self._set_fifo_fill_mode(mode)

        # Segment mode requires samples have an odd divisability to work
        if mode == "segment":
            self.samples = self.samples

    def _get_fifo_fill_mode(self):
        result = self.oa.sendMessage(CODE_READ, ADDR_ADVCLK, maxResp=4)
        mode = result[3] & 0x30

        if mode == 0x00:
            return "normal"

        if mode == 0x10:
            return "enable"

        if mode == 0x20:
            return "segment"

        return "????"

    def _set_fifo_fill_mode(self, mode):
        if mode == "normal":
            mask = 0
        elif mode == "enable":
            mask = 1
        elif mode == "segment":
            mask = 2
        else:
            raise ValueError("Invalid option for fifo mode: {}".format(mask))

        result = self.oa.sendMessage(CODE_READ, ADDR_ADVCLK, maxResp=4)
        result[3] &= ~(0x30)
        result[3] |= mask << 4
        self.oa.sendMessage(CODE_WRITE, ADDR_ADVCLK, result, readMask= [0x3f, 0xff, 0xff, 0xfd])


    @property
    def segments(self):
        """Number of sample segments to capture.

        .. warning:: Supported by CW-Husky only. For segmenting on CW-lite or
        CW-pro, see 'fifo_fill_mode' instead.

        This setting must be a 16-bit positive integer. 

        In normal operation, segments=1. 

        Multiple segments are useful in two scenarios:
        (1) Capturing only subsections of a power trace, to allow longer effective captures.
            After a trigger event, the requested number of samples is captured every 'segment_cycles' 
            clock cycles.
        (2) Speeding up capture times by capturing 'segments' power traces from a single arm + capture
            event. Here, the requested number of samples is captured at every trigger event, without
            having to re-arm and download trace data between every trigger event.

        .. warning:: when capturing multiple segments with presamples, the total number of samples 
        per segment must be a multiple of 3. Incorrect sample data will be obtained if this is not 
        the case.

        :Getter: Return the current number of presamples

        :Setter: Set the number of presamples.

        Raises:
           ValueError: if segments is outside of range [1, 2^16-1]
        """

        return self._get_segments()

    @segments.setter
    def segments(self, num):
        if num < 1 or num > 2**16-1:
            raise ValueError("Number of segments must be in range [1, 2^16-1]")
        self._set_segments(num)

    def _get_segments(self):
        if self.oa is None:
            return 0

        cmd = self.oa.sendMessage(CODE_READ, ADDR_SEGMENTS, maxResp=2)
        segments = int.from_bytes(cmd, byteorder='little')
        return segments


    def _set_segments(self, num):
        self.oa.sendMessage(CODE_WRITE, ADDR_SEGMENTS, list(int.to_bytes(num, length=2, byteorder='little')))


    @property
    def errors(self):
        """Internal error flags (FPGA FIFO over/underflow)
        .. warning:: Supported by CW-Husky only.
        """
        return self._get_errors()

    def _get_errors(self):
        if self.oa is None:
            return 0
        raw = self.oa.sendMessage(CODE_READ, ADDR_FIFO_STAT, maxResp=1)[0]
        stat = ''
        if raw & 1:  stat += 'slow FIFO underflow, '
        if raw & 2:  stat += 'slow FIFO overflow, '
        if raw & 4:  stat += 'fast FIFO underflow, '
        if raw & 8:  stat += 'fast FIFO overflow, '
        if raw & 16: stat += 'presample error, '
        if raw & 32: stat += 'ADC clipped, '
        if stat == '':
            stat = 'no errors'
        return stat


    @property
    def segment_cycles(self):
        """Number of clock cycles separating segments.

        .. warning:: Supported by CW-Husky only. For segmenting on CW-lite or
        CW-pro, see 'fifo_fill_mode' instead.

        This setting must be a 20-bit positive integer. 

        When 'segments' is greater than one, set segment_cycles to a non-zero value to capture a new 
        segment every 'segment_cycles' clock cycles.

        :Getter: Return the current value of segment_cycles.

        :Setter: Set segment_cycles.

        Raises:
           ValueError: if segments is outside of range [0, 2^16-1]
        """

        return self._get_segment_cycles()

    @segment_cycles.setter
    def segment_cycles(self, num):
        if num < 0 or num > 2**20-1:
            raise ValueError("Number of segments must be in range [0, 2^20-1]")
        self._set_segment_cycles(num)

    def _get_segment_cycles(self):
        if self.oa is None:
            return 0

        cmd = self.oa.sendMessage(CODE_READ, ADDR_SEGMENT_CYCLES, maxResp=3)
        segment_cycles = int.from_bytes(cmd, byteorder='little')
        return segment_cycles


    def _set_segment_cycles(self, num):
        self.oa.sendMessage(CODE_WRITE, ADDR_SEGMENT_CYCLES, list(int.to_bytes(num, length=3, byteorder='little')))



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


    def _set_test_mode(self, enabled):
        self._test_mode = enabled
        if enabled:
            self.oa.sendMessage(CODE_WRITE, ADDR_DATA_SOURCE, [0])
            self.oa.sendMessage(CODE_WRITE, ADDR_NO_CLIP_ERRORS, [1])
        else:
            self.oa.sendMessage(CODE_WRITE, ADDR_DATA_SOURCE, [1])
            self.oa.sendMessage(CODE_WRITE, ADDR_NO_CLIP_ERRORS, [0])


    def _get_test_mode(self):
        return self._test_mode

    @property
    def test_mode(self):
        """The ChipWhisperer's test mode. Only available on CW-Husky.

        When test mode is enabled, an internally-generated count-up pattern is
        captured, instead of the ADC sample data.

        :Getter: Return True if test mode is enabled and False otherwise

        :Setter: Enable or disable test mode
        """
        return self._get_test_mode()

    @test_mode.setter
    def test_mode(self, enabled):
        self._set_test_mode(enabled)


    def _set_bits_per_sample(self, bits):
        self._bits_per_sample = bits
        # update FPGA:
        if bits == 8:
            val = 1
        else:
            val = 0
        self.oa.sendMessage(CODE_WRITE, ADDR_ADC_LOW_RES, [val])
        # Notify capture system:
        self.oa.setBitsPerSample(bits)

    def _get_bits_per_sample(self):
        return self._bits_per_sample

    @property
    def bits_per_sample(self):
        """Bits per ADC sample. Only available on CW-Husky.

        Husky has a 12-bit ADC; optionally, we read back only 8 bits per
        sample.  This does *not* allow for more samples to be collected; it
        only allows for a faster sampling rate in streaming mode.

        :Getter: return the number of bits per sample that will be received.

        :Setter: set the number of bits per sample to receive.
        """
        return self._get_bits_per_sample()

    @bits_per_sample.setter
    def bits_per_sample(self, bits):
        if bits not in [8,12]:
            raise ValueError("Valid settings: 8 or 12.")
        self._set_bits_per_sample(bits)


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
        if self._is_husky and samples < 7:
            scope_logger.warning('There may be issues with this few samples')
        self._numSamples = samples
        self.oa.setNumSamples(samples)

    def _get_num_samples(self, cached=True):
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
        self.oa.sendMessage(CODE_WRITE, ADDR_OFFSET, list(int.to_bytes(offset, length=4, byteorder='little')))

    def _get_offset(self):
        if self.oa is None:
            return 0

        cmd = self.oa.sendMessage(CODE_READ, ADDR_OFFSET, maxResp=4)
        offset = int.from_bytes(cmd, byteorder='little')
        return offset

    def _set_presamples(self, samples):
        if self._is_husky:
            min_samples = 8
            max_samples = min(self.samples, 32768)
        else:
            min_samples = 0
            max_samples = self.samples
        if samples < min_samples and samples != 0:
            raise ValueError("Number of pre-trigger samples cannot be less than %d" % min_samples)
        if samples > max_samples:
            if self._is_husky:
                raise ValueError("Number of pre-trigger samples cannot be larger than the lesser of [total number of samples, 32768] (%d)." % max_samples)
            else:
                raise ValueError("Number of pre-trigger samples cannot be larger than the total number of samples (%d)." % max_samples)

        self.presamples_desired = samples

        if self._is_pro or self._is_lite or self._is_husky:
            #CW-1200 Hardware / CW-Lite / CW-Husky
            samplesact = samples
            self.presamples_actual = samples
        else:
            #Other Hardware
            if samples > 0:
                scope_logger.warning('Pre-sample on CW-Lite is unreliable with many FPGA bitstreams. '
                                'Check data is reliably recorded before using in capture.')

            #enforce samples is multiple of 3
            samplesact = int(samples / 3)

            #Old crappy FIFO system that requires the following
            if samplesact > 0:
                samplesact = samplesact + self.presampleTempMargin

            self.presamples_actual = samplesact * 3

        self.oa.sendMessage(CODE_WRITE, ADDR_PRESAMPLES, list(int.to_bytes(samplesact, length=4, byteorder='little')))


        #print "Requested presamples: %d, actual: %d"%(samples, self.presamples_actual)

        self.oa.presamples_desired = samples

        return self.presamples_actual

    def _get_presamples(self, cached=False):
        """If cached returns DESIRED presamples"""
        if self.oa is None:
            return 0

        if cached:
            return self.presamples_desired

        temp = self.oa.sendMessage(CODE_READ, ADDR_PRESAMPLES, maxResp=4)
        samples = int.from_bytes(temp, byteorder='little')

        #CW1200/CW-Lite/Husky reports presamples using different method
        if self._is_pro or self._is_lite or self._is_husky:
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

        if self._support_get_duration:

            temp = self.oa.sendMessage(CODE_READ, ADDR_TRIGGERDUR, maxResp=4)

            #Old versions don't support this feature
            if temp is None:
                self._support_get_duration = False
                return -1

            samples = int.from_bytes(temp, byteorder='little')
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
        get a faster or slower clock as desired. Minimum clock in practice
        is 3.2MHz.

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
        if freq < 3.2E6: #practical min limit of clkgen
            scope_logger.warning("Requested clock value below minimum of 3.2MHz - DCM may not lock!")
        if self._get_clkgen_src() == "extclk":
            inpfreq = self._get_extclk_freq()
        else:
            inpfreq = self._hwinfo.sysFrequency()
        if self.oa.hwInfo.is_cwhusky():
            sets = self._calculateHuskyClkGenMulDiv(freq, inpfreq)
        else:
            sets = self._calculateClkGenMulDiv(freq, inpfreq)
        self._setClkgenMulWrapper(sets[0])
        self._setClkgenDivWrapper(sets[1:])
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


    def _calculateHuskyClkGenMulDiv(self, freq, inpfreq=96e6, vcomin=600e6, vcomax=1200e6):
        """Calculate Multiply & Divide settings based on input frequency"""
        lowerror = 1e99
        best = (0,0,0)
        for maindiv in range(1,6):
            mmin = int(np.ceil(vcomin/inpfreq*maindiv))
            mmax = int(np.ceil(vcomax/inpfreq*maindiv))
            for mul in range(mmin,mmax+1):
                if mul/maindiv < vcomin/inpfreq or mul/maindiv > vcomax/inpfreq:
                    continue
                for secdiv in range(1,127):
                    calcfreq = inpfreq*mul/maindiv/secdiv
                    err = abs(freq - calcfreq)
                    if err < lowerror:
                        lowerror = err
                        best = (mul, maindiv, secdiv)
        if best == (0,0,0):
            raise ValueError("Couldn't find a legal div/mul combination")
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
            if self.oa.hwInfo.is_cwhusky():
                return self._get_husky_clkgen_mul()

            else:
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
        if self.oa.hwInfo.is_cwhusky():
            self._set_husky_clkgen_mul(mul)
        else:
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


    def _set_husky_clkgen_mul(self, mul):
        # calculate register value:
        if type(mul) != int:
            raise ValueError("Only integers are supported")
        muldiv2 = int(mul/2)
        lo = muldiv2
        if mul%2:
            hi = lo+1
        else:
            hi = lo
        raw = list(int.to_bytes(lo + (hi<<6) + 0x1000, length=2, byteorder='little'))
        # data to write:
        self.oa.sendMessage(CODE_WRITE, ADDR_DRP_DATA, raw)
        # write data to addres 0x14:
        self.oa.sendMessage(CODE_WRITE, ADDR_DRP_ADDR, [0x94])


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
            if self.oa.hwInfo.is_cwhusky():
                return self._get_husky_clkgen_div()

            else:
                result = self.oa.sendMessage(CODE_READ, ADDR_ADVCLK, maxResp=4)
                val = result[2]
                val += 1

                if (result[3] & 0x02):
                    # Done loading value yet
                    return val

                self._clkgenLoad()

                timeout -= 1

        openadc_logger.error("CLKGEN Failed to load divider value. Most likely clock input to CLKGEN is stopped, check CLKGEN"
                      " source settings. CLKGEN clock results are currently invalid.")
        return 1

    @clkgen_div.setter
    def clkgen_div(self, div):
        if self.oa.hwInfo.is_cwhusky():
            # Husky PLL takes two dividers; if only one was provided, set the other to 1
            if type(div) == int:
                div = [div, 1]
            self._set_husky_clkgen_div(div)
        else:
            self._setClkgenDivWrapper(div)


    def _set_husky_clkgen_div(self, div):
        main_div = div[0]
        sec_div = div[1]
        if type(main_div) != int or type(sec_div) != int:
            raise ValueError("Only integers are supported")
        # 1. Set main divider:
        if main_div == 1:
            raw = list(int.to_bytes(0x1000, length=2, byteorder='little'))
        else:
            div2 = int(main_div/2)
            lo = div2
            if main_div % 2:
                hi = lo+1
            else:
                hi = lo
            raw = list(int.to_bytes(lo + (hi<<6), length=2, byteorder='little'))
        # data to write:
        self.oa.sendMessage(CODE_WRITE, ADDR_DRP_DATA, raw)
        # write data to addres 0x16:
        self.oa.sendMessage(CODE_WRITE, ADDR_DRP_ADDR, [0x96])

        # 2. Set clkout0 secondary divider:
        if sec_div == 1:
            raw = list(int.to_bytes(0x0040, length=2, byteorder='little'))
            # data to write:
            self.oa.sendMessage(CODE_WRITE, ADDR_DRP_DATA, raw)
            # write data to address 0x09:
            self.oa.sendMessage(CODE_WRITE, ADDR_DRP_ADDR, [0x89])
        else:
            div2 = int(sec_div/2)
            lo = div2
            if sec_div%2:
                hi = lo+1
            else:
                hi = lo
            raw = list(int.to_bytes(lo + (hi<<6) + 0x1000, length=2, byteorder='little'))
            # data to write:
            self.oa.sendMessage(CODE_WRITE, ADDR_DRP_DATA, raw)
            # write data to address 0x08:
            self.oa.sendMessage(CODE_WRITE, ADDR_DRP_ADDR, [0x88])


    def _setClkgenDivWrapper(self, div):
        if self.oa.hwInfo.is_cwhusky():
            self._set_husky_clkgen_div(div)
        else:
            if hasattr(div, "__getitem__"):
                div = div[0]
            if div < 1:
                div = 1

            result = self.oa.sendMessage(CODE_READ, ADDR_ADVCLK, maxResp=4)
            div -= 1
            result[2] = div
            result[3] |= 0x01
            self.oa.sendMessage(CODE_WRITE, ADDR_ADVCLK, result, readMask=self._readMask)
            result[3] &= ~(0x01)
            self.oa.sendMessage(CODE_WRITE, ADDR_ADVCLK, result, readMask=self._readMask)


    def _get_husky_clkgen_div(self):
        # 1. read DIVREG to get main divider:
        self.oa.sendMessage(CODE_WRITE, ADDR_DRP_ADDR, [0x16])
        raw = self.oa.sendMessage(CODE_READ, ADDR_DRP_DATA, maxResp=2)
        if raw[1] & 0x10:
            maindiv = 1
        else:
            # extract high time and low time
            lo = (raw[0] & 0x3f)
            hi = (raw[0]>>6) + ((raw[1] & 0x0f)<<2)
            maindiv = lo + hi
        # 2. read CLKOUT2 to ensure fractional mode is disabled and check NO_COUNT bit for CLKOUT divider:
        self.oa.sendMessage(CODE_WRITE, ADDR_DRP_ADDR, [0x9])
        raw = self.oa.sendMessage(CODE_READ, ADDR_DRP_DATA, maxResp=2)
        if raw[1] & 0x08:
            logging.error('CLKGEN fractional mode is enabled. This is unexpected.')
        if raw[0] & 0x40:
            secdiv = 1
        else:
            # 3. read CLKOUT divider:
            self.oa.sendMessage(CODE_WRITE, ADDR_DRP_ADDR, [0x8])
            raw = self.oa.sendMessage(CODE_READ, ADDR_DRP_DATA, maxResp=2)
            # extract high time and low time
            lo = (raw[0] & 0x3f)
            hi = (raw[0]>>6) + ((raw[1] & 0x0f)<<2)
            secdiv = lo + hi
        return maindiv*secdiv


    def _get_husky_clkgen_mul(self):
        # 1. read CLKFBOUT2 to ensure fractional mode is disabled:
        self.oa.sendMessage(CODE_WRITE, ADDR_DRP_ADDR, [0x11])
        raw = self.oa.sendMessage(CODE_READ, ADDR_DRP_DATA, maxResp=2)
        if raw[1] & 0x08:
            print('WARNING: fractional mode is enabled. This is unexpected. Reported multiplier value will be incorrect.')
        # 2. check "NO COUNT" bit:
        if raw[0] & 0x04:
            mul = 1
        else:
            # 3. read CLKFBOUT:
            self.oa.sendMessage(CODE_WRITE, ADDR_DRP_ADDR, [0x14])
            raw = self.oa.sendMessage(CODE_READ, ADDR_DRP_DATA, maxResp=2)
            # extract high time and low time
            lo = (raw[0] & 0x3f)
            hi = (raw[0]>>6) + ((raw[1] & 0x0f)<<2)
            mul = lo + hi
            #if lo != hi: print('WARNING: high and low times unequal (%d, %d) ! Duty cycle is not 50/50. This is unexpected. % (hi, lo))
        return mul

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
            scope_logger.warning("No phase shift loaded")
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
            scope_logger.error("ADVCLK register not present. Version mismatch")
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

        #Get sample frequency
        samplefreq = float(self.oa.hwInfo.sysFrequency()) / float(pow(2,23))

        temp = self.oa.sendMessage(CODE_READ, ADDR_FREQ, maxResp=4)
        freq = int.from_bytes(temp, byteorder='little')

        measured = freq * samplefreq
        return int(measured)

    def _getAdcFrequency(self):
        """Return the external frequency measured on 'CLOCK' pin. Returned value
           is in Hz"""
        if self.oa is None:
            return 0

        #Get sample frequency
        samplefreq = float(self.oa.hwInfo.sysFrequency()) / float(pow(2,23))

        temp = self.oa.sendMessage(CODE_READ, ADDR_ADCFREQ, maxResp=4)
        freq = int.from_bytes(temp, byteorder='little')

        measured = freq * samplefreq

        return int(measured)

    def _adcSampleRate(self):
        """Return the sample rate, takes account of decimation factor (if set)"""
        return self._getAdcFrequency() / self.oa.decimate()


class OpenADCInterface(object):

    cached_settings = None
    def __init__(self, serial_instance):
        self.serial = serial_instance
        self.offset = 0.5
        self.ddrMode = False
        self.sysFreq = 0
        self._streammode = False
        self._bits_per_sample = 10
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
        self._is_husky = False
        self._fast_fifo_read = True

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

    def setFastSMC(self, fast):
        self.serial.set_smc_speed(fast)

    def setBitsPerSample(self, bits):
        self._bits_per_sample = bits

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

            scope_logger.error('%d errors in %d' % (totalerror, totalbytes))

    def sendMessage(self, mode, address, payload=None, Validate=False, maxResp=None, readMask=None):
        """Send a message out the serial port"""

        if payload is None:
            payload = []

        #Get length
        length = len(payload)

        if ((mode == CODE_WRITE) and (length < 1)) or ((mode == CODE_READ) and (length != 0)):
            scope_logger.warning('Invalid payload for mode')
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

                        scope_logger.error(errmsg)
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
            self.serial.write(message)

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
                    scope_logger.warning('Timeout in read: %d (address: 0x%02x)' % (len(result), address))
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

                        scope_logger.error(errmsg)

### Generic
    def setSettings(self, state, validate=False):
        cmd = bytearray(1)
        cmd[0] = state
        self.cached_settings = state
        self.sendMessage(CODE_WRITE, ADDR_SETTINGS, cmd, Validate=validate)

    def settings(self, use_cached=False):
        if (not use_cached) or (not self.cached_settings):
            sets = self.sendMessage(CODE_READ, ADDR_SETTINGS)
            if sets is None:
                self.cached_settings = 0
            else:
                self.cached_settings = sets[0]
        return self.cached_settings

    def setReset(self, value):
        if value:
            self.setSettings(self.settings() | SETTINGS_RESET, validate=False)
            #TODO: Hack to adjust the hwMaxSamples since the number should be smaller than what is being returned
            self.hwMaxSamples = self.numSamples() - 45
            #TODO: another hack... if reconnecting to Husky which was previously set to have a small number of samples, avoid feeding through a negative number... don't quite understand what's happening here and why...
            if self.hwMaxSamples < 0:
                self.hwMaxSamples = 0
            self.setNumSamples(self.hwMaxSamples)
        else:
            self.setSettings(self.settings() & ~SETTINGS_RESET)

    def triggerNow(self):
        initial = self.settings()
        self.setSettings(initial | SETTINGS_TRIG_NOW)
        time.sleep(0.05)
        self.setSettings(initial & ~SETTINGS_TRIG_NOW)

    def getStatus(self):
        result = self.sendMessage(CODE_READ, ADDR_STATUS)

        if len(result) == 1:
            return result[0]
        else:
            return None

    def setNumSamples(self, samples):
        self.sendMessage(CODE_WRITE, ADDR_SAMPLES, list(int.to_bytes(samples, length=4, byteorder='little')))
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
        self.sendMessage(CODE_WRITE, ADDR_DECIMATE, list(int.to_bytes(decsamples, length=2, byteorder='little')))


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
        temp = self.sendMessage(CODE_READ, ADDR_SAMPLES, maxResp=4)
        samples = int.from_bytes(temp, byteorder='little')
        return samples

    def getBytesInFifo(self):
        temp = self.sendMessage(CODE_READ, ADDR_BYTESTORX, maxResp=4)
        samples = int.from_bytes(temp, byteorder='little')
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
        # keeps calling self.setting() - what if we cache it?
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

    def capture(self, offset=None, adc_freq=29.53E6, samples=24400):
        timeout = False
        sleeptime = 0
        if offset:
            sleeptime = (29.53E6*8*offset)/(100000*adc_freq) #rougly 8ms per 100k offset
            sleeptime /= 1000

        if self._streammode:

            # Wait for a trigger, letting the UI run when it can
            starttime = datetime.datetime.now()
            while self.serial.cmdReadStream_isDone() == False:
                # Wait for a moment before re-running the loop
                #time.sleep(0.05)
                diff = datetime.datetime.now() - starttime

                # If we've timed out, don't wait any longer for a trigger
                if (diff.total_seconds() > self._timeout):
                    scope_logger.warning('Timeout in OpenADC capture(), no trigger seen! Trigger forced, data is invalid. Status: %02x'%status)
                    timeout = True
                    self.triggerNow()
                    break


            self._stream_rx_bytes, stream_timeout = self.serial.cmdReadStream()
            timeout |= stream_timeout
            #Check the status now
            # XXX-Husky? bytes_left, overflow_bytes_left, unknown_overflow = self.serial.cmdReadStream_getStatus()
            #scope_logger.debug("Streaming done, results: rx_bytes = %d, bytes_left = %d, overflow_bytes_left = %d"%(self._stream_rx_bytes, bytes_left, overflow_bytes_left))
            scope_logger.debug("Streaming done, results: rx_bytes = %d"%(self._stream_rx_bytes))
            self.arm(False)

            if stream_timeout:
                if self._stream_rx_bytes == 0: # == (self._stream_len - 3072):
                    scope_logger.warning("Streaming mode OVERFLOW occured as trigger too fast - Adjust offset upward (suggest = 200 000)")
                else:
                    scope_logger.warning("Streaming mode OVERFLOW occured during capture - ADC sample clock probably too fast for stream mode (keep ADC Freq < 10 MHz)")
                timeout = True
        else:
            status = self.getStatus()
            starttime = datetime.datetime.now()

            # Wait for a trigger, letting the UI run when it can
            while ((status & STATUS_ARM_MASK) == STATUS_ARM_MASK) | ((status & STATUS_FIFO_MASK) == 0):
                status = self.getStatus()

                # Wait for a moment before re-running the loop
                #time.sleep(sleeptime) ## <-- This causes the capture slowdown
                #util.better_delay(sleeptime) ## faster sleep method
                diff = datetime.datetime.now() - starttime

                # If we've timed out, don't wait any longer for a trigger
                if (diff.total_seconds() > self._timeout):
                    scope_logger.warning('Timeout in OpenADC capture(), no trigger seen! Trigger forced, data is invalid. Status: %02x'%status)
                    timeout = True
                    self.triggerNow()

                    #Once in timeout mode we can't rely on STATUS_ARM_MASK anymore - just wait for FIFO to fill up
                    if (status & STATUS_FIFO_MASK) == 0:
                        break

                # Give the UI a chance to update (does nothing if not using UI)

            #time.sleep(0.005)
            #time.sleep(sleeptime*10)

            # If using large offsets, system doesn't know we are delaying api

            # NOTE: This doesn't actually delay until adc starts reading
            # so need to actually do the manual delay
            #nosampletimeout = self._nosampletimeout * 10
            #while (self.getBytesInFifo() == 0) and nosampletimeout:
            #    logging.debug("Bytes in Fifo: {}".format(self.getBytesInFifo()))
            #    time.sleep(0.001)
            #    nosampletimeout -= 1

            #if nosampletimeout == 0:
            #    logging.warning('No samples received. Either very long offset, or no ADC clock (try "Reset ADC DCM"). '
            #                    'If you need such a long offset, increase "scope.qtadc.sc._nosampletimeout" limit.')
            #    timeout = True

        # give time for ADC to finish reading data
        # may need to adjust delay
        cap_delay = (7.37E6 * 4 * samples) / (adc_freq * 24400)
        cap_delay *= 0.001
        time.sleep(cap_delay+sleeptime)
        # 0.000819672131147541
        # 
        #time.sleep(sleeptime) #need to do this one as well
        self.arm(False) # <------ ADC will stop reading after this
        return timeout

    def flush(self):
        # Flush output FIFO
        self.sendMessage(CODE_READ, ADDR_ADCDATA, None, False, None)

    def readData(self, NumberPoints=None, progressDialog=None):
        scope_logger.debug("Reading data from OpenADC...")
        if self._is_husky: 
            return self.readHuskyData(NumberPoints)
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
                    scope_logger.warning("Stream mode: Expected sync byte (AC) at location %d but got %x" % (i, self._sbuf[i]))
                    break
                s = i + 1
                data[dbuf2_idx: (dbuf2_idx + (bsize - 1))] = self._sbuf[s:(s + (bsize - 1))]

                # Write to next section
                dbuf2_idx += (bsize - 1)

            scope_logger.debug("Stream mode: read %d bytes"%len(data))

            # Turn raw bytes into samples
            datapoints = self.processData(data, 0.0)

            if datapoints is not None and len(datapoints):
                scope_logger.debug("Stream mode: done, %d samples processed"%len(datapoints))
            else:
                scope_logger.warning("Stream mode: done, no samples resulted from processing")
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
                #print(bytesToRead)

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
                #print(data)

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

            if datapoints is None:
                return []
            if len(datapoints) > NumberPoints:
                datapoints = datapoints[0:NumberPoints]

            # if len(datapoints) < NumberPoints:
            # print len(datapoints),
            # print NumberPoints

            return datapoints



    def readHuskyData(self, NumberPoints=None):
        if self._bits_per_sample == 12:
            bytesToRead = int(np.ceil(NumberPoints*1.5))
        else:
            bytesToRead = NumberPoints
        # Husky hardware always captures a multiple of 3 samples. We want to read all the captured samples.
        if bytesToRead % 3:
            bytesToRead += 3  - bytesToRead % 3
        scope_logger.debug("XXX reading with NumberPoints=%d, bytesToRead=%d" % (NumberPoints, bytesToRead))

        if self._stream_mode:
            print('yo yo YO! (streaming)')
            data = self._sbuf
        else:
            if self._fast_fifo_read:
                # switch FPGA and SAM3U into fast read timing mode
                self.sendMessage(CODE_WRITE, ADDR_FAST_FIFO_READ, [1])
                self.serial.set_smc_speed(1)
            data = self.sendMessage(CODE_READ, ADDR_ADCDATA, None, False, bytesToRead)
            # switch FPGA and SAM3U back to regular read timing mode
            self.sendMessage(CODE_WRITE, ADDR_FAST_FIFO_READ, [0])
            self.serial.set_smc_speed(0)

        # XXX Husky debug:
        scope_logger.debug("XXX read %d bytes; NumberPoints=%d, bytesToRead=%d" % (len(data), NumberPoints, bytesToRead))
        if data is not None:
            data = np.array(data)
            datapoints = self.processHuskyData(NumberPoints, data)
        if datapoints is None:
            return []
        return datapoints[:NumberPoints]



    def processHuskyData(self, NumberPoints, data):
        if self._bits_per_sample == 12:
            #print('XXX NumberPoints: %d, len(data): %d' % (NumberPoints, len(data)))
            # slower, easier to follow data process:
            #samples = np.zeros(NumberPoints, dtype=np.int16)
            #for i in range(NumberPoints):
            #    if (i%2 == 0):
            #        j = int(i*1.5)
            #        samples[i] = (data[j]<<4) + (data[j+1]>>4)
            #    else:
            #        j = int((i-1)*1.5)
            #        samples[i] = ((data[j+1] & 0xf)<<8) + data[j+2]


            # faster, harder to follow data process:
            data = np.array(data, dtype=np.int16)

            evenNumberPoints = NumberPoints + NumberPoints % 2
            i = np.arange(len(data), dtype=np.int32)
            a = data[i%3==0][:int(evenNumberPoints/2)]
            b = data[i%3==1][:int(evenNumberPoints/2)]
            c = data[i%3==2][:int(evenNumberPoints/2)]
            #print('XXX i:%d, a:%d, b:%d, c:%d, np:%d' % (len(i), len(a), len(b), len(c), NumberPoints))
            samples = np.zeros(evenNumberPoints, dtype=np.int16)
            i = np.arange(evenNumberPoints, dtype=np.int32)
            try:
                samples[i%2==0] = (a << 4) + (b >> 4)
                samples[i%2==1] = ((b & 0x0F) << 8) + c
            except:
                scope_logger.error("Husky processing error: data={}, a={}, b={}, c={}, NumPoints={}".format(data, a, b, c, NumberPoints))
                scope_logger.error("lendata={}, lena={}, lenb={}, lenc={}".format(len(data), len(a), len(b), len(c)))
                raise
            #samples = samples[:NumberPoints]

            # print("Samples equal?: {}".format((fast_samples==samples).all()))
            # print(a[0], b[0], c[0], data[0], data[1], data[2])
            # for i in range(NumberPoints):
            #     if fast_samples[i] != samples[i]:
            #         pass
            #         #print("{} bad, {} vs {}".format(i, fast_samples[i], samples[i]))

        else:
            #print('QWERTY')
            samples = data

        # fpData = samples / 2**self._bits_per_sample - self.offset

        #scope_logging.debug("Processed data, ended up with %d samples total"%len(fpData))
        # print(data[0], data[1], data[2], samples[0], samples[1])

        #return fpData # XXX Husky: temporary
        return samples



    def processData(self, data, pad=float('NaN'), debug=False):
        if data[0] != 0xAC:
            scope_logger.warning('Unexpected sync byte in processData(): 0x%x' % data[0])
            #print(data)
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
            scope_logger.debug("Trigger_data: {} len={}".format(trigger, len(trigger)))

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
            scope_logger.warning('Trigger not found in ADC data. No data reported!')
            scope_logger.debug('Trigger not found typically caused by the actual \
            capture starting too late after the trigger event happens')
            scope_logger.debug('Data: {}'.format(orig_data))


        #Ensure that the trigger point matches the requested by padding/chopping
        diff = self.presamples_desired - trigsamp
        if diff > 0:
               #fpData = [pad]*diff + fpData
               fpData = np.append([pad]*diff, fpData)
               scope_logger.warning('Pretrigger not met: Do not use downsampling and pretriggering at same time.')
               scope_logger.debug('Pretrigger not met: can attempt to increase presampleTempMargin(in the code).')
        else:
               fpData = fpData[-diff:]

        scope_logger.debug("Processed data, ended up with %d samples total"%len(fpData))

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

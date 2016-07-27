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
from chipwhisperer.common.utils.parameter import Parameter, Parameterized, setupSetParam

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
ADDR_SAMPLES    = 16
ADDR_PRESAMPLES = 17
ADDR_BYTESTORX  = 18
ADDR_DDR        = 20
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


class HWInformation(Parameterized):
    _name = 'HW Information'

    def __init__(self, oaiface):
        self.oa = oaiface
        self.oa.hwInfo = self
        self.sysFreq = 0
        self.params = Parameter(name=self.getName(), type='group')
        self.params.addChildren([
            {'name': 'Version', 'type': 'str', 'get':self.versions, 'readonly':True},
            {'name': 'Synth Date', 'type': 'str', 'get':self.synthDate, 'readonly':True},
            {'name': 'System Freq', 'type': 'int', 'siPrefix':True, 'suffix': 'Hz', 'get':self.sysFrequency, 'readonly':True},
            {'name': 'Max Samples', 'type': 'int', 'get':self.maxSamples, 'readonly':True}
        ])

        self.vers = None

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

        self.sysFreq = long(freq)
        return self.sysFreq

    def __del__(self):
        self.oa.hwInfo = None


class GainSettings(Parameterized):
    _name = 'Gain Setting'

    def __init__(self, oaiface):
        self.oa = oaiface
        self.gainlow_cached = False
        self.gain_cached = 0
        self.params = Parameter(name=self.getName(), type='group')
        self.params.addChildren([
            {'name': 'Mode', 'type': 'list', 'values': {"high", "low"}, 'default': 'low', 'set':self.setMode, 'get':self.mode, 'linked':['Result'],
                     'help': '%namehdr%'+
                             'Sets the AD8331 Low Noise Amplifier into to "High" or "Low" gain mode. Low mode ranges from ' +
                             '-4.5dB to +43.5dB, and High mode ranges from +7.5dB to +55.5dB. Better performance is found ' +
                             'using the "High" gain mode typically.'},
            {'name': 'Setting', 'type': 'int', 'limits': (0, 78), 'default': 0, 'set':self.setGain, 'get':self.gain, 'linked':['Result'],
                     'help':'%namehdr%'+
                            'Sets the AD8331 gain value. This is a unitless number which ranges from 0 (minimum) to 78 (maximum).' +
                            ' The resulting gain in dB is given in the "calculated" output.'},
            {'name': 'Result', 'type': 'float', 'suffix':'dB', 'readonly':True, 'get':self.gainDB,
                     'help':'%namehdr%'+
                            'Gives the gain the AD8331 should have, based on the "High/Low" setting and the "gain setting".'},
        ])

    @setupSetParam("Mode")
    def setMode(self, gainmode):
        '''Set the gain Mode'''
        if gainmode == "high":
            self.oa.setSettings(self.oa.settings() | SETTINGS_GAIN_HIGH)
            self.gainlow_cached = False
        elif gainmode == "low":
            self.oa.setSettings(self.oa.settings() & ~SETTINGS_GAIN_HIGH)
            self.gainlow_cached = True
        else:
            raise ValueError, "Invalid Gain Mode, only 'low' or 'high' allowed"

    def mode(self):
        return "low" #TODO: Read it from hardware!

    @setupSetParam("Setting")
    def setGain(self, gain):
        '''Set the Gain range 0-78'''
        if (gain < 0) | (gain > 78):
            raise ValueError,  "Invalid Gain, range 0-78 Only"

        self.gain_cached = gain

        cmd = bytearray(1)
        cmd[0] = gain
        self.oa.sendMessage(CODE_WRITE, ADDR_GAIN, cmd)

    def gain(self, cached=False):
        if cached == False:
            self.gain_cached = self.oa.sendMessage(CODE_READ, ADDR_GAIN)[0]

        return self.gain_cached

    def gainDB(self):
        #GAIN (dB) = 50 (dB/V) * VGAIN - 6.5 dB, (HILO = LO)
        # GAIN (dB) = 50 (dB/V) * VGAIN + 5.5 dB, (HILO = HI)

        gainV = (float(self.gain_cached) / 256.0) * 3.3

        if self.gainlow_cached:
            gaindb = 50.0 * gainV - 6.5
        else:
            gaindb = 50.0 * gainV + 5.5

        return gaindb


class TriggerSettings(Parameterized):
    _name = 'Trigger Setup'

    def __init__(self, oaiface):
        self.oa = oaiface
        self._numSamples = 0
        self.presamples_desired = 0
        self.presamples_actual = 0
        self.presampleTempMargin = 24
        self._timeout = 2
        self._stream_mode = False

        self.params = Parameter(name=self.getName(), type='group')
        child_list = [
            {'name': 'Refresh Status', 'type':'action', 'linked':['Trigger Pin State'], 'visible':False,
                     'help':'%namehdr%'+
                            'Refreshes the "Trigger Pin State" status.'},
            #{'name': 'Source', 'type': 'list', 'values':["digital", "analog"], 'set':self.setSource, 'get':self.source,
            #         'help':'%namehdr%'+
            #                'Selects if trigger system is based on digital signal (including internally generated), or an ADC level.'},
            {'name': 'Trigger Pin State', 'type':'bool', 'readonly':True, 'get':self.extTriggerPin,
                     'help':'%namehdr%'+
                            'Gives the status of the digital signal being used as the trigger signal, either high or low.'},
            {'name': 'Mode', 'type':'list', 'values':["rising edge", "falling edge", "low", "high"], 'default':"low", 'set':self.setMode, 'get':self.mode,
                     'help':'%namehdr%'+
                            'When using a digital system, sets the trigger mode:\n\n'
                            '  =============== ==============================\n' +
                            '  Mode            Description\n' +
                            '  =============== ==============================\n' +
                            '  Rising Edge     Trigger on rising edge only.\n' +
                            '  Falling Edge    Trigger on falling edge only.\n' +
                            '  Low             Trigger when line is "low".\n' +
                            '  High            Trigger when line is "high".\n' +
                            '  =============== ==============================\n\n' +
                            'Note the "Trigger Mode" should be set to "Rising Edge" if using either the "SAD Trigger" or "IO Trigger" modes.\n\n'+
                            'If using STREAM mode (CW-Pro only), the trigger should use a rising or falling edge. Using a constant level is possible, but ' +
                            'normally requires additional delay added via "offset" (see stream mode help for details).'
            },
            {'name': 'Timeout (secs)', 'type':'float', 'step':1, 'limits':(0, 1E99), 'set':self.setTimeout, 'get':self.timeout,
                     'help':'%namehdr%'+
                            'If no trigger occurs in this many seconds, force the trigger.'},
            {'name': 'Offset', 'type':'int', 'limits':(0, 4294967294), 'set':self.setOffset, 'get':self.offset,
                     'help':'%namehdr%'+
                            'Delays this many samples after the trigger event before recording samples. Based on the ADC clock cycles. ' +
                            'If using a 4x mode for example, an offset of "1000" would mean we skip 250 cycles of the target device.'},
            {'name': 'Pre-Trigger Samples', 'type':'int', 'limits':(0, self.oa.hwMaxSamples), 'set':self.setPresamples, 'get':self.presamples,
                     'help':'%namehdr%'+
                            'Record a certain number of samples before the main samples are captured. If "offset" is set to 0, this means ' +
                            'recording samples BEFORE the trigger event.'},
            {'name': 'Total Samples', 'type':'int', 'limits':(0, self.oa.hwMaxSamples), 'set':self.setNumSamples, 'get':self.numSamples,
                     'help':'%namehdr%'+
                            'Total number of samples to record. Note the capture system has an upper limit. Older FPGA bitstreams had a lower limit of about 256 samples.'+
                            'If using the ChipWhisperer-Lite/ChipWhisperer-Pro (CW1173/CW1200) this is no longer the case, and can be set to almost any number.'},
        ]

        if self.oa.hwInfo and self.oa.hwInfo.is_cw1200():
            child_list.append(
            {'name': 'Stream Mode', 'type': 'bool', 'default': self._stream_mode, 'set': self.setStreamMode,
             'get': self.getStreamMode,
             'help': '%namehdr%' +
                     'Streams data over high-speed USB allowing to capture more samples (the exact max sample value and '
                     'sample rate is unknown since it depends on how fast your computer can read from the buffer).'
                     ' A slow sampling rate (ADC Freq < 10 MHz) may be required.\n\n' +
                     'This feature is currently in BETA.'})
        self.params.addChildren(child_list)

    @setupSetParam("Stream Mode")
    def setStreamMode(self, enabled):
        self._stream_mode = enabled

        if enabled:
            self.findParam('Pre-Trigger Samples').setValue(0, ignoreReadonly=True)
            self.findParam('Pre-Trigger Samples').setReadonly(True)
            self.findParam('Total Samples').setLimits((0, 100E6))
        else:
            self.findParam('Pre-Trigger Samples').setReadonly(False)
            self.findParam('Total Samples').setLimits((0, self.oa.hwMaxSamples))

        #Write to FPGA
        base = self.oa.sendMessage(CODE_READ, ADDR_SETTINGS)[0]
        if enabled:
            val = base | (1<<4)
        else:
            val = base & ~(1<<4)
        self.oa.sendMessage(CODE_WRITE, ADDR_SETTINGS, [val])

        #Notify capture system
        self.oa.setStreamMode(enabled)

    def getStreamMode(self):
        return self._stream_mode

    def fifoOverflow(self):
        return self.oa.getStatus() & STATUS_OVERFLOW_MASK

    @setupSetParam("Total Samples")
    def setNumSamples(self, samples):
        self._numSamples = samples
        self.oa.setNumSamples(samples)

    def numSamples(self, cached=False):
        if self.oa is None:
            return 0

        if cached:
            return self._numSamples
        else:
            return self.oa.numSamples()

    @setupSetParam("Timeout (secs)")
    def setTimeout(self, timeout):
        self._timeout = timeout
        if self.oa:
            self.oa.setTimeout(timeout)

    def timeout(self):
        return self._timeout

    @setupSetParam("Offset")
    def setOffset(self,  offset):
        cmd = bytearray(4)
        cmd[0] = ((offset >> 0) & 0xFF)
        cmd[1] = ((offset >> 8) & 0xFF)
        cmd[2] = ((offset >> 16) & 0xFF)
        cmd[3] = ((offset >> 24) & 0xFF)
        self.oa.sendMessage(CODE_WRITE, ADDR_OFFSET, cmd)

    def offset(self):
        if self.oa is None:
            return 0

        cmd = self.oa.sendMessage(CODE_READ, ADDR_OFFSET, maxResp=4)
        offset = cmd[0]
        offset |= cmd[1] << 8
        offset |= cmd[2] << 16
        offset |= cmd[3] << 24
        return offset

    @setupSetParam("Pre-Trigger Samples")
    def setPresamples(self, samples):

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

    def presamples(self, cached=False):
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

    #@setupSetParam("Source")
    #def setSource(self,  src):
    #    return

    #def source(self):
    #    return "digital"

    @setupSetParam("Mode")
    def setMode(self,  mode):
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
            raise ValueError,  "%s invalid trigger mode"%mode

        cur = self.oa.settings() & ~(SETTINGS_TRIG_HIGH | SETTINGS_WAIT_YES)
        self.oa.setSettings(cur | trigmode)

    def mode(self):
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


class ClockSettings(Parameterized):
    _name = 'Clock Setup'
    readMask = [0x1f, 0xff, 0xff, 0xfd]

    def __init__(self, oaiface, hwinfo=None):
        self.oa = oaiface
        self._hwinfo = hwinfo
        self.params = Parameter(name=self.getName(), type='group')
        self.params.addChildren([
            {'name':'Refresh Status', 'type':'action', 'linked':[('ADC Clock', 'DCM Locked'), ('ADC Clock', 'ADC Freq'), ('CLKGEN Settings', 'DCM Locked'), 'Freq Counter'],
                     'help':'%namehdr%' +
                            'Update if the Digital Clock Manager (DCM) are "locked" and their operating frequency.'},
            {'name':'Reset DCMs', 'type':'action', 'action':self.resetDcms, 'linked':[('CLKGEN Settings', 'Multiply'), ('CLKGEN Settings', 'Divide')],
                      'help':'%namehdr%' +
                            'When the input frequency to the DCM blocks changes, it can cause them to become "unlocked". When they are "unlocked" they are NOT ' +
                            'generating a reliable output frequency. You must press the "Reset" button to cause them to re-lock. This is currently not automatically ' +
                            'done as during regular operation they shouldn\'t become unlocked.\n\nHowever every time you change the DCM block source, it will cause ' +
                            'the blocks to lose lock.'},

            {'name': 'ADC Clock', 'type':'group', 'children': [
                {'name': 'Source', 'type':'list', 'values':{"EXTCLK Direct":("extclk", 4, "clkgen"),
                                                            "EXTCLK x4 via DCM":("dcm", 4, "extclk"),
                                                            "EXTCLK x1 via DCM":("dcm", 1, "extclk"),
                                                            "CLKGEN x4 via DCM":("dcm", 4, "clkgen"),
                                                            "CLKGEN x1 via DCM":("dcm", 1, "clkgen")},
                          'set':self.setAdcSource, 'get':self.adcSource,
                          'help':'%namehdr%' +
                                'The ADC sample clock is generated from this source. Options are either an external input (which input set elsewhere) or an internal clock generator. Details of each option:\n\n' +
                                '=================== ====================== =================== ===============\n' +
                                ' Name                Description            Input Freq Range   Fine Phase Adj.\n' +
                                '=================== ====================== =================== ===============\n' +
                                ' EXCLK Direct       Connects sample clock     1-105 MHz            NO\n' +
                                '                    external pin directly.\n' +
                                ' EXTCLK xN via DCM  Takes external pin,       5-105 MHz (x1)       YES\n\n' +
                                '                    multiplies frequency      5-26.25 MHz (x4)        \n\n' +
                                '                    xN and feeds to ADC.  \n' + 
                                ' CLKGEN xN via DCM  Multiples CLKGEN by       5-105 MHz (x1)       YES\n\n' +
                                '                    xN and feeds to ADC.      5-26.25 MHz (x4)        \n\n' +
                                '=================== ====================== =================== ===============\n'},
                {'name': 'Phase Adjust', 'type':'int', 'limits':(-255, 255), 'set':self.setPhase, 'get':self.phase, 'help':'%namehdr%' +
                         'Makes small amount of adjustment to sampling point compared to the clock source. This can be used to improve the stability ' +
                         'of the measurement. Total phase adjustment range is < 5nS regardless of input frequency.'},
                {'name': 'ADC Freq', 'type': 'int', 'siPrefix':True, 'suffix': 'Hz', 'readonly':True, 'get':self.adcFrequency},
                {'name': 'DCM Locked', 'type':'bool', 'get':self.dcmADCLocked, 'readonly':True},
                {'name':'Reset ADC DCM', 'type':'action', 'action':lambda _ : self.resetDcms(True, False), 'linked':['Phase Adjust']},
            ]},
            {'name': 'Freq Counter', 'type': 'float', 'readonly':True, 'get':self.extFrequency, 'siPrefix':True, 'suffix':'Hz'},
            {'name': 'Freq Counter Src', 'type':'list', 'values':{'EXTCLK Input':0, 'CLKGEN Output':1}, 'set':self.setFreqSrc, 'get':self.freqSrc},
            {'name': 'CLKGEN Settings', 'type':'group', 'children': [
                {'name':'Input Source', 'type':'list', 'values':["system", "extclk"], 'set':self.setClkgenSrc, 'get':self.clkgenSrc},
                {'name':'Multiply', 'type':'int', 'limits':(2, 256), "default":2, 'set':self.setClkgenMul, 'get':self.clkgenMul, 'linked':['Current Frequency']},
                {'name':'Divide', 'type':'int', 'limits':(1, 256), 'set':self.setClkgenDiv, 'get':self.clkgenDiv, 'linked':['Current Frequency']},
                {'name':'Desired Frequency', 'type':'float', 'limits':(3.3E6, 300E6), 'default':0, 'step':1E6, 'siPrefix':True, 'suffix':'Hz',
                                            'set':self.autoMulDiv, 'get':self.getClkgen, 'linked':['Multiply', 'Divide']},
                {'name':'Current Frequency', 'type':'str', 'default':0, 'readonly':True,
                                            'get':self._getClkgenStr},
                {'name':'DCM Locked', 'type':'bool', 'default':False, 'get':self.clkgenLocked, 'readonly':True},
                {'name':'Reset CLKGEN DCM', 'type':'action', 'action':lambda _ : self.resetDcms(False, True), 'linked':['Multiply', 'Divide']},
            ]}
        ])
        self.params.refreshAllParameters()

    @setupSetParam("Freq Counter Src")
    def setFreqSrc(self, src):
        result = self.oa.sendMessage(CODE_READ, ADDR_ADVCLK, maxResp=4)
        result[3] &= ~0x08
        result[3] |= src << 3
        #print "%x"%result[3]
        self.oa.sendMessage(CODE_WRITE, ADDR_ADVCLK, result, readMask=self.readMask)

    def freqSrc(self):
        if self.oa is None:
            return 0
        result = self.oa.sendMessage(CODE_READ, ADDR_ADVCLK, maxResp=4)
        return (result[3] & 0x08) >> 3

    def _getClkgenStr(self):
        return str(self.getClkgen()) + " Hz"

    def getClkgen(self):
        return (self._hwinfo.sysFrequency() * self.clkgenMul()) / self.clkgenDiv()

    @setupSetParam(['CLKGEN Settings', 'Desired Frequency'])
    def autoMulDiv(self, freq):
        inpfreq = self._hwinfo.sysFrequency()
        sets = self.calculateClkGenMulDiv(freq, inpfreq)
        self.setClkgenMul(sets[0])
        self.setClkgenDiv(sets[1])
        self.resetDcms(False, True)

    def calculateClkGenMulDiv(self, freq, inpfreq=30E6):
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

    @setupSetParam(['CLKGEN Settings', 'Multiply'])
    def setClkgenMul(self, mul):
        if mul < 2:
            mul = 2
        self._setClkgenMul(mul)

    def _setClkgenMul(self, mul):
        result = self.oa.sendMessage(CODE_READ, ADDR_ADVCLK, maxResp=4)
        mul -= 1
        result[1] = mul
        result[3] |= 0x01
        self.oa.sendMessage(CODE_WRITE, ADDR_ADVCLK, result, readMask=self.readMask)
        result[3] &= ~(0x01)
        self.oa.sendMessage(CODE_WRITE, ADDR_ADVCLK, result, readMask=self.readMask)

    def clkgenMul(self):
        timeout = 2
        while timeout > 0:
            result = self.oa.sendMessage(CODE_READ, ADDR_ADVCLK, maxResp=4)
            val =  result[1]
            if val==0:
                val = 1  # Fix incorrect initialization on FPGA
                self._setClkgenMul(2)
            val += 1

            if (result[3] & 0x02):
                return val

            self.clkgenLoad()

            timeout -= 1

        # raise IOError("clkgen never loaded value?")
        return 0

    @setupSetParam(['CLKGEN Settings', 'Divide'])
    def setClkgenDiv(self, div):
        if div < 1:
            div = 1

        result = self.oa.sendMessage(CODE_READ, ADDR_ADVCLK, maxResp=4)
        div -= 1
        result[2] = div
        result[3] |= 0x01
        self.oa.sendMessage(CODE_WRITE, ADDR_ADVCLK, result, readMask=self.readMask)
        result[3] &= ~(0x01)
        self.oa.sendMessage(CODE_WRITE, ADDR_ADVCLK, result, readMask=self.readMask)

    def clkgenLoad(self):
        result = self.oa.sendMessage(CODE_READ, ADDR_ADVCLK, maxResp=4)
        result[3] |= 0x01
        self.oa.sendMessage(CODE_WRITE, ADDR_ADVCLK, result, readMask=self.readMask)
        result[3] &= ~(0x01)
        self.oa.sendMessage(CODE_WRITE, ADDR_ADVCLK, result, readMask=self.readMask)

    def clkgenDiv(self):
        if self.oa is None:
            return 2
        timeout = 2
        while timeout > 0:
            result = self.oa.sendMessage(CODE_READ, ADDR_ADVCLK, maxResp=4)
            val =  result[2]
            val += 1

            if (result[3] & 0x02):
                # Done loading value yet
                return val

            self.clkgenLoad()

            timeout -= 1

        logging.error("CLKGEN Failed to load divider value. Most likely clock input to CLKGEN is stopped, check CLKGEN"
                      " source settings. CLKGEN clock results are currently invalid.")
        return 1

    def adcSource(self):
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

    @setupSetParam(['ADC Clock', 'Source'])
    def setAdcSource(self, source="dcm", dcmout=4, dcminput="clkgen"):

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

        self.oa.sendMessage(CODE_WRITE, ADDR_ADVCLK, result)

    @setupSetParam(['CLKGEN Settings', 'Input Source'])
    def setClkgenSrc(self, source="system"):
        result = self.oa.sendMessage(CODE_READ, ADDR_ADVCLK, maxResp=4)

        result[0] = result[0] & ~0x08

        if source == "system":
            pass
        elif source == "extclk":
            result[0] = result[0] | 0x08
        else:
            raise ValueError("source must be 'system' or 'extclk'")

        self.oa.sendMessage(CODE_WRITE, ADDR_ADVCLK, result, readMask=self.readMask)

    def clkgenSrc(self):
        if self.oa is not None and self.oa.sendMessage(CODE_READ, ADDR_ADVCLK, maxResp=4)[0] & 0x08:
            return "extclk"
        else:
            return "system"

    @setupSetParam(['ADC Clock', 'Phase Adjust'])
    def setPhase(self, phase):
        '''Set the phase adjust, range -255 to 255'''

        LSB = phase & 0x00FF
        MSB = (phase & 0x0100) >> 8

        cmd = bytearray(2)
        cmd[0] = LSB
        cmd[1] = MSB | 0x02
        self.oa.sendMessage(CODE_WRITE, ADDR_PHASE, cmd, False)

    def phase(self):
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

    def dcmADCLocked(self):
        result = self.DCMStatus()
        return result[0]

    def clkgenLocked(self):
        result = self.DCMStatus()
        return result[1]

    def DCMStatus(self):
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

    def resetDcms(self, resetMain=True, resetClkgen=True):
        result = self.oa.sendMessage(CODE_READ, ADDR_ADVCLK, maxResp=4)

        #Set reset high on requested blocks only
        if resetMain:
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
            self.clkgenLoad()

    def extFrequency(self):
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
        return long(measured)

    def adcFrequency(self):
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

        return long(measured)


class OpenADCInterface(object):

    def __init__(self, serial_instance):
        self.serial = serial_instance
        self.offset = 0.5
        self.ddrMode = False
        self.sysFreq = 0
        self._streammode = False
        self._sbuf = []
        self.settings()

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
            testData = bytearray(range(250) + range(250)) #bytearray(random.randint(0,255) for r in xrange(500))
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
                    logging.warning('Timeout in read: %d' % len(result))
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
        time.sleep(0.01)
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
        self._sbuf = [0] * bufsizebytes

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

    def capture(self):
        timeout = False

        if self._streammode:

            # Wait for a trigger, letting the UI run when it can
            starttime = datetime.datetime.now()
            while self.serial.cmdReadStream_isDone() == False:
                # Wait for a moment before re-running the loop
                time.sleep(0.05)
                diff = datetime.datetime.now() - starttime

                # If we've timed out, don't wait any longer for a trigger
                if (diff.total_seconds() > self._timeout):
                    logging.warning('Timeout in OpenADC capture(), trigger FORCED')
                    timeout = True
                    self.triggerNow()
                    util.updateUI()
                    break

                # Give the UI a chance to update (does nothing if not using UI)
                util.updateUI()

            _, self._stream_rx_bytes = self.serial.cmdReadStream()
            timeout |= self.serial.streamModeCaptureStream.timeout
            #Check the status now
            bytes_left, overflow_bytes_left, unknown_overflow = self.serial.cmdReadStream_getStatus()
            logging.debug("Streaming done, results: rx_bytes = %d, bytes_left = %d, overflow_bytes_left = %d"%(self._stream_rx_bytes, bytes_left, overflow_bytes_left))
            self.arm(False)

            if overflow_bytes_left == (self._stream_len - 3072):
                logging.warning("Streaming mode OVERFLOW occured as trigger too fast - Adjust offset upward (suggest = 200 000)")
                timeout = True
            elif unknown_overflow:
                logging.warning("Streaming mode OVERFLOW occured during capture - ADC sample clock probably too fast for stream mode (keep ADC Freq < 10 MHz)")
                timeout = True
        else:
            status = self.getStatus()
            starttime = datetime.datetime.now()

            # Wait for a trigger, letting the UI run when it can
            while ((status & STATUS_ARM_MASK) == STATUS_ARM_MASK) | ((status & STATUS_FIFO_MASK) == 0):
                status = self.getStatus()

                # Wait for a moment before re-running the loop
                time.sleep(0.05)
                diff = datetime.datetime.now() - starttime

                # If we've timed out, don't wait any longer for a trigger
                if (diff.total_seconds() > self._timeout):
                    logging.warning('Timeout in OpenADC capture(), trigger FORCED')
                    timeout = True
                    self.triggerNow()

                # Give the UI a chance to update (does nothing if not using UI)
                util.updateUI()

            self.arm(False)

            # If using large offsets, system doesn't know we are delaying api
            nosampletimeout = 100
            while (self.getBytesInFifo() == 0) and nosampletimeout:
                time.sleep(0.05)
                nosampletimeout -= 1

            if nosampletimeout == 0:
                logging.warning('No samples received. Either very long offset, or no ADC clock (try "Reset ADC DCM"). '
                                'If you need such a long offset, manually update "nosampletimeout" limit in source code.')
                timeout = True

        return timeout

    def flush(self):
        # Flush output FIFO
        self.sendMessage(CODE_READ, ADDR_ADCDATA, None, False, None)

    def readData(self, NumberPoints=None, progressDialog=None):

        if self._streammode:
            # Process data
            bsize = self.serial.cmdReadStream_size_of_fpgablock()

            data = [0] * self.serial.cmdReadStream_bufferSize(self._stream_len)[0]
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

            logging.debug("Stream mode: done, %d bytes ready for processing"%len(data))
            datapoints = self.processData(data, 0.0)
            if datapoints:
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

                if data:
                    datapoints = datapoints + self.processData(data, 0.0)

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

    def processData(self, data, pad=float('NaN'), pretrigger_out=None):
        fpData = []
        lastpt = -100

        if data[0] != 0xAC:
            logging.warning('Unexpected sync byte: 0x%x' % data[0])
            return None

        trigfound = False
        trigsamp = 0

        for i in range(1, len(data) - 3, 4):
            #Convert
            temppt = (data[i + 3]<<0) | (data[i + 2]<<8) | (data[i + 1]<<16) | (data[i + 0]<<24)

            #print("%2x "%data[i])

            #print "%x %x %x %x"%(data[i +0], data[i +1], data[i +2], data[i +3]);
            #print "%x"%temppt

            intpt1 = temppt & 0x3FF
            intpt2 = (temppt >> 10) & 0x3FF
            intpt3 = (temppt >> 20) & 0x3FF

            # print "%x %x %x" % (intpt1, intpt2, intpt3)

            if trigfound == False:
                mergpt = temppt >> 30
                if (mergpt != 3):
                       trigfound = True
                       trigsamp = trigsamp + mergpt
                       #print "Trigger found at %d"%trigsamp
                else:
                   trigsamp += 3

            #input validation test: uncomment following and use
            #ramp input on FPGA
            ##if (intpt != lastpt + 1) and (lastpt != 0x3ff):
            ##    print "intpt: %x lstpt %x\n"%(intpt, lastpt)
            ##lastpt = intpt;

            #print "%x %x %x"%(intpt1, intpt2, intpt3)

            fpData.append(float(intpt1) / 1024.0 - self.offset)
            fpData.append(float(intpt2) / 1024.0 - self.offset)
            fpData.append(float(intpt3) / 1024.0 - self.offset)

        #print len(fpData)

        if trigfound == False:
            logging.warning('Trigger not found in ADC data. No data reported!')

        #Ensure that the trigger point matches the requested by padding/chopping
        diff = self.presamples_desired - trigsamp
        if diff > 0:
               fpData = [pad]*diff + fpData
               logging.warning('Pretrigger not met. Increase presampleTempMargin (in the code).')
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
        print "Could not open %s" % ser.name
        sys.exit()
    except ValueError as s:
        print "Invalid settings for serial port"
        ser.close()
        ser = None
        sys.exit()

    adc = OpenADCInterface(ser)
    adc.devicePresent()

    adc_settings = OpenADCSettings()
    adc_settings.setInterface(adc)
    adc_settings.parameters()

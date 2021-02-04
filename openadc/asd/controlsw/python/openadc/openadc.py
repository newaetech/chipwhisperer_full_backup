# -*- coding: cp1252 -*-

# This file is part of the OpenADC Project. See www.newae.com for more details,
# or the codebase at http://www.assembla.com/spaces/openadc .
#
# Copyright (c) 2012-2013, Colin O'Flynn <coflynn@newae.com>. All rights reserved.
# This project is released under the Modified FreeBSD License. See LICENSE
# file which should have came with this code.

import sys
import time
import datetime
from functools import partial

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

# sign extend b low bits in x
# from "Bit Twiddling Hacks"
def SIGNEXT(x, b):
    m = 1 << (b - 1)
    x = x & ((1 << b) - 1)
    return (x ^ m) - m

class BaseLog(object):
    def __init__(self, console=None):
        self.console = console

    def log(self, msg):
        if self.console:
            self.console.append(msg)
        else:
            print msg

class OpenADCSettings(BaseLog):
    def __init__(self, oaiface=None, console=None):
        super(OpenADCSettings, self).__init__(console)
        self.parm_hwinfo = HWInformation(console=console)
        self.parm_gain = GainSettings(console=console)
        self.parm_trigger = TriggerSettings(console=console)
        self.parm_clock = ClockSettings(console=console, hwinfo=self.parm_hwinfo)

        self.params = [
            self.parm_hwinfo,
            self.parm_gain,
            self.parm_trigger,
            self.parm_clock
            ]

        if oaiface is not None:
            self.setInterface(oaiface)

    def setFindParam(self, findParam):
        self.findParam = findParam

        for t in self.params:
            if hasattr(t, 'setFindParam'):
                t.setFindParam(findParam)

    def setInterface(self, oaiface):
        self.oa = oaiface
        for p in self.params:
            p.setInterface(oaiface)

    def parameters(self,  doUpdate=True):
        """Return a dict of all parameter/settings. Useful for saving."""
        paramdict = []
        for p in self.params:
            paramref = p.param
            paramdict.append(paramref)

        if doUpdate:
            for p in paramdict:
                for pset in p['children']:
                    try:
                        if 'children' in pset:
                            for subset in pset['children']:
                                if pset['type'] != 'action':
                                    subset['value']=subset['get']()
                        else:
                            if pset['type'] != 'action':
                                pset['value'] = pset['get']()
                    except KeyError:
                        print "Error in %s.%s - no 'get'"%(p['name'], pset['name'])

        return paramdict

    def setParameters(self, paramdict):
        """Set all parameters/settings from a dict. Can pass only part of the dictionary too for changes."""
        return

class HWInformation(BaseLog):
    def __init__(self, console=None):
        super(HWInformation, self).__init__(console)
        self.name = "Hardware Information"
        self.sysFreq = 0
        self.param = {'name': 'HW Information', 'type': 'group', 'children': [
                {'name': 'Version', 'type': 'str', 'value': "unknown", 'get':self.versions, 'readonly':True},
                {'name': 'Synth Date', 'type': 'str', 'value': "unknown", 'get':self.synthDate, 'readonly':True},
                {'name': 'System Freq', 'type': 'int', 'value': 0, 'siPrefix':True, 'suffix': 'Hz', 'get':self.sysFrequency, 'readonly':True},
                {'name': 'Max Samples', 'type': 'int', 'value': 0, 'get':self.maxSamples, 'readonly':True}
                ]}

        self.vers = None

    def setInterface(self, oa):
        self.oa = oa
        oa.hwInfo = self

    def versions(self):
        result = self.oa.sendMessage(CODE_READ, ADDR_VERSIONS, maxResp=6)

        regver = result[0] & 0xff
        hwtype = result[1] >> 3
        hwver = result[1] & 0x07
        hwList = ["Default/Unknown", "LX9 MicroBoard", "SASEBO-W", "ChipWhisperer Rev2 LX25",
                  "Reserved?", "ZedBoard", "Papilio Pro", "SAKURA-G", "ChipWhisperer Lite"]

        try:
            textType = hwList[hwtype]
        except:
            textType = "Invalid/Unknown"

        self.vers = (regver, hwtype, textType, hwver)

        #TODO: Temp fix for wrong HW reporting
        if hwtype == 1:
            self.sysFreq = 40E6

        return self.vers

    def synthDate(self):
        return "unknown"

    def maxSamples(self):
        return self.oa.hwMaxSamples

    def sysFrequency(self, force=False):
        if (self.sysFreq > 0) & (force == False):
            return self.sysFreq

        '''Return the system clock frequency in specific firmware version'''
        freq = 0x00000000;

        temp = self.oa.sendMessage(CODE_READ, ADDR_SYSFREQ, maxResp=4)
        freq = freq | (temp[0] << 0);
        freq = freq | (temp[1] << 8);
        freq = freq | (temp[2] << 16);
        freq = freq | (temp[3] << 24);

        self.sysFreq = long(freq)

        return self.sysFreq

class GainSettings(BaseLog):
    def __init__(self, console=None):
        super(GainSettings, self).__init__(console)
        self.name = "Gain Setting"
        self.param = {'name': 'Gain Setting', 'type': 'group', 'children': [
                {'name': 'Mode', 'type': 'list', 'values': {"high", "low"}, 'value':"low", 'set':self.setMode, 'get':self.mode,
                         'help': '%namehdr%'+
                                 'Sets the AD8331 Low Noise Amplifier into to "High" or "Low" gain mode. Low mode ranges from ' +
                                 '-4.5dB to +43.5dB, and High mode ranges from +7.5dB to +55.5dB. Better performance is found ' +
                                 'using the "High" gain mode typically.'},
                {'name': 'Setting', 'type': 'int', 'value':0, 'limits': (0, 78), 'set':self.setGain, 'get':self.gain, 'linked':['Result'],
                         'help':'%namehdr%'+
                                'Sets the AD8331 gain value. This is a unitless number which ranges from 0 (minimum) to 78 (maximum).' +
                                ' The resulting gain in dB is given in the "calculated" output.'},
                {'name': 'Result', 'type': 'float', 'suffix':'dB', 'readonly':True, 'get':self.gainDB,
                         'help':'%namehdr%'+
                                'Gives the gain the AD8331 should have, based on the "High/Low" setting and the "gain setting".'},
                ]}
        self.gainlow_cached = False
        self.gain_cached = 0

    def setInterface(self, oa):
        self.oa = oa

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
        return "low"

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

        gainV = (float(self.gain_cached) / 256.0) * 3.3;

        if self.gainlow_cached:
            gaindb = 50.0 * gainV - 6.5
        else:
            gaindb = 50.0 * gainV + 5.5

        return gaindb

class TriggerSettings(BaseLog):
    def __init__(self, console=None):
        super(TriggerSettings, self).__init__(console)
        self.name = "Trigger Settings"
        self.param = {'name': 'Trigger Setup', 'type':'group', 'children': [
            {'name': 'Refresh Status', 'type':'action', 'linked':['Digital Pin State'], 'visible':False,
                     'help':'%namehdr%'+
                            'Refreshes the "Digital Pin State" status.'},
            {'name': 'Source', 'type': 'list', 'values':["digital", "analog"], 'value':"digital", 'set':self.setSource, 'get':self.source,
                     'help':'%namehdr%'+
                            'Selects if trigger system is based on digital signal (including internally generated), or an ADC level. Currently ' +
                            'only the digital trigger system is supported.'},
            {'name': 'Digital Pin State', 'type':'bool', 'value':False, 'readonly':True, 'get':self.extTriggerPin,
                     'help':'%namehdr%'+
                            'Gives the status of the digital signal being used as the trigger signal, either high or low.'},
            {'name': 'Mode', 'type':'list', 'values':["rising edge", "falling edge", "low", "high"], 'value':'low', 'set':self.setMode, 'get':self.mode,
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
                            'Note the "Trigger Mode" should be set to "Rising Edge" if using either the "SAD Trigger" or "IO Trigger" modes.'
                            },
            {'name': 'Timeout (secs)', 'type':'float', 'value':2, 'step':1, 'limits':(0, 1E99), 'set':self.setTimeout, 'get':self.timeout,
                     'help':'%namehdr%'+
                            'If no trigger occurs in this many seconds, force the trigger.'},
            {'name': 'Offset', 'type':'int', 'value':0, 'limits':(0, 4294967294), 'set':self.setOffset, 'get':self.offset,
                     'help':'%namehdr%'+
                            'Delays this many samples after the trigger event before recording samples. Based on the ADC clock cycles. ' +
                            'If using a 4x mode for example, an offset of "1000" would mean we skip 250 cycles of the target device.'},
            {'name': 'Pre-Trigger Samples', 'type':'int', 'value':0, 'limits':(0, 1000000), 'set':self.setPresamples, 'get':self.presamples,
                     'help':'%namehdr%'+
                            'Record a certain number of samples before the main samples are captured. If "offset" is set to 0, this means ' +
                            'recording samples BEFORE the trigger event.'},
            {'name': 'Total Samples', 'type':'int', 'value':0, 'limits':(0, 1000000), 'set':self.setMaxSamples, 'get':self.maxSamples,
                     'help':'%namehdr%'+
                            'Total number of samples to record. Note the capture system has an upper limit, and may have a practical lower limit (i.e.,' +
                            ' if this value is set too low the system may not capture samples. Suggest to always set > 256 samples.'},
        ]}
        self.maxsamples = 0
        self.presamples_desired = 0
        self.presamples_actual = 0

        self.presampleTempMargin = 24
        self._timeout = 2


    def setInterface(self, oa):
        self.oa = oa
        self.oa.presamples_desired = self.presamples_desired
        if self.oa and hasattr(self.oa, 'setTimeout'):
            self.oa.setTimeout(self._timeout)

    def setMaxSamples(self, samples):
        self.maxsamples = samples
        self.oa.setMaxSamples(samples)

    def maxSamples(self,  cached=False):
        if cached:
            return self.maxsamples
        else:
            return self.oa.maxSamples()

    def setTimeout(self, timeout):
        self._timeout = timeout
        if self.oa:
            self.oa.setTimeout(timeout)

    def timeout(self):
        return self._timeout

    def setOffset(self,  offset):
        cmd = bytearray(4)
        cmd[0] = ((offset >> 0) & 0xFF)
        cmd[1] = ((offset >> 8) & 0xFF)
        cmd[2] = ((offset >> 16) & 0xFF)
        cmd[3] = ((offset >> 24) & 0xFF)
        self.oa.sendMessage(CODE_WRITE, ADDR_OFFSET, cmd)

    def offset(self):
        cmd = self.oa.sendMessage(CODE_READ, ADDR_OFFSET, maxResp=4)
        offset = cmd[0]
        offset |= cmd[1] << 8
        offset |= cmd[2] << 16
        offset |= cmd[3] << 24
        return offset


    def setPresamples(self, samples):
        #enforce samples is multiple of 3
        samplesact = int(samples / 3)

        #Account for shitty hardware design
        if samplesact > 0:
            samplesact = samplesact + self.presampleTempMargin

        cmd = bytearray(4)
        cmd[0] = ((samplesact >> 0) & 0xFF)
        cmd[1] = ((samplesact >> 8) & 0xFF)
        cmd[2] = ((samplesact >> 16) & 0xFF)
        cmd[3] = ((samplesact >> 24) & 0xFF)
        self.oa.sendMessage(CODE_WRITE, ADDR_PRESAMPLES, cmd)

        self.presamples_actual = samplesact*3
        self.presamples_desired = samples

        #print "Requested presamples: %d, actual: %d"%(samples, self.presamples_actual)

        self.oa.presamples_desired = samples

        return self.presamples_actual

    def presamples(self, cached=False):
        """If cached returns DESIRED presamples"""

        if cached:
            return self.presamples_desired

        samples = 0x00000000;

        temp = self.oa.sendMessage(CODE_READ, ADDR_PRESAMPLES, maxResp=4)
        samples = samples | (temp[0] << 0)
        samples = samples | (temp[1] << 8)
        samples = samples | (temp[2] << 16)
        samples = samples | (temp[3] << 24)

        self.presamples_actual = samples*3

        return samples*3

    def setSource(self,  src):
        return

    def source(self):
        return "digital"

    def setMode(self,  mode):
        """ Input to trigger module options: 'rising edge', 'falling edge', 'high', 'low' """
        if mode == 'rising edge':
            trigmode = SETTINGS_TRIG_HIGH | SETTINGS_WAIT_YES;

        elif mode == 'falling edge':
            trigmode = SETTINGS_TRIG_LOW | SETTINGS_WAIT_YES;

        elif mode == 'high':
            trigmode = SETTINGS_TRIG_HIGH | SETTINGS_WAIT_NO;

        elif mode == 'low':
            trigmode = SETTINGS_TRIG_LOW | SETTINGS_WAIT_NO;

        else:
            raise ValueError,  "%s invalid trigger mode"%mode

        cur = self.oa.settings() & ~(SETTINGS_TRIG_HIGH | SETTINGS_WAIT_YES);
        self.oa.setSettings(cur | trigmode)

    def mode(self):
        sets = self.oa.settings()
        case = sets & (SETTINGS_TRIG_HIGH | SETTINGS_WAIT_YES);

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
        sets = self.oa.getStatus()
        if sets & STATUS_EXT_MASK:
            return True
        else:
            return False

class ClockSettings(BaseLog):

    readMask = [0x1f, 0xff, 0xff, 0xfd]

    def __init__(self, console=None, hwinfo=None):
        super(ClockSettings, self).__init__(console)
        self.name = "Clock Setup"
        self.findParam = None
        self.param = {'name': 'Clock Setup', 'type':'group', 'children': [
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
                {'name': 'Source', 'type':'list', 'values':{"EXTCLK Direct":("extclk", 4, "clkgen"), "EXTCLK x4 via DCM":("dcm", 4, "extclk"), "EXTCLK x1 via DCM":("dcm", 1, "extclk"), "CLKGEN x4 via DCM":("dcm", 4, "clkgen"), "CLKGEN x1 via DCM":("dcm", 1, "clkgen")}, 'value':("dcm", 1, "extclk"), 'set':self.setAdcSource, 'get':self.adcSource,
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
                {'name': 'Phase Adjust', 'type':'int', 'value':0, 'limits':(-255, 255), 'set':self.setPhase, 'get':self.phase, 'help':'%namehdr%' +
                         'Makes small amount of adjustment to sampling point compared to the clock source. This can be used to improve the stability ' +
                         'of the measurement. Total phase adjustment range is < 5nS regardless of input frequency.'},
                {'name': 'ADC Freq', 'type': 'int', 'value': 0, 'siPrefix':True, 'suffix': 'Hz', 'readonly':True, 'get':self.adcFrequency},
                {'name': 'DCM Locked', 'type':'bool', 'value':False, 'get':self.dcmADCLocked, 'readonly':True},
                {'name':'Reset ADC DCM', 'type':'action', 'action':partial(self.resetDcms, True, False), 'linked':['Phase Adjust']},
            ]},
            {'name': 'Freq Counter', 'type': 'int', 'value': 0, 'siPrefix':True, 'suffix': 'Hz', 'readonly':True, 'get':self.extFrequency},
            {'name': 'Freq Counter Src', 'type':'list', 'values':{'EXTCLK Input':0, 'CLKGEN Output':1}, 'value':0, 'set':self.setFreqSrc, 'get':self.freqSrc},
            {'name': 'CLKGEN Settings', 'type':'group', 'children': [
                {'name':'Input Source', 'type':'list', 'values':["system", "extclk"], 'value':"system", 'set':self.setClkgenSrc, 'get':self.clkgenSrc},
                {'name':'Multiply', 'type':'int', 'limits':(2, 256), 'value':2, 'set':self.setClkgenMul, 'get':self.clkgenMul},
                {'name':'Divide', 'type':'int', 'limits':(1, 256), 'value':2, 'set':self.setClkgenDiv, 'get':self.clkgenDiv},
                {'name':'DCM Locked', 'type':'bool', 'value':False, 'get':self.clkgenLocked, 'readonly':True},
                {'name':'Reset CLKGEN DCM', 'type':'action', 'action':partial(self.resetDcms, False, True), 'linked':['Multiply', 'Divide']},
                {'name':'Desired Frequency', 'type':'float', 'limits':(3.3E6, 200E6), 'value':0, 'step':100E6, 'siPrefix':True, 'suffix':'Hz',
                                            'set':self.autoMulDiv, 'linked':['Multiply', 'Divide']},
            ]},
        ]}

        self._hwinfo = hwinfo

    def setInterface(self, oa):
        self.oa = oa

    def setFindParam(self, fp):
        self.findParam = fp

    def setFreqSrc(self, src):
        result = self.oa.sendMessage(CODE_READ, ADDR_ADVCLK, maxResp=4)
        result[3] = result[3] & ~(0x08)
        result[3] |= src << 3
        #print "%x"%result[3]
        self.oa.sendMessage(CODE_WRITE, ADDR_ADVCLK, result, readMask=self.readMask)

    def freqSrc(self):
        result = self.oa.sendMessage(CODE_READ, ADDR_ADVCLK, maxResp=4)
        return ((result[3] & 0x08) >> 3)

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

    def setClkgenMul(self, mul):
        if mul < 2:
            mul = 2

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
            val += 1

            if (result[3] & 0x02):
                return val

            self.clkgenLoad()

            timeout -= 1

        # raise IOError("clkgen never loaded value?")
        return 0

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

        #raise IOError("clkgen never loaded value?")
        return 0


    def adcSource(self):
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
        result = self.oa.sendMessage(CODE_READ, ADDR_ADVCLK, maxResp=4)
        if result[0] & 0x08:
            return "extclk"
        else:
            return "system"

    def setPhase(self, phase):
        '''Set the phase adjust, range -255 to 255'''

        LSB = phase & 0x00FF;
        MSB = (phase & 0x0100) >> 8;

        cmd = bytearray(2)
        cmd[0] = LSB;
        cmd[1] = MSB | 0x02;
        self.oa.sendMessage(CODE_WRITE, ADDR_PHASE, cmd, False)

    def phase(self):
        result = self.oa.sendMessage(CODE_READ, ADDR_PHASE, maxResp=2);

        if (result[1] & 0x02):
            LSB = result[0]
            MSB = result[1] & 0x01

            phase = LSB | (MSB << 8)

            #Sign Extend
            phase = SIGNEXT(phase, 9)

            return phase
        else:
            self.log("No phase shift loaded")
            return 0

    def dcmADCLocked(self):
        result = self.DCMStatus()
        return result[0]

    def clkgenLocked(self):
        result = self.DCMStatus()
        return result[1]

    def DCMStatus(self):
        result = self.oa.sendMessage(CODE_READ, ADDR_ADVCLK, maxResp=4)
        if (result[0] & 0x80) == 0:
            self.log("ERROR: ADVCLK register not present. Version mismatch")
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
        freq = 0x00000000;

        #Get sample frequency
        samplefreq = float(self.oa.hwInfo.sysFrequency()) / float(pow(2,23))

        temp = self.oa.sendMessage(CODE_READ, ADDR_FREQ, maxResp=4)
        freq = freq | (temp[0] << 0);
        freq = freq | (temp[1] << 8);
        freq = freq | (temp[2] << 16);
        freq = freq | (temp[3] << 24);

        measured = freq * samplefreq
        return long(measured)

    def adcFrequency(self):
        """Return the external frequency measured on 'CLOCK' pin. Returned value
           is in Hz"""
        freq = 0x00000000;

        #Get sample frequency
        samplefreq = float(self.oa.hwInfo.sysFrequency()) / float(pow(2,23))

        temp = self.oa.sendMessage(CODE_READ, ADDR_ADCFREQ, maxResp=4)
        freq = freq | (temp[0] << 0);
        freq = freq | (temp[1] << 8);
        freq = freq | (temp[2] << 16);
        freq = freq | (temp[3] << 24);

        measured = freq * samplefreq

        return long(measured)


class OpenADCInterface(BaseLog):
    def __init__(self, serial_instance, debug=None, console=None):
        super(OpenADCInterface, self).__init__(console)
        self.serial = serial_instance
        self.offset = 0.5
        self.ddrMode = False
        self.sysFreq = 0

        self.settings();

        #self.params = OpenADCSettings(self)

        # Send clearing function if using streaming mode
        if hasattr(self.serial, "stream") and self.serial.stream == False:
            pass
        else:
            nullmessage = bytearray([0] * 20)
            self.serial.write(str(nullmessage));

        self.setReset(True)
        self.setReset(False)

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

               self.log("%d errors in %d"%(totalerror, totalbytes))


    def sendMessage(self, mode, address, payload=None, Validate=True, maxResp=None, readMask=None):
        """Send a message out the serial port"""

        if payload is None:
            payload = []

        #Get length
        length = len(payload)

        if ((mode == CODE_WRITE) and (length < 1)) or ((mode == CODE_READ) and (length != 0)):
            self.log("Invalid payload for mode")
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
                        errmsg += "\n"
                        errmsg += "  Read data: "
                        if check:
                            for c in check: errmsg += "%02x" % c
                            errmsg += "\n"
                        else:
                            errmsg += "<Timeout>"

                        self.log(errmsg)

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
                    self.log("Timeout in read: %d" % len(result))
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
                        errmsg += "\n"
                        errmsg += "  Read data: "
                        if check:
                            for c in check: errmsg += "%02x" % c
                            errmsg += "\n"
                        else:
                            errmsg += "<Timeout>"

                        self.log(errmsg)

### Generic
    def setSettings(self, state, validate=True):
        cmd = bytearray(1)
        cmd[0] = state
        self.sendMessage(CODE_WRITE, ADDR_SETTINGS, cmd, Validate=validate);

    def settings(self):
        sets = self.sendMessage(CODE_READ, ADDR_SETTINGS)
        if sets:
            return sets[0]
        else:
            return 0

    def setReset(self, value):
        if value:
            self.setSettings(self.settings() | SETTINGS_RESET, validate=False);
            self.hwMaxSamples = self.maxSamples()
        else:
            self.setSettings(self.settings() & ~SETTINGS_RESET);

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

    def setMaxSamples(self, samples):
        cmd = bytearray(4)
        cmd[0] = ((samples >> 0) & 0xFF)
        cmd[1] = ((samples >> 8) & 0xFF)
        cmd[2] = ((samples >> 16) & 0xFF)
        cmd[3] = ((samples >> 24) & 0xFF)
        self.sendMessage(CODE_WRITE, ADDR_SAMPLES, cmd)

    def maxSamples(self):
        '''Return the number of samples captured in one go'''
        samples = 0x00000000;
        temp = self.sendMessage(CODE_READ, ADDR_SAMPLES, maxResp=4)
        samples = samples | (temp[0] << 0)
        samples = samples | (temp[1] << 8)
        samples = samples | (temp[2] << 16)
        samples = samples | (temp[3] << 24)
        return samples

    def getBytesInFifo(self):
        samples = 0
        temp = self.sendMessage(CODE_READ, ADDR_BYTESTORX, maxResp=4)

        samples = samples | (temp[0] << 0)
        samples = samples | (temp[1] << 8)
        samples = samples | (temp[2] << 16)
        samples = samples | (temp[3] << 24)
        return samples

    def flushInput(self):
        try:
            self.serial.flushInput()
        except AttributeError:
            return

    def devicePresent(self):
        msgin = bytearray([])
        msgin.append(0xAC);

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

    def setDDRAddress(self, addr):
        cmd = bytearray(1)
        cmd[0] = ((addr >> 0) & 0xFF)
        self.sendMessage(CODE_WRITE, ADDR_DDR1, cmd)
        cmd[0] = ((addr >> 8) & 0xFF)
        self.sendMessage(CODE_WRITE, ADDR_DDR2, cmd)
        cmd[0] = ((addr >> 16) & 0xFF)
        self.sendMessage(CODE_WRITE, ADDR_DDR3, cmd)
        cmd[0] = ((addr >> 24) & 0xFF)
        self.sendMessage(CODE_WRITE, ADDR_DDR4, cmd)

    def getDDRAddress(self):
        addr = 0x00000000;
        temp = self.sendMessage(CODE_READ, ADDR_DDR1)
        addr = addr | (temp[0] << 0);
        temp = self.sendMessage(CODE_READ, ADDR_DDR2)
        addr = addr | (temp[0] << 8);
        temp = self.sendMessage(CODE_READ, ADDR_DDR3)
        addr = addr | (temp[0] << 16);
        temp = self.sendMessage(CODE_READ, ADDR_DDR4)
        addr = addr | (temp[0] << 24);
        return addr

    def arm(self):
        self.setSettings(self.settings() | SETTINGS_ARM);

    def capture(self, waitingCallback=None):
        #Wait for trigger
        timeout = False

        status = self.getStatus()

        starttime = datetime.datetime.now()

        while ((status & STATUS_ARM_MASK) == STATUS_ARM_MASK) | ((status & STATUS_FIFO_MASK) == 0):
            status = self.getStatus()
            time.sleep(0.05)

            diff = datetime.datetime.now() - starttime

            if (diff.total_seconds() > self._timeout):
                self.log("Timeout in OpenADC capture(), trigger FORCED")
                timeout = True
                self.triggerNow()

            if waitingCallback:
                waitingCallback()

        self.setSettings(self.settings() & ~SETTINGS_ARM);
        
        # If using large offsets, system doesn't know we are delaying capture        
        nosampletimeout = 100
        while (self.getBytesInFifo() == 0) and nosampletimeout:
            time.sleep(0.05)
            nosampletimeout -= 1

        if nosampletimeout == 0:
            print "WARNING: No samples received from ADC. Either very long offset, or no ADC clock."
            print "         If you need such a long offset, manually update 'nosampletimeout' limit in source code."
        
        return timeout

    def flush(self):
        # Flush output FIFO
        self.sendMessage(CODE_READ, ADDR_ADCDATA, None, False, None)

    def readData(self, NumberPoints=None, progressDialog=None):
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

            # +1 for sync byte
            data = self.sendMessage(CODE_READ, ADDR_ADCDATA, None, False, bytesToRead + 1)  # BytesPerPackage)

            # for p in data:
            #       print "%x "%p,

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
        lastpt = -100;

        if data[0] != 0xAC:
            self.log("Unexpected sync byte: 0x%x"%data[0])
            return None

        trigfound = False
        trigsamp = 0

        for i in range(1, len(data) - 3, 4):
            #Convert
            temppt = (data[i + 3]<<0) | (data[i + 2]<<8) | (data[i + 1]<<16) | (data[i + 0]<<24)

            #print("%2x "%data[i])

            #print "%x %x %x %x"%(data[i +0], data[i +1], data[i +2], data[i +3]);
            #print "%x"%temppt

            intpt1 = temppt & 0x3FF;
            intpt2 = (temppt >> 10) & 0x3FF;
            intpt3 = (temppt >> 20) & 0x3FF;

            # print "%x %x %x" % (intpt1, intpt2, intpt3)

            if trigfound == False:
                mergpt = temppt >> 30;
                if (mergpt != 3):
                       trigfound = True
                       trigsamp = trigsamp + mergpt
                       # print "Trigger found at %d"%trigsamp
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

        #Ensure that the trigger point matches the requested by padding/chopping
        diff = self.presamples_desired - trigsamp
        if diff > 0:
               fpData = [pad]*diff + fpData
               print "WARNING: Pretrigger not met. Increase presampleTempMargin."
        else:
               fpData = fpData[-diff:]

        return fpData

if __name__ == "__main__":
    import serial

    ser = serial.Serial()
    ser.port     = "com6"
    ser.baudrate = 512000
    ser.timeout  = 1.0

    try:
        ser.open()
    except serial.SerialException, e:
        print "Could not open %s"%ser.name
        sys.exit()
    except ValueError, s:
        print "Invalid settings for serial port"
        ser.close()
        ser = None
        sys.exit()

    adc = OpenADCInterface(ser)
    adc.devicePresent()

    adc_settings = OpenADCSettings()
    adc_settings.setInterface(adc)
    adc_settings.parameters()

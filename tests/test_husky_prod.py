# -*- coding: utf-8 -*-
#
# Copyright (c) 2025, NewAE Technology Inc
# All rights reserved.
#
# Find this and more at newae.com - this file is part of the chipwhisperer
# project, http://www.chipwhisperer.com . ChipWhisperer is a registered
# trademark of NewAE Technology Inc in the US & Europe.
#
#    This file is part of chipwhisperer.
#
#    Licensed under the Apache License, Version 2.0 (the "License");
#    you may not use this file except in compliance with the License.
#    You may obtain a copy of the License at
#
#       http://www.apache.org/licenses/LICENSE-2.0
#
#    Unless required by applicable law or agreed to in writing, software
#    distributed under the License is distributed on an "AS IS" BASIS,
#    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#    See the License for the specific language governing permissions and
#    limitations under the License.
#==========================================================================

# Run with pytest -v -rs

import chipwhisperer as cw
import pytest
import time
import numpy as np
import random
import os

from test_common import *

""" 
Args:
    stress: run more iterations of the tests that stess temperature and voltage rails

"""

print('\n\n\n\n**************************************************************************************')
print('* NOTE: This script is intended for production testing of Husky.                     *')
print('* A SAM4S target should be attached, otherwise some important tests will be skipped. *')
print('* Runs in about 90 seconds.                                                          *')
print('* Use --stress to run extra iterations of tests which drive the temperature higher;  *')
print('* this increases runtime to about 5 minutes.                                         *')
print('**************************************************************************************\n\n')

# default to sam4s:
test_platform = "sam4s"
#test_platform = "stm32f3" (not supported by this script)
logfilename = "test_husky_prod_xadc.log"

if "HUSKY_HW_LOC" in os.environ:
    locboth = os.environ["HUSKY_HW_LOC"].split(',')
    loca = int(locboth[0].replace('(', ''))
    locb = int(locboth[1].replace(')', ''))
    hw_loc = (loca, locb)
    print("hw_loc added {}".format(hw_loc))
else:
    hw_loc = None

if "HUSKY_TARGET_PLATFORM" in os.environ:
    test_platform = os.environ["HUSKY_TARGET_PLATFORM"]

print("Husky target platform {}".format(test_platform))
scope = cw.scope(hw_location=hw_loc)
target = cw.target(scope)
scope.errors.clear()
verbose = False
cw.scope_logger.setLevel(cw.logging.ERROR) # don't want to see warnings when setting clock past its specifications

scope.XADC._user_reset() # reset max/min stats
scope.sc.reset_fpga()
scope.adc.clip_errors_disabled = True
scope.adc.lo_gain_errors_disabled = True
scope.clock.clkgen_freq = 10e6
scope.clock.clkgen_src = 'system'
scope.clock.adc_mul = 1
time.sleep(0.1)
assert scope.clock.pll.pll_locked == True, 'Unexpected clock-setting problem.'
assert scope.clock.adc_freq == 10e6, 'Not getting expected ADC clock frequency.'
target.baud = 38400 * 10 / 7.37

if scope._is_husky_plus:
    MAXCLOCK = 250e6
    OVERCLOCK1 = 255e6
    OVERCLOCK2 = 280e6
    MAXSAMPLES = 327828
    MAXSEGMENTSAMPLES = 295056
else:
    MAXCLOCK = 200e6
    OVERCLOCK1 = 210e6
    OVERCLOCK2 = 250e6
    MAXSAMPLES = 131124
    MAXSEGMENTSAMPLES = 98352

reset_setup(scope,target)

def correct_fw():
    reset_target(scope)
    target.flush()
    target.write('x\n')
    time.sleep(0.2)
    resp = target.read()
    target.simpleserial_write('i', b'')
    time.sleep(0.1)
    response = target.read().split('\n')[0]
    if response == 'ChipWhisperer simpleserial-trace, compiled Sep  2 2022, 13:55:43':
        return True
    else:
        return False

# Check firmware and program if needed.
# Here, what we would like to do is something pytest doesn't make easy (or possible?), so this is hacky/not ideal for 
# it would be great to have a command-line switch disable target FW checking (to save time when no target attached), but pytest doesn't make this easy:
target_attached = False
if correct_fw():
    target_attached = True
else:
    try:
        print('Incorrect firmware detected; programming...')
        prog = cw.programmers.SAM4SProgrammer
        cw.program_target(scope, prog, "../firmware/mcu/simpleserial-trace/simpleserial-trace-CW308_SAM4S.hex")
        if correct_fw():
            target_attached = True
    except Exception as e:
        print('***** WARNING: unable to program firmware (is there a SAM4S target attached?): %s' % e)
        print('***** Many tests will not be able to run. Attach a SAM4S target to prevent this.')
        target_attached = False

ktp = cw.ktp.Basic()
key, text = ktp.next()


testData = [
    # samples   presamples  testmode    clock       fastreads   adcmul  bit stream  segs    segcycs reps    desc
    ('max',     0,          'internal', 20e6,       True,       1,      12, False,  1,      0,      1,      'maxsamples12'),
    ('max',     0,          'internal', 'max',      True,       1,      12, False,  1,      0,      1,      'fastest'),
    ('max',     0,          'internal', 'over2',    True,       1,      12, False,  1,      0,      1,      'overclocked'),
    ('max',     0,          'internal', 50e6,       True,       4,      12, False,  1,      0,      1,      '4xfast'),
    ('max',     0,          'ADCramp',  20e6,       True,       1,      12, False,  1,      0,      1,      'ADCslow'),
    ('max',     0,          'ADCramp',  'max',      True,       1,      12, False,  1,      0,      3,      'ADCfast'),
    ('max',     0,          'ADCramp',  'over2',    True,       1,      12, False,  1,      0,      1,      'ADCoverclocked'),
    ('max',     0,          'ADCalt',   'max',      True,       1,      12, False,  1,      0,      3,      'ADCaltfast')
]

testTargetData = [
    # samples   presamples  testmode    clock       fastreads   adcmul  bit stream  threshold   seg_size,   check   segs    segcycs desc
    (200,       0,          'internal', 20e6,       True,       1,      8,  False,  65536,      65536,      True,   1,      0,      'quick'),
    (900000,    0,          'internal', 10e6,       True,       1,      8,  True ,  65536,      65536,      True,   1,      0,      'midstream'),
    ('max',     0,          'internal', 15e6,       True,       1,      12, False,  65536,      65536,      True,   1,      0,      'maxsamples12')
]


testGlitchOutputWidthSweepData = [
    # clock     offset    oversamp    reps  steps_per_point desc
    (200e6,     1200,     2,          8,    40,             ''),
]

testRWData = [
    #address    nbytes  reps    desc
    (16,        4,      500,    'SAMPLES'),
    (4,         8,      500,    'ECHO'),
]

testADCTriggerData = [
    #gain       threshold   bits    reps    desc
    (23,        0.8,        12,     5,      ''),
]


testSADTriggerData = [
    #clock  adc_mul bits   emode,   threshold   interval_threshold   offset  reps    desc
    (10e6,  1,      8,     False,   10,         4,                   0,      10,     '8bits'),
    (10e6,  'max',  8,     False,   10,         5,                   0,      20,     'fastest'),
]


def test_fpga_version():
    common_fpga_version_check(scope)

def test_fw_version():
    common_fw_version_check(scope)

@pytest.fixture(autouse=True)
def xadc_check(xadc, log):
    # runs before test:
    #...
    yield
    # runs after test:
    if xadc:
        #print(' temp=%4.1f, XADC=%s' % (scope.XADC.temp, scope.XADC.status), end='')
        print(' temp=%4.1f ' % scope.XADC.temp, end='')
        if scope.XADC.status != 'good':
            print(scope.XADC.status, end='')
            if 'VCCint' in scope.XADC.status: 
                print(' vccint=%1.3f/%1.3f/%1.3f' % (scope.XADC.vccint, scope.XADC.get_vcc('vccint', 'min'),  scope.XADC.get_vcc('vccint', 'max')), end='')
            if 'VCCbram' in scope.XADC.status: 
                print(' vccbram=%1.3f/%1.3f/%1.3f' % (scope.XADC.vccbram, scope.XADC.get_vcc('vccbram', 'min'),  scope.XADC.get_vcc('vccbram', 'max')), end='')
            if 'VCCaux' in scope.XADC.status: 
                print(' vccaux=%1.3f/%1.3f/%1.3f' % (scope.XADC.vccaux, scope.XADC.get_vcc('vccaux', 'min'),  scope.XADC.get_vcc('vccaux', 'max')), end='')
    if log:
        logfile = open(logfilename, 'a')
        logfile.write('%4.1f %1.3f %1.3f %1.3f %1.3f %1.3f %1.3f %1.3f %1.3f %1.3f\n' % 
                (scope.XADC.temp,
                 scope.XADC.vccint, scope.XADC.get_vcc('vccint', 'min'),  scope.XADC.get_vcc('vccint', 'max'),
                 scope.XADC.vccbram, scope.XADC.get_vcc('vccbram', 'min'),  scope.XADC.get_vcc('vccbram', 'max'),
                 scope.XADC.vccaux, scope.XADC.get_vcc('vccaux', 'min'),  scope.XADC.get_vcc('vccaux', 'max')
                ))
        logfile.close()
    scope.XADC.status = 0 # clear any errors after each test

@pytest.fixture(autouse=True)
def cooldown():
    # runs before test:
    #...
    yield
    # runs after test:
    # in case the last testcase leaves scope in a high-power-consuming state that would eventually lead to XADC shutoff:
    scope.clock.clkgen_freq = 7.37e6
    reset_setup(scope,target)

@pytest.mark.parametrize("address, nbytes, reps, desc", testRWData)
def test_reg_rw(address, nbytes, reps, desc):
    reset_setup(scope,target)
    for i in range(reps):
        data = int.to_bytes(random.randrange(2**(8*nbytes)), length=nbytes, byteorder='little')
        scope.sc.sendMessage(0xc0, address, bytearray(data), Validate=False)
        temp = scope.fpga_buildtime # just a dummy read
        read_data = scope.sc.sendMessage(0x80, address, maxResp=nbytes)
        assert read_data == data, "rep %d: expected %0x, got %0x; this is a highly unusual error which indicates inability to communicate with the FPGA" % (i, int.from_bytes(data, byteorder='little'), int.from_bytes(read_data, byteorder='little'))

@pytest.mark.skipif(not target_attached, reason='No target detected')
def test_target_power():
    #scope.io.cwe.setTargetPowerSlew(fastmode=True) # will fail if this is commented out
    for i in range(4):
        scope.io.target_pwr = 0
        time.sleep(0.2)
        scope.io.target_pwr = 1
        time.sleep(0.2)
    common_xadc_check(scope, False, "failure indicates that the target soft-power-up logic needs adjustment, this needs follow-up")

@pytest.mark.parametrize("samples, presamples, testmode, clock, fastreads, adcmul, bits, stream, segments, segment_cycles, reps, desc", testData)
def test_internal_ramp(stress, samples, presamples, testmode, clock, fastreads, adcmul, bits, stream, segments, segment_cycles, reps, desc):
    if stress:
        reps *= 3
    reset_setup(scope,target)
    if clock == 'max':
        clock = MAXCLOCK
    elif clock == 'over2':
        clock = OVERCLOCK2
    scope.clock.clkgen_freq = clock
    scope.clock.adc_mul = adcmul
    time.sleep(0.1)
    assert scope.clock.pll.pll_locked == True, 'Unexpected clock-setting problem.'
    assert abs(scope.clock.adc_freq - clock*adcmul)/scope.clock.adc_freq < 0.01, 'Not getting expected ADC clock frequency.'

    if testmode == 'internal':
        scope.adc.test_mode = True
        scope.ADS4128.mode = 'normal'
    elif testmode == 'ADCramp':
        scope.ADS4128.mode = 'test ramp'
        scope.adc.test_mode = False
    elif testmode == 'ADCalt':
        scope.ADS4128.mode = 'test alternating'
        scope.adc.test_mode = False
    else:
        raise ValueError

    scope.sc._fast_fifo_read_enable = fastreads
    scope.adc.stream_mode = stream
    if samples == 'max':
        samples = MAXSAMPLES
    scope.adc.samples = samples
    scope.adc.presamples = presamples
    scope.adc.segments = segments
    scope.adc.segment_cycles = segment_cycles
    scope.adc.bits_per_sample = bits
    scope.adc.clip_errors_disabled = True
    scope.adc.lo_gain_errors_disabled = True
    scope.adc.segment_cycle_counter_en = True
    for i in range(reps):
        scope.sc.arm(False)
        scope.arm()
        scope.sc.triggerNow()
        scope.sc.arm(False)
        assert scope.capture() == False, 'unable to capture (rep %d), highly unusual error' % i
        raw = np.int64(scope.get_last_trace(True))
        errors, first_error = check_ramp(raw, testmode, bits, samples, segment_cycles)
        assert errors == 0, "%d errors (rep %d); First error: %d; scope.adc.errors: %s" % (errors, i, first_error, scope.adc.errors)
        assert scope.adc.errors == False
    scope.sc._fast_fifo_read_enable = True # return to default




@pytest.mark.parametrize("samples, presamples, testmode, clock, fastreads, adcmul, bits, stream, threshold, seg_size, check, segments, segment_cycles, desc", testTargetData)
@pytest.mark.skipif(not target_attached, reason='No target detected')
def test_target_internal_ramp (samples, presamples, testmode, clock, fastreads, adcmul, bits, stream, threshold, seg_size, check, segments, segment_cycles, desc):
    reset_setup(scope,target)
    scope.clock.clkgen_freq = clock
    scope.clock.adc_mul = adcmul
    time.sleep(0.1)
    assert scope.clock.pll.pll_locked == True, 'Unexpected clock-setting problem.'
    assert abs(scope.clock.adc_freq - clock*adcmul)/scope.clock.adc_freq < 0.01, 'Not getting expected ADC clock frequency.'
    target.baud = 38400 * clock / 1e6 / 7.37

    if testmode == 'internal':
        scope.adc.test_mode = True
        scope.ADS4128.mode = 'normal'
    elif testmode == 'ADC':
        scope.ADS4128.mode = 'test ramp'
        scope.adc.test_mode = False
    else:
        raise ValueError

    scope.io.nrst = 0
    time.sleep(0.1)
    scope.io.nrst = 'high_z'
    time.sleep(0.1)

    target.flush()
    target.write('x\n')
    time.sleep(0.2)
    assert target.read() != '', 'unable to communicate with target'

    scope.trigger.module = 'basic'
    scope.adc.basic_mode = "rising_edge"
    scope.trigger.triggers = "tio4"
    scope.io.tio1 = "serial_rx"
    scope.io.tio2 = "serial_tx"
    scope.io.hs2 = "clkgen"

    scope.sc._fast_fifo_read_enable = fastreads
    if samples == 'max':
        samples = MAXSAMPLES
    scope.adc.samples = samples
    scope.adc.presamples = presamples
    scope.adc.segments = segments
    scope.adc.segment_cycles = segment_cycles
    scope.adc.stream_mode = stream
    scope.adc.stream_segment_threshold = threshold
    scope.adc.stream_segment_size = seg_size
    scope.adc.segment_cycle_counter_en = True
    scope.adc.bits_per_sample = bits
    scope.adc.clip_errors_disabled = True
    scope.adc.lo_gain_errors_disabled = True
    scope.userio.mode = 'fpga_debug'
    scope.userio.fpga_mode = 0
    if samples > 10000000:
        scope.adc.timeout = 5
    if samples > 20000000:
        scope.adc.timeout = 10
    target.flush()
    ret = cw.capture_trace(scope, target, text, key)
    raw = np.int64(scope.get_last_trace(True))
    if verbose: print('Words read before error: %d ' % int.from_bytes(scope.sc.sendMessage(0x80, 47, maxResp=4), byteorder='little'))
    if 'overflow' in desc:
        assert 'overflow' in scope.adc.errors
        scope.errors.clear()
        time.sleep(2)
    else:
        assert scope.adc.errors == False, 'unexpected ADC errors: %s' % scope.adc.errors
    if check: 
        errors, first_error = check_ramp(raw, testmode, bits, samples, segment_cycles)
        assert errors == 0, "%d errors in ramp pattern; First error: %d" % (errors, first_error)
    scope.sc._fast_fifo_read_enable = True # return to default


@pytest.mark.parametrize("clock, offset, oversamp, steps_per_point, reps, desc", testGlitchOutputWidthSweepData)
@pytest.mark.skipif(not scope.LA.present, reason='Cannot test glitch without internal logic analyzer. Rebuild FPGA to test.')
def test_glitch_stress_test(stress, clock, offset, oversamp, reps, steps_per_point, desc):
    if stress:
        reps *= 3
    # sweep glitch width
    reset_setup(scope,target)
    scope.clock.clkgen_freq = clock
    scope.clock.adc_mul = 1
    time.sleep(0.1)
    assert scope.clock.pll.pll_locked == True, 'Unexpected clock-setting problem.'
    assert abs(scope.clock.adc_freq - clock*1)/scope.clock.adc_freq < 0.01, 'Not getting expected ADC clock frequency.'

    margin = 2
    setup_glitch(scope, offset, 0, oversamp)
    stepsize = int(scope.glitch.phase_shift_steps / scope.LA.oversampling_factor / steps_per_point)

    for r in range(reps):
        prev_width = 0
        # sweep width and check that width of glitch increases by expected amount each time:
        for i, width in enumerate(range(-scope.glitch.phase_shift_steps, scope.glitch.phase_shift_steps - stepsize, stepsize)):
            scope.glitch.width = width
            scope.LA.arm()
            scope.glitch.manual_trigger()
            raw = scope.LA.read_capture_data()
            glitch = scope.LA.extract(raw, 0)
            measured_width = len(np.where(glitch > 0)[0])

            # determine expected width
            if width < -scope.glitch.phase_shift_steps // 2:
                expected_width = i // steps_per_point
                increasing = True
            elif width < 0:
                expected_width = oversamp - i // steps_per_point
                increasing = False
            elif width < scope.glitch.phase_shift_steps // 2:
                expected_width = i // steps_per_point - oversamp
                increasing = True
            else:
                expected_width = oversamp*2 - i // steps_per_point
                increasing = False

            if increasing:
                assert measured_width + margin >= prev_width, "Glitch width did not increase"
            else:
                assert measured_width - margin <= prev_width, "Glitch width did not decrease"
            assert abs(measured_width - expected_width) <= margin, "Glitch width not within margin (expected %d, measured %d)" % (expected_width, measured_width)
            prev_width = measured_width

    scope.glitch.enabled = False
    scope.LA.enabled = False
    common_xadc_check(scope, False, 'Glitch stress test pushing things too far? If temperature is just above 65C, could be ok.')



@pytest.mark.parametrize("gain, threshold, bits, reps, desc", testADCTriggerData)
@pytest.mark.skipif(not target_attached, reason='No target detected')
def test_adc_trigger (gain, threshold, bits, reps, desc):
    reset_setup(scope,target)
    scope.default_setup(verbose=False)
    time.sleep(0.1)
    assert scope.clock.pll.pll_locked == True, 'Unexpected clock-setting problem.'
    reset_target(scope)
    time.sleep(0.1)
    target.baud = 38400
    scope.gain.db = gain
    scope.adc.clip_errors_disabled = False
    scope.adc.lo_gain_errors_disabled = True
    scope.adc.segment_cycle_counter_en = False
    scope.adc.segments = 1
    scope.adc.samples = 500
    scope.adc.presamples = 0
    scope.adc.bits_per_sample = bits
    for i in range(reps):
        scope.trigger.module = 'basic'
        scope.trigger.triggers = 'tio4'
        reftrace = cw.capture_trace(scope, target, bytearray(16), bytearray(16))
        #print("Gain:%d, max:%f, min:%f" % (gain, max(reftrace.wave), min(reftrace.wave)))
        # 1. trigger on positive swing:
        scope.trigger.module = 'ADC'
        #print('Min/Max: %3.2f / %3.2f' % (min(reftrace.wave), max(reftrace.wave)))
        scope.trigger.level = threshold * max(reftrace.wave)
        #print(scope.trigger.level)
        powertrace = cw.capture_trace(scope, target, bytearray(16), bytearray(16))
        assert powertrace is not None, 'ADC-triggered capture (max) failed on iteration %d' % i
        # 2. trigger on negative swing:
        scope.trigger.level = threshold * min(reftrace.wave)
        #print(scope.trigger.level)
        powertrace = cw.capture_trace(scope, target, bytearray(16), bytearray(16))
        assert powertrace is not None, 'ADC-triggered capture (min) failed on iteration %d' % i



@pytest.mark.parametrize("clock, adc_mul, bits, emode, threshold, interval_threshold, offset, reps, desc", testSADTriggerData)
@pytest.mark.skipif(not target_attached, reason='No target detected')
def test_sad_trigger (stress, clock, adc_mul, bits, emode, threshold, interval_threshold, offset, reps, desc):
    if stress:
        reps *= 3
    reset_setup(scope,target)
    scope.clock.clkgen_freq = clock
    if adc_mul == 'max':
        adc_mul = int(MAXCLOCK/clock)
    elif adc_mul == 'over':
        adc_mul = int(OVERCLOCK2/clock)
    scope.clock.adc_mul = adc_mul
    time.sleep(0.1)
    assert scope.clock.pll.pll_locked == True, 'Unexpected clock-setting problem.'
    assert abs(scope.clock.adc_freq - clock*adc_mul)/scope.clock.adc_freq < 0.01, 'Not getting expected ADC clock frequency.'
    target.baud = 38400 * clock / 1e6 / 7.37
    reset_target(scope)

    scope.adc.stream_mode = False
    scope.errors.clear()
    scope.trace.enabled = False
    scope.trace.target = None

    scope.SAD.always_armed = False
    scope.SAD.emode = emode
    scope.adc.lo_gain_errors_disabled = True
    scope.adc.clip_errors_disabled = False
    scope.adc.segment_cycle_counter_en = False
    scope.adc.segments = 1
    scope.adc.samples = scope.SAD.sad_reference_length * 2
    scope.adc.presamples = 0
    scope.adc.bits_per_sample = bits
    scope.adc.offset = offset
    scope.SAD.multiple_triggers = False

    scope.trigger.module = 'basic'
    # scope.gain.db = 23.7
    scope.gain.db = 12
    reftrace = cw.capture_trace(scope, target, bytearray(16), bytearray(16), as_int=True)
    assert scope.adc.errors == False, 'Unexpected capture error on reference trace: %s' % scope.adc.errors

    scope.SAD.reference = reftrace.wave
    if scope._is_husky_plus:
        pass # TODO: adjust as needed? e.g. threshold = int(threshold*1.5)
    scope.SAD.threshold = threshold
    scope.SAD.interval_threshold = interval_threshold
    scope.trigger.module = 'SAD'
    scope.adc.offset = 0

    # set presamples so that the waveforms line up;
    # + sad_reference_length because trigger happens at the end of the SAD pattern;
    # + latency for the latency of the SAD triggering logic.
    scope.adc.presamples = scope.SAD.sad_reference_length + scope.SAD.latency
    for rep in range(reps):
        sadtrace = cw.capture_trace(scope, target, bytearray(16), bytearray(16), as_int=True)
        assert sadtrace is not None, 'SAD-triggered capture failed on rep {}'.format(rep)
        assert scope.adc.errors == False, 'Unexpected capture error: %s on rep %d' % (scope.adc.errors, rep)
        sad = 0
        samples = 0
        for r,s,e in zip(reftrace.wave.astype(int), sadtrace.wave.astype(int), scope.SAD.enabled_samples):
            samples += 1
            if bits == 12:
                s = s >> 4
                r = r >> 4
            if e:
                if abs(r-s) > interval_threshold:
                    sad += 1
        assert sad <= threshold, 'FPGA or Python bug? SAD=%d, threshold=%d (iteration %d)' %(sad, threshold, rep)
    common_xadc_check(scope, False, 'SAD stress test pushing things too far? If temperature is just above 65C, could be ok.')


def test_sad_timeouts(stress):
    # have SAD timeout while always_armed on purpose, to get things hot (lighter version of what we do in HuskySADStressTest.ipynb)
    if stress:
        reps = 10
        timeout = 20
    else:
        reps = 3
        timeout = 10
    reset_setup(scope,target)
    clock = 10e6
    adcmul = int(MAXCLOCK / clock)
    scope.clock.clkgen_freq = clock
    scope.clock.adc_mul = adcmul
    time.sleep(0.1)
    assert scope.clock.pll.pll_locked == True, 'Unexpected clock-setting problem.'
    assert abs(scope.clock.adc_freq - clock*adcmul)/scope.clock.adc_freq < 0.01, 'Not getting expected ADC clock frequency.'
    target.baud = 38400 * clock / 1e6 / 7.37
    #print('set adc_mul: %d / %d' % (adcmul, scope.clock.adc_mul))
    
    scope.adc.samples = 20000
    scope.adc.presamples = 0
    scope.adc.offset = 0
    scope.adc.bits_per_sample = 8
    scope.gain.db = 10

    scope.SAD.reference = np.asarray([128]*1024, dtype=np.uint8)
    scope.SAD.threshold = 1 # so low that captures will timeout
    scope.SAD.interval_threshold = 2 # so low that captures will timeout
    scope.adc.timeout = timeout
    scope.SAD.multiple_triggers = False
    scope.SAD.emode = False
    scope.SAD.always_armed = True
    scope.trigger.module = 'SAD'

    for i in range(reps):
        sadtrace = cw.capture_trace(scope, target, bytearray(16), bytearray(16), as_int=True)
        common_xadc_check(scope, False, 'SAD stress test pushing things too far? If temperature is just above 65C, could be ok.')

    scope.SAD.always_armed = False

def test_xadc():
    common_xadc_check(scope, True, 'Final XADC check, it would be odd for this to trip now.')

def test_finish():
    # just restore some defaults:
    scope.default_setup(verbose=False)


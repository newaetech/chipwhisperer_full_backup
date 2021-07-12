# Run with pytest -v -rs

import chipwhisperer as cw
import pytest
import time
import numpy as np

scope = cw.scope()
scope.clock.clkgen_src = 'system'
target = cw.target(scope)

# TODO: program FW?
scope.clock.clkgen_freq = 10e6
scope.clock.adc_mul = 1
time.sleep(0.1)
assert scope.clock.pll.pll_locked == True
assert scope.clock.adc_freq == 10e6
target.baud = 38400 * 10 / 7.37

scope.io.tio1 = "serial_rx"
scope.io.tio2 = "serial_tx"
scope.io.hs2 = "clkgen"

time.sleep(0.2)
scope.io.nrst = 0
time.sleep(0.2)
scope.io.nrst = 'high_z'
time.sleep(0.2)

# see if a target is attached:
target.flush()
target.write('x\n')
time.sleep(0.2)
resp = target.read()
print("Got: %s" % resp)
if resp == '':
    target_attached = False
else:
    target_attached = True

ktp = cw.ktp.Basic()
key, text = ktp.next()
scope.sc._timeout = 3
scope.adc.timeout = 3
scope.adc.offset = 0

# these are default off, but just in case:
scope.glitch.enabled = False
scope.LA.enabled = False

def check_segmented_ramp(raw, samples, segment_cycles, verbose=False):
    errors = 0
    MOD = 2**scope.adc.bits_per_sample
    current_count = raw[0]
    for i, byte in enumerate(raw[1:]):
        if byte != (current_count+1)%MOD:
            if verbose: print("Byte %d: expected %d got %d" % (i, (current_count+1)%MOD, byte))
            errors += 1
            current_count = byte
        else:
            current_count += 1
            if (i+2) % samples == 0:
                current_count += segment_cycles - samples
    return errors


def check_ramp(raw, testmode, samples, segment_cycles, verbose=False):
    errors = 0
    MOD=2**scope.adc.bits_per_sample
    current_count = raw[0]

    if testmode == 'ADC':
        if segment_cycles > 0:
            raise ValueError('not supported')
        started = False
        for i, byte in enumerate(raw[1:]):
            if started:
                if count4 < 3:
                    if byte != current_count:
                        if verbose: print("Byte %d: expected %d got %d" % (i, current_count, byte))
                        errors += 1
                        started = False
                        current_count = byte
                    #print("Got %d, count=%d" % (byte, count4))
                    count4 += 1
                else:
                    count4 = 0
                    if byte != (current_count+1)%MOD:
                        if verbose: print("Byte %d: expected %d got %d" % (i, (current_count+1)%MOD, byte))
                        errors += 1
                    current_count = byte
                    #print(current_count)
            elif byte != current_count:
                started = True
                #print("Starting!")
                count4 = 0
                current_count = byte

    elif testmode == 'internal':
        for i, byte in enumerate(raw[1:]):
            if byte != (current_count+1)%MOD:
                if verbose: print("Byte %d: expected %d got %d" % (i, (current_count+1)%MOD, byte))
                errors += 1
                current_count = byte
            else:
                current_count += 1
                if (i+2) % samples == 0:
                    current_count += segment_cycles - samples

    else:
        return ValueError('oops')
    
    return errors



testData = [
    # samples   presamples  testmode    clock       adcmul  bit stream  segs    segcycs desc
    (10,        0,          'internal', 20e6,       1,      8,  False,  1,      0,      'quick'),
    (131070,    0,          'internal', 20e6,       1,      8,  False,  1,      0,      'maxsamples8'),
    (131070,    0,          'internal', 20e6,       1,      12, False,  1,      0,      'maxsamples12'),
    (300,       0,          'internal', 20e6,       1,      8,  False,  10,     1000,   'evensegments8'),
    (50,        0,          'internal', 20e6,       1,      8,  False,  100,    100,    'oddsegments8'),
    (300,       0,          'internal', 20e6,       1,      12, False,  10,     1000,   'evensegments12'),
    (50,        0,          'internal', 20e6,       1,      12, False,  100,    100,    'oddsegments12'),
    (300,       30,         'internal', 20e6,       1,      12, False,  20,     500,    'presamplesegments'),
    (131070,    0,          'internal', 10e6,       1,      12, False,  1,      0,      'slow'),
    (131070,    0,          'internal', 80e6,       1,      12, False,  1,      0,      'fast'),
    (131070,    0,          'internal', 180e6,      1,      12, False,  1,      0,      'fastest'),
    (131070,    0,          'internal', 2.5e6,      4,      12, False,  1,      0,      '4xslow'),
    (131070,    0,          'internal', 45e6,       4,      12, False,  1,      0,      '4xfast'),
    (131070,    0,          'ADC',      20e6,       1,      12, False,  1,      0,      'ADCslow'),
    (131070,    0,          'ADC',      180e6,      1,      12, False,  1,      0,      'ADCfast'),
    (131070,    0,          'ADC',      45e6,       4,      12, False,  1,      0,      'ADC4xfast'),
]


testTargetData = [
    # samples   presamples  testmode    clock       adcmul  bit stream  threshold   check   segs    segcycs desc
    (200,       0,          'internal', 20e6,       1,      8,  False,  65536,      True,   1,      0,      'quick'),
    (131070,    0,          'internal', 20e6,       1,      12, False,  65536,      True,   1,      0,      'maxsamples12'),
    (200000,    0,          'internal', 20e6,       1,      8,  True ,  65536,      True,   1,      0,      'quickstream8'),
    (2000000,   0,          'internal', 16e6,       1,      12, True ,  65536,      True,   1,      0,      'longstream12'),
    (10000000,  0,          'internal', 16e6,       1,      12, True ,  65536,      False,  1,      0,      'vlongstream12'), # TODO: 20M used to pass?
    (500000,    0,          'internal', 20e6,       1,      12, True ,  16384,      True,   1,      0,      'over'), # TODO: 1M used to pass?
    (2000,      0,          'internal', 10e6,       1,      8,  False,  65536,      True,   1,      0,      'back2nostream'),
]

testGlitchOffsetData = [
    # offset    oversamp    desc
    (0,         40,         ''),
    (400,       40,         ''),
    (800,       40,         ''),
    (1600,      40,         ''),
]

testGlitchWidthData = [
    # offset    oversamp    desc
    (0,         40,         ''),
    (400,       40,         ''),
    (800,       40,         ''),
    (1600,      40,         ''),
]

testGlitchOuputData = [
    # offset    oversamp    desc
    (0,         40,         ''),
    (600,       40,         ''),
    (1200,      40,         ''),
    (0,         60,         ''),
    (0,         20,         ''),
]





def test_fpga_version():
    assert scope.get_fpga_buildtime() == 'FPGA build time: 7/10/2021, 15:0'


def test_fw_version():
    assert scope.fw_version['major'] == 1
    assert scope.fw_version['minor'] == 10
    assert scope.sam_build_date == 'Jun 24 2021'


@pytest.mark.parametrize("samples, presamples, testmode, clock, adcmul, bits, stream, segments, segment_cycles, desc", testData)
def test_internal_ramp(samples, presamples, testmode, clock, adcmul, bits, stream, segments, segment_cycles, desc):
    scope.clock.clkgen_freq = clock
    scope.clock.adc_mul = adcmul
    time.sleep(0.1)
    assert scope.clock.pll.pll_locked == True
    assert scope.clock.adc_freq == clock * adcmul

    if testmode == 'internal':
        scope.adc.test_mode = True
        scope.ADS4128.mode = 'normal'
    elif testmode == 'ADC':
        scope.ADS4128.mode = 'test ramp'
        scope.adc.test_mode = False
    else:
        raise ValueError

    scope.adc.stream_mode = stream
    scope.adc.samples = samples
    scope.adc.presamples = presamples
    scope.adc.segments = segments
    scope.adc.segment_cycles = segment_cycles
    scope.adc.bits_per_sample = bits
    scope.adc.clip_errors_disabled = True
    scope.sc.arm(False)
    scope.arm()
    scope.sc.triggerNow()
    scope.sc.arm(False)
    assert scope.capture() == False
    raw = scope.get_last_trace()
    assert check_ramp(raw, testmode, samples, segment_cycles) == 0
    assert scope.adc.errors == 'no errors'


def setup_glitch(offset, width, oversamp):
    # set up glitch:
    scope.glitch.enabled = True
    scope.clock.pll.update_fpga_vco(600e6)
    scope.glitch.clk_src = 'pll'
    scope.glitch.repeat = 1
    scope.glitch.output = 'glitch_only'
    scope.glitch.trigger_src = 'manual'
    scope.glitch.offset = offset
    scope.glitch.width = width
    assert scope.glitch.mmcm_locked
    # set up LA:
    scope.LA.enabled = True
    scope.LA.oversampling_factor = oversamp
    scope.LA.capture_group = 0
    assert scope.LA.locked


@pytest.mark.parametrize("offset, oversamp, desc", testGlitchOffsetData)
@pytest.mark.skipif(not scope.LA.present, reason='Cannot test glitch without internal logic analyzer. Rebuild FPGA to test.')
def test_glitch_offset(offset, oversamp, desc):
    # first set clock back to 10M:
    scope.clock.clkgen_freq = 10e6
    scope.clock.adc_mul = 1
    time.sleep(0.1)
    assert scope.clock.pll.pll_locked == True
    assert scope.clock.adc_freq == 10e6

    setup_glitch(offset, 0, oversamp)
    margin = 0.1

    # glitch and measure:
    scope.glitch.manual_trigger()
    source    = scope.LA.read_capture(1)
    mmcm1out  = scope.LA.read_capture(2)

    # check offset:
    offset_percent = offset / scope.glitch.phase_shift_steps * 2 # (100% = fully offset)
    mmcm1_not_equal = len(np.where(abs(source-mmcm1out) > 0)[0])
    points = len(source)
    ratio = mmcm1_not_equal / points
    assert (ratio > offset_percent - margin) and (ratio < offset_percent + margin), "Ratio out of bounds (%f)" % ratio
    scope.glitch.enabled = False
    scope.LA.enabled = False

@pytest.mark.parametrize("width, oversamp, desc", testGlitchWidthData)
@pytest.mark.skipif(not scope.LA.present, reason='Cannot test glitch without internal logic analyzer. Rebuild FPGA to test.')
def test_glitch_width(width, oversamp, desc):
    setup_glitch(0, width, oversamp)
    margin = 0.05

    # glitch and measure:
    scope.glitch.manual_trigger()
    mmcm1out  = scope.LA.read_capture(2)
    mmcm2out  = scope.LA.read_capture(3)

    # check width:
    offset_percent = 1 - width / scope.glitch.phase_shift_steps * 2 # (100% = fully offset)
    mmcm2_not_equal = len(np.where(abs(mmcm2out-mmcm1out) > 0)[0])
    points = len(mmcm1out)
    ratio = mmcm2_not_equal / points
    assert (ratio > offset_percent - margin) and (ratio < offset_percent + margin), "Ratio out of bounds (%f)" % ratio
    scope.glitch.enabled = False
    scope.LA.enabled = False

@pytest.mark.parametrize("offset, oversamp, desc", testGlitchOuputData)
@pytest.mark.skipif(not scope.LA.present, reason='Cannot test glitch without internal logic analyzer. Rebuild FPGA to test.')
def test_glitch_output(offset, oversamp, desc):
    margin = 1
    prev_width = 0
    setup_glitch(offset, 0, oversamp)
    stepsize = int(scope.glitch.phase_shift_steps / scope.LA.oversampling_factor)

    # sweep width and check that width of glitch increases by expected amount each time:
    for i, width in enumerate(range(0, scope.glitch.phase_shift_steps//2 - stepsize, stepsize)):
        setup_glitch(offset, width, oversamp)
        scope.glitch.manual_trigger()
        glitch = scope.LA.read_capture(0)
        measured_width = len(np.where(glitch > 0)[0])
        assert measured_width >= prev_width, "Glitch width did not increase"
        assert abs(measured_width - i) <= margin, "Glitch width not within margin"
        prev_width = measured_width

    scope.glitch.enabled = False
    scope.LA.enabled = False


@pytest.mark.parametrize("samples, presamples, testmode, clock, adcmul, bits, stream, threshold, check, segments, segment_cycles, desc", testTargetData)
@pytest.mark.skipif(not target_attached, reason='No target detected')
def test_target_internal_ramp (samples, presamples, testmode, clock, adcmul, bits, stream, threshold, check, segments, segment_cycles, desc):
    scope.clock.clkgen_freq = clock
    scope.clock.adc_mul = adcmul
    time.sleep(0.1)
    assert scope.clock.pll.pll_locked == True
    assert scope.clock.adc_freq == clock * adcmul
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
    assert target.read() != ''

    scope.adc.basic_mode = "rising_edge"
    scope.trigger.triggers = "tio4"
    scope.io.tio1 = "serial_rx"
    scope.io.tio2 = "serial_tx"
    scope.io.hs2 = "clkgen"

    scope.adc.samples = samples
    scope.adc.presamples = presamples
    scope.adc.segments = segments
    scope.adc.segment_cycles = segment_cycles
    scope.adc.stream_mode = stream
    scope.adc.stream_segment_threshold = threshold
    scope.adc.bits_per_sample = bits
    scope.adc.clip_errors_disabled = True
    ret = cw.capture_trace(scope, target, text, key)
    print('Words read before error: %d ' % int.from_bytes(scope.sc.sendMessage(0x80, 47, maxResp=4), byteorder='little'))
    assert scope.adc.errors == 'no errors'
    if check: assert check_ramp(ret.wave, testmode, samples, segment_cycles) == 0


def test_xadc():
    assert scope.XADC.status == 'good'
    assert scope.XADC.temp < 55.0
    assert scope.XADC.max_temp < 60.0   # things can get hotter with glitching


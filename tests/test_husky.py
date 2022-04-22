# Run with pytest -v -rs

import chipwhisperer as cw
import pytest
import time
import numpy as np
import random

""" 
Args:
    reps: number of times to run certain test (default: 1)
          Currently used for glitch sweep test, to catch events such as
          extra glitches which only occur sporadically.

"""

print('\n\n\n\n**************************************************************************************')
print('* IMPORTANT NOTE:                                                                    *')
print('* This script is intended for basic regression testing of Husky during               *')
print('* development. If you are having issues connecting to your Husky or target           *')
print('* device, running this script is unlikely to provide you with useful information.    *')
print('* Instead, seek assistance on forum.newae.com or discord by providing details of     *')
print('* your setup (including the target), and the full error log from your Jupyter        *')
print('* notebook.                                                                          *')
print('*                                                                                    *')
print('* While this test can be run on a stand-alone Husky, some of the tests require a     *')
print('* target with a specific FW (which supports segmenting and trace):                   *')
print('* simpleserial-trace.                                                                *')
print('* The expected .hex file and this script should be updated together.                 *')
print('* If this FW is recompiled, the trace.set_isync_matches() call will have to be       *')
print('* modified with updated instruction addresses.                                       *')
print('**************************************************************************************\n\n')

scope = cw.scope(name='Husky')
target = cw.target(scope)
scope.trace.target = target
trace = scope.trace
scope.errors.clear()
verbose = False

def reset_target():
    scope.io.nrst = 0
    time.sleep(0.2)
    scope.io.nrst = 'high_z'
    time.sleep(0.2)


# TODO: program FW?
scope.sc.reset_fpga()
scope.adc.clip_errors_disabled = True
scope.adc.lo_gain_errors_disabled = True
scope.clock.clkgen_freq = 10e6
scope.clock.clkgen_src = 'system'
scope.clock.adc_mul = 1
time.sleep(0.1)
assert scope.clock.pll.pll_locked == True
assert scope.clock.adc_freq == 10e6
target.baud = 38400 * 10 / 7.37

scope.trigger.triggers = 'tio4'
scope.trigger.module = 'basic'
scope.io.tio1 = "serial_rx"
scope.io.tio2 = "serial_tx"
scope.io.hs2 = "clkgen"

time.sleep(0.2)
reset_target()

# see if a target is attached:
target.flush()
target.write('x\n')
time.sleep(0.2)
resp = target.read()
if resp == '':
    target_attached = False
else:
    target_attached = True

# next, check for a particular FW:
if target_attached:
    target.simpleserial_write('i', b'')
    time.sleep(0.1)
    if target.read().split('\n')[0] == 'ChipWhisperer simpleserial-trace, compiled Mar 14 2022, 21:06:34':
        trace_fw = True
    else:
        trace_fw = False
else:
    trace_fw = False


ktp = cw.ktp.Basic()
key, text = ktp.next()
scope.adc.timeout = 3
scope.adc.offset = 0

# these are default off, but just in case:
scope.glitch.enabled = False
scope.LA.enabled = False
scope.LA.clkgen_enabled = True
scope.LA.capture_depth = 512
scope.LA.downsample = 1

@pytest.fixture()
def reps(pytestconfig):
    return int(pytestconfig.getoption("reps"))

def check_ramp(raw, testmode, samples, segment_cycles, verbose=False):
    errors = 0
    first_error = None
    MOD=2**scope.adc.bits_per_sample
    current_count = raw[0]

    if testmode == 'ADCramp':
        if segment_cycles > 0 and (samples % 4 or segment_cycles % 4):
            raise ValueError('not supported: for ADC testing, samples and segment_cycles must be a multiple of 4')
        started = False
        for i, byte in enumerate(raw[1:]):
            if started:
                if count4 < 3:
                    if byte != current_count:
                        if verbose: print("Byte %d: expected %d got %d" % (i, current_count, byte))
                        errors += 1
                        if not first_error:
                            first_error = i
                        started = False
                        current_count = byte
                    #print("Got %d, count=%d" % (byte, count4))
                    count4 += 1
                else:
                    count4 = 0
                    if byte != (current_count+1)%MOD:
                        if verbose: print("Byte %d: expected %d got %d" % (i, (current_count+1)%MOD, byte))
                        errors += 1
                        if not first_error:
                            first_error = i
                    current_count = byte
                    #print(current_count)
                if (i+2) % samples == 0:
                    current_count = (current_count + (segment_cycles - samples)//4) % MOD
            elif byte != current_count:
                started = True
                #print("Starting!")
                count4 = 0
                current_count = byte

    elif testmode == 'ADCalt':
        if segment_cycles > 0 or scope.adc.bits_per_sample != 12:
            raise ValueError('not supported')
        for i, byte in enumerate(raw[1:]):
            if current_count == 0xaaa:
                current_count = 0x555
            elif current_count == 0x555:
                current_count = 0xaaa
            else:
                errors += 1
                if not first_error:
                    first_error = i
                if verbose: print("Byte %d: unexpected value %0x" % current_count)
            if byte != current_count:
                errors += 1
                if not first_error:
                    first_error = i
                if verbose: print("Byte %d: unexpected value %0x" % current_count)

    elif testmode == 'internal':
        for i, byte in enumerate(raw[1:]):
            if byte != (current_count+1)%MOD:
                if verbose: print("Byte %d: expected %d got %d" % (i, (current_count+1)%MOD, byte))
                errors += 1
                if not first_error:
                    first_error = i
                current_count = byte
            else:
                current_count += 1
                if (i+2) % samples == 0:
                    current_count = (current_count + segment_cycles - samples) % MOD

    else:
        return ValueError('oops')
    
    return errors, first_error


def find0to1trans(data):
    pattern = [0,1]
    return [i for i in range(0,len(data)) if list(data[i:i+len(pattern)])==pattern]


testData = [
    # samples   presamples  testmode    clock       fastreads   adcmul  bit stream  segs    segcycs reps    desc
    (10,        0,          'internal', 20e6,       True,       1,      8,  False,  1,      0,      1,      'quick'),
    (131070,    0,          'internal', 20e6,       True,       1,      8,  False,  1,      0,      1,      'maxsamples8'),
    (131070,    0,          'internal', 20e6,       True,       1,      12, False,  1,      0,      1,      'maxsamples12'),
    (300,       0,          'internal', 20e6,       True,       1,      8,  False,  10,     1000,   1,      'evensegments8'),
    (50,        0,          'internal', 20e6,       True,       1,      8,  False,  100,    100,    1,      'oddsegments8'),
    (300,       0,          'internal', 20e6,       True,       1,      12, False,  10,     1000,   1,      'evensegments12'),
    (50,        0,          'internal', 20e6,       True,       1,      12, False,  100,    100,    1,      'oddsegments12'),
    (300,       30,         'internal', 20e6,       True,       1,      12, False,  20,     500,    1,      'presamplesegments'),
    (131070,    0,          'internal', 10e6,       True,       1,      12, False,  1,      0,      1,      'slow'),
    (131070,    0,          'internal', 80e6,       True,       1,      12, False,  1,      0,      1,      'fast'),
    (131070,    0,          'internal', 200e6,      True,       1,      12, False,  1,      0,      10,     'fastest'),
    (131070,    0,          'internal', 250e6,      True,       1,      12, False,  1,      0,      1,      'overclocked'),
    (131070,    0,          'internal', 5e6,        True,       4,      12, False,  1,      0,      1,      '4xslow'),
    (131070,    0,          'internal', 50e6,       True,       4,      12, False,  1,      0,      1,      '4xfast'),
    (131070,    0,          'ADCramp',  20e6,       True,       1,      12, False,  1,      0,      1,      'ADCslow'),
    (131070,    0,          'ADCramp',  200e6,      True,       1,      12, False,  1,      0,      10,     'ADCfast'),
    (131070,    0,          'ADCramp',  50e6,       True,       4,      12, False,  1,      0,      1,      'ADC4xfast'),
    (131070,    0,          'ADCramp',  250e6,      True,       1,      12, False,  1,      0,      1,      'ADCoverclocked'),
    (8192,      0,          'ADCramp',  10e6,       True,       1,      12, False,  12,     10000,  1,      'ADClongsegments'),
    (64,        0,          'ADCramp',  200e6,      True,       1,      12, False,  1536,   400,    10,     'ADCfastsegments'),
    (300,       30,         'ADCramp',  200e6,      True,       1,      12, False,  327,    400,    10,     'ADCfastsegmentspresamples'),
    (300,       30,         'ADCramp',  250e6,      True,       1,      12, False,  327,    400,    1,      'ADCoverclockedsegmentspresamples'),
    (131070,    0,          'ADCalt',   20e6,       True,       1,      12, False,  1,      0,      10,     'ADCaltslow'),
    (131070,    0,          'ADCalt',   200e6,      True,       1,      12, False,  1,      0,      10,     'ADCaltfast'),
    (131070,    0,          'ADCalt',   250e6,      True,       1,      12, False,  1,      0,      1,      'ADCaltoverclocked'),
    (500,       0,          'internal', 20e6,       False,      1,      12, False,  1,      0,      1,      'slowreads'),
    (131070,    0,          'internal', 20e6,       False,      1,      12, False,  1,      0,      1,      'maxslowreads'),
]

testADCsweep = [
    # samples   presamples  freq_start  freq_stop   freq_step   testmode    fastreads   adcmul  bit stream  segs    segcycs reps    desc
    (30,        15,         48e6,       56e6,       1e6,        'internal', True,       1,      12, False,  327,    100,    50,     'int_segmentspresamples_slow'),
    (30,        15,         100e6,      108e6,      1e6,        'internal', True,       1,      12, False,  327,    100,    50,     'int_segmentspresamples_fast'),
    (30,        15,         10e6,       210e6,      5e6,        'internal', True,       1,      12, False,  327,    100,    2,      'int_segmentspresamples_full'),
    (300,       30,         48e6,       56e6,       1e6,        'internal', True,       1,      12, False,  327,    400,    10,     'int_segmentspresamples_long'),
    (8192,      0,          10e6,       210e6,      5e6,        'ADCramp',  True,       1,      12, False,  12,     100000, 2,      'longsegments'),
    (64,        0,          10e6,       210e6,      5e6,        'ADCramp',  True,       1,      12, False,  1536,   400,    2,      'shortsegments'),
]

testTargetData = [
    # samples   presamples  testmode    clock       fastreads   adcmul  bit stream  threshold   check   segs    segcycs desc
    (200,       0,          'internal', 20e6,       True,       1,      8,  False,  65536,      True,   1,      0,      'quick'),
    (131070,    0,          'internal', 15e6,       True,       1,      12, False,  65536,      True,   1,      0,      'maxsamples12'),
    (200000,    0,          'internal', 20e6,       True,       1,      8,  True ,  65536,      True,   1,      0,      'quickstream8'),
    (2000000,   0,          'internal', 16e6,       True,       1,      12, True ,  65536,      True,   1,      0,      'longstream12'),
    (6000000,   0,          'internal', 16e6,       True,       1,      12, True ,  65536,      False,  1,      0,      'vlongstream12'),
    (500000,    0,          'internal', 20e6,       True,       1,      12, True ,  16384,      True,   1,      0,      'over'),
    (3000000,   0,          'internal', 24e6,       True,       1,      12, True ,  65536,      False,  1,      0,      'overflow'),
    (200000,    0,          'internal', 15e6,       True,       1,      12, True ,  65536,      True,   1,      0,      'postfail'),
    (2000,      0,          'internal', 10e6,       True,       1,      8,  False,  65536,      True,   1,      0,      'back2nostream'),
    (500000,    0,          'internal', 12e6,       False,      1,      12, True ,  65536,      True,   1,      0,      'slowreads1'),
    (2000000,   0,          'internal', 10e6,       False,      1,      12, True ,  65536,      True,   1,      0,      'slowreads2'),
]

testSegmentData = [
    # offset    presamples  samples clock       adcmul  seg_count   segs    segcycs desc
    (0,         0,          90,    7.37e6,     4,      False,      20,     0,      'segments_trigger_no_offset'),
    (0,         10,         90,    7.37e6,     4,      False,      20,     0,      'segments_trigger_no_offset_presamp'),
    (10,        0,          90,    7.37e6,     4,      False,      20,     0,      'segments_trigger_offset10'),
    (50,        0,          90,    7.37e6,     4,      False,      20,     0,      'segments_trigger_offset50'),
    (50,        20,         90,    7.37e6,     4,      False,      20,     0,      'segments_trigger_offset50_presamp'),
    (0,         0,          90,    7.37e6,     4,      True,       20,     29472,  'segments_counter_no_offset'),
    (0,         30,         90,    7.37e6,     4,      True,       20,     29472,  'segments_counter_no_offset_presamp'),
    (10,        0,          90,    7.37e6,     4,      True,       20,     29472,  'segments_counter_offset10'),
    (50,        0,          90,    7.37e6,     4,      True,       20,     29472,  'segments_counter_offset50'),
    (50,        40,         90,    7.37e6,     4,      True,       20,     29472,  'segments_counter_offset50_presamp'),
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

testGlitchOutputWidthSweepData = [
    # offset    oversamp    steps_per_point desc
    (0,         40,         2,              ''),
    (600,       40,         2,              ''),
    (1200,      40,         2,              ''),
    (-1200,     40,         2,              ''),
    (0,         30,         2,              ''),
    (0,         20,         2,              ''),
]

testGlitchOutputOffsetSweepData = [
    # TODO: these used to pass at oversamp=40, but that may be too aggressive?
    # width     oversamp    steps_per_point desc
    (200,       35,         2,              ''),
    (-200,      35,         2,              ''),
    (1000,      35,         2,              ''),
    (-1000,     35,         2,              ''),
    (3000,      35,         2,              ''),
    (-3000,     35,         2,              ''),
    (500,       30,         2,              ''),
    (500,       20,         2,              ''),
]

testGlitchOutputDoublesData = [
    #vco        glitches    oversamp    stepsize    desc
    (600e6,     1,          20,         1,          ''),
    (1200e6,    1,          20,         1,          ''),
    (600e6,     2,          20,         1,          ''),
]

testRWData = [
    #address    nbytes  reps    desc
    (16,        4,      1000,   'SAMPLES'),
    (4,         8,      1000,   'ECHO'),
]

testTraceData = [
    #raw_capture    interface  trigger_source       desc
    (False,         'swo',     'firmware trigger',  'pattern-matched SWO trace, target-triggered'),
    (True,          'swo',     'firmware trigger',  'raw SWO trace, target-triggered'),
    (False,         'swo',     0,                   'pattern-matched SWO trace, trace-triggered'),
]

testTraceSegmentData = [
    #interface  triggers    desc
    ('swo',     1,          '1triggers'),
    ('swo',     10,         '10triggers'),
    ('swo',     21,         '21triggers'),
]

testSADTriggerData = [
    #bits   threshold   offset  reps    desc
    (8,     25,         0,      50,     '8bits'),
    (12,    25,         0,      50,     '12bits'),
]

testUARTTriggerData = [
    #clock      pin     pattern     reps    desc
    (10e6,      'tio1', 'r7DF7',    10,     'tio1_10M'),
    (10e6,      'tio2', 'p000000',  10,     'tio2_10M'),
    (20e6,      'tio1', 'r7DF7',    10,     'tio1_20M'),
    (20e6,      'tio2', 'p000000',  10,     'tio2_20M'),
]

testADCTriggerData = [
    #gain       threshold   reps    desc
    (1,         0.9,        3,     ''),
    (10,        0.9,        3,     ''),
    (1,         0.5,        3,     ''),
    (10,        0.5,        3,     ''),
]


def test_fpga_version():
    assert scope.fpga_buildtime == '4/14/2022, 09:50'


def test_fw_version():
    assert scope.fw_version['major'] == 1
    assert scope.fw_version['minor'] == 0
    assert scope.sam_build_date == '16:50:38 Oct 21 2021'


@pytest.mark.parametrize("address, nbytes, reps, desc", testRWData)
def test_reg_rw(address, nbytes, reps, desc):
    for i in range(reps):
        data = int.to_bytes(random.randrange(2**(8*nbytes)), length=nbytes, byteorder='little')
        scope.sc.sendMessage(0xc0, address, bytearray(data), Validate=False)
        temp = scope.fpga_buildtime # just a dummy read
        assert scope.sc.sendMessage(0x80, address, maxResp=nbytes) == data


@pytest.mark.parametrize("samples, presamples, testmode, clock, fastreads, adcmul, bits, stream, segments, segment_cycles, reps, desc", testData)
def test_internal_ramp(samples, presamples, testmode, clock, fastreads, adcmul, bits, stream, segments, segment_cycles, reps, desc):
    scope.clock.clkgen_freq = clock
    scope.clock.adc_mul = adcmul
    time.sleep(0.1)
    assert scope.clock.pll.pll_locked == True
    assert scope.clock.adc_freq == clock * adcmul

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
        assert scope.capture() == False
        raw = scope.get_last_trace(True)
        errors, first_error = check_ramp(raw, testmode, samples, segment_cycles)
        assert errors == 0, "%d errors; First error: %d" % (errors, first_error)
        assert scope.adc.errors == False
    scope.sc._fast_fifo_read_enable = True # return to default



def last_zero_run(a):
    # Create an array that is 1 where a is 0, and pad each end with an extra 0.
    iszero = np.concatenate(([0], np.equal(a, 0).view(np.int8), [0]))
    absdiff = np.abs(np.diff(iszero))
    # Runs start and end where absdiff is 1.
    ranges = np.where(absdiff == 1)[0].reshape(-1, 2)
    return ranges[-1]

@pytest.mark.parametrize("samples, presamples, freq_start, freq_stop, freq_step, testmode, fastreads, adcmul, bits, stream, segments, segment_cycles, reps, desc", testADCsweep)
def test_adc_freq_sweep(samples, presamples, freq_start, freq_stop, freq_step, testmode, fastreads, adcmul, bits, stream, segments, segment_cycles, reps, desc):
    outfilename = 'test_adc_freq_sweep_%s.out' % desc
    outfile = open(outfilename, 'w')
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
    scope.adc.samples = samples
    scope.adc.presamples = presamples
    scope.adc.segments = segments
    scope.adc.segment_cycles = segment_cycles
    scope.adc.segment_cycle_counter_en = True
    scope.adc.bits_per_sample = bits
    scope.adc.clip_errors_disabled = True
    scope.adc.lo_gain_errors_disabled = True

    all_passed = True

    for clock in range(int(freq_start), int(freq_stop), int(freq_step)):
        scope.clock.clkgen_freq = clock
        scope.clock.adc_mul = adcmul
        time.sleep(0.1)
        assert scope.clock.pll.pll_locked == True
        assert abs(scope.clock.adc_freq - clock * adcmul) <= 10e6
        for i in range(reps):
            scope.sc.arm(False)
            scope.arm()
            scope.sc.triggerNow()
            scope.sc.arm(False)
            assert scope.capture(poll_done=True) == False
            raw = scope.get_last_trace(True)
            errors, first_error = check_ramp(raw, testmode, samples, segment_cycles)

            if errors or scope.adc.errors != False:
                all_passed = False
                #zero_start, zero_stop = last_zero_run(raw)
                #zero_length = zero_stop - zero_start
                #outfile.write('{} MHz: FAIL on iteration {}! {} ramp errors; scope.adc.errors:{}; first error:{}; state:{}, last zero run: {} zeros starting at sample {}; first error: {}\n'.format(scope.clock.adc_freq/1e6, i, errors, scope.adc.errors, scope.adc.first_error, scope.adc.first_error_state, zero_length, zero_start, first_error))
                outfile.write('{} MHz: FAIL on iteration {}! {} ramp errors; scope.adc.errors:{}; first error:{}; state:{}, first error: {}\n'.format(scope.clock.adc_freq/1e6, i, errors, scope.adc.errors, scope.adc.first_error, scope.adc.first_error_state, first_error))
                break # no point running more reps once it fails
            else:
                outfile.write('{} MHz: pass\n'.format(scope.clock.adc_freq/1e6))
                outfile.flush()

    outfile.close()
    assert all_passed, "see %s for details" % outfilename
    scope.sc._fast_fifo_read_enable = True # return to default


def setup_glitch(offset, width, oversamp):
    # set up glitch:
    scope.glitch.enabled = True
    scope.glitch.clk_src = 'pll'
    scope.clock.pll.update_fpga_vco(600e6)
    scope.glitch.repeat = 1
    scope.glitch.output = 'glitch_only'
    scope.glitch.trigger_src = 'manual'
    scope.glitch.offset = offset
    scope.glitch.width = width
    assert scope.glitch.mmcm_locked
    # set up LA:
    scope.LA.enabled = True
    scope.LA.oversampling_factor = oversamp
    scope.LA.capture_group = 'glitch'
    scope.LA.trigger_source = "glitch_source"
    scope.LA.capture_depth = 512
    assert scope.LA.locked

def setup_trace(interface):
    errors = 0
    scope.adc.segments = 1
    scope.adc.samples = 30
    scope.clock.clkgen_freq = 7.37e6
    scope.clock.adc_mul = 4
    target.baud = 38400
    time.sleep(0.1)
    assert scope.clock.pll.pll_locked == True
    reset_target()
    time.sleep(0.5)
    target.baud = 38400
    trace.enabled = True
    if interface == 'parallel':
        trace.clock.fe_clock_src = 'target_clock'
        assert trace.clock.fe_clock_alive
        trace.trace_mode = 'parallel'
        time.sleep(0.1)
        trace.resync()
    elif interface == 'swo':
        trace.clock.fe_clock_src = 'target_clock'
        assert trace.clock.fe_clock_alive
        trace.trace_mode = 'SWO'
        trace.jtag_to_swd()
        acpr = 0
        trigger_freq_mul = 8
        trace.clock.swo_clock_freq = scope.clock.clkgen_freq * trigger_freq_mul
        trace.target_registers.TPI_ACPR = acpr
        trace.swo_div = trigger_freq_mul * (acpr + 1)
        assert trace.clock.swo_clock_locked
        assert scope.userio.status & 0x4, "Are D0/1/2 connected to the target TMS/TCK/TDO?"
    assert trace.uart_state == 'ERX_IDLE', 'Maybe need to do a trace._uart_reset() here?'
    trace.target_registers.DWT_CTRL = 0x40000021
    trace.capture.trigger_source = 'firmware trigger'
    trace.capture.mode = 'while_trig'
    trace.set_isync_matches(addr0=0x080018c4, addr1=0x0800188c, match='both')
    trace.set_periodic_pc_sampling(enable=0)
    trace.capture.use_husky_arm = False


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
    scope.LA.arm()
    scope.glitch.manual_trigger()
    raw = scope.LA.read_capture_data()
    source    = scope.LA.extract(raw, 1)
    mmcm1out  = scope.LA.extract(raw, 2)

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
    scope.LA.arm()
    scope.glitch.manual_trigger()
    raw = scope.LA.read_capture_data()
    mmcm1out  = scope.LA.extract(raw, 2)
    mmcm2out  = scope.LA.extract(raw, 3)

    # check width:
    offset_percent = 1 - width / scope.glitch.phase_shift_steps * 2 # (100% = fully offset)
    mmcm2_not_equal = len(np.where(abs(mmcm2out-mmcm1out) > 0)[0])
    points = len(mmcm1out)
    ratio = mmcm2_not_equal / points
    assert (ratio > offset_percent - margin) and (ratio < offset_percent + margin), "Ratio out of bounds (%f)" % ratio
    scope.glitch.enabled = False
    scope.LA.enabled = False

@pytest.mark.parametrize("offset, oversamp, steps_per_point, desc", testGlitchOutputWidthSweepData)
@pytest.mark.skipif(not scope.LA.present, reason='Cannot test glitch without internal logic analyzer. Rebuild FPGA to test.')
def test_glitch_output_sweep_width(reps, offset, oversamp, steps_per_point, desc):
    margin = 2
    setup_glitch(offset, 0, oversamp)
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


@pytest.mark.parametrize("width, oversamp, steps_per_point, desc", testGlitchOutputOffsetSweepData)
@pytest.mark.skipif(not scope.LA.present, reason='Cannot test glitch without internal logic analyzer. Rebuild FPGA to test.')
def test_glitch_output_sweep_offset(reps, width, oversamp, steps_per_point, desc):
    # This doesn't verify the offset itself -- that's covered by test_glitch_offset().
    # What it does verify is:
    # 1. that the offset change as the offset setting is swept;
    # 2. that there are no "double glitches" - by looking at the glitches themselves, but also by looking
    #    at the width of the glitch "go" signal
    margin = 3
    setup_glitch(0, width, oversamp)
    stepsize = int(scope.glitch.phase_shift_steps / scope.LA.oversampling_factor / steps_per_point)

    for r in range(reps):
        prev_offset = 0
        # sweep offset and check that glitch offset increases by expected amount each time:
        for i, offset in enumerate(range(-scope.glitch.phase_shift_steps, scope.glitch.phase_shift_steps - stepsize, stepsize)):
            scope.glitch.offset = offset
            scope.LA.arm()
            scope.glitch.manual_trigger()
            raw = scope.LA.read_capture_data()
            glitch = scope.LA.extract(raw, 0)
            source = scope.LA.extract(raw, 1)
            go = scope.LA.extract(raw, 4)

            # measure observed offset
            glitchtrans = find0to1trans(glitch)
            sourcetrans = find0to1trans(source)
            assert len(glitchtrans) == 1, "Offset=%d: Expected to find a single glitch but found %d" % (offset, len(glitchtrans))
            g = glitchtrans[0]
            measured_offset = None
            for s in sourcetrans:
                if s > g:
                    measured_offset = s - g
                    break
            assert measured_offset, "Offset=%d: Could not measure offset between source clock and glitch clock" % offset

            golen = len(np.where(go > 0)[0])
            assert abs(golen - oversamp) < oversamp *1.2, "Go width exceeds margin, could lead to extra glitches: %d at offset=%d" % (golen, offset)

            if i > 0:
                # account for full period jump:
                if measured_offset - prev_offset > oversamp/2:
                    prev_offset += oversamp
                # sampling jitter can make us go back and forth a bit:
                elif prev_offset - measured_offset > oversamp/2:
                    prev_offset -= oversamp
                assert prev_offset - measured_offset < margin, "Offset change out of bounds: new offset=%d, previous offset=%d" % (measured_offset, prev_offset)
            prev_offset = measured_offset

    scope.glitch.enabled = False
    scope.LA.enabled = False


@pytest.mark.parametrize("vco, glitches, oversamp, stepsize, desc", testGlitchOutputDoublesData)
@pytest.mark.skipif(not scope.LA.present, reason='Cannot test glitch without internal logic analyzer. Rebuild FPGA to test.')
def test_glitch_output_doubles(reps, vco, glitches, oversamp, stepsize, desc):
    # Similar to test_glitch_output_sweep_offset() but only look at the width of glitch "go".
    # Intended to be a more exhaustive test for double glitches, by sweeping with a finer increment.
    # Since double glitches are an MMCM1/offset problem (width has no effect), we save having to check for different widths.
    # Use a higher VCO frequency for finer grain, and reduce LA oversampling since that doesn't matter as much here.
    setup_glitch(0, 0, oversamp)
    scope.clock.pll.update_fpga_vco(vco)
    scope.glitch.repeat = glitches
    failing_offsets = []
    maxwidth = 0

    for r in range(reps):
        # sweep offset and check that glitch offset increases by expected amount each time:
        for i, offset in enumerate(range(-scope.glitch.phase_shift_steps, scope.glitch.phase_shift_steps - stepsize, stepsize)):
            scope.glitch.offset = offset
            scope.LA.arm()
            scope.glitch.manual_trigger()
            raw = scope.LA.read_capture_data()
            go = scope.LA.extract(raw, 4)

            # check width of glitch "go" signal
            golen = len(np.where(go > 0)[0])
            if golen and (abs(golen/glitches - oversamp) > oversamp/4):
                failing_offsets.append(offset)
                if golen > maxwidth:
                    maxwidth = golen

    assert failing_offsets == [], "Max width seen: %d; failing offsets: %s" % (maxwidth, failing_offsets)
    scope.clock.pll.update_fpga_vco(600e6)
    scope.glitch.enabled = False
    scope.LA.enabled = False



@pytest.mark.parametrize("samples, presamples, testmode, clock, fastreads, adcmul, bits, stream, threshold, check, segments, segment_cycles, desc", testTargetData)
@pytest.mark.skipif(not target_attached, reason='No target detected')
def test_target_internal_ramp (samples, presamples, testmode, clock, fastreads, adcmul, bits, stream, threshold, check, segments, segment_cycles, desc):
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
    scope.trigger.module = 'basic'
    scope.io.tio1 = "serial_rx"
    scope.io.tio2 = "serial_tx"
    scope.io.hs2 = "clkgen"

    scope.sc._fast_fifo_read_enable = fastreads
    scope.adc.samples = samples
    scope.adc.presamples = presamples
    scope.adc.segments = segments
    scope.adc.segment_cycles = segment_cycles
    scope.adc.stream_mode = stream
    scope.adc.stream_segment_threshold = threshold
    scope.adc.segment_cycle_counter_en = True
    scope.adc.bits_per_sample = bits
    scope.adc.clip_errors_disabled = True
    scope.adc.lo_gain_errors_disabled = True
    ret = cw.capture_trace(scope, target, text, key)
    raw = scope.get_last_trace(True)
    if verbose: print('Words read before error: %d ' % int.from_bytes(scope.sc.sendMessage(0x80, 47, maxResp=4), byteorder='little'))
    if desc == 'overflow':
        assert 'fast FIFO' in scope.adc.errors
        scope.errors.clear()
        time.sleep(2)
    else:
        assert scope.adc.errors == False
    if check: 
        errors, first_error = check_ramp(raw, testmode, samples, segment_cycles)
        assert errors == 0, "%d errors; First error: %d" % (errors, first_error)
    scope.sc._fast_fifo_read_enable = True # return to default


@pytest.mark.parametrize("offset, presamples, samples, clock, adcmul, seg_count, segs, segcycs, desc", testSegmentData)
@pytest.mark.skipif(not target_attached, reason='No target detected')
def test_segments (offset, presamples, samples, clock, adcmul, seg_count, segs, segcycs, desc):
    # This requires a specific target firmware to work properly:
    # simpleserial-aes where the number of triggers can be set via 'n' commmand.
    # The segcycs value for seg_count=True requires a very specific firmware, otherwise the test is likely to fail.
    # If the firmware changes, you'll need to run this capture in a notebook with segmenting disabled and manually
    # measure the distance between each AES iteration (which should be fairly easy to do visually, and which shouldn't
    # change much from what's here), then update the segcycs input that's provided here.
    errors = 0
    scope.clock.clkgen_freq =clock
    scope.clock.adc_mul = adcmul
    time.sleep(0.1)
    assert scope.clock.pll.pll_locked == True
    #assert scope.clock.adc_freq == clock * adcmul
    #target.baud = 38400 * clock / 1e6 / 7.37
    target.baud = 38400

    scope.adc.test_mode = False
    scope.ADS4128.mode = 'normal'

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
    scope.trigger.module = 'basic'
    scope.io.tio1 = "serial_rx"
    scope.io.tio2 = "serial_tx"
    scope.io.hs2 = "clkgen"

    scope.adc.samples = samples
    scope.adc.presamples = presamples
    scope.adc.segments = segs
    scope.adc.segment_cycles = segcycs
    scope.adc.segment_cycle_counter_en = seg_count
    scope.adc.offset = offset
    scope.adc.stream_mode = False
    scope.adc.bits_per_sample = 12
    scope.adc.clip_errors_disabled = False
    scope.adc.lo_gain_errors_disabled = True

    scope.gain.db = 10

    target.set_key(bytearray(16))
    target.simpleserial_write('n', bytearray([0, segs]))
    scope.arm()
    target.simpleserial_write('f', bytearray(16))
    ret = scope.capture()
    if ret:
        if verbose: print("Timeout.")
        errors += 1
    time.sleep(0.1)
    if not target.is_done():
        if verbose: print("Target did not finish.")
        errors += 1
    wave = scope.get_last_trace()
    r = target.simpleserial_read('r', target.output_len)

    rounds = []
    rounds_off_by_one = []
    for i in range(segs):
        rounds.append(wave[i*samples:(i+1)*samples-1])
        rounds_off_by_one.append(wave[i*samples+1:(i+1)*samples])

    # check for errors two ways: point-by-point difference, and sum of SAD
    for i in range(1, segs):
        if max(abs(rounds[0] - rounds[i])) > max(abs(wave))/5:
            errors += 1

    # Strategy: SAD between two rounds should be a "small" number. Instead of
    # defining "small", we take the ratio of SAD and SAD with an artificially
    # shifted trace. If this is not a big number, something is wrong.
    ratios = []
    for i in range(1, segs):
        ratio = np.sum(abs(rounds[i] - rounds[0])) / np.sum(abs(rounds_off_by_one[i] - rounds[0]))
        if ratio < 1:
            ratio = 1/ratio
        ratios.append(ratio)
        if ratio < 4:
            errors += 1
            bad_ratio = ratio

    #assert errors == 0, "Ratios = %s; errors: %s" % (ratios, scope.adc.errors)
    assert errors == 0
    scope.adc.clip_errors_disabled = True


@pytest.mark.parametrize("raw_capture, interface, trigger_source, desc", testTraceData)
@pytest.mark.skipif(not trace_fw, reason='No target detected or incorrect FW.')
def test_trace (raw_capture, interface, trigger_source, desc):
    # This requires a specific target firmware to work properly:
    # simpleserial-aes where the number of triggers can be set via 's' commmand.
    setup_trace(interface)
    assert trace.uart_state == 'ERX_IDLE', 'UART is still stuck!'
    scope.adc.clip_errors_disabled = True
    scope.adc.lo_gain_errors_disabled = True
    scope.adc.segment_cycle_counter_en = False
    scope.adc.segments = 1
    scope.adc.samples = 300
    trace.capture.max_triggers = 1
    if trigger_source == 'firmware trigger':
        scope.trigger.module = 'basic'
        scope.trigger.triggers = 'tio4'
        scope.trace.capture.mode = 'while_trig'
    else:
        scope.trigger.module = 'trace'
        scope.trace.capture.mode = 'count_cycles'
        scope.trace.capture.count = 30000
    trace.capture.trigger_source = trigger_source
    if raw_capture:
        trace.capture.raw = True
        trace.capture.rules_enabled = []
    else:
        trace.capture.raw = False
        trace.set_pattern_match(0, [3, 8, 32])
    trace.arm_trace()
    powertrace = cw.capture_trace(scope, target, text, key)
    raw = trace.read_capture_data()
    if raw_capture:
        if interface == 'parallel':
            frames = trace.get_raw_trace_packets(raw, removesyncs=True, verbose=False)
            assert False # TODO!
        else:
            frames = trace.get_raw_trace_packets(raw, removesyncs=False, verbose=False)
            assert frames[0][1][:3]  == [3,8,32], "Got unexpected raw data: %s" % frames[0][1][:3]
    else:
        times = trace.get_rule_match_times(raw, rawtimes=False, verbose=False)
        assert len(times) == 21, "Expected 21 events, got %d" % len(times)
        if trigger_source == 0:
            check_times = times[1:]
            lasttime = times[0][0]
        else:
            check_times = times
            lasttime = 0
        for t in check_times:
            delta = t[0] - lasttime
            assert 200 < delta < 600, "Time delta out of range: %d" % delta
            lasttime= t[0]
    trace.enabled = False


@pytest.mark.parametrize("interface, triggers, desc", testTraceSegmentData)
@pytest.mark.skipif(not trace_fw, reason='No target detected or incorrect FW.')
def test_segment_trace (interface, triggers, desc):
    errors = 0
    scope.default_setup()
    setup_trace(interface)
    scope.adc.clip_errors_disabled = True
    scope.adc.lo_gain_errors_disabled = True
    scope.adc.segment_cycle_counter_en = False
    scope.trigger.module = 'trace'
    scope.trace.capture.mode = 'off'
    trace.capture.trigger_source = 0
    trace.capture.raw = False
    trace.capture.max_triggers = triggers
    trace.set_pattern_match(0, [3, 8, 32])
    scope.adc.presamples = 0
    scope.adc.samples = 30
    scope.adc.segments = triggers
    match_count = trace.capture.matched_pattern_counts[0]
    trace.arm_trace()
    powertrace = cw.capture_trace(scope, target, text, key)
    assert len(powertrace.wave) == scope.adc.samples * triggers
    assert trace.capture.triggers_generated == triggers
    assert trace.capture.matched_pattern_data[:6] == '030820'
    trace.enabled = False

@pytest.mark.parametrize("bits, threshold, offset, reps, desc", testSADTriggerData)
@pytest.mark.skipif(not target_attached, reason='No target detected')
def test_sad_trigger (bits, threshold, offset, reps, desc):
    # TODO: there's something weird with this test where often the first call (first element of testSADTriggerData) passes
    # and the next one fails. It's not a 8-bit/12-bit thing because if you reverse the order, the same holds. And sometimes everything passes.
    # TBD...
    scope.errors.clear()
    scope.trace.enabled = False
    scope.trace.target = None
    scope.default_setup()
    reset_target()
    time.sleep(0.5)
    target.baud = 38400
    scope.adc.clip_errors_disabled = False
    scope.adc.lo_gain_errors_disabled = False
    scope.adc.segment_cycle_counter_en = False
    scope.adc.segments = 1
    scope.adc.samples = scope.SAD._sad_reference_length * 2
    scope.adc.presamples = 0
    scope.adc.bits_per_sample = bits
    scope.adc.offset = offset
    scope.SAD.multiple_triggers = False

    scope.trigger.module = 'basic'
    scope.gain.db = 10
    reftrace = cw.capture_trace(scope, target, bytearray(16), bytearray(16))
    assert scope.adc.errors == False, scope.adc.errors

    scope.SAD.reference = reftrace.wave
    scope.SAD.threshold = threshold
    scope.trigger.module = 'SAD'

    scope.adc.presamples = scope.SAD._sad_reference_length + 6
    for r in range(reps):
        sadtrace = cw.capture_trace(scope, target, bytearray(16), bytearray(16))
        assert sadtrace is not None, 'SAD-triggered capture failed'
        #assert scope.adc.errors == False
        sad = 0
        for i in range(scope.SAD._sad_reference_length):
            sad += abs(reftrace.wave[i] - sadtrace.wave[i])
        sad = int(sad*2**scope.SAD._sad_bits_per_sample)
        assert sad <= threshold, 'SAD=%d, threshold=%d (iteration: %d)' %(sad, threshold, r)


@pytest.mark.parametrize("clock, pin, pattern, reps, desc", testUARTTriggerData)
@pytest.mark.skipif(not target_attached, reason='No target detected')
def test_uart_trigger (clock, pin, pattern, reps, desc):
    scope.default_setup()
    scope.clock.clkgen_freq = clock
    scope.clock.adc_mul = 1
    time.sleep(0.1)
    assert scope.clock.pll.pll_locked == True
    assert scope.clock.adc_freq == clock
    reset_target()
    time.sleep(0.1)
    target.baud = 38400 * clock / 1e6 / 7.37

    scope.gain.db = 10
    scope.adc.clip_errors_disabled = False
    scope.adc.lo_gain_errors_disabled = True
    scope.adc.segment_cycle_counter_en = False
    scope.adc.segments = 1
    scope.adc.samples = 128
    scope.adc.presamples = 0

    scope.trigger.module = 'UART'
    scope.trigger.triggers = pin
    scope.UARTTrigger._uart_reset()
    assert scope.UARTTrigger.uart_state == 'ERX_IDLE', 'UART is still stuck!'
    scope.UARTTrigger.enabled = True
    scope.UARTTrigger.baud = target.baud
    scope.UARTTrigger.set_pattern_match(0, pattern)
    scope.UARTTrigger.trigger_source = 0

    for i in range(reps):
        start_count = scope.UARTTrigger.matched_pattern_counts[0]
        powertrace = cw.capture_trace(scope, target, bytearray(16), bytearray(16))
        assert powertrace is not None, 'UART-triggered capture failed'
        if pin == 'tio1':
            ss_comm = target.simpleserial_last_read
        elif pin == 'tio2':
            ss_comm = target.simpleserial_last_sent
        else:
            raise ValueError('Not supported: please trigger from tio1 or tio2')
        # we don't check the power trace itself (e.g. measure SAD against a tio4-triggered capture with the correct offset), but we check
        # several other things which indirectly tell us that the UART-triggered capture worked:
        assert ss_comm[:len(pattern)] == pattern, "Target last read (%s) doesn't match pattern (%s)" % (ss_comm, pattern)
        assert scope.UARTTrigger.matched_pattern_data[:len(pattern)] == pattern, "matched_pattern_data (%s) doesn't match pattern (%s)" % (scope.UARTTrigger.matched_pattern_data, pattern)
        assert scope.UARTTrigger.matched_pattern_counts[0] == (start_count + 1) % 256, "Match count didn't increase by 1"


@pytest.mark.parametrize("gain, threshold, reps, desc", testADCTriggerData)
@pytest.mark.skipif(not target_attached, reason='No target detected')
def test_adc_trigger (gain, threshold, reps, desc):
    scope.default_setup()
    time.sleep(0.1)
    assert scope.clock.pll.pll_locked == True
    reset_target()
    time.sleep(0.1)
    target.baud = 38400
    scope.gain.db = gain
    scope.adc.clip_errors_disabled = False
    scope.adc.lo_gain_errors_disabled = True
    scope.adc.segment_cycle_counter_en = False
    scope.adc.segments = 1
    scope.adc.samples = 500
    scope.adc.presamples = 0
    for i in range(reps):
        scope.trigger.module = 'basic'
        scope.trigger.triggers = 'tio4'
        reftrace = cw.capture_trace(scope, target, bytearray(16), bytearray(16))
        #print("Gain:%d, max:%f, min:%f" % (gain, max(reftrace.wave), min(reftrace.wave)))
        # 1. trigger on positive swing:
        scope.trigger.module = 'ADC'
        scope.trigger.level = threshold * max(reftrace.wave)
        #print(scope.trigger.level)
        powertrace = cw.capture_trace(scope, target, bytearray(16), bytearray(16))
        assert powertrace is not None, 'ADC-triggered capture (max) failed'
        # 1. trigger on positive swing:
        scope.trigger.level = threshold * min(reftrace.wave)
        #print(scope.trigger.level)
        powertrace = cw.capture_trace(scope, target, bytearray(16), bytearray(16))
        assert powertrace is not None, 'ADC-triggered capture (min) failed'


def test_xadc():
    assert scope.XADC.status == 'good'
    if target_attached:
        # if target isn't attached, last tests run are glitch so will be hotter
        assert scope.XADC.temp < 55.0
    assert scope.XADC.max_temp < 65.0   # things can get hotter with glitching

def test_finish():
    # just restore some defaults:
    scope.default_setup()


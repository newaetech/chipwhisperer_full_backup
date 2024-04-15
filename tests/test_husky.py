# Run with pytest -v -rs

import chipwhisperer as cw
import pytest
import time
import numpy as np
import random
import os

""" 
Args:
    reps: number of times to run certain tests (default: 1)
        Used by some tests only:
        - test_glitch_output_sweep_width
        - test_glitch_output_sweep_offset
        - test_glitch_output_doubles
        - test_glitch_modes
    fulltest: run all tests. Use to validate a new bitfile. Very slow.
        When not set, a subset of tests, and the reps for other tests are
        reduced, to dramatically reduce the runtime; this is useful when
        validating a new Husky device with a known working bitfile, and is the
        default setting.
    swo_trace: run TraceWhisperer tests. Requires a specific target firmware, and
        jumper cables from USERIO D0/1/2 to to target's TMS/TCK/TDO. Disabled
        by default.

Note: in addition to what's controlled by the --fulltest option, some individual
tests are skipped when their description string ("desc") contains SLOW.

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

test_platform = "stm32f3"
logfilename = "test_husky_xadc.log"

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
scope = cw.scope(name='Husky', hw_location=hw_loc)
if test_platform == 'cw305':
    target = cw.target(scope, cw.targets.CW305, force=False)
else:
    target = cw.target(scope)
scope.errors.clear()
verbose = False
cw.scope_logger.setLevel(cw.logging.ERROR) # don't want to see warnings when setting clock past its specifications

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

# use this at the start of each testcase to remove dependency on order of tests:
def reset_setup():
    scope.trigger.module = 'basic'
    scope.trigger.triggers = 'tio4'
    scope.io.tio1 = "serial_rx"
    scope.io.tio2 = "serial_tx"
    scope.io.hs2 = "clkgen"
    scope.adc.timeout = 3
    scope.adc.offset = 0
    scope.glitch.enabled = False
    scope.LA.enabled = False
    scope.LA.clkgen_enabled = True
    scope.LA.capture_depth = 512
    scope.LA.downsample = 1
    scope.trace.enabled = False
    target.baud = 38400 * 10 / 7.37

reset_setup()

time.sleep(0.2)
if test_platform != 'cw305':
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
else:
    target_attached = False

# next, check for a particular FW:
if target_attached:
    target.simpleserial_write('i', b'')
    time.sleep(0.1)
    if target.read().split('\n')[0] == 'ChipWhisperer simpleserial-trace, compiled Mar 14 2022, 21:06:34':
        trace_fw = True
        scope.trace.target = target
        trace = scope.trace
    else:
        trace_fw = False
else:
    trace_fw = False


ktp = cw.ktp.Basic()
key, text = ktp.next()

def check_ramp(raw, testmode, bits_per_sample, samples, segment_cycles, verbose=False):
    errors = 0
    first_error = None
    MOD=2**bits_per_sample
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
        if segment_cycles > 0 or bits_per_sample != 12:
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

def find_edges(data):
    return [i for i in range(0,len(data)) if list(data[i:i+2]) in [[0,1], [1,0]]]

testData = [
    # samples   presamples  testmode    clock       fastreads   adcmul  bit stream  segs    segcycs reps    desc
    (8,         0,          'internal', 20e6,       True,       1,      8,  False,  1,      0,      1,      'smallest_capture'),
    ('max',     0,          'internal', 20e6,       True,       1,      8,  False,  1,      0,      1,      'maxsamples8_SLOW'),
    ('max',     0,          'internal', 20e6,       True,       1,      12, False,  1,      0,      1,      'maxsamples12'),
    (300,       0,          'internal', 20e6,       True,       1,      8,  False,  10,     1000,   1,      'evensegments8_SLOW'),
    (50,        0,          'internal', 20e6,       True,       1,      8,  False,  100,    100,    1,      'oddsegments8_SLOW'),
    (300,       0,          'internal', 20e6,       True,       1,      12, False,  10,     1000,   1,      'evensegments12_SLOW'),
    (50,        0,          'internal', 20e6,       True,       1,      12, False,  100,    100,    1,      'oddsegments12'),
    (300,       30,         'internal', 20e6,       True,       1,      12, False,  20,     500,    1,      'presamplesegments'),
    ('max',     0,          'internal', 10e6,       True,       1,      12, False,  1,      0,      1,      'slow_SLOW'),
    ('max',     0,          'internal', 80e6,       True,       1,      12, False,  1,      0,      1,      'fast_SLOW'),
    ('max',     0,          'internal', 'max',      True,       1,      12, False,  1,      0,      10,     'fastest'),
    ('max',     0,          'internal', 'over2',    True,       1,      12, False,  1,      0,      1,      'overclocked'),
    ('max',     0,          'internal', 5e6,        True,       4,      12, False,  1,      0,      1,      '4xslow_SLOW'),
    ('max',     0,          'internal', 50e6,       True,       4,      12, False,  1,      0,      1,      '4xfast'),
    ('max',     0,          'ADCramp',  20e6,       True,       1,      12, False,  1,      0,      1,      'ADCslow'),
    ('max',     0,          'ADCramp',  'max',      True,       1,      12, False,  1,      0,      10,     'ADCfast_SLOW'),
    ('max',     0,          'ADCramp',  50e6,       True,       4,      12, False,  1,      0,      1,      'ADC4xfast'),
    ('max',     0,          'ADCramp',  'over2',    True,       1,      12, False,  1,      0,      1,      'ADCoverclocked'),
    (8192,      0,          'ADCramp',  10e6,       True,       1,      12, False,  12,     10000,  1,      'ADClongsegments_SLOW'),
    (64,        0,          'ADCramp',  'max',      True,       1,      12, False,  1536,   400,    10,     'ADCfastsegments'),
    (300,       30,         'ADCramp',  'max',      True,       1,      12, False,  327,    400,    10,     'ADCfastsegmentspresamples'),
    (300,       30,         'ADCramp',  'over2',    True,       1,      12, False,  327,    400,    1,      'ADCoverclockedsegmentspresamples'),
    ('max',     0,          'ADCalt',   20e6,       True,       1,      12, False,  1,      0,      10,     'ADCaltslow_SLOW'),
    ('max',     0,          'ADCalt',   'max',      True,       1,      12, False,  1,      0,      10,     'ADCaltfast'),
    ('max',     0,          'ADCalt',   'over2',    True,       1,      12, False,  1,      0,      1,      'ADCaltoverclocked_SLOW'),
    (500,       0,          'internal', 20e6,       False,      1,      12, False,  1,      0,      1,      'slowreads'),
    ('max',     0,          'internal', 20e6,       False,      1,      12, False,  1,      0,      1,      'maxslowreads_SLOW'),
]

testADCsweep = [
    # samples   presamples  freq_start  freq_stop   freq_step   testmode    fastreads   adcmul  bit stream  segs    segcycs reps    desc
    (30,        15,         48e6,       56e6,       1e6,        'internal', True,       1,      12, False,  327,    100,    50,     'int_segmentspresamples_slow'),
    (30,        15,         100e6,      108e6,      1e6,        'internal', True,       1,      12, False,  327,    100,    50,     'int_segmentspresamples_fast'),
    (30,        15,         10e6,       'over1',    5e6,        'internal', True,       1,      12, False,  327,    100,    2,      'int_segmentspresamples_full'),
    (300,       30,         48e6,       56e6,       1e6,        'internal', True,       1,      12, False,  327,    400,    10,     'int_segmentspresamples_long'),
    (8192,      0,          10e6,       'over1',    5e6,        'ADCramp',  True,       1,      12, False,  12,     100000, 2,      'longsegments'),
    (64,        0,          10e6,       'over1',    5e6,        'ADCramp',  True,       1,      12, False,  1536,   400,    2,      'shortsegments'),
]

testTargetData = [
    # samples   presamples  testmode    clock       fastreads   adcmul  bit stream  threshold   seg_size,   check   segs    segcycs desc
    (900000,    0,          'internal', 10e6,       True,       1,      8,  True ,  65536,      65536,      True,   1,      0,      'midstream'),
    (900000,    0,          'internal', 8e6,        True,       1,      8,  True ,  65536,      65536,      True,   1,      0,      'slowstream'),
    (900000,    0,          'internal', 5e6,        True,       1,      12, True ,  65536,      65536,      True,   1,      0,      'slowerstream12'),
    (900000,    0,          'internal', 5e6,        True,       1,      8,  True ,  65536,      65536,      True,   1,      0,      'slowerstream'),
    (4000000,   0,          'internal', 5e6,        True,       1,      8,  True ,  65536,      65536,      True,   1,      0,      'slowerstream_SLOW'),
    # N.B.: much more slow stream tests over at test_husky_stream.py
    (200,       0,          'internal', 20e6,       True,       1,      8,  False,  65536,      65536,      True,   1,      0,      'quick'),
    ('max',     0,          'internal', 15e6,       True,       1,      12, False,  65536,      65536,      True,   1,      0,      'maxsamples12'),
    (400000,    0,          'internal', 20e6,       True,       1,      8,  True ,  65536,      65536,      True,   1,      0,      'quickstream8'),
    (2000000,   0,          'internal', 16e6,       True,       1,      12, True ,  65536,      65536,      True,   1,      0,      'longstream12_SLOW'),
    (6000000,   0,          'internal', 16e6,       True,       1,      12, True ,  65536,      65536,      False,  1,      0,      'vlongstream12_SLOW'),
    (500000,    0,          'internal', 20e6,       True,       1,      12, True ,  16384,      65536,      True,   1,      0,      'over_SLOW'),
    (3000000,   0,          'internal', 24e6,       True,       1,      12, True ,  65536,      65536,      False,  1,      0,      'overflow_SLOW'),
    (200000,    0,          'internal', 15e6,       True,       1,      12, True ,  65536,      65536,      True,   1,      0,      'postfail_SLOW'),
    (2000,      0,          'internal', 10e6,       True,       1,      8,  False,  65536,      65536,      True,   1,      0,      'back2nostream_SLOW'),
    (500000,    0,          'internal', 12e6,       False,      1,      12, True ,  65536,      65536,      True,   1,      0,      'slowreads1_SLOW'),
    (2000000,   0,          'internal', 10e6,       False,      1,      12, True ,  65536,      65536,      True,   1,      0,      'slowreads2_SLOW'),
]


if test_platform == "sam4s":
    testSegmentData = [
        # offset    presamples  samples stream  clock       adcmul  seg_count   segs    segcycs desc
        (0,         0,          8,      False,  7.37e6,     4,      False,      20,     0,      'segments_tiny'),
        (0,         0,          90,     False,  7.37e6,     4,      False,      20,     0,      'segments_trigger_no_offset'),
        (0,         10,         90,     False,  7.37e6,     4,      False,      20,     0,      'segments_trigger_no_offset_presamp'),
        (10,        0,          90,     False,  7.37e6,     4,      False,      20,     0,      'segments_trigger_offset10_SLOW'),
        (50,        0,          90,     False,  7.37e6,     4,      False,      20,     0,      'segments_trigger_offset50_SLOW'),
        (50,        20,         90,     False,  7.37e6,     4,      False,      20,     0,      'segments_trigger_offset50_presamp'),
        (0,         10,         33,     False,  7.37e6,     4,      False,      'max',  0,      'segments_trigger_max_SLOW'),
        (0,         0,          100,    True,   7.37e6,     4,      False,      2000,   0,      'segments_trigger_stream_SLOW'),
        (0,         0,          90,     False,  7.37e6,     4,      True,       20,     32500,  'segments_counter_no_offset'),
        (0,         30,         90,     False,  7.37e6,     4,      True,       20,     32500,  'segments_counter_no_offset_presamp_SLOW'),
        (10,        0,          90,     False,  7.37e6,     4,      True,       20,     32500,  'segments_counter_offset10_SLOW'),
        (50,        0,          90,     False,  7.37e6,     4,      True,       20,     32500,  'segments_counter_offset50_SLOW'),
        (50,        40,         90,     False,  7.37e6,     4,      True,       20,     32500,  'segments_counter_offset50_presamp'),
    ]
else:
    testSegmentData = [
        # offset    presamples  samples stream  clock       adcmul  seg_count   segs    segcycs desc
        (0,         0,          8,      False,  7.37e6,     4,      False,      20,     0,      'segments_tiny'),
        (0,         0,          90,     False,  7.37e6,     4,      False,      20,     0,      'segments_trigger_no_offset'),
        (0,         10,         90,     False,  7.37e6,     4,      False,      20,     0,      'segments_trigger_no_offset_presamp'),
        (10,        0,          90,     False,  7.37e6,     4,      False,      20,     0,      'segments_trigger_offset10_SLOW'),
        (50,        0,          90,     False,  7.37e6,     4,      False,      20,     0,      'segments_trigger_offset50_SLOW'),
        (50,        20,         90,     False,  7.37e6,     4,      False,      20,     0,      'segments_trigger_offset50_presamp'),
        (0,         10,         33,     False,  7.37e6,     4,      False,      'max',  0,      'segments_trigger_max_SLOW'),
        (0,         0,          100,    True,   7.37e6,     4,      False,      2000,   0,      'segments_trigger_stream_SLOW'),
        (0,         0,          90,     False,  7.37e6,     4,      True,       20,     29472,  'segments_counter_no_offset'),
        (0,         30,         90,     False,  7.37e6,     4,      True,       20,     29472,  'segments_counter_no_offset_presamp_SLOW'),
        (10,        0,          90,     False,  7.37e6,     4,      True,       20,     29472,  'segments_counter_offset10_SLOW'),
        (50,        0,          90,     False,  7.37e6,     4,      True,       20,     29472,  'segments_counter_offset50_SLOW'),
        (50,        40,         90,     False,  7.37e6,     4,      True,       20,     29472,  'segments_counter_offset50_presamp'),
    ]


testGlitchOffsetData = [
    # clock     margin  offset    oversamp    desc
    (10e6,      0.1,    0,         40,         ''),
    (10e6,      0.1,    400,       40,         'SLOW'),
    (10e6,      0.1,    800,       40,         'SLOW'),
    (10e6,      0.1,    1600,      40,         ''),
    (20e6,      0.2,    200,       20,         ''),
    (20e6,      0.2,    500,       20,         'SLOW'),
    (100e6,     0.6,    0,         5,          ''),
    (100e6,     0.6,    50,        5,          'SLOW'),
    (100e6,     0.6,    100,       5,          ''),
]

testGlitchWidthData = [
    # offset    oversamp    desc
    (0,         40,         ''),
    (400,       40,         ''),
    (800,       40,         ''),
    (1600,      40,         ''),
]

testGlitchOutputWidthSweepData = [
    # clock     offset    oversamp    steps_per_point desc
    (10e6,      0,         40,         2,              ''),
    (10e6,      600,       40,         2,              'SLOW'),
    (10e6,      1200,      40,         2,              ''),
    (10e6,      -1200,     40,         2,              ''),
    (10e6,      0,         20,         4,              'SLOW'),
    (50e6,      200,       8,          10,             ''),
    (100e6,     400,       4,          20,             ''),
    (200e6,     600,       2,          40,             'SLOW'),
]

testMissingGlitchData = [
    # clock     vco     span    width   num_glitches    reps    oversamp    stepsize    desc
    (10e6,      600e6,  100,    1000,   1,              5,      20,         1,          ''),
    (10e6,      600e6,  100,    1000,   10,             5,      20,         1,          ''),
    (15e6,      600e6,  100,    1000,   10,             5,      20,         1,          ''),
    (25e6,      600e6,  100,    1000,   10,             5,      10,         1,          ''),
    #(10e6,      600e6,  100,    1000,   10,             100,    20,         1,          ''), #slow! keep commented out
    #(10e6,      1200e6, 200,    2000,   10,             100,    20,         1,          ''), #slow! keep commented out
    #(10e6,      600e6,  3360,   1000,   10,             20,     20,         1,          ''), #slow! keep commented out
]

testGlitchOutputOffsetSweepData = [
    # TODO: these used to pass at oversamp=40, but that may be too aggressive?
    # clock     width     oversamp    steps_per_point desc
    (10e6,      200,       35,         2,              ''),
    (10e6,      -200,      35,         2,              ''),
    (10e6,      1000,      35,         2,              ''),
    (10e6,      -1000,     35,         2,              ''),
    (10e6,      3000,      35,         2,              ''),
    (10e6,      -3000,     35,         2,              ''),
    (10e6,      500,       30,         2,              ''),
    (10e6,      500,       20,         2,              ''),
    (50e6,      100,       8,          10,             'may_fail'), # these may fail because we're pushing the clock
    (50e6,      200,       8,          10,             'may_fail'),
    (100e6,     100,       4,          20,             'may_fail'),
    (100e6,     150,       4,          20,             'may_fail'),
    (125e6,     50,        4,          30,             'may_fail'),
    (125e6,     70,        4,          30,             'may_fail'),
    # note: finding glitches at 200 MHz doesn't work reliably because oversampling isn't high enough
]

testGlitchOutputDoublesData = [
    #clock      vco        glitches    oversamp    stepsize    desc
    (10e6,      600e6,     1,          20,         1,          ''),
    (10e6,      1200e6,    1,          20,         1,          ''),
    (10e6,      600e6,     2,          20,         1,          ''),
    (50e6,      600e6,     1,          8,          1,          ''),
    (100e6,     600e6,     1,          4,          1,          ''),
    (100e6,     600e6,     2,          4,          1,          ''),
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
    #clock  adc_mul bits   threshold   offset  reps    desc
    (10e6,  1,      8,     250,        0,      50,     '8bits'),
    (10e6,  1,      12,    250,        0,      50,     '12bits'),
    (10e6,  1,      8,     250,        0,      10,     '8bits_SLOW'),
    (10e6,  10,     8,     250,        0,      50,     'fast_SLOW'),
    (10e6,  18,     8,     250,        0,      50,     'faster_SLOW'),
    (10e6,  'max',  8,     250,        0,      50,     'fastest'),
    (10e6,  'over', 8,     250,        0,      50,     'overclocked_SLOW'),
]

if test_platform == "sam4s":
    testMultipleSADTriggerData = [
        #clock  adc_mul bits   half threshold   plus_thresh segments    offset  reps    desc
        (10e6,  4,      8,     0,   150,        2000,       10,         2700,   20,     'regular'),
        (10e6,  4,      8,     1,   100,        500,        10,         2700,   20,     'half'),
        (10e6,  20,     8,     0,   300,        800,        10,         13500,  20,     'fast'), # TODO: SAM4S + Plus not faring well at 250 MHz here
    ]
else:
    testMultipleSADTriggerData = [
        #clock  adc_mul bits   half threshold   plus_thresh segments    offset  reps    desc
        (10e6,  4,      8,     0,   200,        400,        11,         3525,   20,     'regular'),
        (10e6,  4,      8,     1,   100,        300,        11,         3525,   20,     'half'),
        (10e6,  'max',  8,     0,   350,        600,        11,         17625,  20,     'fast'),
    ]


testUARTTriggerData = [
    #clock      pin     pattern     mask                            bytes_compared  reps    desc
    (10e6,      'tio1', 'r7DF7',    None,                           8,              10,     'tio1_10M'),
    (10e6,      'tio1', 'r7DF7xxx', [255,255,255,255,0,0,0,0],      5,              10,     'tio1_10M'),
    (10e6,      'tio1', 'r7Dxxxxx', [255,255,0,0,0,0,0,0],          3,              10,     'tio1_10M'),
    (10e6,      'tio2', 'p000000',  None,                           8,              10,     'tio2_10M'),
    (20e6,      'tio1', 'r7DF7',    None,                           8,              10,     'tio1_20M'),
    (20e6,      'tio2', 'p000000',  None,                           8,              10,     'tio2_20M'),
]

testADCTriggerData = [
    #gain       threshold   bits    reps    desc
    (1,         0.9,        12,     3,     ''),
    (10,        0.9,        12,     3,     ''),
    (5,         0.9,        8,      3,     'SLOW'),
    (5,         0.5,        8,      3,     ''),
    (1,         0.5,        12,     3,     'SLOW'),
    (10,        0.5,        12,     3,     'SLOW'),
]

testEdgeTriggerData = [
    #pin        edges       oversamp,   check,  reps,   desc
    ('tio1',    2,          4,          True,   3,      ''),
    ('tio1',    4,          4,          True,   3,      'SLOW'),
    ('tio1',    100,        4,          False,  50,     ''),
    ('tio2',    3,          4,          True,   10,     'SLOW'),
    ('tio2',    5,          4,          True,   10,     ''),
    ('tio2',    50,         4,          False,  50,     'SLOW'),
]

testUserioEdgeTriggerData = [
    #pins           max_edges   reps    desc
    ([3,4,5,6,7],   260,        3,      ''),    # exclude pins 0-2 since they are used for trace and could be target-driven
]

testGlitchTriggerData = [
    #module             pattern         reps,   desc
    ('basic',           [0,1],          100,    'basic_glitch_arm_active'),
    ('basic',           [0,1,0],        100,    'basic_glitch_arm_inactive'),
    ('edge_counter',    [1,0,1,0],      100,    'edge_glitch_arm_inactive'),
    ('edge_counter',    [0,1,0,1,0,1],  100,    'edge_glitch_arm_active'),
]

testPLLData = [
    #freq   adc_mul xtal    oversample  tolerance   reps    desc
    (5e6 ,  1,      False,  40,         1,          20,     'CW305_ref'),
    (10e6,  1,      False,  20,         1,          20,     'CW305_ref_SLOW'),
    (15e6,  1,      False,  16,         1,          20,     'CW305_ref'),
    (50e6,  1,      False,  6,          0,          20,     'CW305_ref_SLOW'),
    (75e6,  1,      False,  4,          0,          20,     'CW305_ref_SLOW'),
    (5e6 ,  4,      False,  20,         1,          20,     'CW305_ref_mul4'),
    (15e6,  3,      False,  16,         1,          20,     'CW305_ref_mul3'),
    (20e6,  2,      False,  15,         1,          20,     'CW305_ref_mul2_SLOW'),
    (25e6,  2,      False,  12,         0,          20,     'CW305_ref_mul2_SLOW'),

    (5e6 ,  1,      True,   20,         1,          20,     'xtal_ref'),
    (10e6,  1,      True,   20,         1,          20,     'xtal_ref_SLOW'),
    (15e6,  1,      True,   16,         1,          20,     'xtal_ref'),
    (50e6,  1,      True,   6,          1,          20,     'xtal_ref_SLOW'),
    (75e6,  1,      True,   4,          1,          20,     'xtal_ref_SLOW'),
    (5e6 ,  4,      True,   40,         1,          20,     'xtal_ref_mul4'),
    (15e6,  3,      True,   16,         1,          20,     'xtal_ref_mul3'),
    (20e6,  2,      True,   15,         1,          20,     'xtal_ref_mul2_SLOW'),
    (25e6,  2,      True,   12,         1,          20,     'xtal_ref_mul2_SLOW'),
]


def test_fpga_version():
    if scope._is_husky_plus:
        assert scope.fpga_buildtime == '1/12/2024, 15:19'
    else:
        assert scope.fpga_buildtime == '1/12/2024, 09:25'

def test_fw_version():
    assert scope.fw_version['major'] == 1
    assert scope.fw_version['minor'] == 5
    assert scope.sam_build_date == '13:17:41 Feb  9 2023'

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

@pytest.mark.parametrize("address, nbytes, reps, desc", testRWData)
def test_reg_rw(address, nbytes, reps, desc):
    reset_setup()
    for i in range(reps):
        data = int.to_bytes(random.randrange(2**(8*nbytes)), length=nbytes, byteorder='little')
        scope.sc.sendMessage(0xc0, address, bytearray(data), Validate=False)
        temp = scope.fpga_buildtime # just a dummy read
        read_data = scope.sc.sendMessage(0x80, address, maxResp=nbytes)
        assert read_data == data, "rep %d: expected %0x, got %0x" % (i, int.from_bytes(data, byteorder='little'), int.from_bytes(read_data, byteorder='little'))

@pytest.mark.skipif(not target_attached, reason='No target detected')
def test_target_power():
    #scope.io.cwe.setTargetPowerSlew(fastmode=True) # will fail if this is commented out
    for i in range(4):
        scope.io.target_pwr = 0
        time.sleep(0.2)
        scope.io.target_pwr = 1
        time.sleep(0.2)
    assert scope.XADC.status == 'good'

@pytest.mark.parametrize("samples, presamples, testmode, clock, fastreads, adcmul, bits, stream, segments, segment_cycles, reps, desc", testData)
def test_internal_ramp(fulltest, samples, presamples, testmode, clock, fastreads, adcmul, bits, stream, segments, segment_cycles, reps, desc):
    if not fulltest and 'SLOW' in desc:
        pytest.skip("use --fulltest to run")
        return None
    if not fulltest:
        reps = 1 # reduce number of reps to speed up
    reset_setup()
    if clock == 'max':
        clock = MAXCLOCK
    elif clock == 'over2':
        clock = OVERCLOCK2
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
        assert scope.capture() == False
        raw = scope.get_last_trace(True)
        errors, first_error = check_ramp(raw, testmode, bits, samples, segment_cycles)
        assert errors == 0, "%d errors; First error: %d; scope.adc.errors: %s" % (errors, first_error, scope.adc.errors)
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
def test_adc_freq_sweep(fulltest, samples, presamples, freq_start, freq_stop, freq_step, testmode, fastreads, adcmul, bits, stream, segments, segment_cycles, reps, desc):
    if not fulltest:
        pytest.skip("use --fulltest to run")
        return None
    reset_setup()
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

    if freq_stop == 'over1':
        freq_stop = OVERCLOCK1

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
            errors, first_error = check_ramp(raw, testmode, bits, samples, segment_cycles)

            if errors or scope.adc.errors != False:
                all_passed = False
                #zero_start, zero_stop = last_zero_run(raw)
                #zero_length = zero_stop - zero_start
                #outfile.write('{} MHz: FAIL on iteration {}! {} ramp errors; scope.adc.errors:{}; first error:{}; state:{}, last zero run: {} zeros starting at sample {}; first error: {}\n'.format(scope.clock.adc_freq/1e6, i, errors, scope.adc.errors, scope.adc.first_error, scope.adc.first_error_state, zero_length, zero_start, first_error))
                outfile.write('{} MHz: FAIL on iteration {}! {} ramp errors; scope.adc.errors:{}; first error:{}; state:{}, first error: {}, rep: {}\n'.format(scope.clock.adc_freq/1e6, i, errors, scope.adc.errors, scope.adc.first_error, scope.adc.first_error_state, first_error, i))
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
    scope.LA.trigger_source = "glitch_trigger"
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


@pytest.mark.parametrize("clock, margin, offset, oversamp, desc", testGlitchOffsetData)
@pytest.mark.skipif(not scope.LA.present, reason='Cannot test glitch without internal logic analyzer. Rebuild FPGA to test.')
def test_glitch_offset(fulltest, clock, margin, offset, oversamp, desc):
    if not fulltest and 'SLOW' in desc:
        pytest.skip("use --fulltest to run")
        return None
    reset_setup()
    scope.clock.clkgen_freq = clock
    scope.clock.adc_mul = 1
    time.sleep(0.1)
    assert scope.clock.pll.pll_locked == True
    assert scope.clock.adc_freq == clock

    setup_glitch(offset, 0, oversamp)

    # glitch and measure:
    scope.LA.arm()
    scope.glitch.manual_trigger()
    raw = scope.LA.read_capture_data()
    source    = scope.LA.extract(raw, 1)
    mmcm1out  = scope.LA.extract(raw, 2)

    # check offset:
    offset_percent = offset / scope.glitch.phase_shift_steps * 2 # (100% = fully offset)
    assert offset_percent < 1, "Internal error: offset too big."
    mmcm1_not_equal = len(np.where(abs(source-mmcm1out) > 0)[0])
    points = len(source)
    ratio = mmcm1_not_equal / points
    assert (ratio > offset_percent - margin) and (ratio < offset_percent + margin), "Ratio out of bounds (%f)" % ratio
    scope.glitch.enabled = False
    scope.LA.enabled = False

@pytest.mark.parametrize("width, oversamp, desc", testGlitchWidthData)
@pytest.mark.skipif(not scope.LA.present, reason='Cannot test glitch without internal logic analyzer. Rebuild FPGA to test.')
def test_glitch_width(width, oversamp, desc):
    reset_setup()

    scope.clock.clkgen_freq = 10e6
    scope.clock.adc_mul = 1
    time.sleep(0.1)
    assert scope.clock.pll.pll_locked == True
    assert scope.clock.adc_freq == 10e6

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

@pytest.mark.parametrize("clock, offset, oversamp, steps_per_point, desc", testGlitchOutputWidthSweepData)
@pytest.mark.skipif(not scope.LA.present, reason='Cannot test glitch without internal logic analyzer. Rebuild FPGA to test.')
def test_glitch_output_sweep_width(fulltest, reps, clock, offset, oversamp, steps_per_point, desc):
    if not fulltest and 'SLOW' in desc:
        pytest.skip("use --fulltest to run")
        return None
    reset_setup()
    scope.clock.clkgen_freq = clock
    scope.clock.adc_mul = 1
    time.sleep(0.1)
    assert scope.clock.pll.pll_locked == True
    assert scope.clock.adc_freq == clock

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



@pytest.mark.parametrize("clock, vco, span, width, num_glitches, reps, oversamp, stepsize, desc", testMissingGlitchData)
@pytest.mark.skipif(not target_attached, reason='No target detected')
def test_missing_glitch_sweep_offset(fulltest, clock, vco, span, width, num_glitches, reps, oversamp, stepsize, desc):
    # Checks for missing glitches (https://github.com/newaetech/chipwhisperer-husky-fpga/issues/4)
    # Similar to test_glitch_output_sweep_offset() but doesn't use LA and only sweeps around sensitive spots
    # and uses more repetitions.
    if not fulltest:
        pytest.skip("use --fulltest to run")
        return None
    reset_setup()
    scope.clock.clkgen_freq = clock
    scope.clock.adc_mul = 1
    time.sleep(0.1)
    assert scope.clock.pll.pll_locked == True
    assert scope.clock.adc_freq == clock
    target.baud = 38400 * clock / 1e6 / 7.37
    reset_target()

    scope.clock.pll.update_fpga_vco(vco)
    setup_glitch(0, width, oversamp)
    scope.glitch.num_glitches = num_glitches
    scope.glitch.trigger_src = 'ext_single'
    scope.adc.samples = 16
    errors = []
    for offset in range(scope.glitch.phase_shift_steps//2-span, scope.glitch.phase_shift_steps//2+span, stepsize):
        scope.glitch.offset = offset
        for i in range(reps):
            ext_offsets = []
            for j in range(num_glitches):
                ext_offsets.append(random.randrange(2,5))
            scope.glitch.ext_offset = ext_offsets
            scope.glitch.repeat = [1]*num_glitches
            trace = cw.capture_trace(scope, target, bytearray(16), bytearray(16))
            assert trace is not None, 'capture failed (offset=%d, rep=%d)' % (offset, i)
            if scope.glitch.state != 'idle':
                errors.append(offset)
                #print("Not in idle! offset = %d, rep = %d" % (offset, i))
                scope.glitch.state = None
    assert errors == []


@pytest.mark.parametrize("clock, width, oversamp, steps_per_point, desc", testGlitchOutputOffsetSweepData)
@pytest.mark.skipif(not scope.LA.present, reason='Cannot test glitch without internal logic analyzer. Rebuild FPGA to test.')
def test_glitch_output_sweep_offset(fulltest, reps, clock, width, oversamp, steps_per_point, desc):
    # This doesn't verify the offset itself -- that's covered by test_glitch_offset().
    # What it does verify is:
    # 1. that the offset change as the offset setting is swept;
    # 2. that there are no "double glitches" - by looking at the glitches themselves, but also by looking
    #    at the width of the glitch "go" signal
    # 3. that there are no missing glitches
    if not fulltest:
        pytest.skip("use --fulltest to run")
        return None
    reset_setup()
    scope.clock.clkgen_freq = clock
    scope.clock.adc_mul = 1
    time.sleep(0.1)
    assert scope.clock.pll.pll_locked == True
    assert scope.clock.adc_freq == clock

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


@pytest.mark.parametrize("clock, vco, glitches, oversamp, stepsize, desc", testGlitchOutputDoublesData)
@pytest.mark.skipif(not scope.LA.present, reason='Cannot test glitch without internal logic analyzer. Rebuild FPGA to test.')
def test_glitch_output_doubles(fulltest, reps, clock, vco, glitches, oversamp, stepsize, desc):
    # Similar to test_glitch_output_sweep_offset() but only look at the width of glitch "go".
    # Intended to be a more exhaustive test for double glitches, by sweeping with a finer increment.
    # Since double glitches are an MMCM1/offset problem (width has no effect), we save having to check for different widths.
    # Use a higher VCO frequency for finer grain, and reduce LA oversampling since that doesn't matter as much here.
    if not fulltest:
        pytest.skip("use --fulltest to run")
        return None
    reset_setup()
    scope.clock.clkgen_freq = clock
    scope.clock.adc_mul = 1
    time.sleep(0.1)
    assert scope.clock.pll.pll_locked == True
    assert scope.clock.adc_freq == clock

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
            assert scope.LA.fifo_empty() == False, "scope.LA didn't capture on iteration %d, offset=%d" % (i, offset)
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



@pytest.mark.parametrize("samples, presamples, testmode, clock, fastreads, adcmul, bits, stream, threshold, seg_size, check, segments, segment_cycles, desc", testTargetData)
@pytest.mark.skipif(not target_attached, reason='No target detected')
def test_target_internal_ramp (fulltest, samples, presamples, testmode, clock, fastreads, adcmul, bits, stream, threshold, seg_size, check, segments, segment_cycles, desc):
    if not fulltest and 'SLOW' in desc:
        pytest.skip("use --fulltest to run")
        return None
    reset_setup()
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
    raw = scope.get_last_trace(True)
    if verbose: print('Words read before error: %d ' % int.from_bytes(scope.sc.sendMessage(0x80, 47, maxResp=4), byteorder='little'))
    if 'overflow' in desc:
        assert 'overflow' in scope.adc.errors
        scope.errors.clear()
        time.sleep(2)
    else:
        assert scope.adc.errors == False
    if check: 
        errors, first_error = check_ramp(raw, testmode, bits, samples, segment_cycles)
        assert errors == 0, "%d errors; First error: %d" % (errors, first_error)
    scope.sc._fast_fifo_read_enable = True # return to default


@pytest.mark.parametrize("offset, presamples, samples, stream, clock, adcmul, seg_count, segs, segcycs, desc", testSegmentData)
@pytest.mark.skipif(not target_attached, reason='No target detected')
def test_segments (fulltest, offset, presamples, samples, stream, clock, adcmul, seg_count, segs, segcycs, desc):
    # This requires a specific target firmware to work properly:
    # simpleserial-aes where the number of triggers can be set via 'n' commmand.
    # The segcycs value for seg_count=True requires a very specific firmware, otherwise the test is likely to fail.
    # If the firmware changes, you'll need to run this capture in a notebook with segmenting disabled and manually
    # measure the distance between each AES iteration (which should be fairly easy to do visually, and which shouldn't
    # change much from what's here), then update the segcycs input that's provided here.
    funcparams = str(locals())
    if not fulltest and 'SLOW' in desc:
        pytest.skip("use --fulltest to run")
        return None
    reset_setup()
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

    scope.trigger.module = 'basic'
    scope.adc.basic_mode = "rising_edge"
    scope.trigger.triggers = "tio4"
    scope.io.tio1 = "serial_rx"
    scope.io.tio2 = "serial_tx"
    scope.io.hs2 = "clkgen"

    scope.adc.samples = samples
    scope.adc.presamples = presamples
    if segs == 'max':
        scope.adc.timeout = 10
        segs = MAXSEGMENTSAMPLES // samples
    scope.adc.segments = segs
    scope.adc.segment_cycles = segcycs
    scope.adc.segment_cycle_counter_en = seg_count
    scope.adc.offset = offset
    scope.adc.stream_mode = stream
    scope.adc.bits_per_sample = 12
    scope.adc.clip_errors_disabled = False
    scope.adc.lo_gain_errors_disabled = True

    scope.gain.db = 10

    target.set_key(bytearray(16))
    target.simpleserial_write('n', list(int.to_bytes(segs, length=2, byteorder='big')))
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
    for i in range(2, segs):
        if max(abs(rounds[1] - rounds[i])) > max(abs(wave))/1.2:
            #print('Max violation: %f, %f' % (max(abs(rounds[0] - rounds[i])), max(abs(wave))/1.5))
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
        if ratio < 3:
            errors += 1
            bad_ratio = ratio

    assert errors == 0, "Ratios = %s; errors: %s, params = %s" % (ratios, scope.adc.errors, funcparams)
    scope.adc.clip_errors_disabled = True


@pytest.mark.parametrize("raw_capture, interface, trigger_source, desc", testTraceData)
@pytest.mark.skipif(not trace_fw, reason='No target detected or incorrect FW.')
def test_trace (swo_trace, raw_capture, interface, trigger_source, desc):
    # This requires a specific target firmware to work properly:
    # simpleserial-aes where the number of triggers can be set via 's' commmand.
    if not swo_trace:
        pytest.skip("use --swo_trace to run")
        return None
    reset_setup()
    scope.default_setup(verbose=False)
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
        trace.set_pattern_match(0, [3, 8, 32, 0, 0, 0, 0, 0], [255, 255, 255, 0, 0, 0, 0, 0])
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
def test_segment_trace (swo_trace, interface, triggers, desc):
    if not swo_trace:
        pytest.skip("use --swo_trace to run")
        return None
    reset_setup()
    errors = 0
    scope.default_setup(verbose=False)
    setup_trace(interface)
    scope.adc.clip_errors_disabled = True
    scope.adc.lo_gain_errors_disabled = True
    scope.adc.segment_cycle_counter_en = False
    scope.trigger.module = 'trace'
    scope.trace.capture.mode = 'off'
    trace.capture.trigger_source = 0
    trace.capture.raw = False
    trace.capture.max_triggers = triggers
    trace.set_pattern_match(0, [3, 8, 32, 0, 0, 0, 0, 0], [255, 255, 255, 0, 0, 0, 0, 0])
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

@pytest.mark.parametrize("clock, adc_mul, bits, threshold, offset, reps, desc", testSADTriggerData)
@pytest.mark.skipif(not target_attached, reason='No target detected')
def test_sad_trigger (fulltest, clock, adc_mul, bits, threshold, offset, reps, desc):
    if not fulltest and 'SLOW' in desc:
        pytest.skip("use --fulltest to run")
        return None
    if not fulltest:
        reps = 3 # go faster
    reset_setup()
    scope.clock.clkgen_freq = clock
    if adc_mul == 'max':
        adc_mul = int(MAXCLOCK/clock)
    elif adc_mul == 'over':
        adc_mul = int(OVERCLOCK2/clock)
    scope.clock.adc_mul = adc_mul
    time.sleep(0.1)
    assert scope.clock.pll.pll_locked == True
    assert scope.clock.adc_freq == clock * adc_mul
    target.baud = 38400 * clock / 1e6 / 7.37
    reset_target()

    scope.adc.stream_mode = False
    scope.errors.clear()
    scope.trace.enabled = False
    scope.trace.target = None

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
    reftrace = cw.capture_trace(scope, target, bytearray(16), bytearray(16))
    assert scope.adc.errors == False, (scope.adc.errors, scope.gain)

    scope.SAD.reference = reftrace.wave
    if scope._is_husky_plus:
        threshold *= 3
    scope.SAD.threshold = threshold
    scope.trigger.module = 'SAD'
    scope.adc.offset = 0

    # set presamples so that the waveforms line up;
    # + sad_reference_length because trigger happens at the end of the SAD pattern;
    # + latency for the latency of the SAD triggering logic.
    scope.adc.presamples = scope.SAD.sad_reference_length + scope.SAD.latency
    for r in range(reps):
        sadtrace = cw.capture_trace(scope, target, bytearray(16), bytearray(16))
        assert sadtrace is not None, 'SAD-triggered capture failed on rep {}'.format(r)
        assert scope.adc.errors == False
        sad = 0
        for i in range(scope.SAD.sad_reference_length):
            sad += abs(reftrace.wave[i] - sadtrace.wave[i])
        sad = int(sad*2**scope.SAD._sad_bits_per_sample)
        assert sad <= threshold, 'SAD=%d, threshold=%d (iteration: %d)' %(sad, threshold, r)


@pytest.mark.parametrize("clock, adc_mul, bits, half, threshold, plus_thresh, segments, offset, reps, desc", testMultipleSADTriggerData)
@pytest.mark.skipif(not target_attached, reason='No target detected')
def test_multiple_sad_trigger (fulltest, clock, adc_mul, bits, half, threshold, plus_thresh, segments, offset, reps, desc):
    if not fulltest and 'SLOW' in desc:
        pytest.skip("use --fulltest to run")
        return None
    if not fulltest:
        reps = 3 # go faster
    reset_setup()
    scope.clock.clkgen_freq = clock
    if adc_mul == 'max':
        adc_mul = int(MAXCLOCK/clock)
    elif adc_mul == 'over':
        adc_mul = int(OVERCLOCK2/clock)
    scope.clock.adc_mul = adc_mul
    time.sleep(0.1)
    assert scope.clock.pll.pll_locked == True
    assert scope.clock.adc_freq == clock * adc_mul
    target.baud = 38400 * clock / 1e6 / 7.37
    reset_target()

    scope.errors.clear()
    scope.trace.enabled = False
    scope.trace.target = None

    scope.SAD.multiple_triggers = True
    scope.SAD.half_pattern = half
    scope.adc.lo_gain_errors_disabled = True
    scope.adc.clip_errors_disabled = False
    scope.adc.segment_cycle_counter_en = False
    scope.adc.segments = 1
    scope.adc.samples = scope.SAD.sad_reference_length * 2
    scope.adc.presamples = 0
    scope.adc.bits_per_sample = bits
    scope.adc.offset = offset

    scope.trigger.module = 'basic'
    # scope.gain.db = 23.7
    scope.gain.db = 12
    reftrace = cw.capture_trace(scope, target, bytearray(16), bytearray(16))
    assert scope.adc.errors == False, (scope.adc.errors, scope.gain)

    scope.SAD.reference = reftrace.wave
    if scope._is_husky_plus:
        threshold = plus_thresh

    scope.SAD.threshold = threshold
    scope.trigger.module = 'SAD'
    scope.adc.offset = 0

    # set presamples so that the waveforms line up;
    # + sad_reference_length because trigger happens at the end of the SAD pattern;
    # + latency for the latency of the SAD triggering logic.
    scope.adc.presamples = scope.SAD.sad_reference_length + scope.SAD.latency
    scope.adc.segments = segments
    scope.adc.samples -= scope.adc.samples %3
    for r in range(reps):
        sadtrace = cw.capture_trace(scope, target, bytearray(16), bytearray(16))
        assert sadtrace is not None, 'SAD-triggered capture failed on rep {}'.format(r)
        assert scope.SAD.num_triggers_seen == scope.adc.segments
        assert scope.adc.errors == False
        for s in range(scope.adc.segments):
            sad = 0
            for i in range(scope.SAD.sad_reference_length):
                sad += abs(reftrace.wave[i] - sadtrace.wave[i+s*scope.adc.samples])
            sad = int(sad*2**scope.SAD._sad_bits_per_sample)
            assert sad <= threshold, 'SAD=%d, threshold=%d (iteration: %d, segment %d)' %(sad, threshold, r, s)



@pytest.mark.parametrize("clock, pin, pattern, mask, bytes_compared, reps, desc", testUARTTriggerData)
@pytest.mark.skipif(not target_attached, reason='No target detected')
def test_uart_trigger (fulltest, clock, pin, pattern, mask, bytes_compared, reps, desc):
    if not fulltest:
        reps = 2 # reduce number of reps to speed up
    reset_setup()
    scope.default_setup(verbose=False)
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
    scope.UARTTrigger.set_pattern_match(0, pattern, mask)
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
        assert pattern[:bytes_compared] in ss_comm, "Target last read (%s) doesn't contain pattern (%s)" % (ss_comm, pattern)
        pattern_start = scope.UARTTrigger.pattern_size - len(pattern)
        #pattern_stop = max(pattern_start + bytes_compared, 8)
        pattern_stop = min(pattern_start + bytes_compared, 8)
        assert scope.UARTTrigger.matched_pattern_data()[pattern_start:pattern_stop] == pattern[:bytes_compared], "matched_pattern_data (%s) doesn't match pattern (%s)" % (scope.UARTTrigger.matched_pattern_data(), pattern)
        assert scope.UARTTrigger.matched_pattern_counts[0] == (start_count + 1) % 256, "Match count didn't increase by 1"


@pytest.mark.parametrize("gain, threshold, bits, reps, desc", testADCTriggerData)
@pytest.mark.skipif(not target_attached, reason='No target detected')
def test_adc_trigger (fulltest, gain, threshold, bits, reps, desc):
    if not fulltest and 'SLOW' in desc:
        pytest.skip("use --fulltest to run")
        return None
    reset_setup()
    scope.default_setup(verbose=False)
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
    scope.adc.bits_per_sample = bits
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


@pytest.mark.parametrize("pin, edges, oversamp, check, reps, desc", testEdgeTriggerData)
@pytest.mark.skipif(not target_attached, reason='No target detected')
def test_edge_trigger (fulltest, pin, edges, oversamp, check, reps, desc):
    if not fulltest and 'SLOW' in desc:
        pytest.skip("use --fulltest to run")
        return None
    reset_setup()
    scope.default_setup(verbose=False)
    time.sleep(0.1)
    assert scope.clock.pll.pll_locked == True
    reset_target()
    time.sleep(0.1)
    target.baud = 38400
    scope.adc.clip_errors_disabled = True
    scope.adc.lo_gain_errors_disabled = True
    scope.trigger.module = 'edge_counter'
    scope.trigger.triggers = pin
    scope.trigger.edges = edges

    for i in range(reps):
        if check:
            # Use scope.LA to check that the trigger occurs when it should.
            # We can only do this when # edges is small (when it's too big,
            # scope.LA's storage isn't deep enough to capture everything)
            scope.LA.enabled = True
            scope.LA.clk_source = 'pll'
            scope.LA.oversampling_factor = oversamp
            scope.LA.downsample = 1
            scope.LA.capture_group = 'CW 20-pin'
            scope.LA.trigger_source = 'falling_' + pin
            scope.LA.capture_depth = 16000
            scope.io.glitch_trig_mcx = 'trigger'
            scope.LA.arm()
            trace = cw.capture_trace(scope, target, bytearray(16), bytearray(16))
            #print(scope)
            assert not scope.LA.fifo_empty()
            raw = scope.LA.read_capture_data()
            tio1 = scope.LA.extract(raw, 0)
            tio2 = scope.LA.extract(raw, 1)
            trig = scope.LA.extract(raw, 7)
            if pin == 'tio1':
                input_edges = find_edges(tio1)
            elif pin == 'tio2':
                input_edges = find_edges(tio2)
            else:
                raise ValueError('unsupported pin')
            trig_edges = find_edges(trig)
            assert len(input_edges)+1 >= edges, "Couldn't observe enough edges!"
            assert abs(trig_edges[0] - oversamp - input_edges[edges-2]) < oversamp, "Rep %d: Trigger not where expected! input_edges=%s, trig_edges=%s" % (i, input_edges, trig_edges)

        else:
            # otherwise, we just check for a successful capture
            trace = cw.capture_trace(scope, target, bytearray(16), bytearray(16))
            assert trace is not None, 'Capture failed. Observed %d edges' % scope.trigger.edges_seen

def armed():
    return scope.sc.getStatus() & 0x1

@pytest.mark.parametrize("pins, max_edges, reps, desc", testUserioEdgeTriggerData)
def test_userio_edge_triggers(fulltest, pins, max_edges, reps, desc):
    # This tests triggering from USERIO pins and also further tests edge triggering.
    # Note that there would be nothing to gain from also testing with scope.trigger.mode = 'normal'
    # since the logic controlling the input to the trigger module doesn't care about the mode.
    if not fulltest:
        pytest.skip("use --fulltest to run")
        return None
    reset_setup()
    scope.default_setup(verbose=False)
    time.sleep(0.1)
    scope.trigger.module = 'edge_counter'
    scope.userio.mode = 'normal'
    scope.userio.direction = 0
    # only drive the pins that we'll be using:
    for pin in pins:
        scope.userio.direction += 2**pin
    for rep in range(reps):
        for userio_pin in (pins):
            scope.trigger.triggers = 'userio_d' + str(userio_pin)
            edges = random.randrange(1, max_edges+1)
            scope.trigger.edges = edges
            scope.userio.drive_data = random.randrange(0, 0x100)
            edges_applied = 0
            scope.sc.arm(False)
            assert not armed()
            scope.arm()
            while (edges_applied < edges):
                new_value = random.randrange(0, 0x100)
                if (new_value & 2**userio_pin) != (scope.userio.status & 2**userio_pin):
                    edges_applied += 1
                scope.userio.drive_data = new_value
                if edges_applied < edges:
                    assert armed(), "Pin %d, rep %d: scope disarmed after only %d edges! This should not have happened until %d edges." % (userio_pin, rep, edges_applied, edges)
            assert not armed(), "Pin %d, rep %d: scope is still armed after all edges have been applied." % (userio_pin, rep)


@pytest.mark.skipif(not target_attached, reason='No target detected')
def test_glitch_modes (fulltest, reps):
    if not fulltest:
        pytest.skip("use --fulltest to run")
        return None
    scope.reset_fpga()
    reset_setup()
    scope.default_setup(verbose=False)
    time.sleep(0.1)
    assert scope.clock.pll.pll_locked == True
    reset_target()
    time.sleep(0.1)
    target.baud = 38400
    scope.adc.clip_errors_disabled = True
    scope.adc.lo_gain_errors_disabled = True
    scope.glitch.enabled = True
    scope.glitch.clk_src = 'pll'
    scope.glitch.state = None
    scope.glitch.output = 'clock_xor'
    scope.glitch.repeat = 1
    scope.LA.enabled = True
    scope.LA.oversampling_factor = 4
    scope.LA.downsample = 1
    scope.LA.capture_group = 'glitch'
    scope.LA.trigger_source = "glitch"
    for i in range(reps):
        capture_depth = 1000000
        while capture_depth > scope.LA.max_capture_depth:
            randomize_glitches()
            capture_depth = scope.glitch.num_glitches * scope.LA.oversampling_factor * max(scope.glitch.ext_offset) * 2
        scope.LA.capture_depth = capture_depth
        scope.glitch.trigger_src = 'ext_single'
        glitch_single(trigger=True, expected=True)
        glitch_single(trigger=False, expected=False)

        scope.glitch.trigger_src = 'ext_continuous'
        glitch_single(trigger=False, expected=True)
        glitch_single(trigger=True, expected=True)
        glitch_single(trigger=False, expected=True)

        scope.glitch.trigger_src = 'ext_single'
        glitch_single(trigger=True, expected=True)

        glitch_manual()

        scope.glitch.trigger_src = 'ext_single'
        glitch_single(trigger=True, expected=True)

        glitch_continuous()


def randomize_glitches():
    scope.glitch.num_glitches = random.randrange(2, 32)
    offsets = []
    repeats = []
    repeat = 0
    for i in range(scope.glitch.num_glitches):
        offset = random.randrange(repeat, 100)
        repeat = random.randrange(1, 90)
        offsets.append(offset)
        repeats.append(repeat)
        scope.glitch.ext_offset = offsets
        scope.glitch.repeat = repeats
        scope.glitch.output = 'enable_only'

def glitch_single(trigger=True, expected=True):
    assert scope.glitch.state == 'idle'
    scope.LA.trigger_source = "glitch"
    scope.LA.arm()
    if trigger:
        trace = cw.capture_trace(scope, target, bytearray(16), bytearray(16))
    else:
        target.simpleserial_write('p', bytearray(16))
    time.sleep(0.1)
    if expected:
        assert not scope.LA.fifo_empty()
        raw = scope.LA.read_capture_data()
        glitchenable = scope.LA.extract(raw, 6)
        slack = scope.glitch.num_glitches * scope.LA.oversampling_factor * 2
        expected = scope.LA.oversampling_factor * (sum(scope.glitch.repeat) + scope.glitch.num_glitches)
        actual = len(np.where(glitchenable != 0)[0])
        assert (expected-slack) < actual < (expected+slack), "actual=%d, expected=%d, slack=%d" % (actual, expected, slack)
    else:
        assert scope.LA.fifo_empty()
        scope.LA.trigger_now()
        raw = scope.LA.read_capture_data()
        glitchenable = scope.LA.extract(raw, 6)
        assert len(np.where(glitchenable != 0)[0]) == 0

def glitch_manual():
    assert scope.glitch.state == 'idle'
    scope.glitch.trigger_src = 'manual'
    scope.LA.trigger_source = "glitch"
    scope.LA.arm()
    scope.glitch.manual_trigger()
    time.sleep(0.1)
    assert not scope.LA.fifo_empty()
    raw = scope.LA.read_capture_data()
    glitchenable = scope.LA.extract(raw, 6)
    slack = scope.LA.oversampling_factor * 2
    expected = scope.LA.oversampling_factor * scope.glitch.repeat[0]
    actual = len(np.where(glitchenable != 0)[0])
    assert (expected-slack) < actual < (expected+slack), "actual=%d, expected=%d, slack=%d" % (actual, expected, slack)

def glitch_continuous():
    # let's be sure we don't fry a live target!
    # to be extra safe, don't run any assertions until continuous mode is turned off
    scope.io.glitch_lp = False
    scope.io.glitch_hp = False
    assert scope.glitch.state == 'idle'
    errors = 0
    scope.LA.trigger_source = "capture" # not sure why this is needed...
    scope.glitch.trigger_src = 'continuous'
    scope.LA.arm()
    scope.LA.trigger_now()
    time.sleep(0.1)
    if scope.LA.fifo_empty():
        errors += 1
    raw = scope.LA.read_capture_data()
    glitchenable = scope.LA.extract(raw, 6)
    if len(np.where(glitchenable != 1)[0]):
        errors += 1

    scope.glitch.trigger_src = 'manual'
    scope.LA.arm()
    scope.LA.trigger_now()
    time.sleep(0.1)
    if scope.LA.fifo_empty():
        errors += 1
    raw = scope.LA.read_capture_data()
    glitchenable = scope.LA.extract(raw, 6)
    if len(np.where(glitchenable != 0)[0]):
        errors += 1
        print("WARNING: crowbar still active, make sure target is ok")
    assert errors == 0

@pytest.mark.parametrize("module, pattern, reps, desc", testGlitchTriggerData)
#@pytest.mark.skipif(not target_attached, reason='No target detected')
def test_glitch_trigger(fulltest, module, pattern, reps, desc):
    if not fulltest:
        pytest.skip("use --fulltest to run")
        return None
    scope.reset_fpga()
    reset_setup()
    scope.default_setup(verbose=False)
    time.sleep(0.1)
    assert scope.clock.pll.pll_locked == True
    scope.glitch.enabled = True
    scope.glitch.clk_src = 'pll'
    scope.glitch.output = "enable_only"
    scope.glitch.trigger_src = 'ext_single'
    scope.glitch.repeat = 10
    scope.glitch.ext_offset = 0

    scope.io.glitch_lp = False
    scope.io.glitch_hp = False
    scope.io.hs2 = 'glitch'

    scope.LA.enabled = True
    scope.LA.oversampling_factor = 20
    scope.LA.downsample = 1
    scope.LA.capture_group = 'glitch debug'
    scope.LA.trigger_source = "trigger_glitch"
    scope.LA.capture_depth = 500

    scope.trigger.module = module
    scope.trigger.triggers = 'nrst'
    scope.trigger.edges = 4
    scope.adc.basic_mode = 'rising_edge'
    scope.adc.clip_errors_disabled = True
    scope.adc.lo_gain_errors_disabled = True

    scope.io.nrst = False
    slack = scope.LA.oversampling_factor
    expected_edges = [104, 304]

    for i in range(reps):
        scope.LA.arm()
        scope.arm()
        for j in pattern:
            scope.io.nrst = j
        scope.capture()

        assert not scope.LA.fifo_empty(), "LA capture failed"
        raw = scope.LA.read_capture_data()
        hs2 = scope.LA.extract(raw, 8)
        edges = find_edges(hs2)
        assert len(edges) == 2, 'Expected 2 glitch edges, got %d' % len(edges)
        assert not (1 in hs2[:edges[0]+1]), 'unexpected early glitch edge'
        for i in range(2):
            assert abs(edges[i] - expected_edges[i]) < slack, 'edge #%d expected near %d, found at %d' % (i+1, expected_edges[i], edges[i])


@pytest.mark.parametrize("freq, adc_mul, xtal, oversample, tolerance, reps, desc", testPLLData)
def test_pll(fulltest, freq, adc_mul, xtal, oversample, tolerance, reps, desc):
    # This test is meant to check that the relative phase between the target clock and the
    # ADC sampling clock is deterministic, i.e. for the same clock settings, the relative
    # phase will always come up the same. This test was added when it was discovered that
    # when the CDCI6214 reference divider is set to 0.5, the phase is *not* deterministic!
    # There are a bunch of asserts that can lead to the test failing (nostly around LA capture)
    # but all we're really concered about here is that for a given set of test parameters,
    # the relative phase between the ADC and target clocks is constant.
    if not fulltest and 'SLOW' in desc:
        pytest.skip("use --fulltest to run")
        return None
    if not fulltest:
        reps = 10 # reduce number of reps to speed up
    if not xtal and test_platform != 'cw305':
        pytest.skip("requires cw305 test platform")
        return None
    scope.reset_fpga()
    reset_setup()
    scope.default_setup(verbose=False)
    if xtal:
        scope.io.hs2 = 'clkgen'
    else:
        scope.io.hs2 = 'disabled'
    # initial clock setup so that LA can lock:
    if xtal:
        scope.clock.clkgen_src = 'system'
        scope.clock.clkgen_freq = freq
        scope.clock.adc_mul = adc_mul
    else:
        target.pll.pll_enable_set(True)
        target.pll.pll_outenable_set(False, 0)
        target.pll.pll_outenable_set(True, 1)
        target.pll.pll_outenable_set(False, 2)
        target.pll.pll_outfreq_set(freq, 1)
        scope.clock.clkgen_freq = freq
        scope.clock.clkgen_src = 'extclk'
        scope.clock.adc_mul = adc_mul
    # LA setup:
    scope.LA.enabled = True
    if xtal:
        scope.LA.clk_source = 'pll'
    else:
        scope.LA.clk_source = 'target'
    scope.LA.oversampling_factor = oversample
    scope.LA.capture_group = 'CW 20-pin'
    scope.LA.capture_depth = 100
    assert scope.LA.locked
    # measure phase; due to propagation delays it tends to depend with the test parameters,
    # so we measure it once and take that as the golden measurement against which we'll test:
    scope.clock.reset_adc()
    assert scope.clock.pll.pll_locked
    if xtal:
        refclk = 'hs2'
    else:
        refclk = 'target'
    exp_phase = get_adc_clock_phase(refclk)
    half_period = oversample//adc_mul//2
    if exp_phase > half_period:
        exp_phase -= half_period*2
    for i in range(reps):
        for op in ['recal', 'reset']:
            if op == 'recal':
                scope.clock.recal_pll()
            else:
                scope.clock.reset_adc()
            assert scope.clock.pll.pll_locked
            assert scope.LA.locked
            delta = get_adc_clock_phase(refclk)
            half_period = oversample//adc_mul//2
            if delta > half_period:
                delta -= 2*half_period
            if abs(delta - exp_phase) >= oversample//adc_mul//2 + tolerance:
                # this is the real error that we're testing for:
                assert False, 'Uh-oh, looks like a 180 degree phase shift! exp_phase=%d, delta=%d, op=%s, iteration=%d' % (exp_phase, delta, op, i)
            assert abs(delta - exp_phase) <= tolerance, 'Got unexpected delta %d with %s on iteration %d' % (delta, op, i)


def get_adc_clock_phase(refclk='target'):
    done = False
    count = 0
    while not done and count < 30:
        scope.LA.arm()
        scope.LA.trigger_now()
        raw = scope.LA.read_capture_data()
        adcclock = scope.LA.extract(raw, 8)
        if refclk == 'target':
            refclock = scope.LA.extract(raw, 4)
        elif refclk == 'hs2':
            refclock = scope.LA.extract(raw, 5)
        ref_edge = find0to1trans(refclock)[0]
        assert ref_edge < scope.LA.capture_depth - 30, 'got late ref_edge: %d' % ref_edge
        try:
            adc_ref_delta = find0to1trans(adcclock[ref_edge:])[0]
            done = True
        except:
            # not sure why but sometimes the ADC clock comes back all zeros; could be an issue with the PLL or with the LA?
            # what's very strange is that this doesn't happen often, but when it does, adcclock is always all zeros, and 
            # the capture is re-attempted exactly 19 times before it's successful!
            if all(c == 0 for c in adcclock):
                adcclock = 'all zeros'
            print('could not find delta; ref_edge=%3d, lock status=%s; adcclock=%s; trying again' % (ref_edge, scope.clock.pll.pll_locked, adcclock))
            assert scope.LA.locked
            assert scope.clock.pll.pll_locked
            time.sleep(0.5)
            count += 1
    return adc_ref_delta


def test_xadc():
    assert scope.XADC.status == 'good'
    if target_attached:
        # if target isn't attached, last tests run are glitch so will be hotter
        assert scope.XADC.temp < 60.0
    assert scope.XADC.max_temp < 65.0   # things can get hotter with glitching


def test_finish():
    # just restore some defaults:
    scope.default_setup(verbose=False)




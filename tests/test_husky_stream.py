# Run with pytest -v -rs

import chipwhisperer as cw
import pytest
import time
import numpy as np
import random
import os

from test_common import *

""" 
Essentially a (partial) copy of test_husky.py for running much more extensive slow stream tests, to add
coverage after https://github.com/newaetech/chipwhisperer-husky-fpga/issues/17.

Doing it with a separate script to avoid having several hundred skipped tests when running the normal 
test_husky.py regression (unfortunately, pytest doesn't allow tests to be silently skipped, although it's 
been requested and discussed extensively: https://github.com/pytest-dev/pytest/issues/3730).

There may be better ways to do this (e.g. define testTargetData differently based on some "test suite 
selector" parameter -- see https://docs.pytest.org/en/7.1.x/example/parametrize.html) but this is quick & 
dirty & it works.

Should be run with -x because after one failure, all subsequent tests are quite likely to fail.

Takes about 45 minutes to complete. Some failures have been known to occur with
the longest streams (50e6 samples).

Should be run for any change that could impact streaming, such as:
    - SAM3U firmware changes
    - FPGA FIFO changes
    - Python streaming changes
"""

test_platform = "sam4s"

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

reset_setup(scope,target)

time.sleep(0.2)
if test_platform != 'cw305':
    reset_target(scope)
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

ktp = cw.ktp.Basic()
key, text = ktp.next()

testTargetData = []

sampless = [500000, 1000000, 2000000, 5000000, 10000000, 20000000, 50000000]
presampless = [0, 10, 1000]
testmode = 'internal'
clocks = [5e6, 6e6, 7e6, 8e6, 9e6, 10e6, 11e6, 12e6, 13e6, 15e6, 20e6, 25e6]
fastreads = True
adcmul = 1
bitss = [8,12]
stream = True
thresholds = [16384, 32768, 65536]
segs = 1
segcycs = 0
for samples in sampless:
    for presamples in presampless:
        for clock in clocks:
            for bits in bitss:
                for threshold in thresholds:
                    if samples <= 1000000:
                        check = True
                    else:
                        check = False
                    if (clock > 10e6 and threshold < 65536) or (clock > 20e6 and bits == 12) or (samples > 1000000 and threshold < 65536):
                        pass
                    else:
                        testTargetData.append((samples, presamples, testmode, clock, fastreads, adcmul, bits, stream, threshold, threshold, check, segs, segcycs, 'FULL_STREAM_STRESS_TEST' ))


@pytest.mark.parametrize("samples, presamples, testmode, clock, fastreads, adcmul, bits, stream, threshold, seg_size, check, segments, segment_cycles, desc", testTargetData)
@pytest.mark.skipif(not target_attached, reason='No target detected')
def test_target_internal_ramp (fulltest, samples, presamples, testmode, clock, fastreads, adcmul, bits, stream, threshold, seg_size, check, segments, segment_cycles, desc):
    if not fulltest and 'SLOW' in desc:
        pytest.skip("use --fulltest to run")
        return None
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
    assert target.read() != '', 'target not responding'

    scope.trigger.module = 'basic'
    scope.adc.basic_mode = "rising_edge"
    scope.trigger.triggers = "tio4"
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
        assert 'overflow' in scope.adc.errors, 'overflow expected but instead scope.adc.errors = %s' % scope.adc.errors
        scope.errors.clear()
        time.sleep(2)
    else:
        assert scope.adc.errors == False, 'scope.adc.errors = %s' % scope.adc.errors
    if check: 
        errors, first_error = check_ramp(raw, testmode, bits, samples, segment_cycles)
        assert errors == 0, "%d errors; First error: %d" % (errors, first_error)
    scope.errors.clear()
    scope.sc._fast_fifo_read_enable = True # return to default



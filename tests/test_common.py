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

import time

# use this at the start of each testcase to remove dependency on order of tests:
def reset_setup(scope, target):
    scope.trigger.module = 'basic'
    scope.trigger.triggers = 'tio4'
    scope.io.tio1 = "serial_rx"
    scope.io.tio2 = "serial_tx"
    scope.io.hs2 = "clkgen"
    scope.adc.timeout = 3
    scope.adc.offset = 0
    scope.adc.stream_mode = False
    scope.glitch.enabled = False
    scope.LA.enabled = False
    scope.LA.clkgen_enabled = True
    scope.LA.capture_depth = 512
    scope.LA.downsample = 1
    scope.trace.enabled = False
    target.baud = 38400 * 10 / 7.37


def reset_target(scope):
    scope.io.nrst = 0
    time.sleep(0.2)
    scope.io.nrst = 'high_z'
    time.sleep(0.2)


def find0to1trans(data):
    pattern = [0,1]
    return [i for i in range(0,len(data)) if list(data[i:i+len(pattern)])==pattern]


def find_edges(data):
    return [i for i in range(0,len(data)) if list(data[i:i+2]) in [[0,1], [1,0]]]


def setup_glitch(scope, offset, width, oversamp):
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


def last_zero_run(a):
    # Create an array that is 1 where a is 0, and pad each end with an extra 0.
    iszero = np.concatenate(([0], np.equal(a, 0).view(np.int8), [0]))
    absdiff = np.abs(np.diff(iszero))
    # Runs start and end where absdiff is 1.
    ranges = np.where(absdiff == 1)[0].reshape(-1, 2)
    return ranges[-1]


def armed(scope):
    return scope.sc.getStatus() & 0x1


#########################################################################
# these are essentially test_X functions common to multiple test scripts:
#########################################################################

def common_fpga_version_check(scope):
    if scope._is_husky_plus:
        assert scope.fpga_buildtime == '12/16/2024, 13:17'
    else:
        assert scope.fpga_buildtime == '12/11/2024, 12:33'

def common_fw_version_check(scope):
    if scope._is_husky_plus:
        assert scope.fw_version['major'] == 1
        assert scope.fw_version['minor'] == 0
        assert scope.sam_build_date == '12:22:29 Sep 16 2024'
    else:
        assert scope.fw_version['major'] == 1
        assert scope.fw_version['minor'] == 5
        assert scope.sam_build_date == '13:17:41 Feb  9 2023'

def common_xadc_check(scope, verbose=False, error_msg=''):
    failed = False
    if verbose:
        print('\n')
        print('XADC status: %s' % scope.XADC.status)
        print('max temperature: %3.1f celcius' % scope.XADC.max_temp)
        for rail, nominal in zip(['vccint', 'vccaux', 'vccbram'],  [1.0, 1.8, 1.0]):
            for worst,limit in zip(['min', 'max'], ['lower', 'upper']):
                vseen = scope.XADC.get_vcc(rail, worst)
                vlimit = scope.XADC._get_vcc_limit(rail, limit)
                if worst == 'min':
                    vmargin = vseen - vlimit
                else:
                    vmargin = vlimit - vseen
                if vmargin > 0:
                    status = '✅ pass'
                else:
                    status = '❌ FAIL!'
                    failed = True
                print('%7s: nominal: %1.2f, %s seen: %1.2f, limit: %1.2f, margin: %1.3f   %s' % (rail, nominal, worst, vseen, vlimit, vmargin, status))
    if scope.XADC.status != 'good':
        failed = True
        print('\*** nFailing due to XADC status: %s' % scope.XADC.status)
    if scope.XADC.max_temp >= 65.0:
        failed = True
        print('\n*** Failing due to high temperature: %s' % scope.XADC.max_temp)
    assert not failed, error_msg



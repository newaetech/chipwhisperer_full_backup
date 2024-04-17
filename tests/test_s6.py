# Run with pytest -v -rf --tb=short

import chipwhisperer as cw
import pytest
import numpy as np
import warnings
import time
import random

""" 
USAGE:
    CW-Husky is used to measure and validate the glitch output from a
    Spartan6-based CW (i.e. Lite or Pro). Connect Husky HS1 to target CW's HS2
    (and ground).  The target CW drives its glitch clock output on HS2, which
    Husky samples to checks that the glitch clock offset and width are within
    acceptable margins (validation of offset is limited since Husky doesn't
    have access to the glitch source clock... previously, Husky provided the
    glitch source clock to the target CW, however this didn't work reliably due
    to noise).
    Args:
        target: Lite (default) or Pro 
        reps: number of times to run each test (default: 1)
        loose: loosen the test margins (default: False)

"""

print('\n\n\n\n**************************************************************************************')
print('*                                                                                    *')
print('* IMPORTANT NOTE:                                                                    *')
print('* This script is intended for basic regression testing of CW-lite/pro glitching      *')
print('* during  development. If you are having issues connecting to your CW-lite or target *')
print('* device, running this script is unlikely to provide you with useful information.    *')
print('* Instead, seek assistance on forum.newae.com or discord by providing details of     *')
print('* your setup (including the target), and the full error log from your Jupyter        *')
print('* notebook.                                                                          *')
print('*                                                                                    *')
print('**************************************************************************************\n\n')



hscope = None
scope = None

def test_connect(target):
    global hscope
    global scope
    hscope = cw.scope(name='Husky')
    try:
        scope = cw.scope(name=target)
    except:
        pytest.exit("Couldn't connect to target scope (%s)" % target)
    if not hscope.LA.present:
        pytest.exit('Cannot test glitch without internal logic analyzer. Rebuild Husky FPGA to test.')
    # setup Husky LA:
    hscope.trace.clock._warning_frequency = 501e6
    hscope.clock.clkgen_src = 'system'
    hscope.clock.clkgen_freq = 10e6
    hscope.LA.clkgen_enabled = True
    hscope.LA.enabled = True
    hscope.LA.oversampling_factor = 50
    hscope.LA.capture_group = 'CW 20-pin'
    hscope.LA.trigger_source = "glitch_source"
    hscope.LA.capture_depth = 512
    hscope.LA.downsample = 1
    hscope.io.hs2 = 'clkgen'
    hscope.glitch.enabled = True
    hscope.glitch.clk_src = 'pll'
    hscope.glitch.trigger_src = 'manual'
    if not hscope.LA.locked or not hscope.glitch.mmcm_locked:
        pytest.exit('Husky not locked')
    # setup target CW:
    scope.clock.clkgen_freq = 10e6
    scope.glitch.clk_src = 'target'
    scope.glitch.repeat = 1
    scope.glitch.output = 'glitch_only'
    scope.glitch.trigger_src = 'continuous'
    scope.glitch.offset = 10
    scope.glitch.width = 25
    scope.glitch.repeat = 10
    scope.io.hs2 = 'glitch'
    scope.glitch.resetDCMs()


testCoarseOffsetData = [
    #positive   width     oversamp    desc
    (False,     4,         40,         'negative-narrowest'),
    (False,     12,        40,         'negative-narrow'),
    (False,     25,        40,         'negative-mid'),
    (False,     48,        40,         'negative-widest'),

    (True,      4,         40,         'positive-narrowest'),
    (True,      12,        40,         'positive-narrow'),
    (True,      25,        40,         'positive-mid'),
    (True,      48,        40,         'positive-widest'),
]

testCoarseWidthData = [
    #positive   offset    oversamp    desc
    (False,     2,         40,         'negative-small-offset'),
    (False,     25,        40,         'negative-mid-offset'),
    (False,     48,        40,         'negative-big-offset'),

    (True,      2,         40,         'positive-small-offset'),
    (True,      25,        40,         'positive-mid-offset'),
    (True,      48,        40,         'positive-big-offset'),
]

testFineOffsetData = [
    #coarse_offset  width   oversamp    desc
    (-20,           4,      40,         'neg-narrowest'),
    (-20,           12,     40,         'neg-narrow'),
    (-20,           25,     40,         'neg-mid'),
    (-20,           48,     40,         'neg-widest'),

    (10,            4,      40,         'small-narrowest'),
    (10,            12,     40,         'small-narrow'),
    (10,            25,     40,         'small-mid'),
    (10,            48,     40,         'small-widest'),

    (25,            4,      40,         'mid-narrowest'),
    (25,            12,     40,         'mid-narrow'),
    (25,            25,     40,         'mid-mid'),
    (25,            48,     40,         'mid-widest'),

    (45,            4,      40,         'big-narrowest'),
    (45,            12,     40,         'big-narrow'),
    (45,            25,     40,         'big-mid'),
    (45,            48,     40,         'big-widest'),
]

testFineWidthData = [
    #coarse_width   offset  oversamp    desc
    (-25,           5,      40,         'neg-early'),
    (7,             5,      40,         'narrow-early'),
    (25,            5,      40,         'mid-early'),
    (45,            5,      40,         'wide-early'),

    (-25,           25,     40,         'neg-mid'),
    (7,             25,     40,         'narrow-mid'),
    (25,            25,     40,         'mid-mid'),
    (45,            25,     40,         'wide-mid'),

    (-25,           45,     40,         'neg-late'),
    (7,             45,     40,         'narrow-late'),
    (25,            45,     40,         'mid-late'),
    (45,            45,     40,         'wide-late'),
]


testGlitchOutputDoublesData = [
    #glitches   oversamp    fine_step   desc
    (1,         30,         10,         '1repeat_fast'),
    (1,         30,         1,          '1repeat_slow'),
    (2,         30,         10,         '2repeat_fast'),
    (2,         30,         1,          '2repeat_slow'),
]


testClkGen = [
    #target_freq    repeats desc
    (7.37e6,        1,      '7.37'),
    (3.2e6,         1,      'min'),
    (105e6,         1,      'max'),
    ('random',      50,     'random')
]


def test_hfpga_version():
    assert hscope.fpga_buildtime == '1/12/2024, 09:25'


def test_hfw_version():
    assert hscope.fw_version['major'] == 1
    assert hscope.fw_version['minor'] == 5
    assert hscope.sam_build_date == '13:17:41 Feb  9 2023'

def find0to1trans(data):
    pattern = [0,1]
    return [i for i in range(0,len(data)) if list(data[i:i+len(pattern)])==pattern]

# TODO(?): currently ignoring what happens when coarse offset goes from positive to negative
# TODO(?): same as above for coarse width

def set_source_clock(source):
    if source == 'Husky':
        hscope.io.hs2 = 'clkgen'
        scope.glitch.clk_src = 'target'
    elif source == 'internal':
        hscope.io.hs2 = 'disabled'
        scope.glitch.clk_src = 'clkgen'
    else:
        raise ValueError

@pytest.mark.parametrize("positive, width, oversamp, desc", testCoarseOffsetData)
def test_glitch_coarse_offset(target, reps, loose, positive, width, oversamp, desc):
    # NOTE: version of Xtest_glitch_coarse_offset which doesn't use the source clock, which limits what we can check!
    #
    # With a constant glitch width, sweep the offset and check that several measurements are within acceptable margins:
    # 0. CAN'T TEST! offset from source clock
    # 0. CAN'T TEST! as scope.glitch.offset is increased, measured offset increases too
    # 1. glitch pulse width
    # 2. glitch period
    set_source_clock('internal')
    if loose:
        width_margin = 8.0*50/oversamp
        period_margin = 4*50/oversamp
    else:
        width_margin = 3.0*50/oversamp
        period_margin = 2*50/oversamp

    if positive:
        START = 1
        STOP = 50
        INCR = 1
    else:
        START = -1
        STOP = -50
        INCR = -1

    errors = ''
    for rep in range(reps):
        if positive:
            last_offset = 0
        else:
            last_offset = 99
        # 0. collect:
        hscope.LA.oversampling_factor = oversamp
        scope.glitch.width = width
        glitchouts = []
        actual_offsets = []
        for i,o in enumerate(range(START,STOP,INCR)):
            scope.glitch.offset = float(o)
            hscope.LA.arm()
            hscope.glitch.manual_trigger()
            raw = hscope.LA.read_capture_data()
            glitchouts.append(hscope.LA.extract(raw, 4))
            actual_offsets.append(scope.glitch.offset)

        for i,o in enumerate(range(START,STOP,INCR)):
            glitch = glitchouts[i]

            # 3. verify width of each pulse:
            glitchruns = np.split(glitch, np.where(np.diff(glitch) != 0)[0]+1)
            # find first complete run of ones:
            if glitchruns[1][0]:
                glitch1runs = glitchruns[1::2]
            else:
                glitch1runs = glitchruns[2::2]
            # last run is likely incomplete so discard it:
            glitch1runs.pop()
            actual_width = scope.glitch.width
            for run in glitch1runs:
                expected_width = actual_width/100*oversamp
                diff = abs(len(run) - expected_width)
                if diff > width_margin:
                    errors += "Offset %f: Measured glitch width differs from expected width by an amount (%f) exceeding margin (%f).\n" % (actual_offsets[i], diff, width_margin)

            # 4. verify period of each pulse:
            partitions = np.where(glitch[1:] != glitch[:-1])[0]
            for j in range(1, len(partitions)-2, 2):
                period = partitions[j+2] - partitions[j]
                if abs(period - oversamp) > period_margin:
                    errors += "Offset %f: Glitch period %d exceeds expected period margin (%d)" % (actual_offsets[i], period, period_margin)

        assert errors == '', "Errors seen for rep %d:\n%s" % (rep, errors)



@pytest.mark.parametrize("positive, width, oversamp, desc", testCoarseOffsetData)
def Xtest_glitch_coarse_offset(target, reps, loose, positive, width, oversamp, desc):
    # NOTE: disabled because requires access to source clock; replaced by test_glitch_coarse_offset, which does some (but not all) of these checks
    # with a constant glitch width, sweep the offset and check that several measurements are within acceptable margins:
    # 1. offset from source clock
    # 2. as scope.glitch.offset is increased, measured offset increases too
    # 3. glitch pulse width
    # 4. glitch period
    # determined experimentally: offset seen with this setup when scope.glitch.offset = 1.0:
    set_source_clock('Husky')
    if target == 'Lite':
        base_offset =  8
    elif target == 'Pro':
        base_offset =  13
    else:
        raise ValueError
    if loose:
        lo_margin = 3.0*50/oversamp
        hi_margin = 5.0*50/oversamp
        offset_delta_margin = 2*50/oversamp
        width_margin = 8.0*50/oversamp
        period_margin = 3*50/oversamp
    else:
        lo_margin = 2.2*50/oversamp
        hi_margin = 3.5*50/oversamp
        offset_delta_margin = 1*50/oversamp
        width_margin = 3.0*50/oversamp
        period_margin = 2*50/oversamp


    if positive:
        START = 1
        STOP = 50
        INCR = 1
    else:
        START = -1
        STOP = -50
        INCR = -1

    top_margin = 0
    errors = ''
    for rep in range(reps):
        if positive:
            last_offset = 0
        else:
            last_offset = 99
        # 0. collect:
        hscope.LA.oversampling_factor = oversamp
        scope.glitch.width = width
        glitchouts = []
        sources = []
        actual_offsets = []
        observed_offsets = []
        for i,o in enumerate(range(START,STOP,INCR)):
            scope.glitch.offset = float(o)
            hscope.LA.arm()
            hscope.glitch.manual_trigger()
            raw = hscope.LA.read_capture_data()
            glitchouts.append(hscope.LA.extract(raw, 4))
            sources.append(hscope.LA.extract(raw, 5))
            actual_offsets.append(scope.glitch.offset)

        for i,o in enumerate(range(START,STOP,INCR)):
            glitch = glitchouts[i]
            source = sources[i]

            # 1. verify offset:
            glitchtrans = find0to1trans(glitch)
            sourcetrans = find0to1trans(source)
            # measure offset from first corresponding pairs of 0->1 transitions
            firstsource = sourcetrans[0]
            glitchsource = None
            for g in glitchtrans:
                if g > firstsource:
                    glitchsource = g
                    break
            if not glitchsource:
                errors += "Offset %f: no glitch found.\n" % actual_offsets[i]
            offset = glitchsource - firstsource
            observed_offsets.append(offset)
            if offset - last_offset > 45: # handle wraparound
                offset -= 50
            diff = abs((actual_offsets[i]/2) - (offset - base_offset))
            if diff >= hi_margin:
                errors += "Offset %f: Measured offset differs from expected offset by an amount (%f) exceeding margin (%f)\n" % (actual_offsets[i], diff, hi_margin)
            # 2. verify that offset is increasing:
            if positive and (offset + offset_delta_margin < last_offset):
                errors += "Offset %f: Measured decreasing offset over allowed margin: last=%f, new=%f\n" % (actual_offsets[i], last_offset, offset)
            elif not positive and (offset - offset_delta_margin > last_offset):
                errors += "Offset %f: Measured increasing offset over allowed margin: last=%f, new=%f\n" % (actual_offsets[i], last_offset, offset)
            last_offset = offset
            if diff > top_margin:
                top_margin = diff

            # 3. verify width of each pulse:
            glitchruns = np.split(glitch, np.where(np.diff(glitch) != 0)[0]+1)
            # find first complete run of ones:
            if glitchruns[1][0]:
                glitch1runs = glitchruns[1::2]
            else:
                glitch1runs = glitchruns[2::2]
            # last run is likely incomplete so discard it:
            glitch1runs.pop()
            actual_width = scope.glitch.width
            for run in glitch1runs:
                expected_width = actual_width/100*oversamp
                diff = abs(len(run) - expected_width)
                if diff > width_margin:
                    errors += "Offset %f: Measured glitch width differs from expected width by an amount (%f) exceeding margin (%f).\n" % (actual_offsets[i], diff, width_margin)

            # 4. verify period of each pulse:
            partitions = np.where(glitch[1:] != glitch[:-1])[0]
            for j in range(1, len(partitions)-2, 2):
                period = partitions[j+2] - partitions[j]
                if abs(period - oversamp) > period_margin:
                    errors += "Offset %f: Glitch period %d exceeds expected period margin (%d)" % (actual_offsets[i], period, period_margin)

        assert errors == '', "Errors seen for rep %d:\n%s" % (rep, errors)

    if top_margin > lo_margin:
        warnings.warn('High difference from expected offset seen: %f for offset=%f' % (top_margin, actual_offsets[i]))


@pytest.mark.parametrize("positive, offset, oversamp, desc", testCoarseWidthData)
def test_glitch_coarse_width(reps, loose, positive, offset, oversamp, desc):
    # with a constant glitch offset, sweep the width and check that several measurements are within acceptable margins:
    # 1. glitch width
    # 2. number of glitch pulses
    # 3. as scope.glitch.width is increased, measured width increases too
    if loose:
        width_margin = 8.0*50/oversamp
        width_delta_margin = 2*50/oversamp
        num_glitches = 7
    else:
        width_margin = 5.0*50/oversamp
        width_delta_margin = 1*50/oversamp
        num_glitches = 9

    if positive:
        START = 2
        STOP = 50
        INCR = 1
    else:
        START = -2
        STOP = -50
        INCR = -1

    errors = ''
    for rep in range(reps):
        last_width = 0
        # 0. collect:
        hscope.LA.oversampling_factor = oversamp
        scope.glitch.offset = offset
        glitchouts = []
        actual_widths = []
        observed_widths = []
        for i,o in enumerate(range(START,STOP,INCR)):
            scope.glitch.width = float(o)
            hscope.LA.arm()
            hscope.glitch.manual_trigger()
            raw = hscope.LA.read_capture_data()
            glitchouts.append(hscope.LA.extract(raw, 4))
            actual_widths.append(scope.glitch.width)

        for i,o in enumerate(range(START,STOP,INCR)):
            glitch = glitchouts[i]

            # 1. verify width of each pulse:
            glitchruns = np.split(glitch, np.where(np.diff(glitch) != 0)[0]+1)
            # find first complete run of ones:
            if glitchruns[1][0]:
                glitch1runs = glitchruns[1::2]
            else:
                glitch1runs = glitchruns[2::2]
            # last run is likely incomplete so discard it:
            glitch1runs.pop()
            for run in glitch1runs:
                expected_width = abs(actual_widths[i]/100*oversamp)
                diff = abs(len(run) - expected_width)
                if diff > width_margin:
                    errors += "Width %f: Measured glitch width differs from expected width by an amount (%f) exceeding margin (%f).\n" % (actual_widths[i], diff, width_margin)

            # 2. verify number of glitches, accounting for the possibility of missing some when width is very small:
            if abs(actual_widths[i]) < 2:
                margin = 5
            else:
                margin = 0
            if len(glitch1runs) + margin < num_glitches:
                errors += "Width %f: Didn't find expected number of glitches: %d\n" % (actual_widths[i], len(glitch1runs))

            # 3. verify that width is increasing:
            measured_width = len(glitch1runs[0])
            if measured_width + width_delta_margin < last_width:
                errors += "Width %f: Measured decreasing width over allowed margin (%d): last=%f, new=%f\n" % (actual_widths[i], width_delta_margin, last_width, measured_width)
            last_width = measured_width

        assert errors == '', "Errors seen for rep %d:\n%s" % (rep, errors)


@pytest.mark.parametrize("coarse_offset, width, oversamp, desc", testFineOffsetData)
def Xtest_glitch_fine_offset(reps, loose, coarse_offset, width, oversamp, desc):
    # NOTE: disabled because requires access to source clock; nothing really that we can usefully check without this that isn't already
    # covered by test_coarse_offset.
    #
    # With a constant glitch width and coarse offset, sweep the fine offset and check that several measurements are within acceptable margins:
    # 1. as fine offset is increased, the measured offset increases too
    # 2. glitch pulse width
    # 3. glitch period
    # 4. the difference in observed offset between the minimum and maximum offset settings is within range
    # Note that we don't check the offset itself, because it's checked by test_coarse_offset (with fine offset = 0).
    # The offset with different fine offset settings is therefore implicitly checked by the combination of these two tests.

    base_offset =  8 # determined experimentally: offset seen with this setup when scope.glitch.offset = 1.0
    if loose:
        offset_decrease_margin = 2*50/oversamp
        offset_increase_margin = 4*50/oversamp
        width_margin = 4.0*50/oversamp
        period_margin = 3*50/oversamp
        range_max = 9
        range_min = 2
    else:
        offset_decrease_margin = 1*50/oversamp
        offset_increase_margin = 3*50/oversamp
        width_margin = 3.0*50/oversamp
        period_margin = 2*50/oversamp
        range_max = 7
        range_min = 2


    STEPS = 31
    INCR = 511//STEPS

    errors = ''
    for rep in range(reps):
        last_offset = 0
        # 0. collect:
        hscope.LA.oversampling_factor = oversamp
        scope.glitch.width = width
        scope.glitch.offset = coarse_offset
        glitchouts = []
        sources = []
        actual_offsets = []
        observed_offsets = []
        scope.glitch.offset_fine = -255
        assert scope.glitch.offset_fine == -255, "Looks like we're trying to operate out of range here!"
        for i in range(STEPS):
            actual_offsets.append(scope.glitch.offset_fine)
            hscope.LA.arm()
            hscope.glitch.manual_trigger()
            raw = hscope.LA.read_capture_data()
            glitchouts.append(hscope.LA.extract(raw, 4))
            sources.append(hscope.LA.extract(raw, 5))
            scope.glitch.offset_fine += INCR

        for i in range(STEPS):
            glitch = glitchouts[i]
            source = sources[i]
            # measure offset:
            glitchtrans = find0to1trans(glitch)
            sourcetrans = find0to1trans(source)
            # measure offset from first corresponding pairs of 0->1 transitions
            firstsource = sourcetrans[0]
            glitchsource = None
            for g in glitchtrans:
                if g > firstsource:
                    glitchsource = g
                    break
            if not glitchsource:
                errors += "Offset %f: no glitch found.\n" % actual_offsets[i]
            offset = glitchsource - firstsource
            observed_offsets.append(offset)
            if offset - last_offset > 45: # handle wraparound
                offset -= 50
            elif last_offset - offset > 45:
                offset += 50

            # 1. verify that offset is increasing:
            if offset + offset_decrease_margin < last_offset:
                errors += "Offset %f: Measured decreasing offset exceeding allowed margin (%d): last=%f, new=%f\n" % (actual_offsets[i], offset_decrease_margin, last_offset, offset)
            if (i > 0) and (offset > last_offset + offset_increase_margin):
                errors += "Offset %f: Measured offset increase exceeding allowed margin (%d): last=%f, new=%f\n" % (actual_offsets[i], offset_increase_margin, last_offset, offset)
            last_offset = offset

            # 3. verify width of each pulse:
            # TODO: should move this to separate function because this is shared with coarse test... bunch of other stuff too, surely!
            glitchruns = np.split(glitch, np.where(np.diff(glitch) != 0)[0]+1)
            # find first complete run of ones:
            if glitchruns[1][0]:
                glitch1runs = glitchruns[1::2]
            else:
                glitch1runs = glitchruns[2::2]
            # last run is likely incomplete so discard it:
            glitch1runs.pop()
            actual_width = scope.glitch.width
            for run in glitch1runs:
                expected_width = actual_width/100*oversamp
                diff = abs(len(run) - expected_width)
                if diff > width_margin:
                    errors += "Offset %f: Glitch width diff (%f) exceeds margin (%f) too much.\n" % (actual_offsets[i], diff, width_margin)

            # 4. verify period of each pulse:
            partitions = np.where(glitch[1:] != glitch[:-1])[0]
            for i in range(1, len(partitions)-2, 2):
                period = partitions[i+2] - partitions[i]
                if abs(period - oversamp) > period_margin:
                    errors += "Offset %f: Glitch period %d exceeds expected period margin (%d)" % (actual_offsets[i], period, period_margin)

        # 4. verify that the difference in observed offset between the minimum and maximum fine offset settings is within range
        diff = observed_offsets[-1] - observed_offsets[0]
        if diff < 0:
            diff += 50
        if (diff > range_max) or (diff < range_min):
            errors += "Difference between min and max offset (%d) out of acceptable range; expected range: [%d,%d]\n" % (diff, range_min, range_max)

        assert errors == '', "Errors seen for rep %d:\n%s" % (rep, errors)


@pytest.mark.parametrize("coarse_width, offset, oversamp, desc", testFineWidthData)
def test_glitch_fine_width(reps, loose, coarse_width, offset, oversamp, desc):
    # with a constant glitch offset and coarse offset, sweep the fine width and check that several measurements are within acceptable margins:
    # 1. as fine width is increased, the measured width increases too
    # 2. number of glitch pulses
    # 3. the difference in observed width between the minimum and maximum width settings is within range
    # Note that we don't check the width itself, because it's checked by test_coarse_width (with fine width = 0).
    # The width with different fine width settings is therefore implicitly checked by the combination of these two tests.

    if loose:
        width_decrease_margin = 1*50/oversamp
        width_increase_margin = 3*50/oversamp
        range_max = 9
        range_min = 2
        num_glitches = 7
    else:
        width_decrease_margin = 1*50/oversamp
        width_increase_margin = 3*50/oversamp
        range_max = 7
        range_min = 2
        num_glitches = 9

    STEPS = 31
    INCR = 511//STEPS

    errors = ''
    for rep in range(reps):
        if coarse_width > 0:
            last_width = 0
        else:
            last_width = 99
        # 0. collect:
        hscope.LA.oversampling_factor = oversamp
        scope.glitch.offset = offset
        scope.glitch.width = coarse_width
        glitchouts = []
        actual_widths = []
        observed_widths = []
        scope.glitch.width_fine = -255
        assert scope.glitch.width_fine == -255, "Looks like we're trying to operate out of range here!"
        for i in range(STEPS):
            actual_widths.append(scope.glitch.width_fine)
            hscope.LA.arm()
            hscope.glitch.manual_trigger()
            raw = hscope.LA.read_capture_data()
            glitchouts.append(hscope.LA.extract(raw, 4))
            scope.glitch.width_fine += INCR

        for i in range(STEPS):
            glitch = glitchouts[i]

            # measure pulse width:
            glitchruns = np.split(glitch, np.where(np.diff(glitch) != 0)[0]+1)
            # find first complete run of ones:
            if glitchruns[1][0]:
                glitch1runs = glitchruns[1::2]
            else:
                glitch1runs = glitchruns[2::2]
            # last run is likely incomplete so discard it, to make sure we don't run any tests on it
            glitch1runs.pop()
            width = len(glitch1runs[0])
            observed_widths.append(width)

            # 1. verify that width is increasing:
            if coarse_width > 0:
                if width + width_decrease_margin < last_width:
                    errors += "Width %f: Measured decreasing width exceeding allowed margin (%d): last=%f, new=%f\n" % (actual_widths[i], width_decrease_margin, last_width, width)
                if (i > 0) and (width > last_width + width_increase_margin):
                    errors += "Width %f: Measured width increase exceeding allowed margin (%):  last=%f, new=%f\n" % (actual_widths[i], width_increase_margin, last_width, width)
            else:
                if last_width + width_decrease_margin < width:
                    errors += "Width %f: Measured increasing width exceeding allowed margin (%d): last=%f, new=%f\n" % (actual_widths[i], width_decrease_margin, last_width, width)
                if (i > 0) and (last_width > width + width_increase_margin):
                    errors += "Width %f: Measured width decrease exceeding allowed margin (%):  last=%f, new=%f\n" % (actual_widths[i], width_increase_margin, last_width, width)
            last_width = width

            # 2. verify number of glitches, accounting for the possibility of missing some when width is very small:
            if abs(coarse_width) < 2:
                margin = 5
            else:
                margin = 0
            if len(glitch1runs) + margin < num_glitches:
                errors += "Width %f: Didn't find expected number of glitches: %d" % (actual_widths[i], len(glitch1runs))

        # 3. verify that the difference in observed width between the minimum and maximum fine width settings is within range
        diff = observed_widths[-1] - observed_widths[0]
        if coarse_width < 0:
            # with a negative coarse width, fine adjustments move in the opposite direction!
            diff = -diff

        if (diff > range_max) or (diff < range_min):
            errors += "Difference between min and max width (%d) out of acceptable range; expected range: [%d-%d]\n" % (diff, range_min, range_max)

        assert errors == '', "Errors seen for rep %d:\n%s" % (rep, errors)


@pytest.mark.parametrize("glitches, oversamp, fine_step, desc", testGlitchOutputDoublesData)
def test_glitch_output_doubles(reps, glitches, oversamp, fine_step, desc):
    # by looking at the glitch enable output, we can infer the presence of double glitches even if the glitches themselves are too narrow
    # to be caught by our too-low sampling rate
    set_source_clock('internal')
    scope.glitch.repeat = glitches
    failing_offsets = []
    maxwidth = 0

    scope.glitch.output = 'enable_only'
    scope.glitch.trigger_src = 'manual'
    hscope.LA.trigger_source = "HS1"
    hscope.LA.oversampling_factor = oversamp

    maxwidth = 0
    failing_offsets = []
    good = 0
    bad = 0

    for r in range(reps):
        for i in range(30):
            offset_coarse = -49.8 + i*0.3
            scope.glitch.offset = offset_coarse
            for offset_fine in range(-255, 255, fine_step):
                scope.glitch.offset_fine = offset_fine
                if scope.glitch.offset_fine != offset_fine:
                    continue
                hscope.LA.arm()
                scope.glitch.manual_trigger()
                raw = hscope.LA.read_capture_data()
                glitchout = hscope.LA.extract(raw, 4)
                glitchlen = len(np.where(glitchout > 0)[0])
                cycles = glitchlen/oversamp

                if (glitchlen and (abs(glitchlen/glitches - oversamp) > oversamp/4)) or (len(find0to1trans(glitchout)) > 0):
                    bad += 1
                    failing_offsets.append([scope.glitch.offset, offset_fine])
                    if glitchlen > maxwidth:
                        maxwidth = glitchlen
                elif glitchlen:
                    good += 1

    assert failing_offsets == [], "Max width seen: %d; failing offsets: %s" % (maxwidth, failing_offsets)
    assert not bad, "Out of %d tests, double glitches seen in %d cases." % (good+bad, bad)
    # since this test is known and expected to fail, just issue a warning?
    #warnings.warn("Out of %d tests, double glitches seen in %d cases." % (good+bad, bad))


@pytest.mark.parametrize("target_freq, repeats, desc", testClkGen)
def test_clkgen(target_freq, repeats, desc):
    # Check that generate clock is what it should be (to catch https://github.com/newaetech/chipwhisperer/issues/340)
    set_source_clock('internal')
    scope.clock.freq_ctr_src = 'clkgen'
    scope.clock.adc_src = 'clkgen_x1'
    scope.io.hs2 = 'clkgen'
    hscope.clock.clkgen_src = 'system'
    hscope.clock.clkgen_freq = 10e6
    hscope.LA.enabled = True
    hscope.LA.oversampling_factor = 40
    hscope.LA.capture_group = 'CW 20-pin'
    hscope.LA.trigger_source = "HS1"
    hscope.LA.capture_depth = 1024
    hscope.LA.downsample = 1
    hscope.io.hs2 = 'disabled'
    MARGIN = 1 # deviation from expected frequency in percentage
    for i in range(repeats):
        errors = ''
        if target_freq == 'random':
            target_freq = random.randint(3.2e6, 105e6)
        scope.clock.clkgen_freq = target_freq
        # First check via target scope's own measurements:
        time.sleep(0.3)
        if abs(target_freq - scope.clock.clkgen_freq)/target_freq*100 > MARGIN:
            errors += 'scope.clock.clkgen_freq out of range; '
        if abs(target_freq - scope.clock.freq_ctr)/target_freq*100 > MARGIN:
            errors += 'scope.clock.freq_ctr out of range; '

        # Then to be extra-sure, check via Husky LA's measurements; this allows us to check the duty cycle as well:
        hscope.LA.arm()
        raw = hscope.LA.read_capture_data()
        clockout = hscope.LA.extract(raw, 4)
        max_period = 0
        min_period = 1e6
        partitions = np.where(clockout[1:] != clockout[:-1])[0]
        for j in range(1, len(partitions)-2, 2):
            period = partitions[j+2] - partitions[j]
            if period > max_period:
                max_period = period
            if period < min_period:
                min_period = period

        expected_period = hscope.LA.sampling_clock_frequency / target_freq
        if target_freq < 10e6:
            PERIOD_MARGIN = 2
        else:
            PERIOD_MARGIN = 1
        if abs(max_period - expected_period) > PERIOD_MARGIN:
            errors += 'max_period out of range; '
        if abs(min_period - expected_period) > PERIOD_MARGIN:
            errors += 'min_period out of range; '
        assert errors == '', 'target_freq=%f, freq_ctr=%f, clkgen_freq=%f, max_period=%d, min_period=%d, expected_period=%d' % (target_freq, scope.clock.freq_ctr, scope.clock.clkgen_freq, max_period, min_period, expected_period)


def test_hxadc():
    assert hscope.XADC.status == 'good'
    #assert hscope.XADC.temp < 60.0 # things get a bit toasty with this test

def test_shutdown():
    # not actually a test, just turn off Husky MMCMs when we're done
    hscope.LA.enabled = False
    hscope.LA.clkgen_enabled = False
    hscope.glitch.enabled = False




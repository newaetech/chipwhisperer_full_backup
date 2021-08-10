# Run with pytest -v -rf --tb=short

import chipwhisperer as cw
import pytest
import numpy as np
import warnings

""" 
USAGE:
    CW-Husky is used to measure and validate the glitch output from a
    Spartan6-based CW (i.e. Lite or Pro). Connect Husky HS1 to target CW's
    HS2 and vice-versa (and ground).
    Husky generates a 10 MHz clock, which the target CW uses as its glitch
    source clock. The target CW drives its glitch clock output on HS2. Husky
    samples both these clocks (at 500 MS/s) and checks that the glitch clock
    offset and width are within acceptable margins. The glitch output runs
    continously as there is no mechanism to synchronize the capture with a
    discrete glitch event in this setup.
    Args:
        target: Lite (default) or Pro 
        reps: number of times to run each test (default: 1)
        loose: loosen the test margins (default: False)

"""


@pytest.fixture()
def target(pytestconfig):
    return pytestconfig.getoption("target")

@pytest.fixture()
def reps(pytestconfig):
    return int(pytestconfig.getoption("reps"))

@pytest.fixture()
def loose(pytestconfig):
    return int(pytestconfig.getoption("loose"))



hscope = None
scope = None

def test_connect(target):
    global hscope
    global scope
    hscope = cw.scope(name='Husky')
    try:
        scope = cw.scope(name=target)
    except:
        pytest.exit("Couldn't connect to target scope")
    if not hscope.LA.present:
        pytest.exit('Cannot test glitch without internal logic analyzer. Rebuild Husky FPGA to test.')
    # setup Husky LA:
    hscope.clock.clkgen_src = 'system'
    hscope.clock.clkgen_freq = 10e6
    hscope.LA.enabled = True
    hscope.LA.oversampling_factor = 50
    hscope.LA.capture_group = 1 # 20-pin header
    hscope.LA.trigger_source = "glitch_source"
    hscope.io.hs2 = 'clkgen'
    hscope.glitch.enabled = True
    hscope.glitch.clk_src = 'pll'
    hscope.glitch.trigger_src = 'manual'
    if not hscope.LA.locked or not hscope.glitch.mmcm_locked:
        pytest.exit('Husky not locked')
    # setup target CW:
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
    (False,     4,         50,         'negative-narrowest'),
    (False,     12,        50,         'negative-narrow'),
    (False,     25,        50,         'negative-mid'),
    (False,     48,        50,         'negative-widest'),

    (True,      4,         50,         'positive-narrowest'),
    (True,      12,        50,         'positive-narrow'),
    (True,      25,        50,         'positive-mid'),
    (True,      48,        50,         'positive-widest'),
]

testCoarseWidthData = [
    #positive   offset    oversamp    desc
    (False,     2,         50,         'negative-small-offset'),
    (False,     25,        50,         'negative-mid-offset'),
    (False,     48,        50,         'negative-big-offset'),

    (True,      2,         50,         'positive-small-offset'),
    (True,      25,        50,         'positive-mid-offset'),
    (True,      48,        50,         'positive-big-offset'),
]

testFineOffsetData = [
    #coarse_offset  width   oversamp    desc
    (-20,           4,      50,         'neg-narrowest'),
    (-20,           12,     50,         'neg-narrow'),
    (-20,           25,     50,         'neg-mid'),
    (-20,           48,     50,         'neg-widest'),

    (10,            4,      50,         'small-narrowest'),
    (10,            12,     50,         'small-narrow'),
    (10,            25,     50,         'small-mid'),
    (10,            48,     50,         'small-widest'),

    (25,            4,      50,         'mid-narrowest'),
    (25,            12,     50,         'mid-narrow'),
    (25,            25,     50,         'mid-mid'),
    (25,            48,     50,         'mid-widest'),

    (45,            4,      50,         'big-narrowest'),
    (45,            12,     50,         'big-narrow'),
    (45,            25,     50,         'big-mid'),
    (45,            48,     50,         'big-widest'),
]

testFineWidthData = [
    #coarse_width   offset  oversamp    desc
    (-25,           5,      50,         'neg-early'),
    (7,             5,      50,         'narrow-early'),
    (25,            5,      50,         'mid-early'),
    (45,            5,      50,         'wide-early'),

    (-25,           25,     50,         'neg-mid'),
    (7,             25,     50,         'narrow-mid'),
    (25,            25,     50,         'mid-mid'),
    (45,            25,     50,         'wide-mid'),

    (-25,           45,     50,         'neg-late'),
    (7,             45,     50,         'narrow-late'),
    (25,            45,     50,         'mid-late'),
    (45,            45,     50,         'wide-late'),
]


testGlitchOutputDoublesData = [
    # TODO: swith to finer finer_step when current failures are fixed
    #glitches   oversamp    fine_step   desc
    (1,         30,         5,          ''),
    (2,         30,         5,          ''),
]


def test_hfpga_version():
    assert hscope.fpga_buildtime == '8/9/2021, 22:56'


def test_hfw_version():
    assert hscope.fw_version['major'] == 1
    assert hscope.fw_version['minor'] == 10
    assert hscope.sam_build_date == 'Jun 24 2021'

def find0to1trans(data):
    pattern = [0,1]
    return [i for i in range(0,len(data)) if list(data[i:i+len(pattern)])==pattern]

# TODO(?): currently ignoring what happens when coarse offset goes from positive to negative
# TODO(?): same as above for coarse width

@pytest.mark.parametrize("positive, width, oversamp, desc", testCoarseOffsetData)
def test_coarse_offset(target, reps, loose, positive, width, oversamp, desc):
    # with a constant glitch width, sweep the offset and check that several measurements are within acceptable margins:
    # 1. offset from source clock
    # 2. as scope.glitch.offset is increased, measured offset increases too
    # 3. glitch pulse width
    # 4. glitch period
    # determined experimentally: offset seen with this setup when scope.glitch.offset = 1.0:
    if target == 'Lite':
        base_offset =  8
    elif target == 'Pro':
        base_offset =  13
    else:
        raise ValueError
    if loose:
        lo_margin = 3.0
        hi_margin = 5.0
        offset_delta_margin = 2
        width_margin = 8.0
        period_margin = 3
    else:
        lo_margin = 2.2
        hi_margin = 3.5
        offset_delta_margin = 1
        width_margin = 3.0
        period_margin = 2


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
        hscope.LA.oversampling_factor = oversamp # TODO: logic is actually hard-coded for oversamp=50 and 10 MHz clock
        scope.glitch.width = width
        glitchouts = []
        sources = []
        actual_offsets = []
        observed_offsets = []
        for i,o in enumerate(range(START,STOP,INCR)):
            scope.glitch.offset = float(o)
            hscope.glitch.manual_trigger()
            glitchouts.append(hscope.LA.read_capture(4))
            sources.append(hscope.LA.read_capture(5))
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
                expected_width = actual_width/100*50
                diff = abs(len(run) - expected_width)
                if diff > width_margin:
                    errors += "Offset %f: Measured glitch width differs from expected width by an amount (%f) exceeding margin (%f).\n" % (actual_offsets[i], diff, width_margin)

            # 4. verify period of each pulse:
            partitions = np.where(glitch[1:] != glitch[:-1])[0]
            for i in range(1, len(partitions)-2, 2):
                period = partitions[i+2] - partitions[i]
                if abs(period - 50) > period_margin:
                    errors += "Offset %f: Glitch period %d exceeds expected period margin (%d)" % (actual_offsets[i], period, period_margin)

        assert errors == '', "Errors seen for rep %d:\n%s" % (rep, errors)

    if top_margin > lo_margin:
        warnings.warn('High difference from expected offset seen: %f for offset=%f' % (top_margin, actual_offsets[i]))


@pytest.mark.parametrize("positive, offset, oversamp, desc", testCoarseWidthData)
def test_coarse_width(reps, loose, positive, offset, oversamp, desc):
    # with a constant glitch offset, sweep the width and check that several measurements are within acceptable margins:
    # 1. glitch width
    # 2. number of glitch pulses
    # 3. as scope.glitch.width is increased, measured width increases too
    if loose:
        width_margin = 8.0
        width_delta_margin = 2
        num_glitches = 7
    else:
        width_margin = 3.0
        width_delta_margin = 1
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
        hscope.LA.oversampling_factor = oversamp # TODO: logic is actually hard-coded for oversamp=50 and 10 MHz clock
        scope.glitch.offset = offset
        glitchouts = []
        actual_widths = []
        observed_widths = []
        for i,o in enumerate(range(START,STOP,INCR)):
            scope.glitch.width = float(o)
            hscope.glitch.manual_trigger()
            glitchouts.append(hscope.LA.read_capture(4))
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
                expected_width = abs(actual_widths[i]/100*50)
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
def test_fine_offset(reps, loose, coarse_offset, width, oversamp, desc):
    # with a constant glitch width and coarse offset, sweep the fine offset and check that several measurements are within acceptable margins:
    # 1. as fine offset is increased, the measured offset increases too
    # 2. glitch pulse width
    # 3. glitch period
    # 4. the difference in observed offset between the minimum and maximum offset settings is within range
    # Note that we don't check the offset itself, because it's checked by test_coarse_offset (with fine offset = 0).
    # The offset with different fine offset settings is therefore implicitly checked by the combination of these two tests.

    base_offset =  8 # determined experimentally: offset seen with this setup when scope.glitch.offset = 1.0
    if loose:
        offset_decrease_margin = 2
        offset_increase_margin = 4
        width_margin = 4.0
        period_margin = 3
        range_max = 9
        range_min = 2
    else:
        offset_decrease_margin = 1
        offset_increase_margin = 3
        width_margin = 3.0
        period_margin = 2
        range_max = 7
        range_min = 2


    STEPS = 31
    INCR = 511//STEPS

    errors = ''
    for rep in range(reps):
        last_offset = 0
        # 0. collect:
        hscope.LA.oversampling_factor = oversamp # TODO: logic is actually hard-coded for oversamp=50 and 10 MHz clock
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
            hscope.glitch.manual_trigger()
            glitchouts.append(hscope.LA.read_capture(4))
            sources.append(hscope.LA.read_capture(5))
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
                expected_width = actual_width/100*50
                diff = abs(len(run) - expected_width)
                if diff > width_margin:
                    errors += "Offset %f: Glitch width diff (%f) exceeds margin (%f) too much.\n" % (actual_offsets[i], diff, width_margin)

            # 4. verify period of each pulse:
            partitions = np.where(glitch[1:] != glitch[:-1])[0]
            for i in range(1, len(partitions)-2, 2):
                period = partitions[i+2] - partitions[i]
                if abs(period - 50) > period_margin:
                    errors += "Offset %f: Glitch period %d exceeds expected period margin (%d)" % (actual_offsets[i], period, period_margin)

        # 4. verify that the difference in observed offset between the minimum and maximum fine offset settings is within range
        diff = observed_offsets[-1] - observed_offsets[0]
        if diff < 0:
            diff += 50
        if (diff > range_max) or (diff < range_min):
            errors += "Difference between min and max offset (%d) out of acceptable range; expected range: [%d,%d]\n" % (diff, range_min, range_max)

        assert errors == '', "Errors seen for rep %d:\n%s" % (rep, errors)


@pytest.mark.parametrize("coarse_width, offset, oversamp, desc", testFineWidthData)
def test_fine_width(reps, loose, coarse_width, offset, oversamp, desc):
    # with a constant glitch offset and coarse offset, sweep the fine width and check that several measurements are within acceptable margins:
    # 1. as fine width is increased, the measured width increases too
    # 2. number of glitch pulses
    # 3. the difference in observed width between the minimum and maximum width settings is within range
    # Note that we don't check the width itself, because it's checked by test_coarse_width (with fine width = 0).
    # The width with different fine width settings is therefore implicitly checked by the combination of these two tests.

    if loose:
        width_decrease_margin = 1
        width_increase_margin = 3
        range_max = 9
        range_min = 2
        num_glitches = 7
    else:
        width_decrease_margin = 1
        width_increase_margin = 3
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
        hscope.LA.oversampling_factor = oversamp # TODO: logic is actually hard-coded for oversamp=50 and 10 MHz clock
        scope.glitch.offset = offset
        scope.glitch.width = coarse_width
        glitchouts = []
        actual_widths = []
        observed_widths = []
        scope.glitch.width_fine = -255
        assert scope.glitch.width_fine == -255, "Looks like we're trying to operate out of range here!"
        for i in range(STEPS):
            actual_widths.append(scope.glitch.width_fine)
            hscope.glitch.manual_trigger()
            glitchouts.append(hscope.LA.read_capture(4))
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
    scope.glitch.repeat = glitches
    failing_offsets = []
    maxwidth = 0

    scope.glitch.output = 'enable_only'
    scope.glitch.trigger_src = 'manual'
    scope.glitch.repeat = 1
    hscope.LA.trigger_source = "HS1"
    hscope.LA.oversampling_factor = oversamp

    maxwidth = 0
    failing_offsets = []
    good = 0
    bad = 0
    reps = 1 # TODO: override reps because there is no sense in repeating a test that is known to fail

    for r in range(reps):
        for i in range(20):
            # TODO: sweep a wider offset_coarse range once current failures are fixed
            offset_coarse = -49 + i*0.5
            scope.glitch.offset = offset_coarse
            for offset_fine in range(-255, 255, fine_step):
                scope.glitch.offset_fine = offset_fine
                if scope.glitch.offset_fine != offset_fine:
                    continue
                scope.glitch.manual_trigger()
                glitchout = hscope.LA.read_capture(4)
                glitchlen = len(np.where(glitchout > 0)[0])
                cycles = glitchlen/oversamp

                if glitchlen and (abs(glitchlen/glitches - oversamp) > oversamp/4):
                    bad += 1
                    failing_offsets.append([scope.glitch.offset, offset_fine])
                    if glitchlen > maxwidth:
                        maxwidth = glitchlen
                elif glitchlen:
                    good += 1

    #assert failing_offsets == [], "Max width seen: %d; failing offsets: %s" % (maxwidth, failing_offsets) # TODO: this is too noisy when there are so many failures
    assert not bad, "Out of %d tests, double glitches seen in %d cases." % (good+bad, bad)
    # since this test is known and expected to fail, just issue a warning?
    #warnings.warn("Out of %d tests, double glitches seen in %d cases." % (good+bad, bad))


def test_hxadc():
    assert hscope.XADC.status == 'good'
    assert hscope.XADC.temp < 60.0 # things get a bit toasty with this test

def test_shutdown():
    # not actually a test, just turn off Husky MMCMs when we're done
    hscope.LA.enabled = False
    hscope.glitch.enabled = False




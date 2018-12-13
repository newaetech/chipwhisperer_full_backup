"""Example for scripting glitching of the stm32f3 target using the chipwhisperer
tool. Similar to what the glitch explorer does. This script does not spawn a
gui, and uses the 5.0 api.
"""

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
scope.adc.samples = 5000
scope.adc.offset = 0
scope.adc.basic_mode = "rising_edge"
scope.clock.clkgen_freq = 7370000
scope.clock.adc_src = "clkgen_x4"
scope.trigger.triggers = "tio4"
scope.io.tio1 = "serial_rx"
scope.io.tio2 = "serial_tx"
scope.io.hs2 = "glitch"

target.go_cmd = ""
target.key_cmd = ""

# program the STM32F3 with the built hex file
prog = cw.programmers.STM32FProgrammer
fw_path = "../../hardware/victims/firmware/glitch-simple/glitchsimple-CWLITEARM.hex"

cw.programTarget(scope, prog, fw_path)

import time
def reset_target(scope):
    scope.io.nrst = 'low'
    time.sleep(0.05)
    scope.io.nrst = 'high'

# format output table
headers = ['target output', 'width', 'offset', 'success']
glitch_display = GlitchResultsDisplay(headers)

# set glitch parameters
# trigger glitches with external trigger
scope.glitch.trigger_src = 'ext_single'
scope.glitch.ext_offset = 37
scope.glitch.repeat = 10

# named tuples to make it easier to change the scope of the test
Range = namedtuple('Range', ['min', 'max', 'step'])
width_range = Range(-40, 10, 1)
offset_range = Range(-49, -30, 1)

scope.glitch.width = width_range.min
target.init()

attack1_data = []
sample_size = 5

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

print('Done')

# clean up the connection to the scope and target
scope.dis()
target.dis()

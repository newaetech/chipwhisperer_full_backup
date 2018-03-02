"""Example for scripting capturing of traces during AES encryption
with the chipwhisperer tool. This script does not spawn a gui, and
uses the 4.0 api.
"""

from __future__ import division, print_function

import time
import os

import numpy as np
import matplotlib.pyplot as plt
from datetime import datetime
from tqdm import tqdm

import chipwhisperer as cw
from chipwhisperer.capture.acq_patterns.basic import AcqKeyTextPattern_Basic
from chipwhisperer.tests.tools_for_tests import FIRMWARE_DIR
from chipwhisperer.capture.api.programmers import XMEGAProgrammer

scope = cw.scope()
target = cw.target(scope)

# setup scope parameters
scope.gain.gain = 45
scope.adc.samples = 3000
scope.adc.offset = 1250
scope.adc.basic_mode = "rising_edge"
scope.clock.clkgen_freq = 7370000
scope.clock.adc_src = "clkgen_x4"
scope.trigger.triggers = "tio4"
scope.io.tio1 = "serial_rx"
scope.io.tio2 = "serial_tx"
scope.io.hs2 = "clkgen"

# program the target
xmega = XMEGAProgrammer()
xmega.setUSBInterface(scope.scopetype.dev.xmega)
xmega._logging = None
xmega.find()
xmega.erase()
glitch_simple_firmware_dir = os.path.join(FIRMWARE_DIR, 'simpleserial-aes')
glitch_simple_hex = os.path.join(glitch_simple_firmware_dir, r"simpleserial-aes-CW303.hex")
xmega.program(glitch_simple_hex, memtype="flash", verify=True)
xmega.close()

ktp = AcqKeyTextPattern_Basic(target=target)

traces = []
N = 50  # Number of traces
for i in tqdm(range(N)):
    # run aux stuff that should come before trace here

    key, text = ktp.newPair()  # manual creation of a key, text pair can be substituted here

    target.reinit()

    target.setModeEncrypt()  # only does something for targets that support it
    target.loadEncryptionKey(key)
    target.loadInput(text)

    # run aux stuff that should run before the scope arms here

    scope.arm()

    # run aux stuff that should run after the scope arms here

    target.go()
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

    # run aux stuff that should happen after trace here

    traces.append(scope.getLastTrace())

trace_array = np.asarray(traces)  # if you prefer to work with numpy array for number crunching
now = datetime.now()
with open('trace_array_{:02}{:02}.npy'.format(now.hour, now.minute), 'w') as f:
    np.save(f, trace_array)  # save to a file for later processing

# show an example trace
plt.plot(traces[0])
plt.show()

# cleanup the connection to the target and scope
scope.dis()
target.dis()

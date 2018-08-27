"""Example for scripting capturing of traces during AES encryption
with the chipwhisperer tool. This script does not spawn a gui, and
uses the 4.0 api.
"""

import time
import numpy as np
import matplotlib.pyplot as plt
from datetime import datetime
from tqdm import tqdm
import os
import logging

import chipwhisperer as cw
from chipwhisperer.tests.tools_for_tests import FIRMWARE_DIR
from chipwhisperer.capture.api.programmers import XMEGAProgrammer

logging.basicConfig(level=logging.INFO)

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

# program the XMEGA with the built hex file
programmer = XMEGAProgrammer()
programmer.scope = scope
programmer._logging = None
programmer.find()
programmer.erase()
simple_serial_firmware_dir = os.path.join(FIRMWARE_DIR, 'simpleserial-aes')
simple_hex = os.path.join(simple_serial_firmware_dir, r"simpleserial-aes-xmega.hex")
programmer.program(simple_hex, memtype="flash", verify=True)
programmer.close()

traces = []
textin = []
keys = []
N = 50  # Number of traces
target.init()
for i in tqdm(range(N), desc='Capturing traces'):
    # run aux stuff that should come before trace here

    text = [0]*16
    key = [0]*16
    textin.append(text)
    keys.append(key)

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
    _ = target.readOutput()  # clears the response from the serial port
    traces.append(scope.getLastTrace())
trace_array = np.asarray(traces)  # if you prefer to work with numpy array for number crunching
textin_array = np.asarray(textin)
known_keys = np.asarray(keys)  # for fixed key, these keys are all the same

now = datetime.now()
fmt_string = '{:02}{:02}_{}.npy'
trace_file_path = fmt_string.format(now.hour, now.minute, "traces")
textin_file_path = fmt_string.format(now.hour, now.minute, "textins")
keys_file_path = fmt_string.format(now.hour, now.minute, "keys")

print('Saving results to {},{} and {}...'.format(trace_file_path, textin_file_path, keys_file_path), end='')
# save to a files for later processing
np.save(trace_file_path, trace_array)
np.save(textin_file_path, textin_array)
np.save(keys_file_path, known_keys)
print('Done')

# uncomment plt.show() to show an example trace
plt.plot(traces[0])
#plt.show()

# cleanup the connection to the target and scope
scope.dis()
target.dis()

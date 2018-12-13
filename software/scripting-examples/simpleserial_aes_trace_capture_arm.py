"""Example for scripting capturing of traces during AES encryption
with the chipwhisperer tool. This script does not use Jupyter, and
uses the 5.0 api.
"""

import time
import os

import numpy as np
import matplotlib.pyplot as plt
from datetime import datetime
from tqdm import trange

import chipwhisperer as cw

scope = cw.scope()
target = cw.target(scope)

# setup scope parameters
scope.gain.gain = 45
scope.adc.samples = 5000
scope.adc.offset = 0
scope.adc.basic_mode = "rising_edge"
scope.clock.clkgen_freq = 7370000
scope.clock.adc_src = "clkgen_x4"
scope.trigger.triggers = "tio4"
scope.io.tio1 = "serial_rx"
scope.io.tio2 = "serial_tx"
scope.io.hs2 = "clkgen"

# program the target
prog = cw.programmers.STM32FProgrammer
fw_path = "../../hardware/victims/firmware/simpleserial-aes/simpleserial-aes-cwlitearm.hex"

cw.programTarget(scope, prog, fw_path)

ktp = cw.ktp.Basic(target=target)

traces = []
textin = []
keys = []
N = 50  # Number of traces
target.init()
for i in trange(N, desc='Capturing traces'):
    # run aux stuff that should come before trace here

    key, text = ktp.newPair()  # manual creation of a key, text pair can be substituted here
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

    _ = target.readOutput() # throw out the target response
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

# show an example trace
plt.plot(traces[0])
plt.show()

# cleanup the connection to the target and scope
scope.dis()
target.dis()

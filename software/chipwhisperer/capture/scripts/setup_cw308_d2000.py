"""Setup script for CWLite/1200 with CW308T-D2000 Settings

Configures scope settings to prepare for capturing SimpleSerial power traces
"""

import time

# GUI compatibility
try:
    scope = self.scope
except NameError:
    pass

print "***********************************************"
print "1. Ensure J3 on UFO Board routines CLKFB to HS1/IN"
print "2. Confirm baud settings"
    
scope.gain.gain = 45
scope.adc.samples = 25000
scope.adc.offset = 0
scope.adc.basic_mode = "rising_edge"
scope.clock.clkgen_freq = 7370000
scope.clock.adc_src = "extclk_x4"
scope.trigger.triggers = "tio4"
#TX/RX backwards from XMEGA
scope.io.tio1 = "serial_tx"
scope.io.tio2 = "serial_rx"
scope.io.hs2 = None

#Sample delay - you may need to increase this!
time.sleep(0.5)

ext_freq = scope.clock.freq_ctr

if ext_freq > 10:

    baud = (115200 / 32E6) * ext_freq
    print "Based on extclk of %d Hz, setting baud to %d" % (ext_freq, baud)
    target.baud = baud
    
else:
    raise IOError("Did not detect external clock from D2000. Confirm jumpers and rerun, or increase delay before sample."
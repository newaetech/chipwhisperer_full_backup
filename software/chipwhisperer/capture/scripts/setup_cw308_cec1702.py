"""Setup script for CWLite/1200 with CW308T-CEC1702 Settings

Configures scope settings to prepare for capturing SimpleSerial power traces
"""

# GUI compatibility
try:
    scope = self.scope
except NameError:
    pass

print("***********************************************")
print("1. Ensure J3 on UFO Board routines CLKFB to HS1/IN")
    
scope.gain.gain = 40
scope.gain.mode = "high"
scope.adc.samples = 400
scope.adc.offset = 0
scope.adc.basic_mode = "rising_edge"
scope.clock.clkgen_src = "extclk"
scope.clock.clkgen_mul = 8
scope.clock.clkgen_div = 2
scope.clock.adc_src = "clkgen_x1"
scope.trigger.triggers = "tio4"
scope.io.tio1 = "serial_rx"
scope.io.tio2 = "serial_tx"
scope.io.hs2 = None


ext_freq = scope.clock.freq_ctr
print("Input frequency of %d Hz" % ext_freq)

if ext_freq > 11850000 and ext_freq < 12150000:
    print("CLK frequency in expected range.")
else:
    print("WARNING: CLK frequency outside of expected range.")
    

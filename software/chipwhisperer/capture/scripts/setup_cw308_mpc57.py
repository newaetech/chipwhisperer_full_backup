"""Setup script for CWLite/1200 with MPC5748G

Configures scope settings to prepare for capturing SimpleSerial power traces
"""

# GUI compatibility
try:
    scope = self.scope
except NameError:
    pass
    
scope.gain.gain = 45
scope.adc.samples = 5000
scope.adc.offset = 0
scope.adc.basic_mode = "rising_edge"
scope.clock.clkgen_freq = 16000000
scope.clock.adc_src = "clkgen_x4"
scope.trigger.triggers = "tio4"
scope.io.tio1 = "serial_rx"
scope.io.tio2 = "serial_tx"
scope.io.hs2 = "clkgen"

target.baud = 38400

print "Ensure CW308 Board has +5.0V power supply"

# NOTE: If needed you can toggle target power. MPC5748G sometimes needs power cycle to come up.
#import time
#scope.io.target_pwr = False
#time.sleep(0.25)
#scope.io.target_pwr = True



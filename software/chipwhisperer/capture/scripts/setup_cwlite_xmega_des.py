"""Setup script for CWLite/1200 with XMEGA (CW303/CW308-XMEGA/CWLite target)

Configures scope settings to prepare for capturing SimpleSerial power traces
"""

# GUI compatibility
try:
    scope = self.scope
except NameError:
    pass
    
scope.gain.gain = 45
scope.adc.samples = 3500
scope.adc.offset = 15500
scope.adc.basic_mode = "rising_edge"
scope.clock.clkgen_freq = 7370000
scope.clock.adc_src = "clkgen_x4"
scope.trigger.triggers = "tio4"
scope.io.tio1 = "serial_rx"
scope.io.tio2 = "serial_tx"
scope.io.hs2 = "clkgen"

target.key_len = 8
target.input_len = 8
target.output_len = 8

#The following shows the "old" method of accessing elements where the new API
#is not yet ready.
self.api.setParameter(['Generic Settings', 'Basic', 'Fixed Encryption Key', u'2B 7E 15 16 28 AE D2 A6'])

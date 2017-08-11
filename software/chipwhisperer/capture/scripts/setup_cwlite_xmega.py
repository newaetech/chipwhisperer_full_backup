"""Setup script for CWLite/1200 with XMEGA (CW303/CW308-XMEGA/CWLite target)
"""

if self is not None:
    scope = self.scope
    
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
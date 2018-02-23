"""Setup script for CWLite/1200 with XMEGA (CW303/CW308-XMEGA/CWLite target)
specifically for Tutorial A5: the AES-256 bootloader attack
"""

try:
    scope = self.scope
except NameError:
    pass
    
scope.gain.gain = 45
scope.adc.samples = 11000
scope.adc.offset = 0
scope.adc.basic_mode = "rising_edge"
scope.clock.clkgen_freq = 7370000
scope.clock.adc_src = "clkgen_x4"
scope.trigger.triggers = "tio4"
scope.io.tio1 = "serial_rx"
scope.io.tio2 = "serial_tx"
scope.io.hs2 = "clkgen"

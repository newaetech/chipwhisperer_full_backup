"""Setup script for CWLite/1200 with ARM (stm32f3)

Configures scope settings to prepare for capturing SimpleSerial power traces
"""

# GUI compatibility
try:
    scope = self.scope
except NameError:
    pass
    
scope.gain.gain = 55
scope.adc.samples = 25000
scope.adc.offset = 0
scope.adc.basic_mode = "rising_edge"
scope.clock.clkgen_freq = 7370000
scope.clock.adc_src = "clkgen_x1"
scope.trigger.triggers = "tio4"
scope.io.tio1 = "serial_rx"
scope.io.tio2 = "serial_tx"
scope.io.hs2 = "clkgen"
scope.qtadc.sc._nosampletimeout = 250

target.baud = 38400

self.api.setParameter(['Generic Settings', 'Basic', 'Key', 'Random'])
self.api.setParameter(['Generic Settings', 'Basic', 'Plaintext', 'Fixed'])
self.api.setParameter(['Generic Settings', 'Basic', 'Fixed Plaintext', '00 00 00 00 00 00 00 00 00 00 00 00 00 00 80 00'])

target.key_cmd = ""
target.output_cmd = ""
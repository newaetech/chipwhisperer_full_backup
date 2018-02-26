"""Setup script for CWLite/1200 with XMEGA (CW303/CW308-XMEGA/CWLite target)

Configures scope settings to prepare for capturing SimpleSerial power traces
"""

# GUI compatibility
try:
    scope = self.scope
    target = self.target
except NameError:
    pass

scope.glitch.clk_src = 'clkgen'
scope.glitch.ext_offset = 68
scope.glitch.width = 3.0
scope.glitch.offset = -5.0
scope.glitch.trigger_src = "ext_single"

scope.gain.gain = 45
scope.adc.samples = 500
scope.adc.offset = 0
scope.adc.basic_mode = "rising_edge"
scope.clock.clkgen_freq = 7370000
scope.clock.adc_src = "clkgen_x4"
scope.trigger.triggers = "tio4"
scope.io.tio1 = "serial_rx"
scope.io.tio2 = "serial_tx"
scope.io.hs2 = "glitch"

target.go_cmd = "p516261276720736265747267206762206f686c207a76797821\\n"
target.key_cmd = ""
target.output_cmd = ""

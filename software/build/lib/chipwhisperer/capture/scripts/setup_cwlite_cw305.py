"""Setup script for CWLite/1200 with CW305 target
"""

# GUI compatibility
try:
    scope = self.scope
except NameError:
    pass
    
scope.gain.gain = 30
scope.gain.mode = "high"
scope.adc.samples = 250
scope.adc.offset = 0
scope.adc.basic_mode = "rising_edge"
scope.clock.adc_src = "extclk_x4"
scope.clock.adc_phase = 255
scope.trigger.triggers = "tio4"

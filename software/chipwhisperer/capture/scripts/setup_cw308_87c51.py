"""Setup script for CWLite/1200 with XMEGA (CW303/CW308-XMEGA/CWLite target)

Configures scope settings to prepare for capturing SimpleSerial power traces
"""
import time    

# GUI compatibility
try:
    scope = self.scope
    target = self.target
except NameError:
    pass

scope.gain.gain = 45
scope.adc.samples = 5000
scope.adc.offset = 0
scope.adc.basic_mode = "rising_edge"
scope.clock.clkgen_freq = 7370000
scope.clock.adc_src = "clkgen_x4"
scope.trigger.triggers = "tio4"
scope.io.tio1 = "serial_rx"
scope.io.tio2 = "serial_tx"
scope.io.hs2 = "clkgen"

target.baud = 4800

#Reset chip - this is NEEDED for device to start.
#Also you can reset it 
def reset_8751():
    scope.io.tio3 = False
    time.sleep(0.05)
    scope.io.tio3 = True
    time.sleep(0.05)
    scope.io.tio3 = False
    
reset_8751()

print "**************************************************************"
print "Use jumpers on H1/H3/H5 to select application.                "
print "Default should work by opening terminal & see testing printed."
print "Call reset_8751() to reset target (needed when changing jumpers"
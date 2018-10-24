"""Setup script for CWLite/1200 with XMEGA (CW303/CW308-XMEGA/CWLite target)

Configures scope settings to prepare for capturing SimpleSerial power traces
"""

# GUI compatibility
try:
    scope = self.scope
except NameError:
    pass
    
self.scope.adc.clk_freq = 7.5E6
self.scope.io.clkout = 7.5E6
self.scope.io.nrst = False
self.scope.io.pdic = False
self.scope.io.nrst = True

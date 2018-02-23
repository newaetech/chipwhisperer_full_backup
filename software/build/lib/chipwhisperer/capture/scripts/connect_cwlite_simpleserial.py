"""Connection setup script for CW-Lite/1200 and SimpleSerial

Connects to a ChipWhisperer-Lite/1200, then to a SimpleSerial target
Run this before preparing the CWLite settings!
"""

import chipwhisperer as cw

scope = cw.scope()
self.scope = scope

target = cw.target(scope)
self.target = target
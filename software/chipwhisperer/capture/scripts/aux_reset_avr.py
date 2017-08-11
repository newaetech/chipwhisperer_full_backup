"""aux_reset_avr.py
"""

import time
from subprocess import call
import chipwhisperer.capture.auxiliary.ResetAVR

# GUI-only
if self is not None:
    ac = self.ac

AVRResetter = ResetAVR()
ac.registerAux(AVRResetter.traceArm, "before_arm")

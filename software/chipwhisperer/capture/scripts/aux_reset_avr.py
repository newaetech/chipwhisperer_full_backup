"""aux_reset_avr.py

Sets up AVR reset aux module
"""

import time
from subprocess import call
from chipwhisperer.capture.auxiliary.ResetAVR import ResetAVR

# GUI compatibility
try:
    aux_list = self.aux_list
except NameError:
    pass

AVRResetter = ResetAVR()
aux_list.register(AVRResetter.traceArm, "before_arm")

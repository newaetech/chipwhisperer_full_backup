"""aux_reset_avr.py
"""

import time
from subprocess import call

# GUI-only
if self is not None:
    ac = self.ac

# Aux function
def do_thing(scope, target, ac):
    # Whatever
    x = "asd"

# Register function to trigger before arming scope
ac.registerAux(do_thing, "before_arm")

# Other functions
"""
ac.disableAux(reset_avr, "before_arm")
ac.enableAux(reset_avr, "before_arm")
ac.setAuxTiming(reset_avr, "before_arm", "after_capture")
ac.removeAux(reset_avr, "before_arm")
"""
"""Set up resets via CW1173
Contains a few adjustable lines to switch between XMEGA/AVR and change reset
timing (relative to scope arm)
"""

from chipwhisperer.capture.auxiliary.ResetCW1173Read import ResetCW1173

# GUI compatibility
try:
    aux_list = self.aux_list
except NameError:
    pass

# Delay between arming and resetting, in ms
delay_ms = 1000

# Reset XMEGA device
Resetter = ResetCW1173(xmega=True, delay_ms=delay_ms)
# Reset AVR
#Resetter = ResetCW1173(xmega=False, delay_ms=delay_ms)

# Reset before arming - more stable
aux_list.register(Resetter.resetThenDelay, "before_arm")
# Reset after arming - scope can catch entire reset
#aux_list.register(Resetter.delayThenReset, "after_arm")

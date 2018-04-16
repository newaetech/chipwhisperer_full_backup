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
Resetter = ResetCW1173(pin='pdic', delay_ms=delay_ms)
# Reset STM32Fx device
#Resetter = ResetCW1173(pin='nrst', delay_ms=delay_ms)
# Reset AVR
#Resetter = ResetCW1173(pin='nrst', delay_ms=delay_ms)

# Reset before arming
# avoids possibility of false triggers
# need delay in target firmware to avoid race condition
#aux_list.register(Resetter.resetThenDelay, "before_trace")

# Reset after arming
# scope can catch entire reset
# avoids race condition
# target reset can cause false triggers (usually not an issue)
aux_list.register(Resetter.delayThenReset, "after_arm")

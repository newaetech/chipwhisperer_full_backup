"""Sets up AVR reset aux module
"""

from chipwhisperer.capture.auxiliary.ResetAVR import ResetAVR

# GUI compatibility
try:
    aux_list = self.aux_list
except NameError:
    pass

AVRResetter = ResetAVR()
aux_list.register(AVRResetter.reset, "before_arm")

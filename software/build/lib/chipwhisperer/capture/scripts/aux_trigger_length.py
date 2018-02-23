"""Record the trigger length for each trace into an auxiliary project file.

Useful for checking whether targets are vulnerable to timing attacks
"""

from chipwhisperer.capture.auxiliary.recordtriggerlength import RecordTriggerLength

# GUI compatibility
try:
    aux_list = self.aux_list
except NameError:
    pass

recorder = RecordTriggerLength()
aux_list.register(recorder.traceDone, "after_trace")
aux_list.register(recorder.captureComplete, "after_capture")

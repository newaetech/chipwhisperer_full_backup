"""Measure the frequency of some signal using a PicoScope 5000A
"""

from chipwhisperer.capture.auxiliary.FrequencyMeasure import FrequencyMeasure

# GUI compatibility
try:
    aux_list = self.aux_list
except NameError:
    pass

freq_measure = FrequencyMeasure()
aux_list.register(freq_measure.setPrefix, "set_prefix")
aux_list.register(freq_measure.traceArm, "before_arm")
aux_list.register(freq_measure.traceDone, "after_trace")
aux_list.register(freq_measure.captureComplete, "after_capture")

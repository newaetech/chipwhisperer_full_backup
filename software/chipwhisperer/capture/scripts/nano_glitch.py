import time
try:
    scope = self.scope
    target = self.target
except NameError:
    pass

# Glitcher
class LPC_glitch(object):
    def __init__(self, ge_window, api):
        self.ge_window = ge_window
        self.api = api

    def update_parameters(self, scope, target, project):
        pass
        # scope.glitch.ext_offset += 1
        # if scope.glitch.ext_offset > 8000:
            # scope.glitch.ext_offset = 1000
            # scope.glitch.repeat += 1

    def check_read_protect(self, scope, target, project):
        pass
        # driver = target.ser
        # driver.flush()
        # driver.write("?")
        # time.sleep(0.05)
        # driver.write("Synchronized\r\n")
        # time.sleep(0.05)
        # driver.write("12000\r\n")
        # time.sleep(0.05)
        # driver.write("R 0 4\r\n")

    def rst_low(self, scope, target, project):
        scope.io.nrst = 'low'

    def rst_high(self, scope, target, project):
        scope.io.nrst = 'high'



#Create and register glitcher
glitcher = LPC_glitch(self.glitch_explorer, self.api)
self.aux_list.register(glitcher.update_parameters, "before_trace")
self.aux_list.register(glitcher.rst_low, "before_arm")
self.aux_list.register(glitcher.rst_high, "after_arm")
self.aux_list.register(glitcher.check_read_protect, "after_trace")

scope.adc.clk_freq = 5654646546 #just needs to be non zero
scope.glitch.width = 7
scope.glitch.offset=400000


target.baud = 38400


self.project.setTraceFormat(None)
import chipwhisperer as cw
#from chipwhisperer import gui
#from chipwhisperer.acq_controller import basic

project = cw.create_project(r'c:\temp\aes.cwp', overwrite=True)

#gui.show()

scope = cw.scope()
scope.gain_db = 20
scope.trigger.mode = "rising edge"
scope.trigger.timeout = 2
scope.trigger.offset = 1250
scope.trigger.samples = 3000
scope.trigger.presamples = 500

#scope.clkgen.freq = 7.37E6
#scope.adcclk.src = "clkgen_x4"
#scope.reset_clocks()

#scope.trigger_pins.tio4 = True
#scope.trigger_pins.mode = "OR"
#scope.target_hsout = "clkgen"

#scope.glitchhp = False
#scope.glitchlp = False
#scope.target_power = False
#scope.glitch.source = "clkgen"
#scope.glitch.manual()

#ac = basic(scope, target, project)
#ac.test_capture(N=1)

#gui.register(scope)
#gui.register(target)
#gui.register(ac)

#ac.capture(N=1000000, segments=1)

#print scope.adcclk.freq
#print scope.adcclk.samplerate
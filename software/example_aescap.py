import chipwhisperer as cw
#from chipwhisperer.acq_controller import basic

project = cw.create_project(r'c:\temp\aes.cwp', overwrite=True)

#scope = target.api

scope = cw.scope()


scope.gain_db = 20
scope.trigger.basic_mode = "rising edge"
scope.trigger.timeout = 2
scope.trigger.offset = 1250
scope.trigger.samples = 3000
scope.trigger.presamples = 500

scope.clock.clkgen_freq = 7.37E6
scope.clock.adc_reset()
scope.clock.adc_src = "clkgen_x4"
scope.clock.adc_reset()

scope.gpiomux.tio1 = "Serial RXD"
scope.gpiomux.tio2 = "Serial TXD"
scope.gpiomux.tio4 = "High-Z"
scope.gpiomux.tio3 = True

scope.gpiomux.hs2 = "clkout"

#scope.gpiomux.reset_target()

#scope.gpiomux.hs2 = "glitchout"

#scope.gpiomux.extclk_src = "hs2"

#scope.crowbar.highpower = True
#scope.crowbar.lowpower = True

scope.triggermux.triggers = "tio4"

#scope.triggermux.mode = "basic"

#scope.crowbar.highpower = False
#scope.crowbar.lowpower = False
#scope.target_power = False
#scope.glitch.source = "clkgen"
#scope.glitch.manual()

#ac = basic(scope, target, project)
#ac.test_capture(N=1)

#ac.capture(N=1000000, segments=1)


gui.capture()


print scope.trigger.offset
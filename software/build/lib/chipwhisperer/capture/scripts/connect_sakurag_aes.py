"""Connect and Setup script for SAKURA-G loaded with OpenADC and ChipWhisperer Bitstream

Configures scope settings to prepare for capturing power traces
"""
import chipwhisperer.common.utils.util as util

#Old stype API - example of setting things using strings
self.api.setParameter(['Generic Settings', 'Scope Module', 'ChipWhisperer/OpenADC'])
self.api.setParameter(['ChipWhisperer/OpenADC', 'Connection', 'FTDI (SASEBO-W/SAKURA-G)'])
self.api.setParameter(['ChipWhisperer/OpenADC', 'FTDI (SASEBO-W/SAKURA-G)', 'Refresh Device List', None])
self.api.setParameter(['Generic Settings', 'Target Module', 'SAKURA G'])
self.api.setParameter(['SAKURA G', 'Connection via:', 'CW Bitstream, with OpenADC'])

#Call into GUI for connect/disconnect (due to old API)
self.doConDis()

#New style API for configuring scope settings
scope = self.scope

scope.clock.clkgen_div = 2
scope.gain.gain = 40
scope.adc.samples = 400
scope.adc.offset = 0
scope.adc.basic_mode = "falling_edge"
scope.clock.adc_src = "clkgen_x1"

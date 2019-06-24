*************
OpenADC Scope
*************

.. autoclass:: chipwhisperer.scopes.OpenADC

    .. attribute:: gain
        :annotation: scope.gain

        .. autodata:: chipwhisperer.capture.scopes._OpenADCInterface.GainSettings.db
            :annotation: scope.gain.db

        .. autodata:: chipwhisperer.capture.scopes._OpenADCInterface.GainSettings.mode
            :annotation: scope.gain.mode

        .. autodata:: chipwhisperer.capture.scopes._OpenADCInterface.GainSettings.gain
            :annotation: scope.gain.gain

        .. autodata:: chipwhisperer.capture.scopes._OpenADCInterface.GainSettings.db
            :annotation: scope.gain.db

    .. attribute:: adc
        :annotation: scope.adc

    .. attribute:: clock
        :annotation: scope.clock

        .. autodata:: chipwhisperer.capture.scopes._OpenADCInterface.ClockSettings.adc_src
            :annotation: scope.clock.adc_src

        .. autodata:: chipwhisperer.capture.scopes._OpenADCInterface.ClockSettings.adc_phase
            :annotation: scope.clock.adc_phase

        .. autodata:: chipwhisperer.capture.scopes._OpenADCInterface.ClockSettings.adc_freq
            :annotation: scope.clock.adc_freq

        .. autodata:: chipwhisperer.capture.scopes._OpenADCInterface.ClockSettings.adc_rate
            :annotation: scope.clock.adc_rate

        .. autodata:: chipwhisperer.capture.scopes._OpenADCInterface.ClockSettings.adc_locked
            :annotation: scope.clock.adc_locked

        .. autodata:: chipwhisperer.capture.scopes._OpenADCInterface.ClockSettings.freq_ctr
            :annotation: scope.clock.freq_ctr

        .. autodata:: chipwhisperer.capture.scopes._OpenADCInterface.ClockSettings.freq_ctr_src
            :annotation: scope.clock.freq_ctr_src

        .. autodata:: chipwhisperer.capture.scopes._OpenADCInterface.ClockSettings.clkgen_src
            :annotation: scope.clock.clkgen_src

        .. autodata:: chipwhisperer.capture.scopes._OpenADCInterface.ClockSettings.extclk_freq
            :annotation: scope.clock.extclk_freq

        .. autodata:: chipwhisperer.capture.scopes._OpenADCInterface.ClockSettings.clkgen_mul
            :annotation: scope.clock.clkgen_mul

        .. autodata:: chipwhisperer.capture.scopes._OpenADCInterface.ClockSettings.clkgen_div
            :annotation: scope.clock.clkgen_div

        .. autodata:: chipwhisperer.capture.scopes._OpenADCInterface.ClockSettings.clkgen_freq
            :annotation: scope.clock.clkgen_freq

        .. autodata:: chipwhisperer.capture.scopes._OpenADCInterface.ClockSettings.clkgen_locked
            :annotation: scope.clock.clkgen_locked


    .. attribute:: io
        :annotation: scope.io

        .. autodata:: chipwhisperer.capture.scopes.cwhardware.ChipWhispererExtra.GPIOSettings.tio1
            :annotation: scope.io.tio1

        .. autodata:: chipwhisperer.capture.scopes.cwhardware.ChipWhispererExtra.GPIOSettings.tio2
            :annotation: scope.io.tio2

        .. autodata:: chipwhisperer.capture.scopes.cwhardware.ChipWhispererExtra.GPIOSettings.tio3
            :annotation: scope.io.tio3

        .. autodata:: chipwhisperer.capture.scopes.cwhardware.ChipWhispererExtra.GPIOSettings.tio4
            :annotation: scope.io.tio4

        .. autodata:: chipwhisperer.capture.scopes.cwhardware.ChipWhispererExtra.GPIOSettings.pdid
            :annotation: scope.io.pdid

        .. autodata:: chipwhisperer.capture.scopes.cwhardware.ChipWhispererExtra.GPIOSettings.pdic
            :annotation: scope.io.pdic

        .. autodata:: chipwhisperer.capture.scopes.cwhardware.ChipWhispererExtra.GPIOSettings.nrst
            :annotation: scope.io.nrst

        .. autodata:: chipwhisperer.capture.scopes.cwhardware.ChipWhispererExtra.GPIOSettings.glitch_hp
            :annotation: scope.io.glitch_hp

        .. autodata:: chipwhisperer.capture.scopes.cwhardware.ChipWhispererExtra.GPIOSettings.extclk_src
            :annotation: scope.io.extclk_src

        .. autodata:: chipwhisperer.capture.scopes.cwhardware.ChipWhispererExtra.GPIOSettings.hs2
            :annotation: scope.io.hs2

        .. autodata:: chipwhisperer.capture.scopes.cwhardware.ChipWhispererExtra.GPIOSettings.target_pwr
            :annotation: scope.io.target_pwr

    .. attribute:: trigger
        :annotation: scope.trigger

        .. autodata:: chipwhisperer.capture.scopes.cwhardware.ChipWhispererExtra.TriggerSettings.triggers
            :annotation: scope.trigger.triggers

        .. autodata:: chipwhisperer.capture.scopes.cwhardware.ChipWhispererExtra.TriggerSettings.module
            :annotation: scope.trigger.module

        .. # beginning of of the OpenADc Interface

        .. autodata:: chipwhisperer.capture.scopes._OpenADCInterface.TriggerSettings.state
            :annotation: scope.trigger.state

        .. autodata:: chipwhisperer.capture.scopes._OpenADCInterface.TriggerSettings.stream_mode
            :annotation: scope.trigger.stream_mode

        .. autodata:: chipwhisperer.capture.scopes._OpenADCInterface.TriggerSettings.decimate
            :annotation: scope.trigger.decimate

        .. autodata:: chipwhisperer.capture.scopes._OpenADCInterface.TriggerSettings.samples
            :annotation: scope.trigger.samples

        .. autodata:: chipwhisperer.capture.scopes._OpenADCInterface.TriggerSettings.timeout
            :annotation: scope.trigger.timeout

        .. autodata:: chipwhisperer.capture.scopes._OpenADCInterface.TriggerSettings.offset
            :annotation: scope.trigger.offset

        .. autodata:: chipwhisperer.capture.scopes._OpenADCInterface.TriggerSettings.presamples
            :annotation: scope.trigger.presamples

        .. autodata:: chipwhisperer.capture.scopes._OpenADCInterface.TriggerSettings.basic_mode
            :annotation: scope.trigger.basic_mode

        .. autodata:: chipwhisperer.capture.scopes._OpenADCInterface.TriggerSettings.trig_count
            :annotation: scope.trigger.trig_count

    .. attribute:: glitch
        :annotation: scope.glitch

        .. autodata:: chipwhisperer.capture.scopes.cwhardware.ChipWhispererGlitch.GlitchSettings.clk_src
            :annotation: scope.glitch.clk_src

        .. autodata:: chipwhisperer.capture.scopes.cwhardware.ChipWhispererGlitch.GlitchSettings.width
            :annotation: scope.glitch.width

        .. autodata:: chipwhisperer.capture.scopes.cwhardware.ChipWhispererGlitch.GlitchSettings.width_fine
            :annotation: scope.glitch.width_fine

        .. autodata:: chipwhisperer.capture.scopes.cwhardware.ChipWhispererGlitch.GlitchSettings.offset
            :annotation: scope.glitch.offset

        .. autodata:: chipwhisperer.capture.scopes.cwhardware.ChipWhispererGlitch.GlitchSettings.offset_fine
            :annotation: scope.glitch.offset_fine

        .. autodata:: chipwhisperer.capture.scopes.cwhardware.ChipWhispererGlitch.GlitchSettings.trigger_src
            :annotation: scope.glitch.trigger_src

        .. autodata:: chipwhisperer.capture.scopes.cwhardware.ChipWhispererGlitch.GlitchSettings.arm_timing
            :annotation: scope.glitch.arm_timing

        .. autodata:: chipwhisperer.capture.scopes.cwhardware.ChipWhispererGlitch.GlitchSettings.ext_offset
            :annotation: scope.glitch.ext_offset

        .. autodata:: chipwhisperer.capture.scopes.cwhardware.ChipWhispererGlitch.GlitchSettings.repeat
            :annotation: scope.glitch.repeat

        .. autodata:: chipwhisperer.capture.scopes.cwhardware.ChipWhispererGlitch.GlitchSettings.output
            :annotation: scope.glitch.output

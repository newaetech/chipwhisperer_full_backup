
.. _api-scope:

*********
Scope API
*********

The scope object is used to control the capture/glitch
portion of the ChipWhisperer device.

The easiest way to create a scope object is via the :func:`chipwhisperer.scope` function,
which will connect to a ChipWhisperer device and return a scope object of the correct type::

    import chipwhisperer as cw
    scope = cw.scope()

There are currently two types of scopes:

 * :ref:`api-scope-openadc` (Lite, Pro, Husky)
 * :ref:`api-scope-cwnano` (Nano)

These scope objects also inherit common methods from :ref:`api-scope-common`

.. autodata:: chipwhisperer.scope
    :annotation: chipwhisperer.scope()

As of ChipWhisperer 5.6.2, ChipWhisperer can find all connected devices:

.. autodata:: chipwhisperer.list_devices
    :annotation: chipwhisperer.list_devices()

.. _api-scope-openadc:

OpenADC Scope
=============

Supported scopes:

 * `ChipWhisperer-Lite <https://rtfm.newae.com/Capture/ChipWhisperer-Lite/>`_
 * `ChipWhisperer-Pro <https://rtfm.newae.com/Capture/ChipWhisperer-Pro/>`_
 * `ChipWhisperer-Husky <https://www.crowdsupply.com/newae/chipwhisperer-husky>`_

Usage examples:

 * `Power Analysis <https://github.com/newaetech/chipwhisperer-jupyter/tree/master/courses/sca101>`_
 * `Glitch <https://github.com/newaetech/chipwhisperer-jupyter/tree/master/courses/fault101>`_
 * `Pro features <https://github.com/newaetech/chipwhisperer-jupyter/blob/master/demos/CW-Pro%20Using%20SAD%20(Analog%20Pattern)%20Trigger%20-%20Attacking%20AES.ipynb>`_
 * `Husky glitching <https://github.com/newaetech/chipwhisperer-jupyter/blob/master/demos/husky_glitch.ipynb>`_

.. autoclass:: chipwhisperer.scopes.OpenADC
    :members:
    :exclude-members: enable_MPSSE, upgrade_firmware

scope.gain
----------

Class to control ADC gain

    .. autoclass:: chipwhisperer.capture.scopes._OpenADCInterface.GainSettings
        :members:

scope.adc
---------

Class to control non-gain, non-clock ADC settings

    .. autoclass:: chipwhisperer.capture.scopes._OpenADCInterface.TriggerSettings
        :members: 
        :exclude-members: stream_mode, segment_cycles, segment_cycle_counter_en, first_error, first_error_state, errors, segments, bits_per_sample
        

scope.adc Pro/Husky Only
^^^^^^^^^^^^^^^^^^^^^^^^

The following :code:`scope.adc` members are only available on ChipWhisperer-Husky, ChipWhisperer-Pro, or both.

    .. autoclass:: chipwhisperer.capture.scopes._OpenADCInterface.TriggerSettings
        :noindex:

    .. autoproperty:: chipwhisperer.capture.scopes._OpenADCInterface.TriggerSettings.stream_mode

    .. autoproperty:: chipwhisperer.capture.scopes._OpenADCInterface.TriggerSettings.segment_cycles

    .. autoproperty:: chipwhisperer.capture.scopes._OpenADCInterface.TriggerSettings.segment_cycle_counter_en

    .. autoproperty:: chipwhisperer.capture.scopes._OpenADCInterface.TriggerSettings.first_error_state

    .. autoproperty:: chipwhisperer.capture.scopes._OpenADCInterface.TriggerSettings.errors

    .. autoproperty:: chipwhisperer.capture.scopes._OpenADCInterface.TriggerSettings.segments

    .. autoproperty:: chipwhisperer.capture.scopes._OpenADCInterface.TriggerSettings.bits_per_sample

scope.clock
-----------

scope.clock (Lite/Pro Only)
^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. warning:: ChipWhisperer-Lite/Pro only. See 
            :attr:`scope.clock (Husky) <chipwhisperer.capture.scopes.cwhardware.ChipWhispererHuskyClock.ChipWhispererHuskyClock>` for Husky clock documentation.

Class to control target/ADC clocks. A block diagram of the clock module is shown below:

    .. thumbnail:: figures/cwlitepro_clock.png

    .. autoclass:: chipwhisperer.capture.scopes._OpenADCInterface.ClockSettings
        :members:

scope.clock (Husky Only)
^^^^^^^^^^^^^^^^^^^^^^^^

    .. autoclass:: chipwhisperer.capture.scopes.cwhardware.ChipWhispererHuskyClock.ChipWhispererHuskyClock
        :members:

scope.io
--------

Module to control IO pins

    .. autoclass:: chipwhisperer.capture.scopes.cwhardware.ChipWhispererExtra.GPIOSettings
        :members:

scope.trigger
-------------

Basic trigger control module

    .. autoclass:: chipwhisperer.capture.scopes.cwhardware.ChipWhispererExtra.TriggerSettings
        :members:

scope.trigger (Pro/Husky Only)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    .. autoclass:: chipwhisperer.capture.scopes.cwhardware.ChipWhispererExtra.ProTrigger
        :members:

scope.glitch
------------

Module to control glitching. A block diagram of the module is shown below:

        .. thumbnail:: figures/cwlitepro_glitch.png

    .. autoclass:: chipwhisperer.capture.scopes.cwhardware.ChipWhispererGlitch.GlitchSettings
        :members:
        :exclude-members: enabled, mmcm_locked, phase_shift_steps

scope.glitch (Husky Only)
^^^^^^^^^^^^^^^^^^^^^^^^^

The following attributes are only available on, or differ substantially on the ChipWhisperer-Husky

    .. autoclass:: chipwhisperer.capture.scopes.cwhardware.ChipWhispererGlitch.GlitchSettings
        :noindex:

    .. autoattribute:: chipwhisperer.capture.scopes.cwhardware.ChipWhispererGlitch.GlitchSettings.enabled

    .. autoattribute:: chipwhisperer.capture.scopes.cwhardware.ChipWhispererGlitch.GlitchSettings.width
        :noindex:

    .. autoattribute:: chipwhisperer.capture.scopes.cwhardware.ChipWhispererGlitch.GlitchSettings.offset
        :noindex:

    .. autoattribute:: chipwhisperer.capture.scopes.cwhardware.ChipWhispererGlitch.GlitchSettings.clk_src
        :noindex:

    .. autoattribute:: chipwhisperer.capture.scopes.cwhardware.ChipWhispererGlitch.GlitchSettings.phase_shift_steps

    .. autoattribute:: chipwhisperer.capture.scopes.cwhardware.ChipWhispererGlitch.GlitchSettings.mmcm_locked
        
Pro/Husky Only Featuers
-----------------------

SAD
^^^^^^^^^^^^^^
    Communicates with and drives the Sum of Absolute Differences module on the ChipWhisperer Pro.

    Example for triggering off of some previously collected scope data::

        scope.SAD.reference = trace.wave[1000:1000+128]
        scope.SAD.threshold = 5000
        scope.SAD.start()
        scope.trigger.module = "SAD"
        scope.adc.basic_mode = "rising_edge"

        #can now capture as normal
        trace = cw.capture_trace(scope, target, text, key)

    .. autoclass:: chipwhisperer.capture.scopes.cwhardware.ChipWhispererSAD.ChipWhispererSAD
        :members:

DecodeIO 
^^^^^^^^^^^^^^^^^^^

    Communicates with and drives the Digital Pattern Match module on the ChipWhisperer Pro.

    Basic usage for triggering on :code:`'r\n'`::

        # assuming setup scope
        scope.trigger.triggers = 'tio1'
        scope.trigger.module = 'DECODEIO'
        scope.decode_IO.rx_baud = 38400
        scope.decode_IO.decode_type = 'USART'
        scope.decode_IO.trigger_pattern = ['r', '\n']


    .. autoclass:: chipwhisperer.capture.scopes.cwhardware.ChipWhispererDecodeTrigger.ChipWhispererDecodeTrigger
        :members:

.. _api-scope-husky:

Husky Only Features
-------------------

.. clock
.. ^^^^^

.. Mostly the same as the CWPro, with some additional upgraded settings.

.. Like with the FPGA based clock, the target clock on the Husky 
.. can be set directly

..     scope.clock.clkgen_freq = 7.37E6

.. The ADC clock is set as a positive integer multiple of the target clock

..     scope.clock.adc_mul = 4

.. In order to ensure a clean multiple for the ADC, the PLL
.. settings for the whole chip are changed if :code:`adc_mul` or :code:`target_freq`
.. are changed. This means the target clock will drop out for a short period if
.. either are changed.

.. The PLL can use either an onboard XTAL, or a clock output from the onboard FPGA.
.. The FPGA setting can be set to use an external clock (HS1, usually). Otherwise,
.. the XTAL setting is recommended as it results in much less jitter on the ADC

..     scope.clock.clkgen_src = 'system' # XTAL (default)
..     scope.clock.clkgen_src = 'extclk' # ext clock

.. Like with the other FPGA ChipWhisperers, the phase between the target and ADC clocks can be changed.
.. This is a 6 bit signed value mapped onto the same range as earlier ChipWhisperers ([-255, 255])

..     # +50 phase to adc
..     scope.clock.adc_phase = 50


.. .. attribute: .OpenADC.HuskyClock

..     .. autoattribute:: chipwhisperer.capture.scopes.cwhardware.ChipWhispererHuskyClock.ChipWhispererHuskyClock.clkgen_src
..         :annotation: scope.clock.clkgen_src
    
..     .. autoattribute:: chipwhisperer.capture.scopes.cwhardware.ChipWhispererHuskyClock.ChipWhispererHuskyClock.clkgen_freq
..         :annotation: scope.clock.clkgen_freq

..     .. autoattribute:: chipwhisperer.capture.scopes.cwhardware.ChipWhispererHuskyClock.ChipWhispererHuskyClock.adc_mul
..         :annotation: scope.clock.adc_mul

..     .. autoattribute:: chipwhisperer.capture.scopes.cwhardware.ChipWhispererHuskyClock.ChipWhispererHuskyClock.adc_freq
..         :annotation: scope.clock.adc_freq

..     .. autoattribute:: chipwhisperer.capture.scopes.cwhardware.ChipWhispererHuskyClock.ChipWhispererHuskyClock.adc_phase
..         :annotation: scope.clock.adc_phase

..     .. autoattribute:: chipwhisperer.capture.scopes.cwhardware.ChipWhispererHuskyClock.ChipWhispererHuskyClock.extclk_monitor_enabled
..         :annotation: scope.clock.extclk_monitor_enabled

..     .. autoattribute:: chipwhisperer.capture.scopes.cwhardware.ChipWhispererHuskyClock.ChipWhispererHuskyClock.extclk_error
..         :annotation: scope.clock.extclk_error

..     .. autoattribute:: chipwhisperer.capture.scopes.cwhardware.ChipWhispererHuskyClock.ChipWhispererHuskyClock.extclk_tolerance
..         :annotation: scope.clock.extclk_tolerance

..     .. autoattribute:: chipwhisperer.capture.scopes.cwhardware.ChipWhispererHuskyClock.ChipWhispererHuskyClock.fpga_vco_freq
..         :annotation: scope.clock.fpga_vco_freq

..     .. autoattribute:: chipwhisperer.capture.scopes.cwhardware.ChipWhispererHuskyClock.ChipWhispererHuskyClock.adc_src
..         :annotation: scope.clock.adc_src


.. glitch
.. ^^^^^^

.. Glitching differs more substantially. To start, before you can begin glitching, you must enable
.. the glitch module

..     scope.glitch.enabled = True # off by default to save power + keep FPGA cool

.. The next difference is that the Husky has the option to use an external PLL (the same one used to generate the target/ADC clocks),
.. providing a lower jitter clock source for glitching

..     socpe.glitch.clk_src = "pll" # recommended over "clkgen"

.. The final major difference is width and offset. The Husky has a single width and a single offset parameter
.. which are represented as integers. This means that :code:`offset_fine` and :code:`width_fine` are gone,
.. and the minimum step is now 1 instead of ~0.4 (1/256), though the full range of :code:`[-50, 50]%` of the target clock
.. is maintained.

.. The user also now has some control over what this minimum step corresponds to as a % of the target clock.
.. For a full demo, it's recommended that you read our Jupyter Notebook about glitching on the Husky, but in
.. short, :code:`width` and :code:`offset` has a range of :code:`[-scope.glitch.phase_shift_steps, scope.glitch.phase_shift_steps]`,
.. which is adjustable by setting :code:`scope.clock.pll.update_fpga_vco(X)`, where :code:`X` is between :code:`600e6` and :code:`1200e6`

    .. .. attribute:: .OpenADC.HuskyGlitch
    
    ..     :annotation: scope.glitch

    ..     .. autoattribute:: chipwhisperer.capture.scopes.cwhardware.ChipWhispererGlitch.GlitchSettings.mmcm_locked
    ..         :annotation: scope.glitch.mmcm_locked
    ..         :noindex:

    ..     .. autoattribute:: chipwhisperer.capture.scopes.cwhardware.ChipWhispererGlitch.GlitchSettings.phase_shift_steps
    ..         :annotation: scope.glitch.phase_shift_steps
    ..         :noindex:

    ..     .. autoattribute:: chipwhisperer.capture.scopes.cwhardware.ChipWhispererGlitch.GlitchSettings.clk_src
    ..         :annotation: scope.glitch.clk_src
    ..         :noindex:

    ..     .. autoattribute:: chipwhisperer.capture.scopes.cwhardware.ChipWhispererGlitch.GlitchSettings.width
    ..         :annotation: scope.glitch.width
    ..         :noindex:

    ..     .. autoattribute:: chipwhisperer.capture.scopes.cwhardware.ChipWhispererGlitch.GlitchSettings.offset
    ..         :annotation: scope.glitch.offset
    ..         :noindex:

    ..     .. autoattribute:: chipwhisperer.capture.scopes.cwhardware.ChipWhispererGlitch.GlitchSettings.trigger_src
    ..         :annotation: scope.glitch.trigger_src
    ..         :noindex:

    ..     .. autoattribute:: chipwhisperer.capture.scopes.cwhardware.ChipWhispererGlitch.GlitchSettings.manual_trigger
    ..         :annotation: scope.glitch.manual_trigger
    ..         :noindex:

    ..     .. autoattribute:: chipwhisperer.capture.scopes.cwhardware.ChipWhispererGlitch.GlitchSettings.arm_timing
    ..         :annotation: scope.glitch.arm_timing
    ..         :noindex:

    ..     .. autoattribute:: chipwhisperer.capture.scopes.cwhardware.ChipWhispererGlitch.GlitchSettings.ext_offset
    ..         :annotation: scope.glitch.ext_offset
    ..         :noindex:

    ..     .. autoattribute:: chipwhisperer.capture.scopes.cwhardware.ChipWhispererGlitch.GlitchSettings.repeat
    ..         :annotation: scope.glitch.repeat
    ..         :noindex:

    ..     .. autoattribute:: chipwhisperer.capture.scopes.cwhardware.ChipWhispererGlitch.GlitchSettings.output
    ..         :annotation: scope.glitch.output
    ..         :noindex:

scope.LA
^^^^^^^^

Built in logic analyzer for visualizing internal FPGA signals. Notably can
be used for visualizing glitch parameters, as in the 
`Husky Glitch Notebook <https://github.com/newaetech/chipwhisperer-jupyter/blob/master/demos/husky_glitch.ipynb>`_

.. autoclass:: chipwhisperer.capture.scopes.cwhardware.ChipWhispererHuskyMisc.LASettings
    :members:

scope.ADS4128Settings
^^^^^^^^^^^^^^^^^^^^^

.. autoclass:: chipwhisperer.capture.scopes.cwhardware.ChipWhispererHuskyMisc.ADS4128Settings
    :members:

scope.LEDs
^^^^^^^^^^^^^^^^^

.. autoclass:: chipwhisperer.capture.scopes.cwhardware.ChipWhispererHuskyMisc.LEDSettings
    :members:

scope.errors
^^^^^^^^^^^^

.. autoclass:: chipwhisperer.capture.scopes.cwhardware.ChipWhispererHuskyMisc.HuskyErrors
    :members:

scope.userio
^^^^^^^^^^^^

.. autoclass:: chipwhisperer.capture.scopes.cwhardware.ChipWhispererHuskyMisc.USERIOSettings
    :members:

scope.XADC
^^^^^^^^^^

.. autoclass:: chipwhisperer.capture.scopes.cwhardware.ChipWhispererHuskyMisc.XADCSettings
    :members:

.. _api-scope-cwnano:

ChipWhisperer Nano Scope
========================

Supported scopes:

 * `ChipWhisperer-Nano <https://rtfm.newae.com/Capture/ChipWhisperer-Nano/>`_

.. autoclass:: chipwhisperer.scopes.CWNano
    :members:
    :exclude-members: enable_MPSSE, upgrade_firmware

adc
----
    .. autoclass:: chipwhisperer.capture.scopes.cwnano.ADCSettings
        :members:

io
----

    .. autoclass:: chipwhisperer.capture.scopes.cwnano.GPIOSettings
        :members:

glitch
-------

    .. autoclass:: chipwhisperer.capture.scopes.cwnano.GlitchSettings
        :members:

.. _api-scope-common:

Common Scope Attributes
=======================

The following methods and attributes are common to all scopes, as well as the CW305/CW310:

.. autoclass:: chipwhisperer.capture.api.cwcommon.ChipWhispererCommonInterface

    .. automethod:: enable_MPSSE

    .. automethod:: check_feature

    .. automethod:: feature_list

    .. automethod:: _getNAEUSB

    .. automethod:: get_serial_ports

    .. automethod:: upgrade_firmware

    .. automethod:: reset_sam3u

    .. autoattribute:: latest_fw

    .. autoattribute:: latest_fw_str

    .. autoattribute:: fw_version

    .. autoattribute:: fw_version_str

    .. autoattribute:: sam_build_date

    .. autoattribute:: sn

.. _api-scope-update:

Firmware Update
===============

A simplified method of updating firmware has been added as of ChipWhisperer 5.6.0. This new method
is documented at :ref:`Updating_Firmware`.

For versions 5.5.2 and below, you must follow the directions below:

.. seealso:: You can also use the BOSSA 
    `BOSSA <https://www.shumatech.com/web/products/bossa>`_ utiltiy
    to reflash the SAM3U firmware. If you need to build it from source,
    NewAE keeps a slightly modified version of the source at
    https://github.com/newaetech/BOSSA. This utility does **NOT** support the CWNano.

.. autoclass:: chipwhisperer.capture.scopes.cwhardware.ChipWhispererSAM3Update.SAMFWLoader

    .. automethod:: enter_bootloader

    .. automethod:: program

    .. automethod:: auto_program
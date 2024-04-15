
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
 * `Husky glitching <https://github.com/newaetech/chipwhisperer-jupyter/blob/master/demos/husky/03%20-%20Husky%20Glitching.ipynb>`_

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

scope.trigger (Pro Only)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    .. autoclass:: chipwhisperer.capture.scopes.cwhardware.ChipWhispererExtra.ProTrigger
        :members:

scope.trigger (Husky Only)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    .. autoclass:: chipwhisperer.capture.scopes.cwhardware.ChipWhispererExtra.HuskyTrigger
        :members:

scope.glitch
------------

Module to control glitching. A block diagram of the module is shown below:

        .. thumbnail:: figures/cwlitepro_glitch.png

    .. autoclass:: chipwhisperer.capture.scopes.cwhardware.ChipWhispererGlitch.GlitchSettings
        :members:
        :exclude-members: enabled, mmcm_locked, phase_shift_steps, actual_num_glitches, num_glitches

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

    .. autoattribute:: chipwhisperer.capture.scopes.cwhardware.ChipWhispererGlitch.GlitchSettings.num_glitches
        
Pro Only Features
-----------------------

scope.SAD
^^^^^^^^^^^^^^
    Communicates with and drives the Sum of Absolute Differences module on the ChipWhisperer Pro.

    Example for triggering off of some previously collected scope data::

        scope.SAD.reference = trace.wave[1000:1000+128] # change 128 to 32 on Husky
        scope.SAD.threshold = 5000
        scope.SAD.start()
        scope.trigger.module = "SAD"
        scope.adc.basic_mode = "rising_edge"

        #can now capture as normal
        trace = cw.capture_trace(scope, target, text, key)

    .. autoclass:: chipwhisperer.capture.scopes.cwhardware.ChipWhispererSAD.ChipWhispererSAD
        :members:

scope.decode_IO
^^^^^^^^^^^^^^^

.. autoclass:: chipwhisperer.capture.scopes.cwhardware.ChipWhispererDecodeTrigger.ChipWhispererDecodeTrigger
    :members:


.. _api-scope-husky:

Husky Only Features
-------------------

scope.SAD
^^^^^^^^^^^^^^
    Communicates with and drives the Sum of Absolute Differences module on the ChipWhisperer Husky (API is different
    from CW-Pro SAD API).

.. autoclass:: chipwhisperer.capture.scopes.cwhardware.ChipWhispererSAD.HuskySAD
    :members:

scope.trace
^^^^^^^^^^^^^^^^^

.. autoclass:: chipwhisperer.capture.trace.TraceWhisperer.TraceWhisperer
    :members:


scope.UARTTrigger
^^^^^^^^^^^^^^^^^

UART Pattern Matching Trigger. Must set :code:`scope.trigger.module = 'UART'` to select this trigger.

.. autoclass:: chipwhisperer.capture.trace.TraceWhisperer.UARTTrigger
    :members:

scope.LA
^^^^^^^^

Built in logic analyzer for visualizing internal FPGA signals. Notably can
be used for visualizing glitch parameters, as in the 
`Husky Glitch Notebook <https://github.com/newaetech/chipwhisperer-jupyter/blob/master/demos/husky/03%20-%20Husky%20Glitching.ipynb>`_

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

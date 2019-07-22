.. _api:

###
API
###

This is where the documentation for the public API functions, and classes lives.


.. _api-scope:

*****
Scope
*****

The :func:`chipwhisperer.scope` API function creates and returns a scope object
based on the supplied *type* argument. The are two currently supported classes for the
type argument:

 * :ref:`api-scope-openadc`
 * :ref:`api-scope-cwnano`

The **OpenADC** scope provides support for the *ChipWhisperer Lite* and the
*ChipWhisperer Pro*, and the **CWNano** scope provides support for the
*ChipWhisperer Nano*.

The **scope** object provides the API for configuring the capture side of the
hardware (the scope). To create an instance of a connected scope use::

    import chipwhisperer as cw
    scope = cw.scope()

This will return a connected instance of one of the scope types or raise an
exception. If no scope type is given the function will try to determine the scope
type automatically.

.. autodata:: chipwhisperer.scope
    :annotation: chipwhisperer.scope(type=None, sn=None)


.. _api-scope-openadc:

OpenADC Scope
=============

.. autoclass:: chipwhisperer.scopes.OpenADC

    .. attribute:: gain
        :annotation: scope.gain

        .. autodata:: chipwhisperer.capture.scopes._OpenADCInterface.GainSettings.db
            :annotation: scope.gain.db

        .. autodata:: chipwhisperer.capture.scopes._OpenADCInterface.GainSettings.mode
            :annotation: scope.gain.mode

        .. autodata:: chipwhisperer.capture.scopes._OpenADCInterface.GainSettings.gain
            :annotation: scope.gain.gain


    .. attribute:: adc
        :annotation: scope.adc

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

    .. automethod:: chipwhisperer.capture.scopes.OpenADC.OpenADC.default_setup

    .. automethod:: chipwhisperer.capture.scopes.OpenADC.OpenADC.arm

    .. automethod:: chipwhisperer.capture.scopes.OpenADC.OpenADC.capture

    .. automethod:: chipwhisperer.capture.scopes.OpenADC.OpenADC.get_last_trace

    .. method:: dis()

        Disconnects the current scope object.

        :return: A boolean of whether the disconnect was successful.

    .. method:: con(sn=None)

        Connects to attached chipwhisperer hardware (Lite or Pro)

        :param sn: The serial number of the attached device. Does not need to be
            specified unless there are multiple device attached.

        :return: A boolean of whether the connection was successful.

        :raises:
            :OSError: Raised when there is issues connecting to the hardware, such as
                user not having the correct device permissions to access the hardware.


.. _api-scope-cwnano:

ChipWhisperer Nano Scope
========================

.. autoclass:: chipwhisperer.scopes.CWNano

    .. attribute:: adc
        :annotation: scope.adc

        .. autodata:: chipwhisperer.capture.scopes.cwnano.ADCSettings.samples
            :annotation: scope.adc.samples

        .. autodata:: chipwhisperer.capture.scopes.cwnano.ADCSettings.clk_src
            :annotation: scope.adc.clk_src

        .. autodata:: chipwhisperer.capture.scopes.cwnano.ADCSettings.clk_freq
            :annotation: scope.adc.clk_freq

    .. attribute:: io
        :annotation: scope.io

        .. autodata:: chipwhisperer.capture.scopes.cwnano.GPIOSettings.tio1
            :annotation: scope.io.tio1

        .. autodata:: chipwhisperer.capture.scopes.cwnano.GPIOSettings.tio2
            :annotation: scope.io.tio2

        .. autodata:: chipwhisperer.capture.scopes.cwnano.GPIOSettings.tio3
            :annotation: scope.io.tio3

        .. autodata:: chipwhisperer.capture.scopes.cwnano.GPIOSettings.tio4
            :annotation: scope.io.tio4

        .. autodata:: chipwhisperer.capture.scopes.cwnano.GPIOSettings.pdid
            :annotation: scope.io.pdid

        .. autodata:: chipwhisperer.capture.scopes.cwnano.GPIOSettings.pdic
            :annotation: scope.io.pdic

        .. autodata:: chipwhisperer.capture.scopes.cwnano.GPIOSettings.nrst
            :annotation: scope.io.nrst

        .. autodata:: chipwhisperer.capture.scopes.cwnano.GPIOSettings.clkout
            :annotation: scope.io.clkout

    .. attribute:: glitch
        :annotation: scope.glitch

        .. autodata:: chipwhisperer.capture.scopes.cwnano.GlitchSettings.ext_offset
            :annotation: scope.glitch.ext_offset

        .. autodata:: chipwhisperer.capture.scopes.cwnano.GlitchSettings.repeat
            :annotation: scope.glitch.repeat

    .. automethod:: chipwhisperer.capture.scopes.cwnano.CWNano.default_setup

    .. automethod:: chipwhisperer.capture.scopes.cwnano.CWNano.arm

    .. automethod:: chipwhisperer.capture.scopes.cwnano.CWNano.capture

    .. automethod:: chipwhisperer.capture.scopes.cwnano.CWNano.get_last_trace

    .. method:: dis()

        Disconnects the current scope object.

        :return: A boolean of whether the disconnect was successful.

    .. method:: con(sn=None)

        Connects to attached chipwhisperer hardware (ChipWhisperer Nano)

        :param sn: The serial number of the attached device. Does not need to be
            specified unless there are multiple device attached.

        :return: A boolean of whether the connection was successful.

        :raises:
            :OSError: Raised when there is issues connecting to the hardware, such as
                user not having the correct device permissions to access the hardware.


.. _api-scope-update:

Firmware Update
===============

On some occasions it is necessary to reprogram the scope of the ChipWhisperer Nano, Lite
or Pro.

.. warning::
    This is not necessary most of the time. It should be only done if you know exactly
    what you are doing. This process will erase the firmware on the scope and make it
    unusable until reprogrammed.

.. autoclass:: chipwhisperer.capture.scopes.cwhardware.ChipWhispererSAM3Update.SAMFWLoader

    .. automethod:: enter_bootloader

    .. automethod:: program


.. _api-target:

******
Target
******

The target object provides the interface for configuring the target device
under test (DUT). Supported targets:

 * :ref:`api-target-simpleserial` (default)
 * :ref:`api-target-cw305`

The Simple Serial target type provides the target interface for all targets that use
a simple serial connection. A object can be created using::

    import chipwhisperer as cw
    scope = cw.scope()
    target = cw.target(scope)

You now have access to the target configuration using the *target* variable.

.. autodata:: chipwhisperer.target
    :annotation: chipwhisperer.target(scope, target_type=targets.SimpleSerial, **kwargs)


.. _api-target-simpleserial:

Simple Serial Target
====================

.. autoclass:: chipwhisperer.targets.SimpleSerial

    .. automethod:: write

    .. automethod:: read

    .. automethod:: simpleserial_wait_ack

    .. automethod:: simpleserial_write

    .. automethod:: simpleserial_read

    .. automethod:: set_key

    .. automethod:: in_waiting

    .. automethod:: flush

    .. automethod:: close

    .. automethod:: con

    .. autoattribute:: baud


.. _api-target-cw305:

CW305 FPGA Target
=================

.. autoclass:: chipwhisperer.targets.CW305

    .. automethod:: simpleserial_read

    .. automethod:: simpleserial_write

    .. autoproperty:: clksleeptime

    .. autoproperty:: clkusbautooff

    .. attribute:: pll
        :annotation: target.pll

        .. autodata:: chipwhisperer.hardware.naeusb.pll_cdce906.PLLCDCE906.pll_outenable_set
            :annotation: target.pll.pll_outenable_set

        .. autodata:: chipwhisperer.hardware.naeusb.pll_cdce906.PLLCDCE906.pll_outenable_get
            :annotation: target.pll.pll_outenable_get

        .. autodata:: chipwhisperer.hardware.naeusb.pll_cdce906.PLLCDCE906.pll_outfreq_set
            :annotation: target.pll.pll_outfreq_set

        .. autodata:: chipwhisperer.hardware.naeusb.pll_cdce906.PLLCDCE906.pll_outfreq_get
            :annotation: target.pll.pll_outfreq_get

.. _api-target-program:

Program
=======

When testing out different firmware on the target it is useful to be able
to reprogram the target. This can be done using the
:func:`chipwhisperer.program_target` function. There are multiple programmer
types available:

  * :class:`programmers.STM32FProgrammer <chipwhisperer.capture.api.programmers.STM32FProgrammer>`
  * :class:`programmers.XMEGAProgrammer <chipwhisperer.capture.api.programmers.XMEGAProgrammer>`
  * :class:`programmers.AVRProgrammer <chipwhisperer.capture.api.programmers.AVRProgrammer>`

.. autofunction:: chipwhisperer.program_target


.. _api-project:

*******
Project
*******

The project is a way of storing a traces, and other project data together
using a bag-of-files. These projects can be exported and imported to a
new session.

A project can be accessed a few different ways. It can either be loaded from storage,
created new, or imported. Importing currently only supports zip files.

.. autofunction:: chipwhisperer.open_project

.. autofunction:: chipwhisperer.create_project

.. autofunction:: chipwhisperer.import_project

The :func:`open_project <chipwhisperer.open_project>` and the
:func:`create_project <chipwhisperer.create_project>` return a
:class:`Project <chipwhisperer.common.api.ProjectFormat.Project>` instance.

.. automodule:: chipwhisperer.common.api.ProjectFormat

    .. autoclass:: Project

        .. autoattribute:: location

        .. autoattribute:: traces

        .. autoattribute:: waves

        .. autoattribute:: textins

        .. autoattribute:: textouts

        .. autoattribute:: keys

        .. automethod:: get_filename

        .. automethod:: trace_manager

        .. automethod:: export

        .. automethod:: save

        .. automethod:: close

    .. autoclass:: chipwhisperer.common.api.ProjectFormat.Traces

        .. automethod:: append

        .. automethod:: extend


.. _api-capture:

*******
Capture
*******

Before capturing a trace make sure:

 #. The :ref:`scope <api-scope>` has been connected.
 #. The :ref:`scope <api-scope>` is configured.
 #. The :ref:`target <api-target>` has been connected.
 #. The :ref:`target <api-target>` is configured.
 #. The :ref:`target <api-target>` has been :ref:`programmed <api-target-program>`
    with the correct firmware.

Once you completed these steps you can use the
:func:`capture_trace <chipwhisperer.capture_trace>` function to complete a
capture of one trace. There are some helper classes that generate types of
:ref:`key text patterns <api-capture-ktp>` for input to the
:func:`capture_trace <chipwhisperer.capture_trace>` function. The function
will return a :ref:`Trace <api-capture-trace>`.


.. autofunction:: chipwhisperer.capture_trace


.. _api-capture-trace:

Trace
=====

This class is used throughout the ChipWhisperer software to package together
the relevant data for each captured power trace. After a capture is complete the
:func:`capture_trace <chipwhisperer.capture_trace>` function will return
a :class:`Trace <chipwhisperer.common.traces.Trace>`. This trace is *namedtuple* of four
pieces of data (wave, textin, textout, key), where *wave* is the actually
numpy array of the power trace captured during the target's operation. The
individual pieces of data can be accessed as a one would with a tuple or
by using the provided attributes.

.. autoclass:: chipwhisperer.common.traces.Trace

    .. autoattribute:: wave

    .. autoattribute:: textin

    .. autoattribute:: textout

    .. autoattribute:: key

    .. versionadded:: 5.1
        Added Trace class.


.. _api-capture-ktp:

Key Text Patterns
=================

There are a few different types of classes for generating key text patterns
for your capture:

  * :ref:`api-capture-ktp-basic`
  * :ref:`api-capture-ktp-tvla_ttest`


.. _api-capture-ktp-basic:

Basic
-----

.. autoclass:: chipwhisperer.capture.acq_patterns.basic.AcqKeyTextPattern_Basic

    .. autoattribute:: fixed_key

    .. autoattribute:: fixed_text

    .. automethod:: next

    .. automethod:: next_key

    .. automethod:: next_text


.. _api-capture-ktp-tvla_ttest:

TVLA TTest
----------

.. autoclass:: chipwhisperer.capture.acq_patterns.tvlattest.AcqKeyTextPattern_TVLATTest

    .. automethod:: init

    .. automethod:: next


.. _api-analyzer:

********
Analyzer
********

You may want to use your captured traces to perform an attack based on a type
of side-channel analysis, in comes the analyzer. You can import the analyzer API
using::

    import chipwhisperer.analyzer as cwa

One of the ways to analyze your traces is using correlation power analysis (CPA).
The :func:`cpa <chipwhisperer.analyzer.cpa>` function returns a
:class:`CPA <chipwhisperer.analyzer.attacks.cpa_new.CPA>` object that can be used to
run the attack.

.. automodule:: chipwhisperer.analyzer

    .. autodata:: chipwhisperer.analyzer.cpa
        :annotation: cpa(proj, leak_model, algorithm=cpa_algorithms.Progressive)

    .. data:: leakage_models

        Contains all available leakage models that can be passed to the
        :func:`cpa <chipwhisperer.analyzer.cpa>` function as the **leak_model** parameter.

        Instance of the
        :class:`EightBitAES128LeakageModels <chipwhisperer.analyzer.attacks.models.EightBitAES128LeakageModels>`
        class.

        .. versionadded:: 5.1
            Add leakage_models to anlayzer API.


.. _api-analyzer-preprocessing:

Preprocessing
==============

Before performing an attack you may want to apply some pre-processing to the
captured traces. There is currently one pre-processing class that has been
moved to the latest ChipWhisperer API.

.. automodule:: chipwhisperer.analyzer.preprocessing.resync_sad

    .. autoclass:: ResyncSAD

        .. autoattribute:: max_shift

        .. autoattribute:: ref_trace

        .. autoattribute:: target_window

        .. automethod:: preprocess


.. _api-analyzer-leakage_models:

Leakage Models
==============

The analyzer includes various leakage models that can be passed to
the :func:`cpa <chipwhisperer.analyzer.cpa>` as the **leak_model**
argument. The
:data:`leakage_models <chipwhisperer.analyzer.leakage_models>`
variable provides a convenient way to list and access all the available
leakage models::

    import chipwhisperer.analyzer as cwa
    print(cwa.leakage_models)

Any attribute can be chosen from this list and accessed with the dot
syntax.

.. automodule:: chipwhisperer.analyzer.attacks.models

    .. autoclass:: EightBitAES128LeakageModels

        .. autoattribute:: after_key_mix

        .. autoattribute:: inverse_sbox_output

        .. autoattribute:: last_round_state

        .. autoattribute:: last_round_state_diff

        .. autoattribute:: last_round_state_diff_alternate

        .. autoattribute:: mix_columns_output

        .. autoattribute:: plaintext_key_xor

        .. autoattribute:: round_1_2_state_diff_key_mix

        .. autoattribute:: round_1_2_state_diff_sbox

        .. autoattribute:: round_1_2_state_diff_text

        .. autoattribute:: sbox_in_out_diff

        .. autoattribute:: sbox_input_successive

        .. autoattribute:: sbox_output

        .. autoattribute:: sbox_output_successive

        .. autoattribute:: shift_columns_output

        .. automethod:: new_model

.. _api-analyzer-cpa_attack:

CPA Attack
==========

The interface class to carry out a correlation power analysis attack on the
previously captured traces. The easiest way to create a default CPA instance
that is ready to perform an attack is to use the
:func:`cpa <chipwhisperer.analyzer.cpa>` function.

.. automodule:: chipwhisperer.analyzer.attacks.cpa_new

    .. autoclass:: CPA

        .. automethod:: change_project

        .. automethod:: run


Results
-------

The :meth:`cpa.run <chipwhisperer.analyzer.attacks.cpa_new.CPA.run>` function will
return a results object. There are a few methods for interacting with the results
object.

.. autoclass:: chipwhisperer.analyzer.attacks._stats.Results
    :members: best_guesses, simple_PGE, set_known_key, find_maximums


.. _api-analyzer-utilities:

Utilities
=========

This section contains relatively independent functions that are useful for
working with AES, and traces.

.. _api-analyzer-utilities-aes_functions:

AES Functions
-------------

You may want to perform certain AES operations seperately. For this there is the AES
helper functions. They are accessible using::

    from chipwhisperer.analyzer import aes_funcs

Available functions:
  * :func:`aes_funcs.sbox <chipwhisperer.analyzer.attacks.models.aes.funcs.sbox>`
  * :func:`aes_funcs.inv_sbox <chipwhisperer.analyzer.attacks.models.aes.funcs.inv_sbox>`
  * :func:`aes_funcs.subbytes <chipwhisperer.analyzer.attacks.models.aes.funcs.subbytes>`
  * :func:`aes_funcs.inv_subbytes <chipwhisperer.analyzer.attacks.models.aes.funcs.inv_subbytes>`
  * :func:`aes_funcs.shiftrows <chipwhisperer.analyzer.attacks.models.aes.funcs.shiftrows>`
  * :func:`aes_funcs.inv_shiftrows <chipwhisperer.analyzer.attacks.models.aes.funcs.inv_shiftrows>`
  * :func:`aes_funcs.mixcolumns <chipwhisperer.analyzer.attacks.models.aes.funcs.mixcolumns>`
  * :func:`aes_funcs.inv_mixcolumns <chipwhisperer.analyzer.attacks.models.aes.funcs.inv_mixcolumns>`
  * :func:`aes_funcs.key_schedule_rounds<chipwhisperer.analyzer.attacks.models.aes.key_schedule.key_schedule_rounds>`

.. autofunction:: chipwhisperer.analyzer.attacks.models.aes.funcs.sbox

.. autofunction:: chipwhisperer.analyzer.attacks.models.aes.funcs.inv_sbox

.. autofunction:: chipwhisperer.analyzer.attacks.models.aes.funcs.subbytes

.. autofunction:: chipwhisperer.analyzer.attacks.models.aes.funcs.inv_subbytes

.. autofunction:: chipwhisperer.analyzer.attacks.models.aes.funcs.shiftrows

.. autofunction:: chipwhisperer.analyzer.attacks.models.aes.funcs.inv_shiftrows

.. autofunction:: chipwhisperer.analyzer.attacks.models.aes.funcs.mixcolumns

.. autofunction:: chipwhisperer.analyzer.attacks.models.aes.funcs.inv_mixcolumns

.. autofunction:: chipwhisperer.analyzer.attacks.models.aes.key_schedule.key_schedule_rounds


.. _api-analyzer-utilities-snr:

Signal to Noise Ratio
---------------------

Calculate the signal-to-noise ratio of your captured traces::

    import chipwhisperer.analyzer as cwa
    import chipwhisperer as cw

    project = cw.open_project('my_project')
    cwa.calculate_snr(project, cwa.leakage_models.sbox_output)

.. autofunction:: chipwhisperer.analyzer.calculate_snr




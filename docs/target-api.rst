
.. _api-target:

**********
Target API
**********

The target object provides the interface for configuring the target device
under test (DUT). There are currently two UART communication classes:

 * :ref:`api-target-simpleserial` (default)
 * :ref:`api-target-simpleserial2`

and two FPGA based targets:

 * :ref:`api-target-cw305`
 * :ref:`api-target-cw310`

The easiest way to create a target object is to use the :code:`chipwhisperer.target`
function::

    import chipwhisperer as cw
    scope = cw.scope()
    target = cw.target(scope, cw.targets.SimpleSerial)

.. autodata:: chipwhisperer.target
    :annotation: chipwhisperer.target(scope, target_type=targets.SimpleSerial, **kwargs)

Serial Targets
==============

.. _api-target-simpleserial:

Simple Serial Target
--------------------

38400bps ASCII encoded serial. See SimpleSerial for protocol documentation.::

    import chipwhisperer as cw
    scope = cw.scope()
    target = cw.target(scope)

Supported Targets:

 * `Targets integrated into capture boards <https://rtfm.newae.com/Targets/#integrated-targets>`_
 * `CW308 UFO targets <https://rtfm.newae.com/Targets/#cw308-targets>`_

.. autoclass:: chipwhisperer.targets.SimpleSerial
    :members:

.. _api-target-simpleserial2:

Simple Serial V2 Target
-----------------------

230400bps COBS serial. See SimpleSerial for protocol documentation.::

    import chipwhisperer as cw
    scope = cw.scope()
    target = cw.target(scope, cw.targets.SimpleSerial2)

Supported Targets:

 * `Targets integrated into capture boards <https://rtfm.newae.com/Targets/#integrated-targets>`_
 * `CW308 UFO targets <https://rtfm.newae.com/Targets/#cw308-targets>`_

.. autoclass:: chipwhisperer.targets.SimpleSerial2
    :members:

Simple Serial V2 CDC Target
---------------------------

Variant of :ref:`api-target-simpleserial2` that uses the ChipWhisperer's CDC serial port instead of
its custom USB interface.::

    import chipwhisperer as cw
    scope = cw.scope()
    target = cw.target(scope, cw.targets.SimpleSerial2_CDC) # autodetects COM (/dev/ttyACMx) port

.. autoclass:: chipwhisperer.targets.SimpleSerial2_CDC
    :members:


FPGA Targets
============

.. _api-target-cw305:

CW305 FPGA Target
-----------------

Used for controlling and communicating with the 
`CW305 Artix FPGA Target <https://rtfm.newae.com/Targets/CW305%20Artix%20FPGA/>`_.::

    import chipwhisperer as cw
    target = cw.target(None, cw.targets.CW305) # programs default bitstream, see _con() for full arg list

Examples:

 * `Attacking AES <https://github.com/newaetech/chipwhisperer-jupyter/blob/master/demos/PA_HW_CW305_1-Attacking_AES_on_an_FPGA.ipynb>`_
 * `Attacking ECC <https://github.com/newaetech/chipwhisperer-jupyter/tree/master/demos/CW305_ECC>`_


.. autoclass:: chipwhisperer.targets.CW305
    :members:

target.pll
^^^^^^^^^^

.. autoclass:: chipwhisperer.hardware.naeusb.pll_cdce906.PLLCDCE906
    :members:


.. _api-target-cw310:

CW310
-----

Used for controlling and communicating with the 
`CW310 Bergen Board <https://rtfm.newae.com/Targets/CW305%20Artix%20FPGA/>`_.::

    import chipwhisperer as cw
    target = cw.target(None, cw.targets.CW310)

Examples:

 * Coming soon!

.. autoclass:: chipwhisperer.capture.targets.CW310.CW310

    .. automethod:: usb_set_voltage

    .. automethod:: usb_set_current

    .. automethod:: usb_negotiate_pdo

    .. automethod:: gpio_mode

    .. automethod:: _con

    .. automethod:: fpga_write

    .. automethod:: fpga_read

    .. autoattribute:: chipwhisperer.capture.targets.CW310.CW310.cdc_settings
        :annotation: target.cdc_settings

Common Target Methods
---------------------

.. note:: Both the CW310 and CW305 inherit common methods from :ref:`api-scope-common`

FPGA Advanced Control
=====================

Advanced IO control for FPGA targets

.. _cw305-spi-program:

CW305 SPI Program
-----------------

.. autoclass:: chipwhisperer.capture.targets.CW305.FPGASPI
    :members:

.. _cw305-fpga-io:

CW305 SAM3U IO Control
----------------------

.. autoclass:: chipwhisperer.capture.targets.CW305.FPGAIO
    :members:

.. _api-target-program:

Serial Target Programming
=========================

ChipWhisperer includes built in bootloaders for STM32F, XMEGA, and AVR
target boards.  These bootloaders can be accessed via the 
:func:`chipwhisperer.program_target` function. There are multiple programmer
types available:

  * :class:`programmers.STM32FProgrammer <chipwhisperer.capture.api.programmers.STM32FProgrammer>`
  * :class:`programmers.XMEGAProgrammer <chipwhisperer.capture.api.programmers.XMEGAProgrammer>`
  * :class:`programmers.AVRProgrammer <chipwhisperer.capture.api.programmers.AVRProgrammer>`

Example::

    import chipwhisperer as cw
    # ...scope setup, firmware build, etc

    cw.program_target(scope, cw.programmers.STM32FProgrammer, "/path/to/firmware.hex")


.. autofunction:: chipwhisperer.program_target
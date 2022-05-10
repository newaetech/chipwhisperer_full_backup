*************
ChipWhisperer
*************

Welcome to the new home of the documentation for the ChipWhisperer software,
following the release of ChipWhisperer 5.1.0.

If you're new to ChipWhisperer, or haven't been following the project recently,
you may want to check out :ref:`overview <getting_started>` to learn a little
about the project and its recent changes.

After :ref:`installing <install>` ChipWhisperer and setting up your
`hardware`_ run:

.. code:: python

    >>> import chipwhisperer as cw
    >>> scope = cw.scope()
    >>> scope
    cwlite Device
    gain =
        mode = low
        gain = 0
        db   = -6.5
    adc =
        state      = False
        basic_mode = low
        timeout    = 2
        offset     = 0
    ...


You now have access to an object-oriented interface to configure the attached
hardware. To see what is possible with this interface check out the
:ref:`scope section <api-scope>` of the API documentation.

.. _hardware: https://rtfm.newae.com

.. _install:

Installation
============

.. toctree::
    :maxdepth: 1

    windows-install
    linux-install
    mac-install
    virtual-box-inst

Documentation
=============

.. toctree::
    :maxdepth: 2

    getting-started
    starting

API
===

Documentation specific to api functions, classes, and modules live here.

.. toctree::
    :maxdepth: 3

    api


Additional Information
======================

Here is more information about changes, and contribution.

.. toctree::
    :maxdepth: 2
    :glob:

    contributing
    Change Log <changes.rst>
    SimpleSerial <simpleserial.rst>
    Updating Firmware <firmware.rst>
    Logging <logging.rst>
    Drivers <drivers.rst>
    ChipWhisperer as a Debugger <debugging.rst>

Tutorials
---------

Completed tutorial files, which were previously available on this page, are now stored on our
`test results Github page <https://github.com/newaetech/ChipWhisperer-Test-Results>`__ and
are available in reStructured text (which renders on Github) and html.

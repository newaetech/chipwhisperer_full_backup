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

Overview
========
.. toctree::
    :maxdepth: 1

    Overview <getting-started.rst>
    Running ChipWhisperer <starting.rst>


.. _install:

Installation
============

.. toctree::
    :maxdepth: 2

    windows-install
    drivers
    linux-install
    mac-install
    virtual-box-inst

.. _api:

API
===

Documentation specific to api functions, classes, and modules live here.

.. toctree::
    :maxdepth: 2

    scope-api
    firmware
    target-api
    capture-api
    analyzer-api


Additional Features and Info
============================

ChipWhisperer has some additional features that you may find useful, such as target debugging
and control over logging in Python.

.. toctree::
    :maxdepth: 2

    ChipWhisperer as a Debugger <debugging.rst>
    SimpleSerial <simpleserial.rst>
    Logging <logging.rst>
    Custom Firmware <custom-firmware.rst>

Other
============================

Additional information such as the changelog and contributing

.. toctree::
    :maxdepth: 1

    contributing
    Changelog <changes.rst>

Tutorials
---------

Tutorials are available as Jupyter Notebooks on 
our `chipwhisperer-jupyter <https://github.com/newaetech/chipwhisperer-jupyter>`__ Github
repository. 

Completed tutorial files, which were previously available on this page, are now stored on our
`test results Github page <https://github.com/newaetech/ChipWhisperer-Test-Results>`__ and
are available in reStructured text (which renders on Github) and html.

Deprecated Pages
----------------

This website has been restructed for CW 5.6.1. The following pages are deprecated
and will be removed in the future:

.. toctree::

    api <api.rst>
    install <installing.rst>
    prerequisites <prerequisites.rst>
    tutorials <tutorials.rst>
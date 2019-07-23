*************
ChipWhisperer
*************

.. note:: This documentation is currently under construction for version
    5.1.0 of the ChipWhisperer software. And currently is only partially complete.
    It is getting close :)

Welcome to the home of the installation procedure, Python API documentation,
and the side-channel analysis tutorials for the ChipWhisperer tool-chain.

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

To see longer, more in-depth examples of what the tool-chain can do, take a look
at the `Using the ChipWhisperer Analyzer for a Correlation Power Analysis Attack <tutorial-pa_cpa_1-openadc-cwlitearm>`_
tutorial. Check out all the other :ref:`tutorials <tutorials>` to see what is possible. The
content is generated from Jupyter notebooks included with the project. It
includes interactive plots.

If you are new to the latest major changes and other websites and resources
related to side-channel analysis and the ChipWhisperer tool-chain, here is an
:ref:`overview <getting_started>` page for you.

.. _hardware: https://wiki.newae.com/Main_Page#Hardware_Documentation


Documentation
=============

.. toctree::
    :maxdepth: 2

    getting-started
    prerequisites
    installing
    starting


Jupyter Notebook Tutorials
==========================

Jupyter notebook tutorials with output included so you can follow along,
or just check out what the ChipWhisperer tool-chain is capable of.


.. toctree::
    :maxdepth: 4

    tutorials


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
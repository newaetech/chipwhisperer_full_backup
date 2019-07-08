*************
ChipWhisperer
*************

After installing ChipWhisperer and setting up your hardware run:

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


You now have access to an object-oriented interface to configure the attached hardware.


Documentation
=============

.. toctree::
    :maxdepth: 2

    prerequisites
    installing


API
===

Documentation specific to api functions, classes, and modules live here.

.. toctree::
    :maxdepth: 2

    api


Additional Information
======================

Here is more information about the project, changes, decisions and
contribution.

.. toctree::
    :maxdepth: 2
    :glob:

    contributing
    Change Log <changes.rst>


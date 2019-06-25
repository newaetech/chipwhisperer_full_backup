****************************************
Welcome to ChipWhisperer's Documentation 
****************************************

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

Example Attack with captured traces.

See the rest of our `api <sec-api>`_.


Installing
==========
You get your choice of installing ChipWhisperer and its prerequisites: the easy way
or the hard way.

Basic
    * :ref:`sec-virtual-machine` (Recommended)
    * :ref:`sec-install-windows-auto`

Advanced
    Prerequisites Installation
        * :ref:`sec-install-windows-manual`
        * :ref:`sec-install-linux`
        * :ref:`sec-install-mac`

    ChipWhisperer Installation
        * :ref:`sec-install-repo-releases`
        * :ref:`sec-install-repo-pypi`
        * :ref:`sec-install-repo-git`
    
.. toctree::
    :maxdepth: 2
    :hidden:

    installing/virtual-machine
    installing/windows-installer
    installing/linux-manual
    installing/windows-manual
    installing/mac-manual
    installing/repository-install


.. _sec-api:

API
===

Documentation specific to api functions, classes, and modules live here.

.. toctree::
    :maxdepth: 2
    :glob:

    api/scope
    api/target


Contributing
============

We welcome contributions. Here you can find all the information you need.

.. toctree::
    :maxdepth: 2
    :glob:

    contributing/*


Indices and tables
==================

* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`

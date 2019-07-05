*****************************
ChipWhisperer's Documentation
*****************************

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

Before being able to complete this example, :ref:`install <sec-installing>` chipwhisperer.
See the rest of our :ref:`api <sec-api>`.

.. _sec-installing:

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

 * :ref:`Scope <sec-scope>`
    Creation:
         - :func:`scope <chipwhisperer.scope>`
    Types:
         - :ref:`OpenADC <sec-scope-openadc>`
         - :ref:`CWNano <sec-scope-cwnano>`

 * :ref:`Target <sec-target>`
    Creation:
         - :func:`target <chipwhisperer.target>`
    Types:
         - :ref:`SimpleSerial <sec-target-simpleserial>`

 * :ref:`Project <sec-project>`
    Creation:
         - :func:`create_project <chipwhisperer.create_project>`
         - :func:`open_project <chipwhisperer.open_project>`
         - :func:`import_project <chipwhisperer.importt_project>`
    Types:
         - :class:`Project <chipwhisperer.common.api.ProjectFormat.Project>`

.. toctree::
    :maxdepth: 2
    :hidden:
    :glob:

    api/*


Contributing
============

We welcome contributions. Here you can find all the information you need.

.. toctree::
    :maxdepth: 2
    :glob:

    contributing/*

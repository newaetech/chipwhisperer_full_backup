*******
Project
*******

The project is a way of storing a traces, and other project data together
using a bag-of-files. These projects can be exported and imported to a
new session. There is one available project format that is for the general
use case:

 * :ref:`sec-project-format`

.. toctree::
    :maxdepth: 2
    :hidden:

    project-format

Initialization
--------------

A project can be accessed one of two ways. It can either be loaded from a disk or
created new. There are two functions that provide this functionality.

.. autofunction:: chipwhisperer.open_project

.. autofunction:: chipwhisperer.create_project


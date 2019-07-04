*******
Project
*******

The project is a way of storing a traces, and other project data together
using a bag-of-files. These projects can be exported and imported to a
new session.


Initialization
--------------

A project can be accessed one of two ways. It can either be loaded from storage,
created new, or imported.

.. autofunction:: chipwhisperer.open_project

.. autofunction:: chipwhisperer.create_project

.. autofunction:: chipwhisperer.import_project


Usage
-----

The :func:`open_project <chipwhisperer.open_project>` and the
:func:`create_project <chipwhisperer.create_project>` return a
:class:`Project <chipwhisperer.common.api.ProjectFormat.Project>` instance.

.. autoclass:: chipwhisperer.common.api.ProjectFormat.Project

    .. automethod:: chipwhisperer.common.api.ProjectFormat.Project.get_trace_format

    .. automethod:: chipwhisperer.common.api.ProjectFormat.Project.get_filename

    .. automethod:: chipwhisperer.common.api.ProjectFormat.Project.trace_manager

    .. automethod:: chipwhisperer.common.api.ProjectFormat.Project.export

    .. autoattribute:: chipwhisperer.common.api.ProjectFormat.Project.location

    .. autoattribute:: chipwhisperer.common.api.ProjectFormat.Project.traces

.. autoclass:: chipwhisperer.common.api.ProjectFormat.Traces

    .. automethod:: chipwhisperer.common.api.ProjectFormat.Traces.




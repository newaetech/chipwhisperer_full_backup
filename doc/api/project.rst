.. _sec-project:

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

.. autoclass:: chipwhisperer.common.api.ProjectFormat.Project

    .. autoattribute:: chipwhisperer.common.api.ProjectFormat.Project.location

    .. autoattribute:: chipwhisperer.common.api.ProjectFormat.Project.traces

    .. autoattribute:: chipwhisperer.common.api.ProjectFormat.Project.waves

    .. autoattribute:: chipwhisperer.common.api.ProjectFormat.Project.textins

    .. autoattribute:: chipwhisperer.common.api.ProjectFormat.Project.textouts

    .. autoattribute:: chipwhisperer.common.api.ProjectFormat.Project.keys

    .. automethod:: chipwhisperer.common.api.ProjectFormat.Project.get_filename

    .. automethod:: chipwhisperer.common.api.ProjectFormat.Project.trace_manager

    .. automethod:: chipwhisperer.common.api.ProjectFormat.Project.export

    .. automethod:: chipwhisperer.common.api.ProjectFormat.Project.save

.. autoclass:: chipwhisperer.common.api.ProjectFormat.Traces

    .. automethod:: chipwhisperer.common.api.ProjectFormat.Traces.append




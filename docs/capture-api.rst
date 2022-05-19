
.. _capture-api:

****************
Capture Helpers
****************

This page documents the included helper functions and classes
for capturing, storing, and plotting traces.

.. _api-project:

Project
=======

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
        :members: location, traces, waves, textins, textouts, keys, get_filename, trace_manager, export, save, close

    .. autoclass:: chipwhisperer.common.api.ProjectFormat.Traces
        :members: append, extend

.. _api-capture-helpers:

Capture
=======

Before capturing a trace make sure:

 #. The :ref:`scope <api-scope>` has been connected.
 #. The :ref:`scope <api-scope>` is configured.
 #. The :ref:`target <api-target>` has been connected.
 #. The :ref:`target <api-target>` is configured.
 #. The :ref:`target <api-target>` has been :ref:`programmed <api-target-program>`
    with the correct firmware.

Once you completed these steps you can use the
:func:`capture_trace <chipwhisperer.capture_trace>` function to complete a
capture of one trace. There are some helper classes that generate types of
:ref:`key text patterns <api-capture-ktp>` for input to the
:func:`capture_trace <chipwhisperer.capture_trace>` function. The function
will return a :ref:`Trace <api-capture-trace>`.


.. autofunction:: chipwhisperer.capture_trace


.. _api-capture-trace:

Trace
=====

This class is used throughout the ChipWhisperer software to package together
the relevant data for each captured power trace. After a capture is complete the
:func:`capture_trace <chipwhisperer.capture_trace>` function will return
a :class:`Trace <chipwhisperer.common.traces.Trace>`. This trace is *namedtuple* of four
pieces of data (wave, textin, textout, key), where *wave* is the actually
numpy array of the power trace captured during the target's operation. The
individual pieces of data can be accessed as a one would with a tuple or
by using the provided attributes. Example::

    import chipwhisperer as cw
    trace = cw.Trace(wave, textin, textout, key)

This trace groups together the power trace (wave), and the process information
that resulted in that trace such as textin, textout, and key.

.. autoclass:: chipwhisperer.common.traces.Trace
    :members:

.. versionadded:: 5.1
    Added Trace class.


.. _api-capture-ktp:

Key Text Patterns
=================

There are a few different types of classes for generating key text patterns
for your capture:

  * :class:`chipwhisperer.capture.acq_patterns.basic.AcqKeyTextPattern_Basic`
  * :class:`chipwhisperer.capture.acq_patterns.basic.AcqKeyTextPattern_TVLATTest`

.. _api-capture-ktp-basic:

Basic
-----

.. autoclass:: chipwhisperer.capture.acq_patterns.basic.AcqKeyTextPattern_Basic
    :members:

.. _api-capture-ktp-tvla_ttest:

TVLA TTest
----------

.. autoclass:: chipwhisperer.capture.acq_patterns.tvlattest.AcqKeyTextPattern_TVLATTest
    :members:
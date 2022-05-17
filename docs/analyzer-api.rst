
.. _analyzer-api:

********
Analyzer
********

You may want to use your captured traces to perform an attack based on a type
of side-channel analysis, in comes the analyzer. You can import the analyzer API
using::

    import chipwhisperer.analyzer as cwa

One of the ways to analyze your traces is using correlation power analysis (CPA).
The :func:`cpa <chipwhisperer.analyzer.cpa>` function returns a
:class:`CPA <chipwhisperer.analyzer.attacks.cpa_new.CPA>` object that can be used to
run the attack.

.. automodule:: chipwhisperer.analyzer

    .. autoattribute:: chipwhisperer.analyzer.cpa
        :annotation: cpa(proj, leak_model, algorithm=cpa_algorithms.Progressive)

    .. data:: leakage_models

        Contains all available leakage models that can be passed to the
        :func:`cpa <chipwhisperer.analyzer.cpa>` function as the **leak_model** parameter.

        Instance of the
        :class:`EightBitAES128LeakageModels <chipwhisperer.analyzer.attacks.models.EightBitAES128LeakageModels>`
        class.

        .. versionadded:: 5.1
            Add leakage_models to anlayzer API.


.. _api-analyzer-preprocessing:

Preprocessing
==============

Before performing an attack you may want to apply some pre-processing to the
captured traces. There is currently one pre-processing class that has been
moved to the latest ChipWhisperer API.

.. automodule:: chipwhisperer.analyzer.preprocessing.resync_sad

    .. autoclass:: ResyncSAD
        :members: max_shift, ref_trace, target_window, preprocess

.. _api-analyzer-leakage_models:

Leakage Models
==============

The analyzer includes various leakage models that can be passed to
the :func:`cpa <chipwhisperer.analyzer.cpa>` as the **leak_model**
argument. The
:data:`leakage_models <chipwhisperer.analyzer.leakage_models>`
variable provides a convenient way to list and access all the available
leakage models::

    import chipwhisperer.analyzer as cwa
    print(cwa.leakage_models)

Any attribute can be chosen from this list and accessed with the dot
syntax.

.. automodule:: chipwhisperer.analyzer.attacks.models

    .. autoclass:: EightBitAES128LeakageModels
        :members: 

.. _api-analyzer-cpa_attack:

CPA Attack
==========

The interface class to carry out a correlation power analysis attack on the
previously captured traces. The easiest way to create a default CPA instance
that is ready to perform an attack is to use the
:func:`cpa <chipwhisperer.analyzer.cpa>` function.

.. automodule:: chipwhisperer.analyzer.attacks.cpa_new

    .. autoclass:: CPA
        :members: change_project, run


Results
-------

The :meth:`cpa.run <chipwhisperer.analyzer.attacks.cpa_new.CPA.run>` function will
return a results object. There are a few methods for interacting with the results
object.

.. autoclass:: chipwhisperer.analyzer.attacks._stats.Results
    :members: best_guesses, simple_PGE, set_known_key, find_maximums, key_guess


.. _api-analyzer-utilities:

Utilities
=========

This section contains relatively independent functions that are useful for
working with AES, and traces.

.. _api-analyzer-utilities-aes_functions:

AES Functions
-------------

You may want to perform certain AES operations seperately. For this there is the AES
helper functions. They are accessible using::

    from chipwhisperer.analyzer import aes_funcs

Available functions:
  * :func:`aes_funcs.sbox <chipwhisperer.analyzer.attacks.models.aes.funcs.sbox>`
  * :func:`aes_funcs.inv_sbox <chipwhisperer.analyzer.attacks.models.aes.funcs.inv_sbox>`
  * :func:`aes_funcs.subbytes <chipwhisperer.analyzer.attacks.models.aes.funcs.subbytes>`
  * :func:`aes_funcs.inv_subbytes <chipwhisperer.analyzer.attacks.models.aes.funcs.inv_subbytes>`
  * :func:`aes_funcs.shiftrows <chipwhisperer.analyzer.attacks.models.aes.funcs.shiftrows>`
  * :func:`aes_funcs.inv_shiftrows <chipwhisperer.analyzer.attacks.models.aes.funcs.inv_shiftrows>`
  * :func:`aes_funcs.mixcolumns <chipwhisperer.analyzer.attacks.models.aes.funcs.mixcolumns>`
  * :func:`aes_funcs.inv_mixcolumns <chipwhisperer.analyzer.attacks.models.aes.funcs.inv_mixcolumns>`
  * :func:`aes_funcs.key_schedule_rounds<chipwhisperer.analyzer.attacks.models.aes.key_schedule.key_schedule_rounds>`

.. autofunction:: chipwhisperer.analyzer.attacks.models.aes.funcs.sbox

.. autofunction:: chipwhisperer.analyzer.attacks.models.aes.funcs.inv_sbox

.. autofunction:: chipwhisperer.analyzer.attacks.models.aes.funcs.subbytes

.. autofunction:: chipwhisperer.analyzer.attacks.models.aes.funcs.inv_subbytes

.. autofunction:: chipwhisperer.analyzer.attacks.models.aes.funcs.shiftrows

.. autofunction:: chipwhisperer.analyzer.attacks.models.aes.funcs.inv_shiftrows

.. autofunction:: chipwhisperer.analyzer.attacks.models.aes.funcs.mixcolumns

.. autofunction:: chipwhisperer.analyzer.attacks.models.aes.funcs.inv_mixcolumns

.. autofunction:: chipwhisperer.analyzer.attacks.models.aes.key_schedule.key_schedule_rounds


.. _api-analyzer-utilities-snr:

Signal to Noise Ratio
---------------------

Calculate the signal-to-noise ratio of your captured traces::

    import chipwhisperer.analyzer as cwa
    import chipwhisperer as cw

    project = cw.open_project('my_project')
    cwa.calculate_snr(project, cwa.leakage_models.sbox_output)

.. autofunction:: chipwhisperer.analyzer.calculate_snr
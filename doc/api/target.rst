******
Target
******

The target object provides the interface for configuring the target device
under test (DUT). The default target and currently only supported target type:

 * :ref:`sec-simpleserial-target`

The Simple Serial target type provides the target interface for all targets that use
a simple serial connection.

Initialization
--------------

.. autodata:: chipwhisperer.target
    :annotation: chipwhisperer.target(scope, target_type=targets.SimpleSerial, **kwargs)


.. toctree::
    :maxdepth: 2
    :hidden:

    target-simpleserial
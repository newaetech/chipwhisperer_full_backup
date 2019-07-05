.. _sec-target:

******
Target
******

The target object provides the interface for configuring the target device
under test (DUT). The default target and currently only supported target type:

 * :ref:`sec-target-simpleserial`

The Simple Serial target type provides the target interface for all targets that use
a simple serial connection. A object can be created using::

    import chipwhisperer as cw
    scope = cw.scope()
    target = cw.target(scope)

You now have access to the target configuration using the *target* variable.

.. autodata:: chipwhisperer.target
    :annotation: chipwhisperer.target(scope, target_type=targets.SimpleSerial, **kwargs)


.. toctree::
    :maxdepth: 2
    :hidden:

    target-simpleserial
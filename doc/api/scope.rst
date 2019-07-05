.. _sec-scope:

*****
Scope
*****

The :func:`chipwhisperer.scope` API function creates and returns a scope object
based on the supplied *type* argument. The are two currently supported classes for the
type argument:

 * :ref:`sec-scope-openadc`
 * :ref:`sec-scope-cwnano`

The **OpenADC** scope provides support for the *ChipWhisperer Lite* and the
*ChipWhisperer Pro*, and the **CWNano** scope provides support for the 
*ChipWhisperer Nano*.

The **scope** object provides the API for configuring the capture side of the
hardware (the scope). To create an instance of a connected scope use::

    import chipwhisperer as cw
    scope = cw.scope()

This will return a connected instance of one of the scope types or raise an
exception. If no scope type is given the function will try to determine the scope
type automatically.

.. autodata:: chipwhisperer.scope
    :annotation: chipwhisperer.scope(type=None, sn=None)

.. toctree::
    :maxdepth: 2
    :hidden:

    scope-openadc
    scope-cwnano
*****
Scope
*****

The :func:`chipwhisperer.scope` API function creates and returns a scope object
based on the supplied *type* argument. The are two currently supported classes for 
the type argument:

 * :ref:`sec-openadc-scope`
 * :ref:`sec-cwnano-scope`

The **OpenADC** scope provides support for the *ChipWhisperer Lite* and the
*ChipWhisperer Pro*, and the **CWNano** scope provides support for the 
*ChipWhisperer Nano*.

The **scope** object provides the API for configuring the capture side of the
hardware (the scope).

.. toctree::
    :maxdepth: 2
    :hidden:

    scope-openadc
    scope-cwnano


Initialization
--------------

.. autodata:: chipwhisperer.scope
    :annotation: chipwhisperer.scope(type=scopes.OpenADC, sn=None)
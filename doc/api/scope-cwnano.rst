.. _sec-scope-cwnano:

************************
ChipWhisperer Nano Scope
************************

.. autoclass:: chipwhisperer.scopes.CWNano

    .. attribute:: adc
        :annotation: scope.adc

        .. autodata:: chipwhisperer.capture.scopes.cwnano.ADCSettings.samples
            :annotation: scope.adc.samples

        .. autodata:: chipwhisperer.capture.scopes.cwnano.ADCSettings.clk_src
            :annotation: scope.adc.clk_src

        .. autodata:: chipwhisperer.capture.scopes.cwnano.ADCSettings.clk_freq
            :annotation: scope.adc.clk_freq

    .. attribute:: io
        :annotation: scope.io

        .. autodata:: chipwhisperer.capture.scopes.cwnano.GPIOSettings.tio1
            :annotation: scope.io.tio1

        .. autodata:: chipwhisperer.capture.scopes.cwnano.GPIOSettings.tio2
            :annotation: scope.io.tio2

        .. autodata:: chipwhisperer.capture.scopes.cwnano.GPIOSettings.tio3
            :annotation: scope.io.tio3

        .. autodata:: chipwhisperer.capture.scopes.cwnano.GPIOSettings.tio4
            :annotation: scope.io.tio4

        .. autodata:: chipwhisperer.capture.scopes.cwnano.GPIOSettings.pdid
            :annotation: scope.io.pdid

        .. autodata:: chipwhisperer.capture.scopes.cwnano.GPIOSettings.pdic
            :annotation: scope.io.pdic

        .. autodata:: chipwhisperer.capture.scopes.cwnano.GPIOSettings.nrst
            :annotation: scope.io.nrst

        .. autodata:: chipwhisperer.capture.scopes.cwnano.GPIOSettings.clkout
            :annotation: scope.io.clkout

    .. attribute:: glitch
        :annotation: scope.glitch

        .. autodata:: chipwhisperer.capture.scopes.cwnano.GlitchSettings.ext_offset
            :annotation: scope.glitch.ext_offset

        .. autodata:: chipwhisperer.capture.scopes.cwnano.GlitchSettings.repeat
            :annotation: scope.glitch.repeat

    .. automethod:: chipwhisperer.capture.scopes.cwnano.CWNano.default_setup

    .. automethod:: chipwhisperer.capture.scopes.cwnano.CWNano.arm

    .. automethod:: chipwhisperer.capture.scopes.cwnano.CWNano.capture

    .. automethod:: chipwhisperer.capture.scopes.cwnano.CWNano.get_last_trace

    .. method:: dis()

        Disconnects the current scope object.

        :return: A boolean of whether the disconnect was successful.

    .. method:: con(sn=None)

        Connects to attached chipwhisperer hardware (ChipWhisperer Nano)

        :param sn: The serial number of the attached device. Does not need to be
            specified unless there are multiple device attached.

        :return: A boolean of whether the connection was successful.

        :raises:
            :OSError: Raised when there is issues connecting to the hardware, such as
                user not having the correct device permissions to access the hardware.

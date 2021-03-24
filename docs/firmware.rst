.. _Updating_Firmware:

#################
Updating Firmware
#################

The following page details how to update the firmware on your ChipWhisperer Capture
or CW305 device. This consists of two separate steps: erasing firmware and flashing
new firmware.

This page is only for ChipWhisperer capture devices and the CW305 target board **NOT**
for target boards such as the CW308.

.. _single_step_update:

*******************
Single Step Update
*******************

If you're able to connect to your ChipWhisperer, the easiest way to update firmware is to use our
auto update API, which combines the erasing and flashing steps:

.. code:: python3

    import chipwhisperer as cw
    scope = cw.scope()
    prog = cw.SAMFWLoader(scope)
    prog.auto_program()

Or, if you're using a CW305:

.. code:: python3

    import chipwhisperer as cw
    target = cw.target(None, cw.targets.CW305)
    prog = cw.SAMFWLoader(target)
    prog.auto_program()

.. _erasing_firmware:

****************
Erasing Firmware
****************

If you're not able to connect to your ChipWhisperer, but want to update
its firmware, you'll need to erase the firmware using hardware pins
on your ChipWhisperer device. This involves shorting erase
header pins and is described on the https://rtfm.newae.com page
for your device.

.. _loading_firmware:

*****************
Flashing Firmware
*****************

You can flash new firmware onto your ChipWhisperer though our Python API:

.. code:: python3

    import chipwhisperer as cw
    prog = cw.SAMFWLoader(None)
    prog.program(<serial_port>, hardware_type=<hardware_type>)

Where :code:`<serial_port>` is the serial port of the AT91 Serial Bootloader (typically :code:`'/dev/ttyACMX'` or
:code:`'COMX'`) and :code:`<hardware_type>` is your hardware type (for example :code:`'cwlite'`). 
See :ref:`api-scope-update` for more information.

You can find the correct :code:`/dev/ttyACMX` via :code:`lsusb` on Linux or the correct
:code:`COMX` on Windows through device manager.

For Windows users, we also provide an executable for flashing new firmware onto your ChipWhisperer. 
The utility must be run from the command line and usage is the same as the program call above:

.. code:: bash

    firmware_update.exe <COM_PORT> <hardware_type>

For example:

.. code:: bash

    firmware_update.exe COM3 cw1200

It is most useful in the case where you're primarily using a VM, but are unable to flash new firmware
from your VM.

********************************
Special Case: Driverless Windows
********************************

If you're looking to upgrade your firmware and you don't already have
drivers installed, as will be the case on older firmware versions,
you have two options for updating your firmware:

 #. Temporarily installing drivers, then using :ref:`single_step_update`
 #. :ref:`erasing_firmware` using hardware pins, then :ref:`loading_firmware`

These temporary drivers can be installed via `Zadig`_ (use WinUSB), or by using Windows's driver update
on the files in :code:`chipwhisperer/hardware/newae_windowsusb_drivers.zip` (unzip them first). The latter drivers
are out of date, but should be replaced automatically when you update the firmware of your ChipWhisperer.

.. _Zadig: https://zadig.akeo.ie/

******************
Troubleshooting
******************

auto_program Fails
==================

If :code:`auto_program()` fails, you should continue on from :ref:`loading_firmware`.

Unable to flash firmware from a Virutal Machine/Linux
=====================================================

Ensure the AT91 bootloader is being passed through to the VM (commonly an issue on
VirutalBox) and that your user is a member of the :code:`dialout` group.

Incorrect Firmware Flashed onto ChipWhisperer
=============================================

Use the hardware erase pins to erase the firmware of your ChipWhisperer device as described
in :ref:`erasing_firmware`.
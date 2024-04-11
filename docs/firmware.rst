.. _Updating_Firmware:

#################
Updating Firmware
#################

Received a message about a ChipWhisperer firmware update being available? Accidentally
erased the firmware on your ChipWhisperer? If so, this page contains all the information
you need to get your device up and running again!

.. note:: All ChipWhisperer scopes and FPGA targets have a ROM base bootloader,
    meaning it is effectively impossible to "brick" your ChipWhisperer
    by updating or erasing its firmware.

.. warning:: This page only concerns firmware for ChipWhisperer scopes
    and FPGA boards like the CW310 or CW305, not for integrated
    targets or CW308 target boards

.. warning:: This update method for updating firmware only works
    for ChipWhisperer 5.6 and newer.
    For ChipWhisperer versions earlier than 5.6, see :ref:`api-scope-update`.

*********************
Prerequisites
*********************

=====================
Windows Prerequisites
=====================

If you are already running ChipWhisperer natively on Windows,
no steps are required before upgrading or flashing
new firmware onto your ChipWhisperer.

If you have ChipWhisperer installed and are unable to 
connect due to not having the correct driver, follow the steps
shown at :ref:`windows-change-drivers`.

=====================
Linux Prerequisites
=====================

If you are running natively on Linux, you will need to 
ensure you have access to the serial ports. As of
ChipWhisperer 5.6, a rule in :code:`chipwhisperer/hardware/50-newae.rules`
will give serial port access to the current user

Older versions do not include this rule, so it is recommended to replace :code:`/etc/udev/rules.d/50-newae.rules`
with ChipWhisperer 5.6's :code:`chipwhisperer/hardware/50-newae.rules`, then run :code:`$ sudo udevadm control --reload-rules`

=================
Mac Prerequisites
=================

Coming soon!

========================
VirtualBox Prerequisites
========================

In addition to the prerequisites for your guest system, you will also need
to ensure that the serial bootloader is being passed through from your host, like
with your other ChipWhisperer devices. To add a USB passthrough filter
for the bootloader, go to the settings for your VM > USB and create a 
new USB filter with the following settings:

  * Name: ATSAM Bootloader
  * Vendor ID: 03eb
  * Product ID: 6124

This should already be done on the ChipWhisperer VM, but may not be present on
older VMs.

.. _upgrade-firmware-python:

******************
Upgrading Firmware
******************

If you just need to update the firmware on your ChipWhisperer,
the easiest way is to connect to your ChipWhisperer as usual,
then call the :code:`upgrade_firmware()` method. For example,
upgrading a ChipWhisperer-Lite::

    >>> import chipwhisperer as cw
    >>> scope = cw.scope()
    >>> scope.upgrade_firmware()
    Entering bootloader mode...
    Detected com port COM13
    Loading cwlite firmware...
    Opened!
    Connecting...
    Connected!
    Erasing...
    Erased!
    Programming file SAM3U_CW1173.bin...
    Programmed!
    Verifying...
    Verify OK!
    Resetting...
    Upgrade successful

If you don't see the "Resetting..." line, you'll need to power cycle your ChipWhisperer.

************************
Programming New Firmware
************************

If you've accidentally erased the firmware on your ChipWhisperer,
the best way to flash new firmware is to use ChipWhisperer's
:code:`program_sam_firmware()` function. You will need
to specify the device you are upgrading from the following list:

  * 'cwlite'
  * 'cw1200'
  * 'cwnano'
  * 'cwhusky'
  * 'cwbergen'
  * 'cw305'

For example, to flash new firmware to a CW305::

    >>> import chipwhisperer as cw
    >>> cw.program_sam_firmware(hardware_type='cw305')

*********************
Old Firmware Upgrade
*********************

For ChipWhisperer versions earlier than 5.6, see :ref:`api-scope-update`.

***************
Troubleshooting
***************

=======================
Firmware upgrade failed
=======================

If :code:`scope.upgrade_firmware()`/:code:`target.upgrade_firmware()` fails
for any reason, use :code:`cw.program_sam_firmware()`.

=====================================
Could not find bootloader serial port
=====================================

Ensure the device is plugged in. If you're using a VM,
ensure you have the USB device passed through.

==================
Permission Error
==================

If running on Linux, ensure you have the new :code:`50-newae.rules` file in
:code:`/etc/udev/rules.d/`, then run :code:`sudo udevadm control --reload-rules`.

Additionally, try unplugging and replugging your ChipWhisperer. If the udev
rules have been applied properly, you should see a :code:`cw_bootloader%n`
device in :code:`/dev/`, where :code:`%n` is some integer.

If you're still unable to program new firmware, try adding your user
to the :code:`dialout` group:

.. code:: bash

    sudo usermod -a -G dialout $USER

then logging out and back in again.

=======================================
Accidentally flashed incorrect firmware
=======================================

If the firmware you've flashed is for another NewAE device,
then, as of ChipWhisperer 5.7.1 or commit 068ec19, you can use
:code:`quick_firmware_erase()` as follows to erase the firmware on your
device:

.. code:: python

    from chipwhisperer.hardware.naeusb.naeusb import quick_firmware_erase
    quick_firmware_erase(<product_id>)

replacing :code:`<product_id>` with the product ID of the firmware that you've
uploaded. For example, if you've uploaded ChipWhisperer-Lite (0xACE2) firmware to
your ChipWhisperer-Nano (0xACE0), you should use 0xACE2 as your product ID. You
may find it helpful to use :code:`cw.list_devices()` to list connected NewAE devices.

Otherwise, short the erase pins on your ChipWhisperer to
enter bootloader mode, then flash the correct firmware onto your device.

Instructions for shorting the erase pins can be found
on the documentation page for your device at https://rtfm.newae.com

If you run into any issues not covered here, or if none of these steps
work for you, let us know over at https://forum.newae.com
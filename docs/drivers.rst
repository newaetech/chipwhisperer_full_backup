.. _windows-drivers:

###############
Windows Drivers
###############

Over the past year, we have been making a number of changes to the USB interface under 
the hood as we adapt the ChipWhisperer to a "plug and play" model for Windows. ChipWhisperer 
devices with firmware newer than 0.23/1.23 (released November 2020) will automatically 
configure themselves as a WinUSB device, instead of using the old and unmaintained libusb0 
driver. This update makes ChipWhisperer "plug and play" on all Windows, Linux, and Mac - 
if you have the ChipWhisperer software, you can use your device!

Since this firmware update, we have continued to support both USB drivers; however, as of 
the major ChipWhisperer release 5.6, we will no longer support libusb0. We are 
switching to a new Python USB library to take advantage of additional USB features, and this 
new library does not support libusb0.

****************************
Checking for Correct Drivers
****************************

To check if the correct drivers are installed, plug in your ChipWhisperer and open Device Manager. Next,
navigate to :code:`Universal Serial Bus devices` and find your ChipWhisperer device.
Double click on the device, go to :code:`Details`, and change the :code:`Property`
to :code:`Device instance path`. If the path includes :code:`MI_00`, then
you have the correct drivers installed on your device. If you are unable
to find your device, the device isn't under :code:`Universal Serial Bus devices`,
or if path doesn't include :code:`MI_00`, then you are on old firmware and/or
have incorrect drivers installed.

A ChipWhisperer with a correctly installed driver is shown below:

  .. image:: _images/Device\ Manager\ Correct.png
    :width: 400

.. _windows-change-drivers:

*********************
How to Change Drivers
*********************

If you're running on the old drivers, the recommended way to change drivers 
is to upgrade to the latest firmware and let Windows assign the correct driver.

If you're unable to connect due to having old firmware and old drivers, you
can assign the correct driver by:

 1. Installing ChipWhisperer (if you haven't done so already)
 2. Download and run Zadig.exe, available at https://zadig.akeo.ie/
 3. In Zadig, select your ChipWhisperer device (e.g. "ChipWhisperer-Lite"). If it doesn't appear, go to Options and select "List all devices"
 4. Make sure WinUSB is selected as the driver
 5. Click "Install Driver"

From there, you should be able to upgrade your firmware. Instructions are at :ref:`upgrade-firmware-python`.

Correct Zadig settings are shown below:

 .. image:: _images/zadig.png
    :width: 400


Once that's done:

 6. Open Device Manager
 7. Go to device manager and find your device (e.g. "ChipWhisperer Lite") under "Universal Serial Bus devices"
 8. Right click your Device and select "Uninstall Device"
 9. Select "Delete the driver software for this device" and click "Uninstall"
 10. Unplug and replug your ChipWhisperer

.. warning:: It is important that the steps to uninstall the Zadig driver are followed,
            as Zadig installation may interfere with some ChipWhisperer features.

A ChipWhisperer with an incorrectly installed driver is shown below (note that "libusb-win32" has been
replaced by "Microchip Tools" due to another driver):

 .. image:: _images/Device\ Manager.png
    :width: 400

Ensure you have "Delete the driver software for this device" selected when uninstalling the driver:

 .. image:: _images/Uninstall\ Device.png
    :width: 400

A ChipWhisperer with a correctly installed driver is shown below:

  .. image:: _images/Device\ Manager\ Correct.png
    :width: 400

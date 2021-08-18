.. _windows-drivers:

###############
Windows Drivers
###############

Over the past year, we have been making a number of changes to the USB interface under 
the hood as we adapt the ChipWhisperer to a "plug and play" model for Windows. ChipWhisperer 
devices with firmware newer than 0.23/1.23 (released November 2020) will automatically 
configure themselves as a WinUSB device, instead of using the old and unmaintained libusb0 
driver. This update makes ChipWhisperer "plug and play" on all Windows, Linux, and Mac – 
if you have the ChipWhisperer software, you can use your device!

Since this firmware update, we have continued to support both USB drivers; however, as of 
the major ChipWhisperer release 5.6, we will no longer support libusb0. We are 
switching to a new Python USB library to take advantage of additional USB features, and this 
new library does not support libusb0.

*********************
How to Change Drivers
*********************

If you're running on the old drivers, the recommended way to change drivers 
is to upgrade to the latest firmware and let Windows assign the correct driver.

If you're unable to connect due to having old firmware and old drivers, you can update by
following the instructions on our `ChipWhisperer-Jupyter page 
<https://github.com/newaetech/chipwhisperer-jupyter/blob/2aaf7e296b60d7291d28660224338d478a3298a6/ChipWhisperer-Lite%20Firmware%20Upgrade.ipynb>`_:

If you're on the latest firmware and are not getting the correct driver assigned,

 1. Open Device Manager and search for "libusb-win32"
 2. Under that header, find your device (ex. "ChipWhisperer-Lite")
 3. Right click the device and select "Uninstall Device"
 4. Select "delete the driver software for this device" and click "Uninstall"
 5. Unplug and replug your ChipWhisperer
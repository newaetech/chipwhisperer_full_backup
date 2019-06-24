.. _sec-install-windows-auto:

*****************
Windows Installer
*****************

A 32-bit and a 64-bit Windows installer will also be available in the 
future, typically from the `releases`_ page on Github attached as 
assets. The installer packages together:

 * ChipWhisperer directory (same as in GIT)
 * Python binary & required libraries.
 * Windows USB Drivers
 * Binary to run Jupyter

This will install ChipWhisperer to a location on your hard drive. Note 
it must be somewhere your user owns, as the portable Jupyter install 
will not run from areas the user cannot write to. 
*C:\\Users\\<YOUR_USER>* is a good install location, but *C:\\Program 
Files (x86)\\* isn't.

.. note::

   You will need to install compilers for the firmware manually (i.e. 
   `ARM toolchain`_ or the `AVR-GCC standalone`_) and place it on your *PATH*.


.. _ARM toolchain: https://developer.arm.com/open-source/gnu-toolchain/gnu-rm/downloads
.. _AVR-GCC standalone: https://www.microchip.com/mymicrochip/filehandler.aspx?ddocname=en607654
.. _releases: https://github.com/newaetech/chipwhisperer/releases



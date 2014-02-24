.. _tutorial:

Tutorial: Breaking AES (Straightforward)
=========================================

This tutorial will take you through a complete attack on a software AES implementation.
The specific implementation being attacked is a well-known AES implementation written
in C, which is likely to be similar to other implementations used by proprietary systems.

Setting up the Hardware
------------------------

This tutorial uses the :ref:`hwcapturerev2` hardware along with the :ref:`hwmultitarget`
board. Note that you **don't need hardware** to complete the tutorial. Instead you can
download `example traces from the ChipWhisperer Assembla <https://www.assembla.com/spaces/chipwhisperer/wiki/Example_Captures>`__.

This example uses the Atmel AVR in 28-pin DIP programmed with a *simpleserial* communications protocol. This is the default firmware
programmed into the devices, so you shouldn't need to do anything. If you've erased the device, you can see programming instructions
in the :ref:`installing` section.

The Multi-Target board should be plugged into the ChipWhisperer Capture Rev2 via the 20-pin target cable. The *VOUT* SMA connector is
wired to the *LNA* input on the ChipWhisperer-Capture Rev2 front panel. The general hardware setup is as follows:

TODO

Jumpers on the Multi-Target Victim board are as follows:

TODO

Setting up the Software
------------------------

It is assumed that you've already followed the guide in :ref:`installing`. Thus it is assumed you are able to communicate with the ChipWhisperer Capture Rev2 hardware (or 
whatever capture hardware you are using). Note in particular you must have configured the FPGA bitstream in the ChipWhisperer-Capture software, all part of the
description in the :ref:`installing` guide.

Assuming this setup is complete, you can confirm you are able to communicate with the hardware by opening the Capture software, and 


Capturing the Traces
---------------------

This tutorial uses a simple script that ships with the ChipWhisperer Capture software. The easiest method of accomplishing the trace capture is as follows:

1. Close & reopen the capture software (to clear out any previous connection which may be invalid).
2. From the *



Analyzing the Traces
---------------------



Next Steps
-----------









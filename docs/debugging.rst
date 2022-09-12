.. _cwdebugging:

############################
Debugging with ChipWhisperer
############################

One of ChipWhisperer's main objectives has always been to make it easier to learn about and experiment with 
side-channel attacks. This experience has always been best on our three primary targets - STM32F devices,
XMega devices, and AVR devices. One reason for this is that we support programming these devices via built-in
bootloaders, meaning no external hardware is required to use these devices. If you have a ChipWhisperer capture 
board, you're good to go!

As of firmware released with ChipWhisperer 5.6.1, we've extended this programming functionality to many new boards 
by implementing FTDI's MPSSE interface, which is what FTDI devices use to act as USB JTAG devices. This interface 
is supported by `OpenOCD <https://openocd.org/>`_, meaning you'll not only be able to use your ChipWhisperer to program many new devices,
but you'll also be able to use it to debug them as well! This is available on all of our capture boards,
with the ChipWhisperer Husky being able to use a dedicated 20-pin header for JTAG and the other boards via 
repurposing the SPI and PDI pins on the ChipWhisperer 20-pin connector. Additionally, the CW310 Bergen Board will 
support a JTAG connection using the SPI/Address pins between the onboard SAM3X and the FPGA, allowing you to debug a 
soft core running on the FPGA.

Both SWD and JTAG are supported.

.. warning::
    On Windows only a single process can control a USB device, so you need to disconnect from Python when using
    OpenOCD. See :ref:`debug_sec_limitations` for other limitations and details.

**************
MPSSE Tutorial
**************

We now have a tutorial that walks you through programming and debugging a CW308_STM32F3 via ChipWhipserer,
OpenOCD, and gdb `here <https://github.com/newaetech/chipwhisperer-jupyter/blob/master/demos/Debugging%20the%20CW308_STM32F3%20Using%20ChipWhisperer.ipynb>`_

**************************
Checking for MPSSE Support
**************************

If you're using a ChipWhisperer version older than 5.6.1/commit TBD, this feature is not supported. Otherwise,
you can use the feature checking interface to see if your device supports MPSSE:

.. code:: python

    import chipwhisperer as cw
    scope = cw.scope() # also works with the CW310
    if scope.check_feature("MPSSE"):
        print("MPSSE supported!")
    else:
        print("MPSSE not supported.")

********************************
Setting up Your ChipWhisperer
********************************

Hardware Connections
====================

If you are using a ChipWhisperer-Lite, ChipWhisperer-Pro, or ChipWhisperer-Nano,
you will need to make the following connections:

  * Connect SCK to TCLK/SWDCLK
  * Connect PDID to TMS/SWDIO
  * Connect GND on the ChipWhisperer to GND on the target.
  * Connect MISO to TDO. Unused in SWD mode.
  * Connect MOSI to TDI. Unused in SWD mode.
  * Connect PDIC to TRST. Unused in SWD mode and optional in JTAG mode.

If you are using a ChipWhisperer-Husky, you can connect the 20-pin digital IO header
directly to a 20-pin JTAG header on your target. Adapters that convert
the 20-pin JTAG header to other pinouts (e.g. 10-pin ARM Debug) should also work.

On the CW310, no manual connections are required.

Switching ChipWhisperer to MPSSE Mode
=====================================

On bootup, ChipWhisperer devices default to CDC mode, meaning
you must connect to the ChipWhisperer and swap it to MPSSE mode
to use it with OpenOCD:

.. code:: python

    import chipwhisperer as cw
    scope = cw.scope()
    scope.enable_MPSSE(1)

This will cause your ChipWhisperer device to disconnect and reeumerate,
replacing the CDC interface with the custom MPSSE interface. Some devices
must do additional ChipWhisperer configuration after this swap. The 
:code:`scope.enable_MPSSE(1)` call will attempt to handle this automatically,
but if call raises an exception, you will need to connect and finish this manually:

.. code:: python

    scope = cw.scope()
    scope.finish_mpsse_setup()
    scope.dis()

*******************
Basic OpenOCD Usage
*******************

ChipWhisperer includes a configuration file that does all of the common configuration,
included in the main ChipWhisperer repository at :code:`chipwhisperer/cw_openocd.cfg`.

After this file is used by OpenOCD, additional commands are required to select the correct 
ChipWhisperer via its USB VendorID and ProductID, as well as select JTAG or SWD mode. After this,
you can select your target file. Configuration must be done in this order, or openocd will exit 
with an error. The general command line configuration is:

.. code:: bash

    /path/to/openocd -f /path/to/cw_openocd.cfg -c "transport select <jtag or swd>" -c "ftdi vid_pid <VID> <PID>" -f "target/my_target.cfg"

For example, to connect to an STM32F3 over SWD using the ChipWhisperer-Lite (PID 0xace2)

.. code:: bash

    /path/to/openocd -f /path/to/cw_openocd.cfg -c "transport select swd" -c "ftdi vid_pid 0x2b3e 0xace2" -f "target/stm32f3x.cfg"

You can also place these commands into a :code:`.cfg` file by replacing :code:`-f /path/to/file.cfg` with :code:`source [/path/to/file.cfg]\n`
and :code:`-c "CMD"` with :code:`CMD\n`. If you place the following into :code:`/path/to/my_cfg.cfg` and
call :code:`/path/to/openocd -f /path/to/my_cfg.cfg`, it will be equivalent to the command that connected to
the STM32F3 via SWD using the ChipWhisperer-Lite:

.. code::

    source [/path/to/cw_openocd.cfg]
    transport select swd
    ftdi vid_pid 0x2b3e 0xace2
    source [target/stm32f3x.cfg]

.. note::
    The FTDI emulation mode means no special patches are needed for ChipWhisperer support, and forks of OpenOCD (which
    are common where support for a new device has been added by a vendor) should "just work". However the format of the
    OpenOCD configuration file is not always consistent, so you may need to adjust the cw_openocd.cfg file.

Programming via OpenOCD
=======================

After selecting the target config file, the following commands in a :code:`.cfg` file can be used to load
a firmware file onto the target:

.. code::

    init
    targets
    halt
    flash write_image erase /path/to/fw.elf
    flash verify_image /path/to/fw.elf
    reset run
    shutdown

Debugging with OpenOCD and GDB
==============================

Instead of directly uploading firmware via OpenOCD, you may instead connect via :code:`/path/to/my_cfg.cfg`,
then upload and debug using GDB.

.. code:: bash

    /path/to/openocd -f /path/to/my_cfg.cfg

Then in a new terminal:

.. code:: bash

    arm-none-eabi-gdb /path/to/fw.elf

Which should load you into an interactive GDB terminal interface. You can then connect to OpenOCD:

.. code::

    target extended-remote localhost:3333

halt the target:

.. code::

    monitor reset halt

and load the firmware file onto the target:

.. code::

    load

From there, you can use GDB commands to debug the target. We recommend using the :code:`help` command,
as well as taking a look at a basic GDB tutorial to become familiar with using GDB.

If you wish, you can pass initial commands to GDB on the command line. For example, the
connection, halt, and load commands can be replaced with:

.. code:: bash

    arm-none-eabi-gdb /path/to/fw.elf -ex "target extended-remote localhost:3333" -ex "monitor reset halt" -ex "load"

Included OpenOCD Scripts
========================

To make interacting with OpenOCD easier, some scripts are included in ChipWhisperer's `openocd` folder.
This includes both normal openocd scripts/config files, as well as `run_openocd.sh`, a shell script
that puts ChipWhisperer into MPSSE mode and can either use openocd to program a target, or
set openocd up for debugging. Usage can be printed by:

.. code:: bash

    ./run_openocd.sh -h

You can debug by specifying the ChipWhisperer, jtag/swd mode, and the openocd target config file:

.. code:: bash

    ./run_openocd.sh lite swd -- -f target/stm32f3x.cfg

Or program the target board by using the :code:`-p` flag, followed by the path to the firmware file:

.. code:: bash

    ./run_openocd.sh -p /path/to/fw.elf lite swd -- -f target/stm32f3x.cfg

This script will also enable JTAG/SWD over the ChipWhisperer Husky's USERIO pins, unless
:code:`--no-user-io` is specified.

.. _debug_sec_limitations:

***********
Limitations
***********

The MPSSE implementation on ChipWhisperer devices have the following limitations:

Windows Specific Limitations
=============================

On Windows, only a single process can connect to the ChipWhisperer at one time. This means you cannot connect to the
ChipWhisperer via the Python interface and via OpenOCD at the same time.

This appears to be a limitation of LibUSB on Windows and, as such, may be fixed in the future. `There is a LibUSB
issue tracking this <https://github.com/libusb/libusb/issues/1177>`_.

General Limitations
===================

Feature Limitations
-------------------

Pin Based Limitations
^^^^^^^^^^^^^^^^^^^^^

MPSSE mode takes control of some ChipWhisperer pins, meaning the following features will not be available:

  * Non MPSSE target programming (STM32, XMEGA, AVR, SAMBA)
  * ChipWhisperer-Husky stream mode (fixed on CW 5.6.2)
  * Control of PDID, nRST, and the SPI pins
    * nRST is open-drain during MPSSE mode
  * Control of PDIC on Husky (fixed on CW 5.6.2)

You can give normal functionality back to nRST and the SPI pins by running the following::

    scope.io.cwe.setAVRISPMode(0)

MPSSE can be reenabled by running the following command::

    scope.io.cwe.setAVRISPMode(1)

This nRST limitation means that nRST always behaves as a push-pull pin in MPSSE.

If Husky's USERIO header is used for MPSSE, the restrictions to nRST and SPI do not apply.


Other Limitations
^^^^^^^^^^^^^^^^^^^^^

The following features are not available when MPSSE mode is active:

  * CDC serial (normal cw.target based serial still works)
  * ChipWhisperer-Pro stream mode

To regain usage of these features, you must leave MPSSE mode by running::

    scope.enable_MPSSE(0)

Communication Speed
-------------------

The communication speed cannot be adjusted and is not fixed to any one value. In practice,
bits are typically sent out at a rate ~500kbps, with larger gaps between bytes.

Unsupported MPSSE commands
--------------------------

The following MPSSE commands are not supported:

* MCU Host Emulation commands
* General clock commands
* Wait on I/O high/low
* Adaptive clock
* Read data bits

General MPSSE Compatability
---------------------------

This implementation uses much smaller buffers than is required
by MPSSE (64B vs. 64KiB). As such, it is unlikely that this
implementation can be made to work with any software other
than OpenOCD.
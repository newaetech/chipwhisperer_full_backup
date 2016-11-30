Intel® Quark™ Microcontroller Software Interface
################################################

Overview
********

Intel® Quark™ Microcontroller Software Interface (QMSI) is a Hardware
Abstraction Layer (HAL) for Intel® Quark™ Microcontroller products.
It currently support the following SoCs:

* Intel® Quark™ D2000 Microcontroller (D2000)
* Intel® Quark™ SE Microcontroller C1000 (SE C1000)

.. contents::

Support
*******

Information and support regarding Intel® Quark™ MCUs can be found in the
following website:

http://www.intel.com/quark/mcu

Hardware Compatibility
**********************

This release has been validated with the following hardware:

* Intel® Quark™ SE Microcontroller C1000.
* Intel® Quark™ SE Microcontroller C1000 Development Platform.
* Intel® Quark™ Microcontroller D2000.
* Intel® Quark™ Microcontroller D2000 Development Platform.

External Dependencies
*********************

* The ISSM toolchain is required to build the source code. It provides both the
  IAMCU and the ARCMCU toolchains (i586-intel-elfiamcu and arc-elf32, respectively).
  Currently supported version is "2016-05-12".
* OpenOCD is required to flash applications and ROM files onto the SoC.
* GDB is optional, it is used as a supplement to OpenOCD for debugging.
* `Intel® System Studio for Microcontrollers <https://software.intel.com/en-us/intel-system-studio-microcontrollers>`_ is optional.

* The toolchain is provided from both within the ISSM package or `standalone tarballs <https://software.intel.com/en-us/articles/issm-toolchain-only-download>`_.


More info about dependencies can be found in `dependencies <doc/dependencies.rst>`__ file.

License
*******

Please refer to `COPYING <COPYING>`_.

Organization
************
::

	.
	├── board           : Board level drivers
	├── doc             : Project documentation and Guidelines
	│   └── api         : Doxygen documentation
	├── drivers         : Intel® SoC drivers
	│   └── include     : QMSI driver headers
	│   └── sensor      : SE C1000 Sensor Subsystem drivers
	├── examples        : Examples using the QMSI API
	├── include         : Top level headers
	├── soc             : Intel® MCUs SoCs support
	│   ├── quark_d2000 : Intel® Quark™ Microcontroller D2000 support
	│   └── quark_se    : Intel® Quark™ SE Microcontroller C1000 support
	└─ sys              : Application entry and Newlib syscalls


Building
********

The build system is based on the make tool.
Make sure you have downloaded the toolchain as described in `External Dependencies`_.

Environment
===========
You must first set the IAMCU_TOOLCHAIN_DIR environment variable.
Assuming the toolchain was unpacked into *$HOME/issm_2016/* and
that you would find *i586-intel-elfiamcu-gcc* at *$HOME/issm_2016/tools/compiler/gcc-ia/5.2.1/bin*, the variable can be set with:

``export IAMCU_TOOLCHAIN_DIR=$HOME/issm_2016/tools/compiler/gcc-ia/5.2.1/bin``

For SE C1000, if developing for the Sensor Subsystem (ARC), you must also set ARCMCU_TOOLCHAIN_DIR.
Assuming the ARC toolchain was unpacked into *$HOME/issm_2016/* and
that you would find *arc-elf32-gcc* at *$HOME/issm_2016/tools/compiler/gcc-arc/4.8.5/bin*, the variable can be set with:

``export ARCMCU_TOOLCHAIN_DIR=$HOME/issm_2016/tools/compiler/gcc-arc/4.8.5/bin``

SoC Targets
===========

Both D2000 and SE C1000 are supported. You can select them by setting the ``SOC``
variable.

To build for D2000:

``make SOC=quark_d2000``

To build for SE C1000:

``make SOC=quark_se``

SoC Core
========

On SE C1000, there are two separate cores: x86 (Intel® Lakemont) and sensor (ARC).
You can select them by setting the ``TARGET`` variable.

To build for the Lakemont core:

``make SOC=quark_se TARGET=x86``

To build for the ARC:

``make SOC=quark_se TARGET=sensor``

Note: When building applications for ARC on SE C1000, an x86 application must
be present which begins execution of the ARC. A sample application which does
this is present at:

``examples\quark_se\start_arc``

Build modes
===========

Debug and release builds are supported setting the ``BUILD`` variable.

To build in debug mode:

``make BUILD=debug``

To build in release mode:

``make BUILD=release``

Targets
=======

The top level Makefile contains one main make target: ``libqmsi``.
The output directory is ``build``.

Libqmsi is a library archive of all the QMSI drivers for the SoC.

To build the ``libqmsi`` target, run the following command from the top level
directory:

``make libqmsi``

To build any of the provided example apps run make inside the corresponding
directory or use the –C make option from the top level directory.

E.g. to build the ``hello_world`` example app (by default it will be built in
release mode):

``make –C examples/hello_world``

Advanced build options
======================

Some operating systems may use their own interrupt system instead of the one
provided by QMSI. In order to properly integrate with those OSs, the ISRs
defined in QMSI drivers should be compiled as regular functions (e.g. no
interrupt-related prologue and epilogue, no end-of-interrupt handling). So
when interrupts are handled externally, you should set
'ENABLE_EXTERNAL_ISR_HANDLING=1' when building libqmsi.

For instance, the following command builds libqmsi for D2000 with external
interrupt handling support.

``make libqmsi SOC=quark_d2000 ENABLE_EXTERNAL_ISR_HANDLING=1``

Saving CPU context on sleep modes
=================================

The Quark SE has support for sleep states that power off the CPU. When a
wake event happens, the CPU starts over from the reset vector as in a normal
power on. It is possible to build libqmsi with support to restore the CPU
context when coming back from sleep. To do so, build both libqmsi and the
bootloader with 'ENABLE_RESTORE_CONTEXT=1' (see the `Bootloader README`_ for
information on how to compile the bootloader).

``make libqmsi SOC=quark_se ENABLE_RESTORE_CONTEXT=1``

Make sure you have flashed the ROM firmware built with ENABLE_RESTORE_CONTEXT=1
otherwise this feature won't work properly.

Quark D2000 doesn't power off the CPU so the execution context is not lost.
For that reason, the ENABLE_RESTORE_CONTEXT option has no effect on Quark D2000
SoC.

For more information, see `power management <doc/power_management.rst>`_.

Flashing
========

For flashing the board OpenOCD must be used. You can optionally use gdb
as a frontend for OpenOCD as described below.

You must first flash a bootstrap rom before flashing an application.
Assuming the toolchain was unpacked into *$HOME/issm_2016/*, this can be
done with:

``$ cd $HOME/issm_2016/tools/debugger/openocd``

For D2000 start OpenOCD with the following command:

``$ ./bin/openocd -f scripts/board/quark_d2000_onboard.cfg``

For SE C1000 start OpenOCD with the following command:

``$ ./bin/openocd -f scripts/board/quark_se_onboard.cfg``

Create a new terminal session at this point and set environment variables accordingly.
Then launch a GDB session using:

``$ gdb``

To connect to the remote port, enter the following GDB command:

``(gdb) target remote :333X``

For D2000 and SE C1000 (Lakemont), the remote port value is 3333.
For SE C1000 (ARC), the remote port value is 3334.

``(gdb) monitor clk32M 5000``

For D2000, the following commands are used to flash a ROM and application to the device:

``(gdb) monitor load_image $PATH_TO_QM_BOOTLOADER/build/release/quark_d2000/rom/quark_d2000_rom.bin 0x0``

``(gdb) monitor load_image $PATH_TO_QMSI/examples/hello_world/release/quark_d2000/x86/bin/hello_world.bin 0x00180000``

For SE C1000, the following commands are used to flash a ROM and application to the device:

``(gdb) monitor load_image $PATH_TO_QM_BOOTLOADER/build/release/quark_se/rom/quark_se_rom.bin 0xFFFFE000``

Applications for the Lakemont core are flashed using the following command:

``(gdb) monitor load_image $PATH_TO_QMSI/examples/hello_world/release/quark_se/sensor/bin/hello_world.bin 0x40000000``

Applications for the ARC are flashed using the following command:

``(gdb) monitor load_image $PATH_TO_QMSI/examples/hello_world/release/quark_se/x86/bin/hello_world.bin 0x40030000``

Serial Output
=============

You can check UART console output with picocom or screen:

``$ picocom -b 115200 --imap lfcrlf /dev/ttyUSBXXX``

or

``$ screen /dev/ttyUSBXXX 115200``

Where /dev/ttyUSBXXX is the path to the attached UART device.
e.g. /dev/ttyUSB0


Known Issues and Workarounds
****************************

Please see `Known issues <KNOWN_ISSUES.rst>`_. for a list of known issues and
workarounds. For all fixed issues since the previous release please see
`Fixed issues <FIXED_ISSUES.rst>`_.

ROM memory usage
****************
The following table dictates ROM memory usage for this version of QMSI.

======== ======= ========================= ========================
SoC      Build   Data Size (Start address) BSS Size (Start address)
-------- ------- ------------------------- ------------------------
D2000    Debug   12 (0x002801A0)           2048 (0x002801C0)
-------- ------- ------------------------- ------------------------
D2000    Release 12 (0x002801A0)           2048 (0x002801C0)
-------- ------- ------------------------- ------------------------
SE C1000 Debug   20 (0xA800A220)           2048 (0xA800A240)
-------- ------- ------------------------- ------------------------
SE C1000 Release 20 (0xA800A220)           2048 (0xA800A240)
======== ======= ========================= ========================

Change Log
**********

Supported features
==================

* Always-On (AON) Counters.
* Always-On (AON) Periodic Timer.
* Always-On GPIO.
* Analog Comparators.
* Analog-to-Digital Converter (ADC).
* Clock Control.
* Direct Memory Access (DMA).
* DMA support for peripherals:

    + UART master for Lakemont
    + SPI master for Lakemont
    + I2C master for Lakemont
    + I2C slave for Lakemont
* Flash library.
* Flash Protection Regions (FPR).
* Firmware Update over UART (without authentication)
* General Purpose Input Output (GPIO).
* Inter-Integrated Circuit (I2C) master.
* Inter-Integrated Circuit (I2C) slave.
* Interrupt Controller Timer.
* Interrupt Controllers:

    + SE C1000 Lakemont (APIC)
    + SE C1000 ARC
    + D2000 (MVIC)
* SE C1000 Mailbox.
* SE C1000 Sensor Subsystem (ARC):

    + Timer
    + GPIO
    + SPI
    + I2C
    + ADC
* Memory Protection Regions (MPR).
* Pin Muxing.
* Power states.
* Pulse Width Modulation (PWM)/Timers.
* Real-Time Clock (RTC).
* Retention Alternating Regulator (RAR).
* Serial Peripheral Interface (SPI) master.
* System on Chip (SoC) Identification.
* Universal Asynchronous Receiver/Transmitter (UART).
* Update utilities.
* Watchdog Timer (WDT).
* Universal Serial Bus (USB) 1.1
* Power management for SE C1000

Unsupported Features
====================

* Serial Peripheral Interface (SPI) slave.
* I2S

.. Hyperlink targets
.. _`Bootloader README`:
        https://github.com/quark-mcu/qm-bootloader/blob/master/README.rst

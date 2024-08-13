Bootloader: Firmware Managment User Guide
#########################################

.. contents::

Overview
********

The Intel® Quark™ supports Firmware Management (FM) over the following
connections:

- SE C1000  USB         Linux/Windows
- SE C1000  UART        Linux/Windows
- D2000     UART        Linux/Windows

Hardware Setup
**************

The hardware setup consists of two main steps:

* Connecting the board to the computer using one of the supported connections
  (UART, USB).

* Having the possibility to connect the FM GPIO pin to ground.

The FM GPIO pin is the pin used to put the device into FM mode. The device has
to be reset while the GPIO is connected to ground.

Read the `resources documentation <boot_resources.rst>`__ for more
information on the hardware set-up.


FM GPIO pins
============

+----------------------------------+---------------------+----------+--------+
| BOARD                            | FM-PIN              |  UART    | BUTTON |
+==================================+=====================+==========+========+
| Intel® Quark™ SE Microcontroller |                     |          |        |
| C1000 Development Kit            | J14.43 (AON_GPIO_4) | UART 1   | SW1    |
+----------------------------------+---------------------+----------+--------+
| Intel® Quark™ Microcontroller    |                     |          |        |
| D2000 Development Kit            | J4.6 (GPIO_2)       | UART 0   | SW2    |
+----------------------------------+---------------------+----------+--------+


Software Setup
**************

Installing dfu-util
===================

A different variant of dfu-util must be used depending on the used connection.

Installing UART version
-----------------------

*dfu-util-qda* is the tool needed to flash QFU images over UART to the target
or manage the device. You can get the software from
https://github.com/quark-mcu/qm-dfu-util, follow the build instructions in
the `readme file
<https://github.com/quark-mcu/qm-dfu-util/blob/master/README.rst>`__.

Installing USB version
----------------------

*dfu-util* is the tool needed to flash QFU images over USB to the target or
manage the device.

* On Debian and Ubuntu systems you can install dfu-util by typing::

    sudo apt-get install dfu-util

* On Fedora the command line command to install dfu-util is::

    sudo dnf install dfu-util

dfu-util can also be installed manually. Please follow the instructions on
http://dfu-util.sourceforge.net/.

Enabling FM functionality in the bootloader
===========================================

In order to use the Firmware Management (FM) functionality on the target
platform, you must install a ROM with such functionality enabled. To do so,
perform the following steps:

* Get the latest TinyCrypt code from https://github.com/01org/tinycrypt.
* Checkout tag v0.2.0 and export the following environment variable::

    export TINYCRYPT_SRC_DIR=/PATH/TO/SOURCE/tinycrypt

.. warning:: It is highly recommended to perform a flash mass erase before
             flashing the rom.

Enabling UART functionality
---------------------------

* Compile the ROM code:

  Enable the Firmware Manager for UART::

    make rom SOC=quark_se ENABLE_FIRMWARE_MANAGER=uart

* Flash the new rom image to the target::

    python $(QMSI_SRC_DIR)/tools/jflash/jflash.py -r quarkse_dev ./build/release/quark_se/rom/quark_se_rom_fm.bin

Enabling USB functionality
--------------------------

A second stage bootloader is required, in order to use the USB dfu-util.

* Compile the ROM code:

  Enable the second stage bootloader::

    make rom SOC=quark_se ENABLE_FIRMWARE_MANAGER=2nd-stage

* Flash the new rom image to the target::

    python $(QMSI_SRC_DIR)/tools/jflash/jflash.py -r quarkse_dev ./build/release/quark_se/rom/quark_se_rom_fm_2nd_stage.bin

* Compile the 2nd stage bootloader::

    make -C 2nd-stage

* Flash the 2nd stage bootloder to address `0x4005b000`::

    python $(QMSI_SRC_DIR)/tools/jflash/jflash.py -u quarkse_dev ./2nd-stage/release/quark_se/x86/bin/2nd_stage_usb.bin

FM functionality usage
**********************

Creating and flashing a QFU image
=================================

Simple way (using 'make flash')
-------------------------------

* Reset the device while connecting the FM GPIO to ground.
* Compile, upload and run the example app.
* Change to the QMSI directory::

    cd <PATH_TO_QMSI>

  - For the UART connection::

      make -C <APP_DIR> flash SOC=<SOC> TARGET=<TARGET> SERIAL_PORT=<SERIAL_INTERFACE>

  - For the USB connection::

      make -C <APP_DIR> flash SOC=<SOC> TARGET=<TARGET> USB_DEVICE=<VENDOR_ID:PRODUCT_ID>

The SoC can be ``quark_se`` or ``quark_d2000`` depending on the used soc. The
target can be ``x86`` or ``sensor`` depending on the used core.


Make flash example
++++++++++++++++++

This example will show how to build and flash the blinky example for the
Quark SE C1000 x86 core. For UART, the used serial device is assumed to be
``/dev/tty0``; while for USB, the device Vendor ID and Product ID are assumed
to be ``8086`` and ``48FC`` respectively.

* Change the directory to the QMSI base directory::

    cd $QMSI_SRC_DIR

- For the UART connection::

    make -C examples/blinky flash SOC=quark_se TARGET=x86 SERIAL_PORT=/dev/tty0

- For the USB connection::

    make -C examples/blinky flash SOC=quark_se TARGET=x86 USB_DEVICE=8086:48FC

Step by step process (without make flash)
-----------------------------------------


* Change to the QMSI directory::

    cd <PATH_TO_QMSI>

* Build the project::

    make -C <APP_DIR> SOC=<SOC> TARGET=<TARGET>

The soc can be quark_se or quark_d2000 depending on the used soc. The target can
be x86 or sensor depending on the used core.

* Create the dfu file::

    python ./tools/sysupdate/qm_make_dfu.py -v <BINARY_FILE> -p <PARTITION>

The ``-p`` option is used to choose the flash partition. Partition 1 is used
by the x86 core and partition 2 is used by the Sensor Subsystem.

The ``-v`` option makes the tool output some information about the generated
image.

The output DFU image will have the same name of the binary file with the
``.dfu`` extension appended.

* Reset the device while connecting the FM GPIO to ground.
* Download the image.

  - Using a UART connection::

      dfu-util-qda -D <DFU_IMAGE> -p <SERIAL_INTERFACE> -R -a <PARTITION>

  - Using a USB connection::

      dfu-util -D <DFU_IMAGE> -d <VENDOR_ID:PRODUCT_ID> -R -a <PARTITION>

The ``-a`` option is used to choose the flash partition. Partition 1 is used
by the x86 core and partition 2 is used by the Sensor Subsystem.

The ``-R`` option will reset the device after the download is finished.

Step by step example
++++++++++++++++++++

This example will show how to build and flash the blinky example for the Quark
SE C1000 x86 core.  For UART, the used serial device is assumed to be
``/dev/tty0``; while for USB, the device Vendor ID and Product ID are assumed
to be ``8086`` and ``48FC`` respectively.

* Change the directory to the QMSI base directory::

    cd $QMSI_SRC_DIR

* Build the project::

    make -C examples/blinky SOC=quark_se TARGET=x86

* Create the dfu file::

    python ./tools/sysupdate/qm_make_dfu.py -v examples/blinky/release/quark_se/x86/bin/blinky.bin -p 1

* You should get the following output if you use the -v option::

    qm_make_dfu.py: QFU-Header and DFU-Suffix content:
          Partition:   1
          Vendor ID:   0
          Product ID:  0
          Version:     0
          Block Size:  2048
          Blocks:      2
          DFU CRC:     0x8741e6e7
    qm_make_dfu.py: blinky.dfu written

``blinky.dfu`` is your generated QFU image.

* Reset the device while connecting the FM GPIO to ground.
* Download the image.

  - Using a  UART connection::

      dfu-util-qda -D examples/blinky/release/quark_se/x86/bin/blinky.bin.dfu -p /dev/tty0 -R -a 1

  - Using a USB connection::

      dfu-util -D examples/blinky/release/quark_se/x86/bin/blinky.bin.dfu -d 8086:48FC -R -a 1


.. note:: The path of the binary may differ when building a D2000 or a
          Sensor Subsystem image.

Application Erase / System Information Retrieval
================================================

System information can be retrieved by a Python script located in the
repository's tools/sysupdate directory. This script uses the dfu-util(-qda)
binary to communicate with the device.

* Make sure qfu-util(-qda) is installed.
* Go to the tools/sysupdate directory.
* Run the python script `qm_manage.py --help` to display possible commands.

Erase Applications
------------------

* Enter device DFU mode by resetting the device while the FM GPIO is connected
  to ground:

  - Run the following command for the UART connection::

      qm_manage.py erase -p <SERIAL_INTERFACE>

  - Run the following command for the USB connection::

      qm_manage.py erase -d <VENDOR_ID:PRODUCT_ID>


.. note:: All applications except the bootloader will be erased.

System Information
------------------

* Enter device DFU mode by resetting the device while the FM GPIO is connected
  to ground:

  * Run the following command for the UART connection::

     qm_manage.py info -p <SERIAL_INTERFACE>

  * Run the following command for the USB connection::

     qm_manage.py info -d <VENDOR_ID:PRODUCT_ID>


.. note:: By specifying the ``--format`` option, the output format can be set
          to either text (default) or json.

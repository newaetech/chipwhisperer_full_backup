BOOTLOADER RESOURCES
####################

.. contents::

The bootloader uses dedicated resources to guarantee its flow. Hardware
designers and application developers should pay attention how they use the
resources to avoid unexpected behavior. The bootloader will not use resources
associated with a specific compile option if such an option is not set.

Overview
--------

+------------------+---------------+----------------+------------------+
| Resource         | Quark D2000   | Quark SE C1000 | Notes            |
+==================+===============+================+==================+
|                  | | 0x00200000  | | 0x4002F000   |                  |
| Flash            | | 0x00201000  | | 0x40030000   | Reserved         |
+------------------+---------------+----------------+------------------+
| JTAG probe       | GPIO_13       | GPI0_15        | Do not ground    |
+------------------+---------------+----------------+------------------+
| Sleep register   | N/A           | GPS0 bit 1     | x86 restore bit  |
|                  | N/A           | GPS0 bit 2     | arc restore bit  |
+------------------+---------------+----------------+------------------+
| Sleep storage    |               | __x86_restore_ |                  |
|                  |               |  info          |                  |
+                  +               +                +                  +
| - x86 trap       | N/A           | 4 bytes        | Reserved         |
+------------------+---------------+----------------+------------------+
| UART port        | Uart 0        | Uart 1         | Available to app |
+------------------+---------------+----------------+------------------+
| USB controller   | n/a           | USB 0          | Available to app |
+------------------+---------------+----------------+------------------+
| FM register      | GPS0 bit 0    | GPS0 bit 0     | Reserved         |
+------------------+---------------+----------------+------------------+
| FM gpio pin      | GPIO_2        | AON 1          | Do not ground    |
+------------------+---------------+----------------+------------------+


Gpio pins
*********

The bootloader uses the following pins:

* JTAG probe pin:
    - Quark SE C1000:       ``GPIO_15``
    - Quark D2000:          ``GPIO_13``

  This pin is used to put the bootloader into recovery mode. Recovery mode can
  be used when the JTAG is not able to connect to the device during runtime.

* FM pin:  [Compile option: ``ENABLE_FIRMWARE_MANAGER=[uart|2nd-stage]``]
    - Quark SE C1000:       ``AON 0``
    - Quark D2000:    ``GPIO_2``

  This pin is used to start the bootloader in the Firmware Management (FM)
  mode.

**GPIO pins used by the bootloader should not be grounded during the boot
process.**

Sticky registers
****************

The bootloader uses the following sticky registers:

* Resume from sleep:  [Compile option: ``ENABLE_RESTORE_CONTEXT=1``]
    - Quark SE C1000:    ``GPS0 bit1 and the 4 bytes in esram_shared``

  The bootloader will use the sticky register to handle resuming the
  application from a sleep power state as well as the 4 bytes of the
  common RAM defined as a section esram_restore_info will be used to
  save the restore trap address.

* FM sticky bit:  [Compile option: ``ENABLE_FIRMWARE_MANAGER=[uart|2nd-stage]``]
    - All SoC's:    ``GPS0 bit 0``

  This register is used to start the bootloader in Firmware Management (FM)
  mode.

**The application must not use the sticky registers used by the bootloader.**

Peripherals
***********

The following peripherals are used by the bootloader:

* FM UART:  [Compile option: ``ENABLE_FIRMWARE_MANAGER=uart``]
    - Quark Se C1000:       ``UART1``
    - Quark D2000:          ``UART0``

* FM USB:  [Compile option: ``ENABLE_FIRMWARE_MANAGER=2nd-stage`` (and 2nd-stage bootloader programmed)]
    - Quark Se C1000:       ``USB0``
    - Quark D2000:          n/a

Both the FM UART and USB can be freely used by the application.

Time constraints
****************

The bootloader guarantees a boot-time smaller than 10ms during normal boot.
The boot time could increase during the first boot or after a Firmware
Management session.

Flash
*****

A part of the flash of the device is reserved for the bootloader data (BL-data).

+------------------+--------------------------+-------+----------------+
| SoC              | Address range            | Size  | Flash region   |
+==================+==========================+=======+================+
| Quark D2000      | 0x00200000 - 0x00201000  | 4kB   | Data           |
+------------------+--------------------------+-------+----------------+
| Quark SE C1000   | 0x4002F000 - 0x40030000  | 4kB   | System flash 0 |
+------------------+--------------------------+-------+----------------+

QMSI Power Management Architecture
##################################

Overview
********

This write-up summarises the Power Management flow in QMSI.

This document details the complete API. Some features might not
be available on a given SoC.

Please refer to the `SoC specific considerations`_ sections for more
details about your SoC.

Please refer to the `API resources`_ sections for more details about
reserved resources for Power Management.

.. contents::

Architecture
************

The Power Management API deals with two categories of power states:

* CPU states
        These states deal with a single CPU only and do not deal
        with any other core(s) that may be present in the SoC.

* SOC states
        These states deal with the SoC as a whole. Safely entering and
        recovering from these states requires both awareness of and
        interaction with other core(s) in the SoC.

Requirements
============

The following requirements are covered by the Power Management API:
* Enter individual CPU states.
* Enter individual SOC states.
* Save and Restore context after sleep states.

SoC specific considerations
***************************

Quark D2000
===========

Quark D2000 has a Power Management scheme where few states
are available at the CPU/SoC level. Power Management is controlled
at the peripheral level where clocks/power needs to be adjusted individually
in order to achieve the best power consumption.

In the context of sleep at SoC level, Quark D2000 neither loses the CPU context
nor the state of peripheral registers rendering the save/restore API detailed
below unnecessary. After sleep, Quark D2000 continues execution without
starting from the reset vector and does not execute any part
of the bootloader code.

Refer to ``soc/quark_d2000/include/power_states.h`` for more details.

Quark SE C1000
==============

Quark SE C1000 has multiple Core States and SoC states.

When entering a SoC level sleep state, the Quark SE C1000 loses both the CPU
context and the state of peripheral registers.
When waking up, the Quark SE C1000 restarts from the reset vector and the
bootloader code is executed.
The API below can be used to restore the previous execution context provided
that your platform design is such that it preserves the state of RAM across
sleep and restore cycles.

Refer to ``soc/quark_se/include/power_states.h`` for more details.

The API
*******

The API header is located at ``soc/$(SOC)/include/power_states.h`` and
``soc/$(SOC)/include/ss_power_states.h`` if sensor support is
available for your SOC.

See header Doxygen documentation for more details.

API Functions
=============

The header provides functions to:

* Enter CPU states through power_cpu_* functions or power_ss_cpu_* functions
  (when sensor support is available).
* Save and Restore CPU, IRQ and peripherals context as supported by the QMSI
  drivers. See the `Saving and Restoring context`_ section for more details.

Saving and Restoring context
****************************

In the case of the Quark SE C1000, when entering a sleep state, the only SoC
domain that keeps its state is the Always-On domain.
CPU, IRQ and peripheral states are lost.
In order to restore the context when waking up from sleep, the
ENABLE_RESTORE_CONTEXT build option needs to be set to 1.
This option is enabled by default for Quark SE C1000.

**Note: This feature will only work if RAM is preserved during sleep.
This is a platform design prerequisite.**

**Note: The USB peripheral on Quark SE C1000 does not support save / restore.
This is a known hardware limitation.**

In addition, please note that in order for this feature to work as expected,
the bootloader also needs to be built with the ENABLE_RESTORE_FEATURE
turned on.
Please refer to the bootloader documentation for more information about
the bootloader handling of the ENABLE_RESTORE_CONTEXT feature.

Several steps are performed by both the bootloader and QMSI in order
to properly restore the context:

1) Restore the program counter (PC) to the next instruction after sleep.
2) Restore the stack pointer.
3) Restore interrupt state.
4) Restore the state of peripheral registers.

x86 usage
=========

The following example shows how to use the QMSI power management API
to restore the x86 core (without restoring peripherals):

::

        int main(void)
        {
                /* Configure a wake up source. */
                register_my_wake_source();

                /* Save, enter sleep and restore. */
                power_soc_sleep_restore();

                return 0;
        }

Finer control can be achieved by directly calling lower level Power Management
API functions. Please refer to the API for more details.

In the Quark SE C1000, when working with the ARC core (sensor subsystem),
it is the responsibility of the x86 application to either start or resume
ARC code execution. This is performed by calling the `sensor_activation()`
function.

In order to be able to restore to a well-known state, we need to be able to
save and restore the state of both peripherals and interrupts.
This is over and above the state of the CPU core itself.
To this end, APIs have been added to QMSI supporting all the relevant
peripherals:

* qm_<peripheral_name>_save_context(peripheral index, context struct);

* qm_<peripheral_name>_restore_context(peripheral index, context struct);

As a general rule, interrupts and UART should be the first two to be restored.

**AON peripherals as well as those peripherals where nothing needs to be
restored, do not support the save and restore context API**

**Ongoing transfers are not restored (eg: SPI, I2C, UART), the application needs
to ensure peripherals are not busy before attempting to enter sleep otherwise
the transfer will be lost.**

ARC usage
=========

The same set of APIs are available to the ARC core.
The following details need to be taken into account when using sleep on the
ARC side:

* ARC application is started by the x86 application. This is demonstrated in the
  start_arc example: ``examples/quark_se/start_arc/main.c``

The following sample demonstrates the usage of the QMSI function in
order to restore the ARC core without peripherals:

::

        int main(void)
        {
                /* Configure a wake source. */
                register_my_wake_source();

                /* Save, enter sleep and restore. */
                ss_power_soc_sleep_restore();

                return 0;
        }

Finer control can be achieved by directly calling lower level Power Management
API functions. Please refer to the API for more details.

Multicore considerations
========================

In multicore applications, both the x86 and the ARC core need to collaborate to
enter sleep.
A communication channel needs to be setup between the cores so that sleep is
only entered when both cores are ready.

A set of functions are available to enable synchronization:

* power_sleep_wait(): Prepare x86 for sleep, sleep should be triggered from ARC.
* ss_power_sleep_wait(): Prepare ARC for sleep, sleep should be triggered
  from x86.

API resources
=============

For bootloader resouces details, refer to ``doc/boot_resources.rst`` in
the bootloader repository.

The QMSI Power Management API makes usage of the following SoC resources:

+------------------+---------------+----------------+------------------+
| Resource         | Quark D2000   | Quark SE C1000 | Notes            |
+==================+===============+================+==================+
| Sleep register   |               |                |                  |
+                  +               +                +                  +
|                  | GPS1 bit 6    | N/A            | deep sleep bit   |
+                  +               +                +                  +
|                  | N/A           | GPS0 bit 1     | x86 restore bit  |
+                  +               +                +                  +
|                  | N/A           | GPS0 bit 2     | arc restore bit  |
+------------------+---------------+----------------+------------------+
| Sleep storage    |               | __x86_restore_ |                  |
|                  |               |  info          |                  |
+                  +               +                +                  +
| - x86 trap       | N/A           | 4 bytes        | Reserved         |
+------------------+---------------+----------------+------------------+

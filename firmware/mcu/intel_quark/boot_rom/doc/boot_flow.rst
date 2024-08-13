BOOTLOADER FLOW
###############

This document describes the flow of the bootloader, from hardware boot to the
start of the application.

The bootloader has some dedicated resources, they are described in the
`Bootloader Resources document <boot_resources.rst>`_.

FLOW
****

#. Initialize x86 core:
     - Invalidate cache [Quark SE only].
     - Store Built-in-Self-Test (BIST) value in EBP.
     - Load the GDT table into the descriptor.
     - Move to 32bit protected mode.
     - Enable cache  [Quark SE only].

#. Resume from sleep: [Compile option: ``ENABLE_RESTORE_CONTEXT=1``]
    - Resume application execution if the device was put into sleep mode. A
      soft reboot is performed when a device comes out of sleep mode. The
      bootloader checks if the ``GPS1`` sticky register is set and perform a
      jump to the address stored in ``GPS1``.

#. Set-up primary peripherals and registers:
     - Stack pointer set-up
     - RAM set-up
     - Power set-up
     - Clock set-up: check if trim codes are stored in flash; compute trim
       codes and store them in flash if no trim codes were found.

#. JTAG probe hook:
     - The bootloader checks if the JTAG_PROBE_PIN is asserted (grounded) and,
       if so, it waits until the pin is de-asserted (ungrounded). This is used
       to unbrick a device with firmware that is preventing JTAG from working
       correctly.

#. Set-up secondary peripherals:
     #. IRQ set-up
     #. IDT set-up
     #. Set-up interrupt controller

#. Configure flash controller(s): [Compile option: ``ENABLE_FIRMWARE_MANAGER=[uart|2nd-stage]``]
     - Configure flash partition 0.
     - Configure flash partition 1 if the SoC is Quark SE.

#. Sanitize bootloader data: [Compile option: ``ENABLE_FIRMWARE_MANAGER=[uart|2nd-stage]``]
     - The bootloader checks if the meta-data partitions are not corrupted and
       fixes them if needed.

#. Start Firmware Manager: [Compile option: ``ENABLE_FIRMWARE_MANAGER=[uart|2nd-stage]``]
     - The bootloader goes into Firmware Management (FM) mode if the FM pin is
       asserted (grounded) or the FM sticky bit is set.

#. Start x86 application:
     - Start the x86 application if present (the first double word is different
       from 0xffffffff).

#. Sleep:
      - When the x86 application returns, the bootloader halts the x86 core
        (while leaving the state of the Sensor Subsystem unmodified).

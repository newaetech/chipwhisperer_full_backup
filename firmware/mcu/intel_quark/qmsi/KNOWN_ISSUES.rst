Known Issues and Workarounds
****************************

Affected version: QMSI 1.3.0.

=========== ====================================================================
Issue       UART 0 is reserved on SE C1000 development platform
----------- --------------------------------------------------------------------
Implication On SE C1000 development platform, UART 0 is reserved by the BLE
            module.
----------- --------------------------------------------------------------------
Workaround  Use UART 1 instead.
=========== ====================================================================

=========== ====================================================================
Issue       D2000 hangs if the UART prints during soc_deep_sleep before the
            system has fully restored to the active state.
----------- --------------------------------------------------------------------
Implication If the user callback attempts to send data over the UART during a
            soc_deep_sleep callback when the system is still transitioning to
            the active state, the system will hang on wake.
----------- --------------------------------------------------------------------
Workaround  Avoid printing over the UART during user callbacks until after the
            SoC has fully resumed operations in the active state.
=========== ====================================================================

=========== ====================================================================
Issue       I2C transfer speeds are sub-optimal.
----------- --------------------------------------------------------------------
Implication I2C transfer speeds are not as fast as they could be.
----------- --------------------------------------------------------------------
=========== ====================================================================

=========== ====================================================================
Issue       I2C driver has hard dependency on DMA code.
----------- --------------------------------------------------------------------
Implication I2C driver includes DMA code even if functionality is unused.
----------- --------------------------------------------------------------------
Workaround
=========== ====================================================================

=========== ====================================================================
Issue       SE C1000 SPI / Sensor SPI example may return incorrect CHIP ID.
----------- --------------------------------------------------------------------
Implication There is a chance after flashing the example application that the
            returned value of CHIP ID is incorrect causing the application to
	    fail.
----------- --------------------------------------------------------------------
Workaround  After flashing the application onto the board, perform a cold reset.
=========== ====================================================================

=========== ====================================================================
Issue       Bootloader fails to update over USB.
----------- --------------------------------------------------------------------
Implication There is a very small chance that when using dfu-utils to update an
            image on SE C1000 SoC that the transfer will hang.
----------- --------------------------------------------------------------------
Workaround  Re-initiate file transfer.
=========== ====================================================================

=========== ====================================================================
Issue       Mixing debug and release binaries for Mailbox example causes issues.
----------- --------------------------------------------------------------------
Implication When executing Mailbox on SE C1000, if there is a mixture between
            release ande debug binaries used, unexpected results can occur.
----------- --------------------------------------------------------------------
Workaround  Use either only release binaries or debug binaries.
=========== ====================================================================

=========== ====================================================================
Issue       After restoring context on I2C slave, the I2C master reports
            tx_abort events, but slave receives data.
----------- --------------------------------------------------------------------
Implication I2C master reports no ACK bit from slave after restoring context of
            I2C on slave, when running on SE C1000.
----------- --------------------------------------------------------------------
Workaround  Do not enter low power modes on SE C1000 when performing I2C slave
            transfers and using save/restore context functionality.
=========== ====================================================================

=========== ====================================================================
Issue       After restoring context on SS ADC on SE C1000, not all settings are
            restored.
----------- --------------------------------------------------------------------
Implication On SE C1000, after a restore cycle, the SS ADC settings are not
            restored correctly, this includes any calibration and mode settings.
	    There is also a spurious interrupt if interrupts were enabled.
----------- --------------------------------------------------------------------
Workaround  After resuming, re-calibrate and set mode of SS ADC. Disregard
            spurious interrupt.
=========== ====================================================================

=========== ====================================================================
Issue       On SE C1000, if mass erase is performed on the ARC flash partition,
            garbled output can be observed on the UART.
----------- --------------------------------------------------------------------
Implication When using save/restore functionality on SE C1000, if the ARC flash
            is mass erased, garbled text can be seen on the output of the UART
	    instead of expected text.
----------- --------------------------------------------------------------------
Workaround  Perform a cold reset after performing a mass erase of ARC flash.
=========== ====================================================================

=========== ====================================================================
Issue       OpenOCD loses connection with ARC when it is in sleep mode.
----------- --------------------------------------------------------------------
Implication OpenOCD resume command fails to affect ARC after it has been
            restored from sleep mode.
----------- --------------------------------------------------------------------
Workaround  After restoring from sleep mode, re-establish OpenOCD connection
            ARC and then issue resume command.
=========== ====================================================================

=========== ====================================================================
Issue       Cannot use comparator 0 after SE C1000 wakes up.
----------- --------------------------------------------------------------------
Implication On SE C1000, after performing a sleep restore cycle, comparator 0 is
            behaves erratically regarding interrupts.
----------- --------------------------------------------------------------------
Workaround  Use another comparator instead.
=========== ====================================================================

=========== ====================================================================
Issue       SE C1000 SS SPI does not restore clk polarity after save/restore
            cycle.
----------- --------------------------------------------------------------------
Implication On SE C1000, after performing a sleep restore cycle, the SS SPI clk
            polarity is not restored to what it was prior to entering sleep
	    modes.
----------- --------------------------------------------------------------------
Workaround  Re-set clk polarity after restoring context on SE C1000.
=========== ====================================================================

=========== ====================================================================
Issue       SPI master transfer speeds are sub-optimal when using save/restore
            builds on SE C1000.
----------- --------------------------------------------------------------------
Implication SPI master performance is degraded when using save/restore
            functionality on SE C1000.
----------- --------------------------------------------------------------------
Workaround  Either reduce transfer speeds on SPI master or disable save/restore
            context.
=========== ====================================================================

=========== ====================================================================
Issue       Reset button issue with SE C1000 development platform fab E.
----------- --------------------------------------------------------------------
Implication On Fab E of SE C1000, if the SoC has been halted and the reset
            button is pressed, the SoC does not resume properly.
----------- --------------------------------------------------------------------
Workaround  Power down the board and repower it to restore correct operations.
            Or use the reset button while the SoC is in an active state.
=========== ====================================================================

=========== ====================================================================
Issue       SE C1000 DMA freezes if performed after a save/restore cycle.
----------- --------------------------------------------------------------------
Implication When operating in debug mode, after performing a sleep/restore cycle
            if DMA is then configured, the system is flooded with block
	    interrupts and stalls, rendering it unusable.
----------- --------------------------------------------------------------------
Workaround  Disable save/restore context or run in release mode.
=========== ====================================================================

=========== ====================================================================
Issue       SE C1000 ARC performance is reduced after sleep.
----------- --------------------------------------------------------------------
Implication After performing a sleep/restore cycle, the ARC performance drops.
----------- --------------------------------------------------------------------
Workaround  Enable the instruction cache on the ARC core after sleep.
            Refer to sys/sensor/init.c for the exact sequence.
=========== ====================================================================

=========== ====================================================================
Issue       Repeated runs of AON counters for SE C1000 may result in failures.
----------- --------------------------------------------------------------------
Implication AON periodic timer may fail to have ready bit set if example is
            re-run multiple times on SE C1000.
----------- --------------------------------------------------------------------
Workaround  Perform cold reset in-between runs.
=========== ====================================================================
.. _tutorialresync:

Tutorial #A1: Synchronization to Communication Lines
====================================================

One of the first requirements of attacking real devices will be to synchronize to
communications from the device itself.

Using ChipWhisperer-Lite Hardware
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The ChipWhisperer-Lite does not currently have hardware triggers for I2C/SPI/UART. This
is something which will be added in a future firmware release.

Using ChipWhisperer-Capture Rev2 Hardware
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The ChipWhisperer-Capture Rev2 includes an arbitrary digital pattern trigger. The
ChipWhisperer-Capture software provides an example of using this with a UART data
line, to trigger on specific UART data.

See tutorial #A2 for details of using this feature.

Using External Hardware
^^^^^^^^^^^^^^^^^^^^^^^

The triggers built into the ChipWhisperer hardware are considerably more basic than
those available in normal bench test equipment. This means one of the best methods
to achieve advanced triggering is by using standard bench hardware. Examples of
suitable hardware are:

* `Teledyne Lecroy Logic Studio <http://teledynelecroy.com/logicstudio/>`_, a $900
  logic analyzer with hardware trigger out for detection of I2C, SPI, or UART traffic.
  
* Oscilloscope with I2C/Serial/SPI trigger option and trigger out capability.

* Custom FPGA design to look for specific pattern of data.

Simply connect this "Trigger Out" to one of the IO lines on the ChipWhisperer, and specify
that IO line as the trigger event.




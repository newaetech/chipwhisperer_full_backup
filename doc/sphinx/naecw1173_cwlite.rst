.. _naecw1173_cwlite:

CW1173: ChipWhisperer-Lite Board
================================

The ChipWhisperer-Lite Bare Board consists of two main parts: a multi-purpose power analysis capture instrument, and a target board. The target board
is a standard microcontroller which you can implement algorithms onto. For example if you wish to evaluate an AES library, you can program that library
into the target board and perform the power analysis.

.. image:: /images/cw1173/cwlite_basic.png

Quick-Start Guide
-----------------

Software Installation
^^^^^^^^^^^^^^^^^^^^^

Driver Installation
^^^^^^^^^^^^^^^^^^^

Basic Usage Instructions
------------------------

Programming XMEGA Device
^^^^^^^^^^^^^^^^^^^^^^^^

Programming AVR Device
^^^^^^^^^^^^^^^^^^^^^^

Using Glitch Port
^^^^^^^^^^^^^^^^^

Using Measure Port
^^^^^^^^^^^^^^^^^^

20-Pin Connector
^^^^^^^^^^^^^^^^



Breaking Target Section Apart
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

You may wish to break the target section apart from the main capture board. This can easily be accomplished by following these instructions:

1. Using a sharp knife (such as Xacto knife or retractable safety knife), cut the traces on the bottom side of the board along the cut line. Pass the knife
   back and forth several times. Scoring the board deeply will make the breaking process easier and less stressful on the PCB:
   
   .. image:: /images/cw1173/breakstep1.png
   
2. Score the board on the top side:

  .. image:: /images/cw1173/breakstep2.png
  
3. Select a surface to break the board over. It is suggested to have a piece of cardboard or boxboard down to protect components on the bottom side of the ChipWhisperer:

  .. image:: /images/cw1173/breakstep3.png
  
4. Hold the main board section flat, apply even pressure to the target board section. It should snap downward:

  .. image:: /images/cw1173/breakstep4.png
  
5. Separate the two sections:

  .. image:: /images/cw1173/breakstep5.png

You can see a `Video http://www.youtube.com/watch?v=8sIrvG5jqiQ&hd=1>`__ of the process here:

|YouTubeCW1173Break|_

.. |YouTubeCW1173Break| image:: /images/cw1173/cw1173breakvideo.png
.. _YouTubeCW1173Break: http://www.youtube.com/watch?v=8sIrvG5jqiQ&hd=1

Applying even pressure will help prevent damage to the ChipWhisperer-Lite main section. Flexing the PCB too much may cause damage to solder joints, but by holding the entire
board flat against the edge this is prevented.

Advanced Usage
--------------

Mounting Jumpers
^^^^^^^^^^^^^^^^

Note the ChipWhisperer-Lite main board and target section contain a number of jumper options. By default these are not mounted, and solder jumper bridges on the PCB have been
bridged to select the appropriate options when required. Some options are only solder jumpers, which to move the jumper requires a soldering iron to bridge or clear the appropriate
connections.

The following lists jumpers on the ChipWhisperer-Lite / Target Section:

Capture Section Jumpers:
 * JP4 is the "RESET" net for the SAM3U processor.
 * JP2 causes the SAM3U processor flash memory to be erased. When the chip is erased a rom-resident bootloader takes over. See section XXXXX for bootloader details.
 * JP5 selects the IO voltage for the FPGA bank which connects to the 20-pin target. By default SJ6 selects this to be 3.3V. It is not recommended to change this, as
   it is easy to damage the FPGA by feeding an out-of-range voltage in.
 * SJ1 selects if the power supply comes from the Micro-USB connector (default) or an external 5V supply at the +5VIN pin.

Target Section Jumpers:
 * JP7 connects the "MEASURE" SMA to the XMEGA VCC Rail. Shorted by default with SJ4.
 * JP6 connects the "GLITCH" SMA to the XMEGA VCC Rail. Shorted by default with SJ5.
 * JP12 can be used to feed an external voltage into the XMEGA VCC Rail. By default SJ3 connects this to 3.3V.
 * SJ2 selects if the 3.3V rail comes from the 20-pin IDC connector (i.e. ChipWhisperer-Lite board), or via an optional LDO and USB connector. 




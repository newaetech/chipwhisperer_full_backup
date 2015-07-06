.. _naecw303_cwlitexmega:

CW304 Target: NOTDUINO (ATMega328P Board)
=========================================

.. image:: /images/cw304/notduino.jpg

Board Usage
-----------


Using Arduino Software
----------------------


Kit Assembly
------------

The NotDUINO kit includes all the parts to assemble the NotDUINO kit. The following shows the kit parts:

.. image:: /images/cw304/kit_packaged.jpg

Kit Parts
^^^^^^^^^

Which you can break out into the following parts:

.. image:: /images/cw304/kit_parts.jpg

The following lists all the parts in the kit:

 === =================================================================
 Qty Description
 === =================================================================
 1   NPCB-CWTARG-NOTDUINO-02
 5   100nF Ceramic Capacitor (Marked with '104')
 2   220uF Electrolytic Capacitor
 5   51-ohm 1/4W Resistor (Green Brown Black Gold Brown), 1 spare
 3   330-ohm 1/4W Resistor (Orange Orange Black Black Brown), 1 spare
 2   10k-ohm 1/4W Resistor (Brown Black Black Red Brown), 1 spare
 1   Red 5MM LED
 1   Green 5MM LED
 1   ATMega328P-PU
 1   28-pin IC Socket
 2   Tactile Pushbuttons
 2   Jumpers
 1   1x5 Pin Header
 1   1x7 Pin Header
 1   2x3 Pin Header
 1   2x4 Pin Header
 1   20-Pin Shrouded Header
 2   SMA Edge-Mount Connector
 === =================================================================

Note the resistors use a 5-band colour code, which is different from the "regular" colour code you might be
used to. The following shows the resistors, note the colour code listed in the parts list above. There is an
extra resistor provided in the kit in case you drop one.

.. image:: /images/cw304/resistors.jpg

Assembly Process
^^^^^^^^^^^^^^^^

There is no specific assembly procedure for the board. The values of components have been marked on the blank PCB, here are some
general instructions for the assembly process:

1. You can use lead or lead-free solder due to the immersion gold finish of the PCB.
2. Mount parts starting with the lowest height (such as resistors and ceramic capacitors) first.
3. Watch the polarity of LED1 and LED2. The flat edge of the LED should match the flat marking on the silkscreen.
4. The electrolytic capacitors (C7 and C6) have the negative marked on them. Be sure to match up the negative with the "-" marking on the PCB.
   The two capacitors have the negative terminal facing each other.
5. The shrouded header has an orientation - match up the notch in the 20-pin header with the mark on the PCB silkscreen.
6. See the picture at the beginning of this section for details of the assembly.
7. The test-points (including the "GND") can be made using a cut-off resistor lead. This gives you something to clip a test point onto.

Assembly Video
^^^^^^^^^^^^^^

You can see a `Video <http://www.youtube.com/watch?v=zCmWzpyEYe8&hd=1>`__ of the assembly process of YouTube:

|YouTubeCW304Assmembly|_

.. |YouTubeCW304Assmembly| image:: /images/cw304/cw304asm.png
.. _YouTubeCW304Assmembly: http://www.youtube.com/watch?v=zCmWzpyEYe8&hd=1



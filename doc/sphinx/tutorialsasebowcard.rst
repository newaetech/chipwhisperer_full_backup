.. _tutorialsasebowcard:

Tutorial #B10: Using with SASEBO-W CardOS or DPA Contestv4
==========================================================

Background
----------

A number of extremely useful tools for side channel analysis are distributed by Morita Tech Co., Ltd under the `SAKURA <http://satoh.cs.uec.ac.jp/SAKURA/index.html>`_
project name. In fact much the original ChipWhisperer system was developed on these tools, and a great debt is owed to Akashi Satoh for this development.


This tutorial will demonstrate how the ChipWhisperer system can be used in tandem with the SASEBO-W system. The first part of this tutorial will target
the same ATMegaCard used by the SASEBO-W, and the second part of the tutorial will demonstrate how the ATMega328p can be connected to the SASEBO-W using
the interposer board.

Hardware Setup
--------------

About the ATMega Card
^^^^^^^^^^^^^^^^^^^^^

The ATMega Card is shown below:

.. image:: /images/tutorials/basic/scard/megacard.jpg
  :width: 400

This card contains an Atmel ATMega163 die along with a 24C256 EEPROM. You can see the internal pinout of these cards online. It should be noted that this card is
in a SmartCard *form factor*, but is essentially just a very old microcontroller (AtMega163). If you are unable to find this card but still wish to perform these
experiments, there are two other options:

 1. Purchase an ATMega16, which can be programmed with the ATMega163 binary (.hex file). See an `Atmel App-Note <http://www.atmel.com/Images/doc2517.pdf>`_
    on the subject. The AtMega16 *will not* fit on the Multi-Target board, meaning you must build your own board. You can then connect the appropriate
    IO lines to the SmartCard interface.

 2. Rebuild your code for the ATMega328p. This should require minimal changes to the source code, but note you cannot program a .hex file for a Mega163 into
    a Mega328P directly. You will need the complete source code.

Alternatively of course you can package your target algorithm into something like the demo SimpleSerial project too. There is no real need to use the SmartCard APDU
format, and the interface tends to be much slower on the ChipWhisperer system.

Programming
^^^^^^^^^^^

You will need an image to program into the SmartCard. This tutorial uses the SASEBO-W Card OS. Details of this are available from the
`SASEBO-W Page <http://satoh.cs.uec.ac.jp/SAKURA/hardware/SASEBO-W.html>`_. Download the file entitled
*Smartcard sample binary for ATMega 163*, which is described in the document entitled *SASEBO-W Smart Card OS Specification Ver. 0.4-5*.

The first file will have a .hex inside it, which you must program using AVRStudio or similar. To use the built-in programmer,
the following connections should be set:

 1. Remove all jumpers from the AVR and XMEGA sections of the MultiTarget board.

 2. Remove the AtMega328p from the socket.

 3. Set the oscillator for *3.579 MHz* (JP18), and set the *CLKOSC* jumper (JP17).

 4. Mount all four jumpers on the *AVR-PROG* section (JP8).

 5. Shunt both the *GND* and *VCC* resistors, as the programming will fail with those resistors in the power lines (JP7).
 
The following image shows these connections:

.. image:: /images/tutorials/basic/scard/programming.jpg

Then use AVRStudio to program the .hex file. The instructions for doing this are as in :ref:`buildprogrammingavr`, however when selecting
the AVR type select *ATMega163* instead of *ATMega328p*. Check the *Read Signature* option works, if not double-check the above hardware
connections.

Hardware setup for using Card Socket
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The following describes the jumper settings when using the SmartCard socket on the MultiTarget Victim Board:

   1. NO jumpers mounted in XMEGA Portion or AVR Portion, ideally remove the AVR Chip as well
   2. 3.3V IO Level (JP20 set to *INT*.)
   3. The *3.579 MHz* oscillator is selected as the CLKOSC source (JP18)
   4. The *CLKOSC* is connected to the SmartCard Clock Network, along with connected to the *FPGAIN* pin (JP4)
   5. Trigger is selected as *AX2* (JP22) [NOTE: The silkscreen on the multi-target board is incorrect - this is actually the AUX1 pin. This tutorial will
       use the silkscreen notation instead of the actual pin for jumper settings]
   6. Power measurement taken from VCC shunt (JP7)
   7. Jumpers removed from the AVR-PROG header (JP8)
   8. Connect the GPIO1 line to the *RESET* line. This is done with a jumper from a line labeled *RXD* on the AVR section to one of
      the pins on the AVR-PROG section. Note you can alternatively connect this line to the *RST* pin on the blue 8-pin smartcard
      connector header. 

The following image shows this setup:

.. image:: /images/tutorials/basic/scard/attacksettings_new.jpg

Connect the 20-pin cable and SMA cable if not already connected, and plug your programmed MegaCard into the SmartCard socket. This completes
the hardware setup when using the card socket.

Hardware Setup using ATMega16
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The AtMega16 is binary-compatible with the AtMega163, meaning a hex file for the ATMega163 can be programmed directly into the ATMega16. Assuming
you have the SASEBO-W adapter, this simple schematic shows the required circuit:

.. image:: /images/sasebow/atmega16_sch.jpg

Note that we are not using the shunt on the SASEBO-W, as we have added filtering capacitors. Due to the long ribbon cable (with a high-frequency clock),
you'll end up with a lot of noise if you attempted to use the shunt on the SASEBO-W (which means no filtering caps on the AtMega16). The implementation
looks like this:

.. image:: /images/sasebow/atmega16.jpg

You'll have to program the AtMega16 device, which you can do as described in the SASEBO-W Quickstart guide. The only difference is you'll need to
change the device to the mega16. You'll also need to program the fuse bits, as by default the ATMega16 uses an internal oscillator, which has the
wrong frequency for the SASEBO-W communications.

Here's an example of the batch file which will program the ATMega16, along with programming the fuse bytes. You only need to program the fuse bytes
the first time you use this, otherwise you can just program the .hex file::

   avrdude -c sasebow -p m16 -P ft0 -B 115200 -u -e -U flash:w:AES.hex:a -U lfuse:w:0xD0:m -U hfuse:w:0x99:m -v
   pause


Hardware Setup using ATMega328p
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

See :ref:`sasebowmultitarget` for details of connecting the Multi-Target board to the SASEBO-W.

.. image:: /images/sasebow/sasebow_scardfake.jpg

Software Setup and Example Capture (CardOS)
-------------------------------------------

 1. Run the ChipWhisperer Capture software

 2. Select the following options on the *General Settings* tab:

  a. Scope Module: *ChipWhisperer/OpenADC*
  b. Target Module: *Smart Card*
  c. Trace Format: *ChipWhisperer/Native*

 3. Switch to the *Target Settings* tab. Set the following two options:

  a. Reader Hardware: *ChipWhisperer-SER* (NOTE: This is new a new option as of 0.09, the old -USI is not recommended anymore)
  b. SmartCard Protocol: *SASEBO-W SmartCard OS*

 4. Press the *Master Connect* button, the scope and target should both show as connected:

    .. image:: /images/tutorials/basic/scard/allcon.png

 5. Under the *Scope Settings* tab, make the following changes:

  a. OpenADC-->Gain-->Setting: *35*
  b. OpenADC-->Trigger Setup-->Mode: *Rising Edge*
  c. CW Extra-->Clock Source: *TargetIO-IN*
  d. CW Extra-->Trigger Pins: Uncheck *Front Panel A*
  e. CW Extra-->Trigger Pins: Check *Target IO4 (Trigger Line)*
  f. OpenADC-->Clock Setup-->ADC Clock-->Source: *EXTCLK x4 via DCM*
  g. Press the *Reset ADC DCM* button in that area, confirm the *ADC Freq* reads 14.3 MHz indicating the clock routing is working.
  h. OpenADC-->Trigger Setup-->Total Samples: *5000*
  
 6. Switch to the *Target* tab, confirm the correct ATR was found. You can reset the card as well from this page:
 
    .. image:: /images/tutorials/basic/scard/scard_cwser.png

 6. Finally press the *Capture 1* button. You should see a waveform like this:

    .. image:: /images/tutorials/basic/scard/waveform.png

 7. Using the encryption monitor, you can check the encryption monitor:
 
    .. image:: /images/tutorials/basic/scard/encmonitor.png


 8. You can now run a capture campaign and save the traces as before.

Changes for DPAContest v4.2 Card
--------------------------------

As of ChipWhisperer Release 0.09, the DPAContest v4.2 protocol is also supported. This protocol uses AES-128 RSM, making an interesting attack of a protected
target. The following details settings required to attack this card.

1. Change the *TRIG* jumper to *AX1* instead of *AX2*
2. In step 3-b above: Select the *
2. In step 5-b above: Under *Scope Settings*, the option OpenADC-->Trigger Setup-->Mode is set to *Falling Edge*



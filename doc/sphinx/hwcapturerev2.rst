.. _hwcapturerev2:

ChipWhisperer Capture Rev2
=============================================

Introduction to Capture Hardware
------------------------------------

There are three versions of the ChipWhisperer Capture Rev2, also called the CWCR2. They are:

* ChipWhisperer Capture Rev2 in a metal box, part of the *ChipWhisperer Complete Kit*
* ChipWhisperer Capture Rev2 as a PCB within tubing, part of the *ChipWhisperer Simple Kit*
* ChipWhisperer Capture Rev2 as a self-made PCB

Fundamentally all of these units are the same. The self-made PCB may of course have dropped certain
features, so some of these sections may not be relevant.

System Design
------------------------------------

ChipWhisperer Parts
^^^^^^^^^^^^^^^^^^^^^

The following figure highlights some of the hardware features of the ChipWhisperer Capture Rev2. Specifics of
each area will be discussed in this chapter.

.. _figcwcr2:
.. figure:: /images/cwrev2/pcblabels.jpg
    :figclass: figlarge
    
    ChipWhisperer Capture PCB.

    This shows the ChipWhisperer main PCB. Various features are highlighted on this figure.

Removing the Case
^^^^^^^^^^^^^^^^^^^^^

It may be neccesary to access the ChipWhisperer PCB. This is done by removing the rear panel, and sliding
out the bottom panel. Note that friction from the front panel may require the two lower front screws to be
slightly loosened. 

If it's desired to complete remove the main PCB, the front panel will also need to be removed. This is neccesary
to reduce the friction on the main PCB such that it can be slid out of the case. 

Access to the main PCB is shown in the following figures:

.. figure:: /images/cwrev2/caseremove1.jpg
    :figwidth: 35%

    Remove the four screws holding on the rear panel.
    
.. figure:: /images/cwrev2/caseremove2.jpg
    :figwidth: 35%
    
    It may also be possible to change a jumper setting without removing the bottom panel. If full
    access is needed, the bottom panel should be removed by sliding out.

.. figure:: /images/cwrev2/caseremove3.jpg
    :figwidth: 35%

    Two screws on the bottom front of the unit will need to be loosened, which should allow the bottom
    panel to slide freely. It's possible these screws do not need to be loosened, although for most units
    this is required.

.. figure:: /images/cwrev2/caseremove4.jpg
    :figwidth: 35%

    The bottom panel can now be fully removed.

Reflashing the AVR
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The AVRUSB portion mentioned in :ref:`figcwcr2`, which is an AT90USB162 device. This chip is programmed to function
as a programmer for the AVR or XMEGA device on the Multi-Target board. In many instances it's desired to reprogram
this AVR, for example to use it for communications with a SPI target device.

It should be possible to program the AVR by forcing it to revert to the bootloader, a process described below. If
this doesn't work, there is also a manual override available.

Programming Software
"""""""""""""""""""""""

Programming the AVR-USB is done via Atmel's FLIP software, available from `Atmel's Website <http://www.atmel.ca/tools/FLIP.aspx>`__.
As an alternative you can use the `DFUProgrammer <http://dfu-programmer.sourceforge.net/>`__ software, which is open-source
and works on both Windows and Linux.

This programmer will interact with a *bootloader* that is always resident on the AVR-USB device. Once it becomes active
it will require you to install a new driver, since it appears to your computer as a completely different device. Both
FLIP and dfuprogrammer come with drivers, so just point your *New Device Found* wizard to the appropriate location if 
they aren't automatically picked up. For FLIP it would be something like ``C:\Program Files (x86)\Atmel\Flip 3.4.5\usb``.

The sequence of steps to program a new ``.hex`` file into the AVR are as follows:

1. Run the FLIP software. Select the *Device-Select* option (or press Ctrl+S) the first time you run the software
    
    .. image:: /images/cwrev2/flip1.png
       :width: 35 %
    
2. And select the AT90USB162 device. You should only have to do this once.

    .. image:: /images/cwrev2/flip2.png
       :width: 35 %

3. Force the AVR-USB Bootloader to run before the next step. Note the bootloader will automatically time out if
   no command is received, so you should only force it to run once you are ready for step 4. See the next two 
   subsections of this section for details.
   
4. Press the USB button, and hit the *USB* option:

    .. image:: /images/cwrev2/flip3.png
       :width: 35 %
    
5. Press the *Open* button:

    .. image:: /images/cwrev2/flip4.png
       :width: 35 %
    
6. The main window should light up with options! It will now look something like this:

    .. image:: /images/cwrev2/flip5.png
       :width: 75 %
    
7. Load a new HEX file. You can find them in the ChipWhisperer git in the ``chipwhisperer\hardware\capture\chipwhisperer-rev2\avrusb``
   folder of ChipWhisperer releases (or in GIT).
   
    .. image:: /images/cwrev2/flip6.png
       :width: 35 %
    
8. Confirm that the window now reflects your selected file. Press the *Run* button on the left side of the screen
   to start the programming.
   
    .. image:: /images/cwrev2/flip7.png
       :width: 75 %
    
9. You should briefly see some indication of programming and verification pop up. If not try pressing the *Run*
   button again!
      
    .. image:: /images/cwrev2/flip8.png
       :width: 55 %
    
10. Finally press the *Start Application* button. This will cause the bootloader to exit and your application to
    start.
       
    .. image:: /images/cwrev2/flip9.png
       :width: 35 %

Jumping to Bootloader
"""""""""""""""""""""""

Normally you can jump to the bootloader without requiring the manual override. If invalid or corrupt code has been
loaded into the AVR, the manual override will be required. The specific method of jumping to the bootloader depends
which mode the AVR is currently in, the default is the *AVR-Programmer* mode (e.g. all ChipWhisperer units are
shipped in this mode).

From AVR-Programmer Mode
~~~~~~~~~~~~~~~~~~~~~~~~~

1. Start AVRStudio
2. Connect to the AVR Programmer. You DO NOT need an AVR connected, you may receive a message that the SPI command
   has failed, however you can ignore that.
3. Switch to the *HW Settings* tab and press the *Upgrade* button:

    .. image:: /images/cwrev2/avrprog_jump.png
    
4. The device will now jump to the bootloader. If pressing the *Upgrade* button did nothing you may need to use
   the manual mode described below - CWCR2 devices purchased before July 1st, 2014 did not have the software-based
   upgrade feature.

From SPI-Target Mode
~~~~~~~~~~~~~~~~~~~~~~~

1. Start the ChipWhisperer-Capture Hardware, select the *SPI Target*
2. Connect to the Target

   .. image:: /images/cwrev2/spiboot1.png

3. Switch to the Target tab
4. Press the *Jump to Bootloader* button

   .. image:: /images/cwrev2/spiboot2.png

Forcing Bootloader to Run
"""""""""""""""""""""""""""

See the following figure for location of the two jumper pads. You will need to short them with something metallic
such as tweezers or a small flat-head screwdriver. In a pinch a coin such as a dime can be used. The required
operations are:

1. Plug the USB-A Cable in
2. Short the HWBOOT jumper (SJ1)
3. With the HWBOOT jumper shorted, temporarily short the RESET jumper (SJ2)
4. Release the HWBOOT jumper

This will cause the AVR to jump to the bootloader, and your programming software can be run.

.. figure:: /images/cwrev2/forcebootloader.jpg

    Forcing the AVR-USB Bootloader to run.
    
    This shows the two jumper pads (SJ1 and SJ2). SJ1 is the `HWBOOT` feature, and SJ2 is the `RESET` feature.


I/O Connections
------------------------------------

Target-IO Connections
^^^^^^^^^^^^^^^^^^^^^^

The TargetIO connector is on the rear side of the ChipWhisperer Capture R2 (CWCR2), and is a 20-pin connector.
The pinout can be found on the label on the top-side of the CWCR2, or marked on the top-side silk-screen for the
CWCR2.

Looking into the male connector on the ChipWhisperer Capture Rev2, the following is the pinout:

.. image:: /images/cwrev2/rear_targetio.png

The pinout is as follows:

============   =============   ====   ==================================================================
Number          Name           Dir     Description
============   =============   ====   ==================================================================
1                +VUSB (5V)      O     Raw USB Power. Not filtered.
2                GND             O     System GND.
3                +3.3V           O     +3.3V from FPGA Power Supply. Very high current can be supplied.
4                FPGA-HS1       I/O    High Speed Input (normally clock in).
5                PROG-RESET     I/O    Target RESET Pin (AVR Programmer).
6                FPGA-HS2       I/O    High Speed Output (normally clock or glitch out).
7                PROG-MISO      I/O    SPI input: MISO (for SPI + AVR Programmer).
8                VTarget         I     Drive this pin with desired I/O voltage in range 1.5V-5V.
9                PROG-MOSI      I/O    SPI output: MOSI (for SPI + AVR Programmer).
10               FPGA-TARG1     I/O    TargetIO Pin 1 - Usually UART TX.
11               PROG-SCK       I/O    SPI output: SCK (for SPI + AVR Programmer).
12               FPGA-TARG2     I/O    TargetIO Pin 2 - Usually UART RX.
13               PROG-PDIC      I/O    PDI Programming Clock (AVR Programmer), or CS pin (SPI).
14               FPGA-TARG3     I/O    TargetIO Pin 3 - Usually bidirectional IO for smartcard.
15               PROG-PDID      I/O    PDI Programming Data (AVR Programmer).
16               FPGA-TARG4     I/O    TargetIO Pin 4 - Usually trigger input.
17               GND            O
18               +3.3V          O
19               GND            O
20               +VUSB (5V)     O
============   =============   ====   ==================================================================

PLL Connections
^^^^^^^^^^^^^^^^^^^^^^

Expansion & Power Connections
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Front-Panel Analog
^^^^^^^^^^^^^^^^^^^^^^

Internal Connections and Jumpers
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The main ChipWhisperer Capture Rev2 PCB has several jumpers and connectors. This section explains the use of these
jumpers. Note you will need to remove the case (if present) to access these jumpers.

JTAG
""""""""""

Power Jumpers
"""""""""""""""

PLL Enable and I2C
""""""""""""""""""""""""

GND Test-Points
""""""""""""""""""""""""





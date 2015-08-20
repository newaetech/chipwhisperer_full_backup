.. _naecw1173_cwlite:

CW1173: ChipWhisperer-Lite Board
================================

The ChipWhisperer-Lite Bare Board consists of two main parts: a multi-purpose power analysis capture instrument, and a target board. The target board
is a standard microcontroller which you can implement algorithms onto. For example if you wish to evaluate an AES library, you can program that library
into the target board and perform the power analysis.

.. image:: /images/cw1173/cwlite_basic.png

Quick-Start Guide
-----------------

You can see a `Video <http://www.youtube.com/watch?v=MJmkYqA-LeM&hd=1>`__ of the quickstart guide, which will take you through all the setup items discussed here:

|YouTubeCW1173Demo|_

.. |YouTubeCW1173Demo| image:: /images/cw1173/cwlite_demo_video.png
.. _YouTubeCW1173Demo: http://www.youtube.com/watch?v=MJmkYqA-LeM&hd=1


Connection Quick-Start
^^^^^^^^^^^^^^^^^^^^^^

1. Install Python and Dependancies
   
   Windows:
     * Install Python distribution (such as WinPython), see :ref:`installingwinpython`
     * Using the Python Command Prompt, use pip to install the following::
     
        pip install pyside
        pip install configobj
        pip install pyusb
        pip install pyqtgraph 
        
      What is included in releases varies. If you get a message saying it is already installed you can simply continue with this tutorial.
       
   Linux:
     * Python should come pre-installed. If so you'll also need to install the following from your package manager:
     
        * pyside
        * configobj
        * scipy
        * numpy
     
        For example the following shows the install command for Ubuntu::
        
         sudo apt-get install python2.7 python2.7-dev python2.7-libs python-numpy python-scipy python-pyside python-configobj python-setuptools python-pip
         
     * Some packages may require installation via 'pip', as they are not always in your package manager repo::
        
         $pip install pyusb
         $pip install pyqtgraph         
        
   Mac OS-X:
      * Install homebrew, pyside, libusb, along with required Python modules.
      * See :ref:`macosxinstall` for full details.
      
2. Download and install ChipWhisperer

   * Download the latest release linked from ChipWhisperer.com
   * Unzip the file somewhere. Generally 'somewhere' will become your ChipWhisperer working directory. These examples assume
     you've chosen ``C:\chipwhisperer`` as your working directory.
   * Open a Python terminal (again if using WinPython be sure to run the specific Python command prompt),
     and run the following, adjusting paths as needed to refelct your working directory::

       cd c:\chipwhisperer\openadc\controlsw\python
       python setup.py develop
       cd c:\chipwhisperer\software
       python setup.py develop

    If you wish to confirm the installation worked, you can run ChipWhispererAnalyzer in the same terminal::

       cd c:\chipwhisperer\software\chipwhisperer\analyzer
       python ChipWhispererAnalyzer.py
       
4. Connect ChipWhisperer-Lite, install USB Drivers:

   Windows:
     * Plug micro-USB cable into ChipWhisperer-Lite
     * If the "New Hardware Found" wizard doesn't prompt you for drivers, go to the control panel and find the "ChipWhisperer-Lite" device, and select "Update Drivers".
     * You can find drivers in ``c:\chipwhisperer\hardware\capture\chipwhisperer-lite\``. You will likely need to extract them from the ``cwlite_usb_driver.zip`` file. If so
       simply extract them somewhere (i.e. your desktop), and then point the new hardware found wizard to that location.
       
   Linux:
     * Driver installation is not required, but if you do not update the 'udev' system, you will be unable to run ChipWhisperer-Capture as a regular user. To update
       the udev system, first make sure the ChipWhisperer-Lite is
       Make a file called ``/etc/udev/rules.d/99-cwlite.rules`` . The contents of this file should be::

        # allow users to claim the device
        SUBSYSTEM=="usb", ATTRS{idVendor}=="2b3e", ATTRS{idProduct}=="ace2", MODE="0664", GROUP="plugdev"

     * Add your username to the plugdev group::

        $ sudo usermod -a -G plugdev YOUR-USERNAME

     * And reset the udev system::

        $ sudo udevadm control --reload-rules

     * Finally log out & in again for the group change to take effect.
     
     * Connect the micro-USB cable
     
   MAC:
     * No special installation required - must ensure you have installed libusb via homebrew (see instructions at :ref:`macosxinstall`).

5. Run ChipWhisperer-Capture. This can be done from one of three ways:

    * Double-click on ``CWCapture.pyw`` in the ``chipwhisperer\software`` folder. You must have installed Python into your path for this to work.
    * run ``python CWCapture.pyw`` from the  ``chipwhisperer\software`` folder using a command prompt with Python in the path.
    * run ``python ChipWhispererCapture.py`` from the ``chipwhisperer\software\chipwhisperer\capture`` directory.
    
    The last option is the most reliable, in that it should always work on all platforms. If it doesn't start look for possible missing modules or
    other useful errors.
    
    .. hint::
    
      The first time you run ChipWhisperer-Capture or -Analyzer, the default setup of the screen is somewhat insane. You can drag windows around
      or close them to make it look more like the demos here. See the Video quickstart guide as well for details of that.
    
6. From the "Scripts" directory, run the ``ChipWhisperer-Lite: AES SimpleSerial on XMEGA`` script:

   .. image:: /images/cw1173/cwlite_simpleserial.png
   
   This should connect to the ChipWhisperer-Lite, program the FPGA, and run a few captures. Your screen should look something like this now:
   
   .. image:: /images/cw1173/cwdemo_normal.png

7. If the previous step fails, you may need to set the path for the "firmware". This is done by going to the "Tools" menu and selecting the
   "Config CW Firmware" option. Note on MAC OS X a special command is required instead sometimes, see :ref:`macosxinstall`.
   
   From there, hit the "FIND" button beside the "FPGA .zip (Release)" option. Point it to the file located at 
   ``chipwhisperer/hardware/capture/chipwhisperer-lite/cwlite_firmware.zip`` on your filesystem.

8. Your ChipWhisperer-Lite is now connected. See the next section for details of the demo attack.

Capture and Attack Quickstart
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

1. See :ref:`tutorialaes` for details. Note the hardware setup is slightly different -- but you can skip to step 5.5.3, and should be able to pick up
   from there. Be sure to use the ``ChipWhisperer-Lite: AES SimpleSerial on XMEGA`` script instead of the one referenced in step 5.5.3.
   

Important Bugs/Caveats
^^^^^^^^^^^^^^^^^^^^^^

The following includes various things that might trip you up:

1. If you save the project *before* running the capture, you can specify any directory. Traces
   will be copied to the appropriate location during capture.

2. If you save the project *after* running the capture,  you must save the project to
   the same directory that "default-data-dir" exists in. This is normally the folder
   from where you invoked the Python GUI.
   
3. There are a few warnings/exceptions that come up (i.e. divide-by-zero). Generally just
   keep going and see if things are still working. A number of those are on the *TODO*
   list to fix but I didn't get around to it yet.
   
4. The "Total Ops" window which checks for proper AES operation requires PyCrypto to be
   installed.
   
5. By default the XMEGA device was programmed with a partial AES implementation only. This
   is done to avoid any crypto export issues. This does not affect your side-channel analysis,
   but be aware the returned value might not appear to be correct (since only the first couple
   rounds of AES occurred).

Basic Usage Instructions
------------------------

 .. _cwliteprogramming:

CW-Lite: Programming AVR/XMEGA Device
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The CW1173/1180 has built-in support for programming either Atmel AVR or Atmel XMEGA device. This
is designed to allow you to program our target boards (either the built-in XMEGA target, or 
the Multi-Target board).

Note this programmer is fairly simple, and *does not* provide all the features of a
stand-alone programmer. 

AVR Programmer
""""""""""""""

The AVR device programmer requires four connections to the target: RESET, MOSI, MISO, SCK. See
:ref:`20pincwlite` for details of AVR programming pin connections.

Accessing the Programming
'''''''''''''''''''''''''
To access the AVR Programmer, select the "CW-Lite AVR Programmer" from the pull-down Tools menu:

.. image:: /images/cw1173/avrprog_menu.png

Which should give you the AVR Programmer Window.

Clock Source Selection
''''''''''''''''''''''
Note to use the AVR programmer you may require a valid clock source for the AVR. It is suggested to select one
of the setup scripts (such as ``ChipWhisperer-Lite: AES Simple-Serial on ATMega328P``) which will
generate a 7.37 MHz clock.

Check if the device is found by pressing the "Check Signature" button. The status window will show
the detected device based on the signature.

.. image:: /images/cw1173/avrprog_sigok.png

If this fails, double-check connections, and ensure the
clock source to the AVR is suitable. Note some errors will appear as part of the main window log:

.. image:: /images/cw1173/avrprog_fail.png

The default SPI data rate for the programmer is too fast for devices which are running slower
than 2 MHz. If programming a device with a clock source slower than 2 MHz, you will need to
enable the "Slow Clock Mode". In "Slow Clock Mode" the entire SAM3U and FPGA clock is changed from
96 MHz to 12 MHz. Note the default fuse bytes for a virgin ATMega328P result in a 1 MHz clock,
so you will need to use "slow clock mode" to program the correct fuse bytes, after which point
you will not need to use "slow clock mode".

.. note::

  The 'slow clock mode' is used to provide a slower SPI clock than would otherwise be possible. When
  switching into 'slow clock mode' it will cause all DCM blocks in the FPGA to become unlocked. You
  will need to reset the DCM blocks, or simply restart the CW-Capture software and run the setup script.


Programming the Fuses
'''''''''''''''''''''

By default the AVR programmer allows you to modify the LOW fuse byte only, as this byte controls the 
clock source selection. To change the value of the fuse byte:

1. Press the "Read Fuses" button, and the values should be populated
2. Specify the new low fuse value
3. Hit "Write Fuses"

See `an Online Fuse Calculator <http://eleccelerator.com/fusecalc/fusecalc.php?chip=atmega328p>`_ to
better understand what the values mean.

.. tip::
    If programming a virgin ATMega328P device, the default low-fuse value of ``62`` results in the internal
    8 MHz oscillator being divided down to 1 MHz. Any external clock is ignored.

    The low fuse byte must be changed to ``D0`` to use the external clock provided by the ChipWhisperer
    toolchain.
 
Programming the Flash
'''''''''''''''''''''

Programming the flash is accomplished by selecting the new .hex file in the "Find" menu, and pressing
the "Erase/Program/Verify FLASH" button. The "Status" line will show the following information:

* File programmed into device
* Time file was last modified (very useful to confirm you are using changed file when doing development)
* Status of verification, and number of bytes programmed/verified

.. image:: /images/cw1173/avrprog_progok.png

XMEGA Programmer
""""""""""""""""

The XMEGA device programmer requires only two connections to the target: clock (PDIC) and data (PDID).
The PDIC line is usually shared with the RESET pin, and the PDID pin is a specific pin on the XMEGA
device. See :ref:`20pincwlite` for details of XMEGA programming pin connections.

.. image:: /images/cw1173/xmegaprog_main.png


Using Glitch Port
^^^^^^^^^^^^^^^^^

The "GLITCH" port is used for voltage glitching. It's connected to two MOSFET elements, as the following
figure shows:

<TODO>

The CW1173/1180 can be commanded to turn on either of those MOSFETs via the "Glitch Output":

<TODO>

Be careful using this feature, as you don't want to short the MOSFETs for too long. It's also possible
to damage the ChipWhisperer-Lite by burning these MOSFETs up if used incorrectly. See tutorial TODO for
more information on using this feature.

Using Measure Port
^^^^^^^^^^^^^^^^^^

The "MEASURE" port is the input to the low-noise amplifier and ADC.

.. _20pincwlite:

20-Pin Connector
^^^^^^^^^^^^^^^^

The pinout is as follows:

============   =============   ====   ==================================================================
Number          Name           Dir     Description
============   =============   ====   ==================================================================
1                +VUSB (5V)      O     Not Connected on ChipWhisperer-Lite.
2                GND             O     System GND.
3                +3.3V           O     +3.3V to Target Device, can be turned off, 200mA available.
4                FPGA-HS1       I/O    High Speed Input (normally clock in).
5                PROG-RESET     I/O    Target RESET Pin (AVR Programmer).
6                FPGA-HS2       I/O    High Speed Output (normally clock or glitch out).
7                PROG-MISO      I/O    SPI input: MISO (for SPI + AVR Programmer).
8                VTarget         I     Drive this pin with desired I/O voltage in range 1.5V-5V.
9                PROG-MOSI      I/O    SPI output: MOSI (for SPI + AVR Programmer).
10               FPGA-TARG1     I/O    TargetIO Pin 1 - Usually UART TX or RX.
11               PROG-SCK       I/O    SPI output: SCK (for SPI + AVR Programmer).
12               FPGA-TARG2     I/O    TargetIO Pin 2 - Usually UART RX or TX.
13               PROG-PDIC      I/O    PDI Programming Clock (XMEGA Programmer), or CS pin (SPI).
14               FPGA-TARG3     I/O    TargetIO Pin 3 - Usually bidirectional IO for smartcard.
15               PROG-PDID      I/O    PDI Programming Data (XMEGA Programmer).
16               FPGA-TARG4     I/O    TargetIO Pin 4 - Usually trigger input.
17               GND            O
18               +3.3V          O
19               GND            O
20               +VUSB (5V)     O      Not Connected on ChipWhisperer-Lite.
============   =============   ====   ==================================================================

.. warning:

   All IO lines of the CW1173 (ChipWhisperer-Lite bare board) are 3.3V maximum. There is NO IO protection present
   on these pins, so connect them to external devices with *extreme care*.
   
   The CW1180 contains IO protection to avoid damage, but is also designed for 3.3V IO lines. No voltage
   translation is present on either product to ensure maximum flexability of IO drive characteristics
   from the internal FPGA.


8-Pin SmartCard  Connector
^^^^^^^^^^^^^^^^^^^^^^^^^^

The CW1173 contains two 8-pin connectors, which use our standard 8-pin Smart-Card header pinout. One header
connects to the SAM3U device (which has ISO-7816 drivers), one header connects to the FPGA. Note there is
currently no firmware support for these devices, but the hardware is designed for any of the following:

 * Emulating a smart card (use interposer board), or fuzzing a smart card reader
 * Communicating to a smart card
 * Sniffing traffic between a legitimate reader and smart card
 * Side-channel analysis of smart card device

.. warning:

   The Smart-Card lines on the CW1173 have no IO protection. Be extremely careful using these lines - many
   smart-cards are designed to run at 5.0V, or if interfacing to an external reader it will attempt to use
   5.0V at some stage.
   
   The CW1180 contains voltage translation that allows voltages of up to 5.5V to be safely input to the 8-pin
   connector. See the CW1180 product page for more details.

Header J7 (Connects to SAM3U):

============   =============   ====   ==================================================================
Number          Name           Dir     Description
============   =============   ====   ==================================================================
1                VCCIO           O     3.3V Supply (from linear regulator, always on)
2                GND             O     System GND
3                RST            I/O    Reset (SAM3U: PA3)
4                PRESENT         I     Used to detect presence of smart card (SAM3U: PA2)
5                CLK            I/O    Clock (SAM3U: PA25, 'CLK2'. FPGA: P131)
6                I/O            I/O    I/O Line (SAM3U: PA22), 10k pull-up
7                AUX1           I/O    Spare line (SAM3U: PA4)
8                AUX2           I/O    Spare line (SAM3U: PA5)
============   =============   ====   ==================================================================

Header J6 (Connects to FPGA):

============   =============   ====   ==================================================================
Number          Name           Dir     Description
============   =============   ====   ==================================================================
1                VCCIO           O     3.3V Supply (from FPGA supply)
2                GND             O     System GND
3                RST            I/O    Reset (FPGA: P102)
4                PRESENT/VPP     I     Not Connected (mount R60 to connect to P101)     
5                CLK            I/O    Clock (FPGA: P100)
6                I/O            I/O    I/O Line (FPGA: P99), 10k pull-up
7                AUX1           I/O    Spare line (FPGA: P98)
8                AUX2           I/O    Spare line (FPGA: P97)
============   =============   ====   ==================================================================

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

You can see a `Video <http://www.youtube.com/watch?v=8sIrvG5jqiQ&hd=1>`__ of the process here:

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




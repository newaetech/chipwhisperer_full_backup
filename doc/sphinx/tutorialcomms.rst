.. _tutorialcomms:

Tutorial #B1: Building a SimpleSerial Project
=============================================

This tutorial will introduce you to the 'simpleserial' communications system. It will show you how to
perform different operations on data based on input from the ChipWhisperer software. This can be used
for building your own system which you wish to 'break'.

What is SimpleSerial
--------------------

SimpleSerial is the communications protocol used for almost all of the ChipWhisperer demo project. It's a very
basic serial protocol which can be easily implemented on most systems. This system communicates using a standard
asyncronous serial protocol, 38400 baud, 8-N-1.

All messages are sent in ASCII-text, and are normally terminated with a line-feed ('\\n'). This allows you to
interact with the simpleserial system over a standard terminal emulator.

The following message types are defined:

``x``
    Sending a 'x' resets the buffers. This does not require a line-feed termination. It is suggested
    to always send a stream of x's to initilize the system in case the device was already in some other
    mode due to noise/corruption.
    
``k00112233445566778899AABBCCDDEEFF\\n``
    Loads the encryption key ``00112233445566778899AABBCCDDEEFF`` into the system. If not called the system
    may use some default key.
    
``pAABBCCDDEEFF00112233445566778899\\n``
    Encrypts the data ``AABBCCDDEEFF00112233445566778899`` with the key loaded with the 'k' command. The system
    will respond with a string starting with r, as shown next.
    
``rCBBD4A2B34F2571758FF6A797E09859D\\n``
    This is the response from the system. If data has been encrypted with a 'p' for example, the system will
    respond with the 'r' sequence automatically. So sending the earlier example means the result of the encryption
    was ``cbbd4a2b34f2571758ff6a797e09859d``.

Building the Basic Example
--------------------------

You'll need to have installed avr-gcc and avr-libc. You may have already done this by following the installation guide,
or if using the ChipWhisperer-VM it comes prepared with avr-gcc already setup. See the :ref:`installing` guide for details.

Once you have a working compiler (check by typing 'avr-gcc' at the command line - if using Windows you may need to setup a
special batch file to provide you with a avr-gcc command prompt).

1. Copy the directory ``simpleserial-base`` which is found at ``chipwhisperer\hardware\victims\firmware\`` of the 
   chipwhisperer release to a new directory called ``simpleserial-base-lab1``. You must keep it in the same directory, as
   it will reference other files within that directory for the build process.
 
2. Open a terminal with avr-gcc in the path. If using Windows the sidebar on the :ref:`installing` page - you can either
    add WinAVR to your system path, or you can run the 'winavr.bat' file suggested.
  
3. Change the terminal to the newly copied directory. For example::

    cd c:\chipwhisperer\hardware\victims\firmware\simpleserial-base-lab1
    
4. Then run ``make`` to build the system::

    make
    
   Which should have the following output::

    ...Bunch of lines removed...
    Creating Extended Listing: simpleserial-base.lss
    avr-objdump -h -S -z simpleserial-base.elf > simpleserial-base.lss
    
    Creating Symbol Table: simpleserial-base.sym
    avr-nm -n simpleserial-base.elf > simpleserial-base.sym
    
    Size after:
    AVR Memory Usage
    ----------------
    Device: atxmega128d3
    
    Program:    1524 bytes (1.1% Full)
    (.text + .data + .bootloader)
    
    Data:        224 bytes (2.7% Full)
    (.data + .bss + .noinit)
    
    
    Built for platform CW-Lite XMEGA
    
    -------- end --------

5. Pay special attention to the "Build platform" in that output. Ensure this matches your desired target - if not,
   open the ``makefile`` located in the same directory you ran ``make`` from. If using Windows you will need a code
   editor - if you installed WinAVR, it would have installed *Programmer's Notepad* which is an excellent choice.
   Scrolling down in the makefile you should see the following lines::
      
    ######### Specify your platform here

    #Multi-Target Board, AVR Device (ATMega328P)
    #PLATFORM = CW301_AVR
    
    #Multi-Target Board, XMEGA Device
    #PLATFORM = CW301_XMEGA
    
    #CW-Lite XMEGA Target Device (XMEGA128D4)
    PLATFORM = CW303
    
    #NOTDUINO Kit (ATMega328P)
    #PLATFORM = CW304

   Adjust the target platform by uncommenting (comments are lines starting with ``#``) the appropriate platform, and
   commenting out the other choices.
   
6. Run ``make`` again, and ensure the correct plaform is now printed.
   
    
Modifying the Basic Example
---------------------------

At this point we want to modify the system to perform 'something' with the data, such that we can confirm the system is
working. To do so, open the file ``simpleserial-base.c`` with a code editor such as *Programmer's Notepad* (which ships
with WinAVR). 

1. Find the following code block towards the end of the file:

   .. code-block:: c

    /**********************************
     * Start user-specific code here. */
    trigger_high();

    //16 hex bytes held in 'pt' were sent
    //from the computer. Store your response
    //back into 'pt', which will send 16 bytes
    //back to computer. Can ignore of course if
    //not needed

    trigger_low();
    /* End user-specific code here. *
     ********************************/

2. Modify it to increment the value of each sent data byte:

   .. code-block:: c

    /**********************************
     * Start user-specific code here. */
    trigger_high();

    //16 hex bytes held in 'pt' were sent
    //from the computer. Store your response
    //back into 'pt', which will send 16 bytes
    //back to computer. Can ignore of course if
    //not needed
    
    for(int i = 0; i < 16; i++){
        pt[i]++;
    }

    trigger_low();
    /* End user-specific code here. *
     ********************************/

3. Rebuild the example using the ``make`` command. Remember you can press the up arrow on the
   keyboard to get recently typed commands in most OSes.
      
.. _tutorialcommcw1002:
      
Completing Tutorial with CW1002 (ChipWhisperer Capture Rev2)
------------------------------------------------------------
      
Note the following is only applicable to the use of the ChipWhisperer-Capture Rev 2 hardware (aka the CW1002).
See :ref:`tutorialcommcw1173` if you are not using this hardware.
      
.. include:: tutorialbasic_setup.rsti
      
Programming the Example
^^^^^^^^^^^^^^^^^^^^^^^

1.  We assume the hardware is already connected as at the beginning of this tutorial. If not go back & confirm this hardware setup.

2.  Next, you will need to program the AVR itself. On Windows we will make use of the free
    `Atmel Studio 4.19 <www.atmel.com/tools/STUDIOARCHIVE.aspx>`_. You can find a direct link here:
    `Direct Link to Atmel Studio 4.19 Binary <http://www.atmel.com/System/BaseForm.aspx?target=tcm:26-17924>`_
    which will require you to enter an email address to receive the download link.  Note it is possible to use
    ``avrdude``, a command-line program which is part of WinAVR instead if you wish. However since many people
    find the graphical interface of AVRStudio easier, this guide will use AVRStudio. Be sure to install the USB
    drivers as part of the package.

3.  Plug in the USB-A Connector on the rear side of the ChipWhisperer Rev2. This should trigger the driver installation, which
    will detect the device as a *AVR-ISP MK2*. You can leave the USB-Mini cable plugged in during this operation, or unplug the
    USB-Mini cable from the ChipWhisperer & plug in the USB-A cable.

4.  Once AVR Studio is installed, open the main window. From the toolbar select either the *Con* or *AVR* icon, and select the *AVR-ISP MK-II* Device:
    
    .. image:: /images/studio4-connect.png

5.  In the window that opens, select the *Main* tab. Select the device type as *AtMega328P*, and hit *Read Signature*. You should get an indication that the device signature was
    successfully read!

    .. image:: /images/studio4-read-signature.png
    
6.  Finally we can program the chip. To do so switch to the *Program* tab, select the ``simpleserial_nocrypto.hex`` file that was generated in Step 4, and hit *Program*. If it's successful
    you should see some output data saying so.
    
    .. image:: /images/studio4-program.png

.. warning::

    Be sure to select the correct .hex file! Otherwise the system won't work & it can be frusturating to troubleshoot.
    It's a good idea to look at the 'last modified' date which shows up when you go to select the file. Check that it
    roughly corresponds to when you compiled the file.    
    
That's it! You've now built a custom application & programmed it into the AVR. We haven't yet verified it's working, which is the next step.
 
Communicating from CW-Capture Software
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ 

Next, open the CW-Capture software. Then perform the following steps:

.. image:: /images/tutorials/basic/comm/genericsetup.png

1. Switch to the *General Settings* tab

2. As the *Scope Module*, select the *ChipWhisperer/OpenADC* option

3. As the *Target Module*, select the *Simple Serial* option

Next, you'll have to configure the target module:

.. image:: /images/tutorials/basic/comm/targetsetup.png

4. Switch to the *Target Settings* tab

5. As the *connection*, select the *ChipWhisperer* option

Now, download the FPGA Firmware:

.. image:: /images/tutorials/basic/comm/downloadfpga.png

6. Optional: Run the *Download CW Firmware* tool. You should have configured this already before. Note that from release 0.09
   of ChipWhisperer the FPGA is automatically programmed when you attempt to connect, so you can skip this step if using a recent
   release (i.e. any release in 2015 or later).

7. If you switch to the *Debug Logging* output, you should see an indication the FPGA was programmed. If
   you were already using the device, it will skip the download. Normally you can skip steps 6 & 7 if you've
   already performed the FPGA download once since powering on the device. Note that 
   
.. image:: /images/tutorials/basic/comm/connect.png

8. Press the button labeled *Master: DIS*, where DIS has a circle around it. If it works, it will switch
   to green and say *CON*.
   
.. image:: /images/tutorials/basic/comm/connect_run.png

9. Switch to the *General Settings* tab.

10. Hit the *Open Monitor* button.

11. Resize the monitor window. The monitor will show sent & received data to the target.

12. Hit the *Run 1* button. You may have to hit it a few times, as the very first serial data is often lost. You should see
    data populate in the *Text Out* field of the monitor window. Note that each byte of the *Text In* is incremented in the
    *Text Out* field.

.. _tutorialcommcw1173:

Completing Tutorial with CW1173 (ChipWhisperer-Lite Board)
----------------------------------------------------------

Note the following is only applicable to the use of the ChipWhisperer-Lite hardware (aka the CW1173). See :ref:`tutorialcomcw1002`
if you are not using this hardware.

Setting up the Hardware
^^^^^^^^^^^^^^^^^^^^^^^

This tutorial uses the :ref:`naecw1173_cwlite` hardware. No hardware setup is required normally, simply plug in the micro-USB cable:

.. image:: /images/cw1173/regularusb.jpg

Note that under *no circumstances* as part of the setup should you use the CW1173 device to hold up furniture:

.. image:: /images/cw1173/nofurniture.jpg

Setting up the Software
^^^^^^^^^^^^^^^^^^^^^^^

It is assumed that you've already followed the guide in :ref:`installing`. Thus it is assumed you are able to communicate with the
ChipWhisperer CW1173 hardware (or whatever capture hardware you are using). Note in particular you must have configured the FPGA
bitstream in the ChipWhisperer-Capture software, all part of the description in the :ref:`installing` guide.

Assuming this setup is complete, you can confirm you are able to communicate with the hardware by running the example capture of traces
given in the :ref:`naecw1173_cwlite` quick-start.

Programming the Example
^^^^^^^^^^^^^^^^^^^^^^^

Note with the CW1173 you need to configure a clock before programming of the device will succeed. Programming of the target device
will be done as part of the CW-Capture software setup, discussed next.

Communicating from CW-Capture Software
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ 

Next, open the CW-Capture software. Then perform the following steps:

.. image:: /images/tutorials/basic/comm/genericsetup.png

1. Switch to the *General Settings* tab.

2. As the *Scope Module*, select the *ChipWhisperer/OpenADC* option.

3. As the *Target Module*, select the *Simple Serial* option.

Next, you'll have to configure the scope device in use.

.. image:: /images/tutorials/basic/comm/cw1173_scopesetup.png

4. Switch to the *Scope Settings* tab.

5. As the *connection*, select the *ChipWhisperer Lite* protocol.

Next, you'll have to configure the target module:

.. image:: /images/tutorials/basic/comm/cw1173_targetsetup.png

6. Switch to the *Target Settings* tab

7. As the *connection*, select the *ChipWhisperer-Lite* option

.. image:: /images/tutorials/basic/comm/connect.png

8. Press the button labeled *Master: DIS*, where DIS has a circle around it. If it works, it will switch
   to green and say *CON*.  

We will now need to configure the clock to the XMEGA target device.

.. image:: /images/tutorials/basic/comm/cw1173_freqoutput1.png

9. Switch to the *Scope Settings* tab.

10. Scroll down to the CLKGEN section, and enter the *Desired Frequency* as *7.37 MHz*. Hit ``<enter>`` and you should
    see the multiply and divide settings change. Be careful of the units - you may need to click just on the numbers and
    enter that. You can also check the frequency settings using the *Frequency Counter* - to use this switch the
    *Freq Counter Src* to be *CLKGEN*. It should read 7.37 MHz.
    
    We now need to feed this generated clock to an output pin.
    
.. image:: /images/tutorials/basic/comm/cw1173_freqoutput2.png

11. Scroll further down until you find the *Target HS IO-Out* function, set this to *CLKGEN*. The clock is now being
    sent on the *Target-IO HS2* pin, which is used by the XMEGA as the input clock.
        
    For dumb historical reasons the XMEGA and AVR have opposite TX/RX pinouts (on the original multi-target
    board, the AVR and XMEGA are designed such they can communicate, which required opposite TX/RX connections).
    Anyway you now need to switch the IO1 and IO2 functions from default: 
    
.. image:: /images/tutorials/basic/comm/cw1173_txrx.png
    
12. Change the *Target IO1* function to be *RXD*.

13. Change the *Target IO2* function to be *TXD*.

14. You can now program the XMEGA device! To do so, open the XMEGA Programmer from the *Tools* menu:

    .. image:: /images/tutorials/basic/comm/cw1173_xmegaproga.png
    
15. Hit the *Check Signature* button and confirm the device is detected. If not you may have issues with the clock setup.

    .. image:: /images/tutorials/basic/comm/cw1173_xmegaprogb.png
    
16. Using the *Find* button, navigate to the ``simpleserial-base.hex`` which you built earlier with the ``make`` command.
    You can then press the *Erase/Program/Verify* button, and confirm the file is programmed into the XMEGA device:
    
    .. image:: /images/tutorials/basic/comm/cw1173_xmegaprogc.png
    
    Note the programmer dialog not only shows the successful programming status, but also shows when the ``.hex`` file was
    last modified. Always confirm this matches with when you last remember compiling the program -- if it is widely different
    this suggests you have selected the wrong file!
    
    Finally we can check communications with the programmed file:

.. image:: /images/tutorials/basic/comm/cw1173_connectrun.png

17. Close the XMEGA programmer dialog, and switch to the *General Settings* tab.

18. Hit the *Open Monitor* button.

19. Resize the monitor window. The monitor will show sent & received data to the target.

20. Hit the *Run 1* button. You may have to hit it a few times, as the very first serial data is often lost. You should see
    data populate in the *Text Out* field of the monitor window. Note that each byte of the *Text In* is incremented in the
    *Text Out* field.

Conclusion
----------

In this tutorial you have learned how to build a custom program for the microcontroller on the ChipWhisperer target board.
You have programmed the built .hex file into the microcontroller, and confirmed communications with the ChipWhisperer device.

In future labs you will build on this knowledge to attack specific instructions.

Troubleshooting
---------------

Issues with compilation:

  1. You may have to generate the `.dep` and `objdir` directories manually before `make` will work::
  
       mkdir .dep
       mkdir objdir
	   
  2. On Windows 8, you may get an error like `fork: resource temporarily unavailable`. This requires you to install an updated mysys.dll.
     Download from http://www.madwizard.org/download/electronics/msys-1.0-vista64.zip, unzip file, and copy the .dll to
     ``C:\WinAVR-20100110\utils\bin``, replacing the existing file.
	 
  3. For the AVR Studio USB Drivers, you'll need to download a `Special Update <https://gallery.atmel.com/Products/Details/004ccabd-e18e-431a-8557-83deaea23341>`_
     from Atmel.
     
  4. You may wish to use the "ChipWhisperer Virtual Machine" on newer Windows systems, which does not require any of the above setup.


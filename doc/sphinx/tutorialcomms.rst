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

x
    Sending a 'x' resets the buffers. This does not require a line-feed termination. It is suggested
    to always send a stream of x's to initilize the system in case the device was already in some other
    mode due to noise/corruption.
    
k00112233445566778899AABBCCDDEEFF\\n
    Loads the encryption key ``00112233445566778899AABBCCDDEEFF`` into the system. If not called the system
    may use some default key.
    
pAABBCCDDEEFF00112233445566778899\\n
    Encrypts the data ``AABBCCDDEEFF00112233445566778899`` with the key loaded with the 'k' command. The system
    will respond with a string starting with r, as shown next.
    
rCBBD4A2B34F2571758FF6A797E09859D\\n
    This is the response from the system. If data has been encrypted with a 'p' for example, the system will
    respond with the 'r' sequence automatically. So sending the earlier example means the result of the encryption
    was ``cbbd4a2b34f2571758ff6a797e09859d``.

Building the Basic Example
--------------------------

You'll need to have installed WinAVR. You may have already done this by following the installation guide, if not perform the following step:

.. include:: installing_winavr.rsti

2. Copy the directory ``avr-serial-nocrypto`` which is found at ``chipwhisperer\hardware\victims\firmware\`` of the 
   chipwhisperer release somewhere. The following examples assume it has been copied to ``c:\chipwhisperer\user\comm\avr-serial-nocrypto\``.
 
3. Open a terminal with WinAVR in the path. See the sidebar - you can either add WinAVR to
   your system path, or you can run the 'winavr.bat' file suggested.
  
4. Change the terminal to the newly copied directory. For example::

    cd c:\chipwhisperer\user\comm\avr-serial-nocrypto
    
5. Then run ``make MCU=atmega328p`` to build the system::

    make MCU=atmega328p
    
   Which should have the following output::

    ...Bunch of lines removed...
    Creating Extended Listing: simpleserial_nocrypt.lss
    avr-objdump -h -S -z simpleserial_nocrypt.elf > simpleserial_nocrypt.lss

    Creating Symbol Table: simpleserial_nocrypt.sym
    avr-nm -n simpleserial_nocrypt.elf > simpleserial_nocrypt.sym

    Size after:
    AVR Memory Usage
    ----------------
    Device: atmega328p

    Program:     716 bytes (2.2% Full)
    (.text + .data + .bootloader)

    Data:        112 bytes (5.5% Full)
    (.data + .bss + .noinit)

Modifying the Basic Example
---------------------------

At this point we want to modify the system to perform 'something' with the data, such that we can confirm the system is
working. To do so, open the file ``c:\chipwhisperer\user\comm\avr-serial-nocrypto\simpleserial_nocrypt.c`` with a text
editor such as Programmer's Notepad (which ships with WinAVR). 

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

3. Rebuild the example using the ``make MCU=atmega328p`` command. Remember you can press the up arrow on the
   keyboard to get recently typed commands in most OSes.
      
.. include:: tutorialbasic_setup.rsti
      
Programming the Example
-----------------------

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
---------------------------------------- 

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

6. Run the *Download CW Firmware* tool. You should have configured this already before.

7. If you switch to the *Debug Logging* output, you should see an indication the FPGA was programmed. If
   you were already using the device, it will skip the download. Normally you can skip steps 6 & 7 if you've
   already performed the FPGA download once since powering on the device.
   
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

Conclusion
----------

In this tutorial you have learned how to build a custom program for the AtMega328p microcontroller on the Multi-Target board.
You have programmed the built .hex file into the microcontroller, and confirmed communications with the ChipWhisperer device.

In future labs you will build on this knowledge.

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


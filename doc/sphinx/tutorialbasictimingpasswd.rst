.. _tutorialbasictimingpasswd:

Tutorial #B3-1: Timing Analysis with Power for Password Bypass
==============================================================

This tutorial will introduce you to breaking devices by determining when a device is performing certain operations. It will use
a simple password check, and demonstrate how to perform a basic power analysis.

In addition this example shows you how to drive the ChipWhisperer software with a script, rather than using the GUI. This will
be required when attacking new devices which you have not yet added to the core ChipWhisperer software.

Note this is not a prerequisite to the tutorial on breaking AES. You can skip this tutorial if you wish to go ahead with the AES
tutorial.

You can also view a 53-min `Video Version on YouTube <https://www.youtube.com/watch?v=h4eAU6vEONs&hd=1>`_:

|YouTubeB2.1|_

.. |YouTubeB2.1| image:: /images/tutorials/basic/timingpowerbasic/timingvideo.png
.. _YouTubeB2.1: https://www.youtube.com/watch?v=h4eAU6vEONs&hd=1

https://www.youtube.com/watch?v=h4eAU6vEONs&hd=1

Prerequisites
-------------

You should have already completed :ref:`tutorialtimingsimple` to gain a better understanding of the ChipWhisperer interface.

Building the Target Firmware
----------------------------

The target firmware is located in the directory ``chipwhisperer\hardware\victims\firmware\basic-passwdcheck``. Build
the firmware using ``make``, once again being careful to ensure you have modified the ``makefile`` to select the 
correct target. You should end up with something like this being printed::

    Creating Symbol Table: basic-passwdcheck.sym
    avr-nm -n basic-passwdcheck.elf > basic-passwdcheck.sym
    
    Size after:
    AVR Memory Usage
    ----------------
    Device: atxmega128d3
    
    Program:    5400 bytes (3.9% Full)
    (.text + .data + .bootloader)
    
    Data:        524 bytes (6.4% Full)
    (.data + .bss + .noinit)
    
    
    Built for platform CW-Lite XMEGA
    
    -------- end --------

Basic Communications with the Target
------------------------------------

At this point, you should be able to configure the target as in the `tutorialtimingsimple` or `tutorialcomms`. Rather than tediously going
through the setup process again, we'll simply use one of the scripts built into the ChipWhisperer-Capture software. This will demonstrate
how we can use a script as a starting point to simplify our setup.

1. Connect your target hardware (ChipWhisperer-Lite or ChipWhisperer-Capture Rev 2 with target board).

2. Open the ChipWhisperer-Capture software.

3. From the *Example Scripts*, select one which most closely matches your hardware. For example here I'm using a ChipWhisperer-Lite with the
   XMEGA target, so will select that script. Note I'm *NOT* attacking AES, so will need to make some adjustments later.
    .. image:: /images/tutorials/basic/timingpowerbasic/scriptexample.png
    
4. The system should connect to your hardware. Remember you have not yet reprogrammed the target so won't be communicating with the target
   program.
   
5. Using the programming tool (such as XMEGA programming dialog), program the file ``basic-passwdcheck.hex`` into the target device. This file
   is located where you ran ``make`` previously.
   
6. Select *Tools --> Open Terminal*, and press *Connect*. You should see a window such as this:

    .. image:: /images/tutorials/basic/timingpowerbasic/termconn.png

7. At this point we need to reset the target device. The easiest way to do this is use the programmer interface, and press
   the *Check Signature* or *Read Signature* button. This will reset the target device as part of the signature read operation.
   You should see some messages come across the terminal emulator window:
   
    .. image:: /images/tutorials/basic/timingpowerbasic/checksig_print.png
   
   Note a few warnings about the terminal emulator:
    * The on-board buffer is fairly small, and can be easily overflowed. You may notice a few longer lines become trunicated if printing is too fast!
    * You can uncheck the "Show non-ASCII as hex" to avoid having the ``0a`` printed in red. The ``0a`` is the hex character for a newline. Many
      protocols use non-ASCII characters, so to help with debugging it is left enabled by default.
      
8. We've now got some super-secure system! Let's begin with some exploratory tests - in this case I happened to know the correct password is ``h0px3``.

   .. tip::

      In real systems, you may often know *one* of the passwords, which is sufficient to investigate the password checking routines as we will do. You
      also normally have an ability to reset passwords to default. While the reset procedure would erase any data you care about, the attacker will
      be able to use this 'sacrificial' device to learn about possible vulnerabilites. So the assumption that we have access to the password is really
      just saying we have access to *a* password, and will use that knowledge to break the system in general.
      
9. Using the terminal emulator, write the correct password in, and press ``<enter>``. You should be greeted by a welcome message, and if using the
   CW-Lite XMEGA target the green LED will illuminate:
   
   .. image:: /images/tutorials/basic/timingpowerbasic/passok.png

10. The system enters an infinite loop for any password entry. Thus you must reset the system, use the *Programmer Window* to again perform a *Check Signature*
    or *Read Signature* operation.
    
11. Enter an incorrect password - notice a different message is printed, and if using the CW-Lite XMEGA target the red LED will come on.

12. At this point, we'll make some changes to the trigger setup of the ChipWhisperer. In particular, ensure you set the following:

     * Offset = 0
     * Timeout set to 5 seconds or greater (to give yourself time when manually testing)

     .. image:: /images/tutorials/basic/timingpowerbasic/timeout_offset.png

13. Change to the *Target Settings* tab, and delete the *Command* strings. Those strings are used in the AES attack to send a specific command
    to the target device, for now we will be manually sending data:

     .. image:: /images/tutorials/basic/timingpowerbasic/text_targetsettings.png

14. Perform the following actions:

     i) Reset the target device (e.g. by performing the signature check).
     ii) Enter the password ``h0px3`` in the terminal window, but *do not* yet hit enter.
     iii) Press the *Capture 1* button, and immediately switch to the terminal emulator window and press ``<enter>`` to send the password.
    
    You must send the password before the timeout occurs -- you can increase the length of the timeout if needed to give yourself more time! If
    this works you should see the power consumption displayed in the GUI:
    
     .. image:: /images/tutorials/basic/timingpowerbasic/trace_manual_pass.png
  
15. Rather than using the manual terminal, let's now use the GUI to automatically send a password try. Switching back to the *Target Settings* tab,
    write ``h0px3\n`` into the *Go Command* option:
    
     .. image:: /images/tutorials/basic/timingpowerbasic/gocorrect.png

    The *Go Command* is sent right after the scope is armed. In this example it means we can capture the power consumption during the password
    entry phase.
   
16. Now perform the following actions:
   
     i) Reset the target device (e.g. by performing the signature check).
     ii) Press the *Capture 1* button.
    
    Hopefully this resulted in the same waveform as before! Note the device takes around 1 second to 'boot', so if you are too lightning
    fast after resetting the device it won't actually be ready to accept the password. You can keep the terminal emulator window open to
    view the output data.
    
17. Play around with the password entered on the *Go Command* - try all of the following:

    * ``h0px3\n``
    * ``h0px4\n``
    * ``h0paa\n``
    * ``haaaa\n``
    * ``a\n``

    You should notice a distinct change in the password depending how many characters were correct. For example the following shows the
    difference between passwords of ``h0px4`` (which has 4 correct characters) and ``h0paa`` (which has 3 correct characters):
   
     .. image:: /images/tutorials/basic/timingpowerbasic/3vs4.png
    
18. The last step before scripting an entire attack is to figre out how to automatically reset the target device before (or after) each
    capture. There are two ways to do this, and the following steps take you through two examples of how to accomplish this goal.
    
Reset via Spare IO Lines
^^^^^^^^^^^^^^^^^^^^^^^^

TODO - see reset via programming interface for now

Reset via Programming Interface
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The following example targets the ChipWhisperer-Lite XMEGA target. You can modify it for ChipWhisperer-Lite AVR target by replacing
``XMEGA`` with ``AVR`` in the function calls. We'll first learn how to recreate the process of pressing the *Check Signature* button
via the API. This isn't normally exposed, but we can explore that using the Python Console.

1. Type ``self`` into the Python console, the output will look like this::

     >>> self
     <__main__.ChipWhispererCapture object at 0x072F25F8>

   This tells us that the class type of this opject is ``ChipWhispererCapture``. We can open the source code for that class, and determine
   where the "scope" is stored. This takes some effort the first time through, but eventually you would discover there is a ``self.scope``.
   
   Type ``self.scope`` into the Python console::

     >>> self.scope
     <chipwhisperer.capture.scopes.OpenADC.OpenADCInterface object at 0x0D4986C0>
    
   Opening the file ``chipwhisperer\capture\scopes\OpenADC.py`` would tell us that ``.scopetype`` is used to store the next level of the
   interface. We want to reach very far down to get to the AVR/XMEGA programmer interface, so will continue down the rabbit hole::
   
     >>> self.scope.scopetype   
     <chipwhisperer.capture.scopes.OpenADC.OpenADCInterface_NAEUSBChip object at 0x0D5BC4B8>
   
   Finally, checking the source for the ``OpenADCInterface_NAEUSBChip`` class in that same file gives us this source code::
   
        ...code...
        self.cwliteXMEGA = XMEGAProgrammerDialog(global_mod.main_window)

        self.xmegaProgramAct = QAction('CW-Lite XMEGA Programmer', self,
                                       statusTip='Open XMEGA Programmer (ChipWhisperer-Lite Only)',
                                       triggered=self.cwliteXMEGA.show)

        self.cwliteAVR = AVRProgrammerDialog(global_mod.main_window)
        ...code...
        
   We can finally try reaching out and touching the XMEGA or AVR programmer::
   
        >>> self.scope.scopetype.cwliteXMEGA
        <chipwhisperer.capture.utils.XMEGAProgrammer.XMEGAProgrammerDialog object at 0x0D5BC670>
        
   This allows us to touch the XMEGA programmer dialog directly. At this point you can refer to the file
   ``chipwhisperer\capture\utils\XMEGAProgrammer.py`` to confirm the class interface. Part of this will be
   the ``readSignature()`` function, which we can try running::
   
       >>> self.scope.scopetype.cwliteXMEGA.readSignature()
       
   Success! You should see the terminal emulator print the startup message, indicating the target was rebooted.
   
2. Now we need to understand how to force this to be called. This can be done via the *Auxiliary Modules*, which we used
   in the previous part to toggle an IO line. Instead we will define one through the command prompt, before finally using
   it in a custom script.

   At the console, type the following to import some require modules (ignore the >>> which just indicate the console prompt)::

    >>> from time import sleep
    >>> from chipwhisperer.capture.auxiliary.template import AuxiliaryTemplate

3. We will now define a simple
   in the previous part to toggle an IO line. Instead we will define one through the command prompt, before finally using
   it in a custom script.

   At the console, type the following to import some require modules (ignore the >>> which just indicate the console prompt)::

    >>> from time import sleep
    >>> from chipwhisperer.capture.auxiliary.template import AuxiliaryTemplate

3. We will now define a simple
   in the previous part to toggle an IO line. Instead we will define one through the command prompt, before finally using
   it in a custom script.
   
   At the console, type the following to import some require modules (ignore the >>> which just indicate the console prompt)::
   
    >>> from time import sleep
    >>> from chipwhisperer.capture.auxiliary.AuxiliaryTemplate import AuxiliaryTemplate
    
3. We will now define a simple ``reset_device()`` function. You will do this interactively at the console, the objective being
   to enter the following chunk of code::
   
    def reset_device():
        self.scope.scopetype.cwliteXMEGA.readSignature()
        sleep(0.8)
        
   Remember Python is *whitespace sensitive*, so you'll have to be careful with indents in use. To being with, simply type ``def reset_device():``
   at the console and press enter. You'll notice the ``>>>`` changes to ``...`` at the console prompt:
   
     .. image:: /images/tutorials/basic/timingpowerbasic/consoledotdot.png
     
   Now you will enter the next two lines. Remember you must insert at least one space before each line, and it must be consistent between the
   two lines entered. Once you enter the last line, press enter and the ``...`` should change back to ``>>>``
   
     .. image:: /images/tutorials/basic/timingpowerbasic/consolespace.png
     
4. Check you can run ``reset_device()`` at the console and the device resets. If there is an error check you've run the import statements previously
   and for other typos. Re-run the ``def reset_device():`` step if required.
   
5. Now we need to define the class which links the function to a step in the capture. To do so, we want to define the following::

    class resetClass(AuxiliaryTemplate):
      def traceDone(self):
       reset_device()

   Pay very careful attention to the indentation -- when entering via the command line, we need to ensure `` def traceDone(self):`` has one
   level of indents, and ``  reset_device()`` has additional indent. You can use a single space if you want for one-level, and two spaces for
   two-levels for example::
   
    >>> class resetClass(AuxiliaryTemplate):
    ...  def traceDone(self):
    ...   reset_device()
    
6. Finally, generate an object using that class, and confirm it again resets the device::

    >>> rc = resetClass()
    >>> rc.traceDone()
   
7. Now all that is left is to link this class into the Auxiliary interface. This is done simply with the following call::

    >>> self.auxChanged(rc)
    
   ``auxChanged()`` is called with the new Auxiliary module to be loaded (or list of modules). The ``traceDone()`` method will be
   called once a single trace is done.
   
8. Confirm you can press *Capture 1* in the GUI without needing to manually reset the XMEGA target device. Play around with the password
   to again see the effect of changing password length. In particular, start to consider where you might look for an indicator about
   how far in the loop you can go? Play around with 0 correct digits, 1 correct digits, etc.
 

Scripting Communications
------------------------

1. Make a copy of the existing script. You can find it at ``chipwhisperer\software\chipwhisperer\capture\scripts``, for example the default
   one is called ``cwlite-simpleserialxmega.py`` for the XMEGA device. Copy this to another directory that you will use for the attack.
   
2. Rename the script something else - for example ``cwlite-passwordcrack.py``, and open it for editing. You'll notice the following is a
   main chunk of the code, where the parameters are set::
   
        #Example of using a list to set parameters. Slightly easier to copy/paste in this format
        lstexample = [['CW Extra', 'CW Extra Settings', 'Trigger Pins', 'Target IO4 (Trigger Line)', True],
                      ['CW Extra', 'CW Extra Settings', 'Target IOn Pins', 'Target IO1', 'Serial RXD'],
                      ['CW Extra', 'CW Extra Settings', 'Target IOn Pins', 'Target IO2', 'Serial TXD'],
                      ['OpenADC', 'Clock Setup', 'CLKGEN Settings', 'Desired Frequency', 7370000.0],
                      ['CW Extra', 'CW Extra Settings', 'Target HS IO-Out', 'CLKGEN'],
                      ['OpenADC', 'Clock Setup', 'ADC Clock', 'Source', 'CLKGEN x4 via DCM'],
                      ['OpenADC', 'Trigger Setup', 'Total Samples', 3000],
                      ['OpenADC', 'Trigger Setup', 'Offset', 1500],
                      ['OpenADC', 'Gain Setting', 'Setting', 45],
                      ['OpenADC', 'Trigger Setup', 'Mode', 'rising edge'],
                      #Final step: make DCMs relock in case they are lost
                      ['OpenADC', 'Clock Setup', 'ADC Clock', 'Reset ADC DCM', None],
                      ]

   Those parameters come from the *Scripting Parameters* tab. Switch over to it and notice how when you change the text for example, it
   tells you the required parameter name to do this via the API call:
   
     .. image:: /images/tutorials/basic/timingpowerbasic/scriptcommands.png
    
   Note that commands run via the script are also printed, so you can see where the values being set are coming from too. At this point
   close the *ChipWhisperer-Capture* window, as we will confirm the script still works.
   
3. Run the new script (which doesn't have any changes yet). You may have to open a console with Python in the path:
    
    i) If you installed WinPython, run the *WinPython Console* from your WinPython installation directory.
    ii) If using the VMWare image of a Linux machine, this should just be a regular console
    
  Run the script with ``python cwlite-passwordcrack.py``. If the script errors out, it might be that the location of the FPGA bitstream
  is stored in relative terms. To fix this perform the following:
  
   i) Open ChipWhisperer-Capture regularly.
   ii) Run the ChipWhisperer script that you used previously.
   iii) Select *Tools-->Config CW Firmware*
   iv) Under the "FPGA .zip (Release)", hit the "Find" button. Point the system to the file
       ``chipwhisperer/hardware/capture/chipwhisperer-lite/cwlite_firmware.zip`` on your filesystem. Note by default there is
       a relative path.
       
4. Once again on the *Target Settings* tab, delete the various commands. Note the resulting *Script Commands* which you will need to enter
   to achieve this same goal.
   
5. Close ChipWhisperer-Capture.

6. Edit the script, first find the line setting the Trigger Offset::

       ['OpenADC', 'Trigger Setup', 'Offset', 1500],

   And set this to 0, which we were using previously::
   
       ['OpenADC', 'Trigger Setup', 'Offset', 0],
       
7. Next, append the required commands to clear the simpleserial commands::

        #Example of using a list to set parameters. Slightly easier to copy/paste in this format
        lstexample = [['CW Extra', 'CW Extra Settings', 'Trigger Pins', 'Target IO4 (Trigger Line)', True],
                      ...BUNCH MORE COMMANDS HERE HAVE BEEN REMOVED...
                      #Final step: make DCMs relock in case they are lost
                      ['OpenADC', 'Clock Setup', 'ADC Clock', 'Reset ADC DCM', None],
                      
                      #Append your commands here
                      ['Target Connection', 'Load Key Command', u''],
                      ['Target Connection', 'Go Command', u''],
                      ['Target Connection', 'Output Format', u''],                      
                      ]

8. Next, we are going to "hack in" the Auxiliary module. While the following isn't great Python code, the idea is to 
   demonstrate how we can rapidly iterate with the combination of GUI to explore options, and the script to write them
   into place. First, add the imports to the start of the Python script::
   
        from time import sleep
        from chipwhisperer.capture.auxiliary.AuxiliaryTemplate import AuxiliaryTemplate
   
   Find the section of the file that sends the previous commands to the hardware. You will see a line like the following::
   
        #Download all hardware setup parameters
        for cmd in lstexample: cap.setParameter(cmd)

   We will then hack in the script we tested previously, which will insert our custom Auxiliary module::
   
        #Download all hardware setup parameters
        for cmd in lstexample: cap.setParameter(cmd)

        def reset_device():
            cap.scope.scopetype.cwliteXMEGA.readSignature()
            sleep(0.8)

        class resetClass(AuxiliaryTemplate):
          def traceDone(self):
           reset_device()

        rc = resetClass()
        cap.auxChanged(rc)

   Note we changed the references to "self" to "cap", as we are no longer running from within the Capture environment.
   Otherwise we have used the ability of Python to declare classes inside of functions to avoid needing to think about
   how to properly declare everything.

9. Finally, we will set the password. You can enter the password in the Capture *Target Settings* tab, and see the following
   sort of call would set the appropriate password::
   
    cap.setParameter(['Target Connection', 'Go Command', u'h0px3\\n'])
    
   Note the newline is actually escaped, to set the text equivalent of what will be printed. This will result in an actual
   newline going out across the serial port.
   
   Set that command at some point after your call to ``cap.auxChanged()``. Close any open ChipWhisperer-Capture windows, and
   run the script as before. You should connect to the target, and be able to press *Capture 1* and see the correct waveform.
   
10. Next, we will automatically start attacking the system. You needed to figure out where we will look to determine if the
    password check is working. Looking at an example of the power when 0 and 1 bytes are correct, we can see a good point
    that appears to shift forward in time:
    
     .. image:: /images/tutorials/basic/timingpowerbasic/passwordcrackerpts.png
   
    This point corresponds to an offset of 153 samples, and a delta for each character of 72 points. Note the specific point
    will change for different hardware, and may also change if you use different versions of avr-gcc to compile the target
    code. The example code here was compiled with WinAVR 20100110, which has avr-gcc 4.3.3. If you view the video version
    of this tutorial the point numbers are different for example, so be sure to check what they are for your specific system.
    
    Let's start with cracking just the first character, assuming it's a lowercase alphanumeric character::
    
        trylist = "abcdefghijklmnopqrstuvwyx0123456789"
        
        for c in trylist:
            cap.setParameter(['Target Connection', 'Go Command', u'%c\\n'%c])
            cap.capture1()
            
            #TODO: Check data to see if successful??
            print "Try = %c"%c
            
            #Call to pe() causes GUI to process outstanding events, useful if you are calling API directly
            pe()
   
11. We haven't yet pragmatically tested the results, but run the script anyway (to kill it, you'll have to use Ctrl-C on the terminal
    window). You should notice a distinct change of the power signature when it runs through "Try = h".
    
12. We can access ``cap.scope.datapoints`` to get the data points. Let's print that point of interest (again change
    the point for your specific setup)::
        
        for c in trylist:
            cap.setParameter(['Target Connection', 'Go Command', u'%c\\n'%c])
            cap.capture1()
            
            #TODO: Check data to see if successful??
            print "Try = %c"%c
            print cap.scope.datapoints[153]
            
            #Call to pe() causes GUI to process outstanding events, useful if you are calling API directly
            pe()
   
13. Running that example, you can see we can use a simple threshold to detect the correct password. Finally use the
    following paying careful attention that you first:
    
      * Check the offset and delta values (here they are 153 and 72)
      * Note that the "Go Command" has been modified to send the known password characters, otherwise
        it won't work in a progressive manner.
        
    The following is a sample code you can replace the previous with::

        password = ""

        for i in range(0,5):
            print "***CHARACTER %d***"%i
            for c in trylist:
                cap.setParameter(['Target Connection', 'Go Command', password + "%c\\n"%c])
                cap.capture1()
                
                print "Try = %c"%c
                #print cap.scope.datapoints[153 + i*72]
                if cap.scope.datapoints[153 + i*72] > -0.2:
                    print "****CHARACTER %d = %c****"%(i, c)
                    password += c
                    break
                
                elif c == "9":
                    print "****CHARACTER %d FAILED****"%(i)
                    password += "?"
                
                #Call to pe() causes GUI to process outstanding events, useful if you are calling API directly
                pe()    

        print password

That's it! You should have successfully cracked a password using the timing attack. Some notes on this method:

 * The target device has a finite start-up time, which slows down the attack. If you wish, remove some of the
   printf()'s from the target code, recompile and reprogram, and see how quickly you can do this attack.
   
 * The current script doesn't look for the "WELCOME" message when the password is OK. That is an extension that
   allows it to crack any size password.
   
 * If there was a lock-out on a wrong password, the system would ignore it, as it resets the target after every
   attempt.

Conclusion
----------

This tutorial has demonstrated the use of the power side-channel for performing timing attacks. A target with a simple password-based security
system is broken. In addition you have learned about the scripting support in the ChipWhisperer-Capture software.


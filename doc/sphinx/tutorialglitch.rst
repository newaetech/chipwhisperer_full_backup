.. _tutorialglitch:

Tutorial #A2: Clock Glitch Attacks
==================================

This advanced tutorial will demonstrate clock glitch attacks using the ChipWhisperer system. There is a video which briefly outlines
some of the attacks on this page too:

|YouTubeGlitch|_

.. |YouTubeGlitch| image:: /images/tutorials/advanced/glitching/youtube-glitch.png
.. _YouTubeGlitch: http://www.youtube.com/watch?v=Ib8qvAlU8xo&hd=1


Background on Clock Glitching
-----------------------------

Digital hardware devices almost always expect some form of reliable clock. We can manipulate the clock being presented to the device
to cause unintended behaviour. We'll be concentrating on microcontrollers here, however other digital devices (e.g. hardware encryption
accelerators) can also have faults injected using this technique.

Consider a microcontroller first. The following figure is an excerpt the Atmel AVR ATMega328P datasheet:

.. image:: /images/tutorials/advanced/glitching/mcu-unglitched.png

Rather than loading each instruction from FLASH and performing the entire execution, the system has a pipeline to speed up
the execution process. This means that an instruction is being decoded while the next one is being retrieved, as the following
diagram shows:

.. image:: /images/tutorials/advanced/glitching/clock-normal.png

But if we modify the clock, we could have a situation where the system doesn't have enough time to actually perform an instruction. Consider
the following, where `Execute #1` is effectively skipped. Before the system has time to actually execute it another clock edge comes,
causing the microcontroller to start execution of the next instruction:

.. image:: /images/tutorials/advanced/glitching/clock-glitched.png

This causes the microcontroller to `skip` an instruction. Such attacks can be immensely powerful in practice. Consider for example the following
code from `linux-util-2.24`::

    /*
     *   auth.c -- PAM authorization code, common between chsh and chfn
     *   (c) 2012 by Cody Maloney <cmaloney@theoreticalchaos.com>
     *
     *   this program is free software.  you can redistribute it and
     *   modify it under the terms of the gnu general public license.
     *   there is no warranty.
     *
     */

    #include "auth.h"
    #include "pamfail.h"

    int auth_pam(const char *service_name, uid_t uid, const char *username)
    {
        if (uid != 0) {
            pam_handle_t *pamh = NULL;
            struct pam_conv conv = { misc_conv, NULL };
            int retcode;

            retcode = pam_start(service_name, username, &conv, &pamh);
            if (pam_fail_check(pamh, retcode))
                return FALSE;

            retcode = pam_authenticate(pamh, 0);
            if (pam_fail_check(pamh, retcode))
                return FALSE;

            retcode = pam_acct_mgmt(pamh, 0);
            if (retcode == PAM_NEW_AUTHTOK_REQD)
                retcode =
                    pam_chauthtok(pamh, PAM_CHANGE_EXPIRED_AUTHTOK);
            if (pam_fail_check(pamh, retcode))
                return FALSE;

            retcode = pam_setcred(pamh, 0);
            if (pam_fail_check(pamh, retcode))
                return FALSE;

            pam_end(pamh, 0);
            /* no need to establish a session; this isn't a
             * session-oriented activity...  */
        }
        return TRUE;
    }

This is the login code for the Linux OS. Note that if we could skip the check of ``if (uid != 0)`` and simply branch to the end, we could
avoid having to enter a password. This is the power of glitch attacks - not that we are breaking encryption, but simply bypassing the entire
authentication module!

Glitch Hardware
---------------

The ChipWhisperer Glitch system uses the same synchronous methodology as it's Side Channel Analysis (SCA) capture. A system clock (which can come
from either the ChipWhisperer or the Device Under Test (DUT)) is used to generate the glitches. These glitches are then inserted back into the clock,
although it's possible to use the glitches alone for other purposes (i.e. for voltage glitching, EM glitching). 

The generation of glitches is done with two variable phase shift modules, configured as follows:

.. figure:: /images/tutorials/advanced/glitching/glitchgen-phaseshift.png

The enable line is used to determine when glitches are inserted. Glitches can be inserted continuously (useful for development) or triggered by 
some event. The following figure shows how the glitch can be muxd to output to the Device Under Test (DUT).

.. figure:: /images/tutorials/advanced/glitching/glitchgen-mux.png

Hardware Support
^^^^^^^^^^^^^^^^

The phase shift blocks use the Digital Clock Manager (DCM) blocks within the FPGA. These blocks have
limited support for run-time configuration of parameters such as phase delay and frequency generation, and for maximum performance
the configuration must be fixed at design time. The Xilinx-provided run-time adjustment can shift the phase only by about
+/- 5nS in 30pS increments (exact values vary with operating conditions).

For most operating conditions this is insufficient - if attacking a target at 7.37MHz the clock cycle would have a period of 136nS. In order
to provide a larger adjustment range, an advanced FPGA feature called `Partial Reconfiguration` (PR) is used. The PR system requires special
`partial bitstreams` which contain modifications to the FPGA bitstream. These are stored as two files in the folder
``chipwhisperer\software\chipwhisperer\capture\scopes\cw-partial-file``. These two files are ``s6lx25-glitchwidth.p`` and ``s6lx25-glitchoffset.p``.
These files are `keyed` to the bitstream file, and must be generated with a script. On startup the ChipWhisperer software confirms that the
bitstream file and partial reconfiguration files are consistent with each other. If they are not the partial reconfiguration system is disabled,
as loading incorrect partial reconfiguration files could damage the FPGA (although more likely it will not function).

The current system uses the creation time as a key, and the files must have been generated within 12 hours of each other. When loading the FPGA
file the system will print the FPGA creation time::

    FPGA Bitstream Created: Sat May 10 12:25:18 2014
    
If the PR data is inconsistent with the FPGA file, the system will print a message such as this::

    Partial Reconfiguration DISABLED: FPGA File too old, > 12 hours difference from PR files

Python GUI Interface
^^^^^^^^^^^^^^^^^^^^

The portion of the GUI of interest to us is primarily located in this section:

.. figure:: /images/tutorials/advanced/glitching/prgui.png

If the `Partial Reconfiguration` system has been disabled (due to missing PR files or files differing from the FPGA bitstream) the two fields
marked that say `(as % of period)` will be disabled. Only the fields labeled `(fine adjust)` will be available.

Setting up Glitch Example
-------------------------

Firmware Setup
^^^^^^^^^^^^^^

The glitch examples requires you to program the AtMega328P. The software is located at ``chipwhisperer\hardware\victims\firmware\avr-glitch-examples``
of your ChipWhisperer release or GIT. You can build the software by running the make command as follows::

    make MCU=atmega328p
    
You should also open the file ``glitchexample.c`` which is the source code. The subroutine being glitchedin this example looks like this::

    void glitch1(void)
    {
        //Can monitor Port C, Pin 5 (top right pin on 28-DIP)
        DDRC |= 1<<5;
        PORTC |= 1<<5;
        
        //Some fake variable
        volatile uint8_t a = 0;
        
        output_ch_0('A');
        
        //External trigger logic
        trigger_high();
        trigger_low();
        
        //Should be an infinite loop
        while(a != 2){
        ;
        }
        
        //Several examples in case glitching skips a few instructions
        PORTC = 0;
        PORTC = 0;

        output_ch_0('1');
        output_ch_0('2');
        output_ch_0('3');
        output_ch_0('4');

        PORTC = 0;
        PORTC = 0;
        PORTC = 0;
        PORTC = 0;
        PORTC = 0;

        //Several loops in order to try and prevent restarting
        while(1){
        ;
        }
        while(1){
        ;
        }
        while(1){
        ;
        }
        while(1){
        ;
        }
        while(1){
        ;
        }    
    }

You should confirm that ``glitch1()`` is actually called from the main subroutine. There are several glitch examples and it's possible the
wrong subroutine has been setup previously::

    int main(void){
        trigger_setup();
        init_uart0();
        
        /* Uncomment this to get a HELLO message for debug */	
        output_ch_0('h');
        output_ch_0('e');
        output_ch_0('l');
        output_ch_0('l');
        output_ch_0('o');
        output_ch_0('\n');
        _delay_ms(20);
            
        glitch1();
            
        return 1;
        }

One the AVR is programmed (see previous tutorials), you need to setup a few jumpers on the board.

Hardware Setup
^^^^^^^^^^^^^^

The hardware is almost as in previous incarnations. The difference is the 'FPGAOUT' is bridged to the AVR clock. You can either choose to use the
7.37MHz clock on the multi-target board, or use the CLKGEN feature to generate a suitable 7.37MHz clock. This example will use the CLKGEN feature.

The AVR is being used as the glitch target. The following figure shows the expected jumper settings:

.. image:: /images/tutorials/advanced/glitching/glitchhw.jpg

Software Setup
^^^^^^^^^^^^^^

1. Connect to the ChipWhisperer device:

    1. As the *Scope Module*, select the *ChipWhisperer/OpenADC* option

    2. As the *Target Module*, select the *Simple Serial* option

    3. Switch to the *Target Settings* tab, and as the *connection*, select the *ChipWhisperer* option

    4. Run connect on both the Scope & Target. They should both switch to green circles indicating the system is connected.
    
    5. Optional: If the previous step fails, you can try first to manually program the FPGA with the *Download CW Firmware* tool. As of
       release 0.09 this is not required normally. 

2. Setup the CLKGEN Module to Generate a 7.37 MHz clock and route it through the Glitch Generator

    1. Switch the *Freq Counter Src* to the *CLKGEN Output*
    
    2. Set the *Desired Frequency* to 7.37 MHz. Note you should only adjust the 'frequency' portion of this, if you highlight the entire field
       you may not be able to type the frequency into the system.
       
    3. Confirm the *DCM Locked* checkbox is checked, if not hit the *Reset CLKGEN DCM* box. Check the *Freq Counter* to ensure the system is correctly
       generating a 7.37 MHz clock.

    4. Under the *Glitch Module* set the *Clock Source* as *CLKGEN*:
    
       .. image:: /images/tutorials/advanced/glitching/glitchgen-clkgen.png

    5. Under the *Target HS IO-Out* option select the *Glitch Module*:

       .. image:: /images/tutorials/advanced/glitching/targioout.png

3. Connect the Serial Port
    
    1. From the *Tools* menu select *Open Terminal*, and press *Connect* on the terminal:

        .. image:: /images/tutorials/basic/timingpower/termconn.png

    2. The baud rate for this system is 38400, which should be the default for the ChipWhisperer serial port. 

    3. Connect the USB-A cable (if not already connected) to the back of the ChipWhisperer device. Start AVRStudio and open the
       programmer dialog. We will use the `Read Signature` button to reset the AVR every time we want to restart the program. Confirm
       this works by pressing the `Read Signature` button:
   
       .. image:: /images/tutorials/advanced/glitching/readsig.png
   
       When you press this button the AVR will display the `Hello` message, which should look something like this:
   
       .. image:: /images/tutorials/advanced/glitching/termhello.png


We'll now look at glitching this routine. You should inspect the source code to determine that after sending the ``A`` the system goes into
an infinite loop::

        output_ch_0('A');
        
        //External trigger logic
        trigger_high();
        trigger_low();
        
        //Should be an infinite loop
        while(a != 2){
        ;
        }
        
        //Several examples in case glitching skips a few instructions
        PORTC = 0;
        PORTC = 0;

        output_ch_0('1');
        output_ch_0('2');
        output_ch_0('3');
        output_ch_0('4');

Using clock glitching we'll escape from this loop!

.. _glitch-manual1:

Manual Glitch Trigger
---------------------

To begin with, you'll simply use the manual glitch triggering. This works well in the examples where we have a simple loop we are breaking
out of. Doing so requires modifying the `glitch width` and `glitch offset` experimentally. The exact values will vary for every device and setup.

It is recommended to only use the *glitch width (as % of period)* option, as the fine adjust is too small of a change for this lower-speed
example. Other hardware may need the precision added by the fine adjust however!

The following figure shows several different settings for a 7.37 MHz clock. The `width` is set to 10%, which for the 136nS clock period of the 7.37 MHz
clock means the glitch width is about 13.6 nS. When the `offset` is negative, the glitch is placed in-front of the clock. The glitch is XORd with the clock,
meaning this becomes a small positive-going glitch in-front of the regular clock pulse.

If the `offset` is positive, the glitch occurs *after* the rising edge of the clock pulse. Because this glitch pulse is XORd with the clock, it becomes
a negative-going glitch inserted in the 'middle' of the regular clock pulse.

    .. image:: /images/tutorials/advanced/glitching/clockglitch-examplesettings.png

With some background, let's now check some glitches. Assuming you've setup the example as before, do the following:

1. Set the *Glitch Width (as % of period)* to around 7.5
2. Set the *Glitch Offset (as % of period)* to around -10
3. Ensure *Glitch Trigger* is *Manual*
4. Set the *Repeat* to 1
5. Hit the *Manual Trigger* button
6. See if you end up with either the AVR resetting (reprints ``hello\nA``), or glitches out of the loop (prints ``1234``). It may do both. You may need to
   press the *Manual Trigger* button several times quickly.
7. To force a reset of the AVR, use the `Signature Read` option in AVRStudio.
8. Adjust the glith width & offset as needed. 
9. You may also adjust the *Repeat* option, or cause it to glitch several instructions.

The following figure shows several successul glitches:

    .. image:: /images/tutorials/advanced/glitching/glitchsimple-playaround.png
    
**Be aware that you may crash the AVR!** In the previous examples the AVR had reset after each glitch. It may simply go into another infinite loop
however, or even enter invalid states. Again use the `Signature Read` option in AVRStudio to force a hardware reset of the AVR in these cases. It may
appear like the AVR was never glitched, whereas in reality it was glitched into some invalid state.

Automatic Glitch Triggering
---------------------------

The manual trigger used previously is suitable when the embedded system is waiting for further input. For example if the embedded system is waiting for
a password, you could insert glitches without requiring accurate timing. We'll explore the use of the capture trigger for glitching here, which also improves
the repeatability of your glitch attempts.

To use this system, you must first understand the routing of the trigger to the glitch module. The following figure shows the trigger routing, which is more
basic than the power capture trigger:

    .. image:: /images/triggerrouting.png

Note in particular that if using an external IO pin, you only have a *rising edge trigger*. The example glitch program for the AVR includes a line which is
set 'High' at critical moments, allowing you to experiment with this basic IO trigger. 

Basic Trigger on ``glitch1()``
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

This first example will use the automatic trigger to attack the existing system.

1. Assuming your system is still setup to glitch as in :ref:`glitch-manual1`.

2. Set the *Trigger Pins* to only use the *Target IO4* pin:

    .. image:: /images/tutorials/advanced/glitching/glitchsetup-triggerext.png

3. Using AVRStudio hit the *Read Signature* button to reset the AVR. You should once again see the following output when you hit the *Read Signature* button::

    hello
    A

4. Under the *Glitch Trigger* set to *External Trigger*:

    .. image:: /images/tutorials/advanced/glitching/glitchsetup-trigger.png
    
5. Using AVRStudio hit the *Read Signature* button to reset the AVR. You may need to adjust the *Repeat* number slightly, the objective is to have the system
   automatically glitch through the loop on reset! Likely you won't get 100% reliability, but it's possible to check...
   
   .. image:: /images/tutorials/advanced/glitching/glitchexample-basic1.png


Serial IO Line Trigger on ``glitch1()``
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

We'll now attack the same system as before, but using an automatic digital pattern trigger. This will detect
when the DUT sends the ``A`` character indicating the infinite loop is going to happen.

1. Switch the *Trigger Module* to *Digital Pattern Matching*:

   .. image:: /images/tutorials/advanced/glitching/glitchexample-iotrigger1.png

2. Setup the system to monitor both TX & RX lines, using an AND logic. The lines normally idle high, thus
   the AND combination allows you to trigger on either sent or received data:

   .. image:: /images/tutorials/advanced/glitching/glitchexample-iotrigger2.png

3. Under the *Digital Pattern Trigger Module*, set the Baud rate to 38400. Set the *Trigger Character* to ``A``:

   .. image:: /images/tutorials/advanced/glitching/glitchexample-iotrigger3.png

When the glitch sends an ``A``, the glitch will trigger. If you want to check the trigger is occurring, you
can use the normal analog capture. To do so follow these steps:

4. Set the gain setting to *30*:

   .. image:: /images/tutorials/advanced/glitching/glitchexample-iotrigger-analog1.png

5. Set the trigger mode to *rising edge*:

   .. image:: /images/tutorials/advanced/glitching/glitchexample-iotrigger-analog2.png
   
6. Set the ADC clock source to *CLKGEN x4 via DCM*, and ensure the *DCM Locked* checkbox indicates the DCM
   is locked, along with the ADC frequency being *29.5 MHz*. Hit the *Reset ADC DCM* button if this is not
   the case:
   
   .. image:: /images/tutorials/advanced/glitching/glitchexample-iotrigger-analog3.png

Finally - we can check both the triggering and the glitches. To check the triggers our occurring:

7. Hit the *Capture 1* box. Quickly (before the timeout occurs) hit the *Read Signature* button in AVRStudio,
   which will reset the AVR. It should send an ``A`` causing the trigger to occur. If the system is working the
   ADC will capture data, immediately on hitting *Read Signature*. If the trigger is NOT working you will instead
   see a message printed about *Timeout in OpenADC capture(), trigger FORCED* in the *Debug Logging* tab:
   
   .. image:: /images/tutorials/advanced/glitching/glitchexample-iotrigger-analog3.png

8. The glitch trigger will occur whenever the trigger conditions are met, and *DOES NOT* rely on the capture
   to be armed. Thus for example try adjusting the *Repeat* number until you are able to get glitches occuring
   by simply hitting the *Read Signature* button to reset the AVR.
   
  
.. _basic-glitch2:

Basic Trigger on ``glitch2()``
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

In this module we'll be glitching a new function, which compares a received character to some known character. 
The ``glitch2()`` function looks as follows::

    void glitch2()
    {
        char c;  
        
        output_ch_0('B');
           
        c = input_ch_0();
        
        trigger_high();
        trigger_low();
        
        if (c != 'q'){
            output_ch_0('1');
        } else {
            output_ch_0('2');
        }
        output_ch_0('\n');
        output_ch_0('\n');
        output_ch_0('\n');
        output_ch_0('\n');
    }

Here you need to send a character to the DUT for it to respond appropriately. The following steps details this,
assuming you are already connected to the target device, for example by following at minimum the :ref:`glitch-manual1`
example, however if you've completed the serial triggering tutorial you can also use that system setup.

1. Modify the file ``glitchexample.c`` to call ``glitch2()`` instead of ``glitch1()``, which is to say simply
   change the main function called from ``main()`` to ``glitch2()``.
   
2. Run ``make MCU=atmega328p`` in the folder ``chipwhisperer\hardware\victims\firmware\avr-glitch-examples``

3. In order to program the resulting ``.hex`` file, you must **DISABLE** the glitches so you have a clean clock.
   Set the glitch trigger to manual:

   .. image:: /images/tutorials/advanced/glitching/glitchexample-basic2-disableglitch.png
 
4. Program the ``glitchexample.hex`` file using AVRStudio into the microcontroller.

5. Using AVRStudio hit the *Read Signature* button to reset the AVR. You should once again see the following output when you hit the *Read Signature* button::

    hello
    B

6. Try typing a ``q`` character and hit enter, and view the response in the console. If you send a ``q`` the
   system will respond with a ``2``. If you send any other character the system will respond with a ``1``. Once
   the response has been sent you need to reset the AVR using the ``Read Signature`` button!
   
   .. image:: /images/tutorials/advanced/glitching/glitchexample-basic2-comms.png

7. We will now set the *Glitch Trigger* set to *External Trigger*:

   .. image:: /images/tutorials/advanced/glitching/glitchsetup-trigger.png
   
8. Set the trigger routing to use IO-Pin 4 only, uncheck any other options (e.g. front-panel A or TX/RX lines): 

   .. image:: /images/tutorials/advanced/glitching/glitchexample-basic2-setup1.png
   
9. Reset the AVR, and again it will send a ``B`` then wait for input. Send a character (either ``q`` or something
   that is not q such as ``a``). The objective is to observe some odd behaviour, such as always responding with a
   ``2``. In the following example note that sending a ``q`` causes the system to continue operation correctly,
   but other characters cause it to reset:
   
   .. image:: /images/tutorials/advanced/glitching/glitchexample-basic2-results.png
   
   You will have to modify the *Repeat* value! You may also need to tweak the glitch offset and width.
   

Basic Trigger on ``glitch3()``
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
In this module we'll be glitching a new function, which compares a received password to some known password. 
The ``glitch3()`` function looks as follows::

    void glitch3()
    {
        char inp[16];
        char c;
        int cnt = 0;
        output_ch_0('C');

        c = 'A';
        while((c != '\n') & (cnt < 16)){
            c = input_ch_0();
            inp[cnt] = c;
            cnt++;
        }
        
        char passwd[] = "touch";
        char passok = 1;
        
        trigger_high();
        trigger_low();
        
        //Simple test - doesn't check for too-long password!
        for(cnt = 0; cnt < 5; cnt++){
            if (inp[cnt] != passwd[cnt]){
                passok = 0;
            }
        }
        
        if (!passok){
            output_ch_0('B');
            output_ch_0('a');
            output_ch_0('d');
            output_ch_0('\n');
        } else {
            output_ch_0('W');
            output_ch_0('e');
            output_ch_0('l');
            output_ch_0('c');
            output_ch_0('o');
            output_ch_0('m');
            output_ch_0('e');
            output_ch_0('\n');
        }
    }
    
The following assumes you have already completed the setup in the :ref:`basic-glitch2`.

1. Modify the file ``glitchexample.c`` to call ``glitch3()`` instead of ``glitch2()``, which is to say simply
   change the main function called from ``main()`` to ``glitch3()``.
   
2. Run ``make MCU=atmega328p`` in the folder ``chipwhisperer\hardware\victims\firmware\avr-glitch-examples``

3. In order to program the resulting ``.hex`` file, you must **DISABLE** the glitches so you have a clean clock.
   Set the glitch trigger to manual:

   .. image:: /images/tutorials/advanced/glitching/glitchexample-basic2-disableglitch.png
 
4. Program the ``glitchexample.hex`` file using AVRStudio into the microcontroller.

5. Using AVRStudio hit the *Read Signature* button to reset the AVR. You should see the following output when you hit the *Read Signature* button::

    hello
    C

6. Ensure the *TX on Enter* is set to ``\n``. Type ``t`` and send to the target, which should respond with
   ``Welcome``, indicating the password was accepted. Reset the AVR and try other passwords, it will respond with
   ``Bad``. Our objective is to get the ``Welcome`` message with the wrong password!
   
   .. image:: /images/tutorials/advanced/glitching/glitchexample-basic3-serial.png

7. We will now set the *Glitch Trigger* set to *External Trigger*:

   .. image:: /images/tutorials/advanced/glitching/glitchsetup-trigger.png
   
8. The system is now setup to glitch! Using the AVR reset, keep trying new passwords. You will need to finely tune
   all three parameters (Repeat, Glitch Width, Glitch Offset). In addition the glitch may not be reliable - it may
   be only occasionally the password is accepted. However for most secure embedded systems it would not be required
   to have a 'reliable' password glitch entry, just one that works often enough! The following shows an example of
   a successful glitch attack:
   
   .. image:: /images/tutorials/advanced/glitching/glitchexample-basic3-success.png
   
Glitching Onward
----------------

This basic tutorial has introduced you to glitch attacks. They are a powerful tool for bypassing authentication
in embedded hardware devices. There are many ways to expand your knowledge with additional practice, such as:

* Use manual glitches to try simply glitching past the prompt in ``glitch3()``.
* Download some example source code (bootloaders, login prompts, etc) and port them to the AVR. See how you can
  glitch past security checks.
  
  
  

  
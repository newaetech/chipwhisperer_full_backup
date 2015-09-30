.. _tutorialglitch:

.. include:: imagenums.rsti

Tutorial #A2: Introduction to Glitch Attacks (including Glitch Explorer)
========================================================================

This advanced tutorial will demonstrate clock glitch attacks using the ChipWhisperer system. This will introduce you to many
required features of the ChipWhisperer system when it comes to glitching. This will be built on in later tutorials to generate
voltage glitching attacks, or when you wish to attack other targets.

The follow OUTDATED video which briefly
outlines the attack using the CW Capture Rev2 device, note this follows an older version of the tutorial and thus does not yet 
directly follow this guide:

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
`partial bitstreams` which contain modifications to the FPGA bitstream. These are stored as two files inside a "firmware" zip which
contains both the FPGA bitstream along with a file called ``glitchwidth.p`` and a file called ``glitchoffset.p``. If a lone bitstream
is being loaded into the FPGA (i.e. not from the zip-file), the partial reconfiguration system is disabled,
as loading incorrect partial reconfiguration files could damage the FPGA. This damage is mostly theoretical, more likely the FPGA will
fail to function correctly.

If in the course of following this tutorial you find the FPGA appears to stop responding (i.e. certain features no longer work correctly),
it could be the partial reconfiguration data is incorrect.

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

The glitch examples requires you to program the target device. The software to program is located at
``chipwhisperer\hardware\victims\firmware\glitch-simple`` of your ChipWhisperer release or GIT clone.
As before, open the ``Makefile`` and be sure to uncomment the appropriate target to reflect your
hardware::

    #Multi-Target Board, AVR Device (ATMega328P)
    #PLATFORM = CW301_AVR
    
    #CW-Lite XMEGA Target Device (XMEGA128D4)
    PLATFORM = CW303


You can build the software by running the make command as follows::

    make
    
You should also open the file ``glitchsimple.c`` which is the source code. The subroutine being glitched 
in this example looks like this::

    void glitch_infinite(void)
    {
        char str[64];
        //Declared volatile to avoid optimizing away loop.
        //This also adds lots of SRAM access
        volatile uint16_t i, j;
        volatile uint32_t cnt;
        while(1){
            cnt = 0;
            for(i=0; i<500; i++){
                for(j=0; j<500; j++){
                    cnt++;
                }
            }
            sprintf(str, "%lu %d %d\n", cnt, i, j);
            uart_puts(str);
        }
    }

You should confirm that ``glitch_infinite()`` is actually called from the main subroutine. There are several glitch examples
and it's possible the wrong subroutine has been setup previously::

    int main(void){
    
        platform_init();
        init_uart();    
        trigger_setup();
    
        /* Uncomment this to get a HELLO message for debug */
        putch('h');
        putch('e');
        putch('l');
        putch('l');
        putch('o');
        putch('\n');
        _delay_ms(20);
            
            
        while(1){
            glitch_infinite();
        }
            
        return 1;
    }

Once the AVR/XMEGA is programmed (see previous tutorials), you may need to setup a few jumpers depending on your hardware.

Hardware Setup
^^^^^^^^^^^^^^

XMEGA Target (CW1173 + CW303)
'''''''''''''''''''''''''''''
The XMEGA target on the ChipWhisperer-Lite requires no configuration. If you have seperate the boards, you can attached
them with the 20-pin cable.

Multi-Target Board, AVR (CW301)
'''''''''''''''''''''''''''''''
The hardware is almost as in previous incarnations. The difference is the 'FPGAOUT' is bridged to the AVR clock.
This example will use the CLKGEN feature.

The AVR is being used as the glitch target. The following figure shows the expected jumper settings:

.. image:: /images/tutorials/advanced/glitching/glitchhw.jpg

Software Setup
^^^^^^^^^^^^^^

1. Connect to the ChipWhisperer device:

    1. As the *Scope Module*, select the *ChipWhisperer/OpenADC* option

    2. As the *Target Module*, select the *Simple Serial* option
    
    3. Switch to the *Scope Settings* tab, and as the *connection*, select the *ChipWhisperer Rev2* or *ChipWhisperer-Lite* option

    4. Switch to the *Target Settings* tab, and as the *connection*, select the *ChipWhisperer Rev2* or *ChipWhisperer-Lite* option

    5. Run connect on both the Scope & Target. They should both switch to green circles indicating the system is connected.
    
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

    1. For the XMEGA Target (including the CW-Lite integrated target), perform the following:
    
       a. Switch to the *Scope Settings* tab, and scroll down to *Target IOn Pins*
       b. Switch the *Target IO1* to be *Serial RXD*
       c. Switch the *Target IO2* to be *Serial TXD*       
    
    2. From the *Tools* menu select *Open Terminal*, and press *Connect* on the terminal:

        .. image:: /images/tutorials/basic/timingpower/termconn.png

    3. The baud rate for this system is 38400, which should be the default for the ChipWhisperer serial port. 

    4. Using the target programmer window, we will use the `Read Signature` or `Check Signature` button to reset the
       target every time we want to restart the program. Confirm this works by pressing the `Read Signature` button,
       for example if using the ChipWhisperer-Lite integrated programmer, you would see this window:
       
       .. image:: /images/cw1173/xmegaprog_main.png
       
       But if using the external AVR Studio programmer for the ChipWhisperer Capture Rev2, you would see this window:
   
       .. image:: /images/tutorials/advanced/glitching/readsig.png
   
       When you press this button the AVR will display the `Hello` message, which should look something like this:
   
       .. image:: /images/tutorials/advanced/glitching/termhello.png

       HINT: If you uncheck the *RX: Show non-ASCII as hex* you will not see the red text with ASCII values of newline (``0a``).


We'll now look at glitching this routine. You should inspect the source code to determine that a simple series of calculations are
performed::

    void glitch_infinite(void)
    {
        char str[64];
        //Declared volatile to avoid optimizing away loop.
        //This also adds lots of SRAM access
        volatile uint16_t i, j;
        volatile uint32_t cnt;
        while(1){
            cnt = 0;
            for(i=0; i<500; i++){
                for(j=0; j<500; j++){
                    cnt++;
                }
            }
            sprintf(str, "%lu %d %d\n", cnt, i, j);
            uart_puts(str);
        }
    }

If the routine works as expected, we would expect it to print ``250000 500 500``. If a glitch interrupts the program
flow, we would expect some of those values to be incorrect. This could be because a loop was skipped, an addition done incorrectly,
or the program flow was exited unexpectedly.

.. _glitch-manual1:

Manual Glitch Trigger
---------------------

To begin with, you'll simply use the manual glitch triggering. This works well in the examples where we have a simple loop we are breaking
out of. Doing so requires modifying the `glitch width` and `glitch offset` experimentally. The exact values will vary for every device and
setup.

It is recommended to only use the *glitch width (as % of period)* option, as the fine adjust is too small of a change for this lower-speed
example. Other hardware may need the precision added by the fine adjust however!

The following figure shows several different settings for a 7.37 MHz clock. The `width` is set to 10%, which for the 136nS clock period of
the 7.37 MHz clock means the glitch width is about 13.6 nS. When the `offset` is negative, the glitch is placed in-front of the clock.
The glitch is XORd with the clock, meaning this becomes a small positive-going glitch in-front of the regular clock pulse.

If the `offset` is positive, the glitch occurs *after* the rising edge of the clock pulse. Because this glitch pulse is XORd with the
clock, it becomes a negative-going glitch inserted in the 'middle' of the regular clock pulse.

    .. image:: /images/tutorials/advanced/glitching/clockglitch-examplesettings.png

With some background, let's now check some glitches. Assuming you've setup the example as before, do the following:

1. Adjust the settings for *Glitch Width (as % of period)* , *Glitch Offset (as % of period)*, and  *Repeat*
   based on your target and the following table for different targets:

    ============================== =========================== ===================
    Parameter                      AVR on Multi-Target (CW301) CW-Lite XMEGA Board
    ============================== =========================== ===================
    Glitch Width (as % of period)  7.5                         10
    Glitch Offset (as % of period) -10                         26.6
    Repeat                         5                           105
    ============================== =========================== ===================

2. Ensure *Glitch Trigger* is *Manual*
3. Hit the *Manual Trigger* button
4. See if you end up with either the target resetting (reprints ``hello\n``), or if
   the loop count becomes wrong. You may need to  press the *Manual Trigger* button
   several times quickly. The objective is to have an incorrect loop count, meaning
   you caused a glitch!   
5. To force a reset of the target, use the `Signature Read` option on the programmer.
6. Adjust the glith width & offset as needed. 
7. You may also adjust the *Repeat* option, or cause it to glitch several instructions.
   
**Be aware that you may crash the target!** In the previous examples the target could have
reset after each glitch. It may simply go into another infinite loop however, or even enter
invalid states. Again force a hardware reset of the target in these cases. It may
appear like the target was never glitched, whereas in reality it was glitched into some
invalid state.

Automatically Resetting Target
------------------------------

If we are going to start with the target at a pre-determined state, we need to reset the target. There are
two ways of automatically performing this. The method used here will use the existing programmer interface
to reset the device by performing that "read signature" operation we have already been using. The other
method is to toggle a GPIO pin, which is more generic for future use.

To setup the automatic reset, perform the following:

1. Switch to the *General Settings* tab, and select the appropriate "Auxiliary Module":

   a. For ChipWhisperer-Lite (CW1173/CW1180), select "Reset AVR/XMEGA via CW-Lite"
   
   b. For ChipWhisperer-Capture Rev 2, select "Reset AVR via ISP-MKII"
   
   .. image:: /images/tutorials/advanced/glitching/glitching_aux_1.png

2. Switch to the *Aux Settings* tab. Depending on your module you will see different settings here, for
   example the ChipWhisperer-Lite lets you select between AVR and XMEGA targets. The defaults should be
   ok:
   
   .. image:: /images/tutorials/advanced/glitching/glitching_aux_2.png
   
3. Press the "Test Reset" button. This should reset the AVR target. Confirm this by monitoring
   the terminal emulator window, and check the startup message (of "hello") is printed again.


We can now confirm the reset works with the "Capture 1" button. This requires us to disable the normal
routing of the output data to a file for analysis, as we want to just dump data to the terminal emulator.
To do this:

1. Switch to the *Target Settings* tab.

2. Remove all of the *Load Key Command*, *Go Command*, and *Output Format* options:

   .. image:: /images/tutorials/advanced/glitching/targetsettings_clear.png
   
3. Press the "Capture 1" button a few times, which should confirm on each "capture" the target device
   is resetting.
   
4. Finally, we will switch the glitch target to give us a more realistic target to glitch. To do this
   open the file ``chipwhisperer\hardware\victims\firmware\glitch-simple\glitchsimple.c`` and modify
   the call in ``main()``, such that we now call the ``glitch1()`` function. This means the following::
   
        while(1){
            glitch_infinite();
        }
       
   can be changed to::
   
        while(1){
            glitch1();
        }
        
   After which recompile (with ``make``), and reprogram the target device. Note the new function being
   glitched looks like this::
   
        void glitch1(void)
        {
            led_ok(1);
            led_error(0);
            
            //Some fake variable
            volatile uint8_t a = 0;
            
            putch('A');
            
            //External trigger logic
            trigger_high();
            trigger_low();
            
            //Should be an infinite loop
            while(a != 2){
            ;
            }    
            
            led_error(1);
            led_error(1);
            led_error(1);
            led_error(1);
            led_error(1);
            led_error(1);
            led_error(1);
            led_error(1);
            led_error(1);
            
            uart_puts("1234");
            
            led_error(1);
            led_error(1);
            led_error(1);
            led_error(1);
            led_error(1);
            led_error(1);
            led_error(1);
            led_error(1);
        
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
        
5. When you perform a *Capture 1*, the terminal should print ``hello\nA``, based on the above source code. Note the
   objective will be to glitch past the infinite loop, such that ``1234`` is printed. If using the XMEGA target board
   this will also turn on the RED led.
   
   
.. _tutorialglitchtriggering:
   
Automatically Triggering Glitch
-------------------------------

The manual trigger used previously is suitable when the embedded system is waiting for further input. For example if the
embedded system is waiting for a password, you could insert glitches without requiring accurate timing. We'll explore
the use of the capture trigger for glitching here, which also improves the repeatability of your glitch attempts.

To use this system, you must first understand the routing of the trigger to the glitch module. The following figure
shows the trigger routing, which is more basic than the power capture trigger:

    .. image:: /images/triggerrouting.png

Note in particular that if using an external IO pin, you only have a *rising edge trigger*. The example glitch program
includes a line which is set 'High' at critical moments, allowing you to experiment with this basic IO trigger. 

Based on the source code loaded, we currently have a trigger to time the glitch. This is very useful during the 
characterization phase, where we wish to determine what sort of glitch affects this specific hardware. Once we
know that, we can move onto glitching a more "realistic" routine.

Before doing that, we will actually enable the power analysis capture. To do this:

1. Switch to the *Scope Settings* tab.

2. Switch the *ADC Clock Source* as being *CLKGEN x4*.

3. Press *Reset ADC DCM*, confirm the frequency is 29.5 MHz as expected.

4. Switch the *Trigger Setup* --> *Mode* to be *Rising Edge*

5. Switch the *Trigger Setup* --> *Total Samples* to be *1000*

6. Switch the *Gain Setting* --> *Setting* to be *40*. You might need to adjust this for different hardware.

7. Press *Capture 1*, confirm some waveform is displayed. For example with the XMEGA Target on the ChipWhisperer-Lite,
   the waveform looks like this:
   
   .. image:: /images/tutorials/advanced/glitching/basic_waveform.png
   
8. If this does't work: check the trigger in use is the *Target IO4* pin.

Finally, we can enable the trigger of the glitch to occur based on this external trigger pin. This can be accomplished
by:

1. Switch the *Glitch Trigger* mode to *Ext Trigger:Single-Shot*:

   .. image:: /images/tutorials/advanced/glitching/singleshot.png
   
2. Performing a *Capture 1*, you'll notice that the waveform is now perturbed. This is due to the clock glitches causing
   odd power consumption behavior:
   
   .. image:: /images/tutorials/advanced/glitching/basic_waveform_glitchy.png
   
3. Play around a bit with the glitch width, offset, and repeat. You should see different effects in the power consumption
   traces.

Using the Glitch Explorer
-------------------------

Now that we can automatically perform the glitching, we can use the *glitch explorer* to automatically vary glitch parameters
while recording what the target device is doing. Before continuing with the tutorial, we'll go through an overview of the
the glitch explorer.

Glitch Explorer
^^^^^^^^^^^^^^^

The main window of the glitch explorer looks like this:

.. image:: /images/tutorials/advanced/glitchexplorer/ge_main.png

Where you can see the following parts

 1. In |1| the output of the system combined with the parameters of the glitch is displayed (the 'output window').
 2. In |2| you can adjust general parameters of the glitching system, such as what counts as a successful glitch or
    not and how many parameters to fiddle with.
 3. In |3| you actually specify the parameters to adjust during the glitching attempts, and what range you would like
    to adjust them over.

We'll be looking at each of these sections in more detail next.

The Output Window
'''''''''''''''''

.. image:: /images/tutorials/advanced/glitchexplorer/ge_top.png

The output window highlights different types of output. In this example we have an output |1| highlighted in green,
which is flagged as a successful glitch. This example code was waiting for the *rrrr* sequence.

Glitches can also be flagged as 'normal', in which case there is no highlight as in |2|. Finally the glitch could be
flagged as an error, in which case it will be highlighted in red.

In order for the glitch explorer to receive the output value, you must insert the special code ``$GLITCH$`` into the
*Target Settings* --> *Output Format* settings. This will mean data is no longer sent to the terminal emulator when
using the capture 1 or capture multi buttons, but instead is logged in the glitch explorer window.

The Main Settings
'''''''''''''''''
 
Details of the main settings:
 
.. image:: /images/tutorials/advanced/glitchexplorer/ge_middle.png

The number of tuning parameters is set at |1|. This defines how many different parameters to adjust.

The response of the system during normal operation is set at |2|. This defines what happens when no glitching
or unexpected behavior happened.

The desired response of the system if the glitch was successful is set at |3|.

The expected and desired responses are expected to be Python expressions, where ``s`` is a str-type variable
which contains the response of the system. The expression must evaulate to ``True`` or ``False``. For example,
the following shows examples of what you could use as possible expressions:

========================================== =====================
Desired Behavior                            Parameter Expression
========================================== =====================
Check for "hello\n" exactly.               s == "hello\n"
Check for "hello\n" at end of string.      s.endswith("hello\n")
Check for hex 0xAF in last byte position.  ord(s[-1]) == 0xAF
========================================== =====================

Note that there is sometimes garbage in the first position. This occurs because if the target device is being
reset before the glitch, you may see the serial lines floating. These floating lines may cause invalid characters
to be recorded.
 
Parameter Settings
''''''''''''''''''
 
Details of the parameter settings:
 
.. image:: /images/tutorials/advanced/glitchexplorer/ge_bottom.png

The name of the parameter to be tuned is set at |1|. This name will be displayed in the table, so should be some
useful reminder of the parameter name.

The *Script Command* at |2| defines what is modified. This string is simply copied from the *Script Commands* tab of
the main window. This lets you modify *any* parameter of the GUI, but most likely you will want to modify some of the
glitch settings. The numeric value in this string is ignored - the numeric value will be automatically overwritten
with the value of the parameter to try.

The *Data Format* at |3| defines what type of data will be inserted into the parameter. For example for the *Glitch Offset (as % of period)*
used here, this would need to be set to *Float*, meaning fractional values such as "4.5" are allowed. The data format
should match the data format expected. 

The *Range* at |4| defines the range from the minimum to maximum that will be swept for the parameter. Valid values
depend on the parameter being adjusted. For example if you are adjusting the *Glitch Offset* a negative and positive
offset makes sense, but if you were adjusting the *Repeat* it would only make sense to adjust over the range ``(1,255)``.

The *Value* at |5| defines the starting value of the sweep. This is **NOT** automatically set to the minimum value
of your sweep. If you want to perform the full sweep, you must manually set this to the minimum of the range.

The *Step* at |6| defines the amount the value is incremented on each glitch attempt. When the value reaches the maximum
defined by the range, it will loop around to the minimum and continue incrementing.

The *Repeat* at |7| defines how many times to perform the same value. This can be used to determine the reliability of
each glitch value.

If using multiple parameters, they will be looped as shown in this pseudo-code::

  param_value_0 = starting_value_0
  param_value_1 = starting_value_1
  
  while capture_in_progress:
      while param_value_1 < param_value_1_max:
      
        while param_value_0 < param_value_0_max:
            param_value_0 += param_value_0_incr
            run_glitch_here()
              
        param_value_0 = param_value_0_min        
        param_value_1 += param_value_1_incr
    
      param_value_1 = param_value_1_min

Note you can quickly cause very long captures to occur! To run the glitch explorer, you need to set
the appropriate number of traces on the *General Settings* tab, and use the *Capture Multi* to run
the glitch explorer.

Example Running the Glitch Explorer
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

This example will attempt to break out the loop in ``glitch1()``. Moving ahead from where you were
in :ref:`tutorialglitchtriggering`, we will see how we can view the output of the target device in
the glitch explorer.

1. Switch to the *Target Settings* tab, and set the *Output Format* to be ``$GLITCH$``:

   .. image:: /images/tutorials/advanced/glitching/output_glitch.png

2. From the *Tools* menu select *Glitch Monitor* to open the glitch explorer.

3. Press the *Capture 1* button a few times, and you should see the table populated with
   outputs:
   
   .. image:: /images/tutorials/advanced/glitching/ge_setup1.png
   
   We want to mark them as "normal" or "glitch successful" to get the color-coding
   working appropriately.
   
4. Double-click on a normal response, and copy the text. In the *Normal Response* field, we need to
   compare the magic variable ``s`` with that copied text. Do this by setting the *Normal Response*
   to be: ``s == '\x00hello\nA'``.
   
5. We want to mark a string ending with ``1234`` as a pass. Thus in the *Successful Response* field,
   set the test to be ``s.endswith('1234')`` (remember in Python both ``'`` and ``"`` are valid for string
   start/end characters).

6. Press *Capture 1* a few more times, and check the color-coding has changed:

   .. image:: /images/tutorials/advanced/glitching/ge_setup2.png

The next step is to tune the glitch offset to attempt to get a successful clock glitch. These steps are
listed as follows:

1. Change the *Tuning Parameters* to be *1*.

2. Set the following in Parameter 0 options:

    ============== ========================================================
    Option          Value
    ============== ========================================================
    Name           Offset
    Script Command ['Glitch Module', 'Glitch Offset (as % of period)', 0.0]  
    Data Format    Float
    Range          -30 : 30
    Value          -30
    Step           0.5
    Repeat         1
    ============== ========================================================

   The *Script Command* option can be found by manually making an adjustment
   to the *Glitch Offset  (as % of period)* in the GUI, and observing the string printed
   to the *Script Commands* tab. The numeric value of the command is ignored (i.e. the
   third element of the array) by the glitch explorer, instead the tuning value
   will always be inserted.

3. On the main GUI in the *Scope Settings* tab, change the following values
   for the *Glitch Module*:
    
    a. *Repeat* set to 10.
    b. *Glitch Width (as % of period)* set to 8.0.

   These values will be used during the glitch explorer run. We have not specified
   anything for the tuning, so they will not be changed from whatever is already
   in the GUI.    
    
4. On the *General Settings* tab:
    a. Ensure the *Trace Format* is set to *None* (i.e., no traces will be written to disk).
    b. Set the *Number of Traces* to 121.
    
5. Press the *Capture Multi* button. You will get a warning as there is no trace writer, but can just
   hit *Continue Anyway*, since we do not want to store traces to disk.

6. With any luck, at least one of the glitches will be successful:

   If you get a reset (prints 'hello' again),
   you might need to reduce the "repeat" value. If you have no successful glitches, double-check all settings.
   You can continue to the next step anyway, as in that step we will also tune the "glitch width".

We may also need to tune the "Glitch Width". We can use knowledge of the successful glitch from the previous
step to reduce our search space. In this case, assume we had a successful glitch with a width of 8.0 and offset
of 17.5. We'll search around those values to see if we can achieve a more successful glitch performance.

To continue the tutorial, the following steps will be taken:

1. In the *Glitch Explorer*, set the *Tuning Parameters* to *2*.

2. Configure the second parameter with the following:

    ============== ========================================================
    Option          Value
    ============== ========================================================
    Name           Width
    Script Command ['Glitch Module', 'Glitch Width (as % of period)', 0.0]  
    Data Format    Float
    Range          5 : 15
    Value          5
    Step           0.5
    Repeat         1
    ============== ========================================================

3. Change the *Range* of the first parameter *Glitch Offset* to span from 1 to 25, since it appeared that
   negative offsets were never successful in our previous attempts. Be sure to reset the *Value* of this
   parameter to your desired starting point (probably *1*). This will reduce the search time.
   
4. On the main GUI in the *Scope Settings* tab, adjust the *Glitch Module* repeat parameter to be 1. We are
   now attempting to acheive success with a single clock cycle being glitched.
   
5. Still in the main GUI, adjust the number of traces per capture to be 1000. This reflects the number of
   iterations required to run through both loops (20 x 50).
   
6. Hit the *Capture Multi* button and cross your fingers! Hopefully you will see a successful glitch for some
   combination of glitch width and offset. We aren't quite done yet, as you will also need to do some fine-tuning
   to achieve high reliability on the glitch.
   
Record some of the useful parameters by scrolling through the window (WARNING: changing parameters will clear the
table, so record useful values now). In this example there was a success at Offset = 8.5%, and Width = 7.5%. Let's
see how to fine-tune those values:

1. Plug those values into the main GUI *Glitch Module* setting. If we use the *Capture 1* button values are taken from
   the main GUI, instead of the glitch explorer.
   
2. Press the *Capture 1* button a few times. You'll note it records the output of the device, which may not be generating
   successful glitches (NB: the "offset" and "width" recorded in the table may be wrong when using the Capture 1 button,
   as the glitch explorer is not recording values from the main GUI correctly. This is a bug in the display only, the
   correct values are being sent to the device).
   
3. Using arrow keys, nudge the *Glitch Offset (fine adjust)* up and down. Try performing a *Capture 1* to see if you are
   able to achieve a reliable glitch. In this example setting the fine adjust to 44 resulted in a very reliable glitch:
   
   .. image:: /images/tutorials/advanced/glitching/ge_examplebasic2.png
   
   You might want to try seeing if there is an upper limit to this setting, and putting it mid-way between the lower and
   upper limits for generating a glitch.
   
Congrats! You've now performed some tuning to achieve a reliable glitch on the target device. The next step is to glitch
something more fun - like a password check.

Glitching a Password Check
--------------------------

This assumes you now have a set of parameters which caused a reliable glitch. We'll now glitch past a password check,
initially using our trigger as a crutch. The function of interest compares a received password to some known password. 
The ``glitch3()`` function looks as follows::

    void glitch3(void)
    {
        char inp[16];
        char c = 'A';
        unsigned char cnt = 0;
        uart_puts("Password:");
        
        while((c != '\n') & (cnt < 16)){
            c = getch();
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
            uart_puts("Denied\n");
        } else {
            uart_puts("Welcome\n");
        }
    }
        
The following assumes you have already completed the previous steps:

1. Close the glitch explorer.

2. Modify the file ``glitchexample.c`` to call ``glitch3()`` instead of ``glitch1()``, which is to say simply
   change the main function called from ``main()`` to ``glitch3()``.
   
3. Run ``make`` in the folder ``chipwhisperer\hardware\victims\firmware\glitch-simple``.

4. Program the target device with your ``.hex`` file.

5. On the *Target Settings* tab, clear the *Output Format* field. That is remove the ``$GLITCH$`` text, as we
   are no longer using the glitch explorer. If you don't do this, you will not see any output of the device on
   the terminal emulator.
   
6. Open the terminal emulator, and connect to it again (if you closed it).
   
7. Reset the device, it should prompt you for a password. The correct password is ``touch``, try both correct
   and incorrect passwords. The program as designed loops after a password try to prompt you again. You should
   see both correct and incorrect responses:
   
   .. image:: /images/tutorials/advanced/glitching/password_normal.png

8. On the *Scope Settings* tab, adjust the *Timeout(s)* to a larger value such as 20. We need a longer timeout
   to work with the serial terminal.
   
9. Let's try a glitch insertion! Perform the following:

   a. Press the *Capture 1* button. This will reset the target and arm the glitch.
   b. Before the timeout, enter a wrong password such as ``test`` in the terminal and hit enter.
   c. See if you can get the wrong password accepted. If not, let's use the Glitch Explorer to automate the 
      parameter adjustments.
      
10. Switching to the *Auxiliary Settings* tab, adjust the delay on the reset such that you have a *150 mS* delay.
    This will mean once the device resets there is a delay while it prints the startup message.
    
    .. image:: /images/tutorials/advanced/glitching/aux_delay150ms.png
    
11. Switch to the *Target Settings* tab:

    a. In the *Go Command* field, put the bad password such as ``test\n``.
    b. In the *Output Format* field, put ``$GLITCH$`` to route the output to the glitch explorer.
    
12. Open the *Glitch Explorer*, and press *Capture 1*. You should see the *Denied* message come across.

13. In the *Glitch Explorer*, adjust the following settings:

    a. Set *Tuning Parameters* to 1
    b. Set Parameter 0 options to:
    
        ============== ========================================================
        Option          Value
        ============== ========================================================
        Name           Trigger Offset
        Script Command ['Glitch Module', 'Ext Trigger Offset', 97]  
        Data Format    Int
        Range          0 : 200
        Value          0
        Step           1
        Repeat         1
        ============== ========================================================
    
14. Set the number of traces on the *General Settings* tab to 200.

15. On the main GUI, in the *Scope Settings* tab, ensure that you have the number
    of repeats on the *Glitch Module* set to 1. We will start with a single clock
    cycle glitched.
    
16. Press *Capture Multi*. Monitor the glitch outputs, you may see some errors
    or a successful glitch. Note that sometimes the errors are useful - here is
    an example where the glitched code actually dumped the password:

     .. image:: /images/tutorials/advanced/glitching/password_glitch_dump.png
     
    More likely you might see a "Welcome" message indicating the password check was glitched:
   
     .. image:: /images/tutorials/advanced/glitching/password_glitch_success.png
   
17. If the previous step isn't successful, increase the "repeat" count on the
    *Glitch Module* section of the *Scope Settings* tab, and try
    again. In this example I actually needed a repeat count of "3" to get the
    successful "Welcome" message printed above.
    
    You can also increase the repeat count in the glitch explorer, which simply
    tries the same settings multiple times. You will likely find that the successful
    glitch does not have 100% success rate, so using a repeat count of 2 or 3
    is helpful to increase your chances of success.
  
   
Serial IO Line Trigger on ``glitch1()`` (not applicable to CW-Lite)
-------------------------------------------------------------------

The following is possible only with the ChipWhisperer Capture Rev 2 device. The default bitstream for the
CW1173 does not include the Serial IO-Line trigger function due to FPGA resource limitations. This may be
possible in future revisions with FPGA optimization, or if you remove other unneeded functions.

The following requires you to configure the target device to call the ``glitch1()`` routine. We'll use an
automatic digital pattern trigger, instead of the Trigger line. This will detect when the DUT sends the
``A`` character indicating the infinite loop is going to happen.

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
   
Glitching Onward
----------------

This basic tutorial has introduced you to glitch attacks. They are a powerful tool for bypassing authentication
in embedded hardware devices. There are many ways to expand your knowledge with additional practice, such as:

* Use manual glitches to try simply glitching past the prompt in ``glitch3()``.
* Download some example source code (bootloaders, login prompts, etc) and port them to the AVR. See how you can
  glitch past security checks.
  
  
  

  
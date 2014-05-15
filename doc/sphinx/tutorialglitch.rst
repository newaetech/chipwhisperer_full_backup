.. _tutorialglitch:

Tutorial: Glitch Attacks
=====================================

This advanced tutorial will demonstrate glitch attacks using the ChipWhisperer system.

Background on Glitching
-------------------------

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
------------------

The ChipWhisperer Glitch system uses the same synchronous methodology as it's Side Channel Analysis (SCA) capture. A system clock (which can come
from either the ChipWhisperer or the Device Under Test (DUT)) is used to generate the glitches. These glitches are then inserted back into the clock,
although it's possible to use the glitches alone for other purposes (i.e. for voltage glitching, EM glitching). 

The generation of glitches is done with two variable phase shift modules, configured as follows:

.. figure:: /images/tutorials/advanced/glitching/glitchgen-phaseshift.png

The enable line is used to determine when glitches are inserted. Glitches can be inserted continously (useful for development) or triggered by 
some event. The following figure shows how the glitch can be muxd to output to the Device Under Test (DUT).

.. figure:: /images/tutorials/advanced/glitching/glitchgen-mux.png

Hardware Support
^^^^^^^^^^^^^^^^^^^^

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
^^^^^^^^^^^^^^^^^^^^^^

The portion of the GUI of interest to us is primarily located in this section:

.. figure:: /images/tutorials/advanced/glitching/prgui.png

If the `Partial Reconfiguration` system has been disabled (due to missing PR files or files differing from the FPGA bitstream) the two fields
marked that say `(as % of period)` will be disabled. Only the fields labeled `(fine adjust)` will be available.

Setting up Glitch Example
---------------------------

Firmware Setup
^^^^^^^^^^^^^^^

The glitch examples requires you to program the AtMega328P. The software is located at ``chipwhisperer\hardware\victims\firmware\avr-glitch-examples``
of your ChipWhisperer release or GIT. You can build the software by running the make command as follows::

    make MCU=atmega328p
    
You should also open the file ``glitchexample.c`` which is the source code. The subroutine being glitchedin this example looks like this::

    void glitch1(void)
    {
        //Can monitor Port C, Pin 5 (top right pin on 28-DIP)
        DDRC |= 1<<5;
        PORTC = 1<<5;
        
        //Some fake variable
        volatile uint8_t a = 0;
        
        output_ch_0('A');
        
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

    int main
        (
        void
        )
        {
        init_uart0();
        
        /* For 2 MHz crystal use this hack */
        //BAUD0L_REG = 12;

        /* Uncomment this to get a HELLO message for debug */	
        output_ch_0('h');
        output_ch_0('e');
        output_ch_0('l');
        output_ch_0('l');
        output_ch_0('o');
        output_ch_0('\n');
        
            
        glitch1();
            
        return 1;
        }

One the AVR is programmed (see previous tutorials), you need to setup a few jumpers on the board.

Hardware Setup
^^^^^^^^^^^^^^^^^

The hardware is almost as in previous incarnations. The difference is the 'FPGAOUT' is bridged to the AVR clock. You can either choose to use the
7.37MHz clock on the multi-target board, or use the CLKGEN feature to generate a suitable 7.37MHz clock. This example will use the CLKGEN feature.

The AVR is being used as the glitch target. The following figure shows the expected jumper settings:

TODO

Software Setup
^^^^^^^^^^^^^^^^^

1. Connect to the ChipWhisperer device:

    1. As the *Scope Module*, select the *ChipWhisperer/OpenADC* option

    2. As the *Target Module*, select the *Simple Serial* option

    3. Switch to the *Target Settings* tab, and as the *connection*, select the *ChipWhisperer* option

    4. Run the *Download CW Firmware* tool. You should see the FPGA being programmed if required.

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

Manual Glitch Trigger
----------------------

To begin with, you'll simply use the manual glitch triggering. This works well in the examples where we have a simple loop we are breaking
out of. Doing so requires modifying the `glitch width` and `glitch offset` experimentally. The exact values will vary for every device and setup.

It is recommended to only use the *glitch width (as % of period)* option, as the fine adjust is too small of a change for this lower-speed
example. Other hardware may need the precision added by the fine adjust however!

To check if your glitches are working, simply hit the *Manual Trigger* button, which will insert a single glitch. You may find it useful to increase
the *Repeat* number to send several glitches at once. Ideally however we want a single glitch to cause the loop to break out. You know the loop
has broken out if you see additional information printed. The following figure shows several successul glitches:

    .. image:: /images/tutorials/advanced/glitching/glitchsimple-playaround.png
    
Be aware that you may crash the AVR! In the preceeding examples the AVR had reset after each glitch. It may simply go into another infinite loop
however, or even enter invalid states. Use the `Signature Read` option in AVRStudio to force a hardware reset of the AVR.

Automatic Glitch Triggering - Basic
------------------------------------



Automatic Glitch Triggering - Serial IO Line
----------------------------------------------





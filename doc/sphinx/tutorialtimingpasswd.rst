.. _tutorialtimingpasswd:

Tutorial #3: Timing Analysis with Power
===========================================

This tutorial will introduce you to breaking devices by determining when a device is performing certain operations. It will
break simple bootloaders which may otherwise seem 'secure' against password attacks. In particular this uses the excellent
`TinySafeBoot <http://jtxp.org/tech/tinysafeboot_en.htm>`_ bootloader for AVR microcontrollers. The example uses an old
release of TinySafeBoot, since newer releases do not have this vulnerability!

In addition this example shows you how to drive the ChipWhisperer software with a script, rather than using the GUI. This will
be required when attacking new devices which you have not yet added to the core ChipWhisperer software.

Note this is not a prerequisite to the tutorial on breaking AES. You can skip this tutorial if you wish.

Prerequisites
--------------

You should have already completed :ref:`tutorialtimingsimple` to gain a better understanding of the ChipWhisperer interface.

.. include:: tutorialbasic_setup.rsti

Finally program the microcontroller with the file used here:

7. Program the file ``tsb_m328p_d0d1_20140331.hex`` which is found at ``chipwhisperer\hardware\victims\firmware\tinysafeboot-20140331`` into
   the AVR microcontroller. You can find instructions for using the programming software in the ref:`tutorialcomms` examples.
 
   At this point we want to modify the system to check a provided password.
   To do so, open the file ``c:\chipwhisperer\user\timing\avr-serial-nocrypto\simpleserial_nocrypt.c`` with a text
   editor such as Programmer's Notepad (which ships with WinAVR).

Testing the Serial Connection
-------------------------------

These steps differ from previous steps, as we are not going to be using a built-in target. However you can refer to :ref:`tutorialcomms`
for general informationon using the ChipWhisperer-Capture Interface.

1. Start ChipWhisperer-Capture

2. As the *Scope Module*, select the *ChipWhisperer/OpenADC* option

3. As the *Target Module*, select the *Simple Serial* option

4. Switch to the *Target Settings* tab, and as the *connection*, select the *ChipWhisperer* option

5. Run the *Download CW Firmware* tool. You should see the FPGA being programmed if required.



6. Run the scope connect (click the button labeled *Scope: DIS*). Only the scope should switch to
   *CON* and be green circles. *Do not press the master button like last time.*:
   
   .. image:: /images/tutorials/basic/timingpower/connectscope_1.png
   
   When you are done the software should look as follows:
   
   .. image:: /images/tutorials/basic/timingpower/connectscope_2.png

7. From the *Tools* menu select *Open Terminal*, and press *Connect* on the terminal:

   .. image:: /images/tutorials/basic/timingpower/termconn.png

8. Switch back to the *Target Settings* tab, without closing the terminal window. Set the baud rate
   for both TX & RX to ``9600`` baud. Once you start using the terminal these values will switch to
   the actual baud rates in use (the hardware can only generate certain baud rates). You cannot use
   higher bauds for this tutorial as the combined error from the AVR code & ChipWhisperer serial port
   causes communications failures.
   
   .. image:: /images/tutorials/basic/timingpower/termbaud.png
     
9. In the *ChipWhisperer-Serial Terminal*, change the *TX on Enter* to *None*, as we don't want to
   send any character to terminate a string.
      
10. In the *ChipWhisperer-Serial Terminal*, check the *Show non-ASCII as hex* if not clicked.

    .. image:: /images/tutorials/basic/timingpower/term_settingssimple.png

11. Finally send the command ``@@@``, which is the login sequence for the TinySafeBoot bootloader. Simply type
    this in the input line, and press 'enter' to send. You will see the ``@@@`` echoed on the received data in 
    a blue font.
    
12. The objective is to get the login response. You may have to send ``@@@`` a few times for this to be successful,
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
    
    
13. Finally, we want to monitor power when sending this sequence to the device. We'll need to configure a number of OpenADC
    settings for this. The following table shows these settings, please carefully go though and set each of these as given.
    Pay attention to the 'notes' section which has some additional information.
    
    =============================  ================  =====================  ==============================================================
     Group                             Item               Value                   Note
    =============================  ================  =====================  ==============================================================
     Gain Setting                    Setting          40
     Trigger Setup                   Mode             falling edge
     ADC Clock                       Source           EXTCLK x1 via DCM      Will need to reset DCM later
     CW-Extra --> Trigger Pins       Front Panel A    Unchecked
     CW-Extra --> Trigger Pins       Target IO1       Checked                Only 'Target IO1 (Serial TXD)' should be checked
     CW-Extra                        Clock Source     Target IO-IN           Confirm 'Freq Counter' reads 7.37MHz in 'ADC Clock'
     ADC Clock                       Reset ADC DCM    Click Button           Confirm 'ADC Freq' is 7.37MHz, and 'DCM Locked' is checked
                                                                             after pressing button.
    =============================  ================  =====================  ==============================================================

14. Almost there!

Modifying the Target
------------------------

Background on Setup
^^^^^^^^^^^^^^^^^^^^

This tutorial is using an AtMega328p, which is an Atmel AVR device. We are comparing the power consumption of two different
instructions, the ``MUL`` (multiply) instruction and the ``NOP`` (no operation) instruction. Some information on these two
instructions:

mul
   * Multiples two 8-bit numbers together.
   * Takes 2 clock cycles to complete
   * Intuitively expect fairly large power consumption due to complexity of operation required
   
nop
   * Does nothing
   * Takes 1 clock cycle to complete
   * Intuitively expect low power consumption due to core doing nothing

Note that the capture clock is running at 4x the device clock. Thus a single ``mul`` instruction should span 8 samples on our
output graph, since it takes 4 samples to cover a complete clock cycle.

Initial Code
^^^^^^^^^^^^^

The initial code has a power signature something like this (yours will vary based on various physical considerations):

.. image:: /images/tutorials/basic/simplepower/cap_nop_mul.png

Note that the 10 ``mul`` instructions would be expected to take 80 samples to complete, and the 10 ``nop`` instructions should
take 40 samples to complete. By modifying the code we can determine exactly which portion of the trace is corresponding to
which operations.


Increase number of NOPs
^^^^^^^^^^^^^^^^^^^^^^^^^

We will then modify the code to have twenty NOP operations in a row instead of ten. The modified code
looks like this:

   .. code-block:: c

    /**********************************
     * Start user-specific code here. */
    trigger_high();
    
    asm volatile(
    "nop"       "\n\t"
    "nop"       "\n\t"
    "nop"       "\n\t"
    "nop"       "\n\t"
    "nop"       "\n\t"
    "nop"       "\n\t"
    "nop"       "\n\t"
    "nop"       "\n\t"
    "nop"       "\n\t"
    "nop"       "\n\t"
    ::
    );
    
    asm volatile(
    "nop"       "\n\t"
    "nop"       "\n\t"
    "nop"       "\n\t"
    "nop"       "\n\t"
    "nop"       "\n\t"
    "nop"       "\n\t"
    "nop"       "\n\t"
    "nop"       "\n\t"
    "nop"       "\n\t"
    "nop"       "\n\t"
    ::
    );
  
    asm volatile(
    "mul r0,r1" "\n\t"
    "mul r0,r1" "\n\t"
    "mul r0,r1" "\n\t"
    "mul r0,r1" "\n\t"
    "mul r0,r1" "\n\t"
    "mul r0,r1" "\n\t"
    "mul r0,r1" "\n\t"
    "mul r0,r1" "\n\t"          
    "mul r0,r1" "\n\t"
    "mul r0,r1" "\n\t"
    ::
    );

    trigger_low();
    /* End user-specific code here. *
     ********************************/

Note that the ``mul`` operation takes 2 clock cycles on the AVR, and the ``nop`` operation takes 1 clock cycles. Thus we expect
to now see two areas of the power trace which appear to take approximately the same time. The resulting power trace looks like this:

.. image:: /images/tutorials/basic/simplepower/cap_doublenop_mul.png

Pay particular attention to the section between sample number 0 & sample number 180. It is in this section we can compare the two
power graphs to see the modified code. We can actually 'see' the change in operation of the device! It would appear the ``nop`` is 
occuring from approximately 10-90, and the ``mul`` occuring from 90-170. 
    
Add NOP loop after MUL
^^^^^^^^^^^^^^^^^^^^^^^^^

Finally, we will add 10 more NOPs after the 10 MULs. The code should look something like this:

   .. code-block:: c

    /**********************************
     * Start user-specific code here. */
    trigger_high();
    
    asm volatile(
    "nop"       "\n\t"
    "nop"       "\n\t"
    "nop"       "\n\t"
    "nop"       "\n\t"
    "nop"       "\n\t"
    "nop"       "\n\t"
    "nop"       "\n\t"
    "nop"       "\n\t"
    "nop"       "\n\t"
    "nop"       "\n\t"
    ::
    );
    
    asm volatile(
    "nop"       "\n\t"
    "nop"       "\n\t"
    "nop"       "\n\t"
    "nop"       "\n\t"
    "nop"       "\n\t"
    "nop"       "\n\t"
    "nop"       "\n\t"
    "nop"       "\n\t"
    "nop"       "\n\t"
    "nop"       "\n\t"
    ::
    );
  
    asm volatile(
    "mul r0,r1" "\n\t"
    "mul r0,r1" "\n\t"
    "mul r0,r1" "\n\t"
    "mul r0,r1" "\n\t"
    "mul r0,r1" "\n\t"
    "mul r0,r1" "\n\t"
    "mul r0,r1" "\n\t"
    "mul r0,r1" "\n\t"          
    "mul r0,r1" "\n\t"
    "mul r0,r1" "\n\t"
    ::
    );

    asm volatile(
    "nop"       "\n\t"
    "nop"       "\n\t"
    "nop"       "\n\t"
    "nop"       "\n\t"
    "nop"       "\n\t"
    "nop"       "\n\t"
    "nop"       "\n\t"
    "nop"       "\n\t"
    "nop"       "\n\t"
    "nop"       "\n\t"
    ::
    );

    trigger_low();
    /* End user-specific code here. *
     ********************************/
    
With an output graph that looks like this:

  .. image:: /images/tutorials/basic/simplepower/cap_doublenop_mul_nop.png
    
Comparison of All Three
^^^^^^^^^^^^^^^^^^^^^^^^^

The following graph lines the three options up. One can see where adding loops of different operations shows up in the power
consumption.

  .. image:: /images/tutorials/basic/simplepower/nop_mul_comparison.png
    
Clock Phase Adjustment
----------------------------
    
A final area of interest is the clock phase adjustment. The clock phase adjustment is used to shift the ADC sample clock from the
actual device clock by small amounts. This will affect the appearance of the captured waveform, and in more advanced methods is 
used to improve the measurement.

The phase adjustment is found under the *Phase Adjust* option of the *ADC Clock* setting:

  .. image:: /images/tutorials/basic/simplepower/phasesetting.png
  
To see the effect this has, first consider an image of the power measured by a regular oscilloscope (at 1.25GS/s):

  .. image:: /images/tutorials/basic/simplepower/scope_real.png
  
And the resulting waveforms for a variety of different phase shift settings:
  
.. image:: /images/tutorials/basic/simplepower/phase_differences.png 
   
The specifics of the capture are highly dependant on each ChipWhisperer board & target platform. The phase shift allows customization
of the capture waveform for optimum performance, however what constitutes 'optimum performance' is highly dependant on the specifics
of your algorithm.
   
Conclusion
---------------

In this tutorial you have learned how power analysis can tell you the operations being performed on a microcontroller. In future work
we will move towards using this for breaking various forms of security on devices.



.. _tutorialtimingsimple:

Tutorial #B2: Viewing Instruction Power Differences
==================================================

This tutorial will introduce you to measuring the power consumption of a device under attack. It will demonstrate
how you can view the difference between a 'add' instruction and a 'mul' instruction.

Prerequisites
-------------

You should have already completed :ref:`tutorialcomms`. This tutorial assumes you are capable of building a new
AVR/XMEGA code, programming the code, and connecting to the ChipWhisperer.

Setting Up the Example
----------------------
  
1. Copy the directory ``simpleserial-base`` which is found at ``chipwhisperer\hardware\victims\firmware\`` of the 
   chipwhisperer release to a new directory called ``simpleserial-base-lab2``. You must keep it in the same directory, as
   it will reference other files within that directory for the build process.

   If you just completed :ref:`tutorialcomms`, you can simply reuse that code (this builds upon it).
 
   At this point we want to modify the system to perform a number of operations. We won't actually use the input data.
   To do so, open the file ``simpleserial-base.c`` with a text
   editor such as Programmer's Notepad (which ships with WinAVR).

2. Find the following code block towards the end of the file, which may look different if you just completed :ref:`tutorialcomms`.

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

4. Change the terminal to the directory with your source, and run ``make`` to build the system.
   Remember you can press the up arrow on the keyboard to get recently typed commands in most OSes::

    make
    
   Which should have the following output, you will **want to check the build platform is what you expect**::

    ...Bunch of lines removed...
    Creating Extended Listing: simpleserial-base.lss
    avr-objdump -h -S -z simpleserial-base.elf > simpleserial-base.lss
    
    Creating Symbol Table: simpleserial-base.sym
    avr-nm -n simpleserial-base.elf > simpleserial-base.sym
    
    Size after:
    AVR Memory Usage
    ----------------
    Device: atxmega128d3
    
    Program:    1568 bytes (1.1% Full)
    (.text + .data + .bootloader)
    
    Data:        224 bytes (2.7% Full)
    (.data + .bss + .noinit)
    
    
    Built for platform CW-Lite XMEGA
    
    -------- end --------

5.  Following the instructions given in :ref:`tutorialcomms`, program the AVR or XMEGA with your new code. Note you __do not__
    need to close the programming window. If you will be doing frequent modifications to the source code,
    this can simplify your life since you only need to hit the **Program** button in AVRStudio to download new code.
  
6.  Ensure the hardware is setup as in :ref:`tutorialcomms`, if using the CW1002 ensure the SMA cable is also connected as described
    in the previous tutorial.
 
Capturing Power Traces
----------------------

The basic steps to connect to the ChipWhisperer device are described in :ref:`tutorialcomms`. They are repeated here
as well, however see :ref:`tutorialcomms` for pictures & mode details.

1. Start ChipWhisperer-Capture

2. As the *Scope Module*, select the *ChipWhisperer/OpenADC* option.

3. As the *Target Module*, select the *Simple Serial* option.

4. Switch to the *Target Settings* tab, and as the *connection*, select the *ChipWhisperer* (for CW1002) or *ChipWhisperer-Lite* (for CW1173 or CW1180).

5. Switch to the *Scope Settings* tab, and ensure the *connection* is set to either *ChipWhisperer* (for CW1002) or *ChipWhisperer-Lite* (for CW1173 or CW1180).

5. Run the master connect (click the button labeled *Master: DIS*). Both the Target & Scope should switch to
   *CON* and be green circles.
 
6. For the CW1173/CW1180 (ChipWhisperer-Lite based systems), perform the following:

    a. Set the *CLKGEN* frequency to *7.37 MHz*
    b. Set the*Target HS-IO Out* as *CLKGEN* 
 
7. If targetting an XMEGA board (either the ChipWhisperer-Lite XMEGA default target, or the XMEGA on the multi-target board), perform the following:

   a. Set *Target IO1* as *Serial RXD*
   b. Set *Target IO2* as *Serial TXD*
    
8. Switch to the *General Settings* tab, and hit the *Open Monitor* button.

9. Hit the *Run 1* button. You may have to hit it a few times, as the very first serial data is often lost. You should see
   data populate in the *Text Out* field of the monitor window. The *Text In* and *Text Out* aren't actually used in this example,
   so you can close the *Monitor* dialog.

At this point you've completed the same amount of information as the previous tutorial. The following section describes how
to setup the analog capture hardware, which is new (to you). The following is entirely done in the *Scope Settings* tab:

.. image:: /images/tutorials/basic/simplepower/cap1.png

9. Under *Trigger Setup* set the *Mode* to *rising edge*. This means the system will trigger on a rising edge logic level:

.. image:: /images/tutorials/basic/simplepower/cap2.png

10. For the CW1002 (ChipWhisperer Capture Rev 2) hardware only, perform the following:

   a. Under the *Trigger Pins* unselect the *Front Panel A* as an option, and select *Target IO4 (Trigger Line)*. This will
      mean only the trigger pin coming from the AVR target is used to trigger the capture.

   b. In the same area, select the *Clock Source* as being from *Target IO-IN*

      .. image:: /images/tutorials/basic/simplepower/cap3.png
      
   c. You can monitor the *Freq Counter* option, which measures the frequency being used on the *EXTCLK* input. This should
      be 7.37 MHz, which is the oscillator on the multi-target board.
    
   d. Change the *ADC Clock* *source* as being *EXTCLK x4 via DCM*. This routes the external clock through a 4x multiplier,
      and routes it to the ADC.
    
11. For the CW1173/CW1180 (ChipWhisperer-Lite based hardware), perform the following:

   d. Change the *ADC Clock* *source* as being *CLKGEN x4 via DCM*. This routes the device clock through a 4x multiplier,
      and routes it to the ADC. 
    
12. Hit the **Reset ADC DCM** button.
    
.. image:: /images/tutorials/basic/simplepower/cap5.png

13. The *ADC Freq* should show 29.5 MHz (which is 4x 7.37 MHz), and the *DCM Locked* checkbox __MUST__ be checked. If the
    *DCM Locked* checkbox is NOT checked, try hitting the *Reset ADC DCM* button again.
    
14. At this point you can hit the *Capture 1* button, and see if the system works! You should end up with a window looking 
    like this:
    
    .. image:: /images/tutorials/basic/simplepower/cap6.png
    
    Whilst there is a waveform, you need to adjust the capture settings. There are two main settings of importance, the
    analog gain and number of samples to capture.
    
.. image:: /images/tutorials/basic/simplepower/cap7.png
    
15. Under *Gain Setting* set the *Mode* to *high*. Increase the *Gain Setting* to about 25. You'll be able to adjust this
    further during experimentations, you may need to increase this depending on your hardware and target device. For the 
    multi-target board with the CW1002 you will probably need to set this around 40 for example.
    
16. Under *Trigger Setup* set the *Total Samples* to *500*. 

17. Try a few more *Capture 1* traces, and you should see a 'zoomed-in' waveform.

Modifying the Target
--------------------

Background on Setup
^^^^^^^^^^^^^^^^^^^

This tutorial is using either an AtMega328p which is an Atmel AVR device, or AtXMEGA128D4 which is an Atmel XMEGA device.
We are comparing the power consumption of two different instructions, the ``MUL`` (multiply) instruction and the ``NOP``
(no operation) instruction. Some information on these two instructions:

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
^^^^^^^^^^^^

The initial code has a power signature something like this (yours will vary based on various physical considerations, and depending
if you are using an XMEGA or AVR device):

.. image:: /images/tutorials/basic/simplepower/cap_nop_mul.png

Note that the 10 ``mul`` instructions would be expected to take 80 samples to complete, and the 10 ``nop`` instructions should
take 40 samples to complete. By modifying the code we can determine exactly which portion of the trace is corresponding to
which operations.


Increase number of NOPs
^^^^^^^^^^^^^^^^^^^^^^^

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
^^^^^^^^^^^^^^^^^^^^^^

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
^^^^^^^^^^^^^^^^^^^^^^^

The following graph lines the three options up. One can see where adding loops of different operations shows up in the power
consumption.

  .. image:: /images/tutorials/basic/simplepower/nop_mul_comparison.png
    
Clock Phase Adjustment
----------------------
    
A final area of interest is the clock phase adjustment. The clock phase adjustment is used to shift the ADC sample clock from the
actual device clock by small amounts. This will affect the appearance of the captured waveform, and in more advanced methods is 
used to improve the measurement.

The phase adjustment is found under the *Phase Adjust* option of the *ADC Clock* setting:

  .. image:: /images/tutorials/basic/simplepower/phasesetting.png
  
To see the effect this has, first consider an image of the power measured by a regular oscilloscope (at 1.25GS/s):

  .. image:: /images/tutorials/basic/simplepower/scope_real.png
  
And the resulting waveforms for a variety of different phase shift settings:
  
.. image:: /images/tutorials/basic/simplepower/phase_differences.png 
   
The specifics of the capture are highly dependent on each ChipWhisperer board & target platform. The phase shift allows customization
of the capture waveform for optimum performance, however what constitutes 'optimum performance' is highly dependent on the specifics
of your algorithm.
   
Conclusion
----------

In this tutorial you have learned how power analysis can tell you the operations being performed on a microcontroller. In future work
we will move towards using this for breaking various forms of security on devices.



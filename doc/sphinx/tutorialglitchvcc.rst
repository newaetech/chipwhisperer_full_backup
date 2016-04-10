.. _tutorialglitchvcc:

Tutorial #A3: VCC Glitch Attacks
================================

This advanced tutorial will demonstrate power glitch attacks using the ChipWhisperer system.

You can follow along with this video for details of this page too:

|YouTubeVccGlitch|_

.. |YouTubeVccGlitch| image:: /images/tutorials/advanced/vccglitching/youtube-vccglitch.png
.. _YouTubeVccGlitch: http://www.youtube.com/watch?v=hxU5uVbYCYo&hd=1


Background on VCC (Power) Glitching
-----------------------------------

The previous clock glitching tutorials looked into the assumption of a constant clock. But instead we can modify
the voltage of the device, causing for example a failure to correctly read a memory location or otherwise cause
havoc with the proper functioning.

.. figure:: /images/tutorials/advanced/vccglitching/vccglitch_working.png

    Top trace is the VCC (power) measured at the microcontroller pin, lower trace is the
    clock to the device.

.. figure:: /images/tutorials/advanced/vccglitching/vccglitch_working_zoom.png

    A zoom in of the previous figure, showing the offset between the clock and the glitches.

.. figure:: /images/tutorials/advanced/vccglitching/vccglitch_notworking_zoom.png

    Changing the offset means the glitches are ineffective - this requries considerable experimentation
    to discover the value for offset, glitch width, and number of glitches.

Background on Glitch Generation
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

For more details, please see :ref:`tutorialglitch`, this tutorials assumes you have already performed the clock
glitching tutorial. This tutorial will use the *Glitch Explorer*, which is described in the previous tutorial.

The glitch generation hardware is the same as used in the clock glitching attack. The generated glitches are
synchronous to the device clock, and inserted at a precise offset from the clock edge.

Glitches can be inserted continuously or triggered by some event. The following figure shows the generation
of two glitches:

.. figure:: /images/tutorials/advanced/vccglitching/glitchgen-mux-glitchonly.png

The VCC glitching method here uses an electronic switch (a MOSFET) to short the power line to GND at specific
instances. The following figure shows the basic function of this system:

.. figure:: /images/tutorials/advanced/vccglitching/glitch-vccglitcher.png

This method allows use with the standard side-channel analysis development board, which has resistors inserted into the
VCC lines already. The downside of this method is that it can only generate short glitches, since the power consumption
through the shunt resistor will short out the resistor.

The MOSFET glitching hardware is built into the ChipWhisperer-Lite (both CW1173 and CW1180) board. The ChipWhisperer-Capture
Rev2 uses an external VCC glitching board.

Hardware Setup
--------------

ChipWhisperer-Lite (CW1173) with built-in XMEGA Target
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The XMEGA target is currently not supported for this tutorial. The hardware is capable of being glitched,
but we are still working on improving the reliability of the attack on the XMEGA. This section will be
updated when details are available.

ChipWhisperer-Lite (CW1173/CW1180) with external AVR (NOTDUino/Multi-Target)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The AVR is an extremely reliable target to glitch. To do this, you need to connect the following cables:

1. SMA Cable from the *Glitch* port to the VCC shunt.

2. 20-Pin Target Cable for Clock & Data.

3. Optional: SMA Cable from the *Measure* port to the VCC shunt (can be used to monitor glitch insertion).

The following shows an example of connecting the NOTDuino target to the ChipWhisperer-Lite:

   .. image:: /images/tutorials/advanced/vccglitching/notduino_cwlite.jpg

If using a target with only a single SMA, only connect the *Glitch* port. The measure port is optional to
allow you to monitor the VCC line as you are inserting the glitch.

If using the ChipWhisperer-Lite with Multi-Target board, you can connect both ports by doing the following:

    a. Connect the *Glitch* port to the *VOUT* pin.

    b. Connect the *Measure* port to the *LNAIN* pin.

    c. Add the jumper across JP21 to route the *VOUT* to the *LNAIN*. 

Using VCC Glitching Add-on with Multi-Target Board
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

If using the ChipWhisperer Capture Rev2 (CW1002), you do not have a glitch output port. Instead you need the
VCC glitching add-on.

The VCC glitching add-on can be added to the AVR or SmartCard 6-pin headers, or connected to the XMEGA by a few additional
jumper wires. An example of the connection is shown in the :ref:`vccglitchhwsetup` section.

The connection between the glitch input and FPGAOUT pin must be done via a jumper wire. This connects the glitch module output
to the MOSFET input, such that whenever a glitch is requested the VCC on the target is shorted.

Note that the VCC glitcher is **capable of being damaged** or **causing damage to** the multi-target board. In particular the
shunt resistor mounted on the VCC line (e.g. R1 for the AVR) is rated only for 0.1 watts. Shorting a 3.3V VCC to GND however
causes 0.2 watts to be dissipated. Provided the glitch is only short-lived (as the glitches are for this system), the total
average dissipation will not cause damage. If however the glitch module accidently caused a continous short of the VCC to
GND, the power dissipation of the resistor may cause it to burn out.

The hardware is almost as in previous incarnations. Note that unlike the :ref:`tutorialglitch` you do not connect the FPGAOUT
to the AVR Clock, instead you use the 7.37 MHz oscillator bridged to both the AVR clock and FPGAIN. This is the
*same clock setup as in the side-channel power analysis tutorials*.

The following figure shows the clock jumper configuration, which are the same as the settings from :ref:`tutorialaes`.

    .. image:: /images/tutorials/basic/aes/hw-2.jpg
       :width: 600 px

In addition the VCC glitcher board must be mounted, which means removing the jumpers on the 6-pin header around the
AVR. Also the 'IN' pin on the VCC glitcher board connected to the FPGAOUT pin. This is done via a jumper wire. Both
of these are shown in the following figure:

   .. figure:: /images/tutorials/advanced/vccglitching/vccglitcher_routing.jpg

    The control line for the VCC glitcher board comes from the FPGAOUT pin.

   .. figure:: /images/tutorials/advanced/vccglitching/vccglitcher_mounted.jpg

    The VCC glitcher board must be mounted with ``GND`` at the bottom matching, exactly as shown here.


Setting up Glitch Example
-------------------------

Firmware Setup
^^^^^^^^^^^^^^

Just as in the clock glitching example, you will be required to program the AVR microcontroller with an example you can glitch.
Once again program in the glitch example code to use the ``glitch1()`` function, as described in :ref:`tutorialglitch`.

If using the AVR target, be sure to modify the ``makefile`` to select the new target type (i.e. if you previously
targeted the XMEGA, that hex-file will not work on the NOTDuino).

Programming the device is also described in Step #4 in the following section.

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
       generating about a 7.37 MHz clock.

    4. Under the *Glitch Module* set the *Clock Source* as *CLKGEN*:
    
       .. image:: /images/tutorials/advanced/glitching/glitchgen-clkgen.png

    5. Set the *Target HS IO-Out* appropriately. This depends on the hardware in use:
    
        a. For the ChipWhisperer-Lite (CW1173/CW1180), set *Target HS IO-Out* option to *CLKGEN*.
        
        b. For the ChipWhisperer-Capture Rev 2 (CW1002), set *Target HS IO-Out* option to *Glitch Module*.

3. Connect the Serial Port

    1. For the XMEGA Target (including the CW-Lite integrated target), perform the following:
    
       a. Switch to the *Scope Settings* tab, and scroll down to *Target IOn Pins*
       b. Switch the *Target IO1* to be *Serial RXD*
       c. Switch the *Target IO2* to be *Serial TXD*       
    
    2. From the *Tools* menu select *Open Terminal*, and press *Connect* on the terminal:

        .. image:: /images/tutorials/basic/timingpower/termconn.png

    3. The baud rate for this system is 38400, which should be the default for the ChipWhisperer serial port. 

4. Program the ``.hex`` file into the target. When programming the AVR, it needs a working clock source
   before the programming will succeed. Now that this is enabled, you can use the appropriate programmer
   (such as the AVR or XMEGA programming from the *Tools* menu if using a CW-Lite) to program the hex file.
   
   You should see the "Hello" message printed on the terminal emulator window.
   
   .. warning::
   
        Releases of the ChipWhisperer-Capture software prior to 0.13 had a bug in the AVR reset logic, which
        never released the device from reset. You must update your ChipWhisperer-Capture release in order
        to complete this tutorial. 

5. Setup the Glitch Module to NOT output anything by default **VERY IMPORTANT TO AVOID DAMAGE**

    1. Under the *Glitch Module* set the *Output Mode* as *Glitch Only*, this is the step
       that insurances **you do not cause constant glitches**:

        .. image:: /images/tutorials/advanced/vccglitching/glitchexample-capsetup1.png

    2. For the ChipWhisperer-Lite (CW1173 or CW1180):
    
        a. Under the *Glitch Module* set the *Clock Source* as *CLKGEN*
        
            .. image:: /images/tutorials/advanced/vccglitching/glitchexample-capsetupclkgen.png
        
        b. Check the box marked *HS-Glitch Out Enable (Low Power)*:
        
            .. image:: /images/tutorials/advanced/vccglitching/glitchexample-capsetup2b.png
    
    
    3. For the ChipWhisperer-Capture Rev 2 (CW1002):
    
        a. Under the *Glitch Module* set the *Clock Source* as *TargetIO-IN*:
    
            .. image:: /images/tutorials/advanced/vccglitching/glitchexample-capsetup2.png
    
        b. Under the *Target HS IO-Out* option select the *Glitch Module*, ensuring you've already set the
           *Output Mode* as *Glitch Only* under *Glitch Module*:
    
           .. image:: /images/tutorials/advanced/glitching/targioout.png
           
6. Switch the *Glitch Trigger* mode to *Ext Trigger:Single-Shot*:

   .. image:: /images/tutorials/advanced/glitching/singleshot.png
    
7. Switch to the *General Settings* tab, and select the appropriate "Auxiliary Module":

   a. For ChipWhisperer-Lite (CW1173/CW1180), select "Reset AVR/XMEGA via CW-Lite"
   
   b. For ChipWhisperer-Capture Rev 2, select "Reset AVR via ISP-MKII"
   
   .. image:: /images/tutorials/advanced/glitching/glitching_aux_1.png

8. Switch to the *Aux Settings* tab. Depending on your module you will see different settings here, for
   example the ChipWhisperer-Lite lets you select between AVR and XMEGA targets. This normally defaults
   to the "XMEGA" device, so be sure to switch this to the "AVR" device if using the NOTDuino or
   Multi-Target boards!
   
9. Press the *Test Reset* button in the *Aux Settings* tab. This should reset the AVR target. Confirm this by monitoring
   the terminal emulator window, and check the startup message (of "hello") is printed again.
   
   You can also use the "Reset" button on the NOTDuino to confirm the startup message is printed.
   
10. Switch to the *Target Settings* tab, and remove all the text in the *Load Key Command*, *Go Command*, and *Output Format*
    fields.

Optionally, you can also configure the power measurement setup too:

Monitoring Glitch Insertion
^^^^^^^^^^^^^^^^^^^^^^^^^^^

We can optionally enable the power analysis capture, and monitor how the power consumption changes
as we insert a glitch. To do this:

1. Switch to the *Scope Settings* tab.

2. Switch the *ADC Clock Source* as being *CLKGEN x4*.

3. Press *Reset ADC DCM*, confirm the frequency is 29.5 MHz as expected.

4. Switch the *Trigger Setup* --> *Mode* to be *Rising Edge*

5. Switch the *Trigger Setup* --> *Total Samples* to be *1000*

6. Switch the *Gain Setting* --> *Setting* to be *40*. You might need to adjust this for different hardware.

7. Press *Capture 1*, confirm some waveform is displayed. For example with the NOTDuino Target on the
   ChipWhisperer-Lite, the waveform looks like this:
   
   .. image:: /images/tutorials/advanced/vccglitching/waveform-notduino-normal.png
   
8. If this does't work: check the trigger in use is the *Target IO4* pin.

9. Play around a bit with the glitch width, offset, and repeat. You should see different
   effects in the power consumption traces. For example the following shows a narrow
   (15% pulse width) glitch being inserted:
   
   .. image:: /images/tutorials/advanced/vccglitching/waveform-notduino-glitch1.png


Starting the Glitch Attack
^^^^^^^^^^^^^^^^^^^^^^^^^^

We'll now look at glitching this routine. As before after sending the ``A`` the system goes into
an infinite loop, and sends ``1234`` after exiting from the loop. Using VCC glitching we'll escape
from this loop!

Rather than using the manual trigger, we'll jump right into using the Glitch Explorer to break this target. First, we'll setup
some basic glitch parameters for your specific target.

1. Switch to the *Target Settings* tab, and set the *Output Format* to ``$GLITCH$``.

2. Open the *Glitch Explorer*, and hit *Capture 1* a few times. Confirm this populates the table with various examples.

   .. image:: /images/tutorials/advanced/vccglitching/ge-normal.png
   
3. We need to setup the *Normal Response* and *Successful Response*. Note in this example the normal response has a little
   random noise we want to ignore, but we want to capture when the device resets after the glitch and sends the "hello"
   message twice. We could accomplish this with the following bit of Python code::
   
    s.endswith("hello\nA") and (len(s) < 12)

   This looks for both the ending without glitch, and the length of the string isn't too long. In the case of the successful
   glitch, we just want to see if "1234" is printed. This can be accomplished in Python with::
   
    "1234" in s
    
   You can always experiment using the *Python Console* to see how your potential systems work. For example here is checking
   that the first line works::
   
    >>> s = "\x1ahello\nA"
    >>> s.endswith("hello\nA") and (len(s) < 12)
    True
    >>> s = "\x1ahello\nAhello\nA"
    >>> s.endswith("hello\nA") and (len(s) < 12)
    False
    
   Finally, configure the Glitch Explorer:
   
    a. Set the *Normal Response* to ``s.endswith("hello\nA") and (len(s) < 12)``
    b. Set the *Successful Response* to ``"1234" in s``

   You can test the updated color-coding seems to be working too with a few *Capture 1* events.

4. Using the following table, set the *Glitch Width (as % of period)* and *Repeat* on the *Scope Settings* tab:

    ============================== ===============================
    Parameter                      AVR on Multi-Target or NOTDuino
    ============================== ===============================
    Glitch Width (as % of period)  49                                          
    Repeat                         10                        
    ============================== ===============================

5. Finally, let's configure the Glitch Explorer to give us the required sweep of the *Offset* parameter.

   a. Adjust the *Glitch Offset (as % of period)* up or down by 1 in the *Glitch Module* settings. We do this
      only to get the required string printed to the *Script Commands* output.
      
   b. Set the *Tuning Parameters* to *1* in the Glitch Explorer.
   
   c. Set the parameters as appropriate:
   
        ============== ========================================================
        Option          Value
        ============== ========================================================
        Name           Offset
        Script Command ['Glitch Module', 'Glitch Offset (as % of period)', 0.0]  
        Data Format    Float
        Range          -49 : 49
        Value          -49
        Step           0.5
        Repeat         1
        ============== ========================================================

6. On the *General Settings* tab:

    a. Ensure the *Trace Format* is set to *None* (i.e., no traces will be written to disk).
    b. Set the *Number of Traces* to 200.
    
7. Press the *Capture Multi* button. You will get a warning as there is no trace writer, but can just
   hit *Continue Anyway*, since we do not want to store traces to disk.
   
8. Hopefully you will determine some useful parameters for glitching this target:

    .. image:: /images/tutorials/advanced/vccglitching/ge-success.png

9. Try reducing the *Repeat* parameter in the *Glitch Module* settings. See how few cycles you can glitch
   while still achieving a reliable glitch.
   
Once you have the glitch parameter determined, you can work on trying to recreate some of the previous
tutorials such as glitching passed the password prompt.

Glitching More Advanced Targets: Raspberry Pi
---------------------------------------------

It is also possible to glitch more advanced targets, such as the Raspberry Pi
development board! This requires some additional hardware setup which will be
discussed here.

The Raspberry Pi is a small ARM-based computer that runs Linux. This tutorial
will show you how to influence a program running in userland via voltage
glitching.

We will use the ChipWhisperer-Lite board, as it has integrated high-power glitching
MOSFET.

Hardware Setup
^^^^^^^^^^^^^^

.. warning::

    This tutorial can cause permanent damage to your Raspberry Pi board.
    The generation of glitches means driving the power supply and device beyond
    limits specified in the absolute maximum ratings. Only perform this tutorial
    if you are not too attached to your Raspberry Pi board.

    YOU PERFORM THIS TUTORIAL AT YOUR OWN RISK. NEWAE TECHNOLOGY INC. IS NOT
    RESPONSIBLE FOR DAMAGE CAUSED BY FOLLOWING THIS TUTORIAL.

To glitch the board, you must solder a wire onto the *VDD_CORE* power supply,
ideally as close to the BGA power pin as possible. To do this identify the
power plane by looking at the schematic:

  .. image:: /images/tutorials/advanced/vccglitching/rpi_schematic.png

And then solder a wire onto the VCC side of a decoupling capacitor, such
as C65. Check the polarity with a DMM to ensure you have the positive side and
solder a fine wire to it.

    .. image:: /images/tutorials/advanced/vccglitching/rpi_solder1.jpg   
        :width: 400
    
    .. image:: /images/tutorials/advanced/vccglitching/rpi_solder2.jpg    
        :width: 400

We will now mount a connector so we can connect this to the ChipWhisperer-Lite Glitch
port. This will require you to check your specific revision - on this board an empty hole
(test point) labeled "TP2" connects to ground, and made a handy location to connect
the SMA connector to ground.

The following shows an example of soldering the SMA connector onto the board, note the GND
is soldered on both top and bottom to give additional strength:

.. image:: /images/tutorials/advanced/vccglitching/rpi_sma.jpg
    :width: 400

The positive side of the capacitor connects to the inner conductor of
the SMA "GLITCH" port, and connect the outer connector to ground on the Raspberry Pi. At this
point do not yet plug into the GLITCH port, we will do that once setup is complete.

Finally you need to boot the Raspberry Pi and connect to it. This is suggested to be done
with a SSH shell over the Ethernet connection, as the Ethernet connection typically has very
good protection against voltage transients. If you connect the Raspberry Pi to a monitor over
HDMI, there is a chance the glitches may cause invalid voltage levels on the HDMI port which
could damage your monitor.

Once you have connected to it, simply make a file called ``glitch.c`` with the following contents::

    #include <stdio.h>
    
    int main(void){
        int i,j,k,cnt;
        k = 0;
        while(1){
         cnt = 0;
         for(i=0; i<5000; i++){
           for(j=0; j<5000; j++){
              cnt++;
           }
         }
         printf("%d %d %d %d\n", cnt, i, j,k++);
        }
    }

Compile to an executable with::

    $ gcc glitch.c -o glitch

And run the executable::

    $ ./glitch
    25000000 5000 5000 0
    25000000 5000 5000 1
    25000000 5000 5000 2
    25000000 5000 5000 3
    25000000 5000 5000 4
    25000000 5000 5000 5
    
The output is split into two parts. The first three are used to monitor the glitch insertion
(this is the ``25000000 5000 5000``, the second makes it easier for you to confirm if the Raspberry Pi
has crashed.

Now that you have a working system - let's break it!

Glitch Parameters
^^^^^^^^^^^^^^^^^

Glitching the Raspberry Pi is very simple. We just need to generate an appropriately sized glitch,
as the following shows:

1. Start ChipWhisperer-Capture.

2. Set the *Scope Module* to *ChipWhisperer/OpenADC*, and the *connection* to *ChipWhisperer-Lite*.

3. Hit the *Scope Connect* button. There is no target for this example.

4. Set the CLKGEN frequency to *120 MHz*.

5. Set the Glitch module Source to *CLKGEN*.

6. Set the Glitch Mode to *Enable Only*.

7. Ensure the *Glitch Trigger* is *Manual*.

8. Set the *Repeat* to *38*.

9. Click the *HS-Glitch Out Enable (High Power)* check-box.

10. Connect the SMA cable for the glitch output to the Raspberry Pi.

11. With the output of the *glitch* program running, hit the *Manual Trigger* button. This will cause
    a glitch to be inserted, and observe the output of your glitch program.
    
    Most likely the glitch width was insufficient for a glitch to be inserted, so increase the *Repeat*
    count to increase the width, and try pressing the *Manual Trigger* button again. In this example
    a glitch was successfully inserted with a width of *52*, so you might want to try a few larger
    numbers. If you do things wrong your Raspberry Pi will crash and you'll need to reboot it and
    continue experimenting.
    
    The following shows an example of inserting several glitches successfully:
    
    .. image:: /images/tutorials/advanced/vccglitching/rpi-glitch.png




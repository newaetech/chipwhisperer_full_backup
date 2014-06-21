.. _tutorialglitchvcc:

Tutorial #9: VCC Glitch Attacks
=====================================

This advanced tutorial will demonstrate power glitch attacks using the ChipWhisperer system. 

Background on VCC (Power) Glitching
-----------------------------------------

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
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

For more details, please see :ref:`tutorialglitch`, this tutorials assumes you have already read (at least) or
preformed (ideally) the clock glitching tutorial.

A brief overview of the glitch generation hardware will be repeated here. The ChipWhisperer Glitch system uses the same
synchronous methodology as it's Side Channel Analysis (SCA) capture. The generated glitches are synchronous to the device
clock, and inserted at a precise offset from the clock edge.

The generation of glitches is done with two variable phase shift modules, configured as follows:

.. figure:: /images/tutorials/advanced/glitching/glitchgen-phaseshift.png

The enable line is used to determine when glitches are inserted. Glitches can be inserted continuously or triggered by 
some event. The following figure shows the generation of two glitches:

.. figure:: /images/tutorials/advanced/vccglitching/glitchgen-mux-glitchonly.png

The VCC glitching add-on uses an electronic switch (a MOSFET) to short the power line to GND at specific instances. The
following figure shows the basic function of this system:

.. figure:: /images/tutorials/advanced/vccglitching/glitch-vccglitcher.png

This method allows use with the standard side-channel analysis development board, which has resistors inserted into the
VCC lines already. The downside of this method is that it can only generate short glitches, since the power consumption
through the shunt resistor will short out the resistor.

The VCC Glitching Add-On
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The VCC glitching add-on can be added to the AVR or SmartCard 6-pin headers, or connected to the XMEGA by a few additional
jumper wires. An example of the connection is shown in the :ref:`vccglitchhwsetup` section.

The connection between the glitch input and FPGAOUT pin must be done via a jumper wire. This connects the glitch module output
to the MOSFET input, such that whenever a glitch is requested the VCC on the target is shorted.

Note that the VCC glitcher is **capable of being damaged** or **causing damage to** the multi-target board. In particular the
shunt resistor mounted on the VCC line (e.g. R1 for the AVR) is rated only for 0.1 watts. Shorting a 3.3V VCC to GND however
causes 0.2 watts to be dissipated. Provided the glitch is only short-lived (as the glitches are for this system), the total
average dissipation will not cause damage. If however the glitch module accidently caused a continous short of the VCC to
GND, the power dissipation of the resistor may cause it to burn out.


Adjustable VCC Glitches
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Setting up Glitch Example
---------------------------

Firmware Setup
^^^^^^^^^^^^^^^

Just as in the clock glitching example, you will be required to program the AVR microcontroller with an example you can glitch.
Once again program in the glitch example code to use the ``glitch1()`` function, as described in :ref:`tutorialglitch`.

.. _vccglitchhwsetup:

Hardware Setup
^^^^^^^^^^^^^^^^^

The hardware is almost as in previous incarnations. Note that unlike the :ref:`tutorialglitch` you do not connect the FPGAOUT to the AVR Clock,
instead you use the 7.37 MHz oscillator bridged to both the AVR clock and FPGAIN. This is the *same clock setup as in the side-channel power analysis
tutorials*.

The following figure shows the clock jumper configuration, which are the same as the settings from :ref:`tutorialaes`.

    .. image:: /images/tutorials/basic/aes/hw-2.jpg
   
In addition the VCC glitcher board must be mounted, which means removing the jumpers on the 6-pin header around the AVR. Also the 'IN'
pin on the VCC glitcher board connected to the FPGAOUT pin. This is done via a jumper wire. Both of these are shown in the following figure:

   .. figure:: /images/tutorials/advanced/vccglitching/vccglitcher_routing.jpg
   
    The control line for the VCC glitcher board comes from the FPGAOUT pin.
   
   .. figure:: /images/tutorials/advanced/vccglitching/vccglitcher_mounted.jpg
   
    The VCC glitcher board must be mounted with ``GND`` at the bottom matching, exactly as shown here.
   

Software Setup
^^^^^^^^^^^^^^^^^

1. Connect to the ChipWhisperer device:

    1. As the *Scope Module*, select the *ChipWhisperer/OpenADC* option

    2. As the *Target Module*, select the *Simple Serial* option

    3. Switch to the *Target Settings* tab, and as the *connection*, select the *ChipWhisperer* option

    4. Run the *Download CW Firmware* tool. You should see the FPGA being programmed if required.

    5. Run connect on both the Scope & Target. They should both switch to green circles indicating the system is connected.

2. Setup the Glitch Module to NOT output anything by default **VERY IMPORTANT TO AVOID DAMAGE**

    1. Under the *Glitch Module* set the *Output Mode* as *Glitch Only*, this is the step
       that insurances **you do not cause constant glitches**:
    
        .. image:: /images/tutorials/advanced/vccglitching/glitchexample-capsetup1.png
       
    2. Under the *Glitch Module* set the *Clock Source* as *TargetIO-IN*:
    
        .. image:: /images/tutorials/advanced/vccglitching/glitchexample-capsetup2.png

    3. Under the *Target HS IO-Out* option select the *Glitch Module*, ensuring you've already set the
       *Output Mode* as *Glitch Only* under *Glitch Module*:

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

Using VCC glitching we'll escape from this loop!

.. _vccglitch-manual1:

Manual Glitch Trigger
----------------------

To begin with, you'll simply use the manual glitch triggering, just as the clock glitching example. Once again it is recommended to only use the
*glitch width (as % of period)* option, and not the fine adjust. For power glitching this is more prominent than with clock glitching (e.g. you will
require a considerable glitch width!).

With some background, let's now check some glitches. Assuming you've setup the example as before, do the following:

1. Set the *Glitch Width (as % of period)* to around 50
2. Set the *Glitch Offset (as % of period)* to around 25
3. Ensure *Glitch Trigger* is *Manual*
4. Set the *Repeat* to 10
5. Hit the *Manual Trigger* button
6. See if you end up with either the AVR resetting (reprints ``hello\nA``), or glitches out of the loop (prints ``1234``). It may do both. You may need to
   press the *Manual Trigger* button several times quickly.
7. To force a reset of the AVR, use the `Signature Read` option in AVRStudio.
8. Adjust the glith width & offset as needed. 
9. You may also adjust the *Repeat* option, or cause it to glitch several instructions.

The following figure shows several successul glitches:

    .. image:: /images/tutorials/advanced/vccglitching/vccglitch-examplesettings.png
    
The glitches on the power line will look something like this with the given settings:

   .. image:: /images/tutorials/advanced/vccglitching/vccglitch_working_zoom.png
    
**Be aware that you may crash the AVR!** In the previous examples the AVR had reset after each glitch. It may simply go into another infinite loop
however, or even enter invalid states. Again use the `Signature Read` option in AVRStudio to force a hardware reset of the AVR in these cases. It may
appear like the AVR was never glitched, whereas in reality it was glitched into some invalid state.

Automatic Glitch Trigger
-------------------------

Much in the same way you can try automatic glitch triggering in :ref:`tutorialglitch`, you can also perform this
with the VCC glitch triggering. 

You may need larger *Repeat* values compared to clock glitching - as an example tests of glitching past the password
check for ``glitch3()`` required the following values:

  * **Glitch Width (as % of Period):** 50%
  * **Glitch Offset (as % of Period):** 25%
  * **Glitch Trigger:** External Trigger
  * **Repeat:** 55
  * **Output Mode:** Glitch Only

  
  





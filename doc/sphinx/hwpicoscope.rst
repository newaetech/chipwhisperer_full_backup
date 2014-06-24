.. _hwpicoscope:

PicoScope (USB Scope)
========================

The PicoScope is a USB-connected oscilloscope made by `PicoTech <www.http://picotech.com/>`__.
There is a wide range of scopes available to fit your budget, from $160 passport sized scopes
up to 5 GS/s or faster devices.

Using the PicoScope means the ChipWhisperer capture hardware is not required. Note it's
possible to use a combination of ChipWhisperer capture for tasks such as triggering, and
the PicoScope to perform the analog measurement.

Supported Scopes
--------------------

Currently the following scopes are supported, which is primarily based on the supported
scopes in the `pico-python <https://github.com/colinoflynn/pico-python>`__:

 * PicoScope 6000 Series (6402C/D, 6403C/D, 6404C/D, 6407)
 * PicoScope 5000A Series (5242A/B, 5442A/B, 5243A/B, 5443A/B, 5244A/B, 5444A/B)
 * PicoScope 2000A Series (2204A, 2205A, 2206A, 2207A, 2208A)


Multi-Target Board Setup
---------------------------

An oscilloscope can be used with the :ref:`hwmultitarget` board. Depending on the specifics of
your setup, you can use either the *+20dB* output or the *VOUT* output directly. Be aware that if
using the LNA output you need to be very careful about avoiding clipping!

The following shows an example of using the SMA connectors with a SMA-BNC cable:

    .. image:: /images/picoscope/pico5000_mtarget.jpg
    
Alternatively you can use the *VOUT* test point with a standard oscilloscope probe. The *TRIGGER*
test point is connected to the second channel via another oscilloscope probe.

Generic AVR Setup
-------------------

You can of course build a complete AVR test board, as in the following:

    .. image:: /images/picoscope/pico5000_avrproto.jpg
    
Example Setup - Serial Connection
----------------------------------

1. Setup the *Target Module*, *Scope Module*, and *Trace Format*. In this example we'll be using
   the *PicoScope* along with the *Simple Serial* target:

     .. image:: /images/picoscope/setup_system.png

2. If using the Simple Serial, select the appropriate serial port, probably a USB-Serial converter:

    .. image:: /images/picoscope/setup_port.png
    
3. Select the appropriate PicoScope model, and now hit *Master Connect*:

    .. image:: /images/picoscope/select_scope.png

4. Adjust PicoScope settings. Use the *Capture 1* to confirm things are working and look at the
   waveform. Adjust probe settings to reflect your hardware in use (e.g. if using a 10:1 probe
   ensure you set the *Probe Att.*, otherwise range calculations will be incorrect).
   
   If using a SMA-BNC cable and the *VOUT* connector directly, be careful about using the *DC50*
   coupling mode. Remember this will present a constant DC voltage across your scope, and also
   be generating a voltage divider with the shunt resistor in the target board!
   
   If using the Simple Serial AES example at 7.37 MHz, start with a sample rate around 100 MS/s.
   The scope will automatically use the next-highest sample rate. Given a sample rate of S in MS/s, you
   can find sensible values for the offset & length (again assuming the 7.37 MHz AES Simple Serial
   example):
   
     .. math::
     
        \text{Offset} = \frac{S}{29.49} \times 1500
        
        \text{Length} = \frac{S}{29.49} \times 3000
   
   e.g. for a sample rate of 156 MS/s, Offset=7935, Length=15870. These values are based on the
   SimpleSerial ChipWhisperer example, where the offset is 1500 & sample length is 3000.
   
5. Run the 'Capture Many' and save the project, just as in previous examples.



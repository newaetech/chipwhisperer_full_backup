.. _tutorialaes:

Tutorial #B5: Breaking AES (Straightforward)
============================================

This tutorial will take you through a complete attack on a software AES implementation.
The specific implementation being attacked is a well-known AES implementation written
in C, which is likely to be similar to other implementations used by proprietary systems.

This tutorial runs on four different hardware targets. You only need to follow the steps
for your given hardware. 

Capturing with CW1173 + CW303 (ChipWhisperer-Lite with default XMEGA Target)
------------------------------------------------------------------------------------------

NOTE: You can see a Quick-Start Guide and Video for this target on the :ref:`naecw1173_cwlite` page:
      
|YouTubeCW1173Demo|_

.. |YouTubeCW1173Demo| image:: /images/cw1173/cwlite_demo_video.png
.. _YouTubeCW1173Demo: http://www.youtube.com/watch?v=MJmkYqA-LeM&hd=1

Hardware Setup
^^^^^^^^^^^^^^

1. Connect the CW1173 by micro USB cable to computer.

    .. image:: /images/tutorials/basic/aes/cw1173_microusb.jpg

Capturing the Traces
^^^^^^^^^^^^^^^^^^^^

1. Close & reopen the capture software (to clear out any previous connection which may be invalid).
2. From the *Project* menu elect the *Example Scripts* and then *ChipWhisperer-Lite: AES SimpleSerial on XMEGA* (make sure you don't select
   the "SPA" example):

   .. image:: /images/tutorials/basic/aes/runscript_cw1173xmega.png
   
3. The script will automatically connect to the capture hardware and run 2 example traces. You should see something that looks like the following screen:

   .. image:: /images/tutorials/basic/aes/capture_cw1173xmega.png
   
   To complete the tutorial, follow these steps:
   
       1. Switch to the *General Settings* tab
       2. If you wish to change the number of traces, do so here. The default of 50 should be sufficient to break AES though!
       3. Hit the *Capture Many* button (M in a green triangle) to start the capture process.
       4. You will see each new trace plotted in the waveform display.
       5. You'll see the trace count in the status bar. Once it says *Trace 50 done* (assuming you requested 50 traces) the capture process is complete.

4. Finally save this project using the *File --> Save Project* option, give it any name you want.

5. Skip ahead to :ref:`basicaesanalyzing`.

Capturing with CW1173 + CW304 (ChipWhisperer-Lite with NOTDuino)
------------------------------------------------------------------------------

Hardware Setup
^^^^^^^^^^^^^^

1. Set jumpers on NOTDuino to default position (see silkscreen on bottom of
   NOTDuino for default positions).
2. Connect the NOTDuino using the SMA cable on the "measure" port, and the
   20-pin IDC cable:
3. Connect the CW1173 by micro USB cable to computer.

   .. image:: /images/tutorials/basic/aes/cw1173_avr_microusb.jpg

Capturing the Traces
^^^^^^^^^^^^^^^^^^^^

1. Close & reopen the capture software (to clear out any previous connection which may be invalid).
2. From the *Project* menu elect the *Example Scripts* and then *ChipWhisperer-Lite: AES SimpleSerial on ATMega328P*

   .. image:: /images/tutorials/basic/aes/runscript_cw1173avr.png
   
3. The script will automatically connect to the capture hardware and run 2 example traces. You should see something that looks like the following screen:

   .. image:: /images/tutorials/basic/aes/capture.png
   
   To complete the tutorial, follow these steps:
   
       1. Switch to the *General Settings* tab
       2. If you wish to change the number of traces, do so here. The default of 50 should be sufficient to break AES though!
       3. Hit the *Capture Many* button (M in a green triangle) to start the capture process.
       4. You will see each new trace plotted in the waveform display.
       5. You'll see the trace count in the status bar. Once it says *Trace 50 done* (assuming you requested 50 traces) the capture process is complete.

4. Finally save this project using the *File --> Save Project* option, give it any name you want.

5. Skip ahead to :ref:`basicaesanalyzing`.

Capturing with CW1002 + CW301 (Capture Rev2 + Multi-Target)
-------------------------------------------------------------------------

Hardware Setup
^^^^^^^^^^^^^^

This tutorial uses the :ref:`hwcapturerev2` hardware along with the :ref:`hwmultitarget`
board. Note that you **don't need hardware** to complete the tutorial. Instead you can
download `example traces from the ChipWhisperer Site <https://www.assembla.com/spaces/chipwhisperer/wiki/Example_Captures>`__.

This example uses the Atmel AVR in 28-pin DIP programmed with a *simpleserial* communications protocol. This is the default firmware
programmed into the devices, so you shouldn't need to do anything. If you've erased the device, you can see programming instructions
in the :ref:`installing` section.

The Multi-Target board should be plugged into the ChipWhisperer Capture Rev2 via the 20-pin target cable. The *VOUT* SMA connector is
wired to the *LNA* input on the ChipWhisperer-Capture Rev2 front panel. The general hardware setup is as follows:

   .. image:: /images/tutorials/basic/aes/hw-1.jpg
   
   1. 20-Pin Header connects Multi-Target to Capture Hardware
   2. VOUT Connects to SMA Cable
   3. SMA Cable connects to 'LNA' on CHA input
   4. USB-Mini connects to side (NB: Confirm jumper settings in next section first)

Jumpers on the Multi-Target Victim board are as follows:

   .. image:: /images/tutorials/basic/aes/hw-2.jpg
   
   1. NO jumpers mounted in XMEGA Portion or SmartCard Portion (JP10-JP15, JP19, JP7-JP8, JP17)
   2. 3.3V IO Level (JP20 set to INT.)
   3. The 7.37 MHz oscillator is selected as the CLKOSC source (JP18)
   4. The CLKOSC is connected to the AVR CLock Network, along with connected to the FPGAIN pin (JP4)
   5. The TXD & RXD jumpers are set (JP5, JP6)
   6. Power measurement taken from VCC shunt (JP1)
   
   For more information on these jumper settings see :ref:`hwmultitarget` .

Setting up the Software
^^^^^^^^^^^^^^^^^^^^^^^

It is assumed that you've already followed the guide in :ref:`installing`. Thus it is assumed you are able to communicate with
the ChipWhisperer Capture Rev2 hardware. Note in particular you must have configured the FPGA bitstream in the ChipWhisperer-Capture
software, all part of the description in the :ref:`installing` guide.

Capturing the Traces
^^^^^^^^^^^^^^^^^^^^

This tutorial uses a simple script that ships with the ChipWhisperer Capture software. The easiest method of accomplishing the trace capture is as follows:

1. Close & reopen the capture software (to clear out any previous connection which may be invalid).
2. From the *Project* menu elect the *Example Scripts* and then *ChipWhisperer-Rev2: SimpleSerial Target*

   .. image:: /images/tutorials/basic/aes/runscript.png
   
3. The script will automatically connect to the capture hardware and run 2 example traces. You should see something that looks like the following screen:

   .. image:: /images/tutorials/basic/aes/capture.png
   
   To complete the tutorial, follow these steps:
   
       1. Switch to the *General Settings* tab
       2. If you wish to change the number of traces, do so here. The default of 50 should be sufficient to break AES though!
       3. Hit the *Capture Many* button (M in a green triangle) to start the capture process.
       4. You will see each new trace plotted in the waveform display.
       5. You'll see the trace count in the status bar. Once it says *Trace 50 done* (assuming you requested 50 traces) the capture process is complete.

4. Finally save this project using the *File --> Save Project* option, give it any name you want.

5. Skip ahead to :ref:`basicaesanalyzing`.

Capturing with PicoScope + CW301
--------------------------------

<TODO>

.. _basicaesanalyzing:

Analyzing the Traces
--------------------

1. Open the Analyzer software
2. From the *File --> Open Project* option, navigate to the `.cwp` file you save previously. Open this file.
3. Select the *Project --> Manage Traces* option to open the dialog, enable the captured traces by adding a check-mark in the box. Close the dialog with `ESC`:

   .. image:: /images/tutorials/basic/aes/tracemanage.png
   
4. If you wish to view the trace data, follow these steps:
   
   1. Switch to the *Waveform Display* tab
   2. Switch to the *General* parameter setting tab
   3. You can choose to plot a specific range of traces
   4. Hit the *Redraw* button when you change the trace plot range
   5. You can right-click on the waveform to change options, or left-click and drag to zoom  
   6. (oops there is no 6)
   7. Use the toolbar to quickly reset the zoom back to original
   
   .. image:: /images/tutorials/basic/aes/traceplotting.png
   
5. You can view or change the attack options on the *Attack* parameter settings tab:  

   1. The *Hardware Model* settings are correct for the software AES by default
   2. The *Point Setup* makes the attack faster by looking over a more narrow range of points. Often you might have to characterize your device to determine
      the location of specific attack points of interest.
   3. *Traces per Attack* allows you to use only a subset of capture traces on each attack. Or if you have for example 1000 traces, you could average the results of attacking
      50 traces over 200 attack runs.
   4. *Reporting Interval* is how often data is generated. A smaller interval generates more useful output data, but greatly increases computational complexity (e.g. slows down attack).
      If you only care about attacking the system, the reporting interval can be set to the number of traces. In which case the attack runs completely, and you get the results. For this
      tutorial you can set to a smaller number (such as 5).

   .. image:: /images/tutorials/basic/aes/attacksettings.png

6. Finally run the attack by switching to the *Results Table* tab and then hitting the *Attack* button:

   .. image:: /images/tutorials/basic/aes/attack.png
   
7. If you adjusted the *Reporting Interval* to a smaller number such as 5, you'll see the progression of attack results as more traces are used.
   If not you should simply see the final results, which should have the correct key highlighted in red. In the following case the correct key *was* recovered:
   
   .. image:: /images/tutorials/basic/aes/attack-done.png
   
8. You can also switch to the *Output vs Point Plot* window to see *where* exactly the data was recovered:
   
   1. Switch to the *Output vs Point Plot* tab
   2. Turn on one of the bytes to see results.
   3. The *known correct* guess for the key is highlighted in red. The wrong guesses are plotted in green. You can see that the attacked operation appeared
      to occur around sample 40 for key 0. Remember you can click-drag to zoom in, then right-click and select *View All* to zoom back out.
   4. Turn on another byte to see results for it.
   5. This byte occured much later - sample 1240. By exploring where the maximum correlation was found for the correct key-guess of each byte, you
      can determine where exactly the attacked operation occured.

   .. image:: /images/tutorials/basic/aes/attack-done2.png

Next Steps
----------

This has only briefly outlined how to perform a CPA attack. You can move onto more advanced tutorials, especially showing you how the actual
attack works when performed manually.







.. _tutorialilyaxmega:

Tutorial #A6: Replication of Ilya Kizhvatov's XMEGA® Attack
===========================================================

This tutorial will demonstrate how the ChipWhisperer system can be used to replicate published
academic research findings. In this case we will attempt to recreate an attack published by
Dr. Ilya Kizhvatov, which was performed against the AES implementation in the Atmel® XMEGA® device.

If using hardware-accelerated cryptography in your design, it is useful to understand possible
vulnerabilities to side-channel power analysis attacks. It must be **strongly** cautioned that there
are many published attacks against other hardware crypto accelerators: the XMEGA® device is likely
about as secure as any other general-purpose hardware crypto accelerator (i.e. one without explicit
side-channel resistance).

Within the Atmel® product line some devices *do* mention side-channel analysis resistance. Thus
when designing an embedded product, it's up to the designer to understand why they should care about
side-channel analysis resistance, and to select an appropriate device if they need to defend against
such an attack.

Background
----------

As a reference, you will need a copy of Dr. Kizhvatov's paper entitled "Side Channel Analysis of
AVR XMEGA Crypto Engine", published in the Proceedings of the 4th Workshop on Embedded Systems Security.
If you have access to the ACM Digital Library (most likely because you are part of a university), you
can read this paper on the `ACM Digital Library <http://dl.acm.org/citation.cfm?id=1631724>`_.

Otherwise, you can read this paper as part of Chapter 4 of Dr. Kizhvatov's `PhD Thesis <http://www.iacr.org/phds/106_ilyakizhvatov_physicalsecuritycryptographica.pdf>`_,
starting around page 77 of that PDF file.

Setting up the Hardware
-----------------------

This tutorial uses the :ref:`hwcapturerev2` hardware along with the :ref:`hwmultitarget`
board. Note that you **don't need hardware** to complete the tutorial. Instead you can
download `example traces from the ChipWhisperer Site <https://www.assembla.com/spaces/chipwhisperer/wiki/Example_Captures>`__,
just look for the traces titled *XMEGA: AES128 Hardware Accelerator (ChipWhisperer Tutorial #A6)*.

This example uses the XMEGA Device. You can see instructions for programming in the
:ref:`installing` section, this tutorial assumes you have the programmer aspect working.

The Multi-Target board should be plugged into the ChipWhisperer Capture Rev2 via the 20-pin target cable. The *VOUT* SMA connector is
wired to the *LNA* input on the ChipWhisperer-Capture Rev2 front panel. The general hardware setup is as follows:

   1. 20-Pin Header connects Multi-Target to Capture Hardware
   2. VOUT Connects to SMA Cable
   3. SMA Cable connects to 'LNA' on CHA input
   4. USB-Mini connects to side (NB: Confirm jumper settings in next section first)

Jumpers on the Multi-Target Victim board are as follows:

   .. image:: /images/tutorials/advanced/ilyaxmega/xmegajumpers.jpg

   1. NO jumpers mounted in AVR Portion (JP1,JP4-6,JP28) or SmartCard Portion. Note if your multi-target board does not
      have JP28, the TRIG jumper for the AVR, you will have to remove the AVR from the socket.
   2. 3.3V IO Level (JP20 set to INT.)
   3. The 7.37 MHz oscillator is selected as the CLKOSC source (JP18)
   4. The CLKOSC is routed to the FPGAIN pin (requires jumper wire on JP17), along with routed to XTAL1 pin of XMEGA
       (requires jumper wire to JP4/JP15).
   5. The TXD & RXD jumpers are set on the XMEGA portion (JP5, JP6)
   6. The TRIG jumper is set on the XMEGA portion (JP13)
   7. The PWR jumper is set on the XMEGA portion (JP14)
   8. Power measurement taken from VCC shunt (JP12)

   For more information on these jumper settings see the XMEGA section of :ref:`hwmultitarget`.


Building/Programming the XMEGA Target
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

As described in :ref:`installing`, you'll need to configure the AVR-GCC compiler. Assuming you have this setup, you can run ``make`` in the directory
``chipwhisperer\hardware\victims\firmware\xmega-serial``, which should give you an output like this::

   Size after:
   AVR Memory Usage
   ----------------
   Device: atxmega16a4

   Program:    4572 bytes (22.3% Full)
   (.text + .data + .bootloader)

   Data:        369 bytes (18.0% Full)
   (.data + .bss + .noinit)

   -------- end --------

Using either AVRStudio or AVRDude, program the XMega16A4 device (it is connected to the programmer built into the ChipWhisperer) with the resulting
simpleserial.hex file.

Running the Capture
^^^^^^^^^^^^^^^^^^^

1. Close & reopen the capture software (to clear out any previous connection which may be invalid).
2. From the *Project* menu elect the *Example Scripts* and then *ChipWhisperer-Rev2: SimpleSerial Target*

   .. image:: /images/tutorials/basic/aes/runscript.png

3. The script will automatically connect to the capture hardware and run 2 example traces. They will not yet work on the XMega as additional setup is
   required. You must switch the RX/TX pins:

   .. image:: /images/tutorials/advanced/ilyaxmega/xmegarxtx.png

4. Run a 'Capture 1', you should confirm the encryption algorithm is working:

   .. image:: /images/tutorials/advanced/ilyaxmega/capture1_working.png

5. Switch from software to hardware crypto. To do this change the 'Go' command to ``h$TEXT$\n``:

   .. image:: /images/tutorials/advanced/ilyaxmega/gocommand.png

6. Finally, set the offset to 1500, and number of samples to only 1000:

   .. image:: /images/tutorials/advanced/ilyaxmega/slength.png

7. Confirm you now get something like this with a 'capture 1':

   .. image:: /images/tutorials/advanced/ilyaxmega/capture1_fullworking.png

8. To complete the tutorial, follow these steps:

       1. Switch to the *General Settings* tab
       2. Change the number of traces to 3000.
       3. Hit the *Capture Many* button (M in a green triangle) to start the capture process.
       4. You will see each new trace plotted in the waveform display.
       5. Wait until the capture is complete.

4. Finally save this project using the *File --> Save Project* option, give it any name you want.

Analyzing of Power Traces
-------------------------

As in the :ref:`tutorialaes256boot` tutorial, we will be using the Python script file to override the provided HW model. This will allow us to implement
the model given by Kizhvatov for performing the CPA attack.

Remember that when you change settings in the GUI, the system is actually just automatically adjusting the attack script. You could modify the attack script
directly instead of changing GUI settings. Every time you touch the GUI the autogenerated script is overwritten however, so it would be easy to lose your
changes. As an example here is how setting the point range maps to an API call:

   .. image:: /images/tutorials/advanced/aes256/autoscript1.png

We will first automatically configure a script, and then use that as the base for our full attack.

1. Open the Analyzer software

2. From the *File --> Open Project* option, navigate to the `.cwp` file containing the capture of the
   power usage. This can be either the aes128_xmega_hardware.cwp file downloaded, or the capture
   you performed.

3. View the trace data as before, which should look something like this:

   .. image:: /images/tutorials/advanced/ilyaxmega/traces.png

4. Set the 'Reporting Interval' to 50 or 100. We can change this later through the script.

4. We are now ready to insert the custom data into the attack module. On the *General* tab, make a copy of the auto-generated script. Do so by clicking
   on the autogenerated row, hit *Copy*, save the file somewhere. Double-click on the description of the new file and give it a better name. Finally
   hit *Set Active* after clicking on your new file. The result should look like this:

   .. image:: /images/tutorials/advanced/ilyaxmega/customscript.png

9. You can now edit the custom script file using the built-in editor OR with an external editor. In this example the file would be ``C:\Users\Colin\AppData\Local\Temp\cw_testilya.py``.

The following defines the required functions to implement, you should refer to the academic paper for details of the correlation model::

   # Imports
   from chipwhisperer.analyzer.attacks.models.AES128_8bit import getHW

   def AES128_HD_ILYA(pt, ct, key, bnum):
       """Given either plaintext or ciphertext (not both) + a key guess, return hypothetical hamming weight of result"""

       #In real life would recover this one at a time, in our case we know entire full key, so we cheat to make
       #the iterations easier
       knownkey = [0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c]

       if pt != None:
           s1 = pt[bnum-1] ^ knownkey[bnum-1]
           s2 = pt[bnum] ^ key

           #We subtract 8 as way measurements are taken a higher current results in a lower voltage. Normally this
           #doesn't matter due to use of absolute values. In this attack we do not use absolute mode, so we simply
           #"flip" the expected hamming weight, which results in the correlation changing signs.

           return 8-getHW(s1 ^ s2)

       elif ct != None:
           raise ValueError("Only setup for encryption attacks")
       else:
           raise ValueError("Must specify PT or CT")

10. Add the above function to your custom script file.

11. Change the ``setAnalysisAlgorithm`` to use your custom functions byt making the following call, see the full script in the Appendix::

      self.attack.setAnalysisAlgorithm(CPAProgressive,chipwhisperer.analyzer.attacks.models.AES128_8bit, AES128_HD_ILYA)

12. Adjust the attack bytes to *NOT* attack the first byte, as our hacked script will not work with it::

      self.attack.setTargetBytes([1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15])

13. We want to disable 'absolute mode', where by default the absolute value of the CPA attack is taken. We can do this by adding
    a call to `self.attack.setAbsoluteMode(False)` before the return statement, for example::

        self.attack.setPointRange((0,996))
        self.attack.setAbsoluteMode(False)
        return self.attack

14. Run *Start Attack* as before! Wait for the attack to complete, which should show the key (except for the first byte) being
    recovered:

    .. image:: /images/tutorials/advanced/ilyaxmega/analysisrunning.png

15. At this point you can also look at the output values, which one can compare to the shape of the values published in the paper:

    .. image:: /images/tutorials/advanced/ilyaxmega/results_output.png

    For more detailed plotting, turn off the 'Fast Draw' option:

    .. image:: /images/tutorials/advanced/ilyaxmega/fastdraw.png

    You can also use the 'GUI Override' on the byte highlighting to change the highlighted byte.


Appendix A: Full Attack Script
------------------------------

Here is the full attack script::

   # Based on Ilya Kizhvatov's work, published as "Side Channel Analysis of AVR XMEGA Crypto Engine"
   from chipwhisperer.common.autoscript import AutoScriptBase
   #Imports from Preprocessing
   import chipwhisperer.analyzer.preprocessing as preprocessing
   #Imports from Capture
   from chipwhisperer.analyzer.attacks.CPA import CPA
   from chipwhisperer.analyzer.attacks.CPAProgressive import CPAProgressive
   import chipwhisperer.analyzer.attacks.models.AES128_8bit
   #Imports from utilList

   # Imports
   from chipwhisperer.analyzer.attacks.models.AES128_8bit import getHW

   def AES128_HD_ILYA(pt, ct, key, bnum):
       """Given either plaintext or ciphertext (not both) + a key guess, return hypothetical hamming weight of result"""

       #In real life would recover this one at a time, in our case we know entire full key, so we cheat to make
       #the iterations easier
       knownkey = [0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c]

       if pt != None:
           s1 = pt[bnum-1] ^ knownkey[bnum-1]
           s2 = pt[bnum] ^ key

           #We subtract 8 as way measurements are taken a higher current results in a lower voltage. Normally this
           #doesn't matter due to use of absolute values. In this attack we do not use absolute mode, so we simply
           #"flip" the expected hamming weight, which results in the correlation changing signs.

           return 8-getHW(s1 ^ s2)

       elif ct != None:
           raise ValueError("Only setup for encryption attacks")
       else:
           raise ValueError("Must specify PT or CT")

   class userScript(AutoScriptBase):
       preProcessingList = []
       def initProject(self):
           pass

       def initPreprocessing(self):
           self.preProcessingList = []
           return self.preProcessingList

       def initAnalysis(self):
           self.attack = CPA(self.parent, console=self.console, showScriptParameter=self.showScriptParameter)
           self.attack.setAnalysisAlgorithm(CPAProgressive,chipwhisperer.analyzer.attacks.models.AES128_8bit,AES128_HD_ILYA)
           self.attack.setTraceStart(0)
           self.attack.setTracesPerAttack(2999)
           self.attack.setIterations(1)
           self.attack.setReportingInterval(50)
           self.attack.setTargetBytes([1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15])
           self.attack.setKeyround(0)
           self.attack.setDirection('enc')
           self.attack.setTraceManager(self.traceManager())
           self.attack.setProject(self.project())
           self.attack.setPointRange((0,996))
           self.attack.setAbsoluteMode(False)
           return self.attack

       def initReporting(self, results):
           results.setAttack(self.attack)
           results.setTraceManager(self.traceManager())
           self.results = results

       def doAnalysis(self):
           self.attack.doAttack()


Disclaimers
-----------
Atmel and XMEGA are registered trademarks or trademarks of Atmel Corporation or its subsidiaries, in the US and/or other countries.
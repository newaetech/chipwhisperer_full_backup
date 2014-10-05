.. _tutorialaes256boot:

Tutorial #A5: Breaking AES-256 Bootloader
=========================================


This tutorial will take you through a complete attack on an encrypted bootloader using AES-256.
This demonstrates how to using side-channel power analysis on practical systems, along with
discussing how to perform custom scripts along with custom analysis scripts.

Whilst the tutorial assumes you will be performing the entire capture of traces along with the
attack, it is possible to download the traces if you don't have the hardware, in which case skip
to section TODO XREF.

Background
----------


Bootloader Design
^^^^^^^^^^^^^^^^^

Details of AES-256 CBC
^^^^^^^^^^^^^^^^^^^^^^

The system is using the AES algorithm in Cipher Block Chaining (CBC) mode. In general one avoids using
encryption 'as-is' (i.e. Electronic Code Book), since it means any piece of plaintext always maps to the
same piece of ciphertext. Cipher Block Chaining ensures that if you encrypted the same thing a bunch of
times it would always encrypt to a new piece of ciphertext.

You can see another reference on the design of the encryption side, we'll be only talking about the
decryption side here. In this case AES-256 CBC mode is used as follows, where the details of the AES-256
Decryption block will be discussed in detail later.

.. image:: /images/theory/aes256_cbc.png

Specifics of the AES-256 Decryption algorithm are given below, where this AES-256 implementation was
written by `http://www.literatecode.com/ <Ilya O. Levin>`__::

    aes_addRoundKey_cpy(buf, ctx->deckey, ctx->key);
    aes_shiftRows_inv(buf);
    aes_subBytes_inv(buf);

    for (i = 14, rcon = 0x80; --i;)
    {
        if( ( i & 1 ) )
        {
            aes_expandDecKey(ctx->key, &rcon);
            aes_addRoundKey(buf, &ctx->key[16]);
        }
        else aes_addRoundKey(buf, ctx->key);
        aes_mixColumns_inv(buf);
        aes_shiftRows_inv(buf);
        aes_subBytes_inv(buf);
    }
    aes_addRoundKey( buf, ctx->key);

In an AES-128 implementation there is ten rounds (after 'initially' applying the key), each round applied to the
16-byte state of the AES. With AES-256 the state is still 16 bytes, but is applied over 14 rounds (after
initially applying the first part of the key).

In AES-128 we can target the first output of the S-Box, which is sufficient to recover the entire encryption key. For
AES-256 we can recover 16 bytes of the encryption key, as shown in the following figure of the initial setup of the
decryption algorithm:

.. image:: /images/theory/aes128_decrypted.png

This corresponds to the first 3 lines of source code in the AES-256 decryption algorithm::

    aes_addRoundKey_cpy(buf, ctx->deckey, ctx->key);
    aes_shiftRows_inv(buf);
    aes_subBytes_inv(buf);

As the AES-256 key is 32 bytes, we need to extend the attack to one more AES round. Looking
back at the next part of the source code, this corresponds to the first round through this loop::

  for (i = 14, rcon = 0x80; --i;)
    {
        if( ( i & 1 ) )
        {
            aes_expandDecKey(ctx->key, &rcon);
            aes_addRoundKey(buf, &ctx->key[16]);
        }
        else aes_addRoundKey(buf, ctx->key);
        aes_mixColumns_inv(buf);
        aes_shiftRows_inv(buf);
        aes_subBytes_inv(buf);
        //Attack will focus on state of 'buf' at this
        //point in time
    }
    aes_addRoundKey( buf, ctx->key);

Which is shown in this figure:

.. image:: /images/theory/aes128_round2.png

The critical difference between the initial round and this round is the addition of the mixcols
operation. This operation takes four bytes of input and generates four bytes of output - any change
in a single byte will result in a change of all four bytes of output!

It would at first appear we need to perform a guess over 4 bytes instead of 1 byte. This would be
a considerably more complicated operation! We can consider writing that last step as an equation:

 .. math::

    X^{13} &= SBytes^{-1}\left(MixCols^{-1}\left(ShiftRows^{-1}(X^{13} \oplus K^{13})\right)\right)

The MixCols() operation is a linear function, meaning for example the following applies:

 .. math::

    A = MixCols(A + B) = MixCols(A) + MixCols(B)

Which means instead of determining the encryption key, we can determine the encryption key modified
by the inverse MixCols.

 .. math::

    X^{13} = SBytes^{-1}\left(MixCols^{-1}\left(ShiftRows^{-1}(X^{13} \oplus K^{13})\right)\right) \\
    X^{13} = SBytes^{-1}\left(MixCols^{-1}\left(ShiftRows^{-1}(C)\right) \oplus Y^{13}\right) \\
    Y^{13} = MixCols^{-1}\left(ShiftRows^{-1}(K^{13})\right) \\

Once we fully determine the encryption key we can perform the MixCol and ShiftRow operation to
determine the correct key.

 .. math::

    K^{13} = MixCols\left(ShiftRows(Y^{13})\right) \\

Performing the complete AES-256 side channel analysis attack will thus require the following steps:

1. Perform a standard attack (as in AES-128 decryption) to determine the first 16 bytes of the key,
   corresponding to the 14th round encryption key.

2. Using the known 14th round key, calculate the hypothetical outputs of each S-Box from the 13th round
   using the ciphertext processed by the 14th round, and determine the 16 bytes of the 13th round key
   manipulated by inverse mixcols.

3. Perform the mixcol and shift-row operation on the hypothetical key determined above, which will be
   the 13th round key.

4. Using the AES-256 key schedule, reverse the 13th and 14th round keys to determine the original AES-256
   encryption key.


Setting up the Hardware
-----------------------

This tutorial uses the :ref:`hwcapturerev2` hardware along with the :ref:`hwmultitarget`
board. Note that you **don't need hardware** to complete the tutorial. Instead you can
download `example traces from the ChipWhisperer Site <https://www.assembla.com/spaces/chipwhisperer/wiki/Example_Captures>`__.

This example uses the Atmel AVR in 28-pin DIP programmed with a demo bootloader. You can see instructions for programming in the
:ref:`installing` section, this tutorial assumes you have the programmer aspect working.

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


Building/Programming the Bootloader
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^


Setting up the Software
-----------------------

It is assumed that you've already followed the guide in :ref:`installing`. Thus it is assumed you are able to communicate with the ChipWhisperer Capture Rev2 hardware (or
whatever capture hardware you are using). Note in particular you must have configured the FPGA bitstream in the ChipWhisperer-Capture software, all part of the
description in the :ref:`installing` guide.


Capturing the Traces
--------------------


Communication with the Bootloader
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

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


Analyzing the Traces
--------------------

14th Round Key
^^^^^^^^^^^^^^

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

13th Round Key
^^^^^^^^^^^^^^


The following defines the required functions for our AES-256 attack on the 2nd part of the decryption key
(i.e. the 13th round key)::

   # Imports for AES256 Attack
   from chipwhisperer.analyzer.attacks.models.AES128_8bit import getHW
   from chipwhisperer.analyzer.models.aes.funcs import sbox, inv_sbox, inv_shiftrows, inv_mixcolumns, inv_subbytes


   class AES256_ManualRound(object):
       numSubKeys = 16

   def AES256_13th_Round_HW(pt, ct, key, bnum):
       """Given either plaintext or ciphertext (not both) + a key guess, return hypothetical hamming weight of result"""
       if pt != None:
           raise ValueError("Only setup for decryption attacks")
       elif ct != None:
           knownkey = [0xae, 0x83, 0xc1, 0xa5, 0x6b, 0xcb, 0xc6, 0x46, 0x55, 0xa3, 0xbf, 0x8d, 0x58, 0xfa, 0x20, 0x6d]
           xored = [knownkey[i] ^ ct[i] for i in range(0, 16)]
           block = xored
           block = inv_shiftrows(block)
           block = inv_subbytes(block)
           block = inv_mixcolumns(block)
           block = inv_shiftrows(block)
           result = block
           return getHW(inv_sbox((result[bnum] ^ key)))
       else:
           raise ValueError("Must specify PT or CT")

You can look back at the C code of the AES-256 decryption to see how this is implementing the decryption code.
Note that because of the Inverse MixCols operation, we need the entire input ciphertext, and cannot use just
a single byte of the input ciphertext.

Remember the key we determined was actually the key passed through inverse mixcols and
inverse shiftrows. This means we need to pass the key through shiftrows and mixcols to
remove the effect of those two functions, and determine the normal 13th round key. This
can be done via the interactive Python console::

   >>> from chipwhisperer.analyzer.models.aes.funcs import shiftrows,mixcolumns
   >>> knownkey = [0x25, 0xA8, 0xD2, 0xDC, 0xE0, 0xA1, 0x0E, 0x7B, 0x7B, 0x59, 0xD8, 0x9C, 0x1D, 0xC0, 0x55, 0x2A]
   >>> key = shiftrows(knownkey)
   >>> key = mixcolumns(key)
   >>> print " ".join(["%02x" % i for i in key])
   40 25 51 42 b9 71 6c 94 04 f6 89 69 4b d8 16 a2

At this point we have the 13th round key: ``40 25 51 42 b9 71 6c 94 04 f6 89 69 4b d8 16 a2``

13th and 14th Round Keys to Initial Key
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

If you remember that AES decryption is just AES encryption performed in reverse, this means
the two keys we recovered are the 13th and 14th round encryption keys. AES keys are given as
an 'initial' key which is expanded to all round keys. In the case of AES-256 this initial key
is directly used by the initial setup and 1st round of the algorithm.

For this reason the initial key is referred to as the *0/1 Round Key* in this tutorial, and
the key we've found is the *13/14 Round Key*. Writing out the key we do know gives us this::

   40 25 51 42 B9 71 6C 94 04 F6 89 69 4B D8 16 A2 AE 83 C1 A5 6B CB C6 46 55 A3 BF 8D 58 FA 20 6D

You can use the the AES key scheduling tool built into ChipWhisperer to reverse this key:

.. image:: /images/tutorials/advanced/aes256/keyschedule_tool.png

The tool is accessible from the *Tools* menu. Copy and paste the 32-byte known key into the
input text line. Tell the tool this is the 13/14 round key, and it will automatically display
the complete key schedule along with the initial encryption key.

You should find the initial encryption key is::

   1a 2b 3c 4d 1a 2b 3c 4d 1a 2b 3c 4d 1a 2b 3c 4d 1a 2b 3c 4d 1a 2b 3c 4d 1a 2b 3c 4d 1a 2b 3c 4d


Next Steps
----------

This has only briefly outlined how to perform a CPA attack. You can move onto more advanced tutorials, especially showing you how the actual
attack works when performed manually.
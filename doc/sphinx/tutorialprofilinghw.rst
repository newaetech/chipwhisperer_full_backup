.. _tutorialprofilinghw:

Tutorial #B7: Profiling Attacks (with HW Assumption)
====================================================

This tutorial will demonstrate how to perform a profiling attack against the same AES implementation we've been using. To begin with
we'll be using the same Hamming Weight (HW) assumption as previous attacks, although later we'll look into an attack without such
assumptions.

Under this assumption, we are attempting to accurately determine the what leakage corresponds to the *Hamming Weight* of the sensitive
value being targeted. This requires us to have similar knowledge to the CPA attack, mainly we are making an assumption that the system
does indeed leak the Hamming Weight (HW).

Note that unlike the previous attack, we will require *two* acquisition campaigns. We must first create a template, which means we'll need
a system which we control (or at least know the key for). In real life this would typically mean using another copy of the protected device;
e.g. you have a protected device, but you buy another copy which you control. We use the copy we control to characterize the leakage, before
applying what we have learned onto the real device with an unknown key.

Setting up the Hardware and Software
------------------------------------

This tutorial uses the :ref:`hwcapturerev2` hardware along with the :ref:`hwmultitarget`. The setup details are identical to the :ref:`tutorialaes`
hardware setup. See details in that tutorials.


Capturing the Traces
--------------------

Capturing the Template Traces
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

As mentioned, you will need to perform *two* capture campaigns. This tutorial uses a simple script that ships with the ChipWhisperer Capture software
as the building block. The easiest method of accomplishing the trace capture is as follows:

1. Close & reopen the capture software (to clear out any previous connection which may be invalid).

2. From the *Project* menu elect the *Example Scripts* and then *ChipWhisperer-Rev2: SimpleSerial Target*

   .. image:: /images/tutorials/basic/aes/runscript.png

3. The script will automatically connect to the capture hardware and run 2 example traces. You should see something that looks like the following screen:

   .. image:: /images/tutorials/basic/aes/capture.png

4. Change to the *General Settings* tab, and adjust the *Key/Text Pattern* to send a random encryption key along with a random plaintext. This is shown
   in the following diagram:

   .. image:: /images/tutorials/basic/profiling/campaignrandrand.png

5. Save the project now, giving it a name indicating the **key and text is random**. For example you could call it ``tut_randkey_randplain.cwp``. Note by saving
   the project *before* running the capture you will force all trace data to be saved into a directory with the same name. This will make it easier to find
   the data at a later time.

6. Change the number of traces to be *1000*: more traces are needed to form the profile. In this case 1000 is a very small capture, however for this unprotected
   implementation will be sufficient. An interesting experiment is to see how using more or less traces impacts your templates!

7. Hit the *Capture Many* button (M in a green triangle) to start the capture process. You will see each new trace plotted in the waveform display. Once the
   capture is complete, save the project file again.

8. Exit the capture application, again being sure to first save the project.


Capturing the Attack Traces
^^^^^^^^^^^^^^^^^^^^^^^^^^^

1. Follow steps 1-3 of the previous section. Leave the *Key/Text Pattern* as default, where the encryption key is fixed and the plaintext is random.

2. Save the project now in the same directory as the ``.cwp`` file from the previous section, giving it a name indicating the key is **fixed** and text is **random**.
   For example you could call it ``tut_fixedkey_randplain.cwp``, again saving it in the same directory as the previous file.

6. Change the number of traces to be *20*: by comparison the profiling attack will require less attack traces compared to the CPA attack.

7. Hit the *Capture Many* button (M in a green triangle) to start the capture process. You will see each new trace plotted in the waveform display. Once the
   capture is complete, save the project file again.

8. Exit the capture application, again being sure to first save the project.

Generating the Template
-----------------------

The first step in the attack is to generate the template. Again for generating the template this typically requires access to a system almost identical to the device
being attacked. It is assumed that the actual device differs from the device being attacked, but in our example we'll be using the same device.

1. Open the Analyzer software

2. From the *File --> Open Project* option, navigate to the ``tut_randkey_randplain.cwp`` file you save previously. Open this file, ensure it is the file corresponding with the
   **randomly changing encryption key**!

3. Open the *Trace Explorer* by selecting it from the *Tools* menu.

4. Change the *Partition Mode* to *HW AES Intermediate*, and select the *Auto-Save Data to Project* option:

   .. image:: /images/tutorials/basic/profiling/traceexplorer1.png

5. Hit the *Display* button, and then select *All On*. This displays the differences between the various groups of Hamming Weight (HW) values:

   .. image:: /images/tutorials/basic/profiling/traceexplorer2.png

6. We now want to figure out the Points of Interest (POI), which we will use for our template. Hit the *Open POI Table* and perform the following:

    1. Set the *Num POI/Subkey* to 3
    2. Set the *Min Spacing between POI* to 5
    3. Click the *Recalc POI Values* button
    4. Click the *Set as POI in Project* button

  .. image:: /images/tutorials/basic/profiling/traceexplorer3.png

7. Close the Trace Explorer window

8. Change the attack module to *Profiling*:

   .. image:: /images/tutorials/basic/profiling/selectprofiling.png

9. Go to the *Template Generation* section, and ensure you select the appropriate trace range (0-999 in this example). Select the *TraceExplorer Table* as the POI source, since we
   had just populated this with a valid POI data. Notice that if you view the analysis script it will list the poi you selected, if not hit the *Read POI* button:

   .. image:: /images/tutorials/basic/profiling/templategeneration.png

10. Click the *Generate Templates* button. This will dynamically load and run the ``def generateTemplates(self):`` function in the scripting window.

11. Save the project file, which will save the location of the template file. We need to copy this information over to our second project, which is the traces we actually plan on
    attacking.

12. From the *Project* menu select the *Project File Editor (text)*, which gives you access to the raw project file. You will have to hit the *Reload Editor from Disk* button, which
    reads the changes you just saved into this special text editor. For more information on the project file editor see XREF TODO.

13. Scroll down until you find a section titled ``[Template Data 0001 - Templates]``. Note the number may change (e.g. 0002, etc), just use the largest number which will be the most
    recent saved change. Copy this section to a text editor, it will look something like this::

      [Template Data 0001 - Templates]
      tracestart = 0
      traceend = 999
      poi = "[38, 30, 2290]", "[138, 1658, 1650]", "[2134, 238, 2402]", "[338, 2618, 2626]", "[438, 430, 2766]", "[1806, 538, 1690]", "[638, 2174, 2462]", "[738, 2886, 2658]", "[838, 830, 2282]", "[938, 1722, 1858]", "[2290, 1038, 2206]", "[1138, 2690, 2946]", "[1238, 1650, 1798]", "[1338, 1918, 1754]", "[1438, 2342, 2238]", "[2774, 1538, 2722]"
      partitiontype = PartitionHWIntermediate
      filename = avr_keyrand_plainrand_1000traces_data\analysis\templates-PartitionHWIntermediate-0-999.npz

14. Close the Analyzer software, as we will now apply this template.

Applying the Template
---------------------

1. Open the Analyzer software (NB: be sure to open a new window, do not re-use a previous instance)

2. From the *File --> Open Project* option, navigate to the ``tut_fixedkey_randplain.cwp`` file you save previously. Open this file, ensure it is the file corresponding with the
   **fixed encryption key**!

3. Open the project text editor if not already open.

4. Append the *Template Data* section you copied from the previous project to your project file:

   .. image:: /images/tutorials/basic/profiling/projecteditor_addtemplate.png

5. Use the *Save Editor to Disk* button to write these changes. Note this assumes that both the ``tut_fixedkey_randplain.cwp`` and ``tut_randkey_randplain.cwp`` were saved in the
   same directory, as otherwise the reference to the template file will break!

6. Close the Analyzer software. Re-open the software and reload the same project file (e.g. ``tut_fixedkey_randplain.cwp``), this step is needed to ensure the changes you added in
   are loaded into memory.

7. Change the attack module to *Profiling*:

   .. image:: /images/tutorials/basic/profiling/selectprofiling.png

8. Change the *Reporting Interval* to 1, since we want to observe what happens on each added trace:

   .. image:: /images/tutorials/basic/profiling/reportinginterval_1.png

9. Press the *Attack* button. Observe that with only a few traces you are able to determine the encryption key:

   .. image:: /images/tutorials/basic/profiling/attack_working.png

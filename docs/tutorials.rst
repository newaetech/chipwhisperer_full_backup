.. _tutorials:

#########
Tutorials
#########

.. toctree::
    :maxdepth: 2
    :hidden:
    :glob:

    tutorials/*


The lab/tutorial format has recently been updated! New labs are designed
to closely follow online courses offered at https://learn.chipwhisperer.io.
Labs are also designed to encourage greater engagement and problem
solving than the earlier tutorials (less shift+enter).

This page covers some the labs currently available in the latest version of
ChipWhisperer. These labs are all in the form of Jupyter notebooks with
the output generated to follow along. Generally, tutorials that are missing
are more explorative and open ended in nature, or are currently in development.

.. note:: This page usually displays solution files, which have solutions
    for blank code blocks in the labs. For the best learning experience,
    try running the labs yourself first. 

The plots in the tutorials keep their interactivity. This allows you to zoom in
and explore the traces we collected and compare with yours to help you complete
the tutorials.

The tutorials here are grouped by course.

:ref:`tutorials-sca101`

    An introduction to side channel power analysis attacks. These
    attacks involve measuring the power consumption of a device
    while it's performing sensitive operations. This can be
    used to attack password checks and even recover full
    encryption keys. Goes from the basics to attacking real
    AES-128 implementations. Follows https://learn.chipwhisperer.io/courses/power-analysis-101.


:ref:`tutorials-sca201`

    Extends what was shown in SCA101. Explore techniques
    to resynchronize traces, attack different AES-128
    implementations, and break an AES-256 bootloader. Online
    course in development.

:ref:`tutorials-sca202`

    Shifts away from symmetric cryptography and instead 
    focuses on asymmetric crypto. Attacks an avrcryptolib
    RSA implementation using simple power analysis. Online course
    planned.

:ref:`tutorials-sca203`

    Focuses on "Test Vector Leakage Assessment", which
    is a testing methodology used to evaluate a device's
    vulnerability to power analysis. Online course planned.


:ref:`tutorials-fault101`

    An introduction to voltage and clock glitching attacks.
    By disrupting a device's power or clock, we can cause
    unintended behaviour, such as corrupting variables
    or even skipping password checks. Online course in
    development.

:ref:`tutorials-fault201`

    Use the fault attacks your learned about in
    Fault101 to attack cryptographic implementations
    and recover their keys.
    Covers multiple fault attacks against AES-128,
    as well as a fault attack against RSA. Online course planned.

Each tutorial section on this page has links for the jupyter notebook tutorial with
the output for your target. Your hardware will not have exactly the same output when
you follow through with the target, however, it will be close.

.. _tutorials-sca101:

**************
SCA101
**************
Use the power of power measurements. Measuring the power consumption of the target
during sensitive operations can allow you to determine if the target is leaking
information about its sensitive operation (such as encryption). Analysis of the
power consumption may allow you to recover the secret that should have been
inaccessible inside the target (such as the encyption key).


Lab 2_1B - Power Analysis for Password Bypass
---------------------------------------------
Use power analysis to defeat a password check with a timing vulnerability.

  * :ref:`Arm <tutorial-courses_sca101_soln_lab 2_1b -openadc-cwlitearm>`
  * :ref:`XMEGA <tutorial-courses_sca101_soln_lab 2_1b -openadc-cwlitexmega>`
  * :ref:`Nano <tutorial-courses_sca101_soln_lab 2_1b -cwnano-cwnano>`

Lab 3_1 - Large Hamming Weight Swings
-------------------------------------
Explore whether or not power traces differ noticeably based on what data 
a device is manipulating.

  * :ref:`Arm <tutorial-courses_sca101_soln_lab 3_1 -openadc-cwlitearm>`
  * :ref:`XMEGA <tutorial-courses_sca101_soln_lab 3_1 -openadc-cwlitexmega>`
  * :ref:`Nano <tutorial-courses_sca101_soln_lab 3_1 -cwnano-cwnano>`

Lab 3_2 - Recovering Data from a Single Bit 
-------------------------------------------
See how knowing the value of a single bit of an AES intermediate can be used 
to recover an entire byte of the key. Also explore how "noise" affects
your ability to recover a key byte.

  * :ref:`Arm <tutorial-courses_sca101_soln_lab 3_2 -openadc-cwlitearm>`
  * :ref:`XMEGA <tutorial-courses_sca101_soln_lab 3_2 -openadc-cwlitexmega>`
  * :ref:`Nano <tutorial-courses_sca101_soln_lab 3_2 -cwnano-cwnano>`

Lab 3_3 - DPA on Firmware Implementation of AES
-----------------------------------------------
Attack and defeat an AES implementation using differential 
power analysis.

  * :ref:`Arm <tutorial-courses_sca101_soln_lab 3_3 -openadc-cwlitearm>`
  * :ref:`Nano <tutorial-courses_sca101_soln_lab 3_3 -cwnano-cwnano>`

Lab 4_1 - Power and Hamming Weight Relationship 
-----------------------------------------------
Explore the relationship between power consumption 
and the Hamming weight of data being manipulated by
a microcontroller.

  * :ref:`Arm <tutorial-courses_sca101_soln_lab 4_1 -openadc-cwlitearm>`
  * :ref:`XMEGA <tutorial-courses_sca101_soln_lab 4_1 -openadc-cwlitexmega>`
  * :ref:`Nano <tutorial-courses_sca101_soln_lab 4_1 -cwnano-cwnano>`

Lab 4_2 - CPA on Firmware Implementation of AES 
-----------------------------------------------
Use the Power Hamming weight relationship to implement
a more effective attack than DPA, called Correlation Power Analysis 

  * :ref:`Arm <tutorial-courses_sca101_soln_lab 4_2 -openadc-cwlitearm>`
  * :ref:`XMEGA <tutorial-courses_sca101_soln_lab 4_2 -openadc-cwlitexmega>`
  * :ref:`Nano <tutorial-courses_sca101_soln_lab 4_2 -cwnano-cwnano>`

.. _tutorials-sca201:

**************
SCA201
**************
Build upon SCA101 and explore more advanced techniques in the field of
power analysis. Includes things like using Sum of Absolute Difference
to resynchronize jittery traces, exploring different AES implementations,
and attacking more complete target setups, such as a bootloader.

Work in Progress!

.. _tutorials-sca202:

**************
SCA202
**************
In other courses on side channel attacks against cryptography,
we generally focused on symmetric cryptographic algorithms like
AES. In this set of labs, you can explore how asymmetric cryptographic
algorithms can be vulnerable to side channel attacks.

Work in Progress!

.. _tutorials-sca203:

************
SCA203 
************
Explores how Test Vector Leakage Assessment (TVLA) can be used
to evaluate how and where side channel data is leaked from 
power measurements.

Work in Progress!

.. _tutorials-fault101:

*************
Fault 101
*************
There are multiple ways to cause a device under test or target to perform
unintended operations, or skip operations. These unintended side effects
(faults) can be used to skip operations, corrupt the target, perform unintended
operations, etc.. The ChipWhisperer platform supports generating faults using
the target's clock, and supplied voltage; referred to as clock glitching, and
voltage glitching, respectively.

Lab 1_1 - Introduction to Clock Glitching
---------------------------------------------
Embedded devices require a consistant clock to operate. Explore
how inserting small glitches into the clock signal for a device
can corrupt the result of operations.

  * :ref:`Arm <tutorial-courses_fault101_soln_fault 1_1 -openadc-cwlitearm>`
  * :ref:`XMEGA <tutorial-courses_fault101_soln_fault 1_1 -openadc-cwlitexmega>`

Lab 1_2 - Clock Glitching to Bypass Password
------------------------------------------------
Clock glitches can also be used to get a device to skip instructions.
Use this to bypass a password check.


  * :ref:`Arm <tutorial-courses_fault101_soln_fault 1_2 -openadc-cwlitearm>`
  * :ref:`XMEGA <tutorial-courses_fault101_soln_fault 1_2 -openadc-cwlitexmega>`

Lab 1_3 - Clock Glitching to Memory Dump
-----------------------------------------
Devices often place messages in an array, then use a loop
to print out each character. Use clock glitching to skip
the check for the end of the loop and cause the target
to spill much of its memory, including secret data.


  * :ref:`Arm <tutorial-courses_fault101_soln_fault 1_3 -openadc-cwlitearm>`
  * :ref:`XMEGA <tutorial-courses_fault101_soln_fault 1_3 -openadc-cwlitexmega>`

Lab 2_1 - Introduction to Voltage Glitching 
--------------------------------------------
There are many situations where clock glitching 
won't be a feasible attack vector, such as 
when the target device is using an internal clock.
Voltage glitching is another method of glitching a
target - by interrupting a device's power supply for a short
time, we can get many of the same effects as clock glitching.
In this lab, repeat the Lab 1_1 attack with voltage glitching.

  * :ref:`Arm <tutorial-courses_fault101_soln_fault 2_1 -openadc-cwlitearm>`
  * :ref:`XMEGA <tutorial-courses_fault101_soln_fault 2_1 -openadc-cwlitexmega>`


Lab 2_1B - Introduction to Voltage Glitching with CWNano 
---------------------------------------------------------
The ChipWhisperer-Nano has a very different glitch 
interface and different capabilities than the Lite and Pro.
This lab is a Nano specific version of Lab 2_1

  * :ref:`Nano <tutorial-courses_fault101_soln_fault 2_1b -cwnano-cwnano>`


Lab 2_2 - Voltage Glitching to Bypass Password 
-----------------------------------------------
Repeat Lab 1_1 with voltage glitching 

  * :ref:`Arm <tutorial-courses_fault101_soln_fault 2_2 -openadc-cwlitearm>`
  * :ref:`XMEGA <tutorial-courses_fault101_soln_fault 2_2 -openadc-cwlitexmega>`


Lab 2_2B - Voltage Glitching to Bypass Password with CWNano
-----------------------------------------------------------
CWNano specific version of Lab 2_2 

  * :ref:`Nano <tutorial-courses_fault101_soln_fault 2_2b -cwnano-cwnano>`

Lab 2_3 - Voltage Glitching to Memory Dump
-------------------------------------------
Repeat Lab 1_3 with voltage glitching

  * :ref:`Arm <tutorial-courses_fault101_soln_fault 2_3 -openadc-cwlitearm>`
  * :ref:`XMEGA <tutorial-courses_fault101_soln_fault 2_3 -openadc-cwlitexmega>`


.. _tutorials-fault201:

**************
Fault201 
**************
Fault attacks aren't just useful against things like
password checks and print loops, they can also effectively
attack encryption algorithms as well. This set of labs 
explores three different fault attacks against AES, 
as well as one against RSA.


.. _cwcustomfirmware:

########################################
Custom Firmware on ChipWhisperer Targets
########################################

**************
Basic Overview
**************

ChipWhisperer firmware projects can be broken down into four separate parts: the main project files,
the hardware abstraction layer (HAL), crypto files, and simpleserial.

Main Project Files
==================

These files are contained in different project folders and are unique to each project. This includes source files and the project
makefile. For example, simpleserial-aes has :code:`simplserial-aes.c` which contains :code:`main()` and various other
functions for reading a key/plaintext over serial, doing an encryption, etc. The makefile for this project tells the build
which custom files to use, as well as includes the simpleserial makefile and the main build makefile :code:`Makefile.inc`.

HAL
===

The HAL part of the build system is contained in the HAL folder and includes files that are specific to different targets.
For example, the STM32F3, XMEGA, and SAM4S targets all have their own folder in the HAL folder and include sourcefiles
for implementing things like serial communication, clock setup, etc, as well as interface functions to allow 
the main project files to do setup and send and receive serial characters. 
They also include a makefile which specifies which compiler to use, as well as compile and build flags. 
Finally, they will typically have a linker script file, which defines the various memory regions for flash, RAM, etc.

Crypto
======

The crypto part of the build system, contained in the crypto folder, has various software cryptographic implementations within it.
This makes it easy to use a single version of AES across multiple projects. It also simplifies swapping out different cryptographic
algorithms in the same project. 

SimpleSerial
============

The simpleserial folder contains the implementation of the target side of the simpleserial protocol. If you want to modify
anything related to that, this is the spot to do it in.

********
Examples
********

Simple XOR cipher
=================

To illustrate how to make your own project, we'll go through implementing a simple
XOR cipher. Let's start by making a new folder for our project, which we'll call :code:`simpleserial-xor` :

.. code:: bash

    # Assume your in the firmware directory
    mkdir simpleserial-xor

Next, we'll make a source file, :code:`simplserial-xor.c` that can accept a key and can also xor that key with incoming 
plaintext and send it back. Our project files typically support both simpleserial V1 and V2, but here we'll just use V2:

.. code:: C

    #include "hal.h" // HAL function definitions
    #include "simpleserial.h" // include simpleserial
    #include <stdint.h>
    #include <stdlib.h>
    static uint8_t key[16] = {0}; // initialize key as all 0's

    // Read key in
    uint8_t get_key(uint8_t cmd, uint8_t scmd, uint8_t len, uint8_t *buf)
    {
        if (len != 16) {
            return 0x04; // SS_ERR_LEN
        }
        for (uint8_t i = 0; i < 16; i++) {
            key[i] = buf[i]; // set key
        }

        return 0x00; //SS_ERR_OK
    }

    // read plaintext, do encryption, then send ciphertext back
    uint8_t xor_inc(uint8_t cmd, uint8_t scmd, uint8_t len, uint8_t *buf)
    {
        if (len != 16) {
            return 0x04; // SS_ERR_LEN
        }

        trigger_high();
        for (uint8_t i = 0; i < 16; i++) {
            buf[i] ^= key[i]; // do "encryption"
        }
        trigger_low();

        simpleserial_put('r', 16, buf); // send ciphertext back

        return 0x00; //SS_ERR_OK
    }

    int main(void)
    {
        // setup
        platform_init();
        init_uart();
        trigger_setup();
        simpleserial_init();

        // tell simpleserial_get to look for 'p' and 'k' packets
        // and to do these two functions when they're received
        simpleserial_addcmd('p', 16, xor_inc);
        simpleserial_addcmd('k', 16, get_key);

        // look for simpleserial packets
        while(1)
            simpleserial_get();
    }

Note that the above doesn't use the crypto portion of the build system. Unless you want to use your algorithm
across multiple projects, typically it's easier to just include your algorithm in the main project files.

The final thing we need to do is setup our makefile. Your makefile just needs to
include the build filename that you want, the source files, and include the simpleserial and main
makefiles:

.. code:: Makefile

    # what the final build file will be called
    TARGET = simpleserial-xor

    # our C firmware file
    SRC += simpleserial-xor.c

    # Use simpleserial 2
    SS_VER = SS_VER_2_1

    # No crypto required
    CRYPTO_TARGET = NONE

    # include other required build files
    include ../simpleserial/Makefile.simpleserial

    FIRMWAREPATH = ../.
    include $(FIRMWAREPATH)/Makefile.inc

With all that done, you can now build your firmware for any supported target in our build system:

.. code:: bash

    make -j PLATFORM=CW308_SAM4S


You may need to run :code:`make clean` before building your new firmware:

.. code:: bash

    make clean
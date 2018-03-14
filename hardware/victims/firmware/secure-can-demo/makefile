# Hey Emacs, this is a -*- makefile -*-
#----------------------------------------------------------------------------
#
# Makefile for ChipWhisperer SimpleSerial-AES Program
#
#----------------------------------------------------------------------------
# On command line:
#
# make all = Make software.
#
# make clean = Clean out built project files.
#
# make coff = Convert ELF to AVR COFF.
#
# make extcoff = Convert ELF to AVR Extended COFF.
#
# make program = Download the hex file to the device, using avrdude.
#                Please customize the avrdude settings below first!
#
# make debug = Start either simulavr or avarice as specified for debugging, 
#              with avr-gdb or avr-insight as the front end for debugging.
#
# make filename.s = Just compile filename.c into the assembler code only.
#
# make filename.i = Create a preprocessed source file for use in submitting
#                   bug reports to the GCC project.
#
# To rebuild project do "make clean" then "make all".
#----------------------------------------------------------------------------

# Target file name (without extension).
# This is the base name of the compiled .hex file.
TARGET = secure-can
#TARGET = simpleserial-can
# List C source files here.
# Header files (.h) are automatically pulled in.
SRC += secure-can.c
SRC += seccan.c
#SRC += simpleserial-can.c
DEMO = SECCAN

CFLAGS += -DHAL_CAN_MODULE_ENABLED  
CFLAGS += -DSTM32F303xC
CFLAGS += -DHS_INTERNAL_CLOCK=8000000
OPT=0
# -----------------------------------------------------------------------------
ifeq ($(MAKECMDGOALS), can)
	PLATFORM=CW308_STM32F3
	CRYPTO_TARGET=TINYAES128C
endif

#can: test 
	#echo "Building secure can"
	#echo Source files: $(SRC)
#adc: CFLAGS += -DADC_STM
#adc: all

ifeq ($(ISMASTER), NO)
#echo "Making ADC Can"
CFLAGS += -DSTM_ADC
endif

ifeq ($(CRYPTO_TARGET),)
CRYPTO_TARGET = AVRCRYPTOLIB
endif

ifeq ($(CRYPTO_OPTIONS),)
CRYPTO_OPTIONS = AES128C
endif

#Add simpleserial project to build
include ../simpleserial/Makefile.simpleserial

FIRMWAREPATH = ../.
include $(FIRMWAREPATH)/Makefile.inc

test: 
	echo Source files: $SRC

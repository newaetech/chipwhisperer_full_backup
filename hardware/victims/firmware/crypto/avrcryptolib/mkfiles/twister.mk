# Makefile for TWISTER
ALGO_NAME := TWISTER

# comment out the following line for removement of TWISTER from the build process
HASHES += $(ALGO_NAME)

$(ALGO_NAME)_DIR      := twister/
$(ALGO_NAME)_INCDIR   := hfal/
$(ALGO_NAME)_OBJ      := twister-small-asm.o twister-large-asm.o twister-asm.o \
                         twister224.o twister256.o twister384.o twister512.o
$(ALGO_NAME)_TESTBIN  := main-twister-test.o hfal_twister224.o hfal_twister256.o \
                         hfal_twister384.o hfal_twister512.o $(CLI_STD) $(HFAL_STD)
$(ALGO_NAME)_NESSIE_TEST      := "nessie"
$(ALGO_NAME)_PERFORMANCE_TEST := "performance"


# Makefile for DES
ALGO_NAME := DES

# comment out the following line for removement of DES from the build process
BLOCK_CIPHERS += $(ALGO_NAME)

$(ALGO_NAME)_DIR      := des/
$(ALGO_NAME)_INCDIR   := bcal/
$(ALGO_NAME)_OBJ      := des.o
$(ALGO_NAME)_TESTBIN  := main-des-test.o bcal_des.o bcal_tdes.o bcal_tdes2.o $(CLI_STD) $(BCAL_STD)
$(ALGO_NAME)_NESSIE_TEST      := "nessie"
$(ALGO_NAME)_PERFORMANCE_TEST := "performance"


# Makefile for CAST5
ALGO_NAME := CAST5

# comment out the following line for removement of CAST5 from the build process
BLOCK_CIPHERS += $(ALGO_NAME)

$(ALGO_NAME)_DIR      := cast5/
$(ALGO_NAME)_INCDIR   := bcal/
$(ALGO_NAME)_OBJ      := cast5.o cast5-sbox.o
$(ALGO_NAME)_TESTBIN  := main-cast5-test.o bcal_cast5.o $(CLI_STD) $(BCAL_STD)
$(ALGO_NAME)_NESSIE_TEST      := "nessie"
$(ALGO_NAME)_PERFORMANCE_TEST := "performance"


# Makefile for CS-Cipher
ALGO_NAME := CSCIPHER_SMALL_C

# comment out the following line for removement of CS-Cipher from the build process
BLOCK_CIPHERS += $(ALGO_NAME)

$(ALGO_NAME)_DIR      := cscipher/
$(ALGO_NAME)_INCDIR   := bcal/ memxor/
$(ALGO_NAME)_OBJ      := cscipher_small.o cscipher_sbox.o memxor.o memxor_p.o
$(ALGO_NAME)_TESTBIN  := main-cscipher-test.o bcal_cscipher.o $(CLI_STD) $(BCAL_STD) 
$(ALGO_NAME)_NESSIE_TEST      := test nessie
$(ALGO_NAME)_PERFORMANCE_TEST := performance
$(ALGO_NAME)_DEF := SBOX_PROG=0

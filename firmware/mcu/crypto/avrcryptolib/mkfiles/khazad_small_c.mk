# Makefile for Khazad
ALGO_NAME := KHAZAD_SMALL_C

# comment out the following line for removement of CS-Cipher from the build process
BLOCK_CIPHERS += $(ALGO_NAME)

$(ALGO_NAME)_DIR      := khazad/
$(ALGO_NAME)_INCDIR   := bcal/ memxor/ gf256mul/
$(ALGO_NAME)_OBJ      := khazad.o memxor.o gf256mul.o
$(ALGO_NAME)_TESTBIN  := main-khazad-test.o bcal_khazad.o $(CLI_STD) $(BCAL_STD) 
$(ALGO_NAME)_NESSIE_TEST      := test nessie
$(ALGO_NAME)_PERFORMANCE_TEST := performance
$(ALGO_NAME)_DEF := SBOX_PROG=0

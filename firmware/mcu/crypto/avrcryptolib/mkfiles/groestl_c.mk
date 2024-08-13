# Makefile for Grøstl
ALGO_NAME := GROESTL_C

# comment out the following line for removement of Grøstl from the build process
HASHES += $(ALGO_NAME)

$(ALGO_NAME)_DIR      := groestl/
$(ALGO_NAME)_INCDIR   := aes/ memxor/ gf256mul/ hfal/
$(ALGO_NAME)_OBJ      := groestl_small.o groestl_large.o memxor.o aes_sbox.o gf256mul.o
$(ALGO_NAME)_TESTBIN  := hfal_groestl_large.o hfal_groestl_small.o main-groestl-test.o $(CLI_STD) $(HFAL_STD)
$(ALGO_NAME)_NESSIE_TEST      := test nessie
$(ALGO_NAME)_PERFORMANCE_TEST := performance


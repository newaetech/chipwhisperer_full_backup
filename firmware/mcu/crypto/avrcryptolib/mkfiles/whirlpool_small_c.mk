# Makefile for Whirlpool
ALGO_NAME := WHIRLPOOL_SMALL_C

# comment out the following line for removement of Whirlpool from the build process
HASHES += $(ALGO_NAME)

$(ALGO_NAME)_DIR      := whirlpool/
$(ALGO_NAME)_INCDIR   := hfal/ memxor/ gf256mul/
$(ALGO_NAME)_OBJ      := whirlpool.o gf256mul.o memxor.o
$(ALGO_NAME)_TESTBIN  := main-whirlpool-test.o $(CLI_STD) $(HFAL_STD) hfal_whirlpool.o dump-asm.o dump-decl.o
$(ALGO_NAME)_NESSIE_TEST      := "nessie"
$(ALGO_NAME)_PERFORMANCE_TEST := "performance"
$(ALGO_NAME)_DEF := SBOX_PROG=1


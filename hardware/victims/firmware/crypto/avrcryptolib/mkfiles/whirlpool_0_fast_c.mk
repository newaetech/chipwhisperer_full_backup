# Makefile for Whirlpool-0
ALGO_NAME := WHIRLPOOL_0_FAST_C

# comment out the following line for removement of Whirlpool from the build process
HASHES += $(ALGO_NAME)

$(ALGO_NAME)_DIR      := whirlpool/
$(ALGO_NAME)_INCDIR   := hfal/ memxor/ gf256mul/
$(ALGO_NAME)_OBJ      := whirlpool.o gf256mul.o memxor.o
$(ALGO_NAME)_TESTBIN  := main-whirlpool-0-test.o $(CLI_STD) $(HFAL_STD) hfal_whirlpool_0.o dump-asm.o dump-decl.o
$(ALGO_NAME)_NESSIE_TEST      := "nessie"
$(ALGO_NAME)_PERFORMANCE_TEST := "performance"
$(ALGO_NAME)_DEF := SBOX_PROG=0 WHIRLPOOL_0=1


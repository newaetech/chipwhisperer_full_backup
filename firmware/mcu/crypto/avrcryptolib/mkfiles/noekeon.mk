# Makefile for noekeon
ALGO_NAME := NOEKEON

# comment out the following line for removement of noekeon from the build process
BLOCK_CIPHERS += $(ALGO_NAME)

$(ALGO_NAME)_DIR      := noekeon/
$(ALGO_NAME)_INCDIR   := bcal/
$(ALGO_NAME)_OBJ      := noekeon_asm.o
$(ALGO_NAME)_TESTBIN  := main-noekeon-test.o bcal_noekeon.o $(CLI_STD) $(BCAL_STD)
$(ALGO_NAME)_NESSIE_TEST      := test nessie
$(ALGO_NAME)_PERFORMANCE_TEST := performance


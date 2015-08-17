# Makefile for serpent
ALGO_NAME := SERPENT_ASM_BITSLICE

# comment out the following line for removement of serpent from the build process
BLOCK_CIPHERS += $(ALGO_NAME)

$(ALGO_NAME)_DIR      := serpent/
$(ALGO_NAME)_INCDIR   := memxor/ bcal/
$(ALGO_NAME)_OBJ      := serpent-sboxes-bitslice-asm.o serpent-asm.o memxor.o
$(ALGO_NAME)_TESTBIN  := main-serpent-test.o bcal_serpent.o $(CLI_STD) $(BCAL_STD) 
$(ALGO_NAME)_NESSIE_TEST      := "nessie"
$(ALGO_NAME)_PERFORMANCE_TEST := "performance"


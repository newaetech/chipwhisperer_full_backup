# Makefile for SHABEA
ALGO_NAME := SHABEA

# comment out the following line for removement of SHABEA from the build process
BLOCK_CIPHERS_DONTUSE += $(ALGO_NAME)

$(ALGO_NAME)_DIR      := shabea/
$(ALGO_NAME)_INCDIR   := memxor/ bcal/
$(ALGO_NAME)_OBJ      := shabea.o sha256-asm.o memxor.o
$(ALGO_NAME)_TESTBIN  := main-shabea-test.o $(CLI_STD)  \
                         nessie_bc_test.o nessie_common.o performance_test.o
$(ALGO_NAME)_NESSIE_TEST      := "nessie"
$(ALGO_NAME)_PERFORMANCE_TEST := "performance"


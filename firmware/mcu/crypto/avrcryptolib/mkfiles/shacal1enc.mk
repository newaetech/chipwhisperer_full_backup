# Makefile for shacal1
ALGO_NAME := SHACAL1ENC

# comment out the following line for removement of shacal1 from the build process
BLOCK_CIPHERS += $(ALGO_NAME)

$(ALGO_NAME)_DIR      := shacal1/
$(ALGO_NAME)_INCDIR   := sha1/ bcal/
$(ALGO_NAME)_OBJ      := shacal1_enc.o sha1-asm.o
$(ALGO_NAME)_TESTBIN  := main-shacal1_enc-test.o $(CLI_STD)  \
                         nessie_bc_test.o nessie_common.o performance_test.o 
$(ALGO_NAME)_NESSIE_TEST      := "nessie"
$(ALGO_NAME)_PERFORMANCE_TEST := "performance"


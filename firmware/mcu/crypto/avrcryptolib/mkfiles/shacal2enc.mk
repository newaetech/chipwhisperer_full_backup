# Makefile for shacal2
ALGO_NAME := SHACAL2ENC

# comment out the following line for removement of shacal2 from the build process
BLOCK_CIPHERS += $(ALGO_NAME)

$(ALGO_NAME)_DIR      := shacal2/
$(ALGO_NAME)_INCDIR   := sha256/ bcal/
$(ALGO_NAME)_OBJ      := shacal2_enc.o sha256-asm.o
$(ALGO_NAME)_TESTBIN  := main-shacal2_enc-test.o $(CLI_STD) \
                         nessie_bc_test.o nessie_common.o performance_test.o 
$(ALGO_NAME)_NESSIE_TEST      := "nessie"
$(ALGO_NAME)_PERFORMANCE_TEST := "performance"


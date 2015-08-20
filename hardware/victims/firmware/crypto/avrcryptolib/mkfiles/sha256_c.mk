# Makefile for SHA256
ALGO_NAME := SHA256_C

# comment out the following line for removement of SHA256 from the build process
HASHES += $(ALGO_NAME)

$(ALGO_NAME)_DIR      := sha256/
$(ALGO_NAME)_INCDIR   := hfal/
$(ALGO_NAME)_OBJ      := sha256.o
$(ALGO_NAME)_TESTBIN  := main-sha256-test.o $(CLI_STD) $(HFAL_STD) hfal_sha256.o dump-asm.o dump-decl.o
$(ALGO_NAME)_NESSIE_TEST      := "nessie"
$(ALGO_NAME)_PERFORMANCE_TEST := "performance"


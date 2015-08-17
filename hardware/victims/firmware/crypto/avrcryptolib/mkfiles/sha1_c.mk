# Makefile for SHA1
ALGO_NAME := SHA1_C

# comment out the following line for removement of SHA1 from the build process
HASHES += $(ALGO_NAME)

$(ALGO_NAME)_DIR      := sha1/
$(ALGO_NAME)_INCDIR   := hfal/
$(ALGO_NAME)_OBJ      := sha1.o
$(ALGO_NAME)_TESTBIN  := main-sha1-test.o hfal_sha1.o dump-asm.o dump-decl.o $(CLI_STD) $(HFAL_STD)
$(ALGO_NAME)_NESSIE_TEST      := "nessie"
$(ALGO_NAME)_PERFORMANCE_TEST := "performance"


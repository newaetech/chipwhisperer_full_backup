# Makefile for SHA1
ALGO_NAME := SHA1

# comment out the following line for removement of SHA1 from the build process
HASHES += $(ALGO_NAME)

$(ALGO_NAME)_DIR      := sha1/
$(ALGO_NAME)_INCDIR   := hfal/
$(ALGO_NAME)_OBJ      := sha1-asm.o
$(ALGO_NAME)_TESTBIN  := main-sha1-test.o hfal_sha1.o $(CLI_STD) $(HFAL_STD) dump-decl.o dump-asm.o 
$(ALGO_NAME)_NESSIE_TEST      := "nessie"
$(ALGO_NAME)_PERFORMANCE_TEST := "performance"


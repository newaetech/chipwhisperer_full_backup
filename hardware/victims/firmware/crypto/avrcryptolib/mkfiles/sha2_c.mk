# Makefile for SHA-2
ALGO_NAME := SHA2_C

# comment out the following line for removement of SHA-2 from the build process
HASHES += $(ALGO_NAME)

$(ALGO_NAME)_DIR      := sha2/
$(ALGO_NAME)_INCDIR   := hfal/
$(ALGO_NAME)_OBJ      := sha224.o sha256.o sha384.o sha512.o \
                         sha2_small_common.o sha2_large_common.o 
$(ALGO_NAME)_TESTBIN := main-sha2-test.o $(CLI_STD) $(HFAL_STD) \
                        hfal_sha224.o hfal_sha256.o hfal_sha512.o hfal_sha384.o
$(ALGO_NAME)_NESSIE_TEST      := "nessie"
$(ALGO_NAME)_PERFORMANCE_TEST := "performance"


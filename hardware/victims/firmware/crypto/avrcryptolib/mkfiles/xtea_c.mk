# Makefile for XTEA
ALGO_NAME := XTEA_C

# comment out the following line for removement of XTEA from the build process
BLOCK_CIPHERS += $(ALGO_NAME)

$(ALGO_NAME)_DIR      := xtea/
$(ALGO_NAME)_INCDIR   := bcal/
$(ALGO_NAME)_OBJ      := xtea.o
$(ALGO_NAME)_TESTBIN  := main-xtea-test.o bcal_xtea.o $(CLI_STD) $(BCAL_STD)
$(ALGO_NAME)_NESSIE_TEST      := "nessie"
$(ALGO_NAME)_PERFORMANCE_TEST := "performance"


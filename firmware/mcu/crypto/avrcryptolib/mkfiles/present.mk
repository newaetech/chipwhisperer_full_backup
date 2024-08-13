# Makefile for present
ALGO_NAME := PRESENT

# comment out the following line for removement of present from the build process
BLOCK_CIPHERS += $(ALGO_NAME)

$(ALGO_NAME)_DIR      := present/
$(ALGO_NAME)_INCDIR   := bcal/ memxor/
$(ALGO_NAME)_OBJ      := present_common.o present80.o present128.o memxor.o
$(ALGO_NAME)_TESTBIN  := main-present-test.o bcal_present80.o bcal_present128.o $(CLI_STD) $(BCAL_STD)
$(ALGO_NAME)_NESSIE_TEST      := "nessie"
$(ALGO_NAME)_PERFORMANCE_TEST := "performance"


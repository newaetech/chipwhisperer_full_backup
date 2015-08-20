# Makefile for SKIPJACK
ALGO_NAME := SKIPJACK

# comment out the following line for removement of skipjack from the build process
BLOCK_CIPHERS += $(ALGO_NAME)

$(ALGO_NAME)_DIR      := skipjack/
$(ALGO_NAME)_INCDIR   := bcal/
$(ALGO_NAME)_OBJ      := skipjack.o
$(ALGO_NAME)_TESTBIN  := main-skipjack-test.o bcal_skipjack.o $(CLI_STD) $(BCAL_STD) 
$(ALGO_NAME)_NESSIE_TEST      := "nessie"
$(ALGO_NAME)_PERFORMANCE_TEST := "performance"


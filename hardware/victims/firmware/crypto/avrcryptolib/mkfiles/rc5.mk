# Makefile for RC5
ALGO_NAME := RC5

# comment out the following line for removement of RC5 from the build process
BLOCK_CIPHERS += $(ALGO_NAME)

$(ALGO_NAME)_DIR      := rc5/
$(ALGO_NAME)_INCDIR   := bcal/
$(ALGO_NAME)_OBJ      := rc5.o
$(ALGO_NAME)_TESTBIN  := main-rc5-test.o bcal_rc5.o $(CLI_STD) $(BCAL_STD) 
$(ALGO_NAME)_NESSIE_TEST      := test nessie
$(ALGO_NAME)_PERFORMANCE_TEST := performance


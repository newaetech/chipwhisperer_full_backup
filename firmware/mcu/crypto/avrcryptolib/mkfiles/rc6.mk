# Makefile for RC6
ALGO_NAME := RC6

# comment out the following line for removement of RC6 from the build process
BLOCK_CIPHERS += $(ALGO_NAME)

$(ALGO_NAME)_DIR      := rc6/
$(ALGO_NAME)_INCDIR   := bcal/
$(ALGO_NAME)_OBJ      := rc6.o
$(ALGO_NAME)_TESTBIN  := main-rc6-test.o bcal_rc6.o $(CLI_STD) $(BCAL_STD) 
$(ALGO_NAME)_NESSIE_TEST      := test nessie
$(ALGO_NAME)_PERFORMANCE_TEST := performance


# Makefile for Mickey128
ALGO_NAME := MICKEY128

# comment out the following line for removement of Mickey128 from the build process
STREAM_CIPHERS += $(ALGO_NAME)

$(ALGO_NAME)_DIR      := mickey128/
$(ALGO_NAME)_OBJ      := mickey128.o
$(ALGO_NAME)_INCDIR   := memxor/ scal/
$(ALGO_NAME)_TESTBIN  := main-mickey128-test.o $(CLI_STD) $(SCAL_STD) scal_mickey128.o 
$(ALGO_NAME)_NESSIE_TEST      := "nessie"
$(ALGO_NAME)_PERFORMANCE_TEST := "performance"
$(ALGO_NAME)_DEF := NESSIE_ESTREAM=1

# Makefile for A5/1 (from GSM)
ALGO_NAME := A51

# comment out the following line for removement of A5/1 from the build process
STREAM_CIPHERS += $(ALGO_NAME)

$(ALGO_NAME)_OBJ      := A5_1.o
$(ALGO_NAME)_DIR      := a51/
$(ALGO_NAME)_INCDIR   := memxor/ scal/
$(ALGO_NAME)_TESTBIN  := main-a5_1-test.o $(CLI_STD) $(SCAL_STD)
$(ALGO_NAME)_NESSIE_TEST      := "nessie"
$(ALGO_NAME)_PERFORMANCE_TEST := "performance"


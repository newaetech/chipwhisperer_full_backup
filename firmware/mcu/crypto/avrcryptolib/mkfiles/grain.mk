# Makefile for Grain
ALGO_NAME := GRAIN

# comment out the following line for removement of Grain from the build process
STREAM_CIPHERS += $(ALGO_NAME)

$(ALGO_NAME)_DIR      := grain/
$(ALGO_NAME)_OBJ      := grain.o
$(ALGO_NAME)_INCDIR   := memxor/ scal/
$(ALGO_NAME)_TESTBIN  := main-grain-test.o $(CLI_STD) $(SCAL_STD) scal_grain.o
$(ALGO_NAME)_NESSIE_TEST      := "nessie"
$(ALGO_NAME)_PERFORMANCE_TEST := "performance"
$(ALGO_NAME)_DEF := NESSIE_ESTREAM=1

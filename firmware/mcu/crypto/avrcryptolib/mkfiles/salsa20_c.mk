# Makefile for Salsa20
ALGO_NAME := SALSA20_C

# comment out the following line for removement of Salsa20 from the build process
STREAM_CIPHERS += $(ALGO_NAME)

$(ALGO_NAME)_DIR      := salsa20/
$(ALGO_NAME)_OBJ      := salsa20.o
$(ALGO_NAME)_INCDIR   := memxor/ scal/
$(ALGO_NAME)_TESTBIN  := main-salsa20-test.o $(CLI_STD) $(SCAL_STD) scal_salsa20.o
$(ALGO_NAME)_NESSIE_TEST      := "nessie"
$(ALGO_NAME)_PERFORMANCE_TEST := "performance"


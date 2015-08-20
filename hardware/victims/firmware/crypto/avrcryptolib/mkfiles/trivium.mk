# Makefile for Trivium
ALGO_NAME := TRIVIUM

# comment out the following line for removement of Trivium from the build process
STREAM_CIPHERS += $(ALGO_NAME)

$(ALGO_NAME)_DIR      := trivium/
$(ALGO_NAME)_OBJ      := trivium.o
$(ALGO_NAME)_INCDIR   := memxor/ scal/
$(ALGO_NAME)_TESTBIN  := main-trivium-test.o $(CLI_STD) $(SCAL_STD) scal_trivium.o 
$(ALGO_NAME)_NESSIE_TEST      := "nessie"
$(ALGO_NAME)_PERFORMANCE_TEST := "performance"
$(ALGO_NAME)_DEF       := NESSIE_ESTREAM=1

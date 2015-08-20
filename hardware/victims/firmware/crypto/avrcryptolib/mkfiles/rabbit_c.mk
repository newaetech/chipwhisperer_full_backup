# Makefile for Rabbit
ALGO_NAME := RABBIT_C

# comment out the following line for removement of Rabbit from the build process
STREAM_CIPHERS += $(ALGO_NAME)

$(ALGO_NAME)_DIR      := rabbit/
$(ALGO_NAME)_OBJ      := rabbit_c.o
$(ALGO_NAME)_INCDIR   := memxor/ scal/
$(ALGO_NAME)_TESTBIN  := main-rabbit-test.o $(CLI_STD) $(SCAL_STD) scal_rabbit.o
$(ALGO_NAME)_NESSIE_TEST      := "nessie"
$(ALGO_NAME)_PERFORMANCE_TEST := "performance"
$(ALGO_NAME)_DEF := ESTREAM=0

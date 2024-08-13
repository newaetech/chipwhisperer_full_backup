# Makefile for BigInt
ALGO_NAME := BIGINT

# comment out the following line for removement of BigInt from the build process
AUX += $(ALGO_NAME)

$(ALGO_NAME)_DIR      := bigint/
$(ALGO_NAME)_INCDIR   := memxor/ noekeon/
$(ALGO_NAME)_OBJ      := bigint-stub.o bigint_io.o bigint_asm.o 
$(ALGO_NAME)_TESTBIN  := main-bigint-test.o $(CLI_STD)  \
                         performance_test.o noekeon_asm.o noekeon_prng.o memxor.o
			 
$(ALGO_NAME)_PERFORMANCE_TEST := performance


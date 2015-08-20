# Makefile for Entropium PRNG
ALGO_NAME := ENTROPIUM

# comment out the following line for removement of PRNG from the build process
PRNGS += $(ALGO_NAME)

$(ALGO_NAME)_DIR      := entropium/
$(ALGO_NAME)_INCDIR   := sha256/
$(ALGO_NAME)_OBJ      := entropium.o sha256-asm.o
$(ALGO_NAME)_TESTBIN  := main-entropium-test.o $(CLI_STD) performance_test.o
                         
$(ALGO_NAME)_NESSIE_TEST      := "nessie"
$(ALGO_NAME)_PERFORMANCE_TEST := "performance"


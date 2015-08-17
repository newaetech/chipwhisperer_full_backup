# Makefile for Blake
ALGO_NAME := BLAKE_C

# comment out the following line for removement of Blake from the build process
HASHES += $(ALGO_NAME)

$(ALGO_NAME)_DIR      := blake/
$(ALGO_NAME)_INCDIR   := memxor/ hfal/
$(ALGO_NAME)_OBJ      := blake_small.o blake_large.o blake_common.o memxor.o
$(ALGO_NAME)_TESTBIN  := main-blake-test.o hfal_blake_small.o hfal_blake_large.o $(CLI_STD) $(HFAL_STD)
$(ALGO_NAME)_NESSIE_TEST      := test nessie
$(ALGO_NAME)_PERFORMANCE_TEST := performance


# Makefile for BlueMidnightWish
ALGO_NAME := BMW

# comment out the following line for removement of BlueMidnightWish from the build process
HASHES += $(ALGO_NAME)

$(ALGO_NAME)_DIR      := bmw/
$(ALGO_NAME)_INCDIR   := hfal/
$(ALGO_NAME)_OBJ      := bmw_small-asm.o bmw_large.o 
$(ALGO_NAME)_TESTBIN  := main-bmw-test.o hfal_bmw_small.o hfal_bmw_large.o $(CLI_STD) $(HFAL_STD)
$(ALGO_NAME)_NESSIE_TEST      := test nessie
$(ALGO_NAME)_PERFORMANCE_TEST := performance


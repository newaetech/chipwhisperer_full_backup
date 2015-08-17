# Makefile for CubeHash 
ALGO_NAME := CUBEHASH_C

# comment out the following line for removement of CubeHash from the build process
HASHES += $(ALGO_NAME)

$(ALGO_NAME)_DIR      := cubehash/
$(ALGO_NAME)_INCDIR   := memxor/ hfal/
$(ALGO_NAME)_OBJ      := cubehash.o cubehash_rotates.o memxor.o xchg.o
$(ALGO_NAME)_TESTBIN  := main-cubehash-test.o hfal_cubehash.o  $(CLI_STD) $(HFAL_STD)
$(ALGO_NAME)_NESSIE_TEST      := test nessie
$(ALGO_NAME)_PERFORMANCE_TEST := performance


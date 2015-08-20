# Makefile for JH
ALGO_NAME := JH_SIMPLE_SMALL_C

# comment out the following line for removement of JH from the build process
HASHES += $(ALGO_NAME)

$(ALGO_NAME)_DIR      := jh/
$(ALGO_NAME)_INCDIR   := hfal/ memxor/
$(ALGO_NAME)_OBJ      := jh_simple_small_core.o jh_simple_aux.o  memxor.o 
$(ALGO_NAME)_TESTBIN  := hfal_jh.o main-jh-test.o $(CLI_STD) $(HFAL_STD)
$(ALGO_NAME)_NESSIE_TEST      := test nessie
$(ALGO_NAME)_PERFORMANCE_TEST := performance


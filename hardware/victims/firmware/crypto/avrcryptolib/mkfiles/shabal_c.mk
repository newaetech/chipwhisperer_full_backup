# Makefile for Shabal
ALGO_NAME := SHABAL_C

# comment out the following line for removement of Shabal from the build process
HASHES += $(ALGO_NAME)

$(ALGO_NAME)_DIR      := shabal/
$(ALGO_NAME)_INCDIR   := hfal/
$(ALGO_NAME)_OBJ      := shabal.o shabal192.o shabal224.o shabal256.o shabal384.o shabal512.o
$(ALGO_NAME)_TESTBIN  := main-shabal-test.o hfal_shabal.o $(CLI_STD) $(HFAL_STD)
$(ALGO_NAME)_NESSIE_TEST      := test nessie
$(ALGO_NAME)_PERFORMANCE_TEST := performance


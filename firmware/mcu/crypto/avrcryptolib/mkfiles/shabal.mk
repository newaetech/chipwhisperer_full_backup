# Makefile for Shabal
ALGO_NAME := SHABAL

# comment out the following line for removement of Shabal from the build process
HASHES += $(ALGO_NAME)

$(ALGO_NAME)_DIR      := shabal/
$(ALGO_NAME)_INCDIR   := hfal/
$(ALGO_NAME)_OBJ      := shabal-asm.o shabal192-asm.o shabal224-asm.o \
                         shabal256-asm.o shabal384-asm.o shabal512-asm.o
$(ALGO_NAME)_TESTBIN  := main-shabal-test.o hfal_shabal.o $(CLI_STD) $(HFAL_STD)
$(ALGO_NAME)_NESSIE_TEST      := test nessie
$(ALGO_NAME)_PERFORMANCE_TEST := performance


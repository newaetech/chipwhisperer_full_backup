# Makefile for noekeon
ALGO_NAME := OMAC_NOEKEON

# comment out the following line for removement of noekeon from the build process
MACS += $(ALGO_NAME)

$(ALGO_NAME)_DIR      := noekeon/
$(ALGO_NAME)_INCDIR   := memxor/ mfal/
$(ALGO_NAME)_OBJ      := noekeon_asm.o omac_noekeon.o memxor.o
$(ALGO_NAME)_TESTBIN  := main-omac-noekeon-test.o $(CLI_STD)  \
                         nessie_mac_test.o nessie_common.o performance_test.o
$(ALGO_NAME)_NESSIE_TEST      := test nessie
$(ALGO_NAME)_PERFORMANCE_TEST := performance


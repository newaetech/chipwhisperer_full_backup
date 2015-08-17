# Makefile for Skein
ALGO_NAME := SKEIN_C

# comment out the following line for removement of Skein from the build process
HASHES += $(ALGO_NAME)

$(ALGO_NAME)_DIR      := skein/
$(ALGO_NAME)_INCDIR   := memxor/ hfal/
$(ALGO_NAME)_OBJ      := threefish256_enc.o threefish512_enc.o threefish1024_enc.o threefish_mix_c.o\
                         ubi256.o ubi512.o ubi1024.o memxor.o skein256.o skein512.o skein1024.o
$(ALGO_NAME)_TESTBIN  := main-skein-test.o hfal_skein256.o hfal_skein512.o hfal_skein1024.o $(CLI_STD) $(HFAL_STD)
$(ALGO_NAME)_NESSIE_TEST      := test nessie
$(ALGO_NAME)_PERFORMANCE_TEST := performance


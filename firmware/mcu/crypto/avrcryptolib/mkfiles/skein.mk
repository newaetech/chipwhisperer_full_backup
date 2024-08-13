# Makefile for Skein
ALGO_NAME := SKEIN

# comment out the following line for removement of Skein from the build process
HASHES += $(ALGO_NAME)

$(ALGO_NAME)_DIR      := skein/
$(ALGO_NAME)_INCDIR   := hfal/
$(ALGO_NAME)_OBJ      := threefish_mix.o \
                         threefish256_enc_asm.o  ubi256_asm.o  skein256_asm.o \
                         threefish512_enc_asm.o  ubi512_asm.o  skein512_asm.o \
			 threefish1024_enc_asm.o ubi1024_asm.o skein1024_asm.o
$(ALGO_NAME)_TESTBIN  := main-skein-test.o hfal_skein256.o hfal_skein512.o hfal_skein1024.o $(CLI_STD) $(HFAL_STD)
$(ALGO_NAME)_NESSIE_TEST      := test nessie
$(ALGO_NAME)_PERFORMANCE_TEST := performance


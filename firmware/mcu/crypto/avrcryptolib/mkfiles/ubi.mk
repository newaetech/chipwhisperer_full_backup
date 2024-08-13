# Makefile for UBI
ALGO_NAME := UBI

# comment out the following line for removement of ubi from the build process
AUX += $(ALGO_NAME)

$(ALGO_NAME)_DIR      := skein/
$(ALGO_NAME)_INCDIR   := memxor/ bcal/
$(ALGO_NAME)_OBJ      := threefish_mix.o threefish256_enc_asm.o ubi256_asm.o \
                         threefish512_enc_asm.o ubi512_asm.o threefish1024_enc_asm.o \
                         ubi1024_asm.o memxor.o
$(ALGO_NAME)_TESTBIN  := main-ubi-test.o $(CLI_STD) \
                         nessie_common.o performance_test.o
$(ALGO_NAME)_NESSIE_TEST      := test nessie
$(ALGO_NAME)_PERFORMANCE_TEST := performance


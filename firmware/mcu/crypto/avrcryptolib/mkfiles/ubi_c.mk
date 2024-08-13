# Makefile for UBI
ALGO_NAME := UBI_C

# comment out the following line for removement of ubi from the build process
AUX += $(ALGO_NAME)

$(ALGO_NAME)_DIR      := skein/
$(ALGO_NAME)_INCDIR   := memxor/ bcal/
$(ALGO_NAME)_OBJ      := threefish256_enc.o threefish512_enc.o threefish1024_enc.o threefish_mix_c.o\
                         ubi256.o ubi512.o ubi1024.o memxor.o
$(ALGO_NAME)_TESTBIN  := main-ubi-test.o $(CLI_STD) \
                         nessie_common.o performance_test.o
$(ALGO_NAME)_NESSIE_TEST      := test nessie
$(ALGO_NAME)_PERFORMANCE_TEST := performance


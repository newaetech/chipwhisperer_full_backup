# Makefile for threefish
ALGO_NAME := THREEFISH_C

# comment out the following line for removement of threefish from the build process
BLOCK_CIPHERS += $(ALGO_NAME)

$(ALGO_NAME)_DIR      := skein/
$(ALGO_NAME)_INCDIR   := bcal/
$(ALGO_NAME)_OBJ      := threefish256_enc.o threefish512_enc.o threefish1024_enc.o threefish_mix_c.o \
                         threefish_invmix_c.o threefish256_dec.o threefish512_dec.o threefish1024_dec.o
$(ALGO_NAME)_TESTBIN  := main-threefish-test.o bcal_threefish256.o bcal_threefish512.o bcal_threefish1024.o $(CLI_STD) $(BCAL_STD)
$(ALGO_NAME)_NESSIE_TEST      := test nessie
$(ALGO_NAME)_PERFORMANCE_TEST := performance


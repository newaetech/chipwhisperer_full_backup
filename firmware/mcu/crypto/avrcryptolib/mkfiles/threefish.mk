# Makefile for threefish
ALGO_NAME := THREEFISH

# comment out the following line for removement of threefish from the build process
BLOCK_CIPHERS += $(ALGO_NAME)

$(ALGO_NAME)_DIR      := skein/
$(ALGO_NAME)_INCDIR   := bcal/
$(ALGO_NAME)_OBJ      := threefish256_enc_asm.o threefish512_enc_asm.o threefish1024_enc_asm.o\
                         threefish_mix.o threefish_invmix.o \
			 threefish256_dec_asm.o threefish512_dec_asm.o threefish1024_dec_asm.o
$(ALGO_NAME)_TESTBIN  := main-threefish-test.o bcal_threefish256.o bcal_threefish512.o bcal_threefish1024.o $(CLI_STD) $(BCAL_STD)
$(ALGO_NAME)_NESSIE_TEST      := test nessie
$(ALGO_NAME)_PERFORMANCE_TEST := performance


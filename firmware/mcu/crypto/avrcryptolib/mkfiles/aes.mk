# Makefile for AES
ALGO_NAME := AES

# comment out the following line for removement of AES from the build process
BLOCK_CIPHERS += $(ALGO_NAME)

$(ALGO_NAME)_DIR      := aes/
$(ALGO_NAME)_INCDIR   := memxor/ gf256mul/ bcal/
$(ALGO_NAME)_OBJ      := aes_enc-asm.o aes_dec-asm.o aes_sbox-asm.o aes_invsbox-asm.o  \
                         aes_keyschedule-asm.o 
$(ALGO_NAME)_TESTBIN  := main-aes-test.o $(CLI_STD) $(BCAL_STD)  \
                         bcal_aes128.o bcal_aes192.o bcal_aes256.o bcal-cbc.o \
                         dump-asm.o dump-decl.o bcal-cfb_byte.o       \
                         bcal-cfb_bit.o bcal-ofb.o bcal-ctr.o bcal-cmac.o cmacvs.o         \
                         bcal-eax.o memxor.o
$(ALGO_NAME)_NESSIE_TEST      := test nessie
$(ALGO_NAME)_PERFORMANCE_TEST := performance


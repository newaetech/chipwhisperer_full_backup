# Makefile for AES
ALGO_NAME := AES128_C

# comment out the following line for removement of AES from the build process
BLOCK_CIPHERS += $(ALGO_NAME)

$(ALGO_NAME)_DIR      := aes/
$(ALGO_NAME)_INCDIR   := gf256mul/ bcal/
$(ALGO_NAME)_OBJ      := aes_enc.o aes_dec.o aes_sbox.o aes_invsbox.o  \
                         aes_keyschedule.o gf256mul.o aes128_enc.o aes128_dec.o
$(ALGO_NAME)_TESTBIN  := main-aes128-test.o $(CLI_STD) $(BCAL_STD) \
                         bcal_aes128.o
$(ALGO_NAME)_NESSIE_TEST      := test nessie
$(ALGO_NAME)_PERFORMANCE_TEST := performance


# Makefile for MUGI
ALGO_NAME := MUGI_C

# comment out the following line for removement of MUGI_C from the build process
STREAM_CIPHERS += $(ALGO_NAME)

$(ALGO_NAME)_DIR      := mugi/
$(ALGO_NAME)_INCDIR   := memxor/ aes/ gf256mul/
$(ALGO_NAME)_OBJ      := mugi.o gf256mul.o aes_sbox.o memxor.o
$(ALGO_NAME)_TESTBIN  := main-mugi-test.o $(CLI_STD)  \
                         nessie_stream_test.o nessie_common.o  \
                         performance_test.o
$(ALGO_NAME)_NESSIE_TEST      := "nessie"
$(ALGO_NAME)_PERFORMANCE_TEST := "performance"


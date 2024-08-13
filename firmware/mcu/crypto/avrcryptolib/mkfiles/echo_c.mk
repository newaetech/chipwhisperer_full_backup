# Makefile for ECHO 
ALGO_NAME := ECHO_C

# comment out the following line for removement of ECHO from the build process
HASHES += $(ALGO_NAME)

$(ALGO_NAME)_DIR      := echo/
$(ALGO_NAME)_INCDIR   := aes/ memxor/ gf256mul/ hfal/
$(ALGO_NAME)_OBJ      := echo.o memxor.o aes_enc_round-asm.o aes_sbox-asm.o gf256mul.o
$(ALGO_NAME)_TESTBIN  := main-echo-test.o hfal_echo.o  $(CLI_STD) $(HFAL_STD)
$(ALGO_NAME)_NESSIE_TEST      := test nessie
$(ALGO_NAME)_PERFORMANCE_TEST := performance


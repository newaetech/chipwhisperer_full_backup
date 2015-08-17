# Makefile for RSA
ALGO_NAME := RSAES_OAEP

# comment out the following line for removement of RSA from the build process
PK_CIPHERS += $(ALGO_NAME)

$(ALGO_NAME)_DIR      := rsa/
$(ALGO_NAME)_INCDIR   := memxor/ bigint/ noekeon/ hfal/ sha1/ mgf1/
$(ALGO_NAME)_OBJ      := bigint.o bigint_io.o rsa_basic.o rsaes_oaep.o mgf1.o hfal-basic.o hfal_sha1.o sha1.o
$(ALGO_NAME)_TESTBIN := main-rsaes_oaep-test.o $(CLI_STD) random_dummy.o \
                         noekeon.o noekeon_prng.o memxor.o 
			 
$(ALGO_NAME)_PERFORMANCE_TEST := performance


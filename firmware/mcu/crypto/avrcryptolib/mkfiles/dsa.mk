# Makefile for DSA
ALGO_NAME := DSA

# comment out the following line for removement of DSA from the build process
SIGNATURE += $(ALGO_NAME)

$(ALGO_NAME)_DIR      := dsa/
$(ALGO_NAME)_INCDIR   := memxor/ bigint/ sha1/ noekeon/ base64/ hfal/
$(ALGO_NAME)_OBJ      := bigint.o bigint_io.o sha1-asm.o dsa_sign.o dsa_verify.o dsa_key_blob.o base64_enc.o
$(ALGO_NAME)_TESTBIN  := main-dsa-test.o $(CLI_STD) hfal_sha1.o $(HFAL_STD) \
                         noekeon_asm.o noekeon_prng.o memxor.o 
			 
$(ALGO_NAME)_PERFORMANCE_TEST := performance


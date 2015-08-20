# Makefile for MD5
ALGO_NAME := MD5_C

# comment out the following line for removement of MD5 from the build process
HASHES += $(ALGO_NAME)

$(ALGO_NAME)_DIR      := md5/
$(ALGO_NAME)_INCDIR   := hfal/
$(ALGO_NAME)_OBJ      := md5.o md5_sbox.o
$(ALGO_NAME)_TESTBIN  := main-md5-test.o hfal_md5.o $(CLI_STD) $(HFAL_STD)
$(ALGO_NAME)_NESSIE_TEST      := "nessie"
$(ALGO_NAME)_PERFORMANCE_TEST := "performance"


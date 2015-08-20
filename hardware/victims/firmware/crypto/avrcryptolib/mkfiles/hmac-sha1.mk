# Makefile for HMAC-SHA1
ALGO_NAME := HMAC-SHA1

# comment out the following line for removement of HMAC-SHA1 from the build process
MACS += $(ALGO_NAME)

$(ALGO_NAME)_DIR      := hmac-sha1/
$(ALGO_NAME)_INCDIR   := sha1/
$(ALGO_NAME)_OBJ      := hmac-sha1.o sha1-asm.o
$(ALGO_NAME)_TESTBIN  := main-hmac-sha1-test.o $(CLI_STD) \
                         nessie_mac_test.o nessie_common.o
$(ALGO_NAME)_NESSIE_TEST      := "nessie"
$(ALGO_NAME)_PERFORMANCE_TEST := "performance"


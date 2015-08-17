# Makefile for HMAC-SHA256
ALGO_NAME := HMAC-SHA256

# comment out the following line for removement of HMAC-SHA256 from the build process
MACS += $(ALGO_NAME)

$(ALGO_NAME)_DIR      := hmac-sha256/
$(ALGO_NAME)_INCDIR   := sha256/
$(ALGO_NAME)_OBJ      := hmac-sha256.o sha256-asm.o
$(ALGO_NAME)_TESTBIN  := main-hmac-sha256-test.o $(CLI_STD) \
                         nessie_mac_test.o nessie_common.o
$(ALGO_NAME)_NESSIE_TEST      := "nessie"
$(ALGO_NAME)_PERFORMANCE_TEST := "performance"


# Makefile for MQQ160-sign
ALGO_NAME := MQQ160-SIGN

# comment out the following line for removement of MQQ160-sign from the build process
SIGNATURE += $(ALGO_NAME)

$(ALGO_NAME)_DIR      := mqq-sign/
$(ALGO_NAME)_OBJ      := mqq160-sign-asm.o mqq160-sign_P-asm.o mqq160-sign_testkey.o 
$(ALGO_NAME)_TESTBIN  := main-mqq160-sign-test.o performance_test.o stack_measuring.o $(CLI_STD)
$(ALGO_NAME)_NESSIE_TEST      := test 
$(ALGO_NAME)_PERFORMANCE_TEST := performance


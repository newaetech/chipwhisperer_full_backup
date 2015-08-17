# Makefile for MQQ160-sign
ALGO_NAME := MQQ160-SIGN_C

# comment out the following line for removement of MQQ160-sign from the build process
SIGNATURE += $(ALGO_NAME)

$(ALGO_NAME)_DIR      := mqq-sign/
$(ALGO_NAME)_INCDIR   := memxor/ 
$(ALGO_NAME)_OBJ      := mqq160-sign.o mqq160-sign_P.o mqq160-sign_testkey.o memxor.o memxor_p.o
$(ALGO_NAME)_TESTBIN  := main-mqq160-sign-test.o performance_test.o stack_measuring.o $(CLI_STD)
$(ALGO_NAME)_NESSIE_TEST      := test 
$(ALGO_NAME)_PERFORMANCE_TEST := performance


#pragma once

#include <stdint.h>
#include <numicro_8051.h>
#define _SELECT_ASSERT_FUNC(x, EXPR, MSG, ASSERT_MACRO, ...) ASSERT_MACRO
#define STATIC_ASSERT_SIMPLE(EXPR)      _Static_assert(EXPR, "unspecified message")
#define STATIC_ASSERT_MSG(EXPR, MSG)    _Static_assert(EXPR, MSG)
#define STATIC_ASSERT(...)                                                                          \
    _SELECT_ASSERT_FUNC(x, ##__VA_ARGS__,                                                           \
                        STATIC_ASSERT_MSG(__VA_ARGS__),                                             \
                        STATIC_ASSERT_SIMPLE(__VA_ARGS__))


#define     CID_READ				0x0B
#define     DID_READ				0x0C

#define     ERASE_APROM				0x22
#define     READ_APROM				0x00
#define     PROGRAM_APROM			0x21
#define     READ_CFG				0xC0
#define     PROGRAM_CFG				0xE1
#define		READ_UID				0x04


/*   general macros                                                          */
#define _enable_TA TA=0xAA;TA=0x55 // Enable writes to a TA-protected register without disabling interrupts (ensure interrupts are disabled before writing to a TA-protected register)
#define _asgn_TAR(reg, val) _enable_TA;reg=val    // Assign (=) value to TA-protected register without disabling interrupts (ensure interrupts are disabled before using)
#define _anda_TAR(reg, val) _enable_TA;reg&=val   // And-assign (&=) value to TA-protected register without disabling interrupts (ensure interrupts are disabled before using)
#define _nanda_TAR(reg, val) _enable_TA;reg&=~val // Nand-assign (&=~) value to TA-protected register without disabling interrupts (ensure interrupts are disabled before using)
#define _ora_TAR(reg, val) _enable_TA;reg|=val    // Or-assign (|=) value to TA-protected register without disabling interrupts (ensure interrupts are disabled before using)
#define _xora_TAR(reg, val) _enable_TA;reg^=val   // Xor-assign (^=) value to TA-protected register without disabling interrupts (ensure interrupts are disabled before using)

#define asgn_TAR(reg, val) BIT_TMP=EA;EA=0;_asgn_TAR(reg, val);EA=BIT_TMP // Assign (=) value to TA-protected register
#define anda_TAR(reg, val) BIT_TMP=EA;EA=0;_anda_TAR(reg, val);EA=BIT_TMP // And-assign (&=) value to TA-protected register
#define nanda_TAR(reg, val) BIT_TMP=EA;EA=0;_nanda_TAR(reg, val);EA=BIT_TMP // Nand-assign (&=~) value to TA-protected register
#define ora_TAR(reg, val) BIT_TMP=EA;EA=0;_ora_TAR(reg, val);EA=BIT_TMP // Or-assign (|=) value to TA-protected register
#define xora_TAR(reg, val) BIT_TMP=EA;EA=0;_xora_TAR(reg, val);EA=BIT_TMP // Xor-assign (^=) value to TA-protected register

#define tmp_clr_EA(statement) BIT_TMP=EA;EA=0;statement;EA=BIT_TMP // Disable interrupts for the length of the statement, then enables them again if they were already enabled

#define DIV_ROUND_CLOSEST_UNSIGNED(n, d) (((n) + (d) / 2) / (d))
#define DIV_ROUND_CLOSEST(n, d) ((((n) < 0) == ((d) < 0)) ? DIV_ROUND_CLOSEST_UNSIGNED(n, d) : (((n) - (d) / 2) / (d)))



void  InitialUART0_Timer1(UINT32 u32Baudrate); //T1M = 1, SMOD = 1
void  InitialUART0_Timer3(UINT32 u32Baudrate); //Timer3 as Baudrate, SMOD=1, Prescale=0
void  InitialUART1_Timer3(UINT32 u32Baudrate);
void  Send_Data_To_UART0(UINT8 c);
UINT8 Receive_Data_From_UART0(void);
void  Send_Data_To_UART1(UINT8 c);
UINT8 Receive_Data_From_UART1(void);

// unsigned char _sdcc_external_startup (void);

extern BIT BIT_TMP;

#include "numicro_8051.h"
#include "Common.h"
#include "Delay.h"

__bit BIT_TMP;

//----------------------------------------------------------------------------------
// UART0 baud rate initial setting
//----------------------------------------------------------------------------------
#define DIV_ROUND_CLOSEST_POS(n, d) (((n) - (d) / 2) / (d))
#define DIV_ROUND_CLOSEST(n, d) ((((n) < 0) == ((d) < 0)) ? (((n) + (d) / 2) / (d)) : (((n) - (d) / 2) / (d)))

#ifdef FOSC_110592
#define FSYS_DIV16 691200
#endif
#ifdef FOSC_160000
#define FSYS_DIV16 1000000
#endif
#ifdef FOSC_166000
#define FSYS_DIV16 1037500
#endif
#ifdef FOSC_184320
#define FSYS_DIV16 1152000
#endif
#ifdef FOSC_200000
#define FSYS_DIV16 1250000
#endif
#ifdef FOSC_221184
#define FSYS_DIV16 1382400
#endif
#ifdef FOSC_240000
#define FSYS_DIV16 1500000
#endif
void InitialUART0_Timer1(UINT32 u32Baudrate) // T1M = 1, SMOD = 1
{
    P06_QUASI_MODE; // Setting UART pin as Quasi mode for transmit
    P07_QUASI_MODE; // Setting UART pin as Quasi mode for transmit

    SCON = 0x50;  // UART0 Mode1,REN=1,TI=1
    TMOD |= 0x20; // Timer1 Mode1

    set_PCON_SMOD; // UART0 Double Rate Enable
    set_CKCON_T1M;
    clr_T3CON_BRCK; // Serial port 0 baud rate clock source = Timer1

    TH1 = 256 - DIV_ROUND_CLOSEST(FSYS_DIV16, u32Baudrate);
    set_TCON_TR1;
    set_SCON_TI; // For printf function must setting TI = 1
}
//---------------------------------------------------------------
void InitialUART0_Timer3(UINT32 u32Baudrate) // use timer3 as Baudrate generator
{
    P06_QUASI_MODE; // Setting UART pin as Quasi mode for transmit
    P07_QUASI_MODE; // Setting UART pin as Quasi mode for transmit

    SCON = 0x50;    // UART0 Mode1,REN=1,TI=1
    set_PCON_SMOD;  // UART0 Double Rate Enable
    T3CON &= 0xF8;  // T3PS2=0,T3PS1=0,T3PS0=0(Prescale=1)
    set_T3CON_BRCK; // UART0 baud rate clock source = Timer3

    RH3 = HIBYTE(65536 - DIV_ROUND_CLOSEST(FSYS_DIV16, u32Baudrate));
    RL3 = LOBYTE(65536 - DIV_ROUND_CLOSEST(FSYS_DIV16, u32Baudrate));
    set_T3CON_TR3; // Trigger Timer3
    set_SCON_TI;  // For printf function must setting TI = 1
}

UINT8 Receive_Data_From_UART0(void)
{
    UINT8 c;
    while (!RI)
        ;
    c = SBUF;
    RI = 0;
    return (c);
}

void Send_Data_To_UART0(UINT8 c)
{
    TI = 0;
    SBUF = c;
    while (TI == 0)
        ;
}

//----------------------------------------------------------------------------------
// UART1 baud rate initial setting
//----------------------------------------------------------------------------------
void InitialUART1_Timer3(UINT32 u32Baudrate) // use timer3 as Baudrate generator
{
    P02_QUASI_MODE; // Setting UART pin as Quasi mode for transmit
    P16_QUASI_MODE; // Setting UART pin as Quasi mode for transmit

    SCON_1 = 0x50; // UART1 Mode1,REN_1=1,TI_1=1
    T3CON = 0x08;  // T3PS2=0,T3PS1=0,T3PS0=0(Prescale=1), UART1 in MODE 1

    RH3 = HIBYTE(65536 - DIV_ROUND_CLOSEST(FSYS_DIV16, u32Baudrate));
    RL3 = LOBYTE(65536 - DIV_ROUND_CLOSEST(FSYS_DIV16, u32Baudrate));
    set_T3CON_TR3; // Trigger Timer3
}

UINT8 Receive_Data_From_UART1(void)
{
    UINT8 c;

    while (!RI_1)
        ;
    c = SBUF_1;
    RI_1 = 0;
    return (c);
}

void Send_Data_To_UART1(UINT8 c)
{
    TI_1 = 0;
    SBUF_1 = c;
    while (TI_1 == 0)
        ;
}

/*==========================================================================*/
#ifdef SW_Reset
void SW_Reset(void)
{
    TA = 0xAA;
    TA = 0x55;
    set_SWRST;
}
#endif

unsigned char
_sdcc_external_startup(void)
{
    // power on reset disable, as recommended by N76E003 errata
    _enable_TA;
    PORDIS=0x5A;
    _enable_TA;
    PORDIS=0xA5;
    return 0;
}

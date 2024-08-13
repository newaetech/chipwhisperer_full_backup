#include <xc.h>
#include "uart.h"

void init_uart(void)
{
#ifdef AD1PCFG
    AD1PCFG = 0xFFFF;
#endif
    TRISBbits.TRISB5 = 1; //RX pin is PB.5
    TRISBbits.TRISB7 = 0; //TX pin is PB.7
    
    __builtin_write_OSCCONL(OSCCON & 0xbf);
    RPINR18bits.U1RXR = 5; /*U1RX = RP5 */
    RPOR3bits.RP7R = 3; /* U1TX */
    __builtin_write_OSCCONL(OSCCON | 0x40);
      
    // configure U1MODE
    U1MODEbits.UARTEN = 0;  // Bit15 TX, RX DISABLED, ENABLE at end of func
    U1MODEbits.USIDL = 0;   // Bit13 Continue in Idle
    U1MODEbits.IREN = 0;    // Bit12 No IR translation
    U1MODEbits.RTSMD = 0;   // Bit11 Simplex Mode
    U1MODEbits.UEN = 0;     // Bits8,9 TX,RX enabled, CTS,RTS not
    U1MODEbits.WAKE = 0;    // Bit7 No Wake up (since we don't sleep here)
    U1MODEbits.LPBACK = 0;  // Bit6 No Loop Back
    U1MODEbits.ABAUD = 0;   // Bit5 No Autobaud (would require sending '55')
    U1MODEbits.RXINV = 0;   // Bit4 IdleState = 1
    U1MODEbits.BRGH = 0;    // Bit3 16 clocks per bit period
    U1MODEbits.PDSEL = 0;   // Bits1,2 8bit, No Parity
    U1MODEbits.STSEL = 0;   // Bit0 One Stop Bit
    
    U1BRG = 5; /* 38400 baud @ 7.3728 MHz oscillator */
    
    // Load all values in for U1STA SFR
    U1STAbits.UTXISEL1 = 0; //Bit15 Int when Char is transferred (1/2 config!)
    U1STAbits.UTXINV = 0;   //Bit14 N/A, IRDA config
    U1STAbits.UTXISEL0 = 0; //Bit13 Other half of Bit15
    U1STAbits.UTXBRK = 0;   //Bit11 Disabled
    U1STAbits.UTXEN = 0;    //Bit10 TX pins controlled by periph
    U1STAbits.UTXBF = 0;    //Bit9 *Read Only Bit*
    U1STAbits.TRMT = 0;     //Bit8 *Read Only bit*
    U1STAbits.URXISEL = 0;  //Bits6,7 Int. on character recieved
    U1STAbits.ADDEN = 0;    //Bit5 Address Detect Disabled
    U1STAbits.RIDLE = 0;    //Bit4 *Read Only Bit*
    U1STAbits.PERR = 0;     //Bit3 *Read Only Bit*
    U1STAbits.FERR = 0;     //Bit2 *Read Only Bit*
    U1STAbits.OERR = 0;     //Bit1 *Read Only Bit*
    U1STAbits.URXDA = 0;    //Bit0 *Read Only Bit*
    

    IFS0bits.U1TXIF = 0;    // Clear the Transmit Interrupt Flag
    IEC0bits.U1TXIE = 0;    // Disable Transmit Interrupts
    IFS0bits.U1RXIF = 0;    // Clear the Recieve Interrupt Flag
    IEC0bits.U1RXIE = 0;    // Disable Recieve Interrupts
    
    U1MODEbits.UARTEN = 1;  // And turn the peripheral on
    U1STAbits.UTXEN = 1;
    U1STAbits.URXEN = 1;
}

void putch(char c)
{
    while(U1STAbits.UTXBF == 1);
    U1TXREG = c;
}

char getch(void)
{
    
    if (U1STA & (1<<1)){
        U1STA |= 1<<1;
    }
    
    char c;
    while(U1STAbits.URXDA == 0);   
    c = U1RXREG;    
    IFS0bits.U1RXIF = 0;
    return c;
}



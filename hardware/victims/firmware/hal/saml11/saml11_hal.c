#include "sam.h"

void platform_init(void)
{
     SystemInit();
     PORT_SEC->Group[0].DIRSET.reg = 1 << 23;
     PORT_SEC->Group[0].OUTSET.reg = 1 << 23;
}

void init_uart(void)
{
     //PORT_SEC->Group[0].DIRSET.reg = 1 << 8;
     //multiplexing needs to be enabled before setting where stuff is mux'd to
     PORT_SEC->Group[0].PINCFG[16].reg = 0b11; //enable input read, mux
     PORT_SEC->Group[0].PINCFG[17].reg = 0b11; //enable input read, mux
     PORT_SEC->Group[0].PMUX[8].reg = 3 | (3 << 4); //SERCOM_alt mux for PORT

     //DON"T FORGET TO ENABLE PERIPH CLOCK
     MCLK->APBCMASK.bit.SERCOM1_ = 1; //on by default?
     GCLK->PCHCTRL[11].reg = (1 << 6) | 0; //enable generic clock, use src 0

     //LSB first, RX on pad[1] (PA9), Tx on pad[0], internal clock
     SERCOM0->USART.CTRLA.reg = (1 << 30) | (1 << 20) | (0 << 16) | (1 << 2);
     //enable TX and RX

     SERCOM0->USART.BAUD.reg = 55470;

     //DEFAULT FOR ABOVE IS 8n1
     SERCOM0->USART.CTRLA.bit.ENABLE = 1; //enable
     SERCOM0->USART.CTRLB.reg = (1 << 17) | (1 << 16);
     while(SERCOM0->USART.SYNCBUSY.bit.CTRLB); //wait for tx and rx to be enabled
     while(SERCOM0->USART.SYNCBUSY.bit.ENABLE); //wait for enable to complete
     while(!(SERCOM0->USART.CTRLB.reg & (0b11 << 16))); //check for tx and rx enable
}

void putch(char c)
{
     //wait for data register to be empty
     while(!SERCOM0->USART.INTFLAG.bit.DRE);
     SERCOM0->USART.DATA.reg = c;
}

char getch(char c)
{
     //wait for data
     while(!SERCOM0->USART.INTFLAG.bit.RXC);
     return SERCOM0->USART.DATA.reg;
}

void trigger_setup(void)
{
     PORT_SEC->Group[0].DIRSET.reg = 1 << 22;
     PORT_SEC->Group[0].OUTCLR.reg = 1 << 22;
}

void trigger_low(void)
{
     PORT_SEC->Group[0].OUTCLR.reg = 1 << 22;
}

void trigger_high(void)
{
     PORT_SEC->Group[0].OUTSET.reg = 1 << 22;
}

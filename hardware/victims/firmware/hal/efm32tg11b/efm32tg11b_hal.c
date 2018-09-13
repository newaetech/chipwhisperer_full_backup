#include <stdint.h>
#include "efm32tg11b_hal.h"
#include "em_device.h"

void platform_init()
{
     //clock broke for now
     CMU->HFBUSCLKEN0 = 0b100; //enable GPIO clock
     GPIO->P[3].MODEL = 4 << 28;
     GPIO->P[3].DOUT = 1 << 7;
}

void init_uart(void)
{
     CMU->HFPERCLKEN0 |= 1; //enable UART clock
     CMU->HFBUSCLKEN0 |= 0b100; //enable GPIO clock
     GPIO->P[4].MODEH = (4 << 8); //pin 10 push-pull, everything else input
     GPIO->P[4].DOUT = 1 << 10; //set PE10 high
     USART0->CTRL = 0; //UART mode
     USART0->FRAME = 0x05 | (1 << 12); //8n1
     USART0->CLKDIV = 7661; //38400 with 19MHz perip clock (I think this is right?)
     USART0->ROUTEPEN = 0b11; //enable TX and RX pins
     USART0->ROUTELOC0 = 0; //location 0 for tx and rx

     USART0->CMD = 0b101; //enable TX and RX
     uint32_t *status = &USART0->STATUS;
     while (!(*status & 0b11)) {
          USART0->CMD = 0b101;
     }; //wait for tx and rx enable
     while (*status & (1 << 4)); //check for transmitter tristated
}

void putch(char c)
{
     while (USART0->STATUS & (1 << 5)); //wait for transmission to finish
     USART0->TXDATA = c; //transmit data
}

char getch(void)
{
     while (USART0->STATUS & (1 << 7)); //wait for data
     return USART0->RXDATA;
}

void trigger_setup(void)
{
     CMU->HFBUSCLKEN0 = 0b100; //enable GPIO clock
     GPIO->P[2].MODEL = 4 << (4 * 6); //PB6 push pull
     GPIO->P[2].DOUT &= ~(1 << 6); //PB6 low
}
void trigger_low(void)
{
     GPIO->P[2].DOUT &= ~(1 << 6); //PB6 low
}
void trigger_high(void)
{
     GPIO->P[2].DOUT |= (1 << 6); //PB6 high
}

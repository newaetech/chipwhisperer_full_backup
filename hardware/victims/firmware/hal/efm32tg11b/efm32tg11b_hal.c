#include <stdint.h>
#include "efm32tg11b_hal.h"
#include "em_device.h"

//make flashing device lock bootloader
const uint32_t __attribute__((section (".debug_lock"))) user_lock_word[1] = {0xFFFFFFFD};
static uint8_t PT_KEY[16];

void platform_init(void)
{
     CMU->HFXOCTRL = 3; //xtal is ext clk
     CMU->OSCENCMD = 1 << 2; //enable hfxo
     while (!(CMU->STATUS & 1 << 2)); //wait for hfxo to be enabled
     while (!(CMU->STATUS & 1 << 3)); //wait for hfxo to be ready
     CMU->HFCLKSEL = 2; //select HFCLK as HFXO
}

void init_uart(void)
{
     CMU->HFPERCLKEN0 |= 1; //enable UART clock
     CMU->HFBUSCLKEN0 |= 0b100; //enable GPIO clock
     GPIO->P[4].MODEH = (4 << 8) | (1 << 12); //pin 10 push-pull, enable pin 11 input
     GPIO->P[4].DOUT = 1 << 10; //set PE10 high
     USART0->CTRL = 0; //UART mode
     USART0->FRAME = 0x05 | (1 << 12); //8n1
     USART0->CLKDIV = 2815; //38400 with 7.37MHz perip clock (I think this is right?)
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
     while (!(USART0->STATUS & (1 << 6))); //wait for tx buf to be empty
     USART0->TXDATA = c; //transmit data
}

char getch(void)
{
     while (!(USART0->STATUS & (1 << 7))); //wait for data
     return USART0->RXDATA;
}

void trigger_setup(void)
{
     CMU->HFBUSCLKEN0 |= 0b100; //enable GPIO clock
     GPIO->P[1].MODEL = 4 << (4 * 6); //PB6 push pull
     GPIO->P[1].DOUT &= ~(1 << 6); //PB6 low
}
void trigger_low(void)
{
     GPIO->P[1].DOUT &= ~(1 << 6); //PB6 low
}
void trigger_high(void)
{
     GPIO->P[1].DOUT |= (1 << 6); //PB6 high
}

void HW_AES128_Init(void)
{
     //enable CRYPT0 clock
     CMU->HFBUSCLKEN0 |= 0b10;
}

void HW_AES128_LoadKey(uint8_t* key)
{
     for(int i = 0; i < 16; i++)
     {
          PT_KEY[i] = key[i];
     }
}

void HW_AES128_Enc_pretrigger(uint8_t* pt)
{
     for(int i = 0; i < 4; i++)
     {
          uint32_t key32 = (PT_KEY[4*i + 3] << 24) | (PT_KEY[4*i + 2] << 16) | (PT_KEY[4*i + 1] << 8) | (PT_KEY[4*i]);
          CRYPTO0->KEYBUF = key32;
     }

     for(int i = 0; i < 4; i++)
     {
          uint32_t pt32 = (pt[4*i + 3] << 24) | (pt[4*i + 2] << 16) | (pt[4*i + 1] << 8) | (pt[4*i]);
          CRYPTO0->DATA0 = pt32;
     }
}

void HW_AES128_Enc(uint8_t* pt)
{

     CRYPTO0->CMD = 5;
     while (!(CRYPTO0->IF & 1)); //wait for aes to finish
     CRYPTO0->IFC = 1; //clear aes finished flag
}

void HW_AES128_Enc_posttrigger(uint8_t* pt)
{
     for(int i = 0; i < 4; i++)
     {
          uint32_t ct = CRYPTO0->DATA0;
          pt[4*i] = ct & 0xFF;
          pt[4*i + 1] = (ct >> 8) & 0xFF;
          pt[4*i + 2] = (ct >> 16) & 0xFF;
          pt[4*i + 3] = (ct >> 24) & 0xFF;
     }
}

//not working for now, need cypher key to decrypt, so it needs to be stored
void HW_AES128_Dec(uint8_t *pt)
{
     //do aes to get cypher key
     for(int i = 0; i < 4; i++)
     {
          uint32_t key32 = (PT_KEY[4*i + 3] << 24) | (PT_KEY[4*i + 2] << 16) | (PT_KEY[4*i + 1] << 8) | (PT_KEY[4*i]);
          CRYPTO0->KEYBUF = key32;
     }
     CRYPTO0->CMD = 5;
     while (!(CRYPTO0->IF & 1)); //wait for aes to finish

     //shift cypher key into keybuf
     for(int i = 0; i < 4; i++)
     {
          uint32_t key32 = (PT_KEY[4*i + 3] << 24) | (PT_KEY[4*i + 2] << 16) | (PT_KEY[4*i + 1] << 8) | (PT_KEY[4*i]);
          CRYPTO0->KEYBUF = CRYPTO0->KEY;
     }
     for(int i = 0; i < 4; i++)
     {
          uint32_t pt32 = (pt[4*i + 3] << 24) | (pt[4*i + 2] << 16) | (pt[4*i + 1] << 8) | (pt[4*i]);
          CRYPTO0->DATA0 = pt32;
     }
     CRYPTO0->CMD = 6;
     while (!(CRYPTO0->IF & 1)); //wait for aes to finish
     CRYPTO0->IFC = 1; //clear aes finished flag

     for(int i = 0; i < 4; i++)
     {
          uint32_t ct = CRYPTO0->DATA0;
          pt[4*i] = ct & 0xFF;
          pt[4*i + 1] = (ct >> 8) & 0xFF;
          pt[4*i + 2] = (ct >> 16) & 0xFF;
          pt[4*i + 3] = (ct >> 24) & 0xFF;
     }
}

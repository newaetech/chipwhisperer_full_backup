#include "hal.h"
#include <stdint.h>
#include <stdlib.h>

#include "simpleserial.h"
#include "__Lib_AES_defines.h"

void init_uart(void)
{
    UART0_Init(38400);
    Delay_ms(100);
}

void putch(char c)
{
    UART0_Write(c);
}

char getch(void)
{
    return UART0_Read();
}


void platform_init(void)
{
   PWM5_INST_COUNTER_ON_TIME = 1;
   PWM5_INST_COUNTER_OFF_TIME = 1;
   PWM5_INST_CONFIG.B1 = 0;
   PWM5_INST_CONFIG.B2 = 0;
   PWM5_INST_CONFIG.B3 = 0;
   PWM5_INST_CONFIG.B4 = 0;
   PWM5_INST_CONFIG.B5 = 0;
   PWM5_INST_CONFIG.B6 = 0;
   PWM5_INST_CONFIG.B0 = 1;
   GPIO_PIN_CONTROL_002BITS.MUX = 1;
   aes_hash_power(1);
   aes_hash_reset();

}

void trigger_setup(void)
{
    GPIO_Digital_Output(&GPIO_PORT_010_017, _GPIO_PINMASK_7);
    GPIO_Digital_Output(&GPIO_PORT_150_157, _GPIO_PINMASK_6 | _GPIO_PINMASK_7);
    GPIO_OUTPUT_010_017.B7 = 0;
    GPIO_OUTPUT_150_157.B6 = 1;
}

void trigger_high(void)
{
    GPIO_OUTPUT_010_017.B7 = 1;
}

void trigger_low(void)
{
    GPIO_OUTPUT_010_017.B7 = 0;
}

uint32_t key[4];
uint32_t ptbuf[4] aligned(16);
uint32_t outbuf[4] aligned(16);
uint8_t get_key(uint8_t* k)
{
int i;
uint32_t aes_sts;
        memcpy(key, k, 16);
        while (aes_busy());
        if (aes_set_key(key, 0, AES_KEYLEN_128, 1) != AES_OK)
           return 0x01;
        return 0x00;
}

uint8_t get_pt(uint8_t* pt)
{
        uint32_t aes_sts;

        int i;
        if (!pt || !outbuf) {
           return 0x01;
        }


        memcpy(ptbuf, pt, 16);

        /**********************************
        * Start user-specific code here. */
        trigger_high();

        //16 hex bytes held in 'pt' were sent
        //from the computer. Store your response
        //back into 'pt', which will send 16 bytes
        //back to computer. Can ignore of course if
        //not needed

        if (aes_crypt(ptbuf, outbuf, 1, AES_MODE_ECB) != AES_OK)
              return 0x02;
        aes_start(0);
        while (!aes_done_status(&aes_sts));
        trigger_low();
        /* End user-specific code here. *
        ********************************/
        simpleserial_put('r', 16, outbuf);
        aes_stop();
        return 0x00;
}

uint8_t reset(uint8_t* x)
{
        // Reset key here if needed
        return 0x00;
}

  char uart_rd;

int main(void)
{
    platform_init();
    //blink_leds();
    //uart_loop();
        init_uart();
        trigger_setup();

         /* Uncomment this to get a HELLO message for debug */
         /*
        putch('h');
        putch('e');
        putch('l');
        putch('l');
        putch('o');
        putch('\n');       */


        simpleserial_init();
        simpleserial_addcmd('k', 16, get_key);
        simpleserial_addcmd('p', 16, get_pt);
        simpleserial_addcmd('x', 0, reset);
        while(1)
                simpleserial_get();
                
        return 0;
}
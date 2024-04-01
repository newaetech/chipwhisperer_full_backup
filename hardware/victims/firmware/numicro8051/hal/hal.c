
#include <stdint.h>
#include "numicro_8051.h"
#include "Common.h"
#include "Delay.h"
#include "clock.h"
#include "isp_uart0.h"
#include "hal.h"

// NOTE: When using 16.0Mhz, the actual baud rate as processed will have a high deviation when >38400 (see N76E003 datasheet, section 13.5).
// 115200 will be closer to 111111 
// 230400 will be closer to 250000
// The `target.baud` setting must be set to account for this
#if (!defined(BAUD_RATE) || (BAUD_RATE == 0))
#define BAUD_RATE 115200
#endif

volatile uint32_t __data thing = DIV_ROUND_CLOSEST_UNSIGNED(DIV_ROUND_CLOSEST_UNSIGNED(F_CPU, 16), BAUD_RATE);
STATIC_ASSERT(DIV_ROUND_CLOSEST_UNSIGNED(DIV_ROUND_CLOSEST_UNSIGNED(F_CPU, 16), BAUD_RATE) > 0, 
                "F_CPU / 16 / BAUD_RATE must be greater than 0");

int putchar(int c)
{
    Send_Data_To_UART0(c);
    return c;
}

int getchar(void)
{
    int c;
    while (!RI)
        ;
    c = SBUF;
    RI = 0;
    return (c);
}



void init_uart(void)
{
    // setting pushpull mode for LED1, LED2, and LED3 because they sometimes do not have enough voltage
    PIN_LED_STATUS_PUSHPULL_MODE;
    PIN_LED_OK_PUSHPULL_MODE;
    PIN_LED_ERROR_PUSHPULL_MODE;

    // set LED 1 to on
    PIN_LED_STATUS = 1;
    PIN_LED_OK = 0;
    PIN_LED_ERROR = 0;

    InitialUART0_Timer3(BAUD_RATE);
}

void putch(char c)
{
    putchar(c);
}
char getch(void)
{
    return (char)getchar();
}

void trigger_setup(void)
{
    PIN_TRIGGER = 0;
    PIN_TRIGGER_QUASI_MODE;
}

void trigger_low(void)
{
    PIN_TRIGGER = 0;
}
void trigger_high(void)
{
    PIN_TRIGGER = 1;
}

void platform_init(void)
{
#if USE_EXTERNAL_CLOCK
    use_external_clock();
#else // use internal clock
    use_internal_clock();
    enable_output_clock();
#endif
    set_BODCON1_LPBOD1; // set BOD to only turn on every 25 ms
    set_BODCON1_LPBOD0; // set BOD to only turn on every 25 ms
    clr_BODCON0_BODEN;  // disable brown-out detector
    clr_IE_EBOD;        // disable brown-out detector interrupt
    clr_BODCON0_BORST;  // disable brown-out reset
}

// void led_error(unsigned int status){
//     P05 = status;

// }
// void led_ok(unsigned int status){
//     P12 = status;
// }

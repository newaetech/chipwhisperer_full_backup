
#include "k82f_hal.h"
#include <stdio.h>
#include "fsl_flexio.h"
#include "fsl_gpio.h"
#include "fsl_port.h"
#include "fsl_clock.h"
#include "clock_config.h"
#include "fsl_lpuart.h"

#include <stdint.h>


static uint32_t AES_KEY_SCH[44];

void platform_init(void)
{
     BOARD_BootClockRUN();
}

void init_uart(void)
{
     lpuart_config_t config;
     CLOCK_EnableClock(kCLOCK_PortB); //enable port e clock
     /* CLOCK_EnableClock(kCLOCK_Uart1); //enable UART clock */

     LPUART_GetDefaultConfig(&config);
     config.baudRate_Bps = 38400U;
     config.enableTx = true;
     config.enableRx = true;

     PORT_SetPinMux(PORTB, 16, kPORT_MuxAlt3);
     PORT_SetPinMux(PORTB, 17, kPORT_MuxAlt3);


     /* volatile uint32_t uartclock = CLOCK_GetFreq(LPUART0_CLK_SRC); */
     LPUART_Init(LPUART0, &config, 7370000U);
}

void trigger_setup(void)
{
     PORT_SetPinMux(PORTB, 19, kPORT_MuxAsGpio);
     gpio_pin_config_t trigconf = {
          kGPIO_DigitalOutput, 0
     };

     GPIO_PinInit(GPIOB, 3, &trigconf);
     GPIO_WritePinOutput(GPIOB, 19, 0);
}

void trigger_high(void)
{
     GPIO_WritePinOutput(GPIOB, 19, 1);
}

void trigger_low(void)
{
     GPIO_WritePinOutput(GPIOB, 19, 0);
}

char getch(void)
{
     char c;
     LPUART_ReadBlocking(LPUART0, &c, 1);
     return c;
}

void putch(char c)
{
     LPUART_WriteBlocking(LPUART0, &c, 1);
}

//nothing needed?
/* void HW_AES128_Init(void) */
/* { */
/* } */
/* void HW_AES128_LoadKey(uint8_t* key) */
/* { */
/*      MMCAU_AES_SetKey(key, 16, AES_KEY_SCH); */
/* } */
/* void HW_AES128_Enc(uint8_t* pt) */
/* { */
/*      MMCAU_AES_EncryptEcb(pt, AES_KEY_SCH, 10, pt); */
/* } */
/* void HW_AES128_Dec(uint8_t *pt) */
/* { */
/*      MMCAU_AES_DecryptEcb(pt, AES_KEY_SCH, 10, pt); */
/* } */

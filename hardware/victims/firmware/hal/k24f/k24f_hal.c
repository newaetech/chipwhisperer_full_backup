
#include "k24f_hal.h"
#include <stdio.h>
#include "fsl_uart.h"
#include "fsl_gpio.h"
#include "fsl_port.h"
#include "fsl_clock.h"
#include "clock_config.h"
#include "fsl_mmcau.h"
#include "fsl_rnga.h"

#include <stdint.h>


static uint32_t AES_KEY_SCH[44];

void platform_init(void)
{
     BOARD_BootClockRUN();
     RNGA_Init(RNG);
     RNGA_SetMode(RNG, kRNGA_ModeNormal);
}

void init_uart(void)
{
     uart_config_t config;
     CLOCK_EnableClock(kCLOCK_PortE); //enable port e clock
     CLOCK_EnableClock(kCLOCK_Uart1); //enable UART clock
     port_pin_config_t uart_pins = {
          .pullSelect = kPORT_PullUp,
          .slewRate = kPORT_FastSlewRate,
          .passiveFilterEnable = kPORT_PassiveFilterDisable,
          .driveStrength = kPORT_LowDriveStrength,
          .mux = kPORT_MuxAlt3
     };

     PORT_SetPinConfig(PORTE, 1, &uart_pins);
     PORT_SetPinConfig(PORTE, 0, &uart_pins);
     UART_GetDefaultConfig(&config);

     config.baudRate_Bps = 38400u;
     config.rxFifoWatermark = 0;
     config.enableTx = true;
     config.enableRx = true;

     volatile uint32_t uartclock = CLOCK_GetFreq(UART1_CLK_SRC);
     UART_Init(UART1, &config, uartclock);
}

void trigger_setup(void)
{
     PORT_SetPinMux(PORTE, 3, kPORT_MuxAsGpio);
     gpio_pin_config_t trigconf = {
          kGPIO_DigitalOutput, 0
     };

     GPIO_PinInit(GPIOE, 3, &trigconf);
     GPIO_WritePinOutput(GPIOE, 3, 0);
}

void trigger_high(void)
{
     GPIO_WritePinOutput(GPIOE, 3, 1);
}

void trigger_low(void)
{
     GPIO_WritePinOutput(GPIOE, 3, 0);
}

char getch(void)
{
     char c;
     UART_ReadBlocking(UART1, &c, 1);
     return c;
}

void putch(char c)
{
     UART_WriteBlocking(UART1, &c, 1);
}

uint32_t get_rand(void)
{
     uint32_t value;
     RNGA_GetRandomData(RNG, &value, sizeof(value));
     return value;
}

//nothing needed?
void HW_AES128_Init(void)
{
}
void HW_AES128_LoadKey(uint8_t* key)
{
     MMCAU_AES_SetKey(key, 16, AES_KEY_SCH);
}
void HW_AES128_Enc(uint8_t* pt)
{
     MMCAU_AES_EncryptEcb(pt, AES_KEY_SCH, 10, pt);
}
void HW_AES128_Dec(uint8_t *pt)
{
     MMCAU_AES_DecryptEcb(pt, AES_KEY_SCH, 10, pt);
}

void HW_AES128_Enc_pretrigger(uint8_t* pt){;}
void HW_AES128_Enc_posttrigger(uint8_t* pt){;}
